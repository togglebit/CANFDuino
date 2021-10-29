#include <CAN_CANFD.h>
#include <compiler.h>
#include <mcan.h>
#include <mcan_helper.h>
#include <typedef.h>

#include "CAN_CANFD.h"


cCAN_CANFD CanPort0(0, _500K, _500K, MCAN_MODE_CAN);
cCAN_CANFD CanPort1(1, _500K, _500K, MCAN_MODE_CAN);
cCANTXFrame txMsgs[10];

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
    for(i=0;i<10;i++)
    {
      txMsgs[i].id = 0x100 + i;
      txMsgs[i].len = 8;
      txMsgs[i].data[0] = i;
      CanPort1.addTxMessage(&txMsgs[i]);    
    }

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

        //send 10 CAN Messages every 100mS
        if (uSecs >= 100000)
            {
               uSecs = 0;
               CanPort1.TxMsgs(ALL_MSGS);
            }
      }
}
