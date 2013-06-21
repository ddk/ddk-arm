/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpalg.c                                                 */
/*                                                                          */
/*  Description:    Contains initialization and data checking functions.    */
/*                                                                          */
/* ************************************************************************ */

#include "dpuser.h"
#include "dpdef.h"
#include "dputil.h"
#include "dpcom.h"
#include "dpalg.h"
#include "dpcore.h"
#include "dpfrom.h"
#include "dpnvm.h"
#include "dpsecurity.h"
#include "dpjtag.h"

#ifdef ENABLE_IAP_SUPPORT
#include "urow_iap.h"
#endif

/*
* General purpose Global variables needed in the program
*/
DPUCHAR global_uchar;    /* Global tmp should be used once and released    */
DPULONG global_ulong;
DPUINT global_uint;
DPUCHAR global_buf1[global_buf_SIZE];    /* General purpose global_buf1fer */
DPUCHAR global_buf2[global_buf_SIZE];    /* global_buffer to hold UROW data */

DPUCHAR Action_code; /* used to hold the action codes as defined in dpalg.h */
DPUCHAR opcode; /* Holds the opcode value of the IR register prior to loading */
DPUCHAR AES_mode_value; /* Holds the value of the AES mode for dual key devices */

DPULONG device_ID;  /* Holds the device ID */
DPUCHAR device_rev; /* Holds the device revision */
/* Device exception is to test for 015 vs 030 devices. 
*  The value is read out from the data file
*/
DPUCHAR device_exception; 
DPUINT device_rows;        /* Device specific number of rows  */
DPUCHAR device_SD;        /* Device specific number of SD tiles */
DPUCHAR device_family = 0U;    /* Read from the data file AFS, or G3 */
DPUINT device_bsr_bit_length; /* Holds the bit length of the BSR register */

/* DataIndex variable is used to keep track of the position of the data 
* loaded in the file 
*/
DPULONG DataIndex;   
/* dat_support_status contains the support and encryption status of the
* different features in the data file. 
*/
DPUINT dat_support_status; 
/* device_security_flags contains the security (pass key and encryption) 
* bit status of the device 
*/
DPULONG device_security_flags; 
/* error_code holds the error code that could be set in the programming 
* functions 
*/
DPUCHAR error_code; 


/****************************************************************************
* Purpose: main entry function                                              
*  This function needs to be called from the main application function with 
*  the approppriate action code set to intiate the desired action.
****************************************************************************/
DPUCHAR dp_top (void)
{
    dp_init_vars();        
    if ((hardware_interface != GPIO_SEL) && (hardware_interface != IAP_SEL))
    {
        #ifdef ENABLE_DEBUG
        dp_display_text("\r\nHardware interface is not selected.  Please assign hardware_interface to either IAP_SEL or GPIO_SEL prior to calling dp_top.");
        #endif
        
        error_code = DPE_HARDWARE_NOT_SELECTED;
    }
    if (hardware_interface == GPIO_SEL)
    {
      #ifndef ENABLE_GPIO_SUPPORT
        #ifdef ENABLE_DEBUG
        dp_display_text("\r\nENABLE_GPIO_SUPPORT compile switch must be enabled to perform extrnal device programming.");
        #endif
        error_code = DPE_HARDWARE_NOT_SELECTED;      
      #endif
    }
    if (hardware_interface == IAP_SEL)
    {
      #ifndef ENABLE_IAP_SUPPORT
        #ifdef ENABLE_DEBUG
        dp_display_text("\r\nENABLE_IAP_SUPPORT compile switch must be enabled to perform IAP programming.");
        #endif
        error_code = DPE_HARDWARE_NOT_SELECTED;      
      #endif
    }
         
    if (error_code == DPE_SUCCESS)
    {
        if (!(
        (Action_code == DP_READ_USERCODE_ACTION_CODE) ||
        (Action_code == DP_IS_CORE_CONFIGURED_ACTION_CODE) ||
        (Action_code == DP_READ_IDCODE_ACTION_CODE)  
        ))
        {
            dp_check_image_crc();
            if (error_code == DPE_SUCCESS)
            {
                dp_check_dat_support_version();
                if (error_code == DPE_SUCCESS)
                {
                    dp_get_dat_support_status();
                }
            }
        }
        goto_jtag_state(JTAG_TEST_LOGIC_RESET,0u);
        dp_perform_action();
    }
    return error_code;
}

