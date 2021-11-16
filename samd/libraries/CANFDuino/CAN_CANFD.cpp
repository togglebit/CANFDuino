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
#include "CAN_CANFD.h"

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/*
* constructor definition for CAN_CANFD class
*/

cCAN_CANFD::cCAN_CANFD(UINT8 _portNumber, CAN_BAUD_RATE bitRateSTD, CAN_BAUD_RATE bitRateFD, MCAN_MODE canMode)
{
    // const struct mcan_config cfg = {
    portNum = _portNumber   == 0 ? 0 : 1;
    cfg.id   = _portNumber  == 0 ?   ID_CAN0 : ID_CAN1;
    cfg.regs = _portNumber  == 0 ?      CAN0 : CAN1;

    //pointer to message ram
    cfg.msg_ram = mcan_msg_ram;

    cfg.array_size_filt_std = RAM_ARRAY_SIZE_FILT_STD;
    cfg.array_size_filt_ext = RAM_ARRAY_SIZE_FILT_EXT;
    cfg.fifo_size_rx0 = RAM_FIFO_SIZE_RX0;
    cfg.fifo_size_rx1 = 0;
    cfg.array_size_rx = RAM_ARRAY_SIZE_RX;
    cfg.fifo_size_tx_evt = 0;
    cfg.array_size_tx = RAM_ARRAY_SIZE_TX;
    cfg.fifo_size_tx = RAM_FIFO_SIZE_TX;

    cfg.buf_size_rx_fifo0 = 64;
    cfg.buf_size_rx_fifo1 = 0;
    cfg.buf_size_rx = 64;
    cfg.buf_size_tx = 64;

    setBaud(bitRateSTD,bitRateFD,canMode);

    //Set ram size
    mcan_msg_ram_size = ARRAY_SIZE(mcan_msg_ram);

}
void cCAN_CANFD::setBaud(CAN_BAUD_RATE std, CAN_BAUD_RATE fd, MCAN_MODE cMode )
{

    //(see SAMC21 app note) the plus values are to add on what the MCAN driver subtracts back off
    cfg.bit_rate = std;
    cfg.quanta_before_sp = 10 + 2;
    cfg.quanta_after_sp = 3 + 1;
    cfg.quanta_sync_jump = 3 + 1;

    cfg.bit_rate_fd = fd;
    cfg.quanta_before_sp_fd = 10 + 2;
    cfg.quanta_after_sp_fd = 3 + 1;
    cfg.quanta_sync_jump_fd = 3 + 1;

    //set mode
    mode = cMode;

}

