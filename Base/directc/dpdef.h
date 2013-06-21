/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpdef.h                                                 */
/*                                                                          */
/* Description:  This file contains error code definitions                  */
/* and special character definitions                                        */
/*                                                                          */
/* ************************************************************************ */

#ifndef INC_DPDEFINE_H
#define INC_DPDEFINE_H

/************************************************************/
/* Erase Bits Definitions                                   */
/************************************************************/
#define CORE_ERASE_BITS_BYTE0        0x1u   /*Bit 0 */
#define CORE_ERASE_BITS_BYTE1        0x0u   /*Bit 0 */
#define CORE_ERASE_BITS_BYTE2        0x0u   /*Bit 0 */
#define ULOCK_ERASE_BITS_BYTE0       0x2u   /*Bit 1 */
#define ULOCK_ERASE_BITS_BYTE1       0x0u   /*Bit 1 */
#define ULOCK_ERASE_BITS_BYTE2       0x0u   /*Bit 1 */
#define DMK_ERASE_BITS_BYTE0         0x4u   /*Bit 2 */
#define DMK_ERASE_BITS_BYTE1         0x0u   /*Bit 2 */
#define DMK_ERASE_BITS_BYTE2         0x0u   /*Bit 2 */
#define UKEY_ERASE_BITS_BYTE0        0x8u   /*Bit 3 */
#define UKEY_ERASE_BITS_BYTE1        0x0u   /*Bit 3 */
#define UKEY_ERASE_BITS_BYTE2        0x0u   /*Bit 3 */
#define FLOCK_ERASE_BITS_BYTE0      0x10u   /*Bit 4 */
#define FLOCK_ERASE_BITS_BYTE1       0x0u   /*Bit 4 */
#define FLOCK_ERASE_BITS_BYTE2       0x0u   /*Bit 4 */
#define FPRM_ERASE_BITS_BYTE0       0xE0u   /*Bits 5-10*/
#define FPRM_ERASE_BITS_BYTE1        0x7u   /*Bits 5-10*/
#define FPRM_ERASE_BITS_BYTE2        0x0u   /*Bits 5-10*/
#define FKEY_ERASE_BITS_BYTE0        0x0u   /*Bit 11*/
#define FKEY_ERASE_BITS_BYTE1        0x8u   /*Bit 11*/
#define FKEY_ERASE_BITS_BYTE2        0x0u   /*Bit 11*/
#define SECEN_ERASE_BITS_BYTE0       0x0u   /*Bit 12*/
#define SECEN_ERASE_BITS_BYTE1      0x10u   /*Bit 12*/
#define SECEN_ERASE_BITS_BYTE2       0x0u   /*Bit 12*/
#define VIRREF_ERASE_BITS_BYTE0      0x0u   /*Bit 13*/
#define VIRREF_ERASE_BITS_BYTE1     0x20u   /*Bit 13*/
#define VIRREF_ERASE_BITS_BYTE2      0x0u   /*Bit 13*/
#define UROW_ERASE_BITS_BYTE0        0x0u   /*Bit 14*/
#define UROW_ERASE_BITS_BYTE1       0x40u   /*Bit 14*/
#define UROW_ERASE_BITS_BYTE2        0x0u   /*Bit 14*/
#define FROM_ERASE_BITS_BYTE0        0x0u   /*Bit 14*/
#define FROM_ERASE_BITS_BYTE1       0x80u   /*Bit 14*/
#define FORM_ERASE_BITS_BYTE2      0x07Fu   /*Bit 14*/

