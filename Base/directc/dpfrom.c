/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpfrom.c                                                */
/*                                                                          */
/*  Description:    Contains initialization and data checking functions.    */
/*                                                                          */
/* ************************************************************************ */

#include "dpuser.h"
#include "dpdef.h"
#include "dpalg.h"
#include "dputil.h"
#include "dpcom.h"
#include "dpfrom.h"
#include "dpsecurity.h"
#include "dpjtag.h"

DPCHAR FromRowNumber;
#ifdef FROM_SUPPORT
DPUCHAR ucFRomAddressMask;

/************************************************************************************************/
/*  FROM Action Functions                                                                       */
/************************************************************************************************/
#ifndef DISABLE_FROM_SPECIFIC_ACTIONS
void dp_erase_from_action(void)
{
    device_security_flags |= IS_ERASE_ONLY;
    device_security_flags |= IS_RESTORE_DESIGN;
    dp_erase_from();
    return;
}

void dp_program_from_action(void)
{
    dp_erase_from();
    
    /* Encryption support */
    #ifdef FROM_ENCRYPT
    if (error_code == DPE_SUCCESS)
    {
        if (dat_support_status & FROM_DAT_ENCRYPTION_BIT)
        {
            dp_enc_program_from();
        }
    }
    #endif
    #ifdef FROM_PLAIN
    /* Plain text support */
    if (error_code == DPE_SUCCESS)
    {
        if ((dat_support_status & FROM_DAT_ENCRYPTION_BIT) == 0U)
        {
            dp_program_from();
            if (error_code == DPE_SUCCESS)
            {
                dp_verify_from();
            }
        }
    }
    #endif
    return;
}

void dp_verify_from_action(void)
{
    #ifdef FROM_PLAIN    
    /* Plain text support */
    if ((dat_support_status & FROM_DAT_ENCRYPTION_BIT) == 0U)
    {
        dp_verify_from();
    }
    #endif
    return;
}

/************************************************************************************************/

/************************************************************************************************/
/* Common Functions                                                                             */
/************************************************************************************************/
void dp_erase_from(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nErase FlashROM...");
    #endif
    dp_flush_global_buf1();
    global_buf1[0] = UROW_ERASE_BITS_BYTE0;
    global_buf1[1] = UROW_ERASE_BITS_BYTE1;
    global_buf1[2] = UROW_ERASE_BITS_BYTE2;
    
    /* This is for FROM erase.  Need to get which bits are set to erase from the data file. */
    
    global_uchar = (DPUCHAR) dp_get_bytes(FRomAddressMask_ID,0U,1U);
    if (global_uchar & 0x1U)
    {
        global_buf1[1]|=0x80U;
    }
    global_buf1[2] |= (DPUCHAR)(global_uchar >> 1U);
    
    dp_exe_erase();
    return;
}
#endif

/************************************************************************************************/
/*  FROM Plain Text Programming Functions                                                       */
/************************************************************************************************/
#ifdef FROM_PLAIN
void dp_program_from(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nProgramming FlashROM...");
    #endif
    DataIndex=0U;
    global_uint = 0x80U; /* global_uchar could be used in place if FromAddressMaskIndex */
    
    ucFRomAddressMask = (DPUCHAR) dp_get_bytes(FRomAddressMask_ID,0U,1U);
    /* Since RowNumber could be an 8 bit variable or 16 bit, it will wrap around */
    for (FromRowNumber = 7; FromRowNumber >= 0 ;FromRowNumber--)
    {
        if (ucFRomAddressMask & global_uint)
        {
            global_uchar = (DPUCHAR)FromRowNumber;
            opcode = ISC_UFROM_ADDR_SHIFT;
            IRSCAN_in();
            DRSCAN_in(0u,3u,&global_uchar);
            goto_jtag_state(JTAG_UPDATE_DR,0u);		
            
            opcode = ISC_PROGRAM_UFROM;
            IRSCAN_in();
            dp_get_and_DRSCAN_in(FRomStream_ID, FROM_ROW_BIT_LENGTH, DataIndex);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_PROGRAM_UFROM_CYCLES);
            DataIndex = DataIndex + FROM_ROW_BIT_LENGTH;
            
            dp_poll_device();
            if (error_code != DPE_SUCCESS)
            {
                break;
            }
        }
        global_uint>>=1;
    }
    return;
}