/****************************************************************************
* Purpose: Checks the requested action against the compile options of the  
*  DirectC Code and the data file features.  If the data file has support 
*  for a feature that is not supported by the DirectC code because the
*  corresponding compile option is disabled, this function will generate
*  depending on the requested action.                  
****************************************************************************/
#ifndef ENABLE_CODE_SPACE_OPTIMIZATION
void dp_check_action(void)
{
    if ( (Action_code == DP_VERIFY_DEVICE_INFO_ACTION_CODE) || 
    (Action_code == DP_IS_CORE_CONFIGURED_ACTION_CODE) )
    {
    }
    else if ((Action_code == DP_READ_IDCODE_ACTION_CODE) || 
    (Action_code == DP_DEVICE_INFO_ACTION_CODE))
    {
        #ifndef ENABLE_DEBUG
        error_code = DPE_CODE_NOT_ENABLED;
        #endif
    }
    else if (Action_code == DP_ERASE_ACTION_CODE)
    {
        if (((dat_support_status & CORE_DAT_SUPPORT_BIT) == 0U) && 
        ((dat_support_status & FROM_DAT_SUPPORT_BIT) == 0U) && 
        ((dat_support_status & SEC_DAT_SUPPORT_BIT) == 0U))
        {
            error_code = DPE_ACTION_NOT_SUPPORTED;
        }
        #ifdef ENABLE_IAP_SUPPORT        
        if ( (hardware_interface == IAP_SEL) && (device_ID == A2F200_ID) )
        {
            if (dat_support_status & FROM_DAT_SUPPORT_BIT) 
            {
                #ifdef ENABLE_DEBUG
                dp_display_text("\r\nFROM  erase is not supported with IAP on A2F200 devices. Refer to user guide for more information.");
                #endif
                error_code = DPE_ACTION_NOT_SUPPORTED;
            }
        }
        if ( (hardware_interface == IAP_SEL))
        {
            if (dat_support_status & SEC_DAT_SUPPORT_BIT)
            {
                #ifdef ENABLE_DEBUG
                dp_display_text("\r\nSecurity erase is not supported with IAP. Refer to user guide for more information.");
                #endif
                error_code = DPE_ACTION_NOT_SUPPORTED;
            }
        }
        #endif
    }
    else if (Action_code == DP_PROGRAM_ACTION_CODE)
    {
        if (dat_support_status & CORE_DAT_SUPPORT_BIT)
        {
            #ifndef CORE_SUPPORT
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
            if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
            {
                #ifndef CORE_ENCRYPT
                error_code = DPE_CODE_NOT_ENABLED;
                #endif
            }
            /* Checking if core plain is enabled */
            else
            {
                #ifndef CORE_PLAIN
                error_code = DPE_CODE_NOT_ENABLED;
                #endif
            }
        }
        if (dat_support_status & FROM_DAT_SUPPORT_BIT)
        {
            #ifndef FROM_SUPPORT
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
            if (dat_support_status & FROM_DAT_ENCRYPTION_BIT)
            {
                #ifndef FROM_ENCRYPT
                error_code = DPE_CODE_NOT_ENABLED;
                #endif
            }
            else 
            {
                #ifndef FROM_PLAIN
                error_code = DPE_CODE_NOT_ENABLED;
                #endif
            }
            #ifdef ENABLE_IAP_SUPPORT            
            if ( (hardware_interface == IAP_SEL) && (device_ID == A2F200_ID) )
            {
                #ifdef ENABLE_DEBUG     
                dp_display_text("\r\nFROM programming is not supported with IAP on A2F200 devices. Refer to user guide for more information.");
                #endif
                error_code = DPE_ACTION_NOT_SUPPORTED;
            }
            #endif
        }
        if (dat_support_status & SEC_DAT_SUPPORT_BIT)
        {
            #ifndef SECURITY_SUPPORT
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
            #ifdef ENABLE_IAP_SUPPORT
            if ( (hardware_interface == IAP_SEL) )
            {
                #ifdef ENABLE_DEBUG
                dp_display_text("\r\nSecurity programming is not supported with IAP. Refer to user guide for more information.");
                #endif
                error_code = DPE_ACTION_NOT_SUPPORTED;
            }
            #endif
        }
        if ((dat_support_status & 
        (NVM0_DAT_ENCRYPTION_BIT | NVM1_DAT_ENCRYPTION_BIT | 
        NVM2_DAT_ENCRYPTION_BIT | NVM3_DAT_ENCRYPTION_BIT)))
        {
            #ifndef NVM_ENCRYPT
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
            if (((dat_support_status & NVM0_DAT_ENCRYPTION_BIT) == 0U) || 
            ((dat_support_status & NVM1_DAT_ENCRYPTION_BIT) == 0U) || 
            ((dat_support_status & NVM2_DAT_ENCRYPTION_BIT) == 0U) || 
            ((dat_support_status & NVM3_DAT_ENCRYPTION_BIT) == 0U))
            {
                #ifndef NVM_PLAIN
                error_code = DPE_CODE_NOT_ENABLED;
                #endif
            }
            else if (Action_code == DP_VERIFY_NVM_ACTION_CODE)
            {
                error_code = DPE_ACTION_NOT_SUPPORTED;
            }
            else
            {
            }
        }
    }
    else if (Action_code == DP_VERIFY_ACTION_CODE)
    {
        if (((dat_support_status & CORE_DAT_SUPPORT_BIT) == 0U) && 
        (((dat_support_status & FROM_DAT_SUPPORT_BIT) == 0U) || 
        (dat_support_status & FROM_DAT_ENCRYPTION_BIT)) && 
        ((dat_support_status & NVM_DAT_VERIFY_SUPPORT_BIT) == 0U))
        {
            error_code = DPE_ACTION_NOT_SUPPORTED;
        }
        if (dat_support_status & CORE_DAT_SUPPORT_BIT)
        {
            #ifndef CORE_SUPPORT
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
            if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
            {
                #ifndef CORE_ENCRYPT
                error_code = DPE_CODE_NOT_ENABLED;
                #endif
            }
            /* Checking if core plain is enabled */
            else
            {
                #ifndef CORE_PLAIN
                error_code = DPE_CODE_NOT_ENABLED;
                #endif
            }
        }
        if (dat_support_status & FROM_DAT_SUPPORT_BIT)
        {
            #ifndef FROM_SUPPORT
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
            if ((dat_support_status & FROM_DAT_ENCRYPTION_BIT) == 0U)
            {
                #ifndef FROM_PLAIN
                error_code = DPE_CODE_NOT_ENABLED;
                #endif
            }
        }
        if (dat_support_status & NVM_DAT_VERIFY_SUPPORT_BIT)
        {
            #ifndef NVM_SUPPORT
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
            #ifndef NVM_PLAIN
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
        }
    }
    /* Core Code and Data support check */
    else if ((Action_code == DP_ERASE_ARRAY_ACTION_CODE) || 
    (Action_code == DP_PROGRAM_ARRAY_ACTION_CODE) || 
    (Action_code == DP_VERIFY_ARRAY_ACTION_CODE))
    {
        if ((dat_support_status & CORE_DAT_SUPPORT_BIT) == 0U)
        {
            error_code = DPE_ACTION_NOT_SUPPORTED;
        }
        /* Checking if the core support is enabled */
        #ifndef CORE_SUPPORT
        error_code = DPE_CODE_NOT_ENABLED;
        #endif
        if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
        {
            #ifndef CORE_ENCRYPT
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
        }
        /* Checking if core plain is enabled */
        else
        {
            #ifndef CORE_PLAIN
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
        }
    }
    else if (Action_code == DP_ENC_DATA_AUTHENTICATION_ACTION_CODE)
    {
        if ( (dat_support_status & CORE_DAT_ENCRYPTION_BIT) == 0U)
        {
            error_code = DPE_ACTION_NOT_SUPPORTED;
        }
        #ifndef CORE_SUPPORT
        error_code = DPE_CODE_NOT_ENABLED;
        #endif
        #ifndef CORE_ENCRYPT
        error_code = DPE_CODE_NOT_ENABLED;
        #endif
    }
    /* FROM Code and Data support check */
    else if ( (Action_code == DP_ERASE_FROM_ACTION_CODE) || 
    (Action_code == DP_PROGRAM_FROM_ACTION_CODE) || 
    (Action_code == DP_VERIFY_FROM_ACTION_CODE) )
    {
        if ((dat_support_status & FROM_DAT_SUPPORT_BIT) == 0U)
        {
            error_code = DPE_ACTION_NOT_SUPPORTED;
        }
        #ifndef FROM_SUPPORT
        error_code = DPE_CODE_NOT_ENABLED;
        #endif
        if (dat_support_status & FROM_DAT_ENCRYPTION_BIT)
        {
            #ifndef FROM_ENCRYPT
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
            if (Action_code == DP_VERIFY_FROM_ACTION_CODE)
            {
                error_code = DPE_ACTION_NOT_SUPPORTED;
            }
        }
        /* Checking if FROM plain is enabled */
        else
        {
            #ifndef FROM_PLAIN
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
        }
        #ifdef ENABLE_IAP_SUPPORT
        if ( (hardware_interface == IAP_SEL) && (device_ID == A2F200_ID) )
        {
            if ( ( Action_code == DP_ERASE_FROM_ACTION_CODE) || 
            ( Action_code == DP_PROGRAM_FROM_ACTION_CODE) )
            {
                #ifdef ENABLE_DEBUG
                dp_display_text("\r\nFROM programming is not supported with IAP on A2F200 devices. Refer to user guide for more information.");
                #endif
                error_code = DPE_ACTION_NOT_SUPPORTED;
            }
        }
        #endif
    }
    else if ( (Action_code == DP_PROGRAM_NVM_ACTION_CODE) || (Action_code == DP_VERIFY_NVM_ACTION_CODE) || 
        (Action_code == DP_PROGRAM_PRIVATE_CLIENTS_ACTION_CODE) || (Action_code == DP_VERIFY_PRIVATE_CLIENTS_ACTION_CODE))
    {
        if ( (dat_support_status & (NVM0_DAT_SUPPORT_BIT | 
        NVM1_DAT_SUPPORT_BIT | 
        NVM2_DAT_SUPPORT_BIT | 
        NVM3_DAT_SUPPORT_BIT)) == 0U)
        {
            error_code = DPE_ACTION_NOT_SUPPORTED;
        }
        #ifndef NVM_SUPPORT
        error_code = DPE_CODE_NOT_ENABLED;
        #endif
        if ((dat_support_status & 
        (NVM0_DAT_ENCRYPTION_BIT | NVM1_DAT_ENCRYPTION_BIT | 
        NVM2_DAT_ENCRYPTION_BIT | NVM3_DAT_ENCRYPTION_BIT)))
        {
            #ifndef NVM_ENCRYPT
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
        }
        else if ( ( (dat_support_status & NVM0_DAT_ENCRYPTION_BIT) == 0U ) || 
        ( (dat_support_status & NVM1_DAT_ENCRYPTION_BIT) == 0U ) || 
        ( (dat_support_status & NVM2_DAT_ENCRYPTION_BIT) == 0U ) || 
        ( (dat_support_status & NVM3_DAT_ENCRYPTION_BIT) == 0U ) )
        {
            #ifndef NVM_PLAIN
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
        }
        else if (Action_code == DP_VERIFY_NVM_ACTION_CODE)
        {
            error_code = DPE_ACTION_NOT_SUPPORTED;
        }
        else
        {
        }
    }
    else if ( (Action_code == DP_PROGRAM_PRIVATE_CLIENTS_ACTION_CODE) || 
    (Action_code == DP_VERIFY_PRIVATE_CLIENTS_ACTION_CODE) )
    {
        if ( (dat_support_status & NVM0_DAT_SUPPORT_BIT ) == 0U)
        {
            error_code = DPE_ACTION_NOT_SUPPORTED;
        }
        #ifndef NVM_SUPPORT
        error_code = DPE_CODE_NOT_ENABLED;
        #endif
        if (dat_support_status & NVM0_DAT_ENCRYPTION_BIT)
        {
            #ifndef NVM_ENCRYPT
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
        }
        else if ((dat_support_status & NVM0_DAT_ENCRYPTION_BIT) == 0U )      
        {
            #ifndef NVM_PLAIN
            error_code = DPE_CODE_NOT_ENABLED;
            #endif
        }
        else if (Action_code == DP_VERIFY_PRIVATE_CLIENTS_ACTION_CODE)
        {
            error_code = DPE_ACTION_NOT_SUPPORTED;
        }
        else
        {
        }
    }
    else if ( (Action_code == DP_ERASE_SECURITY_ACTION_CODE) || 
    (Action_code == DP_PROGRAM_SECURITY_ACTION_CODE) )
    {
        if ((dat_support_status & SEC_DAT_SUPPORT_BIT) == 0U)
        {
            error_code = DPE_ACTION_NOT_SUPPORTED;
        }
        #ifndef SECURITY_SUPPORT
        error_code = DPE_CODE_NOT_ENABLED;
        #endif
        #ifdef ENABLE_IAP_SUPPORT
        if (hardware_interface == IAP_SEL)
        {
            #ifdef ENABLE_DEBUG
            dp_display_text("\r\nSecurity programming is not supported with IAP. Refer to user guide for more information.");
            #endif
            error_code = DPE_ACTION_NOT_SUPPORTED;
        }
        #endif
    }
    else 
    {
        error_code = DPE_ACTION_NOT_FOUND;
    }
    
    
    #ifdef ENABLE_DEBUG
    if (error_code == DPE_ACTION_NOT_FOUND)
    {
        dp_display_text("\r\nUser Error: Action not found...");
    }
    else if (error_code == DPE_ACTION_NOT_SUPPORTED)
    {
        dp_display_text("\r\nUser Error: Data file does not contain the data needed to support requested action.  Check original STAPL file configuration.");
    }
    else if (error_code == DPE_CODE_NOT_ENABLED)
    {
        dp_display_text("\r\nUser Error: Compiled code does not support the requesed action.  Re-compile the code with the arropriate compile option enabled...");
    }
    else
    {
    }
    #endif
    
    return;
    
}
#else
void dp_check_action(void)
{
    DPUINT dat_status=0u;
    /* Construct code support */
    #ifdef ENABLE_GPIO_SUPPORT            
    const DPUINT code_status = CORE_CODE_PLAIN_SUPPORT_BIT | 
    CORE_CODE_ENCRYPTION_BIT | 
    FROM_CODE_PLAIN_SUPPORT_BIT | 
    FROM_CODE_ENCRYPTION_BIT |
    NVM_CODE_PLAIN_SUPPORT_BITS  | 
    NVM_CODE_ENCRYPTION_BITS |
    SEC_CODE_SUPPORT_BIT;
    #endif
    #ifdef ENABLE_IAP_SUPPORT            
    const DPUINT a2f200_iap_code_status = CORE_CODE_PLAIN_SUPPORT_BIT | 
    CORE_CODE_ENCRYPTION_BIT | 
    NVM_CODE_PLAIN_SUPPORT_BITS  | 
    NVM_CODE_ENCRYPTION_BITS;
    const DPUINT a2f500_iap_code_status = CORE_CODE_PLAIN_SUPPORT_BIT | 
    CORE_CODE_ENCRYPTION_BIT |
    FROM_CODE_PLAIN_SUPPORT_BIT | 
    FROM_CODE_ENCRYPTION_BIT |
    NVM_CODE_PLAIN_SUPPORT_BITS  | 
    NVM_CODE_ENCRYPTION_BITS;
    #endif
    
    
    /* The data file bit mapping is as follows.
    bit 0: CORE support,
    bit 1: FROM support,
    bit 2: NVM support, any NVM block will cause this flag to be set.
    bit 3: NVM0 support,
    bit 4: NVM1 support,
    bit 5: NVM2 support,
    bit 6: NVM3 support,
    bit 7: NVM verify support. It is set if any of the blocks are not encrypted.
    bit 8: Pass key.
    bit 9: AES ket.
    bit 10: CORE Encryption status
    bit 11: FROM Encryption status
    bit 12: NVM0 Encryption status
    bit 13: NVM1 Encryption status
    bit 14: NVM2 Encryption status
    bit 15: NVM3 Encryption status
    
    These bits need to be mapped as follows:
    bit 0: CORE plain status,
    bit 1: FROM plain status,
    bit 2: NVM0 plain status,
    bit 3: NVM1 plain status,
    bit 4: NVM2 plain status,
    bit 5: NVM3 plain status,
    bit 6: Do not care
    bit 7: Do not care
    bit 8: Pass key.
    bit 9: Do not care
    bit 10: CORE Encryption status
    bit 11: FROM Encryption status
    bit 12: NVM0 Encryption status
    bit 13: NVM1 Encryption status
    bit 14: NVM2 Encryption status
    bit 15: NVM3 Encryption status
    
    */
    /* Need to line up the support bits to line up with encrypt status bits so 
    that support bits can be switched to plain support bits instead */
    
    if(hardware_interface == GPIO_SEL)
    {
        dat_status = (dat_support_status & 0x0003u) | 
        ((dat_support_status & 0x0078u) >> 1u);
        /* This statments flips the support bits to plain support bits */
        dat_status = dat_status ^ (dat_support_status >> 10);
        
        /* This statment add the encypt bits for comparisson.  The mask value lines 
        * up with the definitions above*/
        dat_status |= (dat_support_status & 0xFD00u);
        
        /* test for compatiblity */
        #ifdef ENABLE_GPIO_SUPPORT
        if (hardware_interface == GPIO_SEL)
        {
            dat_status = dat_status & (dat_status ^ code_status);
        }
        #endif
        #ifdef ENABLE_IAP_SUPPORT
        if (hardware_interface == IAP_SEL) 
        {
            if (device_ID == A2F200_ID)
            {
                dat_status = dat_status & (dat_status ^ a2f200_iap_code_status);
            }
            else
            {
                dat_status = dat_status & (dat_status ^ a2f500_iap_code_status);
            }
        }
        #endif
        
        if (dat_status)
        {
            #ifdef ENABLE_DEBUG
            #ifdef ENABLE_IAP_SUPPORT
            if (hardware_interface == IAP_SEL) 
            {
                if (device_ID == A2F200_ID)
                {
                    dp_display_text("\r\nUser Error: IAP does not support FROM or security programming on A2F200 devices.  Regenerate data file without FROM and security support.");
                }
                else
                {
                    dp_display_text("\r\nUser Error: IAP does not support security programming on A2F500.  Regenerate data file without security support.");
                }
            }
            #endif
            dp_display_text("\r\nUser Error: Compiled code does not support the requesed action.  Re-compile the code with the arropriate compile option enabled...");
            #endif
            error_code = DPE_CODE_NOT_ENABLED;
        }
    }
    
    return;
}
#endif

