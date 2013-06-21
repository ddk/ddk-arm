/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpfrom.h                                                */
/*                                                                          */
/*  Description:    Contains the function prototypes.                       */
/*                                                                          */
/* ************************************************************************ */

#ifndef INC_dpnvm_H
#define INC_dpnvm_H

#ifdef NVM_SUPPORT
#define PRIVATE_CLIENT_PHANTOM_BLOCK 5u
/****************************************************************************/
/* NVM Opcodes                                                              */
/****************************************************************************/
#define NVM_ADDRESS_SHIFT           0xB8u
#define NVM_DATA_SHIFT              0xB9u
#define NVM_PROGRAM                 0xBAu
#define NVM_READ                    0xB7u
#define ACCESS_NVM                  0xB6u

#define NVM_DATA_SHIFT_ENC          0xCu
#define NVM_PROGRAM_ENC             0xDu

/****************************************************************************/
/* NVM Register length and parameters                                       */
/****************************************************************************/
#define MAX_LOAD_NVM_ADDRESS_POLL   10000u
#define MAX_NVM_READ_POLL           10000u
#define NVM_ADDRESS_LENGTH          35u
#define NVM_DATA_LENGTH             32u
#define ACCESS_NVM_BIT_LENGTH       5u
#define MAX_ATTEMPT_NVM_ACCESS      100u

/****************************************************************************/
/* NVM Calibration Tag Address and data                                     */
/****************************************************************************/
#define NVM_TAG_ADDRESS_BYTE0       0x00u
#define NVM_TAG_ADDRESS_BYTE1       0x10u
#define NVM_TAG_ADDRESS_BYTE2       0x40u
#define NVM_TAG_ADDRESS_BYTE3       0x06u
#define NVM_TAG_ADDRESS_BYTE4       0x01u
#define NVM_TAG_DATA1_BYTE0         0x43u
#define NVM_TAG_DATA1_BYTE1         0x41u
#define NVM_TAG_DATA2_BYTE0         0x43u
#define NVM_TAG_DATA2_BYTE1         0x42u

/****************************************************************************/
/* Function prototypes                                                      */
/****************************************************************************/
void dp_program_nvm_block(DPUCHAR BlockNum);
void dp_verify_nvm_block(DPUCHAR BlockNum);
void dp_enc_program_nvm_block(DPUCHAR BlockNum);
void dp_verify_nvm_action(void);
void dp_verify_nvm_private_clients_action(void);
void dp_program_nvm_action(void);
void dp_program_nvm_private_clients_action(void);
void dp_verify_calibration(void);
void dp_check_device_and_rev(void);
void dp_initialize_access_nvm(void);
void dp_exit_access_nvm(void);
#endif
#endif