void cCAN_CANFD::Initialize()
{
    bool retVal;
    UINT8 i;
    //initialize pins for each port, SAMC21GA16A are port pins B (Group[1]) 
    PORT->Group[1].DIRSET.reg = portNum == 0 ? PORT_PB22 : PORT_PB10;//PORT_PA24 : PORT_PB14;
    PORT->Group[1].DIRCLR.reg = portNum == 0 ? PORT_PB23 : PORT_PB11;//PORT_PA25 : PORT_PB15;

    PORT->Group[1].PINCFG[portNum == 0 ? 22 : 10].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;
    PORT->Group[1].PINCFG[portNum == 0 ? 23 : 11].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;
    PORT->Group[1].PMUX[portNum == 0 ? 22 / 2 : 10 / 2 ].reg = PORT_PMUX_PMUXE(6 /* CAN0 G */) | PORT_PMUX_PMUXO(6 /* CAN0 G */); /* have to write odd and even at once */

    //set clocks
    GCLK->PCHCTRL[portNum == 0 ? CAN0_GCLK_ID : CAN1_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0; 
    MCLK->AHBMASK.reg |= portNum == 0 ? MCLK_AHBMASK_CAN0 : MCLK_AHBMASK_CAN1;

    //config message ram, set pointers
    retVal = mcan_configure_msg_ram(&cfg, &mcan_msg_ram_size);    
    if (!retVal)
        Serial.println("Message RAM not configured");

    //initialize
    retVal = mcan_initialize(&set, &cfg);
    if (!retVal)
        Serial.println("CAN not Initialized");

    //init tx mode
    mcan_set_tx_queue_mode(&set);

    //turn off loop back
    mcan_loopback_off(&set);

    //set mode   
    mcan_set_mode(&set, mode); 

    //enable module Access to the CAN configuration registers is only enabled when both bits CCCR.INIT and CCCR.CCE are set (protected write).
    mcan_enable(&set);

    //set pointers to rx queue
    for (i=0; i<MAX_NUM_RXFRAMES; i++)
    {
        rxMsgs[i].rxMsgInfo.data_len = 64;
        rxMsgs[i].rxMsgInfo.data =  &rxMsgs[i].data[0];

    }

} 


void cCAN_CANFD::addRxMessage(cCANRXFrame *I)
{
    if (numRxFrames < MAX_NUM_RXFRAMES)
    {
        rxFrames[numRxFrames] = I;
        numRxFrames += 1;
    }
}

void cCAN_CANFD::addTxMessage(cCANTXFrame *I)
{
    if (numTxFrames < MAX_NUM_TXFRAMES)
    {
        txFrames[numTxFrames] = I;
        numTxFrames += 1;
    }
}


void cCAN_CANFD::setFiltRxAll()
{
    mcan_filter_id_mask(&set, 0, 0, 0x1fffffff, 0);
    mcan_filter_id_mask(&set, 0, 1, 0x7ff, 0);
}

void cCAN_CANFD::setFiltRxRange(UINT32 startId, UINT32 endId)
{
    mcan_filter_id_range(&set, startId, endId);
}

//look to see if bus-off condition, re-enable
bool cCAN_CANFD::CheckBusStatus()
{
    //look to see if the bus has gone off and reset
    if (!mcan_is_enabled(&set))
    {
        mcan_enable(&set);
    }
    return(mcan_is_enabled(&set));
}


//send all tx messages by loading into fifo 
void cCAN_CANFD::TxMsgs(TX_MODE mode)
{
    UINT8 i;

    switch (mode)
    {
    case NEXT_MSG:

        //send single next message
        mcan_enqueue_outgoing_msg(&set, txFrames[nxtTxFrame]->id, txFrames[nxtTxFrame]->len, txFrames[nxtTxFrame]->data); 

        //go to next frame
        nxtTxFrame = nxtTxFrame < numTxFrames - 1 ? nxtTxFrame + 1 : 0; 

        //increment TX counter
        txCntr += 1;

        break;

    case ALL_MSGS:

        //send 'em all
        for (i = 0; i < numTxFrames; i++)
        {
            mcan_enqueue_outgoing_msg(&set, txFrames[i]->id, txFrames[i]->len, txFrames[i]->data); 

            //increment TX counter
            txCntr += 1;
        }

        break;
    }
}

//send a message  
void cCAN_CANFD::TxMsg(cCANTXFrame *I)
{
    mcan_enqueue_outgoing_msg(&set, I->id, I->len, I->data); 
}

//poll for rx message, pull them out into local rxMsgInfo array
void cCAN_CANFD::RxMsgs()
{
    UINT8 i,x;
    
        //see how many messages have been received by the buffer, pull them out and stuff them into local class copy
        numRxMsgs = mcan_dequeue_received_msg(&set, fifoNum , &rxMsgs[0].rxMsgInfo);
    
        //increment RX counter                      
        rxCntr += numRxMsgs;
                                                                    
        //more than one message dequeue the rest
        if (numRxMsgs > 1 )
        {

            digitalWrite(4,HIGH); 
            for (i=1; (i < numRxMsgs) && (i < MAX_NUM_RXFRAMES); i++)
            {
                mcan_dequeue_received_msg(&set, fifoNum , &rxMsgs[i].rxMsgInfo);
            }
        }

        //fire callbacks to any higher-level protocols (OBD2) if any are registered through addRXmessages
        for (i=0; i< numRxFrames; i++)
        {
            for (x=0; x<numRxMsgs; x++)
            {
                //if this is the message that is registered, fire it's callback
                if (rxFrames[i]->rxMsgInfo.id ==  rxMsgs[x].rxMsgInfo.id)
                {
                    rxFrames[i]->CallbackRx(&rxMsgs[x]);
                }
            }
        }




#ifdef DEBUG_PRINTRX

    for (i=0; (i < numRxMsgs) && (i < MAX_NUM_RXFRAMES); i++)
    {
        //debug dump to screen

        Serial.print(numRxMsgs,DEC);Serial.println(" ");
        Serial.print(rxMsgs[i].rxMsgInfo.id,HEX); Serial.print(" ");
        for (x=0;x<64;x++)
            Serial.print(rxMsgs[i].rxMsgInfo.data[x],HEX); Serial.print(" "); Serial.println(" ");

    }
#endif
    


    digitalWrite(4,LOW); 
}


/**
 * \brief Handler for interrupt line 1 of MCANx.
 */
//void canHandler(void)
//{
//    if (mcan_rx_array_data(&mcan)) 
//     {
//         mcan_clear_rx_array_flag(&mcan);
//         rx_ded_buffer_data = true;
//     }
//}

//void CAN0_Handler(void)
//{
//    canHandler(void);
//}

///void CAN1_Handler(void)
//{

//    canHandler(void);
//}


