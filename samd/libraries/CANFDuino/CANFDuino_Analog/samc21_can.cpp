/**
 * @file
 * @author     Scott Price <prices@hugllc.com>
 * @copyright  © 2017 Hunt Utilities Group, LLC
 * @brief      The main class for HUGnetCANLib.
 * @details
 */
#ifdef ARDUINO_ARCH_SAMD
#include "samc21_can.h"
#include "Arduino.h"

SAMC21_CAN *samc21_can_use_object[2];


/**
* @brief Checks for the presence of a connector id
*
* This function never returns if there is no connector id.
*
* @return void
*/
SAMC21_CAN::SAMC21_CAN(uint8_t _CS, uint8_t canid, uint8_t cantx, uint8_t group)
    : rx_ded_buffer_data(false), _idmode(MCP_ANY), _mode(MCP_LOOPBACK), _cs(_CS), _canid(canid),
      _cantx(cantx), _canrx(cantx + 1), _group(group & 1)
{
    if (_canid == ID_CAN0) {
        samc21_can_use_object[0] = this;
    } else if (_canid == ID_CAN1) {
        samc21_can_use_object[0] = this;
    }
};

uint8_t SAMC21_CAN::begin(uint8_t idmodeset, uint32_t speedset, uint8_t clockset)
{
    uint8_t ret;
    _idmode = idmodeset;
    if ((_canid != ID_CAN0) && (_canid != ID_CAN1)) {
        return CAN_FAIL;  // Don't know what this is
    }
    const struct mcan_config mcan_cfg = {

id :
        _canid,
regs :
        ((_canid == ID_CAN0) ? CAN0 : CAN1),
        msg_ram :
        mcan_msg_ram,

        array_size_filt_std :
        RAM_ARRAY_SIZE_FILT_STD,
        array_size_filt_ext :
        RAM_ARRAY_SIZE_FILT_EXT,
        fifo_size_rx0 :
        RAM_FIFO_SIZE_RX0,
        fifo_size_rx1 : 0,
        array_size_rx :
        RAM_ARRAY_SIZE_RX,
        fifo_size_tx_evt : 0,
        array_size_tx :
        RAM_ARRAY_SIZE_TX,
        fifo_size_tx :
        RAM_FIFO_SIZE_TX,

        buf_size_rx_fifo0 : 64,
        buf_size_rx_fifo1 : 0,
        buf_size_rx : 64,
        buf_size_tx : 64,

        /*
        using values from AT6493 (SAMC21 app note); the plus values are to add on what the MCAN driver subtracts back off
        */
        bit_rate :
        speedset,
        quanta_before_sp : 10 + 2,
        quanta_after_sp : 3 + 1,

        /*
        AT6493 (SAMC21 app note) 'fast' values were unhelpfully the same as normal speed; these are for double (1MBit)
                the maximum peripheral clock of 48MHz on the SAMC21 does restrict us from very high rates
        */
        bit_rate_fd :
        speedset,
        quanta_before_sp_fd : 10 + 2,
        quanta_after_sp_fd : 3 + 1,

        quanta_sync_jump : 3 + 1,
        quanta_sync_jump_fd : 3 + 1,
    };
    PORT->Group[_group].DIRSET.reg = (1 << _cantx);
    PORT->Group[_group].DIRCLR.reg = (1 << _canrx);
    PORT->Group[_group].PINCFG[_cantx].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;
    PORT->Group[_group].PINCFG[_canrx].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;
    PORT->Group[_group].PMUX[_cantx / 2].reg = PORT_PMUX_PMUXE(6 /* CAN0 G */) | PORT_PMUX_PMUXO(6 /* CAN0 G */); /* have to write odd and even at once */
    switch (mcan_cfg.id) {
        case ID_CAN0:
            GCLK->PCHCTRL[CAN0_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;
            MCLK->AHBMASK.reg |= MCLK_AHBMASK_CAN0;
            //NVIC_EnableIRQ(CAN0_IRQn);
            break;
        case ID_CAN1:
            GCLK->PCHCTRL[CAN1_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK1;
            MCLK->AHBMASK.reg |= MCLK_AHBMASK_CAN1;
            //NVIC_EnableIRQ(CAN0_IRQn);
            break;
        default:
            break;
    }
    if (mcan_configure_msg_ram(&mcan_cfg, &mcan_msg_ram_size)) {
        Serial.println("RAM configuration succeeded");
    } else {
        return CAN_FAIL;
    }
    ret = mcan_initialize(&mcan, &mcan_cfg);
    if (ret == 0) {
        Serial.println("CAN initialized");
    } else {
        Serial.print("CAN init failed, code ");
        Serial.println(ret);
        return CAN_FAIL;
    }
    mcan_set_tx_queue_mode(&mcan);
    if (_mode == MCP_LOOPBACK) {
        mcan_loopback_on(&mcan);
          Serial.print("Loopback off");
    } else {
      
        mcan_loopback_off(&mcan);
        Serial.println("Loopback on");
    }
    mcan_set_mode(&mcan, MCAN_MODE_CAN);
    mcan_enable(&mcan);
    // Enable chip standby
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, LOW);
    //mcan_enable_rx_array_flag(&mcan, 0);
    // MCP_ANY means filters don't matter
    if (_idmode == MCP_ANY) {
        init_Mask(FILTER_0, (CAN_EXT_MSG_ID | MSG_ID_ALLOW_ALL_MASK));
        init_Mask(FILTER_1, (CAN_STD_MSG_ID | MSG_ID_ALLOW_ALL_MASK));
    Serial.println("no filters on");
    }
    if (mcan_is_enabled(&mcan)) {
        Serial.println("MCAN is enabled!");
        return CAN_OK;
    }
    Serial.println("Something went wrong!!!!!!!!");
    return CAN_FAIL;
};
uint8_t SAMC21_CAN::init_Mask(uint8_t num, uint8_t ext, uint32_t ulData)
{
    uint32_t id;
    if (ext) {
        id = 0x1fffffff;
        ulData &= id;
        id |= CAN_EXT_MSG_ID;
        if (ext >= mcan.cfg.array_size_filt_ext) {
            return MCP2515_FAIL;
        }
    } else {
        id = 0x7ff;
        ulData &= id;
        id |= CAN_STD_MSG_ID;
        if (ext >= mcan.cfg.array_size_filt_std) {
            return MCP2515_FAIL;
        }
    }
   // Serial.print("num "); Serial.println(num ,HEX);
    //Serial.print("id "); Serial.println(id ,HEX);
    //Serial.print("uldata "); Serial.println(ulData ,HEX);

    mcan_filter_id_mask(&mcan, 0, num, id, ulData);
    return MCP2515_OK;
};              // Initilize Mask(s)
uint8_t SAMC21_CAN::init_Mask(uint8_t num, uint32_t ulData)
{
    return init_Mask(num, ((ulData & CAN_EXT_MSG_ID) == CAN_EXT_MSG_ID), ulData);
};                          // Initilize Mask(s)
uint8_t SAMC21_CAN::init_Filt(uint8_t num, uint8_t ext, uint32_t ulData)
{
    uint32_t mask;
    if (ext) {
        mask = 0x1fffffff;
        if (ext >= mcan.cfg.array_size_filt_ext) {
            return MCP2515_FAIL;
        }
    } else {
        mask = 0x7ff;
        if (ext >= mcan.cfg.array_size_filt_std) {
            return MCP2515_FAIL;
        }
    }
    ulData &= mask;
    mcan_filter_id_mask(&mcan, 0, num, ulData, mask);
    return MCP2515_OK;
};              // Initilize Filter(s)
uint8_t SAMC21_CAN::init_Filt(uint8_t num, uint32_t ulData)
{
    return init_Filt(num, ((ulData & CAN_EXT_MSG_ID) == CAN_EXT_MSG_ID), ulData);
}; // Initilize Filter(s)
uint8_t SAMC21_CAN::setMode(uint8_t opMode)
{
    if ((opMode == MCP_LOOPBACK) && (_mode != MCP_LOOPBACK)) {
        _mode = opMode;
        mcan_disable(&mcan);
        mcan_reconfigure(&mcan);
        mcan_loopback_on(&mcan);
        mcan_enable(&mcan);
    } else if ((opMode == MCP_NORMAL) && (_mode != MCP_NORMAL)) {
        _mode = opMode;
        mcan_disable(&mcan);
        mcan_reconfigure(&mcan);
        mcan_loopback_off(&mcan);
        mcan_enable(&mcan);
    } else {
        return MCP2515_FAIL;
    }
    return MCP2515_OK;
};                                        // Set operational mode

uint8_t SAMC21_CAN::sendMsgBuf(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf)
{
    uint8_t ret = 0xff;
    if (!mcan_is_enabled(&mcan)) {
        return CAN_CTRLERROR;
    }
    if (ext) {
        id |= CAN_EXT_MSG_ID;
    }
    ret = mcan_enqueue_outgoing_msg(&mcan, id, len, buf);
    if (ret != 0xFF) {
        return CAN_OK;
    }
    return CAN_FAILTX;
};      // Send message to transmit buffer
uint8_t SAMC21_CAN::sendMsgBuf(uint32_t id, uint8_t len, uint8_t *buf)
{
    return sendMsgBuf(id, 1, len, buf);
};                 // Send message to transmit buffer
uint8_t SAMC21_CAN::readMsgBuf(uint32_t *id, uint8_t *ext, uint8_t *len, uint8_t *buf)
{
    struct mcan_msg_info msg;
    msg.data = buf;
    msg.data_len = 8;
    uint8_t fifo_entries;
    if (mcan_is_tx_complete(&mcan)) {
        mcan_clear_tx_flag(&mcan);
    }
    fifo_entries = mcan_dequeue_received_msg(&mcan, 0, &msg);
    if (fifo_entries > 0) {
        *id = mcan_get_id(msg.id);
        *len = msg.data_len;
        *ext = (msg.id & CAN_EXT_MSG_ID) == CAN_EXT_MSG_ID;
        return CAN_OK;
    }
    return CAN_NOMSG;
};   

// Read message from receive buffer
uint8_t SAMC21_CAN::readMsgBuf(uint32_t *id, uint8_t *len, uint8_t *buf)
{
    uint8_t ext;
    return readMsgBuf(id, &ext, len, buf);
};               // Read message from receive buffer
uint8_t SAMC21_CAN::checkReceive(void)
{
    return (uint8_t)mcan_rx_fifo_data(&mcan, 0);
};                                           // Check for received data
uint8_t SAMC21_CAN::checkError(void)
{
    return 0;
};                                             // Check for errors
uint8_t SAMC21_CAN::getError(void)
{
    return 0;
};                                               // Check for errors
uint8_t SAMC21_CAN::errorCountRX(void)
{
    return 0;
};                                           // Get error count
uint8_t SAMC21_CAN::errorCountTX(void)
{
    return 0;
};                                           // Get error count
uint8_t SAMC21_CAN::enOneShotTX(void)
{
    return 0;
};                                            // Enable one-shot transmission
uint8_t SAMC21_CAN::disOneShotTX(void)
{
    return 0;
};                                           // Disable one-shot transmission
/*
void CAN0_Handler(void)
{
    Serial.println("A");
    if (mcan_rx_array_data(&(use_object->mcan))) {
        mcan_clear_rx_array_flag(&(use_object->mcan));
        use_object->rx_ded_buffer_data = true;
        Serial.println("Got a Packet 0!");
    }
}


void CAN1_Handler(void)
{
    Serial.println("B");
    if (mcan_rx_array_data(&(use_object->mcan))) {
        mcan_clear_rx_array_flag(&(use_object->mcan));
        use_object->rx_ded_buffer_data = true;
        Serial.println("Got a Packet 1!");
    }
}
*/

#endif //ifdef ARDUINO_ARCH_SAMD
