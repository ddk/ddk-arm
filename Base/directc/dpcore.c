/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpcore.c                                                */
/*                                                                          */
/*  Description:    Contains initialization and data checking functions.    */
/*                                                                          */
/* ************************************************************************ */

#include "dpuser.h"
#include "dpdef.h"
#include "dputil.h"
#include "dpalg.h"
#include "dpcom.h"
#include "dpcore.h"
#include "dpfrom.h"
#include "dpsecurity.h"
#include "dpjtag.h"
#include "dpnvm.h"

DPUINT cycle_count;
#ifdef CORE_SUPPORT
DPUCHAR bol_eol_verify;
DPUCHAR SDNumber;
DPINT RowNumber;


/************************************************************************************************/
/*  Core Action Functions                                                                       */
/************************************************************************************************/
#ifndef DISABLE_CORE_SPECIFIC_ACTIONS
void dp_erase_array_action(void)
{
    device_security_flags |= IS_ERASE_ONLY;
    dp_erase_array();
    return;
}

void dp_program_array_action(void)
{
    #ifdef CORE_ENCRYPT
    if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
    {
        dp_enc_data_authentication();
    }
    #endif
    if (error_code == DPE_SUCCESS)
    {
        dp_erase_array();
    }
    /* Array Programming */
    #ifdef CORE_ENCRYPT
    if (error_code == DPE_SUCCESS)
    {
        if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
        {
            dp_enc_program_array();
            if (error_code == DPE_SUCCESS)
            {
                bol_eol_verify = BOL;
                dp_enc_verify_array();
                if (error_code == DPE_SUCCESS)
                {
                    dp_enc_program_rlock();
                    if (error_code == DPE_SUCCESS)
                    {
                        dp_is_core_configured();
                    }
                }
            }
        }
    }
    #endif
    #ifdef CORE_PLAIN
    /* Plain text support */
    if (error_code == DPE_SUCCESS)
    {
        if ((dat_support_status & CORE_DAT_ENCRYPTION_BIT) == 0U)
        {
            dp_program_array();
            if (error_code == DPE_SUCCESS)
            {
                bol_eol_verify = BOL;
                dp_verify_array();
                if (error_code == DPE_SUCCESS)
                {
                    dp_program_rlock();
                    if (error_code == DPE_SUCCESS)
                    {
                        dp_is_core_configured();
                    }
                }
            }
        }
    }
    #endif


#ifdef NVM_SUPPORT
    if (error_code == DPE_SUCCESS)
    {
        if (
        ((device_family & SFS_BIT) == SFS_BIT) &&  
        (dat_support_status | (NVM0_DAT_SUPPORT_BIT | NVM1_DAT_SUPPORT_BIT | NVM2_DAT_SUPPORT_BIT | NVM3_DAT_SUPPORT_BIT))
        )
        {
            if (hardware_interface == IAP_SEL)
            {
                dp_initialize_access_nvm();
            }
        }
        
        if (dat_support_status & NVM0_DAT_SUPPORT_BIT)
        {
            #ifdef NVM_ENCRYPT
            /* Encryption support */
            if (dat_support_status & NVM0_DAT_ENCRYPTION_BIT)
            {
                if (
                    ((device_family & SFS_BIT) == SFS_BIT) && 
                    ( (hardware_interface == GPIO_SEL) || (enable_mss_support) )
                    )
                {
                    dp_enc_program_nvm_block(PRIVATE_CLIENT_PHANTOM_BLOCK);
                }
            }
            #endif
            #ifdef NVM_PLAIN
            /* Plain text support */
            if ((dat_support_status & NVM0_DAT_ENCRYPTION_BIT) == 0U)
            {
                if (
                    ((device_family & SFS_BIT) == SFS_BIT) && 
                    ( (hardware_interface == GPIO_SEL) || (enable_mss_support) )
                    )
                {
                    dp_program_nvm_block(PRIVATE_CLIENT_PHANTOM_BLOCK);
                    if (error_code == DPE_SUCCESS)
                    {
                        dp_verify_nvm_block(PRIVATE_CLIENT_PHANTOM_BLOCK);
                    }
                }
            }
            #endif
        }
    }
    #endif
    return;
}

