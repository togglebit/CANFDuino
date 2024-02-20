/* CANFDuino_SavvyCANFD.ino (baeed upon GEV-RET.ino)

Created: 7/2/2014 10:10:14 PM
Author: Collin Kidder
Modified: 10/22, D.Kasamis for CANFDuino SAMC21 based processors. This is a quick and dirty mod to GVRET.ino for SAMC21 processors and CANFDuino support
in SavvyCAN. CANFDuino essentially emulates an Arduino DUE using the GVRET protocols. First phase goal was to support dual bus sniffing
and transmission. Logger and other features can/should be added as well as support for CANFD frames, would need a fork and pull of QT Savvy CAN code.
Modified: 2/24, D.Kasamis to include CANFD support in SavvyCAN. Still need to add FD settings adjustment direclty in SavvyCAN

Copyright (c) 2014-2015 Collin Kidder, Michael Neuweiler, Charles Galpin

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
/*
Notes on project:
This code should be autonomous after being set up. That is, you should be able to set it up
then disconnect it and move over to a car or other device to monitor, plug it in, and have everything
use the settings you set up without any external input.
*/


/*DK - CANFDuino
set the maximum number of RX frames for CAN controller (maxed out at 64)
set the max number of RXID's to print to screen per port (will probably run out of lines first)
*/
//17A processor
#define MAX_NUM_RXFRAMES  16
//18A processor has more RAM
//#define MAX_NUM_RXFRAMES  64

#include <CAN_CANFD.h>
#include <compiler.h>
#include <mcan.h>
#include <mcan_helper.h>
#include <typedef.h>
#include <SD.h>
#include <FlashAsEEPROM.h>
#include "GVRET.h"
#include "config.h"

//CAN Port inits, default 500kbaud No CANFD (yet)
cCAN_CANFD CanPort0(0,_500K,_500K,MCAN_MODE_CAN);
cCAN_CANFD CanPort1(1,_500K,_500K,MCAN_MODE_CAN);

UINT8 i, x, PortsOn;



byte serialBuffer[SER_BUFF_SIZE];
int serialBufferLength = 0; //not creating a ring buffer. The buffer should be large enough to never overflow
uint32_t lastFlushMicros = 0;


//CANFDuino doesn't support either of these just yet or logger but leaving here
//DigitalCANToggleSettings digToggleSettings;
//SerialConsole console;

#define NUM_PORTS 2

/**
 * Settings relevent to a CAN port
 */
typedef struct
{
    UINT8  enabled; 
    
    /**
     * Baud rate for CAN (CANFD not supported yet)
     */
    CAN_BAUD_RATE  baudRate; 
    CAN_BAUD_RATE  FDbaudRate;
    /**
     * Filters not currently supported but here when needed
     */
    BOOL filter;
    UINT32 lowerIDFilt; 
    UINT32 upperIDFilt;
    /**
     * if true we don't allow any messing with the bus but rather just passively monitor.
     */
    BOOL listenOnly;
    //set mode 
    MCAN_MODE mode;
}CAN_SET;

/**
 * All settings for system & CAN ports
 */
typedef struct
{
    UINT16 version;
    /**
     * Settings for ports here
     */
    CAN_SET port[NUM_PORTS];
    
    /**
     * use a binary protocol on the serial link or human readable format?
     */
    BOOL useBinarySerialComm; 
    BOOL lawicelMode;
    BOOL lawicelTimestamping;
    UINT16 lawicelPollCounter;
    /**
     * files for logger, ouptut type, extension, base names, incrementing name, logging level and autostart etc.
     */
    FILEOUTPUTTYPE fileOutputType; 
    char fileNameBase[30]; 
    char fileNameExt[4];
    UINT16 fileNum; 
    BOOL appendFile; 
    BOOL autoStartLogging; 
    UINT8 logLevel;

    /**
     * 0 = CANDUE, 1 = GEVCU, 2 = CANDue 1.3 to 2.1, 3 = CANDue 2.2 (CANFDuino emulates CANDUE)
     */
    UINT8 sysType;
    /**
     * stores a validity token to make sure EEPROM is not corrupt
     */
    UINT16 valid; 

}AllSet;


