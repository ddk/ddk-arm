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

#ifndef INC_DPUTIL_H
#define INC_DPUTIL_H

#ifndef ENABLE_CODE_SPACE_OPTIMIZATION
extern DPUCHAR dat_version;
#define V85_DAT 1u
#endif

void dp_flush_global_buf1(void);
void dp_flush_global_buf2(void);
void dp_init_vars(void);
/* Function used to identify which block is supported in the dat file and their encryption status. */
void dp_get_dat_support_status(void);
void dp_compute_crc(void);
void dp_check_image_crc(void);
void dp_check_dat_support_version(void);

#endif /* INC_DPUTIL_H */


