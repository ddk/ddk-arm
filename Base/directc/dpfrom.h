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

#ifndef INC_dpfrom_H
#define INC_dpfrom_H

/****************************************************************************/
/* UFROM Opcodes                                                            */
/****************************************************************************/
#define ISC_UFROM_ADDR_SHIFT	0x9FU
#define ISC_PROGRAM_UFROM	0x9BU
#define ISC_VERIFY_UFROM	0x9CU
#define ISC_READ_UFROM  	0x93U


#define ISC_DESCRAMBLE_UFROM	0xA5U
#define ISC_PROGRAM_ENC_UFROM	0xA6U
#define ISC_VERIFY_ENC_UFROM	0x9DU

/****************************************************************************/
/* UFROM Register Length                                                    */
/****************************************************************************/
#define FROM_ROW_BIT_LENGTH	128U
#define FROM_ROW_BYTE_LENGTH    16U

/****************************************************************************/
/* Function prototypes                                                      */
/****************************************************************************/
void dp_program_from(void);
void dp_verify_from(void);


void dp_enc_program_from(void);
void dp_erase_from_action(void);
void dp_program_from_action(void);
void dp_verify_from_action(void);
void dp_erase_from(void);
#ifdef ENABLE_DEBUG
void dp_read_from(void);
#endif

#endif
