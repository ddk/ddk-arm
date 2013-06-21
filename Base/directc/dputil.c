/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dputil.c                                                */
/*                                                                          */
/*  Description:    Contains initialization and data checking functions.    */
/*                                                                          */
/* ************************************************************************ */

#include "dpuser.h"
#include "dpdef.h"
#include "dputil.h"
#include "dpalg.h"
#include "dpcom.h"
#include "dpjtag.h"

#ifndef ENABLE_CODE_SPACE_OPTIMIZATION
DPUCHAR dat_version;
#endif

void dp_flush_global_buf1(void)
{
    for (global_uchar=0u; global_uchar<global_buf_SIZE; global_uchar++)
    {
        global_buf1[global_uchar] = 0u;
    }
    return;
}

void dp_flush_global_buf2(void)
{
    for (global_uchar=0u; global_uchar<global_buf_SIZE; global_uchar++)
    {
        global_buf2[global_uchar]=0u;
    }
    return;
}

void dp_init_vars(void)
{
    #ifndef ENABLE_CODE_SPACE_OPTIMIZATION
    dat_version = 0u;
    #endif
    
    device_security_flags = 0u;
    error_code = DPE_SUCCESS;
    return;
}
void dp_get_dat_support_status(void)
{
    
    global_uint = 1u;
    dat_support_status = 0u;
    for (global_ulong = 0u;global_ulong < 16u ;global_ulong ++)
    {
        global_uchar=(DPUCHAR) dp_get_bytes( Header_ID,
        DAT_SUPPORT_STATUS_OFFSET + global_ulong,1u);
        if (global_uchar)
        {
            dat_support_status |= global_uint;
        }
        global_uint <<= 1u;
    }
    
    return;
}

void dp_check_dat_support_version(void)
{
    global_uchar = (DPUCHAR) dp_get_bytes(Header_ID, HEADER_SIZE_OFFSET, 1u);
    
    /* The header size of files generated from designer V8.6 is 69 vs. 56 from datgen and version 8.5 */
    if (global_uchar == 56u)
    {
        #ifdef ENABLE_CODE_SPACE_OPTIMIZATION
        #ifdef ENABLE_DEBUG
        dp_display_text("\r\nError: data file loaded is generated from Designer verion V8.5 or stand alone datgen utility. This version of the data file is not supported with ENABLE_CODE_SPACE_OPTIMIZATION compile switch enabled.  Disable ENABLE_CODE_SPACE_OPTIMIZATION compile switch in dpuser.h and try again.");
        #endif
        error_code = DPE_DAT_VERSION_MISMATCH;
        #else
        dat_version = V85_DAT;
        #endif
    }
    
}
/*
* Module: dp_check_image_crc
* 		purpose: Performs crc on the entire image.  
* Return value: 
* 	DPINT: User defined integer value which reports DPE_SUCCESS if there is a match or DPE_CRC_MISMATCH if failed. 
* 
*/
void dp_check_image_crc(void)
{
    DPUINT expected_crc;
    
    #ifdef ENABLE_DEBUG        
    dp_display_text("\r\nChecking data CRC...");
    #endif
    
    global_ulong = dp_get_bytes(Header_ID,0u,4u);
    if ( (global_ulong == 0x69736544u) || (global_ulong == 0x65746341u) )
    {
        requested_bytes = 0u;
        image_size = dp_get_bytes(Header_ID,IMAGE_SIZE_OFFSET,4u);
        expected_crc = (DPUINT) dp_get_bytes(Header_ID,image_size - 2u,2u);
        #ifdef ENABLE_DEBUG        
        dp_display_text("\r\nExpected CRC=");
        dp_display_value( expected_crc ,HEX);
        #endif
        if (image_size == 0u)
        {
            error_code = DPE_CRC_MISMATCH;
            #ifdef ENABLE_DEBUG
            dp_display_text("\r\nData file is not loaded... \r\n");
            #endif
        }
        else 
        {
            #ifdef ENABLE_DEBUG      
            dp_display_text("\r\nCalculating actual CRC...");
            #endif
            /* Global_uint is used to hold the value of the calculated CRC */
            global_uint = 0u;
            /* DataIndex is used to keep track the byte position in the image that is needed per get_data_request */
            DataIndex = 0u;
            requested_bytes = image_size - 2u;
            while (requested_bytes)
            {
                page_buffer_ptr = dp_get_data(Header_ID,DataIndex*8u);
                if (return_bytes > requested_bytes )
                    return_bytes = requested_bytes;
                for (global_ulong=0u; global_ulong< return_bytes;global_ulong++)
                {
                    global_uchar = page_buffer_ptr[global_ulong];
                    dp_compute_crc();
                }
                DataIndex += return_bytes;
                requested_bytes -= return_bytes;
            }
            if (global_uint != expected_crc)
            {
                #ifdef ENABLE_DEBUG
                dp_display_text("\r\nCRC verification failed.  Expected CRC = ");
                dp_display_value(global_uint,HEX);
                dp_display_text(" Actual CRC = ");
                dp_display_value((DPUINT) dp_get_bytes(Header_ID,image_size - 2,2),HEX);
                dp_display_text("\r\n");
                #endif
                error_code = DPE_CRC_MISMATCH;
            }
        }
    }
    else
    {
        #ifdef ENABLE_DEBUG
        dp_display_text("\r\nData file is not valid. ");
        #endif
        error_code = DPE_CRC_MISMATCH;
    }
    
    return;
}


void dp_compute_crc(void)
{
    for (global_jtag_i = 0u; global_jtag_i < 8u; global_jtag_i++)
    {
        device_rows = (global_uchar ^ global_uint) & 0x01u;
        global_uint >>= 1u;
        if (device_rows)
        {
            global_uint ^= 0x8408u;
        }
        global_uchar >>= 1u;
    }
    return;
}


