/*
This code was originally written for the SAMA5D2, but has been modified to work with the SAMC21.

The peripheral is the same (except for Message RAM addressing), but 
Atmel's SAMC21 register definition include files have a different and peculiar syntax.
*/

/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2015, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/** \file
 *  Implements functions for Controller Area Network with Flexible Data-rate,
 *  relying on the MCAN peripheral.
 */
/** \addtogroup can_module
 *@{*/

//DJK modified
// - to remove weird extended ID flag ORing and requreied calls to remove from ID field. 
// - add range filtering 

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/


#include "mcan.h"

/*---------------------------------------------------------------------------
 *      Local definitions
 *---------------------------------------------------------------------------*/
#define __SYSTEM_CLOCK (48000000)


enum mcan_dlc
{
    CAN_DLC_0 = 0,
    CAN_DLC_1 = 1,
    CAN_DLC_2 = 2,
    CAN_DLC_3 = 3,
    CAN_DLC_4 = 4,
    CAN_DLC_5 = 5,
    CAN_DLC_6 = 6,
    CAN_DLC_7 = 7,
    CAN_DLC_8 = 8,
    CAN_DLC_12 = 9,
    CAN_DLC_16 = 10,
    CAN_DLC_20 = 11,
    CAN_DLC_24 = 12,
    CAN_DLC_32 = 13,
    CAN_DLC_48 = 14,
    CAN_DLC_64 = 15
};

/*---------------------------------------------------------------------------
 *        Local functions
 *---------------------------------------------------------------------------*/
uint32_t pmc_get_gck_clock(uint32_t id)
{
    (void)id;
    return __SYSTEM_CLOCK;
}
/**
 * \brief Convert data length to Data Length Code.
 * \param len  length, in bytes
 * \param dlc  address where the matching Data Length Code will be written
 * \return true if a code matched the provided length, false if this exact
 * length is not supported.
 */
static BOOL get_length_code(UINT8 len, enum mcan_dlc *dlc)
{
    assert(dlc);

    if (len <= 8)
    {
        *dlc = (enum mcan_dlc)len;
        return true;
    }
    if (len % 4)
        return false;
    len /= 4;
    if (len <= 6)
    {
        *dlc = (enum mcan_dlc)(len + 6);
        return true;
    }
    if (len % 4)
        return false;
    len /= 4;
    if (len > 4)
        return false;
    *dlc = (enum mcan_dlc)(len + 11);
    return true;
}

/**
 * \brief Convert Data Length Code to actual data length.
 * \param dlc  CAN_DLC_xx enum value
 * \return Data length, expressed in bytes.
 */
static UINT8 get_data_length(enum mcan_dlc dlc)
{
    assert((dlc == CAN_DLC_0 || dlc > CAN_DLC_0) && dlc <= CAN_DLC_64);

    if (dlc <= CAN_DLC_8)
        return(UINT8)dlc;
    if (dlc <= CAN_DLC_24)
        return((UINT8)dlc - 6) * 4;
    return((UINT8)dlc - 11) * 16;
}

/**
 * \brief Compute the size of the Message RAM, depending on the application.
 * \param set  Pointer to a MCAN instance that will be setup accordingly.
 * \param cfg  MCAN configuration to be considered. Only integer size parameters
 * need to be configured. The other parameters can be left blank at this stage.
 * \param size  address where the required size of the Message RAM will be
 * written, expressed in (32-bit) words.
 * \return true if successful, false if a parameter is set to an unsupported
 * value.
 */
