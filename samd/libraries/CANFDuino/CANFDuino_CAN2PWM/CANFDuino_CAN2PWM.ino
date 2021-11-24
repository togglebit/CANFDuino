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

cCAN_CANFD CanPort0(0, _500K, _500K, MCAN_MODE_CAN);

int val;
byte pwm;

void setup()
{
  delay(5000);
          pinMode(0, OUTPUT);
          pinMode(1, OUTPUT);
          pinMode(12, OUTPUT);
          pinMode(13, OUTPUT);
          pinMode(14, OUTPUT);
          pinMode(15, OUTPUT);
          pinMode(17, OUTPUT);
          pinMode(18, OUTPUT);
          pinMode(19, OUTPUT);
          pinMode(21, OUTPUT);
          pinMode(22, OUTPUT);
          pinMode(23, OUTPUT);
          pinMode(24, OUTPUT);
          pinMode(25, OUTPUT);

           // initialize digital pin LED_BUILTIN as an output.
          pinMode(28, OUTPUT);
          digitalWrite(28, LOW);
          Serial.begin(115200);
          Serial.println("system reset");
}


void loop()
{
    //write to all PWM outputs
    delay(500);
    pwm +=1; 
    analogWrite(0, pwm);
    analogWrite(1, pwm);
    analogWrite(12, pwm);
    analogWrite(13, pwm);
    analogWrite(14, pwm);
    analogWrite(15, pwm);
    analogWrite(17, pwm);  
    analogWrite(18, pwm);
    analogWrite(19, pwm);
    analogWrite(20, pwm);
    analogWrite(21, pwm);
    analogWrite(22, pwm);
    analogWrite(23, pwm);
    analogWrite(24, pwm);
    analogWrite(25, pwm);

    Serial.print("PWM Outputs At: "); Serial.println(pwm); 
    
   
}
