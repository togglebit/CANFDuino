
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


//pin mappings for CANFDuino SPI below
#include <SPI.h>

#define DATAOUT  12 //MOSI
#define DATAIN   13//MISO
#define SPICLOCK 14//clk
#define SLAVESELECT 15//ss


int val;
byte pwm;



void setup()
{
  delay(5000);
          pinMode(DATAOUT, OUTPUT);
          pinMode(DATAIN, INPUT);
          pinMode(SPICLOCK,OUTPUT);
          pinMode(SLAVESELECT,OUTPUT);
          delay(10);
          SPI.begin();
        
          // Default speed set to 4MHz, SPI mode set to MODE 0 and Bit order set to MSB first.
          SPI.beginTransaction(SPISettings());
          digitalWrite(SLAVESELECT,HIGH); //release chip       
          
          // initialize digital pin LED_BUILTIN as an output.
          pinMode(28, OUTPUT);
          digitalWrite(28, LOW);
         
          Serial.begin(115200);
          Serial.println("system reset");   
}


void loop()
{

  digitalWrite(SLAVESELECT, LOW);
  SPI.transfer(0b00000110);  //write enable
  digitalWrite(SLAVESELECT, HIGH);
  
  delay(500);
  digitalWrite(28, LOW);
  delay(500);
  digitalWrite(28, HIGH);
  //SPI.endTransaction();   

}
