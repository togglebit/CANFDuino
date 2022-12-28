
/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include <CAN_CANFD.h>
#include <compiler.h>
#include <mcan.h>
#include <mcan_helper.h>

//declare CAN port at 500Kbaud and 2 frames
cCAN_CANFD CanPort0(0, _500K, _500K, MCAN_MODE_CAN);
cCANTXFrame tx1;
cCANTXFrame tx2;

UINT32 count, prevCount, ticks, uSecs, prevRxCtr0,prevRxCtr1;
UINT8 i, x;



void setup()
{
    digitalWrite(27, LOW);
    Serial.begin(115200);
    Serial.println("system reset");
    
    //start CAN hardware
    CanPort0.Initialize();
    CanPort0.setFiltRxAll();
    
    //set ID and length
    tx1.id  = 0x100;
    tx1.len = 8;
    tx2.id  = 0x200;
    tx2.len = 8;

  

}

//determine how often to send/print
#define SEND_TIME 2000000
#define DEBUG 

UINT16 analogs[10];

void loop()
{
        //sample clock to determine elapsed number of microseconds
        count = micros();
         //  micro's elapsed compensate for rollover
        ticks = (prevCount < count) ? (count - prevCount) : (0xFFFFFFFF - prevCount) + count; 
        //capture new previous count
        prevCount = count;
        //  micro uSecs
        uSecs += ticks;

        //send CAN Messages every 2seconds and print to screen for debug
        if (uSecs >= SEND_TIME)
        {
           //clear uSec accumultor
            uSecs = 0;
            
            //grab analog values
            analogReadResolution(12);
            analogs[0] =  analogRead(A0);
            analogs[1] =  analogRead(A1);
            analogs[2] =  analogRead(A2);
            analogs[3] =  analogRead(A3);
            analogs[4] =  analogRead(A4);
            analogs[5] =  analogRead(A5);
            analogs[6] =  analogRead(A6);
            analogs[7] =  analogRead(A7);


            //pack both CAN messages
            //stuff 12bits, for simplicity we are going to stuff 12 bits into 16 bit chunks in two packets (0x100,0x200)
            //MSB will appear first, [0][1] = [0] MSByte, [1]LSByte
            //this will match what is shown int the print to monitor
            x=0;
            for(i=0; i<4; i++)
            {
            
                //channels A0-3 in packet 0x100     
                tx1.data[x++]   = (UINT8)((analogs[i] & 0xF00) >> 8);
                tx1.data[x++]   = (UINT8)analogs[i];                
            }
        
        
            x=0;
            for(i=4; i<8; i++)
            {
                //stuff 12bits, for simplicity we are going to stuff 12 bits into 16 bit chunks in two packets (0x100,0x200)
            
                //channels A4-7 in packet 0x200     
                tx2.data[x++]   = (UINT8)((analogs[i] & 0xF00) >> 8);
                tx2.data[x++]   = (UINT8)analogs[i];                
            }

#ifdef DEBUG
            //**** DEBUG print to screen ***********
            for(i=0; i<8; i++)
                Serial1.println(analogs[i],HEX);
            Serial1.println("******************");
            //**** end DEBUG ******            
#endif 
            
            
            //send CAN messages with 8 analog channels in them
            CanPort0.TxMsg(&tx1);
            CanPort0.TxMsg(&tx2);
        }
}