void dp_perform_action (void)
{
    DPUCHAR Action_done = FALSE;
    /* Check device ID against the ID in the data file. Assign parameters */
    if (!(
    (Action_code == DP_READ_USERCODE_ACTION_CODE) ||
    (Action_code == DP_IS_CORE_CONFIGURED_ACTION_CODE) ||
    (Action_code == DP_READ_IDCODE_ACTION_CODE)  
    ))
    {
        #ifdef ENABLE_CODE_SPACE_OPTIMIZATION
        dp_check_device_ID();
        #else
        if (dat_version == V85_DAT)
        {
            dp_check_device_ID_V85_DAT();
        }
        else
        {
            dp_check_device_ID();
        }
        #endif
    }
    if (error_code == DPE_SUCCESS)
    {
        if (Action_code == DP_IS_CORE_CONFIGURED_ACTION_CODE)
        {
            dp_is_core_configured();
            Action_done = TRUE;
        }
        /* This action is valid for security and device feature combinations */
        #ifdef ENABLE_DEBUG
        if (Action_code == DP_READ_USERCODE_ACTION_CODE)
        {
            dp_read_usercode_action();
            Action_done = TRUE;
        }
        if (Action_code == DP_READ_IDCODE_ACTION_CODE)
        {
            dp_read_idcode_action();
            Action_done = TRUE;
        }
        #endif
        #ifdef NVM_SUPPORT
        #ifndef DISABLE_NVM_SPECIFIC_ACTIONS
        if (Action_done == FALSE)
        {
            if (
                (Action_code == DP_PROGRAM_NVM_ACTIVE_ARRAY_ACTION_CODE) || 
                (Action_code == DP_VERIFY_NVM_ACTIVE_ARRAY_ACTION_CODE) ||
                (Action_code == DP_PROGRAM_PRIVATE_CLIENTS_ACTIVE_ARRAY_ACTION_CODE) || 
                (Action_code == DP_VERIFY_PRIVATE_CLIENTS_ACTIVE_ARRAY_ACTION_CODE)
                )
            {
                dp_check_device_and_rev();
                if (error_code == DPE_SUCCESS)
                {
                    dp_initialize_access_nvm();
                    if (error_code == DPE_SUCCESS)
                    {
                        if (Action_code == DP_PROGRAM_NVM_ACTIVE_ARRAY_ACTION_CODE)
                        {
                            if (dat_support_status & 
                            (NVM0_DAT_SUPPORT_BIT | NVM1_DAT_SUPPORT_BIT | 
                            NVM2_DAT_SUPPORT_BIT | NVM3_DAT_SUPPORT_BIT))
                            {
                                dp_program_nvm_action();
                            }
                            else
                            {
                                #ifdef ENABLE_DEBUG
                                dp_display_text("\r\nAction not found...");
                                #endif
                                error_code = DPE_ACTION_NOT_FOUND;
                            }
                        }
                        else if (Action_code == DP_VERIFY_NVM_ACTIVE_ARRAY_ACTION_CODE)
                        {
                            #ifdef NVM_PLAIN
                            if (dat_support_status & NVM_DAT_VERIFY_SUPPORT_BIT)
                            {
                                dp_verify_nvm_action();
                            }
                            else 
                            {
                                #ifdef ENABLE_DEBUG
                                dp_display_text("\r\nAction not found...");
                                #endif
                                error_code = DPE_ACTION_NOT_FOUND;
                            }
                            #endif
                        }
                        else if (Action_code == DP_PROGRAM_PRIVATE_CLIENTS_ACTIVE_ARRAY_ACTION_CODE)
                        {
                            if (dat_support_status & 
                            (NVM0_DAT_SUPPORT_BIT | NVM1_DAT_SUPPORT_BIT | 
                            NVM2_DAT_SUPPORT_BIT | NVM3_DAT_SUPPORT_BIT))
                            {
                                dp_program_nvm_private_clients_action();
                            }
                            else
                            {
                                #ifdef ENABLE_DEBUG
                                dp_display_text("\r\nAction not found...");
                                #endif
                                error_code = DPE_ACTION_NOT_FOUND;
                            }
                        }
                        else 
                        {
                            #ifdef NVM_PLAIN
                            if (dat_support_status & NVM_DAT_VERIFY_SUPPORT_BIT)
                            {
                                dp_verify_nvm_private_clients_action();
                            }
                            else 
                            {
                                #ifdef ENABLE_DEBUG
                                dp_display_text("\r\nAction not found...");
                                #endif
                                error_code = DPE_ACTION_NOT_FOUND;
                            }
                            #endif
                        }

                    }
                    dp_exit_access_nvm();
                }
                Action_done = TRUE;
            }
        }
        #endif
        #endif
        /* Enter programming mode and load BSR */
        if (Action_done == FALSE)
        {
            #ifdef NVM_SUPPORT
            if (
            ((device_family & SFS_BIT) == SFS_BIT) &&  
            (dat_support_status | (NVM0_DAT_SUPPORT_BIT | NVM1_DAT_SUPPORT_BIT | NVM2_DAT_SUPPORT_BIT | NVM3_DAT_SUPPORT_BIT)) &&
            (
            (Action_code == DP_PROGRAM_ACTION_CODE) ||
            (Action_code == DP_VERIFY_ACTION_CODE) ||
            (Action_code == DP_PROGRAM_NVM_ACTION_CODE) ||
            (Action_code == DP_VERIFY_NVM_ACTION_CODE) ||
            (Action_code == DP_PROGRAM_PRIVATE_CLIENTS_ACTION_CODE) ||
            (Action_code == DP_VERIFY_PRIVATE_CLIENTS_ACTION_CODE) ||
            (
                ((device_family & SFS_BIT) == SFS_BIT) &&
                ((Action_code == DP_PROGRAM_ARRAY_ACTION_CODE) ||(Action_code == DP_VERIFY_ARRAY_ACTION_CODE))
            )
            )
            )
            {
                if (hardware_interface == GPIO_SEL)
                {
                    dp_initialize_access_nvm();
                }
            }
            #endif
            
            dp_initialize();
            if (error_code == DPE_SUCCESS)
            {
                /* Read the security bit settings of the target device */
                dp_read_device_security();
                
                if ((Action_code != DP_READ_IDCODE_ACTION_CODE) && 
                (Action_code != DP_DEVICE_INFO_ACTION_CODE) && 
                (Action_code != DP_VERIFY_DEVICE_INFO_ACTION_CODE))
                {
                    /* Match security bit settings against the data file */
                    dp_check_security_settings();
                    if (error_code == DPE_SUCCESS)
                    {
                        #ifdef ENABLE_CODE_SPACE_OPTIMIZATION
                        dp_get_dat_dual_key_flag();
                        #else
                        if (dat_version != V85_DAT)
                        {
                            dp_get_dat_dual_key_flag();
                        }
                        #endif
                        #if ((!defined ENABLE_CODE_SPACE_OPTIMIZATION))
                            /* Check if the target device is a dual key device */
                        dp_check_dual_key();
                        if (error_code == DPE_SUCCESS)
                            #endif
                        {
                            #ifndef ENABLE_CODE_SPACE_OPTIMIZATION
                            /* Identify arm enabled devices */
                            dp_verify_id_dmk();
                            if (error_code == DPE_SUCCESS)
                            #endif
                            {
                                /* Check requested action against the data file and compile options of DirectC */
                                dp_check_action();
                                if (error_code == DPE_SUCCESS)
                                {
                                    #ifdef NVM_SUPPORT
                                    /* Check for calibration over lap */
                                    if ((Action_code == DP_PROGRAM_NVM_ACTION_CODE) || (Action_code == DP_PROGRAM_ACTION_CODE) )
                                    {
                                        if (dat_support_status & NVM0_DAT_SUPPORT_BIT)
                                        {
                                            #ifdef NVM_ENCRYPT
                                            if (dat_support_status & NVM0_DAT_ENCRYPTION_BIT)
                                            {
                                                #ifdef ENABLE_DEBUG
                                                dp_display_text("\r\nWarning: If you are programming a calibrated device, please regenerate the analog system configuration file with Libero 8.2 SP1 or greater.");
                                                #endif
                                            }
                                            #endif
                                            #ifdef NVM_PLAIN
                                            if ((dat_support_status & NVM0_DAT_ENCRYPTION_BIT) == 0U)
                                            {
                                                dp_verify_calibration();
                                            }
                                            #endif
                                        }
                                    }
                                    #endif
                                }
                            }
                        }
                    }
                }
                /* Perform the action */
                if (error_code == DPE_SUCCESS)
                {
                    switch (Action_code) 
                    {
                        #ifdef ENABLE_DEBUG
                        case DP_DEVICE_INFO_ACTION_CODE:
                        dp_device_info_action();
                        break;
                        case DP_VERIFY_DEVICE_INFO_ACTION_CODE:
                        dp_verify_device_info_action();
                        break;
                        #endif
                        case DP_ERASE_ACTION_CODE: 
                        dp_erase_action();
                        break;         
                        case DP_PROGRAM_ACTION_CODE: 
                        dp_program_action();
                        break;
                        case DP_VERIFY_ACTION_CODE: 
                        if ( (dat_support_status & CORE_DAT_SUPPORT_BIT) || 
                        ( (dat_support_status & FROM_DAT_SUPPORT_BIT) && 
                        ( (dat_support_status & FROM_DAT_ENCRYPTION_BIT) == 0U)) || 
                        (dat_support_status & NVM_DAT_VERIFY_SUPPORT_BIT))
                        {
                            dp_verify_action();
                        }
                        else 
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;
                        #ifndef DISABLE_CORE_SPECIFIC_ACTIONS
                        #ifdef CORE_SUPPORT
                        case DP_ERASE_ARRAY_ACTION_CODE: 
                        if (dat_support_status & CORE_DAT_SUPPORT_BIT)
                        {
                            dp_erase_array_action();
                        }
                        else
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;         
                        case DP_PROGRAM_ARRAY_ACTION_CODE: 
                        if (dat_support_status & CORE_DAT_SUPPORT_BIT)
                        {
                            dp_program_array_action();
                        }
                        else
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;         
                        case DP_VERIFY_ARRAY_ACTION_CODE: 
                        if (dat_support_status & CORE_DAT_SUPPORT_BIT)
                        {
                            dp_verify_array_action();
                        }
                        else
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;         
                        #ifdef CORE_ENCRYPT
                        case DP_ENC_DATA_AUTHENTICATION_ACTION_CODE: 
                        if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
                        {
                            dp_enc_data_authentication_action();
                        }
                        else
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;         
                        #endif
                        #endif
                        #endif
                        #ifndef DISABLE_FROM_SPECIFIC_ACTIONS
                        #ifdef FROM_SUPPORT
                        case DP_ERASE_FROM_ACTION_CODE: 
                        if (dat_support_status & FROM_DAT_SUPPORT_BIT)
                        {
                            dp_erase_from_action();
                        }
                        else
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;         
                        case DP_PROGRAM_FROM_ACTION_CODE: 
                        if (dat_support_status & FROM_DAT_SUPPORT_BIT)
                        {
                            dp_program_from_action();
                        }
                        else
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;         
                        case DP_VERIFY_FROM_ACTION_CODE: 
                        if ((dat_support_status & FROM_DAT_SUPPORT_BIT) && 
                        ((dat_support_status & FROM_DAT_ENCRYPTION_BIT) == 0U))
                        {
                            dp_verify_from_action();
                        }
                        else
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;         
                        #endif
                        #endif
                        #ifndef DISABLE_NVM_SPECIFIC_ACTIONS
                        #ifdef NVM_SUPPORT
                        case DP_PROGRAM_NVM_ACTION_CODE: 
                        if (dat_support_status & (NVM0_DAT_SUPPORT_BIT | NVM1_DAT_SUPPORT_BIT | NVM2_DAT_SUPPORT_BIT | NVM3_DAT_SUPPORT_BIT))
                        {
                            dp_program_nvm_action();
                        }
                        else
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;         
                        case DP_PROGRAM_PRIVATE_CLIENTS_ACTION_CODE: 
                        if (dat_support_status & NVM0_DAT_SUPPORT_BIT )
                        {
                            dp_program_nvm_private_clients_action();
                        }
                        else
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;         
                        #ifdef NVM_PLAIN
                        case DP_VERIFY_NVM_ACTION_CODE: 
                        if (dat_support_status & NVM_DAT_VERIFY_SUPPORT_BIT)
                        {
                            dp_verify_nvm_action();
                        }
                        else 
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;
                        case DP_VERIFY_PRIVATE_CLIENTS_ACTION_CODE: 
                        if (dat_support_status & NVM_DAT_VERIFY_SUPPORT_BIT)
                        {
                            dp_verify_nvm_private_clients_action();
                        }
                        else 
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;
                        #endif
                        #endif
                        #endif
                        #ifndef DISABLE_SEC_SPECIFIC_ACTIONS
                        #ifdef SECURITY_SUPPORT
                        case DP_ERASE_SECURITY_ACTION_CODE: 
                        if (dat_support_status & SEC_DAT_SUPPORT_BIT)
                        {
                            dp_erase_security_action();
                        }
                        else 
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;         
                        case DP_PROGRAM_SECURITY_ACTION_CODE: 
                        if (dat_support_status & SEC_DAT_SUPPORT_BIT)
                        {
                            dp_program_security_action();
                        }
                        else 
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nAction not found...");
                            #endif
                            error_code = DPE_ACTION_NOT_FOUND;
                        }
                        break;         
                        #endif
                        #endif
                        default:
                        #ifdef ENABLE_DEBUG
                        dp_display_text("\r\nAction not found...");
                        #endif
                        error_code = DPE_ACTION_NOT_FOUND;
                        break;
                    }
                }
                dp_exit();
            }
        }
    }
    return;    
}
/****************************************************************************
* Purpose: Reads the target device ID and checks it against the data file.
*   Assign device specific parameters.
****************************************************************************/
#ifndef ENABLE_CODE_SPACE_OPTIMIZATION
void dp_check_device_ID_V85_DAT(void)
{
    opcode = IDCODE;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    DRSCAN_out(IDCODE_LENGTH, (DPUCHAR*)DPNULL, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    
    device_ID = ((DPULONG) global_buf1 [0]) | (((DPULONG) global_buf1 [1]) <<8) | (((DPULONG) global_buf1 [2]) <<16) | (((DPULONG) global_buf1 [3]) <<24); 
    device_rev = (DPUCHAR) (device_ID >> 28);
    
    /* DataIndex is a variable used for loading the array data but not used now.  
    * Therefore, it can be used to store the Data file ID for */
    DataIndex = dp_get_bytes(Header_ID,ID_OFFSET,4u);
    
    /* Identifying target device and setting its parms */
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nActID = ");
    dp_display_value(device_ID,HEX);
    dp_display_text(" ExpID = ");
    dp_display_value(DataIndex,HEX);
    dp_display_text("\r\nDevice Rev = ");
    dp_display_value(device_rev,HEX);
    #endif
    
    if ( ((device_ID & AXXE600X_ID_MASK) == (AXXE600X_ID & AXXE600X_ID_MASK)) && ((device_ID & AXXE600X_ID_MASK) == (DataIndex & AXXE600X_ID_MASK)) )
    {
        device_ID &= AXXE600X_ID_MASK;
        device_SD = AXXE600X_SD;
        device_rows = AXXE600X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AXXE600X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AXXE1500X_ID_MASK) == (AXXE1500X_ID & AXXE1500X_ID_MASK)) && ((device_ID & AXXE1500X_ID_MASK) == (DataIndex & AXXE1500X_ID_MASK)))
    {
        device_ID &= AXXE1500X_ID_MASK;
        device_SD = AXXE1500X_SD;
        device_rows = AXXE1500X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AXXE1500X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AXXE3000X_ID_MASK) == (AXXE3000X_ID & AXXE3000X_ID_MASK)) && ((device_ID & AXXE3000X_ID_MASK) == (DataIndex & AXXE3000X_ID_MASK)))
    {
        device_ID &= AXXE3000X_ID_MASK;
        device_SD = AXXE3000X_SD;
        device_rows = AXXE3000X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AXXE3000X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AXX030X_ID_MASK) == (AXX030X_ID & AXX030X_ID_MASK)) && ((device_ID & AXX030X_ID_MASK) == (DataIndex & AXX030X_ID_MASK)))
    {
        device_ID &= AXX030X_ID_MASK;
        device_SD = AXX030X_SD;
        device_rows = AXX030X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AXX030X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AXX060X_ID_MASK) == (AXX060X_ID & AXX060X_ID_MASK)) && ((device_ID & AXX060X_ID_MASK) == (DataIndex & AXX060X_ID_MASK)))
    {
        device_ID &= AXX060X_ID_MASK;
        device_SD = AXX060X_SD;
        device_rows = AXX060X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AXX060X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AXX125X_ID_MASK) == (AXX125X_ID & AXX125X_ID_MASK)) && ((device_ID & AXX125X_ID_MASK) == (DataIndex & AXX125X_ID_MASK)))
    {
        device_ID &= AXX125X_ID_MASK;
        device_SD = AXX125X_SD;
        device_rows = AXX125X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AXX125X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AXX250X_ID_MASK) == (AXX250X_ID & AXX250X_ID_MASK)) && ((device_ID & AXX250X_ID_MASK) == (DataIndex & AXX250X_ID_MASK)))
    {
        device_ID &= AXX250X_ID_MASK;
        device_SD = AXX250X_SD;
        device_rows = AXX250X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AXX250X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AXX400X_ID_MASK) == (AXX400X_ID & AXX400X_ID_MASK)) && ((device_ID & AXX400X_ID_MASK) == (DataIndex & AXX400X_ID_MASK)))
    {
        device_ID &= AXX400X_ID_MASK;
        device_SD = AXX400X_SD;
        device_rows = AXX400X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AXX400X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AXX600X_ID_MASK) == (AXX600X_ID & AXX600X_ID_MASK)) && ((device_ID & AXX600X_ID_MASK) == (DataIndex & AXX600X_ID_MASK)))
    {
        device_ID &= AXX600X_ID_MASK;
        device_SD = AXX600X_SD;
        device_rows = AXX600X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AXX600X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AXX1000X_ID_MASK) == (AXX1000X_ID & AXX1000X_ID_MASK)) && ((device_ID & AXX1000X_ID_MASK) == (DataIndex & AXX1000X_ID_MASK)))
    {
        device_ID &= AXX1000X_ID_MASK;
        device_SD = AXX1000X_SD;
        device_rows = AXX1000X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AXX1000X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AGLP030X_ID_MASK) == (AGLP030X_ID & AGLP030X_ID_MASK)) && ((device_ID & AGLP030X_ID_MASK) == (DataIndex & AGLP030X_ID_MASK)))
    {
        device_ID &= AGLP030X_ID_MASK;
        device_SD = AGLP030X_SD;
        device_rows = AGLP030X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AGLP030X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AGLP060X_ID_MASK) == (AGLP060X_ID & AGLP060X_ID_MASK)) && ((device_ID & AGLP060X_ID_MASK) == (DataIndex & AGLP060X_ID_MASK)))
    {
        device_ID &= AGLP060X_ID_MASK;
        device_SD = AGLP060X_SD;
        device_rows = AGLP060X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AGLP060X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AGLP125X_ID_MASK) == (AGLP125X_ID & AGLP125X_ID_MASK)) && ((device_ID & AGLP125X_ID_MASK) == (DataIndex & AGLP125X_ID_MASK)))
    {
        device_ID &= AGLP125X_ID_MASK;
        device_SD = AGLP125X_SD;
        device_rows = AGLP125X_ROWS;
        device_family |= AXX_BIT;
        device_bsr_bit_length = AGLP125X_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AFS090_ID_MASK) == (AFS090_ID & AFS090_ID_MASK)) && ((device_ID & AFS090_ID_MASK) == (DataIndex & AFS090_ID_MASK)))
    {
        device_ID &= AFS090_ID_MASK;
        device_SD = AFS090_SD;
        device_rows = AFS090_ROWS;
        device_family |= AFS_BIT;
        device_bsr_bit_length = AFS090_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AFS250_ID_MASK) == (AFS250_ID & AFS250_ID_MASK)) && ((device_ID & AFS250_ID_MASK) == (DataIndex & AFS250_ID_MASK)))
    {
        device_ID &= AFS250_ID_MASK;
        device_SD = AFS250_SD;
        device_rows = AFS250_ROWS;
        device_family |= AFS_BIT;
        device_bsr_bit_length = AFS250_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AFS600_ID_MASK) == (AFS600_ID & AFS600_ID_MASK) ) && ((device_ID & AFS600_ID_MASK) == (DataIndex & AFS600_ID_MASK)))
    {
        device_ID &= AFS600_ID_MASK;
        device_SD = AFS600_SD;
        device_rows = AFS600_ROWS;
        device_family |= AFS_BIT;
        device_bsr_bit_length = AFS600_BSR_BIT_LENGTH;
    }
    else if (((device_ID & AFS1500_ID_MASK) == (AFS1500_ID & AFS1500_ID_MASK) ) && ((device_ID & AFS1500_ID_MASK) == (DataIndex & AFS1500_ID_MASK)))
    {
        device_ID &= AFS1500_ID_MASK;
        device_SD = AFS1500_SD;
        device_rows = AFS1500_ROWS;
        device_family |= AFS_BIT;
        device_bsr_bit_length = AFS1500_BSR_BIT_LENGTH;
    }
    else
    {
        error_code = DPE_IDCODE_ERROR;
    }
    return;
}
#endif
void dp_check_device_ID(void)
{
    opcode = IDCODE;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    DRSCAN_out(IDCODE_LENGTH, (DPUCHAR*)DPNULL, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    
    device_ID = ((DPULONG) global_buf1 [0]) | (((DPULONG) global_buf1 [1]) <<8) | (((DPULONG) global_buf1 [2]) <<16) | (((DPULONG) global_buf1 [3]) <<24); 
    device_rev = (DPUCHAR) (device_ID >> 28);
    
    
    /* DataIndex is a variable used for loading the array data but not used now.  
    * Therefore, it can be used to store the Data file ID for */
    DataIndex = dp_get_bytes(Header_ID,ID_OFFSET,4U);
    
    global_ulong = dp_get_bytes(Header_ID,ID_MASK_OFFSET,4U);
    
    device_ID &= global_ulong;
    DataIndex &= global_ulong;
    /* Identifying target device and setting its parms */
    
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nActID = ");
    dp_display_value(device_ID,HEX);
    dp_display_text(" ExpID = ");
    dp_display_value(DataIndex,HEX);
    dp_display_text("\r\nDevice Rev = ");
    dp_display_value(device_rev,HEX);
    #endif
    
    if ( device_ID == DataIndex )
    {
        device_SD = (DPUCHAR) dp_get_bytes(Header_ID,SD_TILES_OFFSET,1U);
        device_rows = (DPUINT) dp_get_bytes(Header_ID,MAP_ROWS_OFFSET,2U);
        device_family |= (DPUCHAR) dp_get_bytes(Header_ID,DEVICE_FAMILY_OFFSET,1U);
        device_bsr_bit_length = (DPUINT) dp_get_bytes(Header_ID,BSR_LENGTH_OFFSET,2U);
        device_exception = (DPUCHAR) dp_get_bytes(Header_ID,DEVICE_NAME_OFFSET,1U);
    }
    else
    {
        error_code = DPE_IDCODE_ERROR;
    }
    
}
/****************************************************************************
* Purpose: Perform the test register test to identify if the target device
*   is 015 or 030 device.
****************************************************************************/
#if (!defined ENABLE_CODE_SPACE_OPTIMIZATION)
void dp_test_reg_015_030_check(void)
{
    
    opcode = LDVPROP;
    IRSCAN_in();
    
    DRSCAN_out(LDVPROP_LENGTH, (DPUCHAR*)DPNULL, &global_uchar);
    
    
    /* Check for 030 mistmacth */
    if (global_uchar == 0x20U)
    {
        if (device_exception == AXX015_DEVICE)
        {
            error_code = DPE_IDCODE_ERROR;
        }
    }
    else if (global_uchar == 0x21U)
    {
        if (device_exception == AXX030_DEVICE)
        {
            error_code = DPE_IDCODE_ERROR;
        }
    }
    else if (global_uchar == 0x3FU)
    {
        dp_frow_015_030_check();
    }
    else
    {
    }
    
}
/****************************************************************************
* Purpose: Perform the forw bit test to identify if the target device
*   is 015 or 030 device.
****************************************************************************/
void dp_frow_015_030_check(void)
{
    
    global_uchar = 0x4U;
    dp_read_factory_row();
    
    if ( (global_buf1[6] & 0x80U) == 0x80U )
    {
        if (device_exception == AXX030_DEVICE)
        {
            error_code = DPE_IDCODE_ERROR;
        }
    }
    else 
    {
        if (device_exception == AXX015_DEVICE)
        {
            error_code = DPE_IDCODE_ERROR;
        }
    }
}
#endif

void dp_vnr(void)
{
    opcode = DESELECT_ALL_TILES;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,DESELECT_ALL_TILES_CYCLES);
    return;
}
/****************************************************************************
* Purpose: Loads the BSR regsiter with data specified in the data file.  
*   If BSR_SAMPLE is enabled, the data is not loaded.  Instead, the last known
*   State of the IOs is maintained by stepping through DRCapture JTAG state.
****************************************************************************/
void dp_load_bsr(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nSetting BSR Configurations...");
    #endif
    opcode = ISC_SAMPLE;
    IRSCAN_out(&global_uchar);
    
    #ifdef BSR_SAMPLE
    /* Capturing the last known state of the IOs is only valid if the core
    was programmed.  Otherwise, load the BSR with what is in the data file. */
    if (!(global_uchar & 0x4u))
    {
        dp_get_bytes(BsrPattern_ID,0u,1u);
        if (return_bytes)
        {
            dp_get_and_DRSCAN_in(BsrPattern_ID, device_bsr_bit_length, 0u);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
        }
    }
    else 
    {
        goto_jtag_state(JTAG_CAPTURE_DR,0u);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    }
    #else
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nLoading BSR...");
    #endif
    dp_get_bytes(BsrPattern_ID,0u,1u);
    if (return_bytes)
    {
        dp_get_and_DRSCAN_in(BsrPattern_ID, device_bsr_bit_length, 0u);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    }
    #endif
    
    return;
}
/****************************************************************************
* Purpose: Enter programming mode, load BSR and perform DMK verification 
*  and key match to unlock the device if applicable.
****************************************************************************/
void dp_initialize(void)
{
    /* Execute ISC_ENABLE */
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nInitializing Target Device...");
    #endif
    dp_load_bsr();
    dp_vnr();
    /* Enter programming mode */
    opcode = ISC_ENABLE;
    IRSCAN_in();
    DRSCAN_in(0u, 18u, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_ENABLE_CYCLES);
    
    dp_delay(ISC_ENABLE_DELAY);
    
    DRSCAN_out(18u, (DPUCHAR*)DPNULL, global_buf1);
    
    if ((global_buf1[2] & 0x3U) != 0x3U)
    {
        error_code = DPE_INIT_FAILURE;
    }
    
    /* Display factory information */
    global_uchar = 0u;
    dp_read_factory_row();
    /* FSN bits are bits 55..8 */
    /* Needed to do this to avoid Misra-C compliance issues */
    for(global_uchar = 0u; global_uchar < 7u; global_uchar++)
    {
        global_buf1[global_uchar] = global_buf1[global_uchar+1u];
    }
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nFSN: ");
    dp_display_array(global_buf1, 6u ,HEX);
    #endif    
    
    
    /* Check for 015 vs 030 devices if applicable */
    #if ((!defined ENABLE_CODE_SPACE_OPTIMIZATION))
    if (dat_version != V85_DAT)
    {
        if ((device_exception == AXX015_DEVICE) || (device_exception == AXX030_DEVICE))
        {
            dp_test_reg_015_030_check();
        }
    }
    #endif
    /* Perform kdata verification if applicable. */
    if (error_code == DPE_SUCCESS)
    {
        #if ((defined CORE_ENCRYPT) | (defined FROM_ENCRYPT) | (defined NVM_ENCRYPT))
            if ((Action_code != DP_READ_IDCODE_ACTION_CODE) && (Action_code != DP_DEVICE_INFO_ACTION_CODE) && (Action_code != DP_VERIFY_DEVICE_INFO_ACTION_CODE))
        {
            if (dat_support_status & (CORE_DAT_ENCRYPTION_BIT | FROM_DAT_ENCRYPTION_BIT | NVM0_DAT_ENCRYPTION_BIT | NVM1_DAT_ENCRYPTION_BIT | NVM2_DAT_ENCRYPTION_BIT | NVM3_DAT_ENCRYPTION_BIT))
            {
                global_uchar = 0U;
                /* This is to determine if there is a valid kdata in the data file.
                In the case of Dual key plain programming, the encryption status bits are set 
                in the DirectC code to use the encryption functions.  In this case, kdata 
                verification is called although it does not exit. */
                for (global_uint = 0U;global_uint < 16U; global_uint++)
                {
                    global_uchar |= (DPUCHAR) dp_get_bytes(KDATA_ID,(DPULONG)global_uint,1U);
                }
                if (return_bytes && (global_uchar != 0U))
                {
                    dp_verify_enc_key();
                }
            }
        }
        #endif
        /* Perform key matching if applicable */
        if (error_code == DPE_SUCCESS)
        {                 
            dp_get_bytes(UKEY_ID,0U,1U);
            if (return_bytes)
            {
                dp_match_security();
            }
            /* Perform discrete address shifting check if applicable. */
            #if ( (!defined ENABLE_CODE_SPACE_OPTIMIZATION) || (defined ENABLE_DAS_SUPPORT) )
                #ifdef CORE_SUPPORT
            if ( ((device_ID & AXXE600X_ID_MASK) == (AXXE600X_ID & AXXE600X_ID_MASK)) || 
            ((device_ID & AXXE1500X_ID_MASK) == (AXXE1500X_ID & AXXE1500X_ID_MASK)) || 
            ((device_ID & AXXE3000X_ID_MASK) == (AXXE3000X_ID & AXXE3000X_ID_MASK)) || 
            ((((device_ID & AFS600_ID_MASK) == (AFS600_ID & AFS600_ID_MASK) ) && (device_rev > 1U) )) ||
            ((device_ID & AFS1500_ID_MASK) == (AFS1500_ID & AFS1500_ID_MASK)))
            {                    
                dp_das_check();
            }
            #endif
            #endif
        }
    }
    return;
}
/****************************************************************************
* Purpose: Exit programming mode.
****************************************************************************/
void dp_exit(void)
{
    opcode = ISC_DISABLE;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    dp_delay(ISC_DISABLE_DELAY);
    #ifdef NVM_SUPPORT
    if (
    ((device_family & SFS_BIT) == SFS_BIT) &&  
    (dat_support_status | (NVM0_DAT_SUPPORT_BIT | NVM1_DAT_SUPPORT_BIT | NVM2_DAT_SUPPORT_BIT | NVM3_DAT_SUPPORT_BIT)) &&
    (
    (Action_code == DP_PROGRAM_ACTION_CODE) ||
    (Action_code == DP_VERIFY_ACTION_CODE) ||
    (Action_code == DP_PROGRAM_NVM_ACTION_CODE) ||
    (Action_code == DP_VERIFY_NVM_ACTION_CODE) ||
    (Action_code == DP_PROGRAM_PRIVATE_CLIENTS_ACTION_CODE) ||
    (Action_code == DP_VERIFY_PRIVATE_CLIENTS_ACTION_CODE) ||

    (
    ((device_family & SFS_BIT) == SFS_BIT) &&
    ((Action_code == DP_PROGRAM_ARRAY_ACTION_CODE) ||(Action_code == DP_VERIFY_ARRAY_ACTION_CODE))
    )
    )
    )
    {
        dp_exit_access_nvm();
    }
    #endif
    opcode = BYPASS;
    IRSCAN_out(&global_uchar);
    /* This is needed to avoid a potential glitch while exiting programming mode. */
    if (!(global_uchar & 0x4U))
    {
        opcode = HIGHZ;
        IRSCAN_in();
        goto_jtag_state(JTAG_RUN_TEST_IDLE,HIGHZ_CYCLES);
        opcode = BYPASS;
        IRSCAN_in();
    }
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    dp_delay(BYPASS_DELAY);
    goto_jtag_state(JTAG_TEST_LOGIC_RESET,0u);

    return;
}
#ifdef ENABLE_DEBUG
void dp_read_usercode_action(void)
{
    dp_read_silsig();
    return;
}
#endif

