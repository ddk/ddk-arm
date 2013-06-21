/* ************************************************************************ */
/*                                                                          */
/*  DirectC         Copyright (C) Actel Corporation 2010                    */
/*  Version 2.7     Release date August 30, 2010                            */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpuser.c                                                */
/*                                                                          */
/*  Description:    user specific file containing JTAG interface functions  */
/*                  and delay function                                      */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>


#include "../../jtag_1149.h"

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#include "dpuser.h"
#include "dpalg.h"
#include "dpdef.h"
#include "dputil.h"

#include "led.h"
#include "helper.h"

extern unsigned int g_test_done;

/* 
* User attention:
* Include files needed to support hardware JTAG interface operations.
* 
*/
/* #include */

/* This variable is used to select external programming vs IAP programming */
DPUCHAR hardware_interface = GPIO_SEL;
DPUCHAR enable_mss_support = FALSE;

/*
 * User attention:
 * main should be replaced with customer main function.  It should do the following:
 * 1. Set the action_code variable as described in the user guide.
 * 2. Call dp_top function.
 * 
*/
int dp_test_main(void)
{
    int err = 0;
    
    Action_code = DP_READ_IDCODE_ACTION_CODE;
    err = dp_top();
    //printf("\nD: DP_READ_IDCODE_ACTION_CODE = %x\n", err);
    
    Action_code = DP_IS_CORE_CONFIGURED_ACTION_CODE;
    err = dp_top();
    //printf("\nD: DP_IS_CORE_CONFIGURED_ACTION_CODE = %x\n", err);

    //puts("");


    if (!(global_uchar & 0x4U)) {
        puts("[*] FPGA array not enabled.");
    } else {
        puts("[*] FPGA array is enabled.");
    }
    return DPE_SUCCESS;
}

int dp_erase_main(void)
{
    int err = 0;
    
    Action_code = DP_READ_IDCODE_ACTION_CODE;
    err = dp_top();

    
    Action_code = DP_IS_CORE_CONFIGURED_ACTION_CODE;
    err = dp_top();


    printf("\n");


    if (!(global_uchar & 0x4U)) {
        puts("[*] FPGA array not enabled.");
    } else {
        puts("[*] FPGA array is enabled. Erasing.");
        led3_set();
        Action_code = DP_ERASE_ACTION_CODE;
        err = dp_top();
        printf("\nD: DP_ERASE_ACTION_CODE = %x\n", err);
        led3_clr();
            
    }

    return DPE_SUCCESS;
}

int dp_prog_main(void)
{
    int err = 0;

    Action_code = DP_READ_IDCODE_ACTION_CODE;
    err = dp_top();


    Action_code = DP_IS_CORE_CONFIGURED_ACTION_CODE;
    err = dp_top();

    if (!(global_uchar & 0x4U)) {
        puts("[*] FPGA array not enabled. Writing FPGA bitstream to flash ROM.");
        led1_set();
        led4_set();
        Action_code = DP_PROGRAM_ACTION_CODE;
        err = dp_top();
        printf("D: DP_PROGRAM_ACTION_CODE = %x\n", err);
        led4_clr();
        g_test_done = 0;

    } else {
        puts("[*] FPGA array is enabled.");
        led1_set();
        led2_set();
        g_test_done = 1;
    }

    return DPE_SUCCESS;
}

int dp_force_prog_main(void)
{
    int err = 0;

    Action_code = DP_READ_IDCODE_ACTION_CODE;
    err = dp_top();

    Action_code = DP_IS_CORE_CONFIGURED_ACTION_CODE;
    err = dp_top();

    puts("[*] Writing FPGA bitstream to flash ROM");
    led1_set();
    led4_set();
    Action_code = DP_PROGRAM_ACTION_CODE;
    err = dp_top();
    printf("\nD: DP_PROGRAM_ACTION_CODE = %x\n", err);
    led4_clr();
    g_test_done = 0;

    return DPE_SUCCESS;
}

/*
* User attention (done):
* jtag_port_reg:    8 bit Static variable to keep track of the state of all the JTAG pins 
*                    at all times during the programming operation.
* Note: User check the variable size to make sure it can cover the hardware IO register. 
* 
*/
static DPUCHAR jtag_port_reg;
/*
* User attention (done): 
* Module: jtag_inp
*        purpose: report the logic state of tdo jtag pin
* Arguments: None
* Return value: 8 bit value
*        0, 0x80
* 
*/
DPUCHAR jtag_inp(void)
{
    DPUCHAR tdo = 0u;
    DPUCHAR ret = 0x80u;
    
    GPIO_SetDir(JTAG_TDOPORT, JTAG_TDO, 0);

    tdo = (GPIO_ReadValue(JTAG_TDOPORT) >> JTAG_TDOPIN) & 1;

    if (tdo)
        ret = 0x80;
    else
        ret = 0;

    return ret;
}
/*
* User attention (done): 
* Module: jtag_outp
*        purpose: Set the JTAG port (all JTAG pins)
* Arguments: 8 bit value containing the new state of all the JTAG pins
* Return value: None
* 
*/
void jtag_outp(DPUCHAR outdata)
{
    GPIO_SetDir(JTAG_TDOPORT, JTAG_TDO, 1);

    if(outdata & TCK)
        GPIO_SetValue(JTAG_TCKPORT, JTAG_TCK);
    else
        GPIO_ClearValue(JTAG_TCKPORT, JTAG_TCK);
    
    if(outdata & TDI)
        GPIO_SetValue(JTAG_TDIPORT, JTAG_TDI);
    else
        GPIO_ClearValue(JTAG_TDIPORT, JTAG_TDI);

    if(outdata & TMS)
        GPIO_SetValue(JTAG_TMSPORT, JTAG_TMS);
    else
        GPIO_ClearValue(JTAG_TMSPORT, JTAG_TMS);

    if(outdata & TRST)
        GPIO_SetValue(JTAG_TRSTPORT, JTAG_TRST);
    else
        GPIO_ClearValue(JTAG_TRSTPORT, JTAG_TRST);

    if(outdata & TDO)
        GPIO_SetValue(JTAG_TDOPORT, JTAG_TDO);
    else
        GPIO_ClearValue(JTAG_TDOPORT, JTAG_TDO);
    
    return;
}

