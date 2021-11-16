
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
//this is a simple example of using CANFD at 5MBps Rate, sending data from one port to another. Open terminal to determine if it's working.

#include <CAN_CANFD.h>
#include <compiler.h>
#include <mcan.h>
#include <mcan_helper.h>
#include <typedef.h>

cCAN_CANFD CanPort0(0, _1M, _5M, MCAN_MODE_EXT_LEN_DUAL_RATE);
cCAN_CANFD CanPort1(1, _1M, _5M, MCAN_MODE_EXT_LEN_DUAL_RATE);
cCANTXFrame tx1;
cCANTXFrame tx2;
BOOL bit,passFail;
UINT32 count, prevCount, ticks, uSecs, prevRxCtr0,prevRxCtr1;
UINT8 i, x;
  
// the setup function runs once when you press reset or power the board
void setup() 
    {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(28, OUTPUT);
    
    //enable CAN0 and CAN1 transcievers
    pinMode(27, OUTPUT);
    digitalWrite(27, LOW); 
    
    Serial.begin(115200);
    Serial.println("system reset");

    //set TX frame to 0x100
    tx1.id  = 0x100;
    tx1.len = 64;
    tx1.data[0] = 0xFE;
    tx1.data[63] = 0x88;
    CanPort0.addTxMessage(&tx1);

    //set TX frame to 0x200
    tx2.id  = 0x200;
    tx2.len = 64;
    tx2.data[0] = 0x55;
    tx2.data[63] = 0xAB;
    CanPort1.addTxMessage(&tx2);
    
    //start CAN hardware
    CanPort0.Initialize();
    CanPort0.setFiltRxAll();

    //start CAN hardware
    CanPort1.Initialize();
    CanPort1.setFiltRxAll();

    }

// the loop function runs over and over again forever
void loop() 
{
while (1)
        {
         //poll queue on RxMessages
         CanPort0.RxMsgs();
         CanPort1.RxMsgs();

         //sample clock to determine elapsed number of microseconds
         count = micros();
         //  micro's elapsed compensate for rollover
         ticks = (prevCount < count) ? (count - prevCount) : (0xFFFFFFFF - prevCount) + count; 
         //capture new previous count
         prevCount = count;
         
         //calculate total uSecs
         uSecs += ticks;

        //send CAN Messages every 100mS
        if (uSecs >= 100000)
            {
               uSecs = 0;
               CanPort0.TxMsgs(ALL_MSGS);
               CanPort1.TxMsgs(ALL_MSGS);
               //toggle LED
               bit = !bit;
              digitalWrite(28, bit ? LOW : HIGH);                
            }
        
        //set pass flag
        passFail = true;
        //first check to see if CAN port 0 is recieving data from port 1
        if(CanPort0.rxCntr != prevRxCtr0)
        {
            Serial.println("CAN0 is Receiving Data from CAN1,"); 
        }else
        {
            Serial.println("CAN0 is NOT Receiving Data from CAN1,"); 
            
            //indicate test failed
            passFail =  false;
        }
        //capture counter
        prevRxCtr0 = CanPort0.rxCntr;
        
        //first check to see if CAN port 1 is recieving data from port 0
        if(CanPort1.rxCntr != prevRxCtr1)
        {
            Serial.println("CAN1 is Receiving Data from CAN0,"); 
        }else
        {
            Serial.println("CAN1 is NOT Receiving Data from CAN0,"); 
         
            //indicate test failed
            passFail =  false;
        }
        //capture counter
        prevRxCtr1 = CanPort1.rxCntr;

        //print RX counters to screen
        Serial.print("CAN0 RxCtr "); 
        Serial.println(CanPort0.rxCntr,DEC); 
        Serial.print("CAN1 RxCtr "); 
        Serial.println(CanPort1.rxCntr,DEC); 

        //indicate if board has passed or failed construction
        if(passFail)
        {
            Serial.println("The CANFDunio has passed testing!"); 
            Serial.println("///////////////////////////////////////////////////////////////////////////////"); 
        }else
        {
            Serial.println("The CANFDunio has failed testing. Here are some things to check: "); 
            Serial.println(" - install jumper on at least one termination resistor"); 
            Serial.println(" - make sure wiring is CAN0 H -> CAN1 H, CAN0 L -> CAN1 L, GND -> GND"); 
            Serial.println("///////////////////////////////////////////////////////////////////////////////"); 
        }

        //check for bus off state (port disconnected) and reset as needed
        CanPort0.CheckBusStatus();
        CanPort1.CheckBusStatus();
        delay(1000);
        }
}