/************************************************************/
/* Error code definitions                                   */
/************************************************************/
#define DPE_SUCCESS                   0u
#define DPE_IDCODE_ERROR              6u
#define DPE_CHECKSUM_ERROR            9u
#define DPE_INIT_FAILURE             25u
#define DPE_ERASE_ERROR               8u
#define DPE_CORE_PROGRAM_ERROR       10u
#define DPE_CORE_VERIFY_ERROR        11u
#define DPE_PROGRAM_RLOCK_ERROR      10u
#define DPE_POLL_ERROR                7u
#define DPE_PROGRAM_UROW_ERROR       24u
#define DPE_FROM_VERIFY_ERROR        20u
#define DPE_FROM_PROGRAM_ERROR       10u
#define DPE_DMK_VERIFY_ERROR         31u
#define DPE_UROW_ERROR               11u
#define DPE_VERIFY_ERROR             12u
#define DPE_NVM_PROGRAM_ERROR        41u
#define DPE_NVM_VERIFY_ERROR         39u
#define DPE_SEC_PROGRAMMING_ERROR    22u
#define DPE_AES_PROGRAMMING_ERROR    23u
#define DPE_ULOCK_ERROR              16u
#define DPE_MATCH_ERROR              35u
#define DPE_AUTHENTICATION_FAILURE   18u
#define DPE_CORE_ENC_ERROR           33u
#define DPE_CORE_PLAIN_ERROR         37u
#define DPE_FROM_ENC_ERROR           34u
#define DPE_FROM_PLAIN_ERROR         36u
#define DPE_NVM0_ENC_ERROR           47u
#define DPE_NVM0_PLAIN_ERROR         49u
#define DPE_NVM1_ENC_ERROR           47u
#define DPE_NVM1_PLAIN_ERROR         49u
#define DPE_NVM2_ENC_ERROR           47u
#define DPE_NVM2_PLAIN_ERROR         49u
#define DPE_NVM3_ENC_ERROR           47u
#define DPE_NVM3_PLAIN_ERROR         49u
#define DPE_USER_LOCK_SETTING_ERROR  42u
#define DPE_UROW_SETTING_ERROR       43u
#define DPE_AES_SEC_ERROR            31u
#define DPE_SILSIG_PROGRAM_ERROR     14u
#define DPE_PROGRAM_SECURITY_ERROR   38u
#define DPE_NVM_ACCESS_ERROR         54u
#define DPE_CRC_MISMATCH            100u
#define DPE_JTAG_STATE_NOT_HANDLED  110u
#define DPE_ACTION_NOT_FOUND        150u
#define DPE_ACTION_NOT_SUPPORTED    151u
#define DPE_CODE_NOT_ENABLED        152u
#define CALIBRATION_OVERLAP_ERROR   153u
#define DPE_SECURITY_BIT_MISMATCH   154u
#define DPE_DAT_VERSION_MISMATCH    160u
#define DPE_HARDWARE_NOT_SELECTED   170u
#define DPE_DAT_ACCESS_FAILURE      180u
/************************************************************/
/* Block Support status bits                                */
/************************************************************/
#define DAT_SUPPORT_STATUS_OFFSET   39u
#define CORE_DAT_SUPPORT_BIT        0x0001u
#define FROM_DAT_SUPPORT_BIT        0x0002u
#define NVM_DAT_SUPPORT_BIT         0x0004u 
#define NVM0_DAT_SUPPORT_BIT        0x0008u
#define NVM1_DAT_SUPPORT_BIT        0x0010u
#define NVM2_DAT_SUPPORT_BIT        0x0020u
#define NVM3_DAT_SUPPORT_BIT        0x0040u
#define NVM_DAT_VERIFY_SUPPORT_BIT  0x0080u
#define SEC_DAT_SUPPORT_BIT         0x0100u
#define AES_DAT_SUPPORT_BIT         0x0200u
#define CORE_DAT_ENCRYPTION_BIT     0x0400u
#define FROM_DAT_ENCRYPTION_BIT     0x0800u
#define	NVM0_DAT_ENCRYPTION_BIT    0x1000u
#define	NVM1_DAT_ENCRYPTION_BIT    0x2000u
#define	NVM2_DAT_ENCRYPTION_BIT    0x4000u
#define	NVM3_DAT_ENCRYPTION_BIT    0x8000u

