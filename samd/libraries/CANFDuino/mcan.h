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

/**
 * \file
 *
 * \section Purpose
 *
 * Controller Area Network with Flexible Data-rate.
 * Interface for configuring and using the MCAN peripheral.
 */

#ifndef _MCAN_H_
#define _MCAN_H_

/*----------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include "typedef.h"
#include <assert.h>

/* MCAN-related #defines missing from the SAMC21 peripheral definitions */
#include "mcan_helper.h"
#include "compiler.h"
#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 *         Definitions
 *----------------------------------------------------------------------------*/

typedef enum 
{
	/* ISO 11898-1 CAN mode */
	MCAN_MODE_CAN,

	/* Long CAN FD frame.
	 * - TX and RX payloads up to 64 bytes.
	 * - Slow transmitter: TX frames are sent at constant bit rate.
	 * - Fast receiver: both constant-rate (slow) and dual-rate (fast)
	 *   RX frames are supported and received. */
	MCAN_MODE_EXT_LEN_CONST_RATE,

	/* Long and fast CAN FD frame.
	 * - TX and RX payloads up to 64 bytes.
	 * - Fast transmitter: control, data and CRC fields are transmitted at a
	 *   higher bit rate.
	 * - Fast receiver. */
	MCAN_MODE_EXT_LEN_DUAL_RATE,
}MCAN_MODE;

/* Flag signalling a standard (11-bit) message identifiers */
#define CAN_STD_MSG_ID (0x0u << 30)
/* Flag to be bitwise or'ed to extended (29-bit) message identifiers */
#define CAN_EXT_MSG_ID (0x1u << 30)

struct MCAN_MSG_INFO
{
	UINT32 id;
	UINT32 timestamp;
	UINT8 *data;
	UINT8 full_len;
	UINT8 data_len;
};

struct MCAN_CONFIG
{
	UINT32 id;                  /* peripheral ID (ID_xxx) */
	Can *regs;                   /* set of MCAN hardware registers */
	UINT32 *msg_ram;            /* base address of the Message RAM to be
	                               * assigned to this MCAN instance */

	UINT8 array_size_filt_std;  /* # of 11-bit Message ID Rx Filters */
	UINT8 array_size_filt_ext;  /* # of 29-bit Message ID Rx Filters */
	UINT8 fifo_size_rx0;        /* # of Rx Buffers in Rx FIFO 0 */
	UINT8 fifo_size_rx1;        /* # of Rx Buffers in Rx FIFO 1 */
	UINT8 array_size_rx;        /* # of dedicated Rx Buffers */
	UINT8 fifo_size_tx_evt;     /* # of Tx Event Elements in the Tx Event
	                               * FIFO */
	UINT8 array_size_tx;        /* # of dedicated Tx Buffers */
	UINT8 fifo_size_tx;         /* # of Tx Buffers in the Tx FIFO or Tx
	                               * Queue */

	UINT8 buf_size_rx_fifo0;    /* size of the data field in each Rx
	                               * Buffer of Rx FIFO 0, in bytes */
	UINT8 buf_size_rx_fifo1;    /* size of the data field in each Rx
	                               * Buffer of Rx FIFO 1, in bytes */
	UINT8 buf_size_rx;          /* size of the data field in each
	                               * dedicated Rx Buffer, in bytes */
	UINT8 buf_size_tx;          /* size of the data field in each Tx
	                               * Buffer, in bytes. Applies to all Tx
	                               * Buffers, dedicated and in Tx FIFO /
	                               * Queue. */

	UINT32 bit_rate;            /* requested CAN bit rate in CAN mode,
	                               * in bps */
	UINT16 quanta_before_sp;    /* duration of the time segment before the
	                               * sample point (Sync_Seg + Prop_Seg +
	                               * Phase_Seg1), while in CAN mode,
	                               * expressed in CAN time quanta */
	UINT8 quanta_after_sp;      /* duration of the time segment after the
	                               * sample point (Phase_Seg2), while in CAN
	                               * mode, expressed in CAN time quanta */