static BOOL configure_ram(struct MCAN_SET *set, const struct MCAN_CONFIG *cfg, UINT32 *size)
{
    if (cfg->array_size_filt_std > 128 || cfg->array_size_filt_ext > 64
        || cfg->fifo_size_rx0 > 64 || cfg->fifo_size_rx1 > 64
        || cfg->array_size_rx > 64 || cfg->fifo_size_tx_evt > 32
        || cfg->array_size_tx > 32 || cfg->fifo_size_tx > 32
        || cfg->array_size_tx + cfg->fifo_size_tx > 32
        || cfg->buf_size_rx_fifo0 > 64 || cfg->buf_size_rx_fifo1 > 64
        || cfg->buf_size_rx > 64 || cfg->buf_size_tx > 64)
        return false;

    set->ram_filt_std = cfg->msg_ram;
    *size = (UINT32)cfg->array_size_filt_std * MCAN_RAM_FILT_STD_SIZE;
    set->ram_filt_ext = cfg->msg_ram + *size;
    *size += (UINT32)cfg->array_size_filt_ext * MCAN_RAM_FILT_EXT_SIZE;
    set->ram_fifo_rx0 = cfg->msg_ram + *size;
    *size += (UINT32)cfg->fifo_size_rx0 * (MCAN_RAM_BUF_HDR_SIZE
                                           + cfg->buf_size_rx_fifo0 / 4);
    set->ram_fifo_rx1 = cfg->msg_ram + *size;
    *size += (UINT32)cfg->fifo_size_rx1 * (MCAN_RAM_BUF_HDR_SIZE
                                           + cfg->buf_size_rx_fifo1 / 4);
    set->ram_array_rx = cfg->msg_ram + *size;
    *size += (UINT32)cfg->array_size_rx * (MCAN_RAM_BUF_HDR_SIZE
                                           + cfg->buf_size_rx / 4);
    set->ram_fifo_tx_evt = cfg->msg_ram + *size;
    *size += (UINT32)cfg->fifo_size_tx_evt * MCAN_RAM_TX_EVT_SIZE;
    set->ram_array_tx = cfg->msg_ram + *size;
    *size += (UINT32)cfg->array_size_tx * (MCAN_RAM_BUF_HDR_SIZE
                                           + cfg->buf_size_tx / 4);
    *size += (UINT32)cfg->fifo_size_tx * (MCAN_RAM_BUF_HDR_SIZE
                                          + cfg->buf_size_tx / 4);
    return true;
}

/*---------------------------------------------------------------------------
 *      Exported Functions
 *---------------------------------------------------------------------------*/

BOOL mcan_configure_msg_ram(const struct MCAN_CONFIG *cfg, UINT32 *size)
{
    assert(cfg);
    assert(size);

    struct MCAN_SET tmp_set = { .cfg = { 0}};

    return configure_ram(&tmp_set, cfg, size);
}

