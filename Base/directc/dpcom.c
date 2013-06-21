/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpcom.c                                                 */
/*                                                                          */
/*  Description:    Contains functions for data table initialization &      */
/*  pointers passing of the various data blocks needed.                     */
/*                                                                          */
/* ************************************************************************ */
#include "dpuser.h"
#include "dpcom.h"
#include "dpdef.h"
#include "dpjtag.h"


/*
* Paging System Specific Implementation.  User attention required:
*/
extern unsigned long image_buffer; // = ddk_bitstream; /* Pointer to the image in the system memory */

#ifdef USE_PAGING
DPUCHAR page_global_buffer[PAGE_BUFFER_SIZE];  /* Page_global_buffer simulating the global_buf1fer that is accessible by DirectC code*/
DPULONG page_address_offset;
DPULONG start_page_address = 0u;
DPULONG end_page_address = 0u;
#endif

DPULONG return_bytes;
DPULONG requested_bytes;
/*
* current_block_address holds the data block starting address in the image file that the page global_buf1fer data is currently holding
*/
DPULONG current_block_address=0U;
DPUCHAR current_var_ID=Header_ID;
DPULONG image_size=MIN_IMAGE_SIZE;

/*
* Module: dp_get_data
*        purpose: This function is called only when access to the any data block within the image is needed.
*        It is designed to return the pointer address of the first byte containing bit_index in the data block 
*        requested by the var_identifier (var_ID). 
* Arguments: 
*        DPUCHAR * var_ID, an unsigned integer variable contains an identifier specifying which 
*        block address to return.
*        DPULONG bit_index: The bit location of the first bit to be processed (clocked in) within the data block specified by Var_ID.
* Return value: 
*        Address point to the first element in the block.
* 
*/
/*
DPUCHAR* dp_get_data(DPUCHAR var_ID,DPULONG bit_index)
{
    DPUCHAR * data_address = (DPUCHAR*)DPNULL;
    dp_get_data_block_address(var_ID);
    if ((current_block_address == 0U) && (var_ID != Header_ID))
    {
        return_bytes = 0U;
    }
    else
    {
        data_address = dp_get_data_block_element_address(bit_index);
    }
    return data_address;
}
*/

unsigned char* dp_get_data(DPUCHAR var_ID, DPULONG bit_index)
{
    unsigned long image_requested_address=0;
    unsigned long ret = 0;
    unsigned long base = image_buffer;


    if(var_ID == Header_ID)
        current_block_address = 0;

    else
        dp_get_data_block_address(var_ID);

    if( (current_block_address == 0) && (var_ID != Header_ID) ) {
        return_bytes = 0;
        return 0;
    }
    
    image_requested_address = (unsigned long)(current_block_address + (bit_index / 8));

    return_bytes = image_size - image_requested_address;
    ret = base + image_requested_address;

    //printf("D: dp_get_data:\nreturn_bytes = %x\nimage_size = %x\nimage_requested_address = %x\n",
    //    return_bytes, image_size, image_requested_address);
    
    //printf("image_buffer = %x\nret = %x\n",
    //    base, ret);

    return (unsigned char *)ret;

}



/*
* Module: dp_get_header_data
*        purpose: This function is called only when access to the header data block within the image is needed.
*        It is designed to return the pointer address of the first byte containing bit_index in the header block.
* Arguments: 
*        DPULONG bit_index: The bit location of the first bit to be processed (clocked in) within the data block specified by Var_ID.
* Return value: 
*        Address point to the first element in the block.
* 
*/
DPUCHAR* dp_get_header_data(DPULONG bit_index)
{
    /* The first block in the image is the header.  There is no need to get the address of its block.  It is zero. */
    current_block_address = 0U;
    
    /* Calculating the relative address of the data block needed within the image */
    return dp_get_data_block_element_address(bit_index);
}

