/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpchain.h                                               */
/*                                                                          */
/*  Description:    Definitions of chain constants, and functions           */
/*                                                                          */
/* ************************************************************************ */

#ifndef INC_DPCHAIN_H
#define INC_DPCHAIN_H
#ifdef CHAIN_SUPPORT

#define PREIR_DATA_SIZE       1u
#define PREDR_DATA_SIZE       1u
#define POSTIR_DATA_SIZE      1u
#define POSTDR_DATA_SIZE      1u
#define PREIR_LENGTH_VALUE    0u
#define PREDR_LENGTH_VALUE    0u
#define POSTIR_LENGTH_VALUE   0u
#define POSTDR_LENGTH_VALUE   0u

extern DPUCHAR dp_preir_data[PREIR_DATA_SIZE];
extern DPUCHAR dp_predr_data[PREDR_DATA_SIZE];
extern DPUCHAR dp_postir_data[POSTIR_DATA_SIZE];
extern DPUCHAR dp_postdr_data[POSTDR_DATA_SIZE];

extern DPUINT dp_preir_length;
extern DPUINT dp_predr_length;
extern DPUINT dp_postir_length;
extern DPUINT dp_postdr_length;

void dp_do_shift_in(DPULONG start_bit, DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR terminate);
void dp_do_shift_in_out(DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR tdo_data[], DPUCHAR terminate);
#endif /* CHAIN_SUPPORT */
#endif /* INC_DPCHAIN_H */
