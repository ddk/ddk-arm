/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpchain.c                                               */
/*                                                                          */
/*  Description:    Contains chain functions                                */
/*                                                                          */
/* ************************************************************************ */

#include "dpuser.h"
#include "dpchain.h"
#include "dpjtag.h"
#include "dpcom.h"

#ifdef CHAIN_SUPPORT
/* *****************************************************************************
* Variable that must be intialized with appropriate data depending on the chain
* configuration.  See user guide for more information.
*******************************************************************************/
DPUCHAR dp_preir_data[PREIR_DATA_SIZE];
DPUCHAR dp_predr_data[PREDR_DATA_SIZE];
DPUCHAR dp_postir_data[POSTIR_DATA_SIZE];
DPUCHAR dp_postdr_data[POSTDR_DATA_SIZE];

DPUINT dp_preir_length = PREIR_LENGTH_VALUE;
DPUINT dp_predr_length = PREDR_LENGTH_VALUE;
DPUINT dp_postir_length = POSTIR_LENGTH_VALUE;
DPUINT dp_postdr_length = POSTDR_LENGTH_VALUE;

/****************************************************************************
* Purpose: clock data stored in tdi_data into the device.
* terminate is a flag needed to determine if shifting to pause state should 
* be done with the last bit shift.
****************************************************************************/
void dp_shift_in(DPULONG start_bit, DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR terminate)
{
    if (current_jtag_state == JTAG_SHIFT_IR)
    {
        if (dp_preir_length > 0U)
        {
            dp_do_shift_in(0U,dp_preir_length, dp_preir_data,0U);
        }
        if (dp_postir_length > 0U)
        {
            dp_do_shift_in(start_bit,num_bits, tdi_data,0U);
            dp_do_shift_in(0U,dp_postir_length, dp_postir_data, terminate);
        }
        else
        {
            dp_do_shift_in(start_bit,num_bits, tdi_data,terminate);
        }	
    }
    else if (current_jtag_state == JTAG_SHIFT_DR)
    {
        if (dp_predr_length > 0U)
        {
            dp_do_shift_in(0U,dp_predr_length, dp_predr_data,0U);
        }
        if (dp_postdr_length > 0U)
        {
            dp_do_shift_in(start_bit,num_bits, tdi_data,0U);
            dp_do_shift_in(0U,dp_postdr_length, dp_postdr_data, terminate);
        }
        else
        {
            dp_do_shift_in(start_bit,num_bits, tdi_data,terminate);
        }
    }
    else
    {
    }
    return;
}
/****************************************************************************
* Purpose:  clock data stored in tdi_data into the device.
*           capture data coming out of tdo into tdo_data.
* This function will always clock data starting bit postion 0.  
* Jtag state machine will always set the pauseDR or pauseIR state at the 
* end of the shift.
****************************************************************************/
void dp_shift_in_out(DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR tdo_data[])
{
    if (current_jtag_state == JTAG_SHIFT_IR)
    {
        if (dp_preir_length > 0U)
        {
            dp_do_shift_in(0U,dp_preir_length, dp_preir_data,0U);
        }
        if (dp_postir_length > 0U)
        {
            dp_do_shift_in_out(num_bits, tdi_data,tdo_data,0U);
            dp_do_shift_in(0U,dp_postir_length, dp_postir_data, 1U);
        }
        else
        {
            dp_do_shift_in_out(num_bits, tdi_data,tdo_data,1U);
        }	
    }
    else if (current_jtag_state == JTAG_SHIFT_DR)
    {
        if (dp_predr_length > 0U)
        {
            dp_do_shift_in(0U,dp_predr_length, dp_predr_data,0U);
        }
        if (dp_postdr_length > 0U)
        {
            dp_do_shift_in_out(num_bits, tdi_data,tdo_data,0U);
            dp_do_shift_in(0U,dp_postdr_length, dp_postdr_data, 1U);
        }
        else
        {
            dp_do_shift_in_out(num_bits, tdi_data,tdo_data,1U);
        }
    }
    else
    {
    }
    return;
}


void dp_do_shift_in(DPULONG start_bit, DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR terminate)
{
    idx = (DPUCHAR) start_bit >> 3;
    bit_buf = 1U << (DPUCHAR)(start_bit & 0x7U);
    if (tdi_data == (DPUCHAR*)DPNULL)
    {
        data_buf = 0U;
    }
    else
    {
        data_buf = tdi_data[idx] >> ((DPUCHAR)(start_bit & 0x7U));
    }
    if (terminate == 0U)
    {
        num_bits++;
    }
    while (--num_bits)
    {
        dp_jtag_tms_tdi(0U, data_buf&0x1U);
        data_buf >>= 1;
        bit_buf <<= 1;
        if ((bit_buf & 0xffU) == 0U )
        {
            bit_buf = 1U;
            idx++;
            if (tdi_data == (DPUCHAR*)DPNULL)
            {
                data_buf = 0U;
            }
            else 
            {
                data_buf = tdi_data[idx];
            }
        }
    }
    if (terminate)
    {
        dp_jtag_tms_tdi(1U, data_buf & 0x1U);
        if (current_jtag_state == JTAG_SHIFT_IR)
        {
            current_jtag_state = JTAG_EXIT1_IR;
        }
        else if (current_jtag_state == JTAG_SHIFT_DR)
        {
            current_jtag_state = JTAG_EXIT1_DR;
        }
        else
        {
        }
    }
    return;
}