void dp_verify_from(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nVerifying FlashROM...");
    #endif
    dp_vnr();
    
    DataIndex=0U;
    global_uint=0x80U; /* global_uchar could be used in place if FromAddressMaskIndex */
    
    ucFRomAddressMask = (DPUCHAR) dp_get_bytes(FRomAddressMask_ID,0U,1U);
    for (FromRowNumber=7; FromRowNumber >= 0 ;FromRowNumber--)
    {
        if (ucFRomAddressMask & global_uint)
        {
            global_uchar = (DPUCHAR) FromRowNumber;
            opcode = ISC_UFROM_ADDR_SHIFT;
            IRSCAN_in();
            DRSCAN_in(0U,3U,&global_uchar);
            goto_jtag_state(JTAG_UPDATE_DR,0u);
            
            
            opcode = ISC_VERIFY_UFROM;
            IRSCAN_in();
            dp_get_and_DRSCAN_in(FRomStream_ID, FROM_ROW_BIT_LENGTH, DataIndex);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_VERIFY_UFROM_CYCLES);
            dp_delay(ISC_VERIFY_UFROM_DELAY);
            
            dp_poll_device();
            if (error_code == DPE_SUCCESS)
            {
                opcode = ISC_VERIFY_UFROM;
                IRSCAN_in();
                DRSCAN_out(FROM_ROW_BIT_LENGTH,(DPUCHAR*)DPNULL,global_buf1);
                
                if ((global_buf1[0]&0x3U) != 0x3U)
                {
                    error_code = DPE_FROM_VERIFY_ERROR;
                    break;
                }
                DataIndex = DataIndex + FROM_ROW_BIT_LENGTH;
            }
            else 
            {
                FromRowNumber = -1;
            }
        }
        global_uint>>=1;
    }
    return;
}
#endif

#ifdef FROM_ENCRYPT
/*********************** ENCRYPTION **************************/
void dp_enc_program_from(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nProgramming FlashROM...");
    #endif
    if (device_family & DUAL_KEY_BIT)
    {
        global_uchar = 0U;
        dp_set_aes_mode();
    }
    
    DataIndex=0U;
    global_uint = 0x1U; /* global_uint could be used in place if FromAddressMaskIndex */
    
    ucFRomAddressMask = (DPUCHAR) dp_get_bytes(FRomAddressMask_ID,0U,1U);
    
    for (FromRowNumber = 1;FromRowNumber <= 8 ;FromRowNumber++)
    {
        if (ucFRomAddressMask & global_uint)
        {
            dp_init_aes();
            
            opcode = ISC_DESCRAMBLE_UFROM;
            IRSCAN_in();
            dp_get_and_DRSCAN_in(FRomStream_ID, FROM_ROW_BIT_LENGTH, DataIndex);
            
            goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_DESCRAMBLE_UFROM_CYCLES);
            dp_delay(ISC_DESCRAMBLE_UFROM_DELAY);
            
            DataIndex = DataIndex + FROM_ROW_BIT_LENGTH;
            
            opcode = ISC_PROGRAM_ENC_UFROM;
            IRSCAN_in();
            dp_get_and_DRSCAN_in(FRomStream_ID, FROM_ROW_BIT_LENGTH, DataIndex);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_PROGRAM_ENC_UFROM_CYCLES);
            
            DataIndex = DataIndex + FROM_ROW_BIT_LENGTH;
            dp_poll_device();
            if (error_code != DPE_SUCCESS)
            {
                break;
            }
        }
        global_uint<<=1;
    }
    return;
}
#endif
#endif

#ifdef ENABLE_DEBUG
void dp_read_from(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\n\r\nFlashROM Information: ");
    #endif
    dp_vnr();
    
    
    
    for (FromRowNumber=7; FromRowNumber >= 0 ;FromRowNumber--)
    {
        global_uchar = (DPUCHAR) FromRowNumber;
        opcode = ISC_UFROM_ADDR_SHIFT;
        IRSCAN_in();
        DRSCAN_in(0u, 3u, &global_uchar);
        goto_jtag_state(JTAG_UPDATE_DR,0u);
        
        opcode = ISC_READ_UFROM;
        IRSCAN_in();
        DRSCAN_in(0u,FROM_ROW_BIT_LENGTH,(DPUCHAR*)DPNULL);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_READ_UFROM_CYCLES);
        dp_delay(ISC_READ_UFROM_DELAY);
        
        
        DRSCAN_out(FROM_ROW_BIT_LENGTH,(DPUCHAR*)DPNULL,global_buf1);
        dp_display_text("\r\n");
        dp_display_array(global_buf1,FROM_ROW_BYTE_LENGTH,HEX);
    }
    dp_display_text("\r\n==================================================");
    
    return;
}
#endif
