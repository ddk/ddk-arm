/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpsecurity.h                                            */
/*                                                                          */
/*  Description:    Contains the function prototypes.                       */
/*                                                                          */
/* ************************************************************************ */

#ifndef INC_dpsecurity_H
#define INC_dpsecurity_H

/****************************************************************************/
/* Security Opcodes                                                         */
/****************************************************************************/
#define ISC_PROGRAM_UKEY        0x8BU
#define ISC_PROGRAM_DMK         0x91U
#define ISC_VERIFY_DMK          0x0AU
#define AES_INIT                0xDDU
#define ISC_MATCH_UKEY          0x92U
#define ISC_PROGRAM_SECURITY    0xA3U
#define ISC_QUERY_SECURITY      0xA4U
#define AES_MODE                0xACU

/****************************************************************************/
/* Security Bit Locations for AFS devices.                                  */
/****************************************************************************/
#define ULNR0               0x00000001U  /* NVM Block 0 Verify pass key bit */
#define ULNW0               0x00000002U  /* NVM Block 0 write pass key bit */
#define ULNC0               0x00000004U  /* NVM Block 0 programming encryption bit */
#define ULNR1               0x00000008U  /* NVM Block 1 Verify pass key bit */
#define ULNW1               0x00000010U  /* NVM Block 1 write pass key bit */
#define ULNC1               0x00000020U  /* NVM Block 1 programming encryption bit */
#define ULNR2               0x00000040U  /* NVM Block 2 Verify pass key bit */
#define ULNW2               0x00000080U  /* NVM Block 2 write pass key bit */
#define ULNC2               0x00000100U  /* NVM Block 2 programming encryption bit */
#define ULNR3               0x00000200U  /* NVM Block 3 Verify pass key bit */
#define ULNW3               0x00000400U  /* NVM Block 3 write pass key bit */ 
#define ULNC3               0x00000800U  /* NVM Block 3 programming encryption bit */
#define ULARD               0x00001000U  /* Array verification pass key bit */
#define ULAWE               0x00002000U  /* Array erase and programming pass key bit */
#define ULULR               0x00004000U  /* DMK / user security erase and programming protection */
#define ULFLR               0x00008000U  /* Factory key disable bit */
#define ULUFJ               0x00010000U  /* FROM verification pass key bit */
#define ULUFP               0x00020000U  /* FROM erase and programming pass key bit */
#define ULUFE               0x00040000U  /* FROM programming encyption bit */
#define ULUPC               0x00080000U  /* Pass key bit */
#define ULARE               0x00100000U  /* Array programming encryption bit */
#define ULUWE               0x00200000U  /* UROW Erase and programming protection */ 
#define ULOPT0              0x00400000U
#define ULOPT1              0x00800000U
#define SEC_KEY_OK          0x01000000U
#define PERM_LOCK_BIT       0x02000000U
#define IS_ERASE_ONLY       0x04000000U
#define IS_RESTORE_DESIGN   0x08000000U
#define SET_ERASE_SEC       0x10000000U
#define M7_DEVICE           0x20000000U
#define M1_DEVICE           0x40000000U
#define P1_DEVICE           0x80000000U
/****************************************************************************/
/* NVM Register length and parameters                                       */
/****************************************************************************/
#define UKEY_BIT_LENGTH		    128U
#define DMK_BIT_LENGTH		    128U
#define AES_BIT_LENGTH		    128U
#define AES_MODE_BIT_LENGTH         3U
#define ULOCK_A3P_AGL_BIT_LENGTH    44U
#define ULOCK_AFS_BIT_LENGTH	    56U
#define SILGIG_BIT_LENGTH	    32U


/****************************************************************************/
/* Function prototypes                                                      */
/****************************************************************************/
void dp_program_security(void);
void dp_write_sec_key(void);
void dp_write_enc_key(void);
void dp_verify_enc_key(void);
void dp_match_security(void);
void dp_program_ulock(void);
void dp_read_device_security(void);
void dp_program_silsig(void);
void dp_erase_security_action(void);
void dp_erase_security(void);
void dp_program_security_action(void);
void dp_check_dual_key(void);
void dp_verify_id_dmk(void);
void dp_verify_p1_dmk(void);
void dp_verify_m1_dmk(void);
void dp_verify_m7_dmk(void);
void dp_verify_fc_dmk(void);
#endif