void dp_verify_array_action(void)
{
    /* Array verification */
    #ifdef CORE_ENCRYPT
    if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
    {
        bol_eol_verify = EOL;
        dp_enc_verify_array();
    }
    #endif
    #ifdef CORE_PLAIN
    /* Plain text support */
    if ((dat_support_status & CORE_DAT_ENCRYPTION_BIT) == 0U)
    {
        bol_eol_verify = EOL;
        dp_verify_array();
    }
    #endif
    if (error_code == DPE_SUCCESS)
    {
        dp_is_core_configured();
    }
    return;
}

#ifdef CORE_ENCRYPT
void dp_enc_data_authentication_action(void)
{
    dp_enc_data_authentication();
    return;
}
#endif
/************************************************************************************************/


/************************************************************************************************/
/* Common Functions                                                                             */
/************************************************************************************************/
void dp_erase_array(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nErase FPGA Array...");
    #endif
    dp_flush_global_buf1();
    global_buf1[0] = UROW_ERASE_BITS_BYTE0 | CORE_ERASE_BITS_BYTE0;
    global_buf1[1] = UROW_ERASE_BITS_BYTE1 | CORE_ERASE_BITS_BYTE1;
    global_buf1[2] = UROW_ERASE_BITS_BYTE2 | CORE_ERASE_BITS_BYTE2;
    
    dp_exe_erase();
    return;
}
#endif
void dp_exe_program(void)
{
    /* PROGRAM  */
    opcode = ISC_PROGRAM;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_PROGRAM_CYCLES);
    dp_poll_device();
    
    return;
}

void dp_exe_verify(void)
{
    /* Verify0 */
    opcode = ISC_VERIFY0;
    IRSCAN_in();
    DRSCAN_in(0u, 2u, &bol_eol_verify);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_VERIFY0_CYCLES);
    dp_delay(ISC_VERIFY0_DELAY);
    
    dp_poll_device();
    if (error_code != DPE_SUCCESS)
    {
        #ifdef ENABLE_DEBUG
        dp_display_text("\r\nVerify 0 failed\r\nRow Number : "); 
        dp_display_value(((DPULONG)RowNumber - 1u), DEC);
        #endif
        error_code = DPE_CORE_VERIFY_ERROR;
    }
    if (error_code == DPE_SUCCESS)
    {
        opcode = ISC_VERIFY0;
        IRSCAN_in();
        
        DRSCAN_out(2u, global_buf1, &global_uchar);
        if ((global_uchar & 0x3U) != 0U)
        {
            #ifdef ENABLE_DEBUG
            dp_display_text("\r\nVerify 0 failed\r\nRow Number : "); 
            dp_display_value((DPULONG)RowNumber - 1U, DEC);
            #endif
            error_code = DPE_CORE_VERIFY_ERROR;
        }
    }
    
    /* Verify1 */
    if (error_code == DPE_SUCCESS)
    {
        opcode = ISC_VERIFY1;
        IRSCAN_in();
        DRSCAN_in(0u, 2u, &bol_eol_verify);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_VERIFY1_CYCLES);
        dp_delay(ISC_VERIFY1_DELAY);
        
        dp_poll_device();
        if (error_code != DPE_SUCCESS)
        {
            #ifdef ENABLE_DEBUG
            dp_display_text("\r\nVerify 1 failed\r\nRow Number : "); 
            dp_display_value((DPULONG)RowNumber - 1U,DEC);
            #endif
            error_code = DPE_CORE_VERIFY_ERROR;
        }
    }
    
    if (error_code == DPE_SUCCESS)
    {
        opcode = ISC_VERIFY1;
        IRSCAN_in();
        DRSCAN_out(2u, global_buf1, &global_uchar);
        if ((global_uchar & 0x3U) != 0U)
        {
            #ifdef ENABLE_DEBUG
            dp_display_text("\r\nVerify 1 failed\r\nRow Number : "); 
            dp_display_value((DPULONG)RowNumber - 1u, DEC);
            #endif
            error_code = DPE_CORE_VERIFY_ERROR;
        }
    }
    return;
}

void dp_reset_address(void)
{
    opcode = ISC_INCREMENT;
    IRSCAN_in();
    global_uchar = 2u;
    DRSCAN_in(0u, 2u, &global_uchar);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_INCREMENT_CYCLES);
    return;
}