/* Core code enable flags */
#if (defined CORE_SUPPORT && defined CORE_PLAIN)
    #define CORE_CODE_PLAIN_SUPPORT_BIT 0x0001u
#else
#define CORE_CODE_PLAIN_SUPPORT_BIT 0x0000u
#endif

#if (defined CORE_SUPPORT && defined CORE_ENCRYPT)
    #define CORE_CODE_ENCRYPTION_BIT 0x0400u
#else
#define CORE_CODE_ENCRYPTION_BIT 0x0000u
#endif

/* FROM code enable flags */
#if (defined FROM_SUPPORT && defined FROM_PLAIN)
    #define FROM_CODE_PLAIN_SUPPORT_BIT 0x0002u
#else
#define FROM_CODE_PLAIN_SUPPORT_BIT 0x0000u
#endif

#if (defined FROM_SUPPORT && defined FROM_ENCRYPT)
    #define FROM_CODE_ENCRYPTION_BIT 0x0800u
#else
#define FROM_CODE_ENCRYPTION_BIT 0x0000u
#endif

/* NVM code enable flags */
#if (defined NVM_SUPPORT && defined NVM_PLAIN)
    #define NVM_CODE_PLAIN_SUPPORT_BITS 0x003Cu
#else
#define NVM_CODE_PLAIN_SUPPORT_BITS 0x0000u
#endif

#if (defined NVM_SUPPORT && defined NVM_ENCRYPT)
    #define NVM_CODE_ENCRYPTION_BITS 0xF000u
#else
#define NVM_CODE_ENCRYPTION_BITS 0x0000u
#endif

/* Security code enable flags */
#ifdef SECURITY_SUPPORT 
#define SEC_CODE_SUPPORT_BIT 0x0100u
#else 
#define SEC_CODE_SUPPORT_BIT 0x0000u
#endif

/************************************************************/
/* Family code definitions                                  */
/************************************************************/
#define AXX_BIT         0x1u
#define AFS_BIT         0x2u
#define SFS_BIT         0x4u

#define CALIBRATION_BIT 0x10u
#define DUAL_KEY_BIT    0x20u
#define DAS_BIT         0x40u

/************************************************************/
/* Data file Tags to indicate M1, M3 or M7 data             */
/************************************************************/
#ifndef ENABLE_CODE_SPACE_OPTIMIZATION
#define M7 7u
#define M1 1u
#define P1 3u
#endif

#if ((!defined ENABLE_CODE_SPACE_OPTIMIZATION) || defined ENABLE_DAS_SUPPORT)
    /************************************************************/
/* Device specific parameters                               */
/************************************************************/
/* Supported A3PE/A3PEL/AGLE     */
#define AXXE600X_ID                 0x023261CFu
#define AXXE600X_ID_MASK            0x03FFFFFFu
#define AXXE600X_SD                 6u
#define AXXE600X_ROWS               3444u
#define AXXE600X_COLS               1184u
#define AXXE600X_BSR_BIT_LENGTH     1056u

#define AXXE1500X_ID                0x0253A1CFu
#define AXXE1500X_ID_MASK           0x03FFFFFFu
#define AXXE1500X_SD                10u
#define AXXE1500X_ROWS              5644u
#define AXXE1500X_COLS              1956u
#define AXXE1500X_BSR_BIT_LENGTH    1740u

#define AXXE3000X_ID                0x0274E1CFu
#define AXXE3000X_ID_MASK           0x03FFFFFFu
#define AXXE3000X_SD                14u
#define AXXE3000X_ROWS              7844u
#define AXXE3000X_COLS              2728u
#define AXXE3000X_BSR_BIT_LENGTH    2424u

/* Supported A3P/A3PL/AGL     */
#define AXX030X_ID                  0x049011CFu
#define AXX030X_ID_MASK             0x07FFFFFFu
#define AXX030X_SD                  2u
#define AXX030X_ROWS                625u
#define AXX030X_COLS                412u
#define AXX030X_BSR_BIT_LENGTH      288u

