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
#include "OBD2.h"

/**
 * Static re-declarations for cOBDParameter class:
 * Since the header only makes a declaration for statics...re-declare statics for memory allocation.
 * This is necessary for statics in C++
 */
cOBDParameter *cOBDParameter::OBDList[MAX_NUM_PIDS];
UINT8          cOBDParameter::listIdx  = 0;



/**
 * This is the constructor for an OBD parameter. One class should be created and initialized for each parameter ID.
 * If you intend to add parameter ID's that are not in this class already, you'll need to simply add it to the ENUM list "OBD_PID"
 * All other parameters can be dervied from the general OBD2 specficaitons (http://en.wikipedia.org/wiki/OBD-II_PIDs).
 * 
 * 
 * @param _name    -  string describing channel name
 * @param _units   -  string describing engineering units
 * @param _pid     -  parameter ID number
 * @param _size    -  size of the mesage (8, 16 bits etc)
 * @param _signed  -  flag indicating if it is a signed number (sint, uint, etc)
 * @param _mode    -  this is the OBD mode of data we have requested (as provided by the standard) current data, freeze, etc,
 * @param _slope   -  this OBD class assumes a linear relationship between counts and units. This is the slope.
 * @param _offset  -  this sensor class assumes a linear relationship between counts and units. This is the offset.
 * @param _portNum -  physical CAN port to be used for this OBD parameter
 */
cOBDParameter:: cOBDParameter (char _name[STR_LNGTH],
							   char _units[STR_LNGTH],
							   OBD_PID _pid,
							   OBD_PID_SIZE _size,
							   bool _signed,
							   OBD_MODE_REQ _mode,
							   float _slope,
							   float _offset,
							   cCAN_CANFD *_portNum,
                               bool _extended)
{
	UINT8 strSize,i;

	//set simple members, name, units etc

	//copy strings
	for (i=0; i < STR_LNGTH; i++)
	{
		name[i]  = _name[i];
		units[i] = _units[i];
	}

	//set parameter ID
	pid = _pid;

	//set OBD data size
	size = _size;

	//set OBD mode
	dataMode = _mode;

	//set default slope and offset
	m =  _slope;
	b =  _offset;

	//assign scheduler (set port number)
	portNum = _portNum;

	//set periodic rate at which we will make OBD requests
	//setting this QUERY_MSG means that only one message will be transmitted per scheduler "tick". I.E. this would be the same rate
	//that the timer interrupt for the acquisition scheduler runs at
	TXFrame.rate = QUERY_MSG;

	/*
	 * This is where the raw CAN packets to be passed into the lower level scheduler for transmission and reception.
	 */
	
	//
	//********** TX REQUEST FRAME ****************
	//where OBD PID response is CANID: BYTE_0, BYTE_1, BYTE_2, BYTE_3, BYTE_4, BYTE_5, BYTE_6, BYTE_7 
	//
	//							    ----------upper payload-------  	   -----------------lower payload----------------
	//
	//							   | add bytes (2) | mode | PID  |  value[0] (0x55 = NA) |  value[1]  |   value[2]  |  value[3]  |   NA  |
	//
	//
	//NOTE: for transmisison requests now we are going to use the main broadcast message to speak to all ECU's 0x7DF
	//
	TXFrame.id   	 = _extended ? 0x18DB33F1 : 0x7DF;
    TXFrame.data[0]   = 2;
    TXFrame.data[1]   = (UINT8)dataMode;
	TXFrame.data[2]   = (UINT8)pid;
	TXFrame.data[3]   = 0x55;                            
	TXFrame.data[4]   = 0x55;                            
	TXFrame.data[5]   = 0x55;                            
	TXFrame.data[6]   = 0x55;                            
	TXFrame.data[7]   = 0x55;          
    TXFrame.len       = 8;

	//add message to acquisition list in associated acquire class.
	portNum->addTxMessage(&TXFrame);
		
	//
	//********** RX RECEIVE FRAME ****************
	//
	//where OBD PID response is CANID: BYTE_0, BYTE_1, BYTE_2, BYTE_3, BYTE_4, BYTE_5, BYTE_6, BYTE_7 
	//
	//							    ----------upper payload-------  	   -----------------lower payload----------------
	//
	//							   | add bytes | mode & 0x40 (ack) | PID |  value[0] |  value[1]  |   value[2]  |  value[3]  |   NA  |
	//
	//NOTE: for now we are going to assume the main ECU response 0x7E8
	//
	RXFrame.rxMsgInfo.id   	 = _extended ? 0x18DAF10E : 0x7E8;  
    RXFrame.data[0]   = (UINT8)size;
    RXFrame.data[1]   = (UINT8)dataMode;                        
	RXFrame.data[2]   = (UINT8)pid;
	RXFrame.data[3]   = 0x00;
	RXFrame.data[4]   = 0x00;
	RXFrame.data[5]   = 0x00;
	RXFrame.data[6]   = 0x00;
	RXFrame.data[7]   = 0x00;

	//add message to acquisition list for periodic reception in the acquire class
	portNum->addRxMessage(&RXFrame);


	//add message to static list of all OBDMessages created
	OBDList[listIdx] = this;
	listIdx = listIdx < MAX_NUM_PIDS ? listIdx +1 : MAX_NUM_PIDS;
}


