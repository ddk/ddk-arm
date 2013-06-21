/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpcom.h                                                 */
/*                                                                          */
/*  Description:    Contains functions prototypes needed to access the data */
/*  from external flash or other means of communication                     */
/*                                                                          */
/* ************************************************************************ */

#ifndef INC_DPCOM_H
#define INC_DPCOM_H

extern DPULONG return_bytes;
extern DPULONG image_size;
extern DPULONG requested_bytes;

/* user attention is required.  PAGE_BUFFER_SIZE needs to be specified in bytes */
#define PAGE_BUFFER_SIZE 16u
#define MIN_VALID_BYTES_IN_PAGE 16u

#ifdef USE_PAGING
extern DPUCHAR page_global_buffer[PAGE_BUFFER_SIZE];  /* Page_global_buffer simulating the global_buf1fer that is accessible by DirectC code*/
#endif

/* 
* Data block ID definitions
*/
#define Header_ID                 0u
#define ACT_UROW_DESIGN_NAME_ID   1u
#define BsrPattern_ID             2u  
#define SILSIG_ID                 3u		
#define CHECKSUM_ID               4u
#define datastream_ID             5u
#define rlock_ID                  6u
#define FRomAddressMask_ID        7u
#define FRomStream_ID             8u

/* These defintions are the same as NVM block zoro.  They are defined to aviod confusion when pogram and verify NVM functions are called. */
#define NVM_OFFSET                5u
#define NvmParSize_ID             9u 
#define NumOfPart_ID             10u
#define NvmAddr_ID               11u
#define NvmData_ID               12u
#define NvmProtect_ID            13u


#define NvmParSize_0_ID           9u 
#define NumOfPart_0_ID           10u
#define NvmAddr_0_ID             11u
#define NvmData_0_ID             12u
#define NvmProtect_0_ID          13u
#define NvmParSize_1_ID          14u
#define NumOfPart_1_ID           15u
#define NvmAddr_1_ID             16u
#define NvmData_1_ID             17u
#define NvmProtect_1_ID          18u
#define NvmParSize_2_ID          19u
#define NumOfPart_2_ID           20u
#define NvmAddr_2_ID             21u
#define NvmData_2_ID             22u
#define NvmProtect_2_ID          23u
#define NvmParSize_3_ID          24u
#define NumOfPart_3_ID           25u
#define NvmAddr_3_ID             26u
#define NvmData_3_ID             27u
#define NvmProtect_3_ID          28u
#define UKEY_ID                  29u
#define DMK_ID                   30u
#define KDATA_ID                 31u
#define ULOCK_ID                 32u
#define NvmParSizePriv_all_ID    33u
#define NumOfPartPriv_all_ID     34u
#define NvmAddrPriv_all_ID       35u
#define NvmDataPriv_all_ID       36u
#define NvmProtectPriv_all_ID    37u

/*
* Location of special variables needed in the header section of the image file
*/
#define BTYES_PER_TABLE_RECORD    9u
#define ACTEL_HEADER_SIZE        24u
#define HEADER_SIZE_OFFSET       24u
#define IMAGE_SIZE_OFFSET        25u
#define MIN_IMAGE_SIZE           56u
#define M_DEVICE_OFFSET          30u
#define ID_OFFSET                31u

#define DEVICE_FAMILY_OFFSET     55u
#define DEVICE_NAME_OFFSET       56u
#define ID_MASK_OFFSET           57u
#define SD_TILES_OFFSET          61u
#define MAP_ROWS_OFFSET          62u
#define BSR_LENGTH_OFFSET        64u
#define SE_WAIT_OFFSET           66u
#define DUAL_KEY_SUPPORT_OFFSET  67u

DPUCHAR * dp_get_data(DPUCHAR var_ID,DPULONG bit_index);
DPUCHAR * dp_get_header_data(DPULONG bit_index);
void dp_get_page_data(DPULONG image_requested_address);
void dp_get_data_block_address(DPUCHAR requested_var_ID);
DPUCHAR * dp_get_data_block_element_address(DPULONG bit_index);
DPULONG dp_get_bytes(DPUCHAR var_ID,DPULONG byte_index,DPUCHAR bytes_requested);
DPULONG dp_get_header_bytes(DPULONG byte_index,DPUCHAR bytes_requested);
#endif /* INC_DPCOM_H */