/*
* User attention: 
* Module: dp_get_page_data
*        purpose: This function is called by dp_get_data function.  This is done every time a new data block is needed
*        or when requested data is not in the current page.  This function expects the location of the first byte to return
*        and will fill the entire page with valid data.
* Arguments: 
*        DPULONG image_requested_address, a ulong variable containing the relative address location of the first needed byte 
* Return value:
*        None.
* 
*/
#ifdef USE_PAGING
void dp_get_page_data(DPULONG image_requested_address)
{
    DPULONG image_address_index;
    start_page_address = 0u;
    
    image_address_index=image_requested_address;
    return_bytes = PAGE_BUFFER_SIZE;
    /* Image size will initially be the header size which is part of the image file. 
    * This must be done to avoid accessing data that is outside the image boundaries in case the 
    * page_global_buffer size is greater than the image itself
    * image_size variable will be read from the image file at a later time.
    */
    
    /* This is needed to avoid out of bound memory access*/
    if (image_requested_address + return_bytes > image_size)
    {
        return_bytes = image_size - image_requested_address;
    }
    
    while (image_address_index < image_requested_address + return_bytes)
    {
        /*
        Misra - C 2004 deviation:
        image_buffer is a pointer that is being treated as an array.  
        Refer to DirectC user guide for more information.
        */
        page_global_buffer[start_page_address] = image_buffer[image_address_index];
        start_page_address++;
        image_address_index++;
    }
    start_page_address = image_requested_address;
    end_page_address = image_requested_address + return_bytes - 1u;
    
    return;
}
#endif


/*
* Module: dp_get_data_block_address
*        purpose: This function sets current_block_address to the first relative address of the requested 
*       data block within the data file.
* Return value:
*        None.
*/
void dp_get_data_block_address(DPUCHAR requested_var_ID)
{
    DPUINT var_idx;
    DPULONG image_index;
    DPUINT num_vars;
    DPUCHAR variable_ID;    
    
    /* If the current data block ID is the same as the requested one, there is no need to caluclate its starting address.
    *  it is already calculated.
    */
    
    if (current_var_ID != requested_var_ID)
    {
        current_block_address=0U;
        current_var_ID=Header_ID;
        if (requested_var_ID != Header_ID)
        {
            /*The lookup table is at the end of the header*/
            image_index = dp_get_header_bytes((HEADER_SIZE_OFFSET),1U);
            image_size = dp_get_header_bytes(IMAGE_SIZE_OFFSET,4U);
            
            
            /* The last byte in the header is the number of data blocks in the dat file */
            num_vars = (DPUINT) dp_get_header_bytes(image_index-1U,1U);
            
            for (var_idx=0U;var_idx<num_vars;var_idx++)
            {
                variable_ID = (DPUCHAR) dp_get_header_bytes(image_index+BTYES_PER_TABLE_RECORD*var_idx,1U);
                if (variable_ID == requested_var_ID)
                {
                    current_block_address = dp_get_header_bytes(image_index+BTYES_PER_TABLE_RECORD*var_idx+1U,4U);
                    current_var_ID = variable_ID;
                    break;
                }
            }
        }
    }
    return;
}

/*
* Module: dp_get_data_block_element_address
*        purpose: This function return unsigned char pointer of the byte containing bit_index
*       within the data file.
* Return value:
*        DPUCHAR *: unsigned character pointer to the byte containing bit_index.
*/
DPUCHAR * dp_get_data_block_element_address(DPULONG bit_index)
{
    DPULONG image_requested_address;
    /* Calculating the relative address of the data block needed within the image */
    image_requested_address = current_block_address + bit_index / 8U;
    
    #ifdef USE_PAGING
    /* If the data is within the page, adjust the pointer to point to the particular element requested */
    /* For first load, start_page_address = end_page_address = 0u */
    if (
    (start_page_address != end_page_address) &&
    (image_requested_address >= start_page_address) && (image_requested_address <= end_page_address) &&
    ((image_requested_address + MIN_VALID_BYTES_IN_PAGE) <= end_page_address)  
    )
    {
        page_address_offset = image_requested_address - start_page_address;
        return_bytes = end_page_address - image_requested_address + 1u;
    }
    /* Otherwise, call dp_get_page_data which would fill the page with a new data block */
    else 
    {
        dp_get_page_data(image_requested_address);
        page_address_offset = 0u;
    }
    return &page_global_buffer[page_address_offset];
    #else
    return_bytes=image_size - image_requested_address;
    /*
    Misra - C 2004 deviation:
    image_buffer is a pointer that is being treated as an array.  
    Refer to DirectC user guide for more information.
    */
    return (unsigned char *)(image_buffer + image_requested_address);
    #endif
}


