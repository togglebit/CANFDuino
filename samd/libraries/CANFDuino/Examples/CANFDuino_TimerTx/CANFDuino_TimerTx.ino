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

//thi is a simple transmit example sending a batch of messages every 100mS

#include <CAN_CANFD.h>
#include <compiler.h>
#include <mcan.h>
#include <mcan_helper.h>
#include <typedef.h>
#include <SAMC21Timer.h>

cCAN_CANFD CanPort0(0, _500K, _500K, MCAN_MODE_CAN);
cCANTXFrame txMsgs[10];

BOOL bit,passFail;
UINT32 count, prevCount, ticks, uSecs, prevRxCtr0,prevRxCtr1;
UINT8 i, x;

void sendCAN()
{
      CanPort0.TxMsgs(ALL_MSGS);
}

// the setup function runs once when you press reset or power the board
void setup() 
    {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(28, OUTPUT);
        
    Serial.begin(115200);
    Serial.println("system reset");

    //set TX frame to 0x100
    for(i=0;i<10;i++)
    {
      txMsgs[i].id = 0x100 + i;
      txMsgs[i].len = 8;
      txMsgs[i].data[0] = i;
      CanPort0.addTxMessage(&txMsgs[i]);    
    }

    //start CAN hardware
    CanPort0.Initialize();
    CanPort0.setFiltRxAll();
    //setup timer and callback funciton for the timer
    setupTC3TimerInterrupt(100000, sendCAN);


  }

// the loop function runs over and over again forever
void loop() 
{
      //poll queue on RxMessages
        CanPort0.RxMsgs();

          for(i=0; i<CanPort0.numRxMsgs; i++)
          {
            Serial.print("CAN0 Rx Message on ID: 0x");
            Serial.println(CanPort0.rxMsgs[i].rxMsgInfo.id,HEX);
          }
}