void dp_increment_address(void)
{
    opcode = ISC_INCREMENT;
    IRSCAN_in();
    
    global_uchar = 3u;
    DRSCAN_in(0u, 2u, &global_uchar);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_INCREMENT_CYCLES);
    return;
}
#ifdef ENABLE_DAS_SUPPORT
void dp_load_row_address(void)
{
    DPUCHAR Tiles;
    DPUINT TileSize[132] = {
        1U, 48U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U,
        44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 
        44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U,
        44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U,
        44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U,  1U,  1U, 44U, 44U, 44U, 44U,
        44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U,
        44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U,
        44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 
        44U, 44U, 44U, 44U, 44U ,44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U, 44U,
        44U, 44U, 44U,44U, 48U, 1U
    };
    
    DPINT LastSumOfTileRows = 0;
    DPINT SumOfTileRows = 0;
    DPUCHAR Address[24]= { 
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U
    };
    DPUINT IO = 0U;
    DPUCHAR RP = 0U;
    DPUCHAR LH = 0U;
    DPUINT BitsToShift;
    DPUCHAR AddressIndex;
    /* Calculate which tile the row belongs to and what row within the tile we are programming*/
    for(Tiles = 0U; Tiles < 132U; Tiles ++)
    {
        SumOfTileRows += (DPINT) TileSize[Tiles];
        if (RowNumber < SumOfTileRows)
        {
            break;
        }
        else
        {
            LastSumOfTileRows = SumOfTileRows;
        }
    }
    if ((RowNumber - LastSumOfTileRows) >= 38)
    {
        BitsToShift = (TileSize[Tiles] - 1U) - ((DPUINT)RowNumber - (DPUINT)LastSumOfTileRows);
        IO = 0x200u;
        IO >>= BitsToShift;
        RP = (DPUCHAR)(0x20U >> BitsToShift);
        LH = (DPUCHAR)(0x20U >> BitsToShift);
        
        Address[22] |= (DPUCHAR) (IO << 6U);
        Address[23] |= (DPUCHAR) (IO >> 2U);
        Address[22] |= RP;
        Address[21] |= (DPUCHAR) (LH << 2U);
    }
    /* Setting row select bit */
    if ((RowNumber - LastSumOfTileRows) < 38)
    {
        BitsToShift = ((DPUINT)RowNumber - (DPUINT)LastSumOfTileRows) + 4u;
        AddressIndex = (DPUCHAR)(BitsToShift / 8U);
        BitsToShift -= ((DPUINT)AddressIndex * 8U);
        AddressIndex += 16U;
        Address[AddressIndex] |= (1U << BitsToShift);
    }
    /* Setting tile bit */
    AddressIndex = Tiles / 8U;
    BitsToShift = (DPUINT)Tiles - (DPUINT)AddressIndex * 8U;
    Address[AddressIndex] |= (1U << BitsToShift);
    
    /* Shift the address */
    opcode = ISC_ADDRESS_SHIFT;
    IRSCAN_in();
    DRSCAN_in(0u, 192u, Address);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_ADDRESS_SHIFT_CYCLES);
    
    return;
}
#endif

#ifdef CORE_PLAIN
/************************************************************************************************/
/* Plain text array programming functions                                                       */
/************************************************************************************************/
void dp_program_array(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nProgramming FPGA Array...");
    #endif
    dp_reset_address();
    DataIndex = 0U;
    for ( RowNumber = (DPINT)device_rows - 1; RowNumber >= 0; RowNumber-- )
    {
        #ifdef ENABLE_DEBUG
        if ((RowNumber % 100) == 0)
        {
            dp_display_text(".");
        }
        #endif
        #ifdef ENABLE_DAS_SUPPORT
        if ((device_ID & AXXE1500X_ID_MASK) == (AXXE1500X_ID & AXXE1500X_ID_MASK))
        {
            if (device_family & DAS_BIT)
            {
                dp_load_row_address();
            }
        }
        #endif
        dp_load_row_data();
        dp_exe_program();
        if (error_code != DPE_SUCCESS)
        {
            #ifdef ENABLE_DEBUG
            dp_display_text("\r\nFailed to program FPGA Array at row ");
            dp_display_value((DPULONG)RowNumber,DEC);
            #endif
            error_code = DPE_CORE_PROGRAM_ERROR;
            break;
        }
        dp_increment_address();
    }
    return;
}


