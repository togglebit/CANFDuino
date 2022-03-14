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
#ifndef CAN_FD_H
#define CAN_FD_H    

//dump to the screen
//#define DEBUG_PRINTRX

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

//#include "variant.h"
//#include "chip.h"
#include "compiler.h"
#include "mcan.h"

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/

/* size of our custom Rx and Tx Buffer Elements, in words */
#define RAM_BUF_SIZE                  (MCAN_RAM_BUF_HDR_SIZE + 64u / 4)

#define RAM_ARRAY_SIZE_FILT_STD       (8u)
#define RAM_ARRAY_SIZE_FILT_EXT       (8u)

//define number of messages in RX Buffer
#ifndef MAX_NUM_RXFRAMES
    #define MAX_NUM_RXFRAMES    10
#endif
#define RAM_FIFO_SIZE_RX0     MAX_NUM_RXFRAMES

//define number of messages in TX Buffer
#ifndef MAX_NUM_TXFRAMES
    #define MAX_NUM_TXFRAMES    10
#endif 
#define RAM_FIFO_SIZE_TX      MAX_NUM_TXFRAMES


/* no Rx FIFO 1 in our Message RAM */
#define RAM_ARRAY_SIZE_RX             (4u)

/* no Tx Event FIFO in our Message RAM */
#define RAM_ARRAY_SIZE_TX             (4u)


/* size of our custom Message RAM, in words */
#define MSG_RAM_SIZE      ( \
      RAM_ARRAY_SIZE_FILT_STD * MCAN_RAM_FILT_STD_SIZE \
    + RAM_ARRAY_SIZE_FILT_EXT * MCAN_RAM_FILT_EXT_SIZE \
    + RAM_FIFO_SIZE_RX0 * RAM_BUF_SIZE \
    + RAM_ARRAY_SIZE_RX * RAM_BUF_SIZE \
    + RAM_ARRAY_SIZE_TX * RAM_BUF_SIZE \
    + RAM_FIFO_SIZE_TX * RAM_BUF_SIZE )

#define MSG_LEN_CAN     8
#define MSG_LEN_CAN_FD  64
#define MSG_ID_2          (CAN_STD_MSG_ID | 0x444)
#define MSG_ID_2_MASK     0x7FCul     /* bits 0 & 1 are don't care */
/**
 * This ENUM represents the CAN baud rate values
 */
enum CAN_BAUD_RATE
{
    _10M    = 10000000,
    _5M     = 5000000,
    _1M     = 1000000,
    _500K   = 500000,
    _250K   = 250000,
    _125K   = 125000,
    _50K    = 50000,
    _25K    = 25000,
    _10K    = 10000,
    _5K     = 5000,
    NONE    = 0
};

/**
 * This enum represents the periodic transmission rate for messages 
 */
enum ACQ_RATE_CAN
{
    _1Hz_Rate       = 1000,
    _5Hz_Rate       = 200,
    _10Hz_Rate      = 100,
    _100Hz_Rate     = 10,
    QUERY_MSG       = 0xFFFF
};

/**
 * This enum represents the mode to be used for transmitting queued messages by TxMsgs call.
 * All - Immediately send all messages added to pointer queue
 * Next - Send single message, next in the pointer queue
 */
enum TX_MODE
{
    ALL_MSGS,
    NEXT_MSG
};

struct CAN_FRAME
{
    uint32_t id;
    uint8_t data[64];
    uint8_t len;
    uint8_t buf_idx;
};

class cCANTXFrame : public CAN_FRAME
{
public:
     /**
      * this is a transmission rate if used in a scheduler
      */
     ACQ_RATE_CAN rate;
    
     /**
     * This is a function that i    s called by the acquisition scheduler when a receive message matching this CAN ID has been received.
     * This was primarily designed for higher-layer protocols that might use a single CAN ID to transmit multiple channels/messages (OBD2).
     *
     * @param  RX_CAN_FRAME *R - pass along a pointer to RX frame  
     *
     * @return - a flag to accept or reject this CAN frame  
     */
    virtual bool CallbackTx()
    {
        return(true);
    };
};

//this struct is used to deque rx Frames when in queue mode
struct RX_QUEUE_FRAME
{
    MCAN_MSG_INFO rxMsgInfo;
    uint8_t data[64];
};

class cCANRXFrame : public RX_QUEUE_FRAME
{
public:
    /**
     * This is a function that is called by the acquisition scheduler when a receive message matching this CAN ID has been received.
     * This was primarily designed for higher-layer protocols that might use a single CAN ID to transmit multiple channels/messages (OBD2).
     *
     * @param  RX_CAN_FRAME *R - pass along a pointer to RX frame  
     *
     * @return - a flag to accept or reject this CAN frame  
     */
    virtual bool CallbackRx(RX_QUEUE_FRAME *R)
    {
        return(true);
    };
};

