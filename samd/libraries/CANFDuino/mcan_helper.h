#ifndef _MCAN_HELPER_
#define _MCAN_HELPER_

/* MCAN-related #defines missing from the SAMC21 peripheral definitions */

/* -------- MCAN Message RAM : Standard Message ID Rx Filter Element -------- */
#define MCAN_RAM_FILT_STD_SIZE (1u) /**< \brief Size of the 11-bit Message ID Rx Filter Element, in words */
#define MCAN_RAM_FILT_SFID2_Pos 0
#define MCAN_RAM_FILT_SFID2_Msk (0x7ffu << MCAN_RAM_FILT_SFID2_Pos) /**< \brief (S0) Standard Filter ID 2 */
#define MCAN_RAM_FILT_SFID2(value) ((MCAN_RAM_FILT_SFID2_Msk & ((value) << MCAN_RAM_FILT_SFID2_Pos)))
#define   MCAN_RAM_FILT_SFID2_BUF_IDX_Pos 0
#define   MCAN_RAM_FILT_SFID2_BUF_IDX_Msk (0x3fu << MCAN_RAM_FILT_SFID2_BUF_IDX_Pos) /**< \brief (S0) Index of Rx Buffer for storage of a matching message. */
#define   MCAN_RAM_FILT_SFID2_BUF_IDX(value) ((MCAN_RAM_FILT_SFID2_BUF_IDX_Msk & ((value) << MCAN_RAM_FILT_SFID2_BUF_IDX_Pos)))
#define   MCAN_RAM_FILT_SFID2_FE0 (0x1u << 6) /**< \brief (S0) Generate a pulse at m_can_fe0 filter event pin in case the filter matches. */
#define   MCAN_RAM_FILT_SFID2_FE1 (0x1u << 7) /**< \brief (S0) Generate a pulse at m_can_fe1 filter event pin in case the filter matches. */
#define   MCAN_RAM_FILT_SFID2_FE2 (0x1u << 8) /**< \brief (S0) Generate a pulse at m_can_fe2 filter event pin in case the filter matches. */
#define   MCAN_RAM_FILT_SFID2_BUF (0x0u << 9) /**< \brief (S0) Store message in a Rx buffer. */
#define   MCAN_RAM_FILT_SFID2_DBG_A (0x1u << 9) /**< \brief (S0) Debug Message A. */
#define   MCAN_RAM_FILT_SFID2_DBG_B (0x2u << 9) /**< \brief (S0) Debug Message B. */
#define   MCAN_RAM_FILT_SFID2_DBG_C (0x3u << 9) /**< \brief (S0) Debug Message C. */
#define MCAN_RAM_FILT_SFID1_Pos 16
#define MCAN_RAM_FILT_SFID1_Msk (0x7ffu << MCAN_RAM_FILT_SFID1_Pos) /**< \brief (S0) Standard Filter ID 1 */
#define MCAN_RAM_FILT_SFID1(value) ((MCAN_RAM_FILT_SFID1_Msk & ((value) << MCAN_RAM_FILT_SFID1_Pos)))
#define MCAN_RAM_FILT_SFEC_Pos 27
#define MCAN_RAM_FILT_SFEC_Msk (0x7u << MCAN_RAM_FILT_SFEC_Pos) /**< \brief (S0) Standard Filter Element Configuration */
#define MCAN_RAM_FILT_SFEC(value) ((MCAN_RAM_FILT_SFEC_Msk & ((value) << MCAN_RAM_FILT_SFEC_Pos)))
#define   MCAN_RAM_FILT_SFEC_DIS (0x0u << 27) /**< \brief (S0) Disable filter element. */
#define   MCAN_RAM_FILT_SFEC_FIFO0 (0x1u << 27) /**< \brief (S0) Store in Rx FIFO 0 if filter matches. */
#define   MCAN_RAM_FILT_SFEC_FIFO1 (0x2u << 27) /**< \brief (S0) Store in Rx FIFO 1 if filter matches. */
#define   MCAN_RAM_FILT_SFEC_INV (0x3u << 27) /**< \brief (S0) Reject ID if filter matches. */
#define   MCAN_RAM_FILT_SFEC_PTY (0x4u << 27) /**< \brief (S0) Set priority if filter matches. */
#define   MCAN_RAM_FILT_SFEC_PTY_FIFO0 (0x5u << 27) /**< \brief (S0) Set priority and store in FIFO 0 if filter matches. */
#define   MCAN_RAM_FILT_SFEC_PTY_FIFO1 (0x6u << 27) /**< \brief (S0) Set priority and store in FIFO 1 if filter matches. */
#define   MCAN_RAM_FILT_SFEC_BUF (0x7u << 27) /**< \brief (S0) Store into Rx Buffer or as debug message. */
#define MCAN_RAM_FILT_SFT_Pos 30
#define MCAN_RAM_FILT_SFT_Msk (0x3u << MCAN_RAM_FILT_SFT_Pos) /**< \brief (S0) Standard Filter Type */
#define MCAN_RAM_FILT_SFT(value) ((MCAN_RAM_FILT_SFT_Msk & ((value) << MCAN_RAM_FILT_SFT_Pos)))
#define   MCAN_RAM_FILT_SFT_RANGE (0x0u << 30) /**< \brief (S0) Range filter from SF1ID to SF2ID. */
#define   MCAN_RAM_FILT_SFT_DUAL_ID (0x1u << 30) /**< \brief (S0) Dual ID filter for SF1ID or SF2ID. */
#define   MCAN_RAM_FILT_SFT_CLASSIC (0x2u << 30) /**< \brief (S0) Classic filter: SF1ID = filter, SF2ID = mask. */
/* -------- MCAN Message RAM : Extended Message ID Rx Filter Element : F0 Word -------- */
#define MCAN_RAM_FILT_EXT_SIZE (2u) /**< \brief Size of the 29-bit Message ID Rx Filter Element, in words */
#define MCAN_RAM_FILT_EFID1_Pos 0
#define MCAN_RAM_FILT_EFID1_Msk (0x1fffffffu << MCAN_RAM_FILT_EFID1_Pos) /**< \brief (F0) Standard Filter ID 1 */
#define MCAN_RAM_FILT_EFID1(value) ((MCAN_RAM_FILT_EFID1_Msk & ((value) << MCAN_RAM_FILT_EFID1_Pos)))
#define MCAN_RAM_FILT_EFEC_Pos 29
#define MCAN_RAM_FILT_EFEC_Msk (0x7u << MCAN_RAM_FILT_EFEC_Pos) /**< \brief (F0) Extended Filter Element Configuration */
#define MCAN_RAM_FILT_EFEC(value) ((MCAN_RAM_FILT_EFEC_Msk & ((value) << MCAN_RAM_FILT_EFEC_Pos)))
#define   MCAN_RAM_FILT_EFEC_DIS (0x0u << 29) /**< \brief (F0) Disable filter element. */
#define   MCAN_RAM_FILT_EFEC_FIFO0 (0x1u << 29) /**< \brief (F0) Store in Rx FIFO 0 if filter matches. */
#define   MCAN_RAM_FILT_EFEC_FIFO1 (0x2u << 29) /**< \brief (F0) Store in Rx FIFO 1 if filter matches. */
#define   MCAN_RAM_FILT_EFEC_INV (0x3u << 29) /**< \brief (F0) Reject ID if filter matches. */
#define   MCAN_RAM_FILT_EFEC_PTY (0x4u << 29) /**< \brief (F0) Set priority if filter matches. */
#define   MCAN_RAM_FILT_EFEC_PTY_FIFO0 (0x5u << 29) /**< \brief (F0) Set priority and store in FIFO 0 if filter matches. */
#define   MCAN_RAM_FILT_EFEC_PTY_FIFO1 (0x6u << 29) /**< \brief (F0) Set priority and store in FIFO 1 if filter matches. */
#define   MCAN_RAM_FILT_EFEC_BUF (0x7u << 29) /**< \brief (F0) Store into Rx Buffer or as debug message. */
/* -------- MCAN Message RAM : Extended Message ID Rx Filter Element : F1 Word -------- */
#define MCAN_RAM_FILT_EFID2_Pos 0
#define MCAN_RAM_FILT_EFID2_Msk (0x1fffffffu << MCAN_RAM_FILT_EFID2_Pos) /**< \brief (F1) Standard Filter ID 2 */
#define MCAN_RAM_FILT_EFID2(value) ((MCAN_RAM_FILT_EFID2_Msk & ((value) << MCAN_RAM_FILT_EFID2_Pos)))
#define   MCAN_RAM_FILT_EFID2_BUF_IDX_Pos 0
#define   MCAN_RAM_FILT_EFID2_BUF_IDX_Msk (0x3fu << MCAN_RAM_FILT_EFID2_BUF_IDX_Pos) /**< \brief (F1) Index of Rx Buffer for storage of a matching message. */
#define   MCAN_RAM_FILT_EFID2_BUF_IDX(value) ((MCAN_RAM_FILT_EFID2_BUF_IDX_Msk & ((value) << MCAN_RAM_FILT_EFID2_BUF_IDX_Pos)))
#define   MCAN_RAM_FILT_EFID2_FE0 (0x1u << 6) /**< \brief (F1) Generate a pulse at m_can_fe0 filter event pin in case the filter matches. */
#define   MCAN_RAM_FILT_EFID2_FE1 (0x1u << 7) /**< \brief (F1) Generate a pulse at m_can_fe1 filter event pin in case the filter matches. */
#define   MCAN_RAM_FILT_EFID2_FE2 (0x1u << 8) /**< \brief (F1) Generate a pulse at m_can_fe2 filter event pin in case the filter matches. */
#define   MCAN_RAM_FILT_EFID2_BUF (0x0u << 9) /**< \brief (F1) Store message in a Rx buffer. */
#define   MCAN_RAM_FILT_EFID2_DBG_A (0x1u << 9) /**< \brief (F1) Debug Message A. */
#define   MCAN_RAM_FILT_EFID2_DBG_B (0x2u << 9) /**< \brief (F1) Debug Message B. */
#define   MCAN_RAM_FILT_EFID2_DBG_C (0x3u << 9) /**< \brief (F1) Debug Message C. */
#define MCAN_RAM_FILT_EFT_Pos 30
#define MCAN_RAM_FILT_EFT_Msk (0x3u << MCAN_RAM_FILT_EFT_Pos) /**< \brief (F1) Extended Filter Type */
#define MCAN_RAM_FILT_EFT(value) ((MCAN_RAM_FILT_EFT_Msk & ((value) << MCAN_RAM_FILT_EFT_Pos)))
#define   MCAN_RAM_FILT_EFT_RANGE_EIDM (0x0u << 30) /**< \brief (F1) Range filter from EF1ID to EF2ID (Extended ID Mask applied). */
#define   MCAN_RAM_FILT_EFT_DUAL_ID (0x1u << 30) /**< \brief (F1) Dual ID filter for EF1ID or EF2ID. */
#define   MCAN_RAM_FILT_EFT_CLASSIC (0x2u << 30) /**< \brief (F1) Classic filter: EF1ID = filter, EF2ID = mask. */
#define   MCAN_RAM_FILT_EFT_RANGE (0x3u << 30) /**< \brief (F1) Range filter from EF1ID to EF2ID, Extended ID Mask not applied. */
/* -------- MCAN Message RAM : Tx/Rx Buffer Element : T0/R0 Heading Word -------- */
#define MCAN_RAM_BUF_HDR_SIZE (2u) /**< \brief Size of the header in Rx and Tx Buffer Elements, in words */
#define MCAN_RAM_BUF_ID_XTD_Pos 0
#define MCAN_RAM_BUF_ID_XTD_Msk (0x1fffffffu << MCAN_RAM_BUF_ID_XTD_Pos) /**< \brief (T0, R0) Extended (29-bit) Message identifier */
#define MCAN_RAM_BUF_ID_XTD(value) ((MCAN_RAM_BUF_ID_XTD_Msk & ((value) << MCAN_RAM_BUF_ID_XTD_Pos)))
#define MCAN_RAM_BUF_ID_STD_Pos 18
#define MCAN_RAM_BUF_ID_STD_Msk (0x7ffu << MCAN_RAM_BUF_ID_STD_Pos) /**< \brief (T0, R0) Standard (11-bit) Message identifier */
#define MCAN_RAM_BUF_ID_STD(value) ((MCAN_RAM_BUF_ID_STD_Msk & ((value) << MCAN_RAM_BUF_ID_STD_Pos)))
#define MCAN_RAM_BUF_RTR (0x1u << 29) /**< \brief (T0, R0) Remote Transmission Request */
#define MCAN_RAM_BUF_XTD (0x1u << 30) /**< \brief (T0, R0) Flag that signals an extended Message identifier */
#define MCAN_RAM_BUF_ESI (0x1u << 31) /**< \brief (T0, R0) Error State Indicator */
/* -------- MCAN Message RAM : Tx/Rx Buffer Element : T1/R1 Heading Word -------- */
#define MCAN_RAM_BUF_RXTS_Pos 0
#define MCAN_RAM_BUF_RXTS_Msk (0xffffu << MCAN_RAM_BUF_RXTS_Pos) /**< \brief (R1) Rx Timestamp */
#define MCAN_RAM_BUF_DLC_Pos 16
#define MCAN_RAM_BUF_DLC_Msk (0xfu << MCAN_RAM_BUF_DLC_Pos) /**< \brief (T1, R1) Data Length Code */
#define MCAN_RAM_BUF_DLC(value) ((MCAN_RAM_BUF_DLC_Msk & ((value) << MCAN_RAM_BUF_DLC_Pos)))
#define MCAN_RAM_BUF_BRS (0x1u << 20) /**< \brief (T1, R1) Flag that signals a frame transmitted with bit rate switching */
#define MCAN_RAM_BUF_FDF (0x1u << 21) /**< \brief (T1, R1) Flag that signals a frame in CAN FD format */
#define MCAN_RAM_BUF_EFC (0x1u << 23) /**< \brief (T1) Event FIFO Control */
#define MCAN_RAM_BUF_FIDX_Pos 24
#define MCAN_RAM_BUF_FIDX_Msk (0x7fu << MCAN_RAM_BUF_FIDX_Pos) /**< \brief (R1) Filter Index */
#define MCAN_RAM_BUF_MM_Pos 24
#define MCAN_RAM_BUF_MM_Msk (0xffu << MCAN_RAM_BUF_MM_Pos) /**< \brief (T1) Message Marker */
#define MCAN_RAM_BUF_MM(value) ((MCAN_RAM_BUF_MM_Msk & ((value) << MCAN_RAM_BUF_MM_Pos)))
#define MCAN_RAM_BUF_ANMF (0x1u << 31) /**< \brief (R1) Flag that signals a received frame accepted without matching any Rx Filter Element */
/* -------- MCAN Message RAM : Tx Event Element -------- */
#define MCAN_RAM_TX_EVT_SIZE (2u) /**< \brief Size of the Tx Event Element, in words */

#endif