void dp_verify_array(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nVerifying FPGA Array...");
    #endif
    
    dp_reset_address();
    DataIndex = 0U;
    for ( RowNumber = (DPINT)device_rows - 1; RowNumber >= 0; RowNumber-- )
    {
        #ifdef ENABLE_DEBUG
        if ((RowNumber % 100) == 0)
        {
            dp_display_text(".");
        }
        #endif
        #ifdef ENABLE_DAS_SUPPORT
        if ((device_ID & AXXE1500X_ID_MASK) == (AXXE1500X_ID & AXXE1500X_ID_MASK))
        {
            if (device_family & DAS_BIT)
            {
                dp_load_row_address();
            }
        }
        #endif
        dp_load_row_data();
        dp_exe_verify();
        if (error_code != DPE_SUCCESS)
        {
            break;
        }
        dp_increment_address();
    }
    return;
}


/****************************************************************************/
/*  Address and Data Loading                                                */
/****************************************************************************/
void dp_load_row_data(void)
{
    /* Load one row of FPGA Array data  */
    opcode = ISC_DATA_SHIFT;
    IRSCAN_in();
    
    for ( SDNumber = 1u; SDNumber <= device_SD; SDNumber++ )
    {
        for ( global_ulong = 1u; global_ulong <= 8u; global_ulong++ )
        {
            dp_get_and_DRSCAN_in(datastream_ID, ARRAY_ROW_LENGTH, DataIndex);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_DATA_SHIFT_CYCLES);
            DataIndex = DataIndex + ARRAY_ROW_LENGTH;
        }
    }
    return;
}

void dp_program_rlock(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nProgramming RLOCK...");
    #endif
    
    DataIndex = 0u;
    opcode = ISC_DATA_SHIFT;
    IRSCAN_in();
    
    for ( SDNumber = 1u; SDNumber <= device_SD; SDNumber++ )
    {
        for ( global_ulong = 1u; global_ulong <= 8u; global_ulong++ )
        {
            dp_get_and_DRSCAN_in(rlock_ID, ARRAY_ROW_LENGTH, DataIndex);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_DATA_SHIFT_CYCLES);
            DataIndex = DataIndex + ARRAY_ROW_LENGTH;
        }
    }
    
    opcode = ISC_PROGRAM_RLOCK;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_PROGRAM_RLOCK_CYCLES);
    dp_poll_device();
    if (error_code != DPE_SUCCESS)
    {
        #ifdef ENABLE_DEBUG
        dp_display_text("\r\nFailed to program RLOCK ");
        #endif
        error_code = DPE_PROGRAM_RLOCK_ERROR;
    }
    
    return;
}
#endif


#ifdef CORE_ENCRYPT
/************************************************************************************************/
/* Encryption array programming functions                                                       */
/************************************************************************************************/
void dp_enc_program_array(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nProgramming FPGA Array...");
    #endif
    if (device_family & DUAL_KEY_BIT)
    {
        global_uchar = AES_mode_value;
        dp_set_aes_mode();
    }
    dp_init_aes();
    dp_reset_address();
    
    DataIndex = 0U;
    for ( RowNumber = (DPINT)device_rows - 1; RowNumber >= 0; RowNumber-- )
    {
        #ifdef ENABLE_DEBUG
        if ((RowNumber % 100) == 0)
        {
            dp_display_text(".");
        }
        #endif
        #ifdef ENABLE_DAS_SUPPORT
        if ((device_ID & AXXE1500X_ID_MASK) == (AXXE1500X_ID & AXXE1500X_ID_MASK))
        {
            if (device_family & DAS_BIT)
            {
                dp_load_row_address();
            }
        }
        #endif
        dp_load_enc_row_data();
        dp_exe_program();
        if (error_code != DPE_SUCCESS)
        {
            #ifdef ENABLE_DEBUG
            dp_display_text("\r\nFailed to program FPGA Array at row ");
            dp_display_value((DPULONG)RowNumber, DEC);
            #endif
            error_code = DPE_CORE_PROGRAM_ERROR;
            break;
        }
        dp_increment_address();
    }
    return;
}