#define AXX060X_ID                  0x029121CFu
#define AXX060X_ID_MASK             0x06FFFFFFu
#define AXX060X_SD                  2u
#define AXX060X_ROWS                1244u
#define AXX060X_COLS                412u
#define AXX060X_BSR_BIT_LENGTH      372u

#define AXX125X_ID                  0x02A121CFu
#define AXX125X_ID_MASK             0x06FFFFFFu
#define AXX125X_SD                  4u
#define AXX125X_ROWS                1244u
#define AXX125X_COLS                798u
#define AXX125X_BSR_BIT_LENGTH      564u

#define AXX250X_ID                  0x02A141CFu
#define AXX250X_ID_MASK             0x06FFFFFFu
#define AXX250X_SD                  4u
#define AXX250X_ROWS                2300u
#define AXX250X_COLS                798u
#define AXX250X_BSR_BIT_LENGTH      708u

#define AXX400X_ID                  0x02B141CFu
#define AXX400X_ID_MASK             0x06FFFFFFu
#define AXX400X_SD                  6u
#define AXX400X_ROWS                2300u
#define AXX400X_COLS                1184u
#define AXX400X_BSR_BIT_LENGTH      900u

#define	AXX600X_ID                 0x02b261CFu
#define	AXX600X_ID_MASK            0x06FFFFFFu
#define	AXX600X_SD                 6u
#define	AXX600X_ROWS               3444u
#define	AXX600X_COLS               1184u
#define AXX600X_BSR_BIT_LENGTH      1056u

#define AXX1000X_ID                 0x12C281CFu
#define AXX1000X_ID_MASK            0x06FFFFFFu
#define AXX1000X_SD                 8u
#define AXX1000X_ROWS               4500u
#define AXX1000X_COLS               1570u
#define AXX1000X_BSR_BIT_LENGTH     1392u

/* Supported AGLP     */
#define AGLP030X_ID                 0x0E1011CFu
#define AGLP030X_ID_MASK            0x0FFFFFFFu
#define AGLP030X_SD                 2u
#define AGLP030X_ROWS               625u
#define AGLP030X_COLS               412u
#define AGLP030X_BSR_BIT_LENGTH     288u

#define AGLP060X_ID                 0x0E1121CFu
#define AGLP060X_ID_MASK            0x0EFFFFFFu
#define AGLP060X_SD                 2u
#define AGLP060X_ROWS               1244u
#define AGLP060X_COLS               412u
#define AGLP060X_BSR_BIT_LENGTH     372u

#define AGLP125X_ID                 0x0E2121CFu
#define AGLP125X_ID_MASK            0x0EFFFFFFu
#define AGLP125X_SD                 4u
#define AGLP125X_ROWS               1244u
#define AGLP125X_COLS               798u
#define AGLP125X_BSR_BIT_LENGTH     564u

#define AXXN010X_ID                 0x069041CFu
#define AXXN020X_ID                 0x069081CFu
#define AXXN060X_ID                 0x039521CFu
#define AXXN125X_ID                 0x03A521CFu
#define AXXN250X_ID                 0x03A541CFu

/* Supported AFS Devices */
#define AFS090_ID                   0x031921CFu
#define AFS090_ID_MASK              0x0BFFFFFFu
#define AFS090_SD                   3u
#define AFS090_ROWS                 1244u
#define AFS090_COLS                 605u
#define AFS090_BSR_BIT_LENGTH       468u

#define AFS250_ID                   0x032141CFu
#define AFS250_ID_MASK              0x0FFFFFFFu
#define AFS250_SD                   4u
#define AFS250_ROWS                 2300u
#define AFS250_COLS                 798u
#define AFS250_BSR_BIT_LENGTH       708u

