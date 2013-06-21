/*
 * Copyright (c) 2013, The DDK Project
 *    Dmitry Nedospasov <dmitry at nedos dot net>
 *    Thorsten Schroeder <ths at modzero dot ch>
 *
 * All rights reserved.
 *
 * This file is part of Die Datenkrake (DDK).
 * 
 * Die Datenkrake is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.

 * Die Datenkrake is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Die Datenkrake.  If not, see <http://www.gnu.org/licenses/>.
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
 * File: io.c
 * Description: Implementation of all GPIO related functions.
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"

#include "io.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

static   PINSEL_CFG_Type PinCfg;

/** Init Data Clock (DCLK) */

void io_init_fpga_dclk(void)
{

    PinCfg.Funcnum      = 0;    // GPIO
    PinCfg.OpenDrain    = 0;
    PinCfg.Pinmode      = 0;
    PinCfg.Portnum      = FPGA_DCLKPORT;
    PinCfg.Pinnum       = FPGA_DCLKPIN;

    PINSEL_ConfigPin(&PinCfg);

    GPIO_SetDir(FPGA_DCLKPORT, FPGA_DCLK, POUT);
    return;
}

/* Init Data Write Enable (DWE) */
void io_init_fpga_dwe(void)
{
    PinCfg.Funcnum      = 0;    // GPIO
    PinCfg.OpenDrain    = 0;
    PinCfg.Pinmode      = 0;
    PinCfg.Portnum      = FPGA_DWEPORT;
    PinCfg.Pinnum       = FPGA_DWEPIN;

    PINSEL_ConfigPin(&PinCfg);

    GPIO_SetDir(FPGA_DWEPORT, FPGA_DWE, POUT);
    return;
}

/* Init DDK I/O */
void io_init(void)
{
    LPC_SC->PCONP |= PCONP_PCGPIO;
    io_init_fpga_bus();
    io_init_fpga_dclk();
    io_init_fpga_dwe();
    io_init_clkout();
    io_init_fpga_pwrenable();

    return;
}


/** ***************************************************** **/

/* Init parallel 16bit Databus to FPGA */
void io_init_fpga_bus(void)
{
    uint32_t i=0;

    PinCfg.Funcnum      = 0;    // GPIO
    PinCfg.OpenDrain    = 0;
    PinCfg.Pinmode      = 0;
    PinCfg.Portnum      = FPGA_BUS_DATA_PORT;
    PinCfg.Pinnum       = FPGA_BUS_DATA_PIN_LOW;

    for(i=FPGA_BUS_DATA_PIN_LOW; i<=FPGA_BUS_DATA_PIN_HIGH; i++) {
        PinCfg.Pinnum       = i;
        //printf("dbg: init fpga bus port %d pin %d\n", PinCfg.Portnum, PinCfg.Pinnum);
        PINSEL_ConfigPin(&PinCfg);
    }

    FIO_ByteSetDir(FPGA_BUS_DATA_PORT, 2, 0xff, POUT);
    FIO_ByteClearValue(FPGA_BUS_DATA_PORT, 2 , 0xff);


    PinCfg.Funcnum      = 0;    // GPIO
    PinCfg.OpenDrain    = 0;
    PinCfg.Pinmode      = 0;
    PinCfg.Portnum      = FPGA_BUS_ADDR_PORT;
    PinCfg.Pinnum       = FPGA_BUS_ADDR_PIN_LOW;

    for(i=FPGA_BUS_ADDR_PIN_LOW; i<=FPGA_BUS_ADDR_PIN_HIGH; i++) {
        PinCfg.Pinnum       = i;
        //printf("dbg: init fpga bus port %d pin %d\n", PinCfg.Portnum, PinCfg.Pinnum);
        PINSEL_ConfigPin(&PinCfg);
    }

    FIO_ByteSetDir(FPGA_BUS_ADDR_PORT, 0, 0xff, POUT);
    FIO_ByteClearValue(FPGA_BUS_ADDR_PORT, 0 , 0xff);

    fpga_bus_dataclock_low();

    return;
}



