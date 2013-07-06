/*
 * Copyright (c) 2013, The DDK Project
 *    Dmitry Nedospasov <dmitry at nedos dot net>
 *    Thorsten Schroeder <ths at modzero dot ch>
 *
 * All rights reserved.
 *
 * This file is part of Die Datenkrake (DDK).
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <dmitry at nedos dot net> and <ths at modzero dot ch> wrote this file. As
 * long as you retain this notice you can do whatever you want with this stuff.
 * If we meet some day, and you think this stuff is worth it, you can buy us a
 * beer in return. Die Datenkrake Project.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DDK PROJECT BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: jtag_1149.c
 * Description: Implementation of JTAG related functions (for programming the FPGA).
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctl_api.h>
#include <string.h>

#include "jtag_1149.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

#include "helper.h"

static   PINSEL_CFG_Type PinCfg;

volatile uint32_t g_utick = 0;

void jtag_init(void)
{
    uint32_t i=0;

    PinCfg.Funcnum      = 0;    // GPIO
    PinCfg.OpenDrain    = 0;
    PinCfg.Pinmode      = 0;

    puts("");
    printf("Init JTAG port pins: TDO, ");

    PinCfg.Portnum      = JTAG_TDOPORT;
    PinCfg.Pinnum       = JTAG_TDOPIN;
    PINSEL_ConfigPin(&PinCfg);

    printf("TDI, ");
    PinCfg.Portnum      = JTAG_TDIPORT;
    PinCfg.Pinnum       = JTAG_TDIPIN;
    PINSEL_ConfigPin(&PinCfg);

    printf("TMS, ");
    PinCfg.Portnum      = JTAG_TMSPORT;
    PinCfg.Pinnum       = JTAG_TMSPIN;
    PINSEL_ConfigPin(&PinCfg);

    printf("TRST, ");
    PinCfg.Portnum      = JTAG_TRSTPORT;
    PinCfg.Pinnum       = JTAG_TRSTPIN;
    PINSEL_ConfigPin(&PinCfg);

    printf("TCK - done\n");
    PinCfg.Portnum      = JTAG_TCKPORT;
    PinCfg.Pinnum       = JTAG_TCKPIN;
    PINSEL_ConfigPin(&PinCfg);

    GPIO_SetDir(JTAG_TDIPORT, JTAG_TDI, 1);
    GPIO_SetDir(JTAG_TDOPORT, JTAG_TDO, 0);
    GPIO_SetDir(JTAG_TMSPORT, JTAG_TMS, 1);
    GPIO_SetDir(JTAG_TCKPORT, JTAG_TCK, 1);
    GPIO_SetDir(JTAG_TRSTPORT, JTAG_TRST, 1);

    GPIO_ClearValue(JTAG_TDIPORT, JTAG_TDI);

    GPIO_ClearValue(JTAG_TMSPORT, JTAG_TMS);
    GPIO_ClearValue(JTAG_TCKPORT, JTAG_TCK);

    GPIO_ClearValue(JTAG_TRSTPORT, JTAG_TRST);
    GPIO_SetValue(  JTAG_TRSTPORT, JTAG_TRST);

    return;
}