BOOL mcan_initialize(struct MCAN_SET *set, const struct MCAN_CONFIG *cfg)
{

    assert(set);
    assert(cfg);
    assert(cfg->regs);
    assert(cfg->msg_ram);

    Can *mcan = cfg->regs;
    UINT32 *element = NULL, *elem_end = NULL;
    UINT32 freq, regVal32;
    enum mcan_dlc dlc;

    memset(set, 0, sizeof(*set));
    if (!configure_ram(set, cfg, &regVal32))
        return false;
    set->cfg = *cfg;

    /* SAMC21 does not set the MSB of the Message RAM Base Address */

    /* Reset the CC Control Register */
    mcan->CCCR.reg = 0 | CAN_CCCR_INIT;

    mcan_disable(set);
    mcan_reconfigure(set);

    /* Global Filter Configuration: Reject remote frames, reject non-matching frames */
    mcan->GFC.reg = CAN_GFC_RRFE | CAN_GFC_RRFS
                    | CAN_GFC_ANFE(2) | CAN_GFC_ANFS(2);

    /* Extended ID Filter AND mask */
    mcan->XIDAM.reg = 0x1FFFFFFF;

    /* Interrupt configuration - leave initialization with all interrupts off
     * Disable all interrupts */
    mcan->IE.reg = 0;
    mcan->TXBTIE.reg = 0x00000000;
    /* All interrupts directed to Line 0 */
    mcan->ILS.reg = 0x00000000;
    /* Disable both interrupt LINE 0 & LINE 1 */
    mcan->ILE.reg = 0x00;
    /* Clear all interrupt flags */
    mcan->IR.reg = 0xFFCFFFFF;

    /* Configure CAN bit timing */
    if (cfg->bit_rate == 0
        || cfg->quanta_before_sp < 3 || cfg->quanta_before_sp > 257
        || cfg->quanta_after_sp < 1 || cfg->quanta_after_sp > 128
        || cfg->quanta_sync_jump < 1 || cfg->quanta_sync_jump > 128)
        return false;
    /* Retrieve the frequency of the CAN core clock i.e. the Generated Clock */
    freq = pmc_get_gck_clock(cfg->id);

    /* Compute the Nominal Baud Rate Prescaler */
    regVal32 = ROUND_INT_DIV(freq, cfg->bit_rate
                             * (cfg->quanta_before_sp + cfg->quanta_after_sp));
    if (regVal32 < 1 || regVal32 > 512)
        return false;

    /* Apply bit timing configuration */
    mcan->NBTP.reg = CAN_NBTP_NBRP(regVal32 - 1)
                     | CAN_NBTP_NTSEG1(cfg->quanta_before_sp - 1 - 1)
                     | CAN_NBTP_NTSEG2(cfg->quanta_after_sp - 1)
                     | CAN_NBTP_NSJW(cfg->quanta_sync_jump - 1);

    /* Configure fast CAN FD bit timing */
    if (cfg->bit_rate_fd < cfg->bit_rate
        || cfg->quanta_before_sp_fd < 3 || cfg->quanta_before_sp_fd > 33
        || cfg->quanta_after_sp_fd < 1 || cfg->quanta_after_sp_fd > 16
        || cfg->quanta_sync_jump_fd < 1 || cfg->quanta_sync_jump_fd > 8)
        return false;

    /* Compute the Fast Baud Rate Prescaler */
    regVal32 = ROUND_INT_DIV(freq, cfg->bit_rate_fd * (cfg->quanta_before_sp_fd + cfg->quanta_after_sp_fd));
    if (regVal32 < 1 || regVal32 > 32)
        return false;


    // this is all temporary.modified to properly accomodate 2Mb SAEJ2284-4
    if (cfg->bit_rate_fd == 2000000)
    {
        //Get rid of all the subtraction from mcan.c, this works based upon actual desired values of DTSEG1, DTSEG2 and JUMPWIDTH
        mcan->DBTP.reg = CAN_DBTP_DBRP(0)
                         | CAN_DBTP_DTSEG1(cfg->quanta_before_sp_fd)
                         | CAN_DBTP_DTSEG2(cfg->quanta_after_sp_fd)
                         | CAN_DBTP_DSJW(cfg->quanta_sync_jump_fd);

    } else
    {
        /* Apply bit timing configuration */
        mcan->DBTP.reg = CAN_DBTP_DBRP(regVal32 - 1)
                         | CAN_DBTP_DTSEG1(cfg->quanta_before_sp_fd - 1 - 1)
                         | CAN_DBTP_DTSEG2(cfg->quanta_after_sp_fd - 1)
                         | CAN_DBTP_DSJW(cfg->quanta_sync_jump_fd - 1);
    }
    
    //set delay bit and offset for FD bauds greater than 1Mb, include 100nS delay compensation
    if (cfg->bit_rate_fd >= 1000000)
    {
        mcan->DBTP.reg |= 0x00800000;
        mcan->TDCR.reg = (0x13 << 8);

    }

    /* Configure Message RAM starting addresses and element count */
    /* 11-bit Message ID Rx Filters */
    mcan->SIDFC.reg =
    CAN_SIDFC_FLSSA((UINT32)set->ram_filt_std)
    | CAN_SIDFC_LSS(cfg->array_size_filt_std);
    /* 29-bit Message ID Rx Filters */
    mcan->XIDFC.reg =
    CAN_XIDFC_FLESA((UINT32)set->ram_filt_ext)
    | CAN_XIDFC_LSE(cfg->array_size_filt_ext);
    /* Rx FIFO 0 */
    mcan->RXF0C.reg =
    CAN_RXF0C_F0SA((UINT32)set->ram_fifo_rx0)
    | CAN_RXF0C_F0S(cfg->fifo_size_rx0)
    | CAN_RXF0C_F0WM(0)
    | 0;   /* clear MCAN_RXF0C_F0OM */
    /* Rx FIFO 1 */
    mcan->RXF1C.reg =
    CAN_RXF1C_F1SA((UINT32)set->ram_fifo_rx1)
    | CAN_RXF1C_F1S(cfg->fifo_size_rx1)
    | CAN_RXF1C_F1WM(0)
    | 0;   /* clear MCAN_RXF1C_F1OM */
    /* Dedicated Rx Buffers
     * Note: the HW does not know (and does not care about) how many
     * dedicated Rx Buffers are used by the application. */
    mcan->RXBC.reg =
    CAN_RXBC_RBSA((UINT32)set->ram_array_rx);
    /* Tx Event FIFO */
    mcan->TXEFC.reg =
    CAN_TXEFC_EFSA((UINT32)set->ram_fifo_tx_evt)
    | CAN_TXEFC_EFS(cfg->fifo_size_tx_evt)
    | CAN_TXEFC_EFWM(0);
    /* Tx Buffers */
    mcan->TXBC.reg =
    CAN_TXBC_TBSA((UINT32)set->ram_array_tx)
    | CAN_TXBC_NDTB(cfg->array_size_tx)
    | CAN_TXBC_TFQS(cfg->fifo_size_tx)
    | 0;   /* clear MCAN_TXBC_TFQM */

    /* Configure the size of data fields in Rx and Tx Buffer Elements */
    if (!get_length_code(cfg->buf_size_rx_fifo0, &dlc))
        return false;
    regVal32 = CAN_RXESC_F0DS(dlc < CAN_DLC_8 ? 0 : dlc - CAN_DLC_8);
    if (!get_length_code(cfg->buf_size_rx_fifo1, &dlc))
        return false;
    regVal32 |= CAN_RXESC_F1DS(dlc < CAN_DLC_8 ? 0 : dlc - CAN_DLC_8);
    if (!get_length_code(cfg->buf_size_rx, &dlc))
        return false;
    regVal32 |= CAN_RXESC_RBDS(dlc < CAN_DLC_8 ? 0 : dlc - CAN_DLC_8);
    mcan->RXESC.reg = regVal32;
    if (!get_length_code(cfg->buf_size_tx, &dlc))
        return false;
    mcan->TXESC.reg =
    CAN_TXESC_TBDS(dlc < CAN_DLC_8 ? 0 : dlc - CAN_DLC_8);

    /* Configure Message ID Filters
     * ...Disable all standard filters */
    for (element = set->ram_filt_std, elem_end = set->ram_filt_std
         + (UINT32)cfg->array_size_filt_std * MCAN_RAM_FILT_STD_SIZE;
        element < elem_end;
        element += MCAN_RAM_FILT_STD_SIZE)
        element[0] = MCAN_RAM_FILT_SFEC_DIS;
    /* ...Disable all extended filters */
    for (element = set->ram_filt_ext, elem_end = set->ram_filt_ext
         + (UINT32)cfg->array_size_filt_ext * MCAN_RAM_FILT_EXT_SIZE;
        element < elem_end;
        element += MCAN_RAM_FILT_EXT_SIZE)
        element[0] = MCAN_RAM_FILT_EFEC_DIS;

    mcan->NDAT1.reg = 0xFFFFFFFF;   /* clear new (rx) data flags */
    mcan->NDAT2.reg = 0xFFFFFFFF;   /* clear new (rx) data flags */

    regVal32 = mcan->CCCR.reg & ~(CAN_CCCR_BRSE | CAN_CCCR_FDOE);
    mcan->CCCR.reg = regVal32 | CAN_CCCR_PXHD;

    return true;
}