void io_init_fpga_tst(void)
{
    uint32_t i=0;

    PinCfg.Funcnum      = 0;
    PinCfg.OpenDrain    = 0;
    PinCfg.Pinmode      = 0;

    PinCfg.Pinnum       = FPGA_TSTPIN;
    PinCfg.Portnum      = FPGA_TSTPORT;
    PINSEL_ConfigPin(&PinCfg);
    GPIO_SetDir(FPGA_TSTPORT, FPGA_TST, POUT);
    fpga_tst_low();

    return;
}

// test mode pin
void io_init_fpga_tmd(void)
{
    uint32_t i=0;

    PinCfg.Funcnum      = 0;
    PinCfg.OpenDrain    = 0;
    PinCfg.Pinmode      = 0;

    PinCfg.Pinnum       = FPGA_TMDPIN;
    PinCfg.Portnum      = FPGA_TMDPORT;
    PINSEL_ConfigPin(&PinCfg);
    GPIO_SetDir(FPGA_TMDPORT, FPGA_TMD, POUT);
    fpga_tmd_low();

    return;
}


void io_init_fpga_rst(void)
{
    uint32_t i=0;

    PinCfg.Funcnum      = 0;
    PinCfg.OpenDrain    = 0;
    PinCfg.Pinmode      = 0;

    PinCfg.Pinnum       = FPGA_RSTPIN;
    PinCfg.Portnum      = FPGA_RSTPORT;
    PINSEL_ConfigPin(&PinCfg);
    GPIO_SetDir(FPGA_RSTPORT, FPGA_RST, POUT);
    fpga_rst_low();

    return;
}




/* enable FPGA EN_CORE and EN_IO */
void io_init_fpga_pwrenable(void)
{
    uint32_t i=0;

    PinCfg.Funcnum      = 0;
    PinCfg.OpenDrain    = 0;
    PinCfg.Pinmode      = 0;


    // 1V5 core voltage
    PinCfg.Pinnum       = FPGA_CORE_EN_PIN;
    PinCfg.Portnum      = FPGA_CORE_EN_PORT;
    PINSEL_ConfigPin(&PinCfg);
    GPIO_SetDir(FPGA_CORE_EN_PORT, FPGA_CORE_EN, POUT);
    fpga_pwr_enable_core_low();

    // 3V3 IO voltage
    PinCfg.Pinnum       = FPGA_IO_EN_PIN;
    PinCfg.Portnum      = FPGA_IO_EN_PORT;
    PINSEL_ConfigPin(&PinCfg);
    GPIO_SetDir(FPGA_IO_EN_PORT, FPGA_IO_EN, POUT);
    fpga_pwr_enable_io_low();

    return;
}

void io_fpga_disable(void)
{
    fpga_pwr_enable_core_low();
    fpga_pwr_enable_io_low();
}

void io_fpga_enable(void)
{
    fpga_pwr_enable_core_high();
    fpga_pwr_enable_io_high();
}

/* Init clkout pin 1.27 */
void io_init_clkout(void)
{
    uint32_t i=0;

    PinCfg.Funcnum      = 1;    // CLKOUT
    PinCfg.OpenDrain    = 0;
    PinCfg.Pinmode      = 0;
    PinCfg.Portnum      = 1;
    PinCfg.Pinnum       = 27;
    PINSEL_ConfigPin(&PinCfg);

    return;
}


static inline void io_fpga_register_setaddress(uint8_t addr)
{
    FIO_ByteClearValue(FPGA_BUS_ADDR_PORT, 0, 0xFF);
    FIO_ByteSetValue(FPGA_BUS_ADDR_PORT, 0, addr);
}

static inline void io_fpga_register_setdata(uint8_t data)
{
    FIO_ByteClearValue(FPGA_BUS_DATA_PORT, 2, 0xFF);
    FIO_ByteSetValue(FPGA_BUS_DATA_PORT, 2, data);
}

/* Write 8-bit "data" to address "reg" on FPGA */
void io_fpga_register_write(uint8_t reg, uint8_t data)
{
    fpga_dwe_high();

    io_fpga_register_setaddress(reg);
    io_fpga_register_setdata(data);
    
    fpga_bus_dataclock_toggle();
 
}