void dp_enc_verify_array(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nVerifying FPGA Array...");
    #endif
    if (device_family & DUAL_KEY_BIT)
    {
        global_uchar = AES_mode_value;
        dp_set_aes_mode();
    }
    dp_init_aes();
    dp_reset_address();
    
    DataIndex = 0u;
    for ( RowNumber = (DPINT)device_rows - 1; RowNumber >= 0; RowNumber-- )
    {
        #ifdef ENABLE_DEBUG
        if ((RowNumber % 100) == 0)
        {
            dp_display_text(".");
        }
        #endif
        #ifdef ENABLE_DAS_SUPPORT
        if ((device_ID & AXXE1500X_ID_MASK) == (AXXE1500X_ID & AXXE1500X_ID_MASK))
        {
            if (device_family & DAS_BIT)
            {
                dp_load_row_address();
            }
        }
        #endif
        dp_load_enc_row_data();
        dp_exe_verify();
        if (error_code != DPE_SUCCESS)
        {
            break;
        }
        dp_increment_address();
    }
    return;
}

void dp_load_enc_row_data(void)
{
    opcode = DESCRAMBLE;
    IRSCAN_in();
    
    /* Load one row of FPGA Array data  */
    for ( SDNumber = 1u; SDNumber <= device_SD; SDNumber++ )
    {
        
        for ( global_ulong = 0u; global_ulong <= 1u; global_ulong++ )
        {
            dp_get_and_DRSCAN_in(datastream_ID, AES_BLOCK_LENGTH, DataIndex);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,DESCRAMBLE_CYCLES);
            dp_delay(DESCRAMBLE_DELAY);
            DataIndex = DataIndex + AES_BLOCK_LENGTH;
        }
    }
    return;
}

void dp_enc_program_rlock(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nProgramming Rlock...");
    #endif
    
    DataIndex = 0u;
    opcode = DESCRAMBLE;
    IRSCAN_in();
    
    for ( SDNumber = 1u; SDNumber <= device_SD; SDNumber++ )
    {
        for ( global_ulong = 0u; global_ulong <= 1u; global_ulong++ )
        {
            dp_get_and_DRSCAN_in(rlock_ID, AES_BLOCK_LENGTH, DataIndex);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,DESCRAMBLE_CYCLES);
            dp_delay(DESCRAMBLE_DELAY);
            DataIndex = DataIndex + AES_BLOCK_LENGTH;
        }
    }	
    
    opcode = ISC_PROGRAM_RDLC;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_PROGRAM_RDLC_CYCLES);
    dp_poll_device();
    
    return;
}

void dp_enc_data_authentication(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nPerforming Data Authentication...");
    #endif
    
    if (device_family & DUAL_KEY_BIT)
    {
        global_uchar = AES_mode_value;
        dp_set_aes_mode();
    }
    
    dp_init_aes();
    dp_reset_address();
    
    DataIndex = 0u;
    for ( RowNumber = (DPINT)device_rows - 1 ; RowNumber >= 0; RowNumber-- )
    {
        #ifdef ENABLE_DEBUG
        if ((RowNumber % 100) == 0)
        {
            dp_display_text(".");
        }
        #endif
        
        dp_load_enc_row_data();
        dp_exe_authentication();
        if (error_code != DPE_SUCCESS)
        {
            break;
        }
    }
    return;
}

#endif /* End of CORE_ENCRYPT */

#endif /* End of CORE_SUPPORT */


/* This function maybe needed for eNVM encypted programming */
#if ((defined CORE_ENCRYPT) | (defined NVM_ENCRYPT))
void dp_exe_authentication(void)
{
    opcode = ISC_NOOP;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_NOOP_CYCLES);
    dp_delay(ISC_NOOP_DELAY);
    DRSCAN_out(5u, (DPUCHAR*)DPNULL, &global_uchar);
    
    if (global_uchar & 0x8u)
    {
        error_code = DPE_AUTHENTICATION_FAILURE;
        #if defined (ENABLE_DEBUG) 
        dp_display_text("\r\nFailed to authenticate the encrypted data.\r\n");
        #endif
    }
    return;
}
#endif


