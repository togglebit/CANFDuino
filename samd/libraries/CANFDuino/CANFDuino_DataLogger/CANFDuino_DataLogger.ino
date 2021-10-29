
#include "CAN_CANFD.h"
//#include "C:\Users\Togglebit\Documents\Arduino\hardware\CANFDuino\samd\libraries\SPI\SPI.h"
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
const int chipSelect = 14;//14 for Sercom4

cCAN_CANFD CanPort0(0, _500K, _500K, MCAN_MODE_CAN);
cCAN_CANFD CanPort1(1, _500K, _500K, MCAN_MODE_CAN);
CAN_FRAME tx1;
CAN_FRAME tx2;
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
            dataFile.print(msg->rxMsgInfo->id,HEX);
            
            for(i=0; i<tx2.len; i++)
            {
                dataFile.print(","); dataFile.print(msg->rxMsgInfo->data[i],HEX);                
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

    //enable CAN0 and CAN1 transcievers
    pinMode(27, OUTPUT);
    digitalWrite(27, LOW); 

    Serial.begin(9600);
    Serial.println("system reset");
    //set TX frame to 0x200
    tx1.id  = 0x100;
    tx1.len = 8;
    tx1.data[0] = 0x89;
    CanPort0.addTxMessage(&tx1);
  
    //set TX frame to 0x200
    tx2.id  = 0x200;
    tx2.len = 8;
    tx2.data[0] = 0x55;
    CanPort0.addTxMessage(&tx2);

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
        //poll queue on RxMessages
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

        //send CAN Messages every 1000mS
        if (uSecs >= 1000000)
        {
            uSecs = 0;
            CanPort0.TxMsgs();
            //toggle LED
            bit = !bit;
            digitalWrite(28, bit ? LOW : HIGH);                
        }

  }
}