	UINT32 bit_rate_fd;         /* requested CAN bit rate in fast CAN FD
	                               * mode, in bps */
	UINT8 quanta_before_sp_fd;  /* duration of the time segment before the
	                               * sample point (Sync_Seg + Prop_Seg +
	                               * Phase_Seg1), while in fast CAN FD mode,
	                               * expressed in CAN time quanta */
	UINT8 quanta_after_sp_fd;   /* duration of the time segment after the
	                               * sample point (Phase_Seg2), while in
	                               * fast CAN FD mode, expressed in CAN time
	                               * quanta */

	UINT8 quanta_sync_jump;     /* duration of a (re)synchronization jump,
	                               * while in CAN mode, expressed in CAN
	                               * time quanta */
	UINT8 quanta_sync_jump_fd;  /* duration of a (re)synchronization jump,
	                               * while in fast CAN FD mode, expressed in
	                               * CAN time quanta */
};

/* This structure is private to the MCAN Driver.
 * Allocate it but ignore its members. */
struct MCAN_SET
{
	struct MCAN_CONFIG cfg;
	UINT32 *ram_filt_std;
	UINT32 *ram_filt_ext;
	UINT32 *ram_fifo_rx0;
	UINT32 *ram_fifo_rx1;
	UINT32 *ram_array_rx;
	UINT32 *ram_fifo_tx_evt;
	UINT32 *ram_array_tx;
};

/*----------------------------------------------------------------------------
 *         Exported symbols
 *----------------------------------------------------------------------------*/

static inline BOOL mcan_is_enabled(const struct MCAN_SET *set)
{
	Can *mcan = set->cfg.regs;
	return (!(mcan->CCCR.reg & CAN_CCCR_INIT));
}

static inline BOOL mcan_is_extended_id(UINT32 msg_id)
{
	return msg_id & CAN_EXT_MSG_ID ? true : false;
}

static inline UINT32 mcan_get_id(UINT32 msg_id)
{
	return msg_id & CAN_EXT_MSG_ID ? msg_id & 0x1fffffff : msg_id & 0x7ff;
}

static inline BOOL mcan_is_tx_complete(const struct MCAN_SET *set)
{
	Can *mcan = set->cfg.regs;
	return mcan->IR.reg & CAN_IR_TC ? true : false;
}

static inline void mcan_clear_tx_flag(const struct MCAN_SET *set)
{
	Can *mcan = set->cfg.regs;
	mcan->IR.reg = CAN_IR_TC;
}

static inline BOOL mcan_rx_array_data(const struct MCAN_SET *set)
{
	Can *mcan = set->cfg.regs;
	return mcan->IR.reg & CAN_IR_DRX ? true : false;
}

static inline void mcan_clear_rx_array_flag(const struct MCAN_SET *set)
{
	Can *mcan = set->cfg.regs;
	mcan->IR.reg = CAN_IR_DRX;
}

static inline BOOL mcan_rx_fifo_data(const struct MCAN_SET *set, UINT8 fifo)
{
	assert(fifo == 0 || fifo == 1);

	Can *mcan = set->cfg.regs;

	return mcan->IR.reg & (fifo ? CAN_IR_RF1N : CAN_IR_RF0N) ? true
	    : false;
}

static inline void mcan_clear_rx_fifo_flag(const struct MCAN_SET *set,
    UINT8 fifo)
{
	assert(fifo == 0 || fifo == 1);

	Can *mcan = set->cfg.regs;

	mcan->IR.reg = fifo ? CAN_IR_RF1N : CAN_IR_RF0N;
}

/**
 * \brief Compute the size of the Message RAM to be assigned to the MCAN.
 * \param cfg  MCAN configuration to be considered. Only integer size parameters
 * need to be configured. The other parameters can be left blank at this stage.
 * \param size  address where the required size of the Message RAM will be
 * written, expressed in (32-bit) words.
 * \return true if successful, false if a parameter is set to an unsupported
 * value.
 */
BOOL mcan_configure_msg_ram(const struct MCAN_CONFIG *cfg, UINT32 *size);

