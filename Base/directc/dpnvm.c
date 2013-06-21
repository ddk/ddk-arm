/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpnvm.c                                                 */
/*                                                                          */
/*  Description:    Contains initialization and data checking functions.    */
/*                                                                          */
/* ************************************************************************ */


#include "dpuser.h"
#include "dpdef.h"
#include "dpalg.h"
#include "dputil.h"
#include "dpcom.h"
#include "dpnvm.h"
#include "dpsecurity.h"
#include "dpjtag.h"
#include "dpcore.h"

#ifdef NVM_SUPPORT
DPUCHAR Protection;
DPUINT Par;
DPULONG ParSize;
DPUINT NumOfPart;

/************************************************************************************************/
/*  NVM Action Functions                                                                        */
/************************************************************************************************/
#ifndef DISABLE_NVM_SPECIFIC_ACTIONS

void dp_program_nvm_action(void)
{
    if (((device_family & SFS_BIT) == SFS_BIT) && (hardware_interface == IAP_SEL))
    {
        dp_initialize_access_nvm();
    }
    
    if (dat_support_status & NVM0_DAT_SUPPORT_BIT)
    {
        #ifdef NVM_ENCRYPT
        /* Encryption support */
        if (dat_support_status & NVM0_DAT_ENCRYPTION_BIT)
        {
            if (((device_family & SFS_BIT) == SFS_BIT) && ((hardware_interface == GPIO_SEL) || (enable_mss_support)))
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
        if ((dat_support_status & NVM0_DAT_ENCRYPTION_BIT) == 0u)
        {
            if (((device_family & SFS_BIT) == SFS_BIT) && ((hardware_interface == GPIO_SEL) || (enable_mss_support)))
            {
                dp_program_nvm_block(PRIVATE_CLIENT_PHANTOM_BLOCK);
                if (error_code == DPE_SUCCESS)
                {
                    dp_verify_nvm_block(PRIVATE_CLIENT_PHANTOM_BLOCK);
                }
            }
            if (error_code == DPE_SUCCESS)
            {
                dp_program_nvm_block(0u);
                if (error_code == DPE_SUCCESS)
                {
                    dp_verify_nvm_block(0u);
                }
            }
        }
        #endif
    }
    if ((device_family & SFS_BIT) != SFS_BIT)
    {
        if (error_code == DPE_SUCCESS)
        {
            if (dat_support_status & NVM1_DAT_SUPPORT_BIT)
            {
                /* Encryption support */
                #ifdef NVM_ENCRYPT
                if (dat_support_status & NVM1_DAT_ENCRYPTION_BIT)
                {
                    dp_enc_program_nvm_block(1u);
                }
                #endif
                #ifdef NVM_PLAIN
                /* Plain text support */
                if ((dat_support_status & NVM1_DAT_ENCRYPTION_BIT) == 0u)
                {
                    dp_program_nvm_block(1u);
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
                    dp_enc_program_nvm_block(2u);
                }
                #endif
                #ifdef NVM_PLAIN
                /* Plain text support */
                if ((dat_support_status & NVM2_DAT_ENCRYPTION_BIT) == 0u)
                {
                    dp_program_nvm_block(2u);
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
                    dp_enc_program_nvm_block(3u);
                }
                #endif
                #ifdef NVM_PLAIN
                /* Plain text support */
                if ((dat_support_status & NVM3_DAT_ENCRYPTION_BIT) == 0u)
                {
                    dp_program_nvm_block(3u);
                    if (error_code == DPE_SUCCESS)
                    {
                        dp_verify_nvm_block(3u);
                    }
                }
                #endif
            }
        }
    }
    return;
}

void dp_program_nvm_private_clients_action(void)
{
    if (((device_family & SFS_BIT) == SFS_BIT) && (hardware_interface == IAP_SEL))
    {
        dp_initialize_access_nvm();
    }
    
    if (dat_support_status & NVM0_DAT_SUPPORT_BIT)
    {
        #ifdef NVM_ENCRYPT
        /* Encryption support */
        if (dat_support_status & NVM0_DAT_ENCRYPTION_BIT)
        {
            dp_enc_program_nvm_block(PRIVATE_CLIENT_PHANTOM_BLOCK);
        }
        #endif
        #ifdef NVM_PLAIN
        /* Plain text support */
        if ((dat_support_status & NVM0_DAT_ENCRYPTION_BIT) == 0u)
        {
            dp_program_nvm_block(PRIVATE_CLIENT_PHANTOM_BLOCK);
            if (error_code == DPE_SUCCESS)
            {
                dp_verify_nvm_block(PRIVATE_CLIENT_PHANTOM_BLOCK);
            }
        }
        #endif
    }
    return;
}

#ifdef NVM_PLAIN
void dp_verify_nvm_private_clients_action(void)
{
    if (((device_family & SFS_BIT) == SFS_BIT) && (hardware_interface == IAP_SEL))
    {
        dp_initialize_access_nvm();
    }
    if (dat_support_status & NVM0_DAT_SUPPORT_BIT)
    {
        if ((dat_support_status & NVM0_DAT_ENCRYPTION_BIT) == 0u)
        {
            dp_verify_nvm_block(PRIVATE_CLIENT_PHANTOM_BLOCK);
        }
    }
    return;   
}

void dp_verify_nvm_action(void)
{
    if (((device_family & SFS_BIT) == SFS_BIT) && (hardware_interface == IAP_SEL))
    {
        dp_initialize_access_nvm();
    }
    if (dat_support_status & NVM0_DAT_SUPPORT_BIT)
    {
        if ((dat_support_status & NVM0_DAT_ENCRYPTION_BIT) == 0u)
        {
            if (((device_family & SFS_BIT) == SFS_BIT) && ((hardware_interface == GPIO_SEL) || (enable_mss_support)))
            {
                dp_verify_nvm_block(PRIVATE_CLIENT_PHANTOM_BLOCK);
            }
            if (error_code == DPE_SUCCESS)
            {
                dp_verify_nvm_block(0u);
            }
        }
    }
    if ((device_family & SFS_BIT) != SFS_BIT)
    {
        if (error_code == DPE_SUCCESS)
        {
            if (dat_support_status & NVM1_DAT_SUPPORT_BIT)
            {
                if ((dat_support_status & NVM1_DAT_ENCRYPTION_BIT) == 0u)
                {
                    dp_verify_nvm_block(1u);
                }
            }
        }
        
        if (error_code == DPE_SUCCESS)
        {
            if (dat_support_status & NVM2_DAT_SUPPORT_BIT)
            {
                if ((dat_support_status & NVM2_DAT_ENCRYPTION_BIT) == 0u )
                {
                    dp_verify_nvm_block(2u);
                }
            }
        }
        if (error_code == DPE_SUCCESS)
        {
            if (dat_support_status & NVM3_DAT_SUPPORT_BIT)
            {
                if ((dat_support_status & NVM3_DAT_ENCRYPTION_BIT) == 0u )
                {
                    dp_verify_nvm_block(3u);
                }
            }
        }
    }
    return;
}
#endif

void dp_check_device_and_rev(void)
{
    dp_get_bytes(UKEY_ID,0u,1u);
    if (return_bytes)
    {
        #ifdef ENABLE_DEBUG
        dp_display_text("\r\nError: This action is not supported with security programming enabled in the data file.");
        #endif
        error_code = DPE_ACTION_NOT_FOUND;
    }
    else 
    {
        /* Case 1: if device is AFS600 and rev is 2, then fail.*/
        if ( ( (device_ID == AFS600_ID) && (device_rev == 2u) ) ||
        /* Case 2: if device is AFS1500 and rev is 0 then fail.
        or device is AFS1500 and rev == 2 and NVM block 3 is enabled, then fail.*/
        ( (device_ID == AFS1500_ID) && ((device_rev == 0u) || 
        ( (device_rev == 2u) && (dat_support_status & NVM3_DAT_SUPPORT_BIT) )) )
        )
        {
            #ifdef ENABLE_DEBUG
            dp_display_text("\r\nError: This action is not supported in this revision of the silicon.");
            #endif
            error_code = DP_READ_IDCODE_ACTION_CODE;
        }
    }
    return;
}
#endif
/*****************************************************************************/

/*****************************************************************************/
/* PLAIN Programming Support Functions                                       */
/*****************************************************************************/
#ifdef NVM_PLAIN
void dp_program_nvm_block(DPUCHAR BlockNum)
{
    DPUINT Page_number;
    /* Get how many partitions are in the code */
    /* Need to identify if the data is present for the private client support on smart fusion since
    there is no dedicated support bit for it to indicate if the data is present in the file or not. */
    if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
    {
        NumOfPart = (DPUINT) dp_get_bytes(NumOfPartPriv_all_ID,0u, 2u);      
    }
    else
    {
        NumOfPart = (DPUINT) dp_get_bytes(NumOfPart_ID+BlockNum*NVM_OFFSET,0u, 2u);
    }
    if (NumOfPart)
    {
        #ifdef ENABLE_DEBUG
        if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
        {
            dp_display_text("\r\nProgram System Init and Boot Clients...");
        }
        else
        {
            dp_display_text("\r\nProgramming NVM block ");
            dp_display_value((DPULONG)BlockNum,DEC);
        }
        #endif
        
        DataIndex = 0u;
        
        /* Go through all the partions in the file */
        for (Par=0u ; Par < NumOfPart; Par++)
        {
            /* Get partition size and protection */
            if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
            {      
                ParSize = dp_get_bytes (NvmParSizePriv_all_ID,(Par*32u) >> 3u, 4u);
                Protection = (DPUCHAR) dp_get_bytes(NvmProtectPriv_all_ID,(Par*3u) >> 3u, 1u) << 2u;          
            }
            else
            {
                ParSize = dp_get_bytes (NvmParSize_ID + BlockNum*NVM_OFFSET,(Par*32u) >> 3u, 4u);
                Protection = (DPUCHAR) dp_get_bytes(NvmProtect_ID + BlockNum*NVM_OFFSET,(Par*3u) >> 3u, 1u) << 2u;
            }
            
            dp_flush_global_buf2();
            /* Get the starting address of the partition */
            /* 22 is the number of bits per one address */
            /* Par*22 >> 8 is the location of the first byte of the address 
            in the data block*/
            /* The number of bytes to get could be 3 or 4 depending on whether the 
            bits span over 3 or 4 bytes
            (par*22) & 7 is the number of bits that need to be shifted to get to 
            the first valid bit in the address.
            Ex: for par=1, it is equal to 6.
            We add 22 to this value, then the integer value of adding 7 and divide 
            the total by 8 will get the us the number of bytes needed.
            >> ((Par*22) & 7) lines up the address so that it always starts from 
            bit postion 0 */
            if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
            {
                global_ulong = dp_get_bytes (NvmAddrPriv_all_ID,(Par*22u) >> 3u, (DPUCHAR) ((((Par*22u) & 7u) + 22u + 7u) >> 3u)) >> ((Par*22u) & 7u) ;          
            }
            else
            {
                global_ulong = dp_get_bytes (NvmAddr_ID + BlockNum*NVM_OFFSET,(Par*22u) >> 3u, (DPUCHAR) ((((Par*22u) & 7u) + 22u + 7u) >> 3u)) >> ((Par*22u) & 7u) ;
            }
            /* global_buf2 holds the starting address of the NVM partition */
            global_buf2[1] = (DPUCHAR) (global_ulong << 5u) | 0x10u;
            global_buf2[2] = (DPUCHAR) (global_ulong >> 3u);
            global_buf2[3] = (DPUCHAR) (global_ulong >> 11u);
            global_buf2[4] = (DPUCHAR) (global_ulong >> 19u);
            Page_number = (DPUINT)(global_buf2[2] >> 5u);
            Page_number |= (DPUINT) (global_buf2[3] << 3u);
            Page_number |= (DPUINT) ((global_buf2[4] & 0x1) << 11u);
            
            /* Load NVM Address */
            opcode = NVM_ADDRESS_SHIFT;
            IRSCAN_in();
            DRSCAN_in(0u, NVM_ADDRESS_LENGTH, global_buf2);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,NVM_ADDRESS_SHIFT_CYCLES);
            dp_delay(NVM_ADDRESS_SHIFT_DELAY);
            
            /* Perform address polling */
            for ( global_ulong = 1u; 
                global_ulong <= MAX_LOAD_NVM_ADDRESS_POLL; 
            global_ulong++ )
            {
                DRSCAN_out(NVM_ADDRESS_LENGTH, global_buf2, global_buf1);
                if ((global_buf1[4] & 0x4u) == 0u)
                {
                    break;
                }
                
                opcode = NVM_ADDRESS_SHIFT;
                IRSCAN_in();
                DRSCAN_in(0u, NVM_ADDRESS_LENGTH, global_buf2);
                goto_jtag_state(JTAG_PAUSE_DR,NVM_ADDRESS_SHIFT_CYCLES);
                dp_delay(NVM_ADDRESS_SHIFT_POLL_DELAY);
            }
            if (global_ulong > MAX_LOAD_NVM_ADDRESS_POLL)
            {
                error_code = DPE_POLL_ERROR;
                /* This is needed to terminate the first loop */
                Par = NumOfPart;
            }
            
            
            if (error_code == DPE_SUCCESS)
            {
                
                for (global_uint=1u; global_uint <= ParSize ;global_uint++)
                {
                    /* Load the data */
                    
                    #ifdef ENABLE_DEBUG
                    if ((global_uint % 100u) == 0u)
                    {
                        dp_display_text(".");
                    }
                    #endif
                    opcode = NVM_DATA_SHIFT;
                    IRSCAN_in();
                    if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
                    { 
                        dp_get_and_DRSCAN_in(NvmDataPriv_all_ID, NVM_DATA_LENGTH, DataIndex);
                    }
                    else
                    {
                        dp_get_and_DRSCAN_in(NvmData_ID+BlockNum*NVM_OFFSET, NVM_DATA_LENGTH, DataIndex);
                    }
                    
                    goto_jtag_state(JTAG_RUN_TEST_IDLE,NVM_DATA_SHIFT_CYCLES);
                    dp_delay(NVM_DATA_SHIFT_DELAY);
                    DataIndex = DataIndex + NVM_DATA_LENGTH;
                    
                    if ((global_uint % NVM_DATA_LENGTH) == 0u)
                    {
                        opcode = NVM_PROGRAM;
                        IRSCAN_in();
                        DRSCAN_in(0u, 5u, &Protection);
                        goto_jtag_state(JTAG_RUN_TEST_IDLE,NVM_PROGRAM_CYCLES);
                        dp_delay(NVM_PROGRAM_POLL_DELAY);
                        
                        /* Perform NVM Program polling */
                        for ( global_ulong = 1u; 
                            global_ulong <= MAX_LOAD_NVM_ADDRESS_POLL; 
                        global_ulong++ )
                        {
                            
                            DRSCAN_out(5u, &Protection, &global_uchar);
                            if ( ((global_uchar & 0x1cu) == 0u) || ((global_uchar & 0x1cu) == 0xcu) )
                            {
                                break;
                            }
                            opcode = NVM_PROGRAM;
                            IRSCAN_in();
                            DRSCAN_in(0u, 5u, &Protection);
                            goto_jtag_state(JTAG_PAUSE_DR,NVM_PROGRAM_CYCLES);
                            dp_delay(NVM_PROGRAM_POLL_DELAY);
                        }
                        
                        #ifdef ENABLE_DEBUG
                        if ((global_uchar & 0xcu) == 0xcu)
                        {
                            dp_display_text("\r\nWARNING: Write count threshold exceeded. Page ");
                            dp_display_value(Page_number,DEC);
                            dp_display_text(", Block  ");
                            dp_display_value(BlockNum,DEC);
                        }
                        #endif
                        
                        
                        if (global_ulong > MAX_LOAD_NVM_ADDRESS_POLL)
                        {
                            error_code = DPE_POLL_ERROR;
                            /* This is needed to terminate the first loop */                        
                            Par = NumOfPart;
                            break;
                        }
                        Page_number++;
                    }
                }
                
                if (error_code != DPE_SUCCESS)
                {
                    break;
                }
            }
        }
    }
    return;
}