void mcan_reconfigure(struct MCAN_SET *set)
{
    Can *mcan = set->cfg.regs;
    UINT32 regVal32;

    regVal32 = mcan->CCCR.reg & ~CAN_CCCR_CCE;
    assert((regVal32 & CAN_CCCR_INIT) == CAN_CCCR_INIT);
    /* Enable writing to configuration registers */
    mcan->CCCR.reg = regVal32 | CAN_CCCR_CCE;
}

void mcan_set_mode(struct MCAN_SET *set, MCAN_MODE mode)
{
    Can *mcan = set->cfg.regs;
    UINT32 regVal32;

    regVal32 = mcan->CCCR.reg & ~(CAN_CCCR_BRSE | CAN_CCCR_FDOE);
    switch (mode)
    {
        case MCAN_MODE_CAN:
            regVal32 |= 0;
            break;
        case MCAN_MODE_EXT_LEN_CONST_RATE:
            regVal32 |= CAN_CCCR_FDOE;
            break;
        case MCAN_MODE_EXT_LEN_DUAL_RATE:
            regVal32 |= CAN_CCCR_BRSE | CAN_CCCR_FDOE;
            break;
        default:
            return;
    }
    mcan->CCCR.reg = regVal32;
}

MCAN_MODE mcan_get_mode(const struct MCAN_SET *set)
{
    const UINT32 cccr = set->cfg.regs->CCCR.reg;
    MCAN_MODE retVal = 0;

    if (!(cccr & CAN_CCCR_FDOE))
    {
        retVal = MCAN_MODE_CAN;

    } else if (!(cccr & CAN_CCCR_BRSE))
    {
        retVal = MCAN_MODE_EXT_LEN_CONST_RATE;
    } else
    {
        retVal = MCAN_MODE_EXT_LEN_DUAL_RATE;
    }
    return(retVal);
}