/**
 * \brief Initialize the MCAN hardware for the given peripheral.
 * Default: Non-FD, ISO 11898-1 CAN mode; mixed mode TX Buffer + FIFO.
 * \param set  Pointer to uninitialized driver instance data.
 * \param cfg  MCAN configuration to be used.
 * \return true if successful, false if a parameter is set to an unsupported
 * value.
 */
BOOL mcan_initialize(struct MCAN_SET *set, const struct MCAN_CONFIG *cfg);

/**
 * \brief Unlock the peripheral configuration so it can be altered.
 * Prerequisite: the peripheral shall be disabled. In case the device has been
 * enabled, call mcan_disable.
 * \param set  Pointer to driver instance data.
 */
void mcan_reconfigure(struct MCAN_SET *set);

/**
 * \brief Select either the legacy ISO 11898-1 CAN mode or the CAN-FD mode,
 * along with the FD variant in the latter case.
 * Should be called further to mcan_initialize() or mcan_reconfigure(), before
 * mcan_enable().
 * \param set  Pointer to driver instance data.
 * \param mode  CAN mode, and FD variant in case of FD mode.
 */
void mcan_set_mode(struct MCAN_SET *set, MCAN_MODE mode);

/**
 * \brief Query the current CAN mode.
 * \param set  Pointer to driver instance data.
 * \return Currently selected CAN mode, and FD variant in case of CAN FD mode.
 */
MCAN_MODE mcan_get_mode(const struct MCAN_SET *set);

/**
 * \brief Select the TX Queue mode, disable TX FIFO mode.
 * INIT must be set - so this should be called between mcan_initialize() and
 * mcan_enable().
 * \param set  Pointer to driver instance data.
 */
void mcan_set_tx_queue_mode(struct MCAN_SET *set);

/**
 * \brief Initialize the MCAN in loop back mode.
 * INIT must be set - so this should be called between mcan_initialize() and
 * mcan_enable().
 * \param set  Pointer to driver instance data.
 */
void mcan_init_loopback(struct MCAN_SET *set);

/**
 * \brief Enable the peripheral I/O stage. Synchronize with the bus.
 * INIT must be set - so this should be called after mcan_initialize().
 * \param set  Pointer to driver instance data.
 */
void mcan_enable(struct MCAN_SET *set);

/**
 * \brief Disable the peripheral I/O stage. Go Bus_Off.
 * \note Subsequent operations may include reconfiguring the peripheral
 * (mcan_reconfigure) and/or re-enabling it (mcan_enable).
 * \param set  Pointer to driver instance data.
 */
void mcan_disable(struct MCAN_SET *set);

/**
 * \brief Turn the loop-back mode ON.
 * \note TEST must be set in MCAN_CCCR. This mode should have been enabled upon
 * initialization.
 * \param set  Pointer to driver instance data.
 */
void mcan_loopback_on(struct MCAN_SET *set);

/**
 * \brief Turn the loop-back mode OFF.
 * \param set  Pointer to driver instance data.
 */
void mcan_loopback_off(struct MCAN_SET *set);

/**
 * \brief Select either the m_can_int0 or the m_can_int1 interrupt line.
 * Also, enable the 'Message stored to Dedicated Receive Buffer' specific
 * interrupt.
 * \param set  Pointer to driver instance data.
 * \param int_line  The interrupt line to be enabled:
 *    0   -> m_can_int0
 *    1   -> m_can_int1.
 */
void mcan_enable_rx_array_flag(struct MCAN_SET *set, UINT8 int_line);

/**
 * \brief Configure a Dedicated TX Buffer.
 * \param set  Pointer to driver instance data.
 * \param buf_idx  Index of the dedicated transmit buffer to be used.
 * \param id  Message ID.
 * \param len  Data length, in bytes.
 * \return Address of data byte 0, part of the transmit buffer.
 */
UINT8 * mcan_prepare_tx_buffer(struct MCAN_SET *set, UINT8 buf_idx,
    UINT32 id, UINT8 len);

/**
 * \brief Start the transmission of a Dedicated TX Buffer.
 * \param set  Pointer to driver instance data.
 * \param buf_idx  Index of the dedicated transmit buffer to be used.
 */
