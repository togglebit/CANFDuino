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


//set the maximum number of RX frames for CAN controller (maxed out at 64)
//set the max number of RXID's to print to screen per port (will probably run out of lines first)
//17A processor
#define MAX_NUM_RXFRAMES  16
#define MAX_NUM_ID       50

//18A processor
//#define MAX_NUM_RXFRAMES  64
//#define MAX_NUM_ID       200
#include <CAN_CANFD.h>
#include <compiler.h>
#include <mcan.h>
#include <mcan_helper.h>
#include <typedef.h>
#include <SD.h>
#include <FlashAsEEPROM.h>


//********* Tested with the following terminal programs ***************
//PUTTY -> https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html
//	      *** Most flexible option, allows for smaller font, more lines and more packets displayed on screen
//
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


//CAN Port inits
cCAN_CANFD CanPort0(0,_500K,_500K,MCAN_MODE_CAN);
cCAN_CANFD CanPort1(1,_500K,_500K,MCAN_MODE_CAN);

UINT8 i, x, PortsOn;

typedef struct
{
    UINT8  enabled; 
    UINT8  baudRate; 
    UINT8  FDbaudRate; 
    UINT32 lowerID; 
    UINT32 upperID; 

}SETTINGS;

//flash CAN settings
SETTINGS CANSet[2];


typedef struct
{
    UINT16 ID;
    UINT16 ctr;
    UINT32 oldTime;
}ASCII_MSG;

typedef struct 
{
    ASCII_MSG asciiFrame[MAX_NUM_ID];
    UINT8 numIDs;
}MSGS;


class cPrintCAN
{
public:

    MSGS ascMsgs;
    cCAN_CANFD *I;

    void updateLine(String *s, UINT8 lineNo)
    {
        //locate cursor on line number, clear line and print updated values
        GOLINE(lineNo);      
        CLEAR_LINE;     
        Serial.print(*s);
    }

    String makeString(RX_QUEUE_FRAME *RxFrame)
    {
        String ascii;
        UINT8 i;                                                                                    

        if (RxFrame)
        {
            //build the frame ID
            ascii = "0x"+ String(RxFrame->rxMsgInfo.id, HEX) + " ";

            //build the data
            for (i=0; i<RxFrame->rxMsgInfo.data_len; i++)
            {
                ascii += String(RxFrame->rxMsgInfo.data[i], HEX) + " "; 
            }

        }
        return ascii;
    }


//build ascii messages suitable for screen display
    void printCAN(UINT8 startLine)
    {
        bool found;
        String ascii;

        //look through all messages rx for this port
        for (i=0; i<I->numRxMsgs; i++)
        {

            found = false;

            //look to see if we've built this before and update string
            for (x = 0; x <ascMsgs.numIDs; x++)
            {
                //look for ID's we have already printed, if so update to a new string
                if (I->rxMsgs[i].rxMsgInfo.id == ascMsgs.asciiFrame[x].ID)
                {
                    ascMsgs.asciiFrame[x].ctr +=1; 
                    ascii = makeString(&I->rxMsgs[i]) +  String(ascMsgs.asciiFrame[x].ctr, DEC)  + "msgs";
                    updateLine(&ascii,startLine+x);
                    found = true;
                    break;
                }

            }
            if (!found)
            {
                //this is a new ID, add to print screen queue
                ascMsgs.asciiFrame[ascMsgs.numIDs].ID = I->rxMsgs[i].rxMsgInfo.id;
                ascMsgs.asciiFrame[ascMsgs.numIDs].ctr +=1; 
                ascii = makeString(&I->rxMsgs[i]) +  String(ascMsgs.asciiFrame[ascMsgs.numIDs].ctr, DEC) + "msgs";
                updateLine(&ascii,ascMsgs.numIDs+startLine);
                ascMsgs.numIDs = ascMsgs.numIDs < MAX_NUM_ID?  ascMsgs.numIDs + 1 : ascMsgs.numIDs;
            }

        }   
    }
};