/**
 * This method is responsible for extracting the data portion of a received CAN frame (OBD message) based upon 8,16,32bit message sizes.
 * This works in UINT32 data type (calling funciton needs to cast this for sign support).
 * 
 * @return UINT32 - integer that represents OBD message data (no scaling or sign applied)
 */
UINT32 cOBDParameter::getIntData()
{

	//temp  value used for parsing out packet
	UINT32 uValue;

	switch (size)
	{
		case _8BITS:
			{
				uValue = RXFrame.data[3];
			}
			break;

		case _16BITS:
			{
				uValue =  (UINT32)(( RXFrame.data[3] << 8) |  RXFrame.data[4]);
			}
			break;
		
		case _32BITS:
		  {
			  uValue =  (UINT32)(( RXFrame.data[3] << 24) | ( RXFrame.data[4] << 16) | ( RXFrame.data[5] << 8) | RXFrame.data[6]); 
		  }
			break;
	}

	return(uValue);
}

/**
 * This function is responsible for producing engineering unit data (EU). 
 * The sign bit and the appropriate scaling are applied to the OBD data producign a floating point result.
 * Scaling data is provided in the form of (m) & offset (b) data.
 * 
 * @return float - floating point result representing OBD parameter in EU
 */
float cOBDParameter::getData()
{
	float retVal;

	//NOTE: the implementation of the sign bit here needs to be evaluated against the OBDII spec
	if (sign)
	{
		//normalize data based on slope, offset
		retVal =  ((SINT32)getIntData() * m) + b;
	} else
	{
		//normalize data based on slope, offset
		retVal =  ((UINT32)getIntData() * m) + b;
	}

	return(retVal);
}


/**
 * Retrieve the string name for a particular parameter ID
 *
 * @return char* - String representing parameter name
 */
char* cOBDParameter::getName()
{
	return(name);
}

/**
 * Retrieve the string units for a particular parameter ID
 *
 * @return char* - String representing parameter units
 */
char* cOBDParameter::getUnits()
{

	return(units);
}

/**
 * This is a overridden implementaiton of the base class member that is called by the acquisition scheduler when a receive message matching this CAN ID has been received.
 * This is designed for higher-layer protocols that might use a single CAN ID to transmit multiple channels/messages (OBD2).
 * We are checking to see if this was the response to our request.
 *
 * @param R      *R - pointer to RX Frame
 *               
 * @return - a flag to accept or reject this CAN frame
 */
bool cOBDRXFrame::CallbackRx(RX_QUEUE_FRAME *R)
{
	bool retVal = false;
    UINT8 i;

	if (R)
	{
         //check contents of raw CAN frame to see if it matches this OBD parameter
		//we already know that the ID matches, check the data mode (0x40 in most significant nibble is the ack response), check the PID
		if ((R->data[1] == (0x40 | (0x0F & data[1]))) && (R->data[2] == data[2]))
		{
            //we now need to copy out all of the rx queue data to the OBD2 layer
            for(i=0; i<R->rxMsgInfo.data_len; i++)
            {
                data[i] = R->data[i];
            }
			retVal = true;
		}
	}
	return(retVal);
}


/**
 * This is a overridden implementaiton of the base class member that is called by the acquisition scheduler when a receive message matching this CAN ID has been received.
 * This is designed for higher-layer protocols that might use a single CAN ID to transmit multiple channels/messages (OBD2).
 * 
 * @return - a flag to accept or reject this CAN frame
 */
bool cOBDTXFrame::CallbackTx()
{
	return(true);
}