void mcan_init_loopback(struct MCAN_SET *set)
{
    Can *mcan = set->cfg.regs;

    mcan->CCCR.reg |= CAN_CCCR_TEST;
#if 0
    mcan->CCCR.reg |= CAN_CCCR_MON;   /* for internal loop back */
#endif
    mcan->TEST.reg |= CAN_TEST_LBCK;
}

void mcan_set_tx_queue_mode(struct MCAN_SET *set)
{
    Can *mcan = set->cfg.regs;
    mcan->TXBC.reg |= CAN_TXBC_TFQM;
}

void mcan_enable(struct MCAN_SET *set)
{
    UINT32 index, val;

    /* Depending on bus condition, the HW may switch back to the
     * Initialization state, by itself. Therefore, upon timeout, return.
     * [Using an arbitrary timeout criterion.] */
    for (index = 0; index < 1024; index++)
    {
        val = set->cfg.regs->CCCR.reg;
        if (!(val & CAN_CCCR_INIT))
            break;
        if (index == 0)
            set->cfg.regs->CCCR.reg = (val & ~CAN_CCCR_INIT);
    }
}

void mcan_disable(struct MCAN_SET *set)
{
    UINT32 val;
    BOOL initial;

    for (initial = true; true; initial = false)
    {
        val = set->cfg.regs->CCCR.reg;
        if ((val & CAN_CCCR_INIT) == CAN_CCCR_INIT)
            break;
        if (initial)
            set->cfg.regs->CCCR.reg = (val & ~CAN_CCCR_INIT)
                                      | CAN_CCCR_INIT;
    }
}

void mcan_loopback_on(struct MCAN_SET *set)
{
    Can *mcan = set->cfg.regs;
    mcan->TEST.reg |= CAN_TEST_LBCK;
}

void mcan_loopback_off(struct MCAN_SET *set)
{
    Can *mcan = set->cfg.regs;
    mcan->TEST.reg &= ~CAN_TEST_LBCK;
}

void mcan_enable_rx_array_flag(struct MCAN_SET *set, UINT8 line)
{
    assert(line == 0 || line == 1);

    Can *mcan = set->cfg.regs;
    if (line)
    {
        mcan->ILS.reg |= CAN_ILS_DRXL;
        mcan->ILE.reg |= CAN_ILE_EINT1;
    } else
    {
        mcan->ILS.reg &= ~CAN_ILS_DRXL;
        mcan->ILE.reg |= CAN_ILE_EINT0;
    }
    mcan->IR.reg = CAN_IR_DRX;   /* clear previous flag */
    mcan->IE.reg |= CAN_IE_DRXE;   /* enable it */
}

UINT8 * mcan_prepare_tx_buffer(struct MCAN_SET *set, UINT8 buf_idx,
                               UINT32 id, UINT8 len)
{
    assert(buf_idx < set->cfg.array_size_tx);
    assert(len <= set->cfg.buf_size_tx);

    Can *mcan = set->cfg.regs;
    UINT32 *pThisTxBuf = 0;
    UINT32 val;
    MCAN_MODE mode = mcan_get_mode(set);
    enum mcan_dlc dlc;

    if (buf_idx >= set->cfg.array_size_tx)
        return NULL;
    if (!get_length_code(len, &dlc))
        dlc = CAN_DLC_0;
    pThisTxBuf = set->ram_array_tx + buf_idx
                 * (MCAN_RAM_BUF_HDR_SIZE + set->cfg.buf_size_tx / 4);
    if (mcan_is_extended_id(id))
        *pThisTxBuf++ = MCAN_RAM_BUF_XTD | MCAN_RAM_BUF_ID_XTD(id);
    else
        *pThisTxBuf++ = MCAN_RAM_BUF_ID_STD(id);
    val = MCAN_RAM_BUF_MM(0) | MCAN_RAM_BUF_DLC((UINT32)dlc);
    if (mode == MCAN_MODE_EXT_LEN_CONST_RATE)
        val |= MCAN_RAM_BUF_FDF;
    else if (mode == MCAN_MODE_EXT_LEN_DUAL_RATE)
        val |= MCAN_RAM_BUF_FDF | MCAN_RAM_BUF_BRS;
    *pThisTxBuf++ = val;
    /* enable transmit from buffer to set TC interrupt bit in IR,
     * but interrupt will not happen unless TC interrupt is enabled */
    mcan->TXBTIE.reg = (1 << buf_idx);
    return(UINT8 *)pThisTxBuf;   /* now it points to the data field */
}

