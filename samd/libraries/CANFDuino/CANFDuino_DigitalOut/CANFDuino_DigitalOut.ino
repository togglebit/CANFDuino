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


int val;
byte i;
bool toggle;


void setup()
{
        for(i = 0; i<26; i++)
        {
          pinMode(i, OUTPUT);
        }
        //try these IO with pullups on
        pinMode(6,INPUT_PULLUP);
        //try these IO with pullups on
        pinMode(14,INPUT_PULLUP);
        //try these IO with pullups on
        pinMode(15,INPUT_PULLUP);
        //try these IO with pullups on
        pinMode(18,INPUT_PULLUP);
        //try these IO with pullups on
        pinMode(22,INPUT_PULLUP);
        
          // initialize digital pin LED_BUILTIN as an output.
          pinMode(28, OUTPUT);
          Serial.begin(115200);
          Serial.println("system reset");
}


void loop()
{

    //write to all outputs
    for(i = 0; i<26; i++)
        {
           digitalWrite(i, toggle);  
        }
    digitalWrite(28, toggle);
    toggle = toggle? 0:1;
    delay(500);
}
