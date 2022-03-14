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


/*
This is an example of how one could "gateway" a message from one port to another. In this example we listen to all messages on CANport0, looking specficially for message 0x100. 
When we recieve 0x100, we modify it's contents, and re-transmit this on CANport1

*/


#include <CAN_CANFD.h>
#include <compiler.h>
#include <mcan.h>
#include <mcan_helper.h>
#include <typedef.h>

#include "CAN_CANFD.h"
#define NUM_MSGS 10

cCAN_CANFD CanPort0(0, _500K, _500K, MCAN_MODE_CAN);
cCAN_CANFD CanPort1(1, _500K, _500K, MCAN_MODE_CAN);
BOOL bit,passFail;
UINT32 count, prevCount, ticks, uSecs, prevRxCtr0,prevRxCtr1;
UINT8 i, x;

// the setup function runs once when you press reset or power the board
void setup() 
    {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(28, OUTPUT);
    
    pinMode(3, OUTPUT);
    digitalWrite(3, LOW);
    
    Serial.begin(115200);
    Serial.println("system reset");
  
     
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
         //just sit here and look for messages, fire our new function below (callback) when registered ID recieved
         CanPort0.RxMsgs();
         CanPort1.RxMsgs();

         //retransmit port 0 rx Messages on Port 1
         for (i=0;i<CanPort0.numRxMsgs;i++)
         {
             //intercept message 0x64 and modify byte 0 if it equals 55 before re-transmitting on port 1
             if(CanPort0.rxMsgs[i].rxMsgInfo.id == 0x64)
             {
                 if(CanPort0.rxMsgs[i].rxMsgInfo.data[0] == 0x55)
                 {
                    CanPort0.rxMsgs[i].rxMsgInfo.data[0] = 0x88;
                 }
             }
             //send all messages RX on CAN 0, and including moodified message
             CanPort1.TxMsg(&CanPort0.rxMsgs[i]);
         }
         
         //retransmit port 1 rx Messages on Port 0
         for (i=0;i<CanPort1.numRxMsgs;i++)
         {
             //intercept message 0x200 and modify byte 0 if it equals 55 before re-transmitting on port 1
             if(CanPort1.rxMsgs[i].rxMsgInfo.id == 0x200)
             {
                    CanPort1.rxMsgs[i].rxMsgInfo.data[0] = 0xFF;
             }

             //send all messages RX on CAN 1, and including moodified message
             CanPort0.TxMsg(&CanPort1.rxMsgs[i]);
         }

}
