/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpjtag.c                                                */
/*                                                                          */
/*  Description:    Contains JTAG interface functions                       */
/*                                                                          */
/* ************************************************************************ */

#include "dpuser.h"
#include "dpdef.h"
#include "dpjtag.h"
#include "dpcom.h"
#include "dpchain.h"
#include "dpalg.h"

DPUCHAR current_jtag_state;
DPUCHAR *page_buffer_ptr;
DPUCHAR global_jtag_i;
DPUCHAR idx;
DPUCHAR data_buf;
DPUCHAR bit_buf;

/****************************************************************************
* Purpose:  This function is used to shift JTAG states. 
****************************************************************************/
void IRSCAN_in(void)
{
    goto_jtag_state(JTAG_SHIFT_IR,0u);
    dp_shift_in(0u, OPCODE_BIT_LENGTH, &opcode, 1u);
    goto_jtag_state(JTAG_PAUSE_IR,0u);
    return;
}

void IRSCAN_out(unsigned char* outbuf)
{
    goto_jtag_state(JTAG_SHIFT_IR,0u);
    dp_shift_in_out(OPCODE_BIT_LENGTH, &opcode, outbuf);
    goto_jtag_state(JTAG_PAUSE_IR,0u);
    return;
}


void DRSCAN_out(unsigned int bits_to_shift,unsigned char* inbuf,unsigned char* outbuf)
{
    goto_jtag_state(JTAG_SHIFT_DR,0u);
    dp_shift_in_out(bits_to_shift, inbuf, outbuf);
    goto_jtag_state(JTAG_PAUSE_DR,0u);
    return;
}

void DRSCAN_in(unsigned long start_bit_index, unsigned int bits_to_shift,unsigned char* inbuf)
{
    goto_jtag_state(JTAG_SHIFT_DR,0u);
    dp_shift_in(start_bit_index, bits_to_shift, inbuf, 1u);
    goto_jtag_state(JTAG_PAUSE_DR,0u);
    return;
}


void dp_get_and_DRSCAN_in(DPUCHAR Variable_ID,DPUINT total_bits_to_shift, DPULONG start_bit_index)
{
    goto_jtag_state(JTAG_SHIFT_DR,0u);
    dp_get_and_shift_in(Variable_ID, total_bits_to_shift, start_bit_index);
    goto_jtag_state(JTAG_PAUSE_DR,0u);
    return;
}


void dp_get_and_DRSCAN_in_out(DPUCHAR Variable_ID,DPUCHAR total_bits_to_shift, DPULONG start_bit_index,DPUCHAR* tdo_data)
{
    goto_jtag_state(JTAG_SHIFT_DR,0u);
    dp_get_and_shift_in_out(Variable_ID, total_bits_to_shift, start_bit_index,tdo_data);
    goto_jtag_state(JTAG_PAUSE_DR,0u);
    return;
}