void mcan_send_tx_buffer(struct MCAN_SET *set, UINT8 buf_idx)
{
    Can *mcan = set->cfg.regs;

    if (buf_idx < set->cfg.array_size_tx)
        mcan->TXBAR.reg = (1 << buf_idx);
}

UINT8 mcan_enqueue_outgoing_msg(struct MCAN_SET *set, UINT32 id,
                                UINT8 len, const UINT8 *data)
{
    assert(len <= set->cfg.buf_size_tx);

    Can *mcan = set->cfg.regs;
    UINT32 val;
    UINT32 *pThisTxBuf = 0;
    MCAN_MODE mode = mcan_get_mode(set);
    enum mcan_dlc dlc;
    UINT8 putIdx = 255;

    if (!get_length_code(len, &dlc))
        dlc = CAN_DLC_0;
    /* Configured for FifoQ and FifoQ not full? */
    if (set->cfg.fifo_size_tx == 0 || (mcan->TXFQS.reg & CAN_TXFQS_TFQF))
        return putIdx;
    putIdx = (UINT8)((mcan->TXFQS.reg & CAN_TXFQS_TFQPI_Msk)
                     >> CAN_TXFQS_TFQPI_Pos);
    pThisTxBuf = set->ram_array_tx + (UINT32)
                 putIdx * (MCAN_RAM_BUF_HDR_SIZE + set->cfg.buf_size_tx / 4);
    if (mcan_is_extended_id(id))
        *pThisTxBuf++ = MCAN_RAM_BUF_XTD | MCAN_RAM_BUF_ID_XTD(id);
    else
        *pThisTxBuf++ = MCAN_RAM_BUF_ID_STD(id);
    val = MCAN_RAM_BUF_MM(0) | MCAN_RAM_BUF_DLC((UINT32)dlc);
    if (mode == MCAN_MODE_EXT_LEN_CONST_RATE)
        val |= MCAN_RAM_BUF_FDF;
    else if (mode == MCAN_MODE_EXT_LEN_DUAL_RATE)
        val |= MCAN_RAM_BUF_FDF | MCAN_RAM_BUF_BRS;
    *pThisTxBuf++ = val;
    memcpy(pThisTxBuf, data, len);
    /* enable transmit from buffer to set TC interrupt bit in IR,
     * but interrupt will not happen unless TC interrupt is enabled
     */
    mcan->TXBTIE.reg = (1 << putIdx);
    /* request to send */
    mcan->TXBAR.reg = (1 << putIdx);
    return putIdx;
}

BOOL mcan_is_buffer_sent(const struct MCAN_SET *set, UINT8 buf_idx)
{
    Can *mcan = set->cfg.regs;
    return mcan->TXBTO.reg & (1 << buf_idx) ? true : false;
}

void mcan_filter_single_id(struct MCAN_SET *set,
                           UINT8 buf_idx, UINT8 filter, UINT32 id)
{
    assert(buf_idx < set->cfg.array_size_rx);
    assert(id & CAN_EXT_MSG_ID ? filter < set->cfg.array_size_filt_ext
           : filter < set->cfg.array_size_filt_std);
    assert(id & CAN_EXT_MSG_ID ? (id & ~CAN_EXT_MSG_ID) <= 0x1fffffff :
           id <= 0x7ff);

    UINT32 *pThisRxFilt = 0;

    if (buf_idx >= set->cfg.array_size_rx)
        return;
    if (mcan_is_extended_id(id))
    {
        pThisRxFilt = set->ram_filt_ext + filter
                      * MCAN_RAM_FILT_EXT_SIZE;
        *pThisRxFilt++ = MCAN_RAM_FILT_EFEC_BUF
                         | MCAN_RAM_FILT_EFID1(id);
        *pThisRxFilt = MCAN_RAM_FILT_EFID2_BUF
                       | MCAN_RAM_FILT_EFID2_BUF_IDX(buf_idx);
    } else
    {
        pThisRxFilt = set->ram_filt_std + filter
                      * MCAN_RAM_FILT_STD_SIZE;
        *pThisRxFilt = MCAN_RAM_FILT_SFEC_BUF
                       | MCAN_RAM_FILT_SFID1(id)
                       | MCAN_RAM_FILT_SFID2_BUF
                       | MCAN_RAM_FILT_SFID2_BUF_IDX(buf_idx);
    }
}

