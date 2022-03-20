#include <CAN_CANFD.h>
#include <compiler.h>
#include <mcan.h>
#include <mcan_helper.h>
#include <OBD2.h>
#include <typedef.h>
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
const int chipSelect = 14;//14 for Sercom4


BOOL passFail, bit;
UINT32 count, prevCount, ticks, uSecs, mSecs, prevRxCtr0,prevRxCtr1;
UINT8 i, x;

/********************************************************************
This example is built upon the CANAcquisition class and the OBDParmameter class using 11bit (non-extended) OBD2 ID's
This example shows how to set up periodic data acquisition of OBD2 paramters based upon
standard PID's. If you'd like to add another paramter,simply copy one of the definitions and modify it accordingly. 
You may also need to add a new PID to the "OBD_PID" enum in the header file. 
Note as of 8/24/15 this has only been tested on one Toyota vehicle!
As of July 24, 2017 it has also been tested on a Buick Enclave. Works fine!
Tested on 2020 Mazda3 on 5/31/2021
/********************************************************************/

//create the CANport acqisition schedulers
cCAN_CANFD CanPort0(0, _500K, _500K, MCAN_MODE_CAN);


//Chromebrowser-> Serial Terminal https://googlechromelabs.github.io/serial-terminal/ 
//MACRO's for Terminal Commands                                                                                                                                             
//Example: ESC[4;9f moves the cursor to line 4, column 9.
//ESC[K - Erase Line: Clears all characters from the cursor position to the end of the line (including the character at the cursor position). 
#define CLEAR_SCREEN Serial.write(27);     Serial.print("[2J");    Serial.write(27);    Serial.print("[H")     
#define INVISCURSOR  Serial.write(27);     Serial.print("[?25l");     
#define CLEAR_LINE   Serial.write(27);     Serial.print("[2K")   
#define GOLINE(x)    Serial.write(27);     Serial.print("[" + String(x, DEC) + ";0f")
#define GREENFORE    Serial.write(27);     Serial.print("[32m")
#define YELLOWFORE   Serial.write(27);     Serial.print("[33m")
#define BLUEFORE     Serial.write(27);     Serial.print("[36m")
#define WHITEFORE    Serial.write(27);     Serial.print("[37m")
#define CLEAR_SCREEN Serial.write(27);     Serial.print("[2J");    Serial.write(27);    Serial.print("[H")     


/***** DEFINITIONS FOR OBD MESSAGES ON CAN PORT 0, see https://en.wikipedia.org/wiki/OBD-II_PIDs to add your own ***************/
//char _name[10], char _units[10], OBD_PID pid,  uint8_t OBD_PID_SIZE size, bool _signed, OBD_MODE_REQ mode, float32 slope, float32 offset, cAcquireCAN *, extended ID;

cOBDParameter OBD_Speed(      "Speed "        , " KPH"    ,  SPEED       , _8BITS,   false,   CURRENT,  1,      0,  &CanPort0, false);
cOBDParameter OBD_EngineSpeed("Engine Speed " , " RPM"    ,  ENGINE_RPM  , _16BITS,  false,   CURRENT,  0.25,   0,  &CanPort0, false);
cOBDParameter OBD_Throttle(   "Throttle "     , " %"      ,  THROTTLE_POS, _8BITS,   false,   CURRENT,  0.3922, 0,  &CanPort0, false);
cOBDParameter OBD_Coolant(    "Coolant "      , " C"      ,  COOLANT_TEMP, _8BITS,   false ,  CURRENT,  1,    -40,  &CanPort0, false);
cOBDParameter OBD_EngineLoad( "Load "         , " %"      ,  ENGINE_LOAD , _8BITS,   false,   CURRENT,  0.3922, 0,  &CanPort0, false);
cOBDParameter OBD_MAF(        "MAF "          , " grams/s",  ENGINE_MAF  , _16BITS,  false,   CURRENT,  0.01,   0,  &CanPort0, false);
cOBDParameter OBD_IAT(        "IAT "          , " C"      ,  ENGINE_IAT  , _8BITS,   false ,  CURRENT,  1,    -40,  &CanPort0, false);


