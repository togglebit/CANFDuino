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
bool blinker;

//this function will be fired when the fallign edge of digital 18 is detected (ground and un-ground the pin and it will print to screen)
void toggleStatus()
{
  digitalWrite(28,HIGH);    
  Serial.println("interrupt triggered");
}

// the setup function runs once when you press reset or power the board
void setup() 
    {
    // initialize digital pin GPLED  as an output.
    pinMode(28, OUTPUT);
    //setup IO 18 to be an external input with the pullup enabled
    pinMode(18, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(18), toggleStatus, FALLING);        
    Serial.begin(115200);
    Serial.println("system reset");
    }
// the loop function runs over and over again forever
void loop() 
{
  digitalWrite(28,LOW);
}