/**
 * Main settings structure
 */
AllSet S;

/**
 * Use flash to emulate EEPROM, write a bunch of bytes to memory
 * 
 * @param ptr    Starting address of all the stuff we want to store
 * @param size   number of bytes we want to store
 */
void storeSettings(UINT8 *ptr, UINT32 size)
{
    UINT8 i;
    
    for(i=0; i<size; i++)
    {
        EEPROM.write(i,ptr[i]);
    }
    //write it all
    EEPROM.commit();
}

/**
 * Use flash to emulate EEPROM, read a bunch of bytes
 * 
 * @param ptr    Starting address of the RAM we want to write to (read from frlash)
 * @param size   number of bytes we want to read
 */
bool readSettings(UINT8 *ptr, UINT32 size)
{
    bool retVal = true;

    if(EEPROM.isValid())
    {
        //read settings out of EEPROM
        for(i=0; i<size; i++)
        {
            ptr[i]  = EEPROM.read(i);
        }

    }else{
            retVal = false;
         }
    return(retVal);
}  


/**
 * don't filter any CAN traffic, let it all through
 */
void setPromiscuousMode()
{
    CanPort0.setFiltRxAll();
    CanPort1.setFiltRxAll();
}

bool toggle;
/**
 * Converts an accii char to it's decimal number
 *
 * @param *s pointer to ascii char to convert
 */
UINT32 hexStrToDec(char *s) 
{
    UINT32 x = 0;

    for (;;)
    {
        char c = *s;
        if (c >= '0' && c <= '9')
        {
            x *= 16;
            x += c - '0'; 
        } else if (c >= 'A' && c <= 'F')
        {
            x *= 16;
            x += (c - 'A') + 10; 
        } else if (c >= 'a' && c <= 'f')
        {
            x *= 16;
            x += (c - 'a') + 10;
        } else break;
        s++;
    }
    return x;
}

/**
 * Shows a menu in a terminal/serial window prompting for user input
 *
 * @param *str pointer to ascii string to display
 * @return string respresenting user input
 */
String ShowMenu(String *str)
{
    String response;
    //20sec timeout to user serial commmands
    Serial.setTimeout(20000);

    Serial.print(*str);

    //wait for user to hit enter
    response = Serial.readStringUntil(13);
    Serial.println(response);

    delay(3000);

    return(response);
}

/**
 * Shows a menu in a terminal/serial window prompting for user input
 *
 * @param portNum which CAN port to be configured
 * @param std int represening the baud rate the user selected for standard CAN
 * @param fd int represening the baud rate the user selected for FD baud rate CAN
 *
 */
void setBaudRates(UINT8 portNum, UINT8 std, UINT8 fd)
{
    CAN_BAUD_RATE fdBaud, stdBaud;

    switch (std)
    {
    case 0:
        stdBaud = NONE;
        break;

    case 1:
        stdBaud =   _1M; 
        break;    

    case 2:     
        stdBaud =   _500K; 
        break;    

    case 3:
        stdBaud =   _250K; 
        break;

    case 4:
    default:
        stdBaud =   _125K; 
        break;

    };

    switch (fd)
    {
    case 0:
        fdBaud =    NONE; 
        break;
    case 1:
        fdBaud =   _5M; 
        break;
    case 2:
        fdBaud =   _2M; 
        break;
    case 3:
        fdBaud =   _1M; 
        break;
    case 4:
        fdBaud =   _500K; 
        break;
    }

    //clip port    
    portNum = portNum > 0? 1:0;

    //set baud settings (store all settings later)
    S.port[portNum].baudRate = stdBaud;
    //default to enable
    S.port[portNum].enabled = true;
    
    //if in FD mode
    if(fdBaud)
    {
        S.port[portNum].FDbaudRate = fdBaud;
        S.port[portNum].mode = MCAN_MODE_EXT_LEN_DUAL_RATE;
    }else
        S.port[portNum].mode = MCAN_MODE_CAN;
    
  };


char* promptForCANFD()
{

    String response,question;
    question =  String(" Type or send 'F' to enter CANFD baud menu ");
    response = ShowMenu(&question);    
    return(&response[0]);    
}