void dp_verify_nvm_block(DPUCHAR BlockNum)
{	
    DPUINT PageAddress;
    DPULONG TmpData;
    DPUCHAR offset;
    
    /* Get how many partitions are in the code */
    /* Need to identify if the data is present for the private client support on smart fusion since
    there is no dedicated support bit for it to indicate if the data is present in the file or not. */
    
    if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
    {
        NumOfPart = (DPUINT) dp_get_bytes(NumOfPartPriv_all_ID,0u,2u);
    }
    else
    {
        NumOfPart = (DPUINT) dp_get_bytes(NumOfPart_ID+BlockNum*NVM_OFFSET,0u,2u);
    }
    if (NumOfPart)
    {
        #ifdef ENABLE_DEBUG
        if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
        {
            dp_display_text("\r\nVerify System Init and Boot Clients...");
        }
        else
        {
            dp_display_text("\r\nVerifying NVM block ");
            dp_display_value((DPULONG)BlockNum,DEC);
        }
        #endif
        
        
        DataIndex = 0u;
        
        /* Go through all the partions in the file */
        for (Par=0u; Par < NumOfPart; Par++)
        {
            /* Get the starting address of the partition */
            /* 22 is the number of bits per one address */
            /* Par*22 >> 8 is the location of the first byte of the address in the data block*/
            /* The number of bytes to get could be 3 or 4 depending on wether the bits span over 3 or 4 bytes
            (par*22) & 7 is the number of bits that need to be shifted to get to the first valid bit in the address.
            Ex: for par=1, it is equal to 6.
            We add 22 to this value, then the integer value of adding 7 and divide the total by 8 will get the us the number of bytes needed.
            >> ((Par*22) & 7) lines up the address so that it always starts from bit postion 0 
            */
            if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
            {
                global_ulong = dp_get_bytes (NvmAddrPriv_all_ID,(Par*22u) >> 3u,(DPUCHAR)((((Par*22u) & 7u) + 22u + 7u) >> 3u)) >> ((Par*22u) & 7u) ;          
            }
            else
            {
                global_ulong = dp_get_bytes (NvmAddr_ID + BlockNum*NVM_OFFSET,(Par*22u) >> 3u,(DPUCHAR)((((Par*22u) & 7u) + 22u + 7u) >> 3u)) >> ((Par*22u) & 7u) ;
            }
            
            PageAddress = global_ulong >> 8u; /* 14 bit long */
            offset = 0u;					 /* 5 bit long */
            /* Get partition size */
            if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
            { 
                ParSize = dp_get_bytes(NvmParSizePriv_all_ID,(Par*32u) >> 3u,4u);          
            }
            else
            {
                ParSize = dp_get_bytes(NvmParSize_ID+BlockNum*NVM_OFFSET,(Par*32u) >> 3u,4u);
            }
            for (global_uint=0u; global_uint < ParSize ;global_uint++)
            {
                #ifdef ENABLE_DEBUG
                if ((global_uint % 100u) == 0u)
                {
                    dp_display_text(".");
                }
                #endif
                
                if (((global_uint % NVM_DATA_LENGTH) == 0u) && global_uint)
                {
                    PageAddress++;
                    for (global_uchar = 0u; global_uchar < 32u; global_uchar ++)
                    {
                        opcode = NVM_READ;
                        IRSCAN_in();
                        DRSCAN_in(0u, NVM_ADDRESS_LENGTH, (DPUCHAR*)DPNULL);
                        goto_jtag_state(JTAG_RUN_TEST_IDLE,NVM_READ_CYCLES);
                    }
                }
                
                dp_flush_global_buf2();
                
                /* global_buf2 holds the starting address of the NVM partition */
                /* Bit 7 needs to be replaced. */
                global_buf2[1] |= 0x10u;				/* 32 Bit word addressing */
                global_buf2[1] &= 0x7Fu;				/* This is to maintaing bit values 13 and 14 */
                global_buf2[1] |= (DPUCHAR) (offset << 7u);		/* Overwrite offset */
                global_buf2[2] = offset >> 1u;
                global_buf2[2] &= 0xfu;	        /* Overwrite offset */
                global_buf2[2] |= (DPUCHAR) (PageAddress << 5u);	    /* Overwrite PageAddress */
                global_buf2[3] = (DPUCHAR)(PageAddress >> 3u);	/* Overwrite PageAddress */
                global_buf2[4] = (DPUCHAR)(PageAddress >> 11u);	/* Overwrite PageAddress */
                
                /* Load NVM Address */
                opcode = NVM_ADDRESS_SHIFT;
                IRSCAN_in();
                DRSCAN_in(0u, NVM_ADDRESS_LENGTH, global_buf2);
                goto_jtag_state(JTAG_RUN_TEST_IDLE,NVM_ADDRESS_SHIFT_CYCLES);
                dp_delay(NVM_ADDRESS_SHIFT_DELAY);
                
                /* Perform address polling */
                for ( global_ulong = 0u; global_ulong <= MAX_LOAD_NVM_ADDRESS_POLL; global_ulong++ )
                {
                    dp_delay(NVM_ADDRESS_SHIFT_POLL_DELAY);
                    DRSCAN_out(NVM_ADDRESS_LENGTH, global_buf2, global_buf1);
                    if ((global_buf1[4] & 0x6u) == 0u)
                    {
                        break;
                    }
                }
                if (global_ulong > MAX_LOAD_NVM_ADDRESS_POLL)
                {
                    error_code = DPE_POLL_ERROR;
                    /* This is needed to terminate the first loop */
                    Par = NumOfPart;
                    break;
                }
                
                
                /* Read the Data */
                if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
                { 
                    TmpData = dp_get_bytes(NvmDataPriv_all_ID, DataIndex >> 3u, 4u);
                }
                else
                {
                    TmpData = dp_get_bytes(NvmData_ID+BlockNum*NVM_OFFSET, DataIndex >> 3u, 4u);
                }
                opcode = NVM_READ;
                IRSCAN_in();
                DRSCAN_in(0u, NVM_ADDRESS_LENGTH, (DPUCHAR*)DPNULL);
                goto_jtag_state(JTAG_RUN_TEST_IDLE,NVM_READ_CYCLES);
                dp_delay(NVM_READ_DELAY);
                
                for ( global_ulong = 0u; global_ulong <= MAX_NVM_READ_POLL; global_ulong++ )
                {
                    DRSCAN_out(NVM_ADDRESS_LENGTH, (DPUCHAR*)DPNULL, global_buf1);
                    
                    if (TmpData == ( ((DPULONG)global_buf1[0] | 
                    ((DPULONG)global_buf1[1] << 8u) | 
                    ((DPULONG)global_buf1[2] << 16u) | 
                    ((DPULONG)global_buf1[3] << 24u) ) ) )
                    {
                        break;
                    }
                }
                
                /* Second contion is to check for NVM_status condition of 10 where two or more errors are detected */
                if ((global_ulong > MAX_LOAD_NVM_ADDRESS_POLL) || ((global_buf1[4] & 0x3) == 0x2))
                {
                    error_code = DPE_POLL_ERROR;
                    /* This is needed to terminate the first loop */                
                    Par = NumOfPart;
                    global_uint = ParSize;
                }
                
                
                offset ++;
                DataIndex = DataIndex + 32u;
            }
            
        }
    }
    return;
}


