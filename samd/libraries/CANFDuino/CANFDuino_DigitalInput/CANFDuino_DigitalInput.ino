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

long val;
byte i;
bool toggle;


void setup()
{
        for(i = 0; i<26; i++)
        {
          pinMode(i, INPUT_PULLUP);
        }
  
          // initialize digital pin LED_BUILTIN as an output.
          pinMode(27, OUTPUT);
          digitalWrite(27, LOW);
          Serial.begin(115200);
          Serial.println("system reset");
}


void loop()
{
    //read all inputs, print to screen
    for(i = 0; i<26; i++)
        {
           if(digitalRead(i))
           {
            }else
           {
            Serial.print("Input "); Serial.print(i,DEC); Serial.println(" is low");
          
           }
         }
    digitalWrite(27, toggle);
    toggle = toggle? 0:1;
    delay(2000);
}