/**
 * User menu prompting for a CANFD baud rate
 *
 * @param portNum which CAN port to be configured
 * @return ASCII char of the user input response
 */
UINT32 CAN_FDBaudRate(UINT8 portNum)
{

    CAN_BAUD_RATE fdBaud;
    String response,question, port;
    port =  portNum == 0 ? String("CAN 0") : String("CAN 1");
    question =  port + String(" CANFD BaudRate (number then enter): 0 - NONE/OFF   1 - 5M   2 - 2M  3 - 1M   4 - 500k: ");
    response = ShowMenu(&question);    
    return(hexStrToDec(&response[0]));    
}

/**
 * User menu prompting for a standard CAN baud rate
 *
 * @param portNum which CAN port to be configured
 * @return ASCII char of the user input response
 */
UINT32 CAN_BaudRate(UINT8 portNum)
{
    CAN_BAUD_RATE stdBaud;

    String response,question,port;
    port =  portNum == 0 ? String("CAN 0") : String("CAN 1");
    question = port + String(" BaudRate (number then enter): 0 - CANFD Only   1 - 1M   2 - 500k   3 - 250k   4 - 125k: ");
    response = ShowMenu(&question);
    return(hexStrToDec(&response[0]));    
}
/**
 * set baud rates for each CAN port and store settings into NV memory
*/
void setBauds()
{
  //set bauds for each
  setBaudRates(0,CAN_BaudRate(0),CAN_FDBaudRate(0));
  setBaudRates(1,CAN_BaudRate(1),CAN_FDBaudRate(1));
  storeSettings((UINT8*)&S,sizeof(S));
  Serial.print("CAN Port 0 Standard CAN baud rate set to "); Serial.print(S.port[0].baudRate); Serial.print(" CANFD baud rate set to ");Serial.println(S.port[0].FDbaudRate);
  Serial.print("CAN Port 1 Standard CAN baud rate set to "); Serial.print(S.port[1].baudRate); Serial.print(" CANFD baud rate set to ");Serial.println(S.port[1].FDbaudRate);
} 

/**
 * Check to see if the user is configurign the CANFDUino to support CANFD support
*/
void detectFD()
{
 UINT8 in_byte;
   
    in_byte = *promptForCANFD();
    
    //detect if escape key has been pressed during reset
    if(in_byte == 'F')
              {
                    Serial.println("Entering CANFD Support Mode for SAVVY CAN");
                    setBauds();
                    Serial.println("Baud settings for CANFD are now stored. In 3 seconds you can close the serial monitor, unplug USB, install boot jumper, plug in USB and start SavvyCAN.");
                    delay(3000);
              }                  
  }


void setup()
{
    //2Mb baud rate for SavvyCAN
    Serial.begin(2000000);
    
    //this is the GPLED on the CANFDuino board
    pinMode(28, OUTPUT);
    //this is the bootloader bypass pin  
    pinMode(16, INPUT_PULLDOWN);  
    


     //until full FD settings for SavvyCan support is done, assume this sketch is alwasy for an ISO 2Mb/500k FD bus
      //this shoudl be ISO CANFD compliant for 2MB
      S.port[0].FDbaudRate = _500K;
      S.port[1].FDbaudRate = _500K;
      S.port[0].FDbaudRate = _2M;
      S.port[1].FDbaudRate = _2M;
      S.port[0].mode = MCAN_MODE_EXT_LEN_DUAL_RATE;
      S.port[1].mode = MCAN_MODE_EXT_LEN_DUAL_RATE;
    

    //check to see if we are going boot bypass is removed and we will enter CANFD selection menu
    while(!digitalRead(16))
    {
      detectFD();
      //pull GPLED high showing in boot menu
      digitalWrite(28,HIGH);
     
    } 
      
    
    //pull low indicating exited boot
    digitalWrite(28,LOW);
    
    Serial.flush();
    //*/
    //load settings from memory
    readSettings((UINT8*)&S, sizeof(S));

    
    //start both CAN ports! 
    CanPort0.setBaud((CAN_BAUD_RATE)S.port[0].baudRate,(CAN_BAUD_RATE)S.port[0].FDbaudRate,S.port[0].mode);
    CanPort1.setBaud((CAN_BAUD_RATE)S.port[1].baudRate,(CAN_BAUD_RATE)S.port[1].FDbaudRate,S.port[0].mode);
    CanPort0.Initialize();
    CanPort1.Initialize();

}