void dp_verify_calibration(void)
{	
    DPUINT StartPageAddress;
    DPUINT EndPageAddress;
    DPUCHAR offset;
    
    #ifdef ENABLE_DEBUG
    dp_display_text("\r\nChecking for Calibration data... ");
    #endif
    
    global_buf2[0] = NVM_TAG_ADDRESS_BYTE0;
    global_buf2[1] = NVM_TAG_ADDRESS_BYTE1;
    global_buf2[2] = NVM_TAG_ADDRESS_BYTE2;
    global_buf2[3] = NVM_TAG_ADDRESS_BYTE3;
    global_buf2[4] = NVM_TAG_ADDRESS_BYTE4;
    
    /* Load NVM TAG Address */
    opcode = NVM_ADDRESS_SHIFT;
    IRSCAN_in();
    DRSCAN_in(0u, NVM_ADDRESS_LENGTH, global_buf2);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,NVM_ADDRESS_SHIFT_CYCLES);
    dp_delay(NVM_ADDRESS_SHIFT_DELAY);
    
    for ( global_ulong = 0u; global_ulong <= MAX_LOAD_NVM_ADDRESS_POLL; global_ulong++ )
    {
        dp_delay(NVM_ADDRESS_SHIFT_POLL_DELAY);
        DRSCAN_out(NVM_ADDRESS_LENGTH, global_buf2, global_buf1);
        if ((global_buf1[4] & 0x6u) == 0u)
        {
            break;
        }
    }
    if (global_ulong > MAX_LOAD_NVM_ADDRESS_POLL)
    {
        error_code = DPE_POLL_ERROR;
    }
    else 
    {
        /* Read the Data */
        opcode = NVM_READ;
        IRSCAN_in();
        DRSCAN_in(0u, NVM_ADDRESS_LENGTH, (DPUCHAR*)DPNULL);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,NVM_READ_CYCLES);
        dp_delay(NVM_READ_DELAY);
        dp_flush_global_buf1();
        DRSCAN_out(NVM_ADDRESS_LENGTH, (DPUCHAR*)DPNULL, global_buf1);
        if ( (global_buf1[0] == (DPUCHAR)'C') && ((global_buf1[1] == (DPUCHAR)'A') || (global_buf1[1] == (DPUCHAR)'B')) )
        {
            device_family |= CALIBRATION_BIT;
            #ifdef ENABLE_DEBUG
            dp_display_text("\r\nDevice is calibrated...");
            #endif
        }
        
        
        
        if (device_family & CALIBRATION_BIT)
        {
            
            NumOfPart = (DPUINT) dp_get_bytes(NumOfPart_ID,0u,2u);
            DataIndex = 0u;
            
            /* Go through all the partions in the file */
            for (Par=0u;Par < NumOfPart; Par++)
            {
                global_ulong = dp_get_bytes(NvmAddr_ID,(Par*22u) >> 3u,(DPUCHAR)((((Par*22u) & 7u) + 22u + 7u) >> 3u)) >> ((Par*22u) & 7u) ;
                StartPageAddress = global_ulong >> 8u; /* 14 bit long */
                offset = (DPUCHAR) (global_ulong & 0x7fu);
                
                /* Get partition size */
                ParSize = dp_get_bytes(NvmParSize_ID,(Par*32u) >> 3u,4u);
                EndPageAddress = StartPageAddress + ((ParSize + offset + 31u) / 32u ) - 1u;
                if ( (EndPageAddress >= 2098u) && (StartPageAddress <=2110u) )
                {
                    error_code = CALIBRATION_OVERLAP_ERROR;
                    #ifdef ENABLE_DEBUG
                    dp_display_text("\r\nCalibration overlap detected...");
                    #endif
                }
            }
        }
    }
    return;
}
#endif
/************************************************************************************************/

