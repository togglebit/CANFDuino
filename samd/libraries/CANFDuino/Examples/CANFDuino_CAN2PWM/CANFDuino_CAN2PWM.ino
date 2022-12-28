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


cCAN_CANFD CanPort0(0, _500K, _500K, MCAN_MODE_CAN);

UINT8 i,x,z;

void setup()
{
      for(i=18; i<26; i++)
      {
          //we are going to drive pins 18-25
          pinMode(i, OUTPUT);
      }
          Serial.begin(115200);
          Serial.println("system reset");

          CanPort0.Initialize();
          //only look for CAN ID 0x100, this is where we want our PWM messages
          CanPort0.setFiltRxRange(0x100,0x100);
}                                 


void loop()
{

    //poll for messages, only looking for 0x100
    CanPort0.RxMsgs();
    
    //if we have a message, drive outputs to new pwm value
    if(CanPort0.numRxMsgs)
    {
      //PWM outputs can be 0,1,12-26, except 16. 
      //we will use 8channels in this case, start at ouptut 18-25
      x = 18;

      //unload first 8 bytes of can message containing values 0-255
      for(z=0; z<8; z++)
      {
          analogWrite(x++, CanPort0.rxMsgs[i].data[z]);
      }
    }
}