//Get the value of XOR'ing all the bytes together. This creates a reasonable checksum that can be used
//to make sure nothing too stupid has happened on the comm.
uint8_t checksumCalc(uint8_t *buffer, int length)
{
    uint8_t valu = 0;
    for (int c = 0; c < length; c++) {
        valu ^= buffer[c];
    }
    return valu;
}


//rewrite for CANFDuino
void sendFrameToUSB(RX_QUEUE_FRAME *frame, int whichBus)
{
    uint8_t buff[22];
    uint8_t temp;
    uint32_t now = micros();

    if(frame)
    {
    if (S.lawicelMode) {
        //extended id
        if (frame->rxMsgInfo.id > 0x7FF) {
            Serial.print("T");
            sprintf((char *)buff, "%08x", frame->rxMsgInfo.id);
            Serial.print((char *)buff);
        } else {
            Serial.print("t");
            sprintf((char *)buff, "%03x", frame->rxMsgInfo.id);
            Serial.print((char *)buff);
        }
        Serial.print(frame->rxMsgInfo.full_len);
        for (int i = 0; i < frame->rxMsgInfo.full_len; i++) {
            sprintf((char *)buff, "%02x", frame->data[i]);
            Serial.print((char *)buff);
        }
        if (S.lawicelTimestamping) {
            uint16_t timestamp = (uint16_t)millis();
            sprintf((char *)buff, "%04x", timestamp);
            Serial.print((char *)buff);
        }
        Serial.write(13);
    } else {
        if (S.useBinarySerialComm) {
            bool canFD;
            canFD = frame->rxMsgInfo.full_len > 8 ? 1 : 0;
            //canFD = 0; frame->rxMsgInfo.full_len > 8 ? 1 : 0;
            if (frame->rxMsgInfo.id > 0x7FF) frame->rxMsgInfo.id |= 1 << 31;
            serialBuffer[serialBufferLength++] = 0xF1;
            serialBuffer[serialBufferLength++] =  canFD ? 20 : 0; //0 = canbus frame sending, 20 CANFD
            serialBuffer[serialBufferLength++] = (uint8_t)(now & 0xFF);
            serialBuffer[serialBufferLength++] = (uint8_t)(now >> 8);
            serialBuffer[serialBufferLength++] = (uint8_t)(now >> 16);
            serialBuffer[serialBufferLength++] = (uint8_t)(now >> 24);
            serialBuffer[serialBufferLength++] = (uint8_t)(frame->rxMsgInfo.id & 0xFF);
            serialBuffer[serialBufferLength++] = (uint8_t)(frame->rxMsgInfo.id >> 8);
            serialBuffer[serialBufferLength++] = (uint8_t)(frame->rxMsgInfo.id >> 16);
            serialBuffer[serialBufferLength++] = (uint8_t)(frame->rxMsgInfo.id >> 24);
            serialBuffer[serialBufferLength++] = canFD? frame->rxMsgInfo.full_len : frame->rxMsgInfo.full_len + (uint8_t)(whichBus << 4);
            if(canFD)
              serialBuffer[serialBufferLength++] = whichBus;

//            for (int c = 0; c < frame->rxMsgInfo.full_len; c++) {

            for (int c = 0; c < frame->rxMsgInfo.full_len; c++) 
            {
                serialBuffer[serialBufferLength++] = frame->data[c];
            }
            //temp = checksumCalc(buff, 11 + frame.length);
            temp = 0;
            serialBuffer[serialBufferLength++] = temp;
            //Serial.write(buff, 12 + frame->rxMsgInfo.full_len);
        } else {
            Serial.print(micros());
            Serial.print(" - ");
            Serial.print(frame->rxMsgInfo.id, HEX);
            if (frame->rxMsgInfo.id > 0x7FF) Serial.print(" X ");
            else Serial.print(" S ");
            Serial.print(whichBus);
            Serial.print(" ");
            Serial.print(frame->rxMsgInfo.full_len);
            for (int c = 0; c < frame->rxMsgInfo.full_len; c++) {
                Serial.print(" ");
                Serial.print(frame->data[c], HEX);
            }
            Serial.println();
        }
    }

   }
}