#define AFS600_ID                   0x033261CFu
#define AFS600_ID_MASK              0x0FFFFFFFu
#define AFS600_SD                   6u
#define AFS600_ROWS                 3444u
#define AFS600_COLS                 1184u
#define AFS600_BSR_BIT_LENGTH       1056u

#define AFS1500_ID                  0x0353A1CFu
#define AFS1500_ID_MASK             0x0FFFFFFFu
#define AFS1500_SD                  10u
#define AFS1500_ROWS                5644u
#define AFS1500_COLS                1956u
#define AFS1500_BSR_BIT_LENGTH      1740u
#else
#define AFS600_ID                   0x033261CFu
#define AFS1500_ID                  0x0353A1CFu
#endif
#define A2F200_ID                   0x05A131CFu

#ifndef ENABLE_CODE_SPACE_OPTIMIZATION
#define M7KDATA0      0x45u
#define M7KDATA1      0x49u
#define M7KDATA2      0x66u
#define M7KDATA3      0x73u
#define M7KDATA4      0x3Fu
#define M7KDATA5      0x5Fu
#define M7KDATA6      0x01u
#define M7KDATA7      0x26u
#define M7KDATA8      0x11u
#define M7KDATA9      0xE9u
#define M7KDATA10     0xEEu
#define M7KDATA11     0x2Eu
#define M7KDATA12     0x3Au
#define M7KDATA13     0x62u
#define M7KDATA14     0x37u
#define M7KDATA15     0xE1u

#define M1KDATA0      0x77u
#define M1KDATA1      0x50u
#define M1KDATA2      0xE9u
#define M1KDATA3      0x8Fu
#define M1KDATA4      0xB1u
#define M1KDATA5      0x1Eu
#define M1KDATA6      0x29u
#define M1KDATA7      0x3Eu
#define M1KDATA8      0x86u
#define M1KDATA9      0x88u
#define M1KDATA10     0xB4u
#define M1KDATA11     0xCCu
#define M1KDATA12     0x48u
#define M1KDATA13     0x65u
#define M1KDATA14     0xDDu
#define M1KDATA15     0xACu

#define P1KDATA0      0x15u
#define P1KDATA1      0x7du
#define P1KDATA2      0x69u
#define P1KDATA3      0x38u
#define P1KDATA4      0xaeu
#define P1KDATA5      0x09u
#define P1KDATA6      0x5fu
#define P1KDATA7      0x5eu
#define P1KDATA8      0x17u
#define P1KDATA9      0x4eu
#define P1KDATA10     0x5au
#define P1KDATA11     0x37u
#define P1KDATA12     0x14u
#define P1KDATA13     0xe5u
#define P1KDATA14     0xa9u
#define P1KDATA15     0xe7u
#endif

#if ((!defined ENABLE_CODE_SPACE_OPTIMIZATION))
#define FCBYTE0       0x86u
#define FCBYTE1       0x00u
#define FCBYTE2       0x50u
#define FCBYTE3       0x43u
#define FCBYTE4       0x64u
#define FCBYTE5       0x9Cu
#define FCBYTE6       0x52u
#define FCBYTE7       0x40u
#define FCBYTE8       0xC6u
#define FCBYTE9       0x73u
#define FCBYTE10      0xB0u
#define FCBYTE11      0xFBu
#define FCBYTE12      0x75u
#define FCBYTE13      0xE7u
#define FCBYTE14      0xFFu
#define FCBYTE15      0xFDu
#endif



/* General delay and cycle parameters */
#define ISC_ENABLE_CYCLES              3u
#define ISC_ENABLE_DELAY               2157u
#define ISC_DISABLE_DELAY              288u
#define ISC_NOOP_CYCLES                3u
#define ISC_NOOP_DELAY                 264u
#define BYPASS_DELAY                   288u
#define POLL_DELAY                     100u
#define ISC_ERASE_CYCLES               3u
#define ERASE_POLL_DELAY               1000u
#define HIGHZ_CYCLES                   1u                                       
#define DESELECT_ALL_TILES_CYCLES      1u
#define AES_INIT_CYCLES                3u                                      
#define AES_INIT_DELAY                 190u
#define AES_MODE_CYCLES                1u
#define USERCODE_CYCLES                3u                                       


