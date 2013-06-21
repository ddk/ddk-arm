/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpuser.h                                                */
/*                                                                          */
/*  Description:    DP user specific file                                   */
/*                  users should define their own functions                 */
/*                                                                          */
/* ************************************************************************ */
#ifndef INC_DPUSER_H
#define INC_DPUSER_H


/*************** Hardware related constants *****************************/
/*
* User Attention: 
* Port 1 - Bit assignments in the hardware JTAG port register 
* 
*/
#define ddk_TCK  (1<<0)
#define ddk_TDI  (1<<8)
#define ddk_TMS  (1<<4)
#define ddk_TRST (1<<9)
#define ddk_TDO  (1<<1)

#define TCK  (1<<0)
#define TDI  (1<<1)
#define TMS  (1<<2)
#define TRST (1<<3)
#define TDO  (1<<4)

/*************** End of hardware related constants ************************/

/* Compiler switches */
//#define ACTEL_DEBUG
//#define ENABLE_DEBUG
//#define ENABLE_FILE_SYSTEM
#define ENABLE_GPIO_SUPPORT

#define CORE_SUPPORT
#define CORE_PLAIN
//#define CORE_ENCRYPT
#define FROM_SUPPORT
#define FROM_PLAIN
//#define FROM_ENCRYPT
#define NVM_SUPPORT
#define NVM_PLAIN
//#define NVM_ENCRYPT
//#define SECURITY_SUPPORT
#define SILSIG_SUPPORT
#define ENABLE_DAS_SUPPORT
/* #define USE_PAGING */
/* #define CHAIN_SUPPORT */
/* Enable BSR_SAMPLE switch maintains the last known state of the IOs regardless 
*  of the data file setting. */
/* #define BSR_SAMPLE */

/* Code optimization specific compile switches */
/* #define ENABLE_CODE_SPACE_OPTIMIZATION */
/* #define DISABLE_CORE_SPECIFIC_ACTIONS */
/* #define DISABLE_FROM_SPECIFIC_ACTIONS */
/* #define DISABLE_NVM_SPECIFIC_ACTIONS */
/* #define DISABLE_SEC_SPECIFIC_ACTIONS */

/* This option could be used for performing progam_nvm_active_array and 
verify_nvm_active_array actions. 
Set FORCE_NVM_ACCESS to 1 to force NVM access */
#define FORCE_NVM_ACCESS 0u

/*************** End of compiler switches ***********************************/


/***********************************************/
/* DPCHAR    -- 8-bit Windows (ANSI) character */
/*              i.e. 8-bit signed integer      */
/* DPINT     -- 16-bit signed integer          */
/* DPLONG    -- 32-bit signed integer          */
/* DPBOOL    -- boolean variable (0 or 1)      */
/* DPUCHAR   -- 8-bit unsigned integer         */
/* DPUSHORT  -- 16-bit unsigned integer        */
/* DPUINT    -- 16-bit unsigned integer        */
/* DPULONG   -- 32-bit unsigned integer        */
/***********************************************/
typedef unsigned char  DPUCHAR;
typedef unsigned short DPUSHORT;
typedef unsigned int   DPUINT;
typedef unsigned long  DPULONG;
typedef unsigned char  DPBOOL;
typedef   signed char  DPCHAR;
typedef   signed int   DPINT;
typedef   signed long  DPLONG;

#define DPNULL ((void*)0)
#ifndef TRUE
#define TRUE 1U
#endif

#ifndef FALSE
#define FALSE 0U
#endif

#define GPIO_SEL 1u
#define IAP_SEL 2u

//extern DPUCHAR *image_buffer;
extern unsigned long image_buffer;
extern DPUCHAR hardware_interface;
extern DPUCHAR enable_mss_support;

DPUCHAR jtag_inp(void);
void jtag_outp(DPUCHAR outdata);
void dp_jtag_init(void);
void dp_jtag_tms(DPUCHAR tms);
void dp_jtag_tms_tdi(DPUCHAR tms, DPUCHAR tdi);
DPUCHAR dp_jtag_tms_tdi_tdo(DPUCHAR tms, DPUCHAR tdi);
void dp_delay(DPULONG microseconds);

int dp_erase_main(void);
int dp_prog_main(void);
int dp_test_main(void);
int dp_force_prog_main(void);


#ifdef ENABLE_DEBUG
#define HEX 0u
#define DEC 1u
#define CHR 2u

/******************************************************************************/
/* users should define their own functions to replace the following functions */
/******************************************************************************/
void dp_display_text(DPCHAR *text);
void dp_display_value(DPULONG value,DPUINT descriptive);
void dp_display_array(DPUCHAR *value,DPUINT bytes, DPUINT descriptive);
#endif

#endif /* INC_DPUSER_H */