void dp_erase_action(void)
{
    #ifdef SECURITY_SUPPORT
    if (dat_support_status & SEC_DAT_SUPPORT_BIT)
    {
        device_security_flags |= SET_ERASE_SEC;
    }
    #endif
    device_security_flags |= IS_ERASE_ONLY;
    dp_erase();
    return;
}

void dp_program_action(void)
{
    #ifdef CORE_SUPPORT
    #ifdef CORE_ENCRYPT
    if (dat_support_status & CORE_DAT_SUPPORT_BIT)
    {
        if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
        {
            dp_enc_data_authentication();
        }
    }
    #endif
    #endif
    if (error_code == DPE_SUCCESS)
    {
        dp_erase();
    }
    #ifdef CORE_SUPPORT
    /* Array Programming */
    if (error_code == DPE_SUCCESS)
    {
        if (dat_support_status & CORE_DAT_SUPPORT_BIT)
        {
            #ifdef CORE_ENCRYPT
            /* Encryption support */
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
            #endif
            #ifdef CORE_PLAIN
            /* Plain text support */
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
            #endif
        }
    }
    #endif
    #ifdef FROM_SUPPORT
    /* From Programming */
    if (error_code == DPE_SUCCESS)
    {
        if (dat_support_status & FROM_DAT_SUPPORT_BIT)
        {
            #ifdef FROM_ENCRYPT
            /* Encryption support */
            if (dat_support_status & FROM_DAT_ENCRYPTION_BIT)
            {
                dp_enc_program_from();
            }
            #endif
            /* Plain text support */
            #ifdef FROM_PLAIN
            if ((dat_support_status & FROM_DAT_ENCRYPTION_BIT) == 0U)
            {
                dp_program_from();
                if (error_code == DPE_SUCCESS)
                {
                    dp_verify_from();
                }
            }
            #endif
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
                if (error_code == DPE_SUCCESS)
                {
                    dp_enc_program_nvm_block(0U);
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
                if (error_code == DPE_SUCCESS)
                {
                    dp_program_nvm_block(0U);
                    if (error_code == DPE_SUCCESS)
                    {
                        dp_verify_nvm_block(0u);
                    }
                }
            }
            #endif
        }
    }
    if ((device_family & SFS_BIT) != SFS_BIT)
    {
        if (error_code == DPE_SUCCESS)
        {
            if (dat_support_status & NVM1_DAT_SUPPORT_BIT)
            {
                #ifdef NVM_ENCRYPT
                /* Encryption support */
                if (dat_support_status & NVM1_DAT_ENCRYPTION_BIT)
                {
                    dp_enc_program_nvm_block(1U);
                }
                #endif
                #ifdef NVM_PLAIN
                /* Plain text support */
                if ((dat_support_status & NVM1_DAT_ENCRYPTION_BIT) == 0U)
                {
                    dp_program_nvm_block(1U);
                    if (error_code == DPE_SUCCESS)
                    {
                        dp_verify_nvm_block(1u);
                    }
                }
                #endif
            }
        }
        if (error_code == DPE_SUCCESS)
        {
            if (dat_support_status & NVM2_DAT_SUPPORT_BIT)
            {
                #ifdef NVM_ENCRYPT
                /* Encryption support */
                if (dat_support_status & NVM2_DAT_ENCRYPTION_BIT)
                {
                    dp_enc_program_nvm_block(2U);
                }
                #endif
                #ifdef NVM_PLAIN
                /* Plain text support */
                if ((dat_support_status & NVM2_DAT_ENCRYPTION_BIT) == 0U)
                {
                    dp_program_nvm_block(2U);
                    if (error_code == DPE_SUCCESS)
                    {
                        dp_verify_nvm_block(2u);
                    }
                }
                #endif
            }
        }
        if (error_code == DPE_SUCCESS)
        {
            if (dat_support_status & NVM3_DAT_SUPPORT_BIT)
            {
                #ifdef NVM_ENCRYPT
                /* Encryption support */
                if (dat_support_status & NVM3_DAT_ENCRYPTION_BIT)
                {
                    dp_enc_program_nvm_block(3U);
                }
                #endif
                #ifdef NVM_PLAIN
                /* Plain text support */
                if ((dat_support_status & NVM3_DAT_ENCRYPTION_BIT) == 0U)
                {
                    dp_program_nvm_block(3U);
                    if (error_code == DPE_SUCCESS)
                    {
                        dp_verify_nvm_block(3u);
                    }
                }
                #endif
            }
        }
    }
    #endif
    /* If the security is disabled, we need to perform SISLIG programming regardless of the data settings*/
    if (error_code == DPE_SUCCESS)
    {
        #ifdef SECURITY_SUPPORT
        if (dat_support_status & SEC_DAT_SUPPORT_BIT)
        {
            dp_program_security();
        }
        else
        #endif
        {
            #ifdef SILSIG_SUPPORT
            dp_program_silsig();
            #endif
        }
    }
    return;
}