/* ARRAY delay and cycle parameters */
#define ISC_VERIFY0_CYCLES             3u
#define ISC_VERIFY0_DELAY              264u
#define ISC_VERIFY1_CYCLES             3u
#define ISC_VERIFY1_DELAY              264u
#define DESCRAMBLE_CYCLES              3u
#define DESCRAMBLE_DELAY               69u
#define ISC_PROGRAM_CYCLES             3u
#define ISC_INCREMENT_CYCLES           3u
#define ISC_ADDRESS_SHIFT_CYCLES       1u
#define ISC_DATA_SHIFT_CYCLES          3u
#define ISC_PROGRAM_RLOCK_CYCLES       3u
#define ISC_PROGRAM_RDLC_CYCLES        1u
#define READ_FACTORY_DELAY             330u
#define READ_FACTORY_CYCLES            3u
#define FACTORY_ADDRESS_SHIFT_CYCLES   1u                                       

/* FROM delay and cycle parameters */
#define ISC_READ_UFROM_CYCLES          3u
#define ISC_READ_UFROM_DELAY           159u
#define ISC_VERIFY_UFROM_CYCLES        3u
#define ISC_VERIFY_UFROM_DELAY         159u
#define ISC_PROGRAM_UFROM_CYCLES       5u
#define ISC_PROGRAM_UFROM_DELAY        10000u
#define ISC_PROGRAM_ENC_UFROM_CYCLES   5u
#define ISC_UFROM_ADDR_SHIFT_CYCLES    1u
#define ISC_DESCRAMBLE_UFROM_CYCLES    1u
#define ISC_DESCRAMBLE_UFROM_DELAY     69u

/* UROW delay and cycle parameters */
#define ISC_READ_UROW_CYCLES           3u
#define ISC_READ_UROW_DELAY            330u
#define ISC_PROGRAM_UROW_CYCLES        15u

/* NVM delay and cycle parameters */
#define ACCESS_NVM_CYCLES              3u
#define ACCESS_NVM_DELAY               3u
#define ACCESS_NVM_POLL_DELAY          100u
#define NVM_ADDRESS_SHIFT_CYCLES       3u
#define NVM_ADDRESS_SHIFT_DELAY        20u
#define NVM_ADDRESS_SHIFT_POLL_CYCLES  3u
#define NVM_ADDRESS_SHIFT_POLL_DELAY   100u
#define NVM_DATA_SHIFT_CYCLES          3u
#define NVM_DATA_SHIFT_DELAY           6u
#define NVM_PROGRAM_CYCLES             3u
#define NVM_PROGRAM_POLL_DELAY         100u
#define NVM_READ_CYCLES                3u
#define NVM_READ_DELAY                 30u
#define NVM_DATA_SHIFT_ENC_CYCLES      3u
#define NVM_DATA_SHIFT_ENC_DELAY       46u
#define NVM_PROGRAM_ENC_CYCLES         3u
#define NVM_PROGRAM_ENC_DELAY          11535u

/* Security delay and cycle parameters */
#define ISC_PROGRAM_UKEY_CYCLES        3u
#define ISC_PROGRAM_DMK_CYCLES         15u
#define ISC_PROGRAM_SECURITY_CYCLES    3u
#define ISC_QUERY_SECURITY_CYCLES      3u
#define ISC_MATCH_UKEY_CYCLES          3u
#define ISC_MATCH_UKEY_DELAY           1438u
#define ISC_VERIFY_DMK_CYCLES          3u
#define ISC_VERIFY_DMK_DELAY           104u

#endif /* INC_DPDEFINE_H */