/*
Loop executes as often as possible all the while interrupts fire in the background.
The serial comm protocol is as follows:
All commands start with 0xF1 this helps to synchronize if there were comm issues
Then the next byte specifies which command this is.
Then the command data bytes which are specific to the command
Lastly, there is a checksum byte just to be sure there are no missed or duped bytes
Any bytes between checksum and 0xF1 are thrown away

Yes, this should probably have been done more neatly but this way is likely to be the
fastest and safest with limited function calls
*/

cCANTXFrame outFrame;
RX_QUEUE_FRAME echoFrame;
void loop()
{
    static int loops = 0;
    static int out_bus;
    int in_byte;
    static byte buff[20];
    static int step = 0;
    static STATE state = IDLE;
    static uint32_t build_int;
    uint8_t temp8;
    uint16_t temp16;
    static bool markToggle = false;
    int serialCnt;
    uint32_t now = micros();

    //just sit here and look for messages, 
    if(S.port[0].enabled)
    {
        CanPort0.RxMsgs();
    
    for( i = 0; i<CanPort0.numRxMsgs; i++) 
        {
        sendFrameToUSB(&CanPort0.rxMsgs[i], 0);
        
        //DK - not currently supported by CANFDuino but should be easy to add
        //if (digitalRead(ENABLE_PASS_0TO1_PIN)) Can1.sendFrame(incoming); // if pin is NOT shorted to GND
        //toggleRXLED();
        //if (SysSettings.logToFile) sendFrameToFile(incoming, 0);
        //if (digToggleSettings.enabled && (digToggleSettings.mode & 1) && (digToggleSettings.mode & 2)) processDigToggleFrame(incoming);
        }
    }
    if(S.port[1].enabled)
    {
        CanPort1.RxMsgs();   
    
    for( i = 0; i<CanPort1.numRxMsgs; i++) 
        {
        sendFrameToUSB(&CanPort1.rxMsgs[i], 1);
        
         //DK - not currently supported by CANFDuino but should be easy to add
        //if (digitalRead(ENABLE_PASS_0TO1_PIN)) Can1.sendFrame(incoming); // if pin is NOT shorted to GND
        //toggleRXLED();
        //if (SysSettings.logToFile) sendFrameToFile(incoming, 0);
        //if (digToggleSettings.enabled && (digToggleSettings.mode & 1) && (digToggleSettings.mode & 2)) processDigToggleFrame(incoming);
        }
    }

    if (S.lawicelPollCounter > 0) S.lawicelPollCounter--;


if (micros() - lastFlushMicros > SER_BUFF_FLUSH_INTERVAL) 
{
     if (serialBufferLength > 0) 
         {
            Serial.write(serialBuffer, serialBufferLength);
            serialBufferLength = 0;
            lastFlushMicros = micros();
         }
}

    serialCnt = 0;
    while ((Serial.available() > 0) && serialCnt < 128) {
        serialCnt++;
        in_byte = Serial.read();
        switch (state) {
        case IDLE:
          if (in_byte == 0xF1) state = GET_COMMAND;
            
           else if (in_byte == 0xE7) {
            
                S.useBinarySerialComm = true;
                S.lawicelMode = false;
                setPromiscuousMode(); //go into promisc. mode with binary comm
        
                } else {
                //console.rcvCharacter((uint8_t)in_byte);
            }
            break;
        case GET_COMMAND:
            switch (in_byte) {
            case PROTO_BUILD_CAN_FRAME:
                state = BUILD_CAN_FRAME;
                buff[0] = 0xF1;
                step = 0;
                break;
             case PROTO_TIME_SYNC:
                state = TIME_SYNC;
                step = 0;
                buff[0] = 0xF1;
                buff[1] = 1; //time sync
                buff[2] = (uint8_t)(now & 0xFF);
                buff[3] = (uint8_t)(now >> 8);
                buff[4] = (uint8_t)(now >> 16);
                buff[5] = (uint8_t)(now >> 24);
                Serial.write(buff, 6);
                break;
                case PROTO_DIG_INPUTS:
                //immediately return the data for digital inputs - CANFDuino pins 20-23
                temp8 = digitalRead(20) + (digitalRead(21) << 1) + (digitalRead(22) << 2) + (digitalRead(23) << 3);
                buff[0] = 0xF1;
                buff[1] = 2; //digital inputs
                buff[2] = temp8;
                temp8 = checksumCalc(buff, 2);
                buff[3] = temp8;
                Serial.write(buff, 4);
                state = IDLE;
                break;
            case PROTO_ANA_INPUTS:
                //immediately return data on analog inputs  - CANFDuino analog 0-3
                temp16 = analogRead(0);
                buff[0] = 0xF1;
                buff[1] = 3;
                buff[2] = temp16 & 0xFF;
                buff[3] = uint8_t(temp16 >> 8);
                temp16 = analogRead(1);
                buff[4] = temp16 & 0xFF;
                buff[5] = uint8_t(temp16 >> 8);
                temp16 = analogRead(2);
                buff[6] = temp16 & 0xFF;
                buff[7] = uint8_t(temp16 >> 8);
                temp16 = analogRead(3);
                buff[8] = temp16 & 0xFF;
                buff[9] = uint8_t(temp16 >> 8);
                temp8 = checksumCalc(buff, 9);
                buff[10] = temp8;
                Serial.write(buff, 11);
                state = IDLE;
                break;
            case PROTO_SET_DIG_OUT:
                state = SET_DIG_OUTPUTS;
                buff[0] = 0xF1;
                break;
            case PROTO_SETUP_CANBUS:
                state = SETUP_CANBUS;
                step = 0;
                buff[0] = 0xF1;
                break;
                case PROTO_GET_CANBUS_PARAMS:
                //immediately return data on canbus params
                buff[0] = 0xF1;
                buff[1] = 6;
                buff[2] =  S.port[0].enabled + ((UINT8)S.port[0].listenOnly << 4);
                buff[3] =  S.port[0].baudRate;
                buff[4] =  S.port[0].baudRate>>8;
                buff[5] =  S.port[0].baudRate>>16;
                buff[6] =  S.port[0].baudRate>>24;
                buff[7] =  S.port[1].enabled + ((UINT8)S.port[1].listenOnly << 4); 
                buff[8] =  S.port[1].baudRate;      
                buff[9] =  S.port[1].baudRate>>8;   
                buff[10] = S.port[1].baudRate>>16;  
                buff[11] = S.port[1].baudRate>>24;  
                Serial.write(buff, 12);
                state = IDLE;
                break;
            case PROTO_GET_DEV_INFO:
                //immediately return device information
                buff[0] = 0xF1;
                buff[1] = 7;
                buff[2] = CFG_BUILD_NUM & 0xFF;
                buff[3] = (CFG_BUILD_NUM >> 8);
                buff[4] = S.version;
                buff[5] = (UINT8)S.fileOutputType;
                buff[6] = S.autoStartLogging; 
                buff[7] = false;
                Serial.write(buff, 8);
                state = IDLE;
                break;
            case PROTO_SET_SW_MODE:
                buff[0] = 0xF1;
                state = SET_SINGLEWIRE_MODE;
                step = 0;
                break;
             
            case PROTO_KEEPALIVE:
                buff[0] = 0xF1;
                buff[1] = 0x09;
                buff[2] = 0xDE;
                buff[3] = 0xAD;
                Serial.write(buff, 4);
                state = IDLE;
                break;
            case PROTO_SET_SYSTYPE:
                buff[0] = 0xF1;
                state = SET_SYSTYPE;
                step = 0;
                break;
            case PROTO_ECHO_CAN_FRAME:
                state = ECHO_CAN_FRAME;
                buff[0] = 0xF1;
                step = 0;
                break;
            case PROTO_GET_NUMBUSES:
                buff[0] = 0xF1;
                buff[1] = 12;
                buff[2] =  2; //3; //CAN0, CAN1
                Serial.write(buff, 3);
                state = IDLE;
                break;
             case PROTO_GET_EXT_BUSES:
                buff[0]  = 0xF1;
                buff[1]  = 13;
                buff[2]  = false;
                buff[3]  = 0;
                buff[4]  = 0;
                buff[5]  = 0;
                buff[6]  = 0;
                buff[7]  = 0; 
                buff[8]  = 0; 
                buff[9]  = 0;
                buff[10] = 0;
                buff[11] = 0;
                buff[12] = 0; 
                buff[13] = 0; 
                buff[14] = 0;
                buff[15] = 0;
                buff[16] = 0;
                Serial.write(buff, 17);
                state = IDLE;             
                break;
             case PROTO_SET_EXT_BUSES:
                state = SETUP_EXT_BUSES;
                step = 0;
                buff[0] = 0xF1;      
                break;                
            }
            break;
        case BUILD_CAN_FRAME:
            buff[1 + step] = in_byte;
            switch (step) {
            case 0:
                outFrame.id = in_byte;
                break;
            case 1:
                outFrame.id |= in_byte << 8;
                break;
            case 2:
                outFrame.id |= in_byte << 16;
                break;
            case 3:
                outFrame.id |= in_byte << 24;
                if (outFrame.id & 1 << 31) {
                    outFrame.id &= 0x7FFFFFFF;
                    //build_out_frame.extended = true;
                } else //build_out_frame.extended = false;
                break;
            case 4:
                out_bus = in_byte & 3;
                break;
            case 5:
                outFrame.len = in_byte & 0xF;                
                if (outFrame.len > 8) outFrame.len = 8;
                break;                                          
            default:
                if (step < outFrame.len + 6) 
                    {
                    outFrame.data[step - 6] = in_byte;
                }else{
                    state = IDLE;
                    //this would be the checksum byte. Compute and compare.
                    temp8 = checksumCalc(buff, step);
                    
                    if (out_bus == 0)
                        CanPort0.TxMsg(&outFrame);

                    if (out_bus == 1)
                        CanPort1.TxMsg(&outFrame);

                    }
                break;
            }
            step++;
            break;
        case TIME_SYNC:
            state = IDLE;
            break;
        case SET_DIG_OUTPUTS: //todo: validate the XOR byte
            buff[1] = in_byte;
            //temp8 = checksumCalc(buff, 2);
            for (int c = 0; c < 8; c++) {
                //if (in_byte & (1 << c)) setOutput(c, true);
                //else setOutput(c, false);
            }
            state = IDLE;
            break;
        case SETUP_CANBUS: //todo: validate checksum
            switch (step) {
            case 0:
                build_int = in_byte;
                break;
            case 1:
                build_int |= in_byte << 8;
                break;
            case 2:
                build_int |= in_byte << 16;
                break;
            case 3:
                build_int |= in_byte << 24;
                if (build_int > 0) {
                    if (build_int & 0x80000000) 
                     { //signals that enabled and listen only status are also being passed
                        if (build_int & 0x40000000) {
                            S.port[0].enabled = true;
                        } else {
                            S.port[0].enabled = false;
                        }
                        if (build_int & 0x20000000) {
                            S.port[0].listenOnly = true;
                        } else {
                            S.port[0].listenOnly = false;
                        }
                    } else 
                    {
                        //if not using extended status mode then just default to enabling - this was old behavior
                        S.port[0].enabled = true;
                      }
                    build_int = build_int & 0xFFFFF;
                    if (build_int > 1000000) build_int = 1000000;
                    
                    S.port[0].baudRate = (CAN_BAUD_RATE)build_int;
                    CanPort0.setBaud((CAN_BAUD_RATE)S.port[0].baudRate,(CAN_BAUD_RATE)S.port[0].FDbaudRate,S.port[0].mode);
                    CanPort0.Initialize();
                                            
                } else { //disable first canbus
                    S.port[0].enabled = false;
                }
                storeSettings((UINT8*)&S,sizeof(S));

                break;
            case 4:
                build_int = in_byte;
                break;
            case 5:
                build_int |= in_byte << 8;
                break;
            case 6:
                build_int |= in_byte << 16;
                break;
            case 7:
                build_int |= in_byte << 24;
                if (build_int > 0) {
                    if (build_int & 0x80000000) { //signals that enabled and listen only status are also being passed
                        if (build_int & 0x40000000) {
                            S.port[1].enabled = true;
                            //Can1.enable();
                        } else {
                            S.port[1].enabled = false;
                        }
                        if (build_int & 0x20000000) {
                            S.port[1].listenOnly = true;
                        } else {
                             S.port[1].listenOnly = false;
                        }
                    } else {
                       S.port[1].enabled = true;
                    }
                    build_int = build_int & 0xFFFFF;
                    if (build_int > 1000000) build_int = 1000000;
                    
                    S.port[1].baudRate = (CAN_BAUD_RATE)build_int;
                    CanPort1.setBaud((CAN_BAUD_RATE)S.port[1].baudRate,(CAN_BAUD_RATE)S.port[1].FDbaudRate,S.port[1].mode);
                    CanPort1.Initialize();

                } else { //disable second canbus
                    S.port[1].enabled = false;
                }
                state = IDLE;
                //now, write out the new canbus settings to EEPROM
                storeSettings((UINT8 *)&S,sizeof(S));
                
                break;
            }
            step++;
            break;
        case SET_SINGLEWIRE_MODE:
            //DK- currently not supported in CANFDuino
            state = IDLE;
            break;
        
        case SET_SYSTYPE:
            S.sysType = in_byte;
            //EEPROM.write(EEPROM_PAGE, settings);
            //loadSettings();
            state = IDLE;
            break;
        
        case ECHO_CAN_FRAME:
            buff[1 + step] = in_byte;
            switch (step) {
            case 0:
                echoFrame.rxMsgInfo.id = in_byte;
                break;
            case 1:
                echoFrame.rxMsgInfo.id|= in_byte << 8;
                break;
            case 2:
                echoFrame.rxMsgInfo.id|= in_byte << 16;
                break;
            case 3:
                echoFrame.rxMsgInfo.id |= in_byte << 24;
                if (echoFrame.rxMsgInfo.id & 1 << 31) {
                    echoFrame.rxMsgInfo.id &= 0x7FFFFFFF;
                    //build_out_frame.extended = true;
                } else //build_out_frame.extended = false;
                break;
            case 4:
                out_bus = in_byte & 1;
                break;
            case 5:
                echoFrame.rxMsgInfo.full_len = in_byte & 0xF;
                if (echoFrame.rxMsgInfo.full_len  > 8) echoFrame.rxMsgInfo.full_len = 8;

                break;
            default:
                if (step < echoFrame.rxMsgInfo.full_len + 6) {
                    echoFrame.data[step - 6] = in_byte;
                } else {
                    state = IDLE;
                    //this would be the checksum byte. Compute and compare.
                    temp8 = checksumCalc(buff, step);
                     
                    sendFrameToUSB(&echoFrame, 0);
                }
                break;
            }
            step++;
            break;
        
        //DK= CANFDuino doesn't currently support external busses but leaving for future
        case SETUP_EXT_BUSES:
        switch (step) {
            case 0:
                build_int = in_byte;
                break;
            case 1:
                build_int |= in_byte << 8;
                break;
            case 2:
                build_int |= in_byte << 16;
                break;
            case 3:
                build_int |= in_byte << 24;
                break;
            case 4:
                build_int = in_byte;
                break;
            case 5:
                build_int |= in_byte << 8;
                break;
            case 6:
                build_int |= in_byte << 16;
                break;
            case 7:
                build_int |= in_byte << 24;
                break;
            case 8:
                build_int = in_byte;
                break;
            case 9:
                build_int |= in_byte << 8;
                break;
            case 10:
                build_int |= in_byte << 16;
                break;
            case 11:
                build_int |= in_byte << 24;
                state = IDLE;
                break;
            }        
            step++;
            break; 
        }
    }
    //DK - logger not currently supported in CANFDuino but should be added
    //Logger::loop();
    //this should still be here. It checks for a flag set during an interrupt
    //sys_io_adc_poll();
}