void dp_verify_action(void)
{
    #ifdef CORE_SUPPORT    
    /* Array verification */
    if (dat_support_status & CORE_DAT_SUPPORT_BIT)
    {
        #ifdef CORE_ENCRYPT
        /* Encryption support */
        if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
        {
            bol_eol_verify = EOL;
            dp_enc_verify_array();
        }
        #endif
        #ifdef CORE_PLAIN
        /* Plain text support */
        if ((dat_support_status & CORE_DAT_ENCRYPTION_BIT) == 0U )
        {
            bol_eol_verify = EOL;
            dp_verify_array();
        }
        #endif
        if (error_code == DPE_SUCCESS)
        {
            dp_is_core_configured();
        }
    }
    #endif
    #ifdef FROM_SUPPORT
    /* From verification */
    if (error_code == DPE_SUCCESS)
    {
        if (dat_support_status & FROM_DAT_SUPPORT_BIT)
        {
            /* Plain text support */
            #ifdef FROM_PLAIN
            if ((dat_support_status & FROM_DAT_ENCRYPTION_BIT) == 0U)
            {
                dp_verify_from();
            }
            #endif
        }
    }
    #endif
    #ifdef NVM_SUPPORT
    #ifdef NVM_PLAIN
    if (((device_family & SFS_BIT) == SFS_BIT) && (dat_support_status | NVM_DAT_VERIFY_SUPPORT_BIT))
    {
        if (hardware_interface == IAP_SEL)
        {
            dp_initialize_access_nvm();
        }
    }
    
    if (error_code == DPE_SUCCESS)
    {
        if (dat_support_status & NVM0_DAT_SUPPORT_BIT)
        {
            /* Plain support */
            if ((dat_support_status & NVM0_DAT_ENCRYPTION_BIT)  == 0U)
            {
                if (
                    ((device_family & SFS_BIT) == SFS_BIT) && 
                    ( (hardware_interface == GPIO_SEL) )
                    )
                {
                    dp_verify_nvm_block(PRIVATE_CLIENT_PHANTOM_BLOCK);
                }
                if (error_code == DPE_SUCCESS)
                {
                    dp_verify_nvm_block(0U);
                }
            }
        }
    }
    if ((device_family & SFS_BIT) != SFS_BIT)
    {
        if (error_code == DPE_SUCCESS)
        {
            if (dat_support_status & NVM1_DAT_SUPPORT_BIT)
            {
                /* Plain support */
                if ((dat_support_status & NVM1_DAT_ENCRYPTION_BIT)  == 0U)
                {
                    dp_verify_nvm_block(1U);
                }
            }
        }
        if (error_code == DPE_SUCCESS)
        {
            if (dat_support_status & NVM2_DAT_SUPPORT_BIT)
            {
                /* Plain support */
                if ((dat_support_status & NVM2_DAT_ENCRYPTION_BIT)  == 0U)
                {
                    dp_verify_nvm_block(2U);
                }
            }
        }
        if (error_code == DPE_SUCCESS)
        {
            if (dat_support_status & NVM3_DAT_SUPPORT_BIT)
            {
                /* Plain support */
                if ((dat_support_status & NVM3_DAT_ENCRYPTION_BIT)  == 0U)
                {
                    dp_verify_nvm_block(3U);
                }
            }
        }
    }
    #endif
    #endif
    return;
}

#ifdef ENABLE_DEBUG
void dp_verify_device_info_action(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nVerifying device info...");
    #endif
    
    global_uchar = 0U;
    /* This is to determine if there is a valid kdata in the data file.
    In the case of Dual key plain programming, the encryption status bits are set 
    in the DirectC code to use the encryption functions.  In this case, kdata 
    verification is called although it does not exit. */
    for (global_uint = 0U;global_uint < 16U; global_uint++)
    {
        global_uchar |= (DPUCHAR) dp_get_bytes(KDATA_ID,(DPULONG)global_uint,1U);
        if (return_bytes == 0u)
        {
            break;
        }
    }
    if (return_bytes && (global_uchar != 0U))
    {
        dp_verify_enc_key();
    }

    if (error_code == DPE_SUCCESS)
    {
        /* Checking security bit settings against what is in the Data file */
        if (device_family & (AFS_BIT | SFS_BIT))
        {
            global_ulong = dp_get_bytes(ULOCK_ID,0u,3u);
        }
        else 
        {
            global_ulong = dp_get_bytes(ULOCK_ID,0u,2u);
            global_ulong <<= 12u;
        }
        if (global_ulong)
        {   
            global_ulong |= ULFLR;
        }
        if ((device_security_flags & 0x003FFFFFu) != (global_ulong & 0x003FFFFFu))
        {
            #ifdef ENABLE_DEBUG
            dp_display_text("\r\nUser LOCK Setting Error...Security");
            #endif
            error_code = DPE_USER_LOCK_SETTING_ERROR;
        }
        
        if (error_code == DPE_SUCCESS)
        {
            dp_read_urow();
            #ifdef ENABLE_IAP_SUPPORT
            if (hardware_interface == IAP_SEL)
            {
                read_urow_nvm();
            }
            #endif        
            /* Checksum verification -- Bit 112 - 127 */
            #ifdef ENABLE_GPIO_SUPPORT                                
            if (hardware_interface == GPIO_SEL)
            {
                global_uint =  (DPUINT)global_buf2[14] | ((DPUINT)global_buf2[15] << 8u);
            }
            #endif
            #ifdef ENABLE_IAP_SUPPORT                                
            if (hardware_interface == IAP_SEL)
            {
                global_uint =  (DPUINT)actual_iap_urow[14] | ((DPUINT)actual_iap_urow[15] << 8u);
            }
            #endif
            
            global_ulong = (DPULONG) dp_get_bytes(CHECKSUM_ID, 0u, 2u);
            
            if ((global_ulong & 0xFFFFu) != global_uint)
            {
                #ifdef ENABLE_DEBUG
                dp_display_text("\r\nUROW Setting Error...Checksum");
                #endif
                error_code = DPE_UROW_SETTING_ERROR;
            }
            if (error_code == DPE_SUCCESS)
            {
                /* Design Name verification - Bits 32 to 101*/
                for (global_uint=4u; global_uint < 13u; global_uint++)
                {
                    global_uchar = (DPUCHAR) dp_get_bytes(ACT_UROW_DESIGN_NAME_ID,global_uint - 4u, 1u);
                    if (global_uint == 12u)
                    {
                        global_buf2[global_uint] &= 0x3Fu;
                        #ifdef ENABLE_IAP_SUPPORT
                        if (hardware_interface == IAP_SEL)
                        {
                            actual_iap_urow[global_uint] &= 0x3Fu;
                        }
                        #endif
                    }
                    #ifdef ENABLE_GPIO_SUPPORT                                
                    if (hardware_interface == GPIO_SEL)
                    {
                        if (global_uchar != global_buf2[global_uint])
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nUROW Setting Error...Design");
                            #endif
                            error_code = DPE_UROW_SETTING_ERROR;
                            break;
                        }
                    }
                    #endif
                    #ifdef ENABLE_IAP_SUPPORT                
                    if (hardware_interface == IAP_SEL)
                    {
                        if (global_uchar != actual_iap_urow[global_uint])
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nUROW Setting Error...");
                            #endif
                            error_code = DPE_UROW_SETTING_ERROR;
                            break;
                        }
                    }
                    #endif
                }
            }
        }
    }
    return;
}