void storeSettings()
{
    UINT8 memIdx = 0;
    
       //store both CANPorts to EEPROM
    for(i=0; i<2; i++)
    {
     EEPROM.write(memIdx++,CANSet[i].enabled);
     EEPROM.write(memIdx++,CANSet[i].baudRate);
     EEPROM.write(memIdx++,CANSet[i].FDbaudRate);
     EEPROM.write(memIdx++,(UINT8)(CANSet[i].lowerID  & 0x000000FF));
     EEPROM.write(memIdx++,(UINT8)((CANSet[i].lowerID & 0x0000FF00) >> 8));
     EEPROM.write(memIdx++,(UINT8)((CANSet[i].lowerID & 0x00FF0000) >> 16));
     EEPROM.write(memIdx++,(UINT8)((CANSet[i].lowerID & 0xFF000000) >> 24));
    
     EEPROM.write(memIdx++,(UINT8)(CANSet[i].upperID  & 0x000000FF));
     EEPROM.write(memIdx++,(UINT8)((CANSet[i].upperID & 0x0000FF00) >> 8));
     EEPROM.write(memIdx++,(UINT8)((CANSet[i].upperID & 0x00FF0000) >> 16));
     EEPROM.write(memIdx++,(UINT8)((CANSet[i].upperID & 0xFF000000) >> 24));
    }
    //write it all
    EEPROM.commit();
}

bool ReadSettings()
{
    bool retVal = true;
    UINT8 memIdx = 0;

    if(EEPROM.isValid())
    {
        //read both CANPorts out of EEPROM
        for(i=0; i<2; i++)
        {
            CANSet[i].enabled    = EEPROM.read(memIdx++);
            CANSet[i].baudRate   = EEPROM.read(memIdx++);
            CANSet[i].FDbaudRate = EEPROM.read(memIdx++);
            CANSet[i].lowerID    = (EEPROM.read(memIdx++) & 0x000000FF); 
            CANSet[i].lowerID   |= (UINT32)(EEPROM.read(memIdx++) << 8); 
            CANSet[i].lowerID   |= (UINT32)(EEPROM.read(memIdx++) << 16); 
            CANSet[i].lowerID   |= (UINT32)(EEPROM.read(memIdx++) << 24); 
            
            CANSet[i].upperID    = (EEPROM.read(memIdx++) & 0x000000FF); 
            CANSet[i].upperID   |= (UINT32)(EEPROM.read(memIdx++) << 8); 
            CANSet[i].upperID   |= (UINT32)(EEPROM.read(memIdx++) << 16); 
            CANSet[i].upperID   |= (UINT32)(EEPROM.read(memIdx++) << 24); 
        }

    }else{
            //message to the screen, reset
            Serial.println("Flash EEPROM is empty, no settings present. Please enter new settings! Now resetting.");
            delay(2000);
            CLEAR_SCREEN;
            retVal = false;
         }
    return(retVal);
}                          

bool ApplySettings()
{
    bool retVal = true;

    retVal = ReadSettings();
                                    

    switch(CANSet[0].enabled)
    {
      case 0:
        PortsOn = 0;
        setBaudRates(0);
        setFilters(0, CANSet[0].lowerID, CANSet[0].upperID);
        break;

      case 1:
        PortsOn = 1;
        setBaudRates(1);
        setFilters(1, CANSet[1].lowerID, CANSet[1].upperID);
        break;

      case 2:
        PortsOn = 2;
        setBaudRates(0);
        setFilters(0, CANSet[0].lowerID, CANSet[0].upperID);
        
        setBaudRates(1);
        setFilters(1, CANSet[1].lowerID, CANSet[1].upperID);        
      break;
    };

    return(retVal);
}                          



String ShowMenu(String *str)
{
    String response;
    //20sec timeout to user serial commmands
    Serial.setTimeout(20000);

    CLEAR_SCREEN;
    Serial.print(*str);

    //wait for user to hit enter
    response = Serial.readStringUntil(13);
    Serial.println(response);

    delay(1000);

    return(response);
}

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

void setFilters(UINT8 portNum, UINT32 rngLo, UINT32 rngHi)
{

    if(rngLo == 0xFFFFFFFF)
    {
        //no ID filtering, accept all messages
        if (portNum)
        {
            CanPort1.setFiltRxAll();
        } else
        {
            CanPort0.setFiltRxAll();
        }

        Serial.println(String("Port ") + portNum + String(" set to accept all ID's"));

    }else{

        //id filtering set
        if (portNum)
        {
            //set filter ranges
            CanPort1.setFiltRxRange(rngLo,rngHi & 0x1FFFFFFF);
        } else
        {
            CanPort0.setFiltRxRange(rngLo,rngHi & 0x1FFFFFFF); 
        }


        Serial.print(String("Port ") + String(portNum) + String(" set to accept ID's 0x")); Serial.print(rngLo,HEX); Serial.print(" to 0x"); Serial.println(rngHi,HEX);
    }

    delay(2000);
}