/**
 * this is the main calss of the CAN/FD object. One shoudl be declared for each hardware instance
 */
class cCAN_CANFD
{
public:

    /**
     * send and recieved counter
     */
    UINT32 rxCntr;
    UINT32 txCntr;   

    /**
     * number of messages last recieved in rx queue
     * 
     */
    UINT8 numRxMsgs;

    /**
     * array of RXmessages
     */
    RX_QUEUE_FRAME rxMsgs[MAX_NUM_RXFRAMES];


    /**
     * constructor for CAN/FD object
     * 
     * @param _portNumber
     *                   physical port number (0 or 1 for samc21)
     * @param bitRateSTD bit rate for standard CAN
     * @param bitRateFD  bit rate for CAN FD
     * @param canMode    mode of CAN port, (STD,FD, etc)
     */
    cCAN_CANFD(UINT8 _portNumber, CAN_BAUD_RATE bitRateSTD, CAN_BAUD_RATE bitRateFD, MCAN_MODE canMode);


    /**
     * This function sets the mode and baudrates, initialize should be re-called after this call
     * 
     * @param std    stdandard mode baud rate
     * @param fd     fd baud rate
     * @param cMode  mode, fd, standard etc
     */
    void setBaud(CAN_BAUD_RATE std, CAN_BAUD_RATE fd, MCAN_MODE cMode );

    /**
      * Initialization method responsible for configuring hardware pins, turning on peripherial and enabling interrupts. 
      * Should be called on statup
      * 
      */
    void Initialize();

    /**
     * This adds tx messages to queue that will be dumped into the fifo 
     * 
     * @param I      pointer to pre-defined TX frame
     */
    void addTxMessage(cCANTXFrame *I);
    /**
    
     * This adds rx messages primarily used for callback functions on higher-level protocols 
     * 
     * @param I      pointer to pre-defined RX frame
     */
    void addRxMessage(cCANRXFrame *I);
    /**
     * This method is meant to be called periodically when the registered TX frames are to be transmitted
     * 
     * @param mode   Mode to send messages in queue
     *               all - send all messages in pointer queue
     *               next - send the next one in the queue (circular buffer)
     */
    void TxMsgs(TX_MODE mode);

    /**
    * This method is meant to send a single frame
    * 
    * @return 
    */
    void TxMsg(cCANTXFrame *I);

    /**
    * This method is designed for repeating received frames
    * 
    * @return 
    */
    void TxMsg(RX_QUEUE_FRAME *I);

    /** This method is meant to be called in a polling loop to deque frames
     * 
     * @return 
     */
    void RxMsgs();

    /**
     * Open all RX filters to recieve all messages
     */
    void setFiltRxAll();
    
    /**
     * Open filters to a specific range of ID's
     * 
     * @param startId start ID, must be less than end ID
     * @param endId end of range. Ex (0x100-0x200)
     */
    void setFiltRxRange(UINT32 startId, UINT32 endId);

    /**
     * Check to see if bus has gone off state and re-enable. 
     * This can happen if physically disconnected from the bus and reconnected
     * 
     * @return- boolean indicating mCan has been re-initialized 
     */
    bool CheckBusStatus();

private:

    /**
     * can configuration, setup and mode structure
     */
    MCAN_CONFIG cfg;
    MCAN_SET    set; 
    MCAN_MODE  mode;

    /**
     * the Message RAM is allocated from within the SAMC21's RAM
     */
    uint32_t mcan_msg_ram[MSG_RAM_SIZE] __attribute__ ((aligned (4)));

    /**
     * port number
     */
    UINT8  portNum;

    /**
     * message ram
     */
    UINT32 mcan_msg_ram_size;

    /**
     * fifo number
     */
    UINT8 fifoNum;

    /**
     * number of messageID's added to rx queue, implemented for high-level callbacks
     */
    UINT8 numRxFrames;
    
    /**
     * number of messages added to tx queue
     */
    UINT8 numTxFrames;
    /**
     * next message to be sent in transmit queue
     */
    UINT8 nxtTxFrame;
    /**
     * pointer to tx frames
     */
    cCANTXFrame *txFrames[MAX_NUM_TXFRAMES];

    /**
     * pointer to rx frames used by higher-level callbacks
     */
    cCANRXFrame *rxFrames[MAX_NUM_RXFRAMES];

};                            
#endif
