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
//This is a very simple example of recieving CAN bus data from port 1 and writing it to a CSV format on the SD card.
//D.kasamis 2021 
*/

#include "CAN_CANFD.h"
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
const int chipSelect = 14;//14 for Sercom4

cCAN_CANFD CanPort0(0, _500K, _500K, MCAN_MODE_CAN);
cCAN_CANFD CanPort1(1, _500K, _500K, MCAN_MODE_CAN);

BOOL passFail, bit;
UINT32 count, prevCount, ticks, uSecs, prevRxCtr0,prevRxCtr1;
UINT8 i, x;

//fn to write data to SD
void writeCANSD_Header()
{
        File dataFile = SD.open("datalog.csv", FILE_WRITE);
        if (dataFile)
        {
        dataFile.println("ID,B0,B1,B2,B3,B4,B5,B6,B7");
        dataFile.close();
        }
        else
        {
            Serial.println("error opening datalog.csv");
        } 
}

//fn to write data to SD
void writeCANSD(RX_QUEUE_FRAME *msg)
{
        UINT8 i;
        
        if(msg)
        {
        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
        File dataFile = SD.open("datalog.csv", FILE_WRITE);

        // if the file is available, write to it:
        if (dataFile)
        {
            dataFile.print(msg->rxMsgInfo.id,HEX);
            
            for(i=0; i<msg->rxMsgInfo.data_len; i++)
            {
                dataFile.print(","); dataFile.print(msg->rxMsgInfo.data[i],HEX);                
            }
            dataFile.println();
            dataFile.close();
        }
        // if the file isn't open, pop up an error:
        else
        {
            Serial.println("error opening datalog.txt");
        } 
        }
}
// the setup function runs once when you press reset or power the board
void setup() 
{
    //set chip select
    pinMode(chipSelect,OUTPUT);
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(28, OUTPUT);

    pinMode(27, OUTPUT);
    digitalWrite(27, LOW); 

    Serial.begin(9600);
    Serial.println("system reset");
   
    //start CAN hardware
    CanPort0.Initialize();
    CanPort0.setFiltRxAll();

    //start CAN hardware
    CanPort1.Initialize();
    CanPort1.setFiltRxAll();

    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.print("Initializing SD card...");

    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect))
    {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        while (1);
    }
    Serial.println("card initialized.");
    writeCANSD_Header();

}

// the loop function runs over and over again forever
void loop() 
{

    while (1)
    {
        //poll queue on RxMessages CAN port 1
        CanPort1.RxMsgs();

         //first check to see if CAN port 1 is recieving data from port 0, if so log to SD card
         if (CanPort1.rxCntr != prevRxCtr1)
         { 
           for(i=0; i<CanPort1.numRxMsgs; i++)
           {      
            writeCANSD(&CanPort1.rxMsgs[i]); 
           }
           CanPort1.numRxMsgs = 0;        
         prevRxCtr1 = CanPort1.rxCntr;
        }
        
        //sample clock to determine elapsed number of microseconds
        count = micros();
        //  micro's elapsed compensate for rollover
        ticks = (prevCount < count) ? (count - prevCount) : (0xFFFFFFFF - prevCount) + count; 
        //capture new previous count
        prevCount = count;

        //calculate total uSecs
        uSecs += ticks;

        //do this every 1000mS
        if (uSecs >= 1000000)
        {
            uSecs = 0;
            //toggle LED
            bit = !bit;
            digitalWrite(28, bit ? LOW : HIGH);                
        }

  }
}