void mcan_send_tx_buffer(struct MCAN_SET *set, UINT8 buf_idx);

/**
 * \brief Append the provided message to the TX FIFO, or to the TX Queue,
 * depending on whether mcan_set_tx_queue_mode() has been invoked or not.
 * \param set  Pointer to driver instance data.
 * \param id  Message ID.
 * \param len  Data length, in bytes.
 * \param data  Pointer to data.
 * \return Index of the assigned transmit buffer, part of the FIFO / queue.
 * Or 0xff if the TX FIFO / queue was full, or an error occurred.
 */
UINT8 mcan_enqueue_outgoing_msg(struct MCAN_SET *set, UINT32 id,
     UINT8 len, const UINT8 *data);

/**
 * \brief Check if message transmitted from the specified TX Buffer, either
 * dedicated or part of the TX FIFO or TX Queue.
 * \param set  Pointer to driver instance data.
 * \param buf_idx  Index of the transmit buffer to be queried.
 * \return true if the message has been successfully transmitted, false
 * otherwise.
 */
BOOL mcan_is_buffer_sent(const struct MCAN_SET *set, UINT8 buf_idx);

/**
 * \brief Configure RX buffer filter.
 * \param set  Pointer to driver instance data.
 * \param buf_idx  Index of the receive buffer to be used as the recipient.
 * \param filter  Index of the filter to be configured.
 * \param id  Single message identifier. Incoming message need to match exactly
 * to be accepted.
 */
void mcan_filter_single_id(struct MCAN_SET *set, UINT8 buf_idx,
    UINT8 filter, UINT32 id);

/**
 * \brief Configure classic RX filter.
 * The classic filters direct the accepted messages to a FIFO, and include both
 * an ID and an ID mask.
 * \param set  Pointer to driver instance data.
 * \param fifo  Index of the RX FIFO to be used as the recipient.
 * \param filter  Index of the filter to be configured.
 * \param id  Message identifier.
 * \param mask  Message identifier mask to be matched.
 */
void mcan_filter_id_mask(struct MCAN_SET *set, UINT8 fifo, UINT8 filter, UINT32 id, UINT32 mask);


void mcan_filter_id_range(struct MCAN_SET *set, UINT32 startId, UINT32 endId);

/**
 * \brief Check whether some data has been received into the specified RX
 * Buffer.
 * \param set  Pointer to driver instance data.
 * \param buf_idx  Index of the receive buffer to be queried.
 * \return true if the receive buffer is flagged as containing an unfetched
 * frame, and false otherwise.
 */
BOOL mcan_rx_buffer_data(const struct MCAN_SET *set, UINT8 buf_idx);

/**
 * \brief Get RX buffer.
 * \param set  Pointer to driver instance data.
 * \param buf_idx  Index of the receive buffer to be read.
 * \param msg  Address where the CAN message properties will be written.
 * The msg->data and msg->data_len parameters shall be initialized prior to
 * calling this function. Message contents will be copied to msg->data if
 * msg->data is not null and if msg->data_len is large enough.
 */
void mcan_read_rx_buffer(struct MCAN_SET *set, UINT8 buf_idx,
    struct MCAN_MSG_INFO *msg);

/**
 * \brief Detach one received message from the specified RX FIFO, and copy it to
 * a buffer owned by the application.
 * \param set  Pointer to driver instance data.
 * \param fifo  Index of the RX FIFO to dequeue from.
 * \param msg  Address where the CAN message properties will be written.
 * The msg->data and msg->data_len parameters shall be initialized prior to
 * calling this function. Message contents will be copied to msg->data if
 * msg->data is not null and if msg->data_len is large enough.
 * \return: # of FIFO entries at the time the function was entered:
 *    0       -> The FIFO was initially empty.
 *    1       -> The FIFO had 1 entry upon entry, but is empty upon exit.
 *    2 to 64 -> The FIFO had several entries upon entry, and still holds one
 *               or more entries upon exit.
 */
UINT8 mcan_dequeue_received_msg(struct MCAN_SET *set, UINT8 fifo,
    struct MCAN_MSG_INFO *msg);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MCAN_H_ */