void CAN_Filters(UINT8 portNum)
{

    String response1, response2, question, port;
    UINT32 rngLo,rngHi;

    port =  portNum ==0 ? String("CAN 0") : String("CAN 1");
    question = String("On ") + port + String(" accept ID's greater or equal to (number then enter, or type ALL then enter ) 0x");

    response1 = ShowMenu(&question); 

    if (response1 == "ALL")
    {
       //no ID filtering, accept all messages
       setFilters(portNum,0xFFFFFFFF,0xFFFFFFFF);       
       
       //set settings
       CANSet[portNum].lowerID = 0xFFFFFFFF;
        

    } else
    {

        //if we are filtering by range
        question =  port + String(" accept ID's less than or equal to (number then press enter) 0x");
        response2 = ShowMenu(&question); 

        //convert ascii hex numbers and letters to decimal
        rngLo = hexStrToDec(&response1[0]);
        rngHi = hexStrToDec(&response2[0]);
        
        //set settings
        CANSet[portNum].lowerID = rngLo;
        CANSet[portNum].upperID = rngHi;
            
        //set filters
        setFilters(portNum,rngLo,rngHi);

    }


}

void setBaudRates(UINT8 portNum)
{
    CAN_BAUD_RATE fdBaud, stdBaud;

    switch (CANSet[portNum].baudRate)
    {
    case 0:
        stdBaud = (CAN_BAUD_RATE)0;
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

    switch (CANSet[portNum].FDbaudRate)
    {
    case 0:
        fdBaud =    (CAN_BAUD_RATE)0; 
        break;

    case 1:
        fdBaud =   _5M; 
        break;

    case 2:
        fdBaud =   _2M; 
        break;

    case 3:
        fdBaud =   _500K; 
        break;

        break;
    }

    if (portNum == 0)
    {
        //config std and fd baud rates
        CanPort0.setBaud(stdBaud,fdBaud > 0 ? fdBaud:stdBaud, fdBaud > 0 ? MCAN_MODE_EXT_LEN_DUAL_RATE:MCAN_MODE_CAN );
        CanPort0.Initialize();
        Serial.println(String("CAN Port 0 Initialized to Standard CAN baud ") + String(stdBaud) + String(" FD baud ") + String(fdBaud));  
        delay(2000);

    } else
    {

        CanPort1.setBaud(stdBaud,fdBaud > 0 ? fdBaud:stdBaud, fdBaud > 0 ? MCAN_MODE_EXT_LEN_DUAL_RATE:MCAN_MODE_CAN );
        CanPort1.Initialize();
        Serial.println(String("CAN Port 1 Initialized to Standard CAN baud ") + String(stdBaud) + String(" FD baud ") + String(fdBaud));  
        delay(2000);

    }
}

void CAN_FDBaudRate(UINT8 portNum)
{

    CAN_BAUD_RATE fdBaud;
    String response,question, port;
    port =  portNum ==0 ? String("CAN 0") : String("CAN 1");
    question =  port + String(" CANFD BaudRate (number then enter): 0 - NONE/OFF   1 - 5M   2 - 2M   3 - 500k: ");
    response = ShowMenu(&question);    
    
    //storeFD baud rate settings
    CANSet[portNum].FDbaudRate = byte(response.toInt());

    //set baud rates and configure filters
    setBaudRates(portNum);
    CAN_Filters(portNum);
      
}

void CAN_BaudRate(UINT8 portNum)
{
    CAN_BAUD_RATE stdBaud;

    String response,question,port;
    port =  portNum == 0 ? String("CAN 0") : String("CAN 1");
    question = port + String(" BaudRate (number then enter): 0 - CANFD Only   1 - 1M   2 - 500k   3 - 250k   4 - 125k: ");
    response = ShowMenu(&question);
    
    //store baud rate settings
    CANSet[portNum].baudRate = byte(response.toInt());

    CAN_FDBaudRate(portNum);
}



UINT8 EnableBus()
{
    String response,question;
    UINT8 retVal;

    question = String("Enable Bus Sniffing On? (number then enter): 0 - CAN0   1 - CAN1  2 - BOTH: ");
    response = ShowMenu(&question);  
    retVal = byte(response.toInt());
    //set settings
    CANSet[0].enabled = retVal;

    return(retVal);
}



bool UserSetupMenu()
{
    bool retVal = true;;

    Serial.println(""); 
	CLEAR_LINE;
    WHITEFORE;
    Serial.print("Welcome to "); BLUEFORE; Serial.print("CANTerm! "); WHITEFORE; Serial.print("A really cheapo 2 channel OS agnostic "); YELLOWFORE; Serial.println ("CAN & CANFD sniffer.");
    WHITEFORE;
    Serial.println(""); 
    Serial.println("Type ENTER to continue and load last setup, or ESC to enter new a new setup.");
    
    //wait for key entry
    while(true) 
    {
     if(Serial.available() > 0) 
        break;
    }

    if (Serial.read() == 13)
    {
        //enter key pressed
       retVal =  ApplySettings();

    } else
    {
       //any other key pressed, setup menu
        switch (EnableBus())
        {
        
        case 0:
            Serial.println("Port 0 Enabled");
            delay(1000);
            PortsOn = 0;
            CAN_BaudRate(0);
            break;

        case 1:
            Serial.println("Port 1 Enabled");
            delay(1000);
            PortsOn = 1;
            CAN_BaudRate(1);
            break;

        case 2:
            Serial.println("Both Ports Enabled");
            delay(1000);
            PortsOn = 2;
            CAN_BaudRate(0);
            CAN_BaudRate(1);
            break;

        default:
            Serial.println("Both Ports Enabled");
            delay(1000);
            PortsOn = 2;
            CAN_BaudRate(0);
            CAN_BaudRate(1);
            break;
        };
    }
    //write all new settings to flash
    storeSettings();

    return(retVal);
}

void printDisplay() 
{

	CLEAR_SCREEN;	
	Serial.println("  ______    ______   __    __  ________  ________  _______   __       __ ");
	Serial.println(" /      `  /      ` |  `  |  `|        `|        `|       ` |  `     /  $");
	Serial.println("|  $$$$$$`|  $$$$$$`| $$` | $$ `$$$$$$$$| $$$$$$$$| $$$$$$$`| $$`   /  $$");
	Serial.println("| $$   `$$| $$__| $$| $$$`| $$   | $$   | $$__    | $$__| $$| $$$` /  $$$");
	Serial.println("| $$      | $$    $$| $$$$` $$   | $$   | $$  `   | $$    $$| $$$$`  $$$$");
	Serial.println("| $$   __ | $$$$$$$$| $$`$$ $$   | $$   | $$$$$   | $$$$$$$`| $$`$$ $$ $$");
	Serial.println("| $$__/  `| $$  | $$| $$ `$$$$   | $$   | $$_____ | $$  | $$| $$ `$$$| $$");
	Serial.println("  $$    $$| $$  | $$| $$  `$$$   | $$   | $$     `| $$  | $$| $$  `$ | $$");
	Serial.println("  `$$$$$$  `$$   `$$ `$$   `$$    `$$    `$$$$$$$$ `$$   `$$ `$$      `$$");
}
 
//delcare print classes for each 
cPrintCAN PrintCAN0;
cPrintCAN PrintCAN1;

// the setup function runs once when you press reset or power the board
void setup() 
{
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(28, OUTPUT);

    Serial.begin(2000000);
    Serial.println("system reset");

    //init SD card
    //initSD();

    YELLOWFORE;
	printDisplay();

    //good settings loaded
    if(UserSetupMenu())
    {
        //set pointers for print classes
        PrintCAN0.I = &CanPort0;
        PrintCAN1.I = &CanPort1;

        //clear the screen, print port 0 color header and make cursor invisible
        CLEAR_SCREEN;
        BLUEFORE;
        Serial.print("CAN Port 0 Traffic");
        WHITEFORE;
        INVISCURSOR;
   
    }else
    {
        //get new settings
        setup();
    }
}



//Tested at 500k test about 3000frames/sec = 3 frames 10ms, various ID's
void loop() 
{
    //just sit here and look for messages, 
    CanPort0.RxMsgs();
    CanPort1.RxMsgs();   

    if (PortsOn == 0 || PortsOn == 2)
    {
        //print'em if you gott'em
        if (CanPort0.numRxMsgs > 0)
        {
            PrintCAN0.printCAN(2);

        }
    }
    if (PortsOn == 1 || PortsOn == 2)
    {
        if (CanPort1.numRxMsgs >0)
        {
            //start CAN1 messages right after CAN 0
            GOLINE(PrintCAN0.ascMsgs.numIDs+2);
            CLEAR_LINE;
            YELLOWFORE;
            Serial.println("CAN Port 1 Traffic");
            WHITEFORE;
            PrintCAN1.printCAN(PrintCAN0.ascMsgs.numIDs+3);
        }
    }
}



                   
