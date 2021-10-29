/**
 * @file
 * @author     Scott Price <prices@hugllc.com>
 * @copyright  © 2017 Hunt Utilities Group, LLC
 * @brief      The include file for using the SAMC
 * @details
 */
#ifdef ARDUINO_ARCH_SAMD
#ifndef _SAMC_CAN_H_
#define _SAMC_CAN_H_

#include "sam.h"
#include "mcan.h"
#include "mcan_helper.h"
#include "mcp_can_dfs.h"
#define MAX_CHAR_IN_MESSAGE CAN_MAX_CHAR_IN_MESSAGE

#include <inttypes.h>
#include <string.h>

/* size of our custom Rx and Tx Buffer Elements, in words */
#define RAM_BUF_SIZE                  (MCAN_RAM_BUF_HDR_SIZE + 64u / 4)

#define RAM_ARRAY_SIZE_FILT_STD       (8u)
#define RAM_ARRAY_SIZE_FILT_EXT       (8u)
#define RAM_FIFO_SIZE_RX0             (12u)
/* no Rx FIFO 1 in our Message RAM */
#define RAM_ARRAY_SIZE_RX             (4u)
/* no Tx Event FIFO in our Message RAM */
#define RAM_ARRAY_SIZE_TX             (0u)
#define RAM_FIFO_SIZE_TX              (4u)


/* size of our custom Message RAM, in words */
#define MSG_RAM_SIZE      ( \
                            RAM_ARRAY_SIZE_FILT_STD * MCAN_RAM_FILT_STD_SIZE \
                            + RAM_ARRAY_SIZE_FILT_EXT * MCAN_RAM_FILT_EXT_SIZE \
                            + RAM_FIFO_SIZE_RX0 * RAM_BUF_SIZE \
                            + RAM_ARRAY_SIZE_RX * RAM_BUF_SIZE \
                            + RAM_ARRAY_SIZE_TX * RAM_BUF_SIZE \
                            + RAM_FIFO_SIZE_TX * RAM_BUF_SIZE )

#define MSG_LEN_1_CAN     8
#define MSG_LEN_1_CAN_FD  64
#define MSG_LEN_2_CAN     7
#define MSG_LEN_2_CAN_FD  48
#define MSG_ID_ALLOW_ALL_MASK     0x000ul     /* bits 0 & 1 are don't care */
#define RX_BUFFER_0       0
#define RX_BUFFER_1       1
#define FILTER_0          0
#define FILTER_1          1

struct frame_desc {
    uint32_t id;
    uint8_t data[64];
    uint8_t len;
    uint8_t buf_idx;
};

typedef struct {
    uint32_t id;
    uint8_t len;
    uint8_t ext;
    uint8_t buffer[MAX_CHAR_IN_MESSAGE];
    uint8_t ret;
} mcp_can_buf;

class SAMC21_CAN
{

public:
    /**
    * @brief Checks for the presence of a connector id
    *
    * This function never returns if there is no connector id.
    *
    * @return void 
    * SAMC21G18A
    */
    SAMC21_CAN(uint8_t _CS, uint8_t canid = ID_CAN0, uint8_t cantx = 22, uint8_t group = 1);
    uint8_t begin(uint8_t idmodeset, uint32_t speedset, uint8_t clockset);
    uint8_t init_Mask(uint8_t num, uint8_t ext, uint32_t ulData);              // Initilize Mask(s)
    uint8_t init_Mask(uint8_t num, uint32_t ulData);                          // Initilize Mask(s)
    uint8_t init_Filt(uint8_t num, uint8_t ext, uint32_t ulData);              // Initilize Filter(s)
    uint8_t init_Filt(uint8_t num, uint32_t ulData); // Initilize Filter(s)
    uint8_t setMode(uint8_t opMode);                                        // Set operational mode
    uint8_t sendMsgBuf(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf);      // Send message to transmit buffer
    uint8_t sendMsgBuf(uint32_t id, uint8_t len, uint8_t *buf);
    uint8_t readMsgBuf(uint32_t *id, uint8_t *ext, uint8_t *len, uint8_t *buf);
    uint8_t readMsgBuf(uint32_t *id, uint8_t *len, uint8_t *buf);
    uint8_t checkReceive(void);
    uint8_t checkError(void);
    uint8_t getError(void);
    uint8_t errorCountRX(void);
    uint8_t errorCountTX(void);
    uint8_t enOneShotTX(void);
    uint8_t disOneShotTX(void);

    struct mcan_set mcan;
    volatile bool rx_ded_buffer_data;


private:
    uint32_t mcan_msg_ram[MSG_RAM_SIZE] __attribute__((aligned(4)));
    uint32_t mcan_msg_ram_size = ARRAY_SIZE(mcan_msg_ram);
    uint8_t _idmode;
    uint8_t _mode;
    uint8_t _cs;
    uint8_t _canid;
    uint8_t _cantx;
    uint8_t _canrx;
    uint8_t _group;

};


#endif //define _SAMC_CAN_H_
#endif //ifdef ARDUINO_ARCH_SAMD
