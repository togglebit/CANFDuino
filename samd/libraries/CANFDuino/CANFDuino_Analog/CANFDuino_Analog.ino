
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
byte pwm;

void setup()
{
    delay(5000);
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(28, OUTPUT);
    pinMode(27, OUTPUT);
    digitalWrite(27, LOW);
    Serial.begin(115200);
    Serial.println("system reset");
    
  
}


void loop()
{
    uint8_t ret;
    uint32_t id;
    uint8_t len;
    uint8_t buf[8];
    uint8_t i;
    
    delay(500);   
    //read analog inputs 
    val = analogRead(A0);  
    Serial.print("CH0 "); Serial.print(val);        
    val = analogRead(A1);  
    Serial.print(" CH1 "); Serial.print(val);    
    val = analogRead(A2);  
    Serial.print(" CH2 "); Serial.print(val);    
    val = analogRead(A3);  
    Serial.print(" CH3 "); Serial.print(val);    
    val = analogRead(A4);  
    Serial.print(" CH4 "); Serial.print(val);    
    val = analogRead(A5);  
    Serial.print(" CH5 "); Serial.print(val);    
    val = analogRead(A6);  
    Serial.print(" CH6 "); Serial.print(val);    
    val = analogRead(A7);  
    Serial.print(" CH7 "); Serial.print(val);
     val = analogRead(A8);  
    Serial.print(" CH8 "); Serial.print(val);
     val = analogRead(A9);  
    Serial.print(" CH9 "); Serial.println(val);
    
  
}