/*
* No need to change this function
* Module: dp_jtag_init
*        purpose: Set tck and trstb pins to logic level one
* Arguments:
*        None
* Return value: None
* 
*/
void dp_jtag_init(void)
{
    jtag_port_reg = TCK | TRST; 
    jtag_outp(jtag_port_reg);
}

/*
* No need to change this function
* Module: dp_jtag_tms
*        purpose: Set tms pin to a logic level one or zero and pulse tck.
* Arguments: 
*        tms: 8 bit value containing the new state of tms
* Return value: None
* Constraints: Since jtag_outp function sets all the jtag pins, jtag_port_reg is used 
*                to modify the required jtag pins and preseve the reset.
* 
*/
void dp_jtag_tms(DPUCHAR tms)         
{    
    jtag_port_reg &= ~(TMS | TCK);
    jtag_port_reg |= (tms ? TMS : 0u);
    jtag_outp(jtag_port_reg);
    jtag_port_reg |= TCK;
    jtag_outp(jtag_port_reg);
}

/*
* No need to change this function
* Module: dp_jtag_tms_tdi
*        purpose: Set tms amd tdi pins to a logic level one or zero and pulse tck.
* Arguments: 
*        tms: 8 bit value containing the new state of tms
*        tdi: 8 bit value containing the new state of tdi
* Return value: None
* Constraints: Since jtag_outp function sets all the jtag pins, jtag_port_reg is used 
*                to modify the required jtag pins and preseve the reset.
* 
*/
void dp_jtag_tms_tdi(DPUCHAR tms, DPUCHAR tdi)
{
    jtag_port_reg &= ~(TMS | TCK | TDI);
    jtag_port_reg |= ((tms ? TMS : 0u) | (tdi ? TDI : 0u));
    jtag_outp(jtag_port_reg);
    jtag_port_reg |= TCK;
    jtag_outp(jtag_port_reg);
}

/*
* No need to change this function
* Module: dp_jtag_tms_tdi_tdo
*        purpose: Set tms amd tdi pins to a logic level one or zero, 
*                 pulse tck and return tdi level
* Arguments: 
*        tms: 8 bit value containing the new state of tms
*        tdi: 8 bit value containing the new state of tdi
* Return value: 
*        ret: 8 bit variable ontaining the state of tdo.
* Valid return values: 
*        0x80: indicating a logic level high on tdo
*        0: indicating a logic level zero on tdo
* Constraints: Since jtag_outp function sets all the jtag pins, jtag_port_reg is used 
*                to modify the required jtag pins and preseve the reset.
* 
*/
DPUCHAR dp_jtag_tms_tdi_tdo(DPUCHAR tms, DPUCHAR tdi)
{
    DPUCHAR ret = 0x80u;
    jtag_port_reg &= ~(TMS | TCK | TDI);
    jtag_port_reg |= ((tms ? TMS : 0u) | (tdi ? TDI : 0u));
    jtag_outp(jtag_port_reg);
    ret = jtag_inp();
    jtag_port_reg |= TCK;
    jtag_outp(jtag_port_reg);
    return ret;
}

/*
* User attention: (done)
* Module: dp_delay
*        purpose: Execute a time delay for a specified amount of time.
* Arguments: 
*        microseconeds: 32 bit value containing the amount of wait time in microseconds.
* Return value: None
* 
*/

#define MEMORY_BARRIER __asm__ volatile (                         \
      "" :      /* inline assembly code: effectively nothing */   \
      :         /* list of outputs: empty */                      \
      :         /* list of inputs: empty */                       \
      "memory"  /* clobbered data: memory */                      \
   )

void dp_delay(DPULONG microseconds)
{
    unsigned long ticks = microseconds * 100;
    while(ticks--)
        MEMORY_BARRIER;
}


#ifdef ENABLE_DEBUG
void dp_display_text(DPCHAR *text)
{
    printf("%s", text);
}

void dp_display_value(DPULONG value,DPUINT descriptive)
{
    switch(descriptive) {
        case HEX:
            printf("%x", value);
            break;
        case DEC:
            printf("%d", value);
            break;
        case CHR:
            printf("%c", value);
            break;
        default:
            break;
    
    }

}
void dp_display_array(DPUCHAR *value,DPUINT bytes, DPUINT descriptive)
{
    print_hex_ascii_line(value, bytes, 0);
}
#endif