/************************************************************************************************/
/* Encrypted Programming Support Functions                                                      */
/************************************************************************************************/
#ifdef NVM_ENCRYPT
void dp_enc_program_nvm_block(DPUCHAR BlockNum)
{
    DPUINT NvmProtIndex=0u;
    DPUINT i;
    DPUINT j;
    #ifdef USE_PAGING
    DPUCHAR NvmProtData[PAGE_BUFFER_SIZE];
    DPULONG NvmProtDataStartIndex;
    DPULONG NvmProtDataEndIndex;
    DPULONG NvmProtDataCurrentIndex;
    #endif    
    
    /* Get how many partitions are in the code */
    /* Need to identify if the data is present for the private client support on smart fusion since
    there is no dedicated support bit for it to indicate if the data is present in the file or not. */
    if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
    { 
        NumOfPart = (DPUINT) dp_get_bytes(NumOfPartPriv_all_ID,0u,2u);
    }
    else
    {
        NumOfPart = (DPUINT) dp_get_bytes(NumOfPart_ID+BlockNum*NVM_OFFSET,0u,2u);
    }
    
    if (NumOfPart)
    {
        #ifdef ENABLE_DEBUG
        if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
        {
            dp_display_text("\r\nProgram System Init and Boot Clients...");
        }
        else
        {
            dp_display_text("\r\nProgramming NVM block ");
            dp_display_value((DPULONG)BlockNum,DEC);
        }
        #endif
        
        if (device_family & DUAL_KEY_BIT)
        {
            global_uchar = 0u;
            dp_set_aes_mode();
        }
        
        DataIndex = 0u;
        
        #ifdef USE_PAGING
        NvmProtDataStartIndex = 0u;
        NvmProtDataEndIndex = 0u;
        #endif
        
        for (Par=0u;Par < NumOfPart; Par++)
        {
            if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
            { 
                ParSize = dp_get_bytes(NvmParSizePriv_all_ID,(Par*32u) >> 3u,4u);
            }
            else
            {
                ParSize = dp_get_bytes(NvmParSize_ID+BlockNum*NVM_OFFSET,(Par*32u) >> 3u,4u);
            }
            
            dp_init_aes();
            
            ParSize /= 32u;
            for (i = 1u; i <= ParSize; i++)
            {
                #ifdef ENABLE_DEBUG
                if ((i % 100u) == 0u)
                {
                    dp_display_text(".");
                }
                #endif
                
                /* Another buffer is needed to minimize paging. This buffer contains
                page protection data */
                #ifdef USE_PAGING
                /* if the local protection buffer is empty, fill it.  This step needs to  
                be done here.  This is because if data is filled first, it will be dumpped 
                after loading the first page. */
                if ( (NvmProtDataStartIndex == 0u) && (NvmProtDataEndIndex == 0u) )
                {
                    if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
                    {
                        dp_get_data(NvmProtectPriv_all_ID,NvmProtIndex);                                                                
                    }
                    else
                    {
                        dp_get_data(NvmProtect_ID + BlockNum*NVM_OFFSET,NvmProtIndex);                                              
                    }
                    for (NvmProtDataCurrentIndex = 0u; NvmProtDataCurrentIndex < return_bytes; NvmProtDataCurrentIndex++)
                    {
                        NvmProtData[NvmProtDataCurrentIndex] = page_global_buffer[NvmProtDataCurrentIndex];
                    }
                    NvmProtDataStartIndex = NvmProtIndex / 8;
                    NvmProtDataEndIndex = NvmProtDataStartIndex + return_bytes;
                }
                #endif
                for (j=0u; j <= 15u;j++)
                {
                    opcode = NVM_DATA_SHIFT_ENC;
                    IRSCAN_in();
                    if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
                    {
                        dp_get_and_DRSCAN_in_out(NvmDataPriv_all_ID,128u,DataIndex,global_buf1);                  
                    }
                    else
                    {
                        dp_get_and_DRSCAN_in_out(NvmData_ID + BlockNum*NVM_OFFSET,128u,DataIndex,global_buf1);
                    }
                    goto_jtag_state(JTAG_RUN_TEST_IDLE,NVM_DATA_SHIFT_ENC_CYCLES);
                    dp_delay(NVM_DATA_SHIFT_ENC_DELAY);
                    DataIndex = DataIndex + 128u;
                    /* Ignore the first shift out.  It is not valid */
                    if ((j != 0u) && (global_buf1[0] & 0x13u))
                    {
                        #ifdef ENABLE_DEBUG                      
                        dp_display_text("\n\rEmbedded Flash Memory Block (EFMB) failure.");
                        #endif
                        error_code = DPE_NVM_PROGRAM_ERROR;
                        break;
                    }
                }
                /* Checking the status of the last 15 bytes loaded into eNVM */
                DRSCAN_out(128u,(DPUCHAR *)DPNULL, global_buf1);
                if (global_buf1[0] & 0x13u)
                {
                    #ifdef ENABLE_DEBUG                      
                    dp_display_text("\n\rEmbedded Flash Memory Block (EFMB) failure.");
                    #endif
                    error_code = DPE_NVM_PROGRAM_ERROR;
                }
                
                if (error_code == DPE_SUCCESS)
                {
                    opcode = NVM_PROGRAM_ENC;
                    IRSCAN_in();
                    
                    
                    
                    #ifdef USE_PAGING
                    if ( ( (NvmProtIndex / 8u) + 16u) >  NvmProtDataEndIndex )
                    {
                        if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
                        {
                            dp_get_data(NvmProtectPriv_all_ID,NvmProtIndex);                                                                        
                        }
                        else
                        {
                            dp_get_data(NvmProtect_ID + BlockNum*NVM_OFFSET,NvmProtIndex);                                                  
                        }
                        for (NvmProtDataCurrentIndex = 0u; NvmProtDataCurrentIndex < return_bytes; NvmProtDataCurrentIndex++)
                        {
                            NvmProtData[NvmProtDataCurrentIndex] = page_global_buffer[NvmProtDataCurrentIndex];
                        }
                        NvmProtDataStartIndex = NvmProtIndex / 8u;
                        NvmProtDataEndIndex = NvmProtDataStartIndex + return_bytes;
                    }

                    DRSCAN_out(128u,&NvmProtData[NvmProtIndex / 8u - NvmProtDataStartIndex],global_buf1);
                    #else 
                    if (BlockNum == PRIVATE_CLIENT_PHANTOM_BLOCK)
                    {
                        dp_get_and_DRSCAN_in_out(NvmProtectPriv_all_ID,128u,NvmProtIndex,global_buf1);                                                    
                    }
                    else
                    {
                        dp_get_and_DRSCAN_in_out(NvmProtect_ID + BlockNum*NVM_OFFSET,128u,NvmProtIndex,global_buf1);                                  
                    }
                    #endif                
                    goto_jtag_state(JTAG_RUN_TEST_IDLE,NVM_PROGRAM_ENC_CYCLES);
                    dp_delay(NVM_PROGRAM_POLL_DELAY);                    
                    
                    for ( global_ulong = 1u; global_ulong <= MAX_LOAD_NVM_ADDRESS_POLL; global_ulong++ )
                    {
                        DRSCAN_out(128u,(DPUCHAR *)DPNULL,global_buf1);
                        if (( global_buf1[0] & 0x3u ) == 0u )
                        {
                            break;
                        }
                        goto_jtag_state(JTAG_PAUSE_DR,NVM_PROGRAM_ENC_CYCLES);
                        dp_delay(NVM_PROGRAM_POLL_DELAY);
                        opcode = NVM_PROGRAM_ENC;
                        IRSCAN_in();
                    }
                    
                    if (global_ulong > MAX_LOAD_NVM_ADDRESS_POLL)
                    {
                        #ifdef ENABLE_DEBUG                      
                        dp_display_text("\n\rError programming Embedded Flash Memory Block (EFMB).");
                        #endif
                        
                        error_code = DPE_POLL_ERROR;
                        Par = NumOfPart;
                        i = ParSize;
                    }
                    
                    #ifdef ENABLE_DEBUG
                    /* Check for programming cycle over count */
                    if ((global_buf1[0] & 0xcu) == 0xcu)
                    {
                        dp_display_text("\r\nWARNING: Write count threshold exceeded. ");
                    }
                    #endif
                    
                    /* Check for MAC bit */
                    dp_exe_authentication();
                    if (error_code != DPE_SUCCESS)
                    {
                        #ifdef ENABLE_DEBUG                      
                        dp_display_text("\n\rEmbedded Flash Memory Block (EFMB) MAC failure.");
                        #endif
                        error_code = DPE_NVM_PROGRAM_ERROR;
                        Par = NumOfPart;
                        i = ParSize;
                    }
                    /* Check for cycle overcount */
                    
                    NvmProtIndex = NvmProtIndex + 128u;
                }
            }
        }
    }
    return;
}