//fn to write data to SD
void writeOBD_Header()
{
    File dataFile = SD.open("OBD2data.csv", FILE_WRITE);
    if (dataFile)
    {
        String dataHeader = String(String(OBD_Speed.getName()) + "-" + String(OBD_Speed.getUnits()) +  "," + String(OBD_EngineSpeed.getName()) + "-"  + String(OBD_EngineSpeed.getUnits()) +  "," + String(OBD_Throttle.getName()) + "-" + String(OBD_Throttle.getUnits()) +  "," + String(OBD_Coolant.getName()) + "-" + String(OBD_Coolant.getUnits()) +  "," + String(OBD_EngineLoad.getName()) + "-" + String(OBD_EngineLoad.getUnits()) +  "," + String(OBD_Coolant.getName()) + "-" + String(OBD_Coolant.getUnits()) +  "," + String(OBD_MAF.getName()) + "-" + String(OBD_MAF.getUnits()) +  ","+ String(OBD_IAT.getName()) + "-" + String(OBD_IAT.getUnits()) ); 
        //String dataHeader = String(String(OBD_Speed.getName()) + "-" + String(OBD_Speed.getUnits()) + "-");
        
        dataFile.println(dataHeader);
        dataFile.close();
    } else
    {
        Serial.println("error opening and writing header");
    } 
}

void initSD()
{
    //debugging message for monitor to indicate CPU resets are occuring
    Serial.println("System Reset");

    Serial.print("Initializing SD card...");

    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect))
    {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        while (1);
    }
    Serial.println("card initialized.");
    writeOBD_Header();
}

void setup()
{
    //set chip select
    pinMode(chipSelect,OUTPUT);

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(28, OUTPUT);

    //start serial port 

    Serial.begin(115200);
    //start CAN ports
    CanPort0.Initialize();
    CanPort0.setFiltRxAll();

    //init SD card
    initSD();
}



    //print out our latest OBDII data
void LogScreen()
{
    //comment next two lines out if not using chrome terminal or PUTTY terminal
    CLEAR_SCREEN;
    GOLINE(1);
    
    Serial.print(OBD_Speed.getName()); 
    Serial.print(OBD_Speed.getData());
    Serial.println(OBD_Speed.getUnits());   

    Serial.print(OBD_EngineSpeed.getName()); 
    Serial.print(OBD_EngineSpeed.getData());
    Serial.println(OBD_EngineSpeed.getUnits()); 

    Serial.print(OBD_Throttle.getName()); 
    Serial.print(OBD_Throttle.getData());
    Serial.println(OBD_Throttle.getUnits()); 

    Serial.print(OBD_Coolant.getName()); 
    Serial.print(OBD_Coolant.getData());
    Serial.println(OBD_Coolant.getUnits()); 


    Serial.print(OBD_EngineLoad.getName()); 
    Serial.print(OBD_EngineLoad.getData());
    Serial.println(OBD_EngineLoad.getUnits()); 

    Serial.print(OBD_MAF.getName()); 
    Serial.print(OBD_MAF.getData());
    Serial.println(OBD_MAF.getUnits()); 

    Serial.print(OBD_IAT.getName()); 
    Serial.print(OBD_IAT.getData());
    Serial.println(OBD_IAT.getUnits());           

    Serial.println();
}



//write obd2 data to SD
void logOBDData()
{
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("OBD2data.txt", FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile)
    {
     //grab all values 
     String dataline = String(String(OBD_Speed.getData(),DEC )+ "," + String(OBD_EngineSpeed.getData(),DEC) + "," + String(OBD_Throttle.getData(),DEC) + "," + String(OBD_Coolant.getData(),DEC) + "," + String(OBD_EngineLoad.getData(),DEC) + "," + String(OBD_MAF.getData(),DEC) + "," + String(OBD_IAT.getData(),DEC));
     dataFile.println(dataline); 
     dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else
    {
        Serial.println("error opening OBD2data.txt");
    } 
}
 

void loop()
{
    while (1)
    {           
        //poll queue on RxMessages
        CanPort0.RxMsgs();

        //sample clock to determine elapsed number of microseconds
        count = micros();
        //  micro's elapsed compensate for rollover
        ticks = (prevCount < count) ? (count - prevCount) : (0xFFFFFFFF - prevCount) + count; 
        //capture new previous count
        prevCount = count;

        //calculate total uSecs
        uSecs += ticks;

        //send CAN Messages every 100mS
        if (uSecs >= 100000)
        {
            uSecs = 0;
            //we are sending these OBD2 messages individually every 100mS because some cars cannot take requests very quickly (all at once)
            //this means the rate at which all messages are obtained is .1sec * #of obd messages
            CanPort0.TxMsgs(NEXT_MSG);
            
            //led every 1 sec
            if (mSecs == 10)
            {
                //toggle LED
                bit = !bit;
                digitalWrite(28, bit ? LOW : HIGH); 
                  
               //write directly to CSV every second
                logOBDData();
            }
            //print to screen every 5 sec as a check
            if (mSecs == 50)
            {
              LogScreen();
            }
            
            //count up to 5 Sec
            mSecs = mSecs < 50 ? mSecs+1 : 0; 
        }
    }
}
