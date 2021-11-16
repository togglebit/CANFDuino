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
cCANTXFrame tx;
BOOL bit,passFail;
UINT32 count, prevCount, ticks, uSecs, prevRxCtr0,prevRxCtr1;
UINT8 i, x;

//Make a new class so we can make our own function (callback) run when we recieve an ID of interest
class RxTx : public cCANRXFrame
{
      bool  CallbackRx(RX_QUEUE_FRAME *R);
};
  

//instantiate our new class
RxTx rx;

// the setup function runs once when you press reset or power the board
void setup() 
    {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(28, OUTPUT);
    
    pinMode(3, OUTPUT);
    digitalWrite(3, LOW);
    
    Serial.begin(115200);
    Serial.println("system reset");
  
    //we are going to listen for message 0x100, modify byte 0 then re-transmit on port 1. We must "register" this id with can port 0                                                                       
    rx.rxMsgInfo.id = 0x100;
    CanPort0.addRxMessage(&rx);
    
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
         digitalWrite(3, LOW);
}


/**
 * This is a overridden implementaiton of the base class member that is called by the RXMsgs() funciton in the CAN class everytime a matching frame ID is recieved
 * @param R      *R - pointer to RX Frame
 *               
 * @return - a flag to accept or reject this CAN frame
 */
bool RxTx::CallbackRx(RX_QUEUE_FRAME *R)
{
  bool retVal = false;
    UINT8 i;
  if (R)
  {
    digitalWrite(3, HIGH);
    
    //we already know that the ID matches, now copy all contents, modify and re-transmit on CAN1
    tx.id = R->rxMsgInfo.id;
    tx.len = R->rxMsgInfo.data_len;
    
    //copy all data bytes except byte 0
    for(i=1;i<tx.len;i++)
    {
      tx.data[i] - R->data[i];
    }
    //lastly modify byte 0, re-transmit on can port 1
    tx.data[0] = 0x80;
    CanPort1.TxMsg(&tx);
  }
  return(retVal);
}