#endif
void dp_erase(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nErase...");
    #endif
    dp_flush_global_buf1();
    global_buf1[0] = UROW_ERASE_BITS_BYTE0;
    global_buf1[1] = UROW_ERASE_BITS_BYTE1;
    global_buf1[2] = UROW_ERASE_BITS_BYTE2;
    #ifdef CORE_SUPPORT
    if (dat_support_status & CORE_DAT_SUPPORT_BIT)
    {
        global_buf1[0] |= CORE_ERASE_BITS_BYTE0;
        global_buf1[1] |= CORE_ERASE_BITS_BYTE1;
        global_buf1[2] |= CORE_ERASE_BITS_BYTE2;
    }
    #endif
    /* This is for FROM erase.  Need to get which bits are set to erase from the data file. */
    #ifdef FROM_SUPPORT
    if (dat_support_status & FROM_DAT_SUPPORT_BIT)
    {
        
        global_uchar = (DPUCHAR) dp_get_bytes(FRomAddressMask_ID,0U,1U);
        if (global_uchar & 0x1U)
        {
            global_buf1[1]|=0x80U;
        }
        global_buf1[2] |= (DPUCHAR)(global_uchar >> 1U);
        
    }
    #endif
    #ifdef SECURITY_SUPPORT
    /* This is for security erase */
    if (dat_support_status & SEC_DAT_SUPPORT_BIT)
    {
        global_buf1[0] |= 0xeU;
    }
    #endif
    dp_exe_erase();
    return;
}

void dp_exe_erase(void)
{
    /* This is to identify if urow (bit 14 in the ISC_Sector register) is set */
    dp_read_urow();
    #ifdef ENABLE_IAP_SUPPORT
    if (hardware_interface == IAP_SEL)
    {
        read_urow_nvm();
        /* Do not erase UROW.  Bit 14 */
        global_buf1[1u] &= 0xbfu;
    }
    #endif        
    
    
    opcode = ISC_ERASE;
    IRSCAN_in();
    DRSCAN_in(0u, 23u, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_ERASE_CYCLES);
    dp_poll_erase();
    if (error_code != DPE_SUCCESS)
    {
        #ifdef ENABLE_DEBUG
        dp_display_text("\r\nFailed Erase Operation");
        #endif
        error_code = DPE_ERASE_ERROR;
    }
    if (error_code == DPE_SUCCESS)
    {
        #ifdef ENABLE_GPIO_SUPPORT
        if (hardware_interface == GPIO_SEL)
        {
            dp_program_urow();
        }
        #endif
        
        #ifdef ENABLE_IAP_SUPPORT        
        if (hardware_interface == IAP_SEL)
        {
            program_urow_nvm();
        }
        #endif        
        if (error_code != DPE_SUCCESS)
        {
            #ifdef ENABLE_DEBUG
            dp_display_text("\r\nFailed UROW programming");
            #endif
        }
    }
    
    return;
}
/****************************************************************************
* Purpose: Used by the dp_exe_erase function.
****************************************************************************/
void dp_poll_erase(void)
{
    /* Check for erase status */
    opcode = ISC_NOOP;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    for ( global_ulong = 0U; global_ulong <= MAX_ERASE_POLL; global_ulong++ )
    {
        dp_delay(ERASE_POLL_DELAY);
        /* check for ROWBUSY and COLBUSY */
        DRSCAN_out(5u, (DPUCHAR*)DPNULL, &global_uchar);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
        
        if ((global_uchar & 0x3U) == 0U)
        {
            break;
        }
    }
    if(global_ulong > MAX_ERASE_POLL)
    {
        error_code = DPE_ERASE_ERROR;
    }
    return;
}
/****************************************************************************
* Purpose: This function is used during programming and verification.
****************************************************************************/
void dp_poll_device(void)
{
    /* Check for program status */
    opcode = ISC_NOOP;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    for ( global_ulong = 0U; global_ulong <= MAX_PROGRAM_POLL; global_ulong++ )
    {
        dp_delay(POLL_DELAY);
        /* check for ROWBUSY, COLBUSY and and MAC fail */
        DRSCAN_out(5u, (DPUCHAR*)DPNULL, &global_uchar);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
        
        if ((global_uchar & 0xbU) == 0U)
        {
            break;
        }
    }
    if(global_ulong > MAX_PROGRAM_POLL)
    {
        error_code = DPE_POLL_ERROR;
    }
    
    return;
}



/********************************************************************************************************************/
/* Debug enable functions only                                                                                      */
/********************************************************************************************************************/
#ifdef ENABLE_DEBUG
void dp_read_idcode_action(void)
{
    opcode = IDCODE;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    DRSCAN_out(IDCODE_LENGTH, (DPUCHAR*)DPNULL, global_buf1);
    global_ulong = (DPULONG)global_buf1[0] | (DPULONG)global_buf1[1] << 8u | 
    (DPULONG)global_buf1[2] << 16u | (DPULONG)global_buf1[3] << 24u;
    dp_display_text("IDCODE: ");
    dp_display_value(global_ulong,HEX);
    dp_display_text("\r\n");
    
    
    return;
}

void dp_device_info_action(void)
{
    dp_read_silsig();
    dp_read_urow();
    dp_display_urow();
    #ifdef ENABLE_IAP_SUPPORT
    if (hardware_interface == IAP_SEL)
    {
        read_urow_nvm();
        dp_display_urow_nvm();
    }
    #endif        
    
    
    dp_is_core_configured();
    if (!((device_security_flags & ULUFJ) && ((dat_support_status & SEC_DAT_SUPPORT_BIT) == 0u )))
    {
        dp_read_from();
    }
    global_uchar = 0u;
    dp_read_factory_row();
    /* Needed to do this to avoid Misra-C compliance issues */
    for(global_uchar = 0u; global_uchar < 7u; global_uchar++)
    {
        global_buf1[global_uchar] = global_buf1[global_uchar+1u];
    }
    #ifdef ENABLE_DEBUG
    dp_display_text("FSN: ");
    dp_display_array(global_buf1, 6u ,HEX);
    #endif    
    
    
    dp_output_security();
    
    return;
}

void dp_read_silsig(void)
{
    opcode = USERCODE;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,USERCODE_CYCLES);
    DRSCAN_out(SILSIG_BIT_LENGTH, (DPUCHAR*)DPNULL, global_buf1);
    global_ulong =  (DPULONG)global_buf1[0] | ((DPULONG)global_buf1[1] << 8) | ((DPULONG)global_buf1[2] << 16) | ((DPULONG)global_buf1[3] << 24);
    
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nExpected SILSIG: ");
    dp_display_value(global_ulong,HEX);
    #endif
    
}

void dp_display_urow(void)
{
    DPUCHAR algo_version;
    DPUCHAR programmer;
    DPUCHAR programming_method;
    DPUCHAR software_version;
    
    algo_version = (DPUCHAR)(global_buf2[3] >> 1u) & 0xfu;
    if ((global_buf2[0] & 0x1u) != (global_buf2[0] & 0x20u) >> 5u)
    {
        algo_version |= ((DPUCHAR)(global_buf2[2] >> 7u ) & 0x1u) << 4u;
        algo_version |= (global_buf2[3] & 0x1u) << 5u;
        algo_version |= ((DPUCHAR)(global_buf2[0] >> 5u) & 0x1u) << 6u;
    }
    programming_method = (DPUCHAR)(global_buf2[3] >> 5u) & 0x7u;
    programmer = ((DPUCHAR)(global_buf2[0] >> 6u) & 0x3u) | (DPUCHAR)((DPUCHAR)(global_buf2[1] & 0x3u) << 0x2u);
    software_version = ((DPUCHAR)(global_buf2[1] >> 2u)) | ((DPUCHAR)((DPUCHAR)(global_buf2[2] & 0x1u) << 6u));
    
    dp_display_text("\r\nUser information: ");
    dp_display_text("\r\nCYCLE COUNT: ");
    dp_display_value(cycle_count,DEC);
    dp_display_text("\r\nCHECKSUM = ");
    dp_display_array(&global_buf2[14],2u, HEX);
    dp_display_text("\r\nDesign Name = ");
    dp_display_array(&global_buf2[4],9u, HEX);
    
    dp_display_text("\r\nProgramming Method: ");
    switch(programming_method)
    {
        case IEEE1532_PM:
        dp_display_text("IEEE1532");
        break;
        case STAPL_PM:
        dp_display_text("STAPL");
        break;
        case DIRECTC_PM:
        dp_display_text("DIRECTC");
        break;
        case PDB_PM:
        dp_display_text("PDB");
        break;
        case SVF_PM:
        dp_display_text("SVF");
        break;
        case IAP_PM:
        dp_display_text("IAP");
        break;
        default:
        dp_display_text("Unknown");
        break;
    }
    ;
    dp_display_text("\r\nAlgorithm Version: = ");
    dp_display_value((DPULONG)algo_version, DEC);
    dp_display_text("\r\nProgrammer: = ");
    dp_display_value((DPULONG)algo_version, DEC);
    
    dp_display_text("\r\nProgrammer: ");
    switch(programmer)
    {
        case FP:
        dp_display_text("Flash Pro");
        break;
        case FPLITE:
        dp_display_text("Flash Pro Lite");
        break;
        case FP3:
        dp_display_text("FP3");
        break;
        case SCULPTW:
        dp_display_text("Sculptor");
        break;
        case BPW:
        dp_display_text("BP Programmer");
        break;
        case DIRECTCP:
        dp_display_text("DirectC");
        break;
        case STP:
        dp_display_text("STAPL");
        break;
        case FP4:
        dp_display_text("FP4");
        break;
        default:
        dp_display_text("Unknown");
        break;
    }
    ;
    
    dp_display_text("\r\nSoftware Version = ");
    dp_display_value((DPULONG)software_version, DEC);
    
    dp_display_text("\r\n==================================================");
    return;
}

void dp_output_security(void)
{
    dp_display_text("\r\nSecurity Setting : ");
    
    if (device_security_flags & ULUFP)
    {
        dp_display_text("\r\nFlashROM Write/Erase protected by pass key.");
    }
    if (device_security_flags & ULUFJ)
    {
        dp_display_text("\r\nFlashROM Read protected by pass key.");
    }
    if (device_security_flags & ULAWE)
    {
        dp_display_text("\r\nArray Write/Erase protected by pass key.");
    }
    if (device_security_flags & ULARD)
    {
        dp_display_text("\r\nArray Verify protected by pass key.");
    }
    if (device_security_flags & ULUFE)
    {
        dp_display_text("\r\nEncrypted FlashROM Programming Enabled.");
    }
    if (device_security_flags & ULARE)
    {
        dp_display_text("\r\nEncrypted FPGA Array Programming Enabled.");
    }
    if (device_family & (AFS_BIT | SFS_BIT))
    {        
        if (device_security_flags & ULNW0)
        {
            dp_display_text("\r\nNVM block 0 Write protected by pass key.");
        }
        if (device_security_flags & ULNR0)
        {
            dp_display_text("\r\nNVM block 0 Read protected by pass key.");
        }
        if (device_security_flags & ULNW1)
        {
            dp_display_text("\r\nNVM block 1 Write protected by pass key.");
        }
        if (device_security_flags & ULNR1)
        {
            dp_display_text("\r\nNVM block 1 Read protected by pass key.");
        }
        if (device_security_flags & ULNC0)
        {
            dp_display_text("\r\nEncrypted NVM block 0 Programming Enabled.");
        }
        if (device_security_flags & ULNC1)
        {
            dp_display_text("\r\nEncrypted NVM block 1 Programming Enabled.");
        }
    }
    
    dp_display_text("\r\n==================================================");
    return;
}
#endif
/********************************************************************************************************************/


void dp_read_urow(void)
{
    
    /* read UROW */
    dp_vnr();
    
    opcode = ISC_READ_UROW;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_READ_UROW_CYCLES);
    dp_delay(ISC_READ_UROW_DELAY);
    DRSCAN_out(UROW_BIT_LENGTH, (DPUCHAR*)DPNULL, global_buf2);
    
    cycle_count =  ((DPUINT)global_buf2[12] >> 6U) | ((DPUINT)global_buf2[13] << 2U);
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nUSER_UROW = ");
    dp_display_array(global_buf2,16u, HEX);
    #endif
    
    return;
}