void mcan_filter_id_mask(struct MCAN_SET *set, UINT8 fifo, UINT8 filter, UINT32 id, UINT32 mask)
{

    UINT32 *pThisRxFilt = 0;
    UINT32 val;

    if (id > 0x7FF)
    {

        pThisRxFilt = set->ram_filt_ext + filter * MCAN_RAM_FILT_EXT_SIZE;

        *pThisRxFilt++ = (fifo ? MCAN_RAM_FILT_EFEC_FIFO1 : MCAN_RAM_FILT_EFEC_FIFO0) | MCAN_RAM_FILT_EFID1(id);

        *pThisRxFilt = MCAN_RAM_FILT_EFT_CLASSIC | MCAN_RAM_FILT_EFID2(mask);
    } else
    {

        pThisRxFilt = set->ram_filt_std + filter  * MCAN_RAM_FILT_STD_SIZE;

        val = MCAN_RAM_FILT_SFT_CLASSIC | MCAN_RAM_FILT_SFID1(id) | MCAN_RAM_FILT_SFID2(mask);

        *pThisRxFilt = (fifo ? MCAN_RAM_FILT_SFEC_FIFO1 : MCAN_RAM_FILT_SFEC_FIFO0) | val;
    }
}

void mcan_filter_id_range(struct MCAN_SET *set, UINT32 startId, UINT32 endId)
{
    UINT32 *pThisRxFilt = 0;
    UINT32 val;


    if (endId >= startId)
    {

        //setup filter zero for extended ID's (both FIFO 0)
        if (endId > 0x7FF)
        {
            pThisRxFilt = set->ram_filt_ext;

            *pThisRxFilt++ = MCAN_RAM_FILT_EFEC_FIFO0 | MCAN_RAM_FILT_EFID1(startId);

            *pThisRxFilt = MCAN_RAM_FILT_EFT_RANGE | MCAN_RAM_FILT_EFID2(endId);
        }

        //setup filter 1 for standard ID's
        if (endId <0x7FF || startId <0x7FF)
        {
            pThisRxFilt = set->ram_filt_std + 1 * MCAN_RAM_FILT_STD_SIZE;

            val = MCAN_RAM_FILT_EFT_RANGE_EIDM | MCAN_RAM_FILT_SFID1(startId) | MCAN_RAM_FILT_SFID2(endId > 0x7FF ? 0x7FF : endId);

            *pThisRxFilt = MCAN_RAM_FILT_SFEC_FIFO0 | val;
        }

    }
}


BOOL mcan_rx_buffer_data(const struct MCAN_SET *set, UINT8 buf_idx)
{
    Can *mcan = set->cfg.regs;

    if (buf_idx < 32)
        return mcan->NDAT1.reg & (1 << buf_idx) ? true : false;
    else if (buf_idx < 64)
        return mcan->NDAT2.reg & (1 << (buf_idx - 32)) ? true : false;
    else
        return false;
}

void mcan_read_rx_buffer(struct MCAN_SET *set, UINT8 buf_idx,
                         struct MCAN_MSG_INFO *msg)
{
    assert(buf_idx < set->cfg.array_size_rx);

    Can *mcan = set->cfg.regs;
    const UINT32 *pThisRxBuf = 0;
    UINT32 tempRy;   /* temp copy of RX buffer word */
    UINT8 len;

    if (buf_idx >= set->cfg.array_size_rx)
    {
        msg->id = 0;
        msg->timestamp = 0;
        msg->full_len = 0;
        msg->data_len = 0;
        return;
    }
    pThisRxBuf = set->ram_array_rx + (buf_idx
                                      * (MCAN_RAM_BUF_HDR_SIZE + set->cfg.buf_size_rx / 4));
    tempRy = *pThisRxBuf++;   /* word R0 contains ID */
    if (tempRy & MCAN_RAM_BUF_XTD)
        msg->id = tempRy & MCAN_RAM_BUF_ID_XTD_Msk;
    //msg->id = CAN_EXT_MSG_ID | (tempRy & MCAN_RAM_BUF_ID_XTD_Msk) >> MCAN_RAM_BUF_ID_XTD_Pos;
    else
        msg->id = (tempRy & MCAN_RAM_BUF_ID_STD_Msk)
                  >> MCAN_RAM_BUF_ID_STD_Pos;
    tempRy = *pThisRxBuf++;   /* word R1 contains DLC & time stamp */
    msg->full_len = len = get_data_length((enum mcan_dlc)
                                          ((tempRy & MCAN_RAM_BUF_DLC_Msk) >> MCAN_RAM_BUF_DLC_Pos));
    msg->timestamp = (tempRy & MCAN_RAM_BUF_RXTS_Msk)
                     >> MCAN_RAM_BUF_RXTS_Pos;
    if (msg->data)
    {
        /* copy the data from the Rx Buffer Element to the
         * application-owned buffer */
        if (len > set->cfg.buf_size_rx)
            len = set->cfg.buf_size_rx;
        if (len > msg->data_len)
            len = msg->data_len;
        memcpy(msg->data, pThisRxBuf, len);
        msg->data_len = len;
    } else
        msg->data_len = 0;
    /* clear the new data flag for the buffer */
    if (buf_idx < 32)
        mcan->NDAT1.reg = (1 << buf_idx);
    else
        mcan->NDAT2.reg = (1 << (buf_idx - 32));
}