/****************************************************************************
* Purpose:  This function is used to shift JTAG states. 
****************************************************************************/
void goto_jtag_state(DPUCHAR target_state, DPUCHAR cycles)
{
    DPUCHAR count = 0u;
    DPUCHAR tms_bits = 0u;
    if (target_state != current_jtag_state)
    {
        switch (target_state) {
            
            case JTAG_TEST_LOGIC_RESET:
            dp_jtag_init();
            count = 5u;
            tms_bits = 0x1Fu;
            break;
            
            case JTAG_SHIFT_DR:
            if ((current_jtag_state == JTAG_TEST_LOGIC_RESET) || (current_jtag_state == JTAG_RUN_TEST_IDLE))
            {
                count = 4u;
                tms_bits = 0x2u;
            }
            else if ((current_jtag_state == JTAG_PAUSE_IR) || (current_jtag_state == JTAG_PAUSE_DR))
            {
                count = 5u;
                tms_bits = 0x7u;
            }
            else 
            {
            }
            break;
            
            case JTAG_SHIFT_IR:
            if ((current_jtag_state == JTAG_TEST_LOGIC_RESET) || (current_jtag_state == JTAG_RUN_TEST_IDLE))
            {
                count = 5u;
                tms_bits = 0x6u;
            }
            else if ((current_jtag_state == JTAG_PAUSE_DR) || (current_jtag_state == JTAG_PAUSE_IR))
            {
                count = 6u;
                tms_bits = 0xfu;
            }
            else if (current_jtag_state == JTAG_UPDATE_DR)
            {
                count = 4u;
                tms_bits = 0x3u;
            }
            else 
            {
            }
            break;
            
            case JTAG_RUN_TEST_IDLE:
            if (current_jtag_state == JTAG_TEST_LOGIC_RESET)
            {
                count = 1u;
                tms_bits = 0x0u;
            }
            else if ((current_jtag_state == JTAG_EXIT1_IR) || (current_jtag_state == JTAG_EXIT1_DR))
            {
                count = 2u;
                tms_bits = 0x1u;
            }
            else if ((current_jtag_state == JTAG_PAUSE_DR) || (current_jtag_state == JTAG_PAUSE_IR))
            {
                count = 3u;
                tms_bits = 0x3u;
            }
            else if (current_jtag_state == JTAG_CAPTURE_DR)
            {
                count = 3u;
                tms_bits = 0x3u;
            }
            else 
            {
            }
            break;
            
            case JTAG_PAUSE_IR:
            if (current_jtag_state == JTAG_EXIT1_IR)
            {
                count = 1u;
                tms_bits = 0x0u;
            }
            break;
            
            case JTAG_PAUSE_DR:
            if (current_jtag_state == JTAG_EXIT1_DR)
            {
                count = 1u;
                tms_bits = 0x0u;
            }
            else if (current_jtag_state == JTAG_RUN_TEST_IDLE)
            {
                count = 4u;
                tms_bits = 0x5u;
            }
            else 
            {
            }
            break;
            
            case JTAG_UPDATE_DR:
            if ((current_jtag_state == JTAG_EXIT1_DR) || (current_jtag_state == JTAG_EXIT1_IR))
            {
                count = 1u;
                tms_bits = 0x1u;
            }
            else if (current_jtag_state == JTAG_PAUSE_DR)
            {
                count = 2u;
                tms_bits = 0x3u;
            }
            else 
            {
            }
            break;
            
            case JTAG_CAPTURE_DR:
            if (current_jtag_state == JTAG_PAUSE_IR)
            {
                count = 5u;
                tms_bits = 0xeu;
            }
            else 
            {
            }
            break;
            
            default:
            error_code = DPE_JTAG_STATE_NOT_HANDLED;
            break;
        }
        
        for (global_jtag_i = 0u; global_jtag_i < count; global_jtag_i++)
        {
            dp_jtag_tms(tms_bits&0x1u);
            tms_bits >>= 1u;
        }
        current_jtag_state = target_state;
    }
    for (global_jtag_i=0u; global_jtag_i < cycles; global_jtag_i++) 
    {
        dp_jtag_tms(0u);
    }
    return;
}


void dp_wait_cycles(DPUCHAR cycles)
{
    for (global_jtag_i=0U; global_jtag_i < cycles; global_jtag_i++) 
    {
        dp_jtag_tms(0U);
    }
    return;
}

#ifndef CHAIN_SUPPORT
/****************************************************************************
* Purpose: clock data stored in tdi_data into the device.
* terminate is a flag needed to determine if shifting to pause state should 
* be done with the last bit shift.
****************************************************************************/
void dp_shift_in(DPULONG start_bit, DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR terminate)
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
/****************************************************************************
* Purpose:  clock data stored in tdi_data into the device.
*           capture data coming out of tdo into tdo_data.
* This function will always clock data starting bit postion 0.  
* Jtag state machine will always be set the pauseDR or pauseIR state at the 
* end of the shift.
****************************************************************************/
void dp_shift_in_out(DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR tdo_data[])
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
        bit_buf <<= 1U;
        data_buf >>= 1U;
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
    return;
}
/****************************************************************************
* Purpose:  Get the data block specified by Variable_ID from the image dat
* file and clocks it into the device.
****************************************************************************/
void dp_get_and_shift_in(DPUCHAR Variable_ID,DPUINT total_bits_to_shift, DPULONG start_bit_index)
{
    DPULONG page_start_bit_index;
    DPUINT bits_to_shift;
    DPUCHAR terminate;
    page_start_bit_index = start_bit_index & 0x7U;
    requested_bytes =  (page_start_bit_index + total_bits_to_shift + 7U) >> 3U;
    
    terminate = 0U;
    while (requested_bytes)
    {
        page_buffer_ptr = dp_get_data(Variable_ID,start_bit_index);
        
        if (return_bytes >= requested_bytes )
        {
            return_bytes = requested_bytes;
            bits_to_shift = total_bits_to_shift;
            terminate = 1U;
        }
        else 
        {
            bits_to_shift = (DPUCHAR) (return_bytes * 8U - page_start_bit_index);
        }
        dp_shift_in(page_start_bit_index, bits_to_shift, page_buffer_ptr,terminate);
        
        requested_bytes = requested_bytes - return_bytes;
        total_bits_to_shift = total_bits_to_shift - bits_to_shift;
        start_bit_index += bits_to_shift;
        page_start_bit_index = start_bit_index & 0x7u;
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
    requested_bytes =  ( ( (DPULONG) total_bits_to_shift ) + 7U) >> 3U;
    page_buffer_ptr = dp_get_data(Variable_ID,start_bit_index);
    
    if (return_bytes >= requested_bytes )
    {
        return_bytes = requested_bytes;
        dp_shift_in_out( (DPUINT) total_bits_to_shift, page_buffer_ptr,tdo_data);
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