void dp_program_urow(void)
{
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nProgramming UROW...");
    #endif
    
    if (((device_security_flags & IS_ERASE_ONLY) == 0U) && (global_buf1[0] & CORE_ERASE_BITS_BYTE0) )
    {
        #ifdef CORE_SUPPORT
        if (cycle_count < 1023U)
        {
            cycle_count++;
        }
        else
        {
        }
        #endif
    }
    
    if ((device_security_flags & PERM_LOCK_BIT) && (device_security_flags & ULAWE))
    {
        device_security_flags |= IS_RESTORE_DESIGN;
    }
    if (((device_security_flags & IS_ERASE_ONLY)) && ((device_security_flags & IS_RESTORE_DESIGN) == 0U))
    {
        dp_flush_global_buf2();
        global_buf2[12] |= ((DPUCHAR) (cycle_count << 6));
        global_buf2[13] = (DPUCHAR) (cycle_count >> 2);
    }
    else 
    {
        /* Constucting the UROW data */
        if ((device_security_flags & IS_RESTORE_DESIGN) == 0U)
        {
            for (global_uchar=0U; global_uchar < 2U; global_uchar++)
            {
                global_buf2[global_uchar+14U] = (DPUCHAR) dp_get_bytes(CHECKSUM_ID,global_uchar,1U);
            }
            
            for (global_uchar=0U; global_uchar < 9U; global_uchar++)
            {
                global_buf2[global_uchar + 4U] = (DPUCHAR) dp_get_bytes(ACT_UROW_DESIGN_NAME_ID,global_uchar,1U);
            }
            global_buf2[12] |= ((DPUCHAR) (cycle_count << 6));
            global_buf2[13] = (DPUCHAR) (cycle_count >> 2);
            global_buf2[3] |= GPIO_PROGRAMMING_METHOD << 5;
            global_buf2[3] |= (ALGO_VERSION & 0xfu) << 1u;
            if (ALGO_VERSION & 0x40u)
            {
                global_buf2[0] |= 0x20u;
            }
            else
            {
                global_buf2[0] |= 0x1u;
            }
            global_buf2[2] |= ((ALGO_VERSION >> 4u) & 1u) << 7u;
            global_buf2[3] |= ((ALGO_VERSION >> 5u) & 1u);
            global_buf2[0] |= DC_PROGRAMMING_SW << 6u;
            global_buf2[1] |= (DC_PROGRAMMING_SW >> 2u) & 0x3u;
            global_buf2[1] |= DC_SOFTWARE_VERSION << 2u;
            global_buf2[2] |= (DC_SOFTWARE_VERSION >> 6u) & 0x1u;
        }        
    }
    
    for (global_uchar=0U; global_uchar < global_buf_SIZE; global_uchar++)
    {
        global_buf1[global_uchar] = 0xffU;
    }
    
    for (global_uchar=0U; global_uchar < 8U; global_uchar++)
    {
        opcode = ISC_UFROM_ADDR_SHIFT;
        IRSCAN_in();
        DRSCAN_in(0u,3u,&global_uchar);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_UFROM_ADDR_SHIFT_CYCLES);    
        
        opcode = ISC_PROGRAM_UFROM;
        IRSCAN_in();
        DRSCAN_in(0u,FROM_ROW_BIT_LENGTH,global_buf1);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_PROGRAM_UFROM_CYCLES);    
        dp_delay(ISC_PROGRAM_UFROM_DELAY);
    }
    
    opcode = ISC_PROGRAM_UROW;
    IRSCAN_in();
    DRSCAN_in(0u,UROW_BIT_LENGTH,global_buf2);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_PROGRAM_UROW_CYCLES);    
    
    dp_poll_device();
    if (error_code != DPE_SUCCESS)
    {
        error_code = DPE_PROGRAM_UROW_ERROR;
    }
    
    if (error_code == DPE_SUCCESS)
    {
        dp_vnr();
        
        /* readback and verify UROW */
        opcode = ISC_READ_UROW;
        IRSCAN_in();
        goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_READ_UROW_CYCLES);
        dp_delay(ISC_READ_UROW_DELAY);
        
        DRSCAN_out(UROW_BIT_LENGTH,(DPUCHAR*)DPNULL,global_buf1);
        for ( global_uchar = 0U; global_uchar < 16U; global_uchar++ )
        {
            if (global_buf1[global_uchar] != global_buf2[global_uchar])
            {
                error_code = DPE_PROGRAM_UROW_ERROR;
                break;
            }
        }
    }
    return;
}