UINT8 mcan_dequeue_received_msg(struct MCAN_SET *set, UINT8 fifo,
                                struct MCAN_MSG_INFO *msg)
{
    assert(fifo == 0 || fifo == 1);

    Can *mcan = set->cfg.regs;
    UINT32 *pThisRxBuf = 0;
    UINT32 tempRy;   /* temp copy of RX buffer word */
    UINT8 buf_elem_data_size, len;
    UINT32 *fifo_ack_reg;
    UINT32 get_index;
    UINT8 fill_level = 0;   /* default: fifo empty */

    if (fifo)
    {
        get_index = (mcan->RXF1S.reg & CAN_RXF1S_F1GI_Msk) >>
                    CAN_RXF1S_F1GI_Pos;
        fill_level = (UINT8)((mcan->RXF1S.reg & CAN_RXF1S_F1FL_Msk)
                             >> CAN_RXF1S_F1FL_Pos);
        pThisRxBuf = set->ram_fifo_rx1;
        buf_elem_data_size = set->cfg.buf_size_rx_fifo1;
        fifo_ack_reg = (UINT32 *) & mcan->RXF1A;
    } else
    {
        get_index = (mcan->RXF0S.reg & CAN_RXF0S_F0GI_Msk)
                    >> CAN_RXF0S_F0GI_Pos;
        fill_level = (UINT8)((mcan->RXF0S.reg & CAN_RXF0S_F0FL_Msk)
                             >> CAN_RXF0S_F0FL_Pos);
        pThisRxBuf = set->ram_fifo_rx0;
        buf_elem_data_size = set->cfg.buf_size_rx_fifo0;
        fifo_ack_reg = (UINT32 *) & mcan->RXF0A.reg;
    }

    if (fill_level == 0)
        return 0;

    pThisRxBuf += get_index * (MCAN_RAM_BUF_HDR_SIZE + buf_elem_data_size
                               / 4);
    tempRy = *pThisRxBuf++;   /* word R0 contains ID */
    if (tempRy & MCAN_RAM_BUF_XTD)
        msg->id = tempRy & MCAN_RAM_BUF_ID_XTD_Msk;
    //msg->id = CAN_EXT_MSG_ID | (tempRy & MCAN_RAM_BUF_ID_XTD_Msk) >> MCAN_RAM_BUF_ID_XTD_Pos;
    else
        msg->id = (tempRy & MCAN_RAM_BUF_ID_STD_Msk)
                  >> MCAN_RAM_BUF_ID_STD_Pos;
    tempRy = *pThisRxBuf++;   /* word R1 contains DLC & timestamps */
    msg->full_len = len = get_data_length((enum mcan_dlc)
                                          ((tempRy & MCAN_RAM_BUF_DLC_Msk) >> MCAN_RAM_BUF_DLC_Pos));
    msg->timestamp = (tempRy & MCAN_RAM_BUF_RXTS_Msk)
                     >> MCAN_RAM_BUF_RXTS_Pos;
    if (msg->data)
    {
        /* copy the data from the Rx Buffer Element to the
         * application-owned buffer */
        if (len > buf_elem_data_size)
            len = buf_elem_data_size;
        //if (len > msg->data_len)        //DKasamis removed this for a bug that seemed to latch the dlc wrong.
        //	len = msg->data_len;
        memcpy(msg->data, pThisRxBuf, len);
        msg->data_len = len;
    } else
        msg->data_len = 0;
    /* acknowledge reading the fifo entry */
    *fifo_ack_reg = get_index;
    /* return entries remaining in FIFO */
    return(fill_level);
}

/**@}*/