void dp_do_shift_in_out(DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR tdo_data[], DPUCHAR terminate)
{
    bit_buf = 1U;
    idx = 0U;
    tdo_data[idx] = 0U;
    
    if (tdi_data == (DPUCHAR*)DPNULL)
    {
        data_buf = 0U;
    }
    else 
    {   
        data_buf = tdi_data[idx];
    }
    
    while (--num_bits)
    {
        if ((bit_buf & 0xffU) == 0U )
        {
            bit_buf = 1U;
            idx++;
            tdo_data[idx] = 0U;
            if (tdi_data == (DPUCHAR*)DPNULL)
            {
                data_buf = 0U;
            }
            else 
            {
                data_buf = tdi_data[idx];
            }
        }
        if (dp_jtag_tms_tdi_tdo(0U, data_buf&0x1U))
        {
            tdo_data[idx] |= bit_buf;
        }
        bit_buf <<= 1;
        data_buf>>=1;
    }
    if ((bit_buf & 0xffU) == 0U )
    {
        bit_buf = 1U;
        idx++;
        tdo_data[idx] = 0U;
        if (tdi_data == (DPUCHAR*)DPNULL)
        {
            data_buf = 0U;
        }
        else 
        {
            data_buf = tdi_data[idx];
        }
    }
    if(terminate)
    {
        if (dp_jtag_tms_tdi_tdo(1U, data_buf&0x1U))
        {
            tdo_data[idx] |= bit_buf;
        }
        if (current_jtag_state == JTAG_SHIFT_IR)
        {
            current_jtag_state = JTAG_EXIT1_IR;
        }
        else if (current_jtag_state == JTAG_SHIFT_DR)
        {
            current_jtag_state = JTAG_EXIT1_DR;
        }
        else
        {
        }
    }
    else 
    {
        if (dp_jtag_tms_tdi_tdo(0U, data_buf&0x1U))
        {
            tdo_data[idx] |= bit_buf;
        }
    }
    return;
}
/****************************************************************************
* Purpose:  Gets the data block specified by Variable_ID from the image dat
* file and clocks it into the device.
****************************************************************************/
void dp_get_and_shift_in(DPUCHAR Variable_ID,DPUINT total_bits_to_shift, DPULONG start_bit_index)
{
    DPULONG page_start_bit_index;
    DPUINT bits_to_shift;
    DPUCHAR terminate;
    page_start_bit_index = start_bit_index & 0x7U;
    requested_bytes = (DPULONG) (page_start_bit_index + total_bits_to_shift + 7U) >> 3U;
    
    if (current_jtag_state == JTAG_SHIFT_IR)
    {
        if (dp_preir_length > 0U)
        {
            dp_do_shift_in(0U,dp_preir_length, dp_preir_data,0U);
        }
    }
    else if (current_jtag_state == JTAG_SHIFT_DR)
    {
        if (dp_predr_length > 0U)
        {
            dp_do_shift_in(0U,dp_predr_length, dp_predr_data,0U);
        }
    }
    else
    {
    }
    
    terminate = 0U;
    while (requested_bytes)
    {
        page_buffer_ptr = dp_get_data(Variable_ID,start_bit_index);
        
        if (return_bytes >= requested_bytes )
        {
            return_bytes = requested_bytes;
            bits_to_shift = total_bits_to_shift;
            terminate = 1U;
            if (((current_jtag_state == JTAG_SHIFT_IR) && dp_postir_length) || ((current_jtag_state == JTAG_SHIFT_DR) && dp_postdr_length))
            {
                terminate =0U;
            }
        }
        else 
        {
            bits_to_shift = (DPUCHAR) (return_bytes * 8U - page_start_bit_index);
        }
        dp_do_shift_in(page_start_bit_index, bits_to_shift, page_buffer_ptr,terminate);
        
        requested_bytes = requested_bytes - return_bytes;
        total_bits_to_shift = total_bits_to_shift - bits_to_shift;
        start_bit_index += bits_to_shift;
        page_start_bit_index = start_bit_index & 0x7U;
    }
    
    if (current_jtag_state == JTAG_SHIFT_IR)
    {
        if (dp_postir_length > 0U)
        {
            dp_do_shift_in(0U,dp_postir_length, dp_postir_data,1U);
        }
    }
    else if (current_jtag_state == JTAG_SHIFT_DR)
    {
        if (dp_postdr_length > 0U)
        {
            dp_do_shift_in(0U,dp_postdr_length, dp_postdr_data,1U);
        }
    }
    else
    {
    }
    return;
}

/****************************************************************************
* Purpose:  Get the data block specified by Variable_ID from the image dat
* file and clocks it into the device.  Capture the data coming out of tdo 
* into tdo_data
****************************************************************************/
void dp_get_and_shift_in_out(DPUCHAR Variable_ID,DPUCHAR total_bits_to_shift, DPULONG start_bit_index,DPUCHAR* tdo_data)
{
    requested_bytes = ((DPULONG)total_bits_to_shift + 7u) >> 3u;
    page_buffer_ptr = dp_get_data(Variable_ID,start_bit_index);
    
    if (return_bytes >= requested_bytes )
    {
        return_bytes = requested_bytes;
        dp_shift_in_out((DPUINT)total_bits_to_shift, page_buffer_ptr,tdo_data);
    }
    else
    {
        #ifdef ENABLE_DEBUG
        dp_display_text("\r\nError: Page buffer size is not big enough...");
        #endif
    }
    
    return;
}
#endif