#endif
void dp_initialize_access_nvm(void)
{
    DPUCHAR mask;
    
    /* global_uchar is a global variable used to clock the data into the device with access_nvm instruction */
    global_uint = (dat_support_status >> 3u) & 0xFu; 
    mask = (DPUCHAR) global_uint;
    global_uint |= FORCE_NVM_ACCESS << 4u;
    
    /* Load access_nvm instruction and data */
    opcode = ACCESS_NVM;
    IRSCAN_in();
    DRSCAN_in(0u, ACCESS_NVM_BIT_LENGTH, (DPUCHAR *) (&global_uint));    
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ACCESS_NVM_CYCLES);
    dp_delay(ACCESS_NVM_DELAY);
    
    error_code = DPE_NVM_ACCESS_ERROR;
    for ( global_ulong = 0u; global_ulong <= MAX_ATTEMPT_NVM_ACCESS; global_ulong++ )
    {
        DRSCAN_out(ACCESS_NVM_BIT_LENGTH, (DPUCHAR*)DPNULL, &global_uchar);
        if ((global_uchar & mask) != 0u )
        {
            dp_delay(ACCESS_NVM_POLL_DELAY);
            opcode = ACCESS_NVM;
            IRSCAN_in();
            DRSCAN_in(0u, ACCESS_NVM_BIT_LENGTH, (DPUCHAR *) (&global_uint));
            goto_jtag_state(JTAG_RUN_TEST_IDLE,ACCESS_NVM_CYCLES);
            dp_delay(ACCESS_NVM_DELAY);
        }
        else 
        {
            error_code = DPE_SUCCESS;
            break;
        }
    }
    
    if (error_code != DPE_SUCCESS)
    {
        dp_exit_access_nvm();
        #ifdef ENABLE_DEBUG
        dp_display_text("\r\nFailed to access Embedded Flash Memory. \r\nnvmBusy: ");
        dp_display_value(global_uchar,HEX);
        dp_display_text("\r\nnvmAccess: ");
        dp_display_value(global_uint,HEX);
        #endif
    }
    
    
    #ifdef NVM_ENCRYPT    
    if (error_code == DPE_SUCCESS)
    {
        if (
        ( Action_code == DP_PROGRAM_NVM_ACTIVE_ARRAY_ACTION_CODE ) || 
        ( Action_code == DP_VERIFY_NVM_ACTIVE_ARRAY_ACTION_CODE ) ||
        ( Action_code == DP_PROGRAM_PRIVATE_CLIENTS_ACTIVE_ARRAY_ACTION_CODE ) || 
        ( Action_code == DP_VERIFY_PRIVATE_CLIENTS_ACTIVE_ARRAY_ACTION_CODE )
        )
        {
            
            global_uchar = 0u;
            /* This is to determine if there is a valid kdata in the data file.
            In the case of Dual key plain programming, the encryption status bits are set 
            in the DirectC code to use the encryption functions.  In this case, kdata 
            verification is called although it does not exit. */
            for (global_uint = 0u; global_uint < 16u; global_uint++)
            {
                global_uchar |= (DPUCHAR) dp_get_bytes(KDATA_ID,(DPULONG)global_uint,1u);
            }
            
            
            if (return_bytes && (global_uchar != 0u))
            {
                dp_verify_enc_key();
            }
        }
    }
    #endif
    return;
}

void dp_exit_access_nvm(void)
{
    opcode = ACCESS_NVM;
    IRSCAN_in();
    DRSCAN_in(0u, ACCESS_NVM_BIT_LENGTH, (DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE, ACCESS_NVM_CYCLES);
    dp_delay(ACCESS_NVM_DELAY);
    return;
}

#endif