void dp_init_aes(void)
{
    opcode = AES_INIT;
    IRSCAN_in();
    DRSCAN_in(0u, AES_BIT_LENGTH, (DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,AES_INIT_CYCLES);
    dp_delay(AES_INIT_DELAY);
    return;
}

void dp_set_aes_mode(void)
{
    opcode = AES_MODE;
    IRSCAN_in();
    DRSCAN_in(0u, AES_MODE_BIT_LENGTH, &global_uchar);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,AES_MODE_CYCLES);
    return;
    
}
/************************************************************************************************************************************/
/* --- If data is encrypted for a given block, the device must also be encrytpted for that block                                    */
/* --- If a device block is encrypted, the data must either be encrypted or a VALID key must be present                             */
/*     It is enough to check if the match result is valid if the data is not encrypted.                                             */
/************************************************************************************************************************************/
#ifndef ENABLE_CODE_SPACE_OPTIMIZATION
void dp_check_security_settings(void)
{
    /* Plain text vs encryption check */
    if ( (Action_code == DP_ERASE_ACTION_CODE) || (Action_code == DP_PROGRAM_ACTION_CODE) || 
    (Action_code == DP_VERIFY_ACTION_CODE) || (Action_code == DP_ERASE_ARRAY_ACTION_CODE) || (Action_code == DP_PROGRAM_ARRAY_ACTION_CODE) || 
    (Action_code == DP_VERIFY_ARRAY_ACTION_CODE) || (Action_code == DP_ENC_DATA_AUTHENTICATION_ACTION_CODE) )
    {
        if (dat_support_status & CORE_DAT_SUPPORT_BIT)
        {
            /* Data is encrypted */
            if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
            {
                /* Device must be encrypted */
                if ((device_security_flags & ULARE) == 0U)
                {
                    #ifdef ENABLE_DEBUG
                    dp_display_text("\r\nFPGA Array Encryption is enforced. Plain text programming and verification is prohibited.");
                    #endif
                    error_code = DPE_CORE_ENC_ERROR;
                }
            }
            /* Data is plain text */
            else
            {
                /* Device must not be encrytped or a valid key must be present */
                if ((device_security_flags & ULARE) && ((device_security_flags & SEC_KEY_OK) == 0U))
                {
                    #ifdef ENABLE_DEBUG
                    dp_display_text("\r\nFPGA Array Encryption is not enforced.");
                    dp_display_text("\r\nCannot gaurantee valid AES key present in target device.");
                    dp_display_text("\r\nUnable to proceed with Encrypted FPGA Array operation.");
                    #endif
                    error_code = DPE_CORE_PLAIN_ERROR;
                }
                if (error_code == DPE_SUCCESS)
                {
                    if ((device_security_flags & ULARD) && ((device_security_flags & SEC_KEY_OK) == 0U))
                    {
                        #ifdef ENABLE_DEBUG
                        dp_display_text("\r\nFPGA Array Verification is protected by pass key.");
                        dp_display_text("\r\nA valid pass key needs to be provided.");
                        #endif
                        error_code = DPE_CORE_PLAIN_ERROR;
                    }
                }
                if (error_code == DPE_SUCCESS)
                {
                    if ( (Action_code == DP_ERASE_ACTION_CODE) || (Action_code == DP_PROGRAM_ACTION_CODE) || 
                    (Action_code == DP_ERASE_ARRAY_ACTION_CODE) || (Action_code == DP_PROGRAM_ARRAY_ACTION_CODE) || 
                    (Action_code == DP_ENC_DATA_AUTHENTICATION_ACTION_CODE) )
                    {
                        /* If device is keyed, the security match must pass */
                        if ((device_security_flags & ULAWE) && ((device_security_flags & SEC_KEY_OK) == 0U))
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nFPGA Array Write/Erase is protected by pass key.");
                            dp_display_text("\r\nA valid pass key needs to be provided.");
                            #endif
                            error_code = DPE_CORE_PLAIN_ERROR;
                        }
                    }
                }
            }
        }
    }
    if (error_code == DPE_SUCCESS)
    {
        if ( (Action_code == DP_ERASE_ACTION_CODE) || (Action_code == DP_PROGRAM_ACTION_CODE) || 
        (Action_code == DP_VERIFY_ACTION_CODE) || (Action_code == DP_ERASE_FROM_ACTION_CODE) || (Action_code == DP_PROGRAM_FROM_ACTION_CODE) || 
        (Action_code == DP_VERIFY_FROM_ACTION_CODE) )
        {
            if (dat_support_status & FROM_DAT_SUPPORT_BIT)
            {
                /* Data is encrypted */
                if (dat_support_status & FROM_DAT_ENCRYPTION_BIT)
                {
                    /* Device must be encrypted */
                    if ((device_security_flags & ULUFE) == 0U)
                    {
                        #ifdef ENABLE_DEBUG
                        dp_display_text("\r\nFlashROM Encryption is enforced. Plain text programming and verification is prohibited.");
                        #endif
                        error_code = DPE_FROM_ENC_ERROR;
                    }
                }
                /* Data is plain text */
                else
                {
                    /* Device must not be encrytped or a valid key must be present */
                    if ((device_security_flags & ULUFE) && ((device_security_flags & SEC_KEY_OK) == 0U))
                    {
                        #ifdef ENABLE_DEBUG
                        dp_display_text("\r\nFlashROM Encryption is not enforced.");
                        dp_display_text("\r\nCannot gaurantee valid AES key present in target device.");
                        dp_display_text("\r\nUnable to proceed with Encrypted FlashROM programming.");
                        #endif
                        error_code = DPE_FROM_PLAIN_ERROR;
                    }
                    if (error_code == DPE_SUCCESS)
                    {
                        if ((device_security_flags & ULUFJ) && ((device_security_flags & SEC_KEY_OK) == 0U))
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nFROM Verification is protected by pass key.");
                            dp_display_text("\r\nA valid pass key needs to be provided.");
                            #endif
                            error_code = DPE_CORE_PLAIN_ERROR;
                        }
                    }
                    if (error_code == DPE_SUCCESS)
                    {
                        if ( (Action_code == DP_ERASE_ACTION_CODE) || (Action_code == DP_PROGRAM_ACTION_CODE) || 
                        (Action_code == DP_ERASE_FROM_ACTION_CODE) || (Action_code == DP_PROGRAM_FROM_ACTION_CODE))
                        {
                            /* If device is keyed, the security match must pass */
                            if ((device_security_flags & ULUFP) && ((device_security_flags & SEC_KEY_OK) == 0U))
                            {
                                #ifdef ENABLE_DEBUG
                                dp_display_text("\r\nFROM Write/Erase is protected by pass key.");
                                dp_display_text("\r\nA valid pass key needs to be provided.");
                                #endif
                                error_code = DPE_CORE_PLAIN_ERROR;
                            }
                        }
                    }                    
                }
            }
        }
    }
    if (error_code == DPE_SUCCESS)
    {
        if ( (Action_code == DP_PROGRAM_ACTION_CODE) || (Action_code == DP_VERIFY_ACTION_CODE) || 
        (Action_code == DP_PROGRAM_NVM_ACTION_CODE) || (Action_code == DP_VERIFY_NVM_ACTION_CODE) ||
        (Action_code == DP_PROGRAM_PRIVATE_CLIENTS_ACTION_CODE) || (Action_code == DP_VERIFY_PRIVATE_CLIENTS_ACTION_CODE)
        )
        {
            if (dat_support_status & NVM0_DAT_SUPPORT_BIT)
            {
                /* Data is encrypted */
                if (dat_support_status & NVM0_DAT_ENCRYPTION_BIT)
                {
                    /* Device must be encrypted */
                    if ((device_security_flags & ULNC0) == 0U)
                    {
                        #ifdef ENABLE_DEBUG
                        dp_display_text("\r\nNVM block 0 Encryption is enforced. Plain text programming is prohibited.");
                        #endif
                        error_code = DPE_NVM0_ENC_ERROR;
                    }
                }
                /* Data is plain text */
                else
                {
                    /* Device must not be encrytped or a valid key must be present */
                    if ((device_security_flags & ULNC0) && ((device_security_flags & SEC_KEY_OK) == 0U))
                    {
                        #ifdef ENABLE_DEBUG
                        dp_display_text("\r\nNVM block 0 Encryption is not enforced.");
                        dp_display_text("\r\nCannot gaurantee valid AES key present in target device.");
                        dp_display_text("\r\nUnable to proceed with Encrypted NVM programming.");
                        #endif
                        error_code = DPE_NVM0_PLAIN_ERROR;
                    }
                    if (error_code == DPE_SUCCESS)
                    {
                        if ((device_security_flags & ULNR0) && ((device_security_flags & SEC_KEY_OK) == 0U))
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nNVM block 0 Read is protected by pass key.");
                            dp_display_text("\r\nA valid pass key needs to be provided.");
                            #endif
                            error_code = DPE_NVM0_PLAIN_ERROR;
                        }
                    }
                    if (error_code == DPE_SUCCESS)
                    {
                        if ( (Action_code == DP_PROGRAM_ACTION_CODE) || (Action_code == DP_PROGRAM_NVM_ACTION_CODE) || (Action_code == DP_PROGRAM_PRIVATE_CLIENTS_ACTION_CODE))
                        {
                            /* If device is keyed, the security match must pass */
                            if ((device_security_flags & ULNW0) && ((device_security_flags & SEC_KEY_OK) == 0U))
                            {
                                #ifdef ENABLE_DEBUG
                                dp_display_text("\r\nNVM block 0 write is protected by pass key.");
                                dp_display_text("\r\nA valid pass key needs to be provided.");
                                #endif
                                error_code = DPE_NVM0_PLAIN_ERROR;
                            }
                        }
                    }
                }
            }
        }
    }
    if (error_code == DPE_SUCCESS)
    {
        if (dat_support_status & NVM1_DAT_SUPPORT_BIT)
        {
            /* Data is encrypted */
            if (dat_support_status & NVM1_DAT_ENCRYPTION_BIT)
            {
                /* Device must be encrypted */
                if ((device_security_flags & ULNC1) == 0U)
                {
                    #ifdef ENABLE_DEBUG
                    dp_display_text("\r\nNVM block 1 Encryption is enforced. Plain text programming is prohibited.");
                    #endif
                    
                    error_code = DPE_NVM1_ENC_ERROR;
                }
            }
            /* Data is plain text */
            else
            {
                /* Device must not be encrytped or a valid key must be present */
                if ((device_security_flags & ULNC1) && ((device_security_flags & SEC_KEY_OK) == 0U))
                {
                    #ifdef ENABLE_DEBUG
                    dp_display_text("\r\nNVM block 1 Encryption is not enforced.");
                    dp_display_text("\r\nCannot gaurantee valid AES key present in target device.");
                    dp_display_text("\r\nUnable to proceed with Encrypted NVM programming.");
                    #endif
                    error_code = DPE_NVM1_PLAIN_ERROR;
                }
                if (error_code == DPE_SUCCESS)
                {
                    if ((device_security_flags & ULNR1) && ((device_security_flags & SEC_KEY_OK) == 0U))
                    {
                        #ifdef ENABLE_DEBUG
                        dp_display_text("\r\nNVM block 1 Read is protected by pass key.");
                        dp_display_text("\r\nA valid pass key needs to be provided.");
                        #endif
                        error_code = DPE_NVM1_PLAIN_ERROR;
                    }
                }
                if (error_code == DPE_SUCCESS)
                {
                    if ( (Action_code == DP_PROGRAM_ACTION_CODE) || (Action_code == DP_PROGRAM_NVM_ACTION_CODE))
                    {
                        /* If device is keyed, the security match must pass */
                        if ((device_security_flags & ULNW1) && ((device_security_flags & SEC_KEY_OK) == 0U))
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nNVM block 1 write is protected by pass key.");
                            dp_display_text("\r\nA valid pass key needs to be provided.");
                            #endif
                            error_code = DPE_NVM1_PLAIN_ERROR;
                        }
                    }
                }
            }
        }
    }
    if (error_code == DPE_SUCCESS)
    {
        if (dat_support_status & NVM2_DAT_SUPPORT_BIT)
        {
            /* Data is encrypted */
            if (dat_support_status & NVM2_DAT_ENCRYPTION_BIT)
            {
                if ((device_security_flags & ULNC2) == 0U)
                {
                    #ifdef ENABLE_DEBUG
                    dp_display_text("\r\nNVM block 2 Encryption is enforced. Plain text programming is prohibited.");
                    #endif
                    error_code = DPE_NVM2_ENC_ERROR;
                }
            }
            /* Data is plain text */
            else
            {
                /* Device must not be encrytped or a valid key must be present */
                if ((device_security_flags & ULNC2) && ((device_security_flags & SEC_KEY_OK) == 0U))
                {
                    #ifdef ENABLE_DEBUG
                    dp_display_text("\r\nNVM block 2 Encryption is not enforced.");
                    dp_display_text("\r\nCannot gaurantee valid AES key present in target device.");
                    dp_display_text("\r\nUnable to proceed with Encrypted NVM programming.");
                    #endif
                    error_code = DPE_NVM2_PLAIN_ERROR;
                }
                if (error_code == DPE_SUCCESS)
                {
                    if ((device_security_flags & ULNR2) && ((device_security_flags & SEC_KEY_OK) == 0U))
                    {
                        #ifdef ENABLE_DEBUG
                        dp_display_text("\r\nNVM block 2 Read is protected by pass key.");
                        dp_display_text("\r\nA valid pass key needs to be provided.");
                        #endif
                        error_code = DPE_NVM2_PLAIN_ERROR;
                    }
                }
                if (error_code == DPE_SUCCESS)
                {
                    if ( (Action_code == DP_PROGRAM_ACTION_CODE) || (Action_code == DP_PROGRAM_NVM_ACTION_CODE))
                    {
                        /* If device is keyed, the security match must pass */
                        if ((device_security_flags & ULNW2) && ((device_security_flags & SEC_KEY_OK) == 0U))
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nNVM block 2 write is protected by pass key.");
                            dp_display_text("\r\nA valid pass key needs to be provided.");
                            #endif
                            error_code = DPE_NVM2_PLAIN_ERROR;
                        }
                    }
                }
            }
        }
    }
    if (error_code == DPE_SUCCESS)
    {
        if (dat_support_status & NVM3_DAT_SUPPORT_BIT)
        {
            /* Data is encrypted */
            if (dat_support_status & NVM3_DAT_ENCRYPTION_BIT)
            {
                /* Device must be encrypted */
                if ((device_security_flags & ULNC3) == 0U)
                {
                    #ifdef ENABLE_DEBUG
                    dp_display_text("\r\nNVM block 3 Encryption is enforced. Plain text programming is prohibited.");
                    #endif
                    error_code = DPE_NVM3_ENC_ERROR;
                }
            }
            /* Data is plain text */
            else
            {
                /* Device must not be encrytped or a valid key must be present */
                if ((device_security_flags & ULNC3) && ((device_security_flags & SEC_KEY_OK) == 0U))
                {
                    #ifdef ENABLE_DEBUG
                    dp_display_text("\r\nNVM block 3 Encryption is not enforced.");
                    dp_display_text("\r\nCannot gaurantee valid AES key present in target device.");
                    dp_display_text("\r\nUnable to proceed with Encrypted NVM programming.");
                    #endif
                    error_code = DPE_NVM3_PLAIN_ERROR;
                }
                if (error_code == DPE_SUCCESS)
                {
                    if ((device_security_flags & ULNR3) && ((device_security_flags & SEC_KEY_OK) == 0U))
                    {
                        #ifdef ENABLE_DEBUG
                        dp_display_text("\r\nNVM block 3 Read is protected by pass key.");
                        dp_display_text("\r\nA valid pass key needs to be provided.");
                        #endif
                        error_code = DPE_NVM3_PLAIN_ERROR;
                    }
                }
                if (error_code == DPE_SUCCESS)
                {
                    if ( (Action_code == DP_PROGRAM_ACTION_CODE) || (Action_code == DP_PROGRAM_NVM_ACTION_CODE))
                    {
                        /* If device is keyed, the security match must pass */
                        if ((device_security_flags & ULNW3) && ((device_security_flags & SEC_KEY_OK) == 0U))
                        {
                            #ifdef ENABLE_DEBUG
                            dp_display_text("\r\nNVM block 3 Write is protected by pass key.");
                            dp_display_text("\r\nA valid pass key needs to be provided.");
                            #endif
                            error_code = DPE_NVM3_PLAIN_ERROR;
                        }
                    }
                }
            }
        }
    }
    /* Plain text vs encryption check */
    if (error_code == DPE_SUCCESS)
    {
        if ( (Action_code == DP_ERASE_SECURITY_ACTION_CODE) || (Action_code == DP_PROGRAM_SECURITY_ACTION_CODE) )
        {
            if (dat_support_status & SEC_DAT_SUPPORT_BIT)
            {
                /* Device must be encrypted */
                if ((device_security_flags & ULUPC) && ((device_security_flags & SEC_KEY_OK) == 0U))
                {
                    #ifdef ENABLE_DEBUG
                    dp_display_text("\r\nError, pass key match failure.");
                    #endif
                    error_code = DPE_MATCH_ERROR;
                }
            }
        }
    }
    return;
}
#else
void dp_check_security_settings(void)
{
    DPUCHAR dev_enc_status = 0u;
    DPUCHAR dat_enc_status = 0u;
    
    /* global_ulong is used as a mask to check if any of the device blocks are protected 
    with a passkey but the data file has no pass key to match */
    global_ulong = 0u;
    /* global_uchar is used to mask out checking security settings of blocks that are not 
    supported in the data file */
    global_uchar = (DPUCHAR) ((dat_support_status & 0x3u) | ((dat_support_status >> 1) & 0x3cu));
    
    /* Get the block encryption settings from the data file. */
    dat_enc_status = (DPUCHAR) (dat_support_status >> 10);
    /* Line up the device security bit settings to match the order of the dat file */
    #ifdef CORE_SUPPORT
    dev_enc_status |= (DPUCHAR) ((device_security_flags & ULARE) >> 20);
    /* Find out if the array is protected by a pass key */
    if (!((global_uchar & 0x1u) && (dev_enc_status & 0x1u)))
    {
        global_ulong |= ULARD | ULAWE;
    }
    #else
    /* If the code of a certain block is disabled, the corresponding security bits are ignored */
    dat_enc_status &= 0xFEu;
    #endif
    #ifdef FROM_SUPPORT
    /* Line up the device security bit settings to match the order of the dat file */
    dev_enc_status |= (DPUCHAR) ((device_security_flags & ULUFE) >> 17);
    /* Find out if FORM is protected by a pass key */
    if ( !((global_uchar & 0x2u) && (dev_enc_status & 0x2u) ))
    {
        global_ulong |= ULUFJ | ULUFP;
    }
    #else
    /* If the code of a certain block is disabled, the corresponding security bits are ignored */
    dat_enc_status &= 0xFD;
    #endif
    #ifdef NVM_SUPPORT
    /* Line up the device security bit settings to match the order of the dat file */
    dev_enc_status |= (DPUCHAR) ((device_security_flags & ULNC0) | 
    ((device_security_flags & ULNC1) >> 2) | 
    ((device_security_flags & ULNC2) >> 4) | 
    ((device_security_flags & ULNC3) >> 6));
    /* Find out if any of the NVM blocks are protected by a pass key */
    if (!((global_uchar & 0x3Cu) && (dev_enc_status & 0x3Cu)))
    {
        global_ulong |= ULNR0 | ULNW0 | ULNR1 | ULNW1 | ULNR2 | ULNW2 | ULNR3 | ULNW3;
    }
    #else
    /* If the code of a certain block is disabled, the corresponding security bits are ignored */
    dat_enc_status &= 0xC3;
    #endif
    
    
    /*  If the pass key does not exist in the data file, then there are three possiblities:
    1. Data file is compiled for plain text with no security programming.
    2. Data file is encrypted for all the supported blobks.
    3. Data file is compiled for a cobmbination of both options 1 and 2. 
    
    For case # 1 and 3, the test should fail if there is a mismatch with the encryption bit settings.      
    For case # 2 and 3, the test should fail if the any of the blocks are protected by a pass key and 
    those blocks are not encrypted.
    
    */
    dp_get_bytes(UKEY_ID,0u,1u);
    if (return_bytes == 0u)
    {
        /* This test is to check for a mismatch with the encryption bit settings */
        if  ( ((dev_enc_status & global_uchar) != (dat_enc_status & global_uchar)) ||
        /* If the data is not encrypted, we will need to fail if any of the blocks are protected. */
        /* dat_status & global status is a check to make sure that the file is not encrypted.
        This check is only valid for plain text files.  */
        (device_security_flags & global_ulong) )
        {
            
            error_code = DPE_SECURITY_BIT_MISMATCH;
        }
    }
    /* If the pass key exists in the data file, then check the match result */
    else if (dev_enc_status)
    {
        if ((device_security_flags & SEC_KEY_OK) == 0u)
        {
            error_code = DPE_MATCH_ERROR;
        }
    }
    else
    {
    }
}
#endif


void dp_get_dat_dual_key_flag(void)
{
    global_uchar = (DPUCHAR) dp_get_bytes(Header_ID,DUAL_KEY_SUPPORT_OFFSET,1U);
    if ( global_uchar )
    {
        device_family |= DUAL_KEY_BIT;
        
        /* setting the AES mode */
        if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
        {
            AES_mode_value = 2U;
        }
        else 
        {
            AES_mode_value = 1U;
        }
        dat_support_status |= CORE_DAT_ENCRYPTION_BIT;
    }
    
    return;
}
void dp_read_factory_row(void)
{
    
    dp_vnr();
    opcode = FACTORY_ADDRESS_SHIFT;
    IRSCAN_in();
    DRSCAN_in(0u, 3u, &global_uchar);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,FACTORY_ADDRESS_SHIFT_CYCLES);
    
    opcode = READ_FACTORY;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,READ_FACTORY_CYCLES);
    dp_delay(READ_FACTORY_DELAY);
    
    DRSCAN_out(FACTORY_ROW_BIT_LENGTH, (DPUCHAR*)DPNULL, global_buf1);
    
    
    return;
}

void dp_is_core_configured(void)
{
    opcode = BYPASS;
    IRSCAN_out(&global_uchar);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    /* This is needed to avoid a potential glitch while exiting programming mode. */
    if (!(global_uchar & 0x4U))
    {
        #ifdef ENABLE_DEBUG
        if ((Action_code == DP_PROGRAM_ACTION_CODE) || (Action_code == DP_PROGRAM_ARRAY_ACTION_CODE) || 
        (Action_code == DP_VERIFY_ACTION_CODE) || (Action_code == DP_VERIFY_ARRAY_ACTION_CODE))
        {
            dp_display_text("\r\nError: Failed to enable FPGA array.");
            error_code = DPE_PROGRAM_RLOCK_ERROR;
        }
        else
        {
            dp_display_text("\r\nFPGA array is not enabled.");
        }
        #endif
    }
    #ifdef ENABLE_DEBUG
    else
    {
        dp_display_text("\r\nFPGA array is programmed and enabled.");
    }
    #endif
    return;
}
/****************************************************************************
* Purpose: To detect if discrete address shifting is required if applicable.
****************************************************************************/
#if ( (!defined ENABLE_CODE_SPACE_OPTIMIZATION) || (defined ENABLE_DAS_SUPPORT) )
void dp_das_check(void)
{
    #ifdef ENABLE_DAS_SUPPORT
    if (((device_ID & AXXE1500X_ID_MASK) == (AXXE1500X_ID & AXXE1500X_ID_MASK)) && ((device_ID & AXXE1500X_ID_MASK) == (DataIndex & AXXE1500X_ID_MASK)))
    {
        if ((global_buf1[6] & 0x70U) == 0x0U)
        {
            device_family |= DAS_BIT;
        }
    }
    #endif
    
    return;
}
#endif
