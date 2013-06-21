/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpjtag.h                                                */
/*                                                                          */
/*  Description:    Definitions of JTAG constants, types, and functions     */
/*                                                                          */
/* ************************************************************************ */

#ifndef INC_DPJTAG_H
#define INC_DPJTAG_H

/****************************************************************************/
/* JTAG states codes used to identify current and target JTAG states        */
/****************************************************************************/
#define JTAG_TEST_LOGIC_RESET	1u
#define JTAG_RUN_TEST_IDLE		2u
#define JTAG_SHIFT_DR		    3u
#define JTAG_SHIFT_IR		    4u
#define JTAG_EXIT1_DR		    5u
#define JTAG_EXIT1_IR		    6u
#define JTAG_PAUSE_DR		    7u
#define JTAG_PAUSE_IR		    8u
#define JTAG_UPDATE_DR		    9u
#define JTAG_UPDATE_IR		    10u
#define JTAG_CAPTURE_DR		    11u

/****************************************************************************/
/* Function prototypes                                                      */
/****************************************************************************/
void dp_wait_cycles(DPUCHAR cycles);
void goto_jtag_state(DPUCHAR target_state,DPUCHAR cycles);
void dp_shift_in(DPULONG start_bit, DPUINT num_bits, DPUCHAR tdi_data[],DPUCHAR terminate);
void dp_shift_in_out(DPUINT num_bits, DPUCHAR tdi_data[],DPUCHAR tdo_data[]);
void dp_get_and_shift_in(DPUCHAR Variable_ID,DPUINT total_bits_to_shift, DPULONG start_bit_index);
void dp_get_and_shift_in_out(DPUCHAR Variable_ID,DPUCHAR total_bits_to_shift, DPULONG start_bit_index,DPUCHAR* tdo_data);
void IRSCAN_in(void);
void IRSCAN_out(unsigned char* outbuf);
void DRSCAN_out(unsigned int bits_to_shift,unsigned char* inbuf,unsigned char* outbuf);
void DRSCAN_in(unsigned long start_bit_index, unsigned int bits_to_shift,unsigned char* inbuf);
void dp_get_and_DRSCAN_in(DPUCHAR Variable_ID,DPUINT total_bits_to_shift, DPULONG start_bit_index);
void dp_get_and_DRSCAN_in_out(DPUCHAR Variable_ID,DPUCHAR total_bits_to_shift, DPULONG start_bit_index,DPUCHAR* tdo_data);

extern DPUCHAR * page_buffer_ptr;
extern DPUCHAR global_jtag_i;
extern DPUCHAR current_jtag_state;
extern DPUCHAR idx;
extern DPUCHAR data_buf;
extern DPUCHAR bit_buf;
extern DPUCHAR error_code;


#endif /* INC_DPJTAG_H */
