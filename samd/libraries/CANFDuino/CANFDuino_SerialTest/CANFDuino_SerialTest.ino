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
This is a simple loopback test for the 2 other UARTS, see pinouts in variant file to use RX/TX pins. 
If you connect TX to RX and watch the monitor you will see the resulting data printed to the termial on serial 1 and you will know this port is working.
D.kasamis
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
          Serial2.begin(115200);
          Serial3.begin(115200);        
          Serial.begin(115200);
          Serial.println("system reset");
}
String msg,msg2; 

void loop()
{
    //write to all outputs
    Serial2.print("Serial2Test");Serial2.print(i,DEC);
    msg = Serial2.readString();
      Serial.println(msg);
 
    Serial3.print("Serial3Test");Serial3.print(i,DEC);
    msg2 = Serial3.readString();
    Serial.println(msg2);
    
    digitalWrite(28, toggle);
    toggle = toggle? 0:1;
    i=i+1;
    delay(2000);
}
