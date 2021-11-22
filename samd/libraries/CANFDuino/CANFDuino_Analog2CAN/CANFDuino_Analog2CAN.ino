
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

        //send CAN Messages every 100mS
        if (uSecs >= 100000)
        {
            //grab analog values
            analogs[0] =  analogRead(A0);
            analogs[1] =  analogRead(A1);
            analogs[2] =  analogRead(A2);
            analogs[3] =  analogRead(A3);
            analogs[4] =  analogRead(A4);
            analogs[5] =  analogRead(A5);
            analogs[6] =  analogRead(A6);
            analogs[7] =  analogRead(A7);

            for(i=0; i<4; i++)
            {
                //stuff 12bits, for simplicity we are going to stuff 12 bits into 16 bit chunks in two packets (0x100,0x200)
            
                //channels A0-3 in packet 0x100     
                tx1.data[x++]   = (UINT8)analogs[i];
                tx1.data[x++]   = (UINT8)((analogs[i] & 0xF00) >> 8);                
            }
        
        
            x=0;
            for(i=4; i<8; i++)
            {
                //stuff 12bits, for simplicity we are going to stuff 12 bits into 16 bit chunks in two packets (0x100,0x200)
            
                //channels A0-3 in packet 0x100     
                tx2.data[x++]   = (UINT8)analogs[i];
                tx2.data[x++]   = (UINT8)((analogs[i] & 0xF00) >> 8);                
            }

        
        }

            //send CAN message with 4 analog channels in it 
            CanPort0.TxMsg(&tx1);
            CanPort0.TxMsg(&tx2);
}