/*
* Module: dp_get_bytes
*        purpose: This function is designed to return all the requested bytes specified.  Maximum is 4.
*        DPINT * var_ID, an integer variable contains an identifier specifying which 
*        block address to return.
*        DPULONG byte_index: The relative address location of the first byte in the specified data block.
*        DPULONG requested_bytes: The number of requested bytes.
* Return value: 
*        DPULONG:  The combined value of all the requested bytes.
* Restrictions:
*        requested_bytes cannot exceed 4 bytes since DPULONG can only hold 4 bytes.
*
*/
DPULONG dp_get_bytes(DPUCHAR var_ID,DPULONG byte_index,DPUCHAR bytes_requested)
{
    DPULONG ret = 0U;
    DPUCHAR i;
    DPUCHAR j;
    j=0U;
    
    while (bytes_requested)
    {
        page_buffer_ptr = dp_get_data(var_ID,byte_index*8U);
        /* If Data block does not exist, need to exit */
        if (return_bytes == 0u)
        {
            break;
        }
        if (return_bytes > (DPULONG) bytes_requested )
        {      
            return_bytes = (DPULONG) bytes_requested;
        }
        for (i=0u; i < (DPUCHAR) return_bytes;i++)
        {
            /*
            Misra - C 2004 deviation:
            page_buffer_ptr is a pointer that is being treated as an array.  
            Refer to DirectC user guide for more information.
            */
            ret |= ((DPULONG) (page_buffer_ptr[i])) << ( j++ * 8U); 
        }
        byte_index += return_bytes;
        bytes_requested = bytes_requested - (DPUCHAR) return_bytes;
    }
    return ret;
}

/*
* Module: dp_get_header_bytes
*        purpose: This function is designed to return all the requested bytes specified from the header section.
*      Maximum is 4.
*        DPINT * var_ID, an integer variable contains an identifier specifying which 
*        block address to return.
*        DPULONG byte_index: The relative address location of the first byte in the specified data block.
*        DPULONG requested_bytes: The number of requested bytes.
* Return value: 
*        DPULONG:  The combined value of all the requested bytes.
* Restrictions:
*        requested_bytes cannot exceed 4 bytes since DPULONG can only hold 4 bytes.
*
*/
DPULONG dp_get_header_bytes(DPULONG byte_index,DPUCHAR bytes_requested)
{
    DPULONG ret = 0U;
    DPUCHAR i;
    DPUCHAR j=0u;
    
    while (bytes_requested)
    {
        page_buffer_ptr = dp_get_header_data(byte_index*8U);
        /* If Data block does not exist, need to exit */
        if (return_bytes == 0U)
        {
            break;
        }
        if (return_bytes > (DPULONG) bytes_requested )
        {
            return_bytes = (DPULONG) bytes_requested;
        }
        for (i=0u; i < (DPUCHAR)return_bytes; i++)
        {
            /*
            Misra - C 2004 deviation:
            page_buffer_ptr is a pointer that is being treated as an array.  
            Refer to DirectC user guide for more information.
            */
            ret |= (((DPULONG) page_buffer_ptr[i])) << ( j++ * 8U);            
        }
        byte_index += return_bytes;
        bytes_requested = bytes_requested - (DPUCHAR) return_bytes;
    }
    return ret;
}

