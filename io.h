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
 * File: io.h
 * Description: header & definitions of all GPIO related functions.
 */

#ifndef _IO_H
#define _IO_H

#include <LPC17xx.h>

// XXX remove me
#define PCONP_PCGPIO    0x00008000

#define PIN     0       // input
#define POUT    1       // output

/*!
   Initialize FPGA reset line.
 */
void io_init_fpga_rst(void);

/*!
   Initialize FPGA data write enable line.
 */
void io_init_fpga_dwe(void);

/*!
   Initialize FPGA databus i/o.
 */
void io_init_fpga_bus(void);

/*!
   Initialize FPGA databus clock line.
 */
void io_init_fpga_dclk(void);


/*!
   Initialize CLKOUT.
 */
void io_init_clkout(void);

/*!
   Initialize power enable lines.
 */
void io_init_fpga_pwrenable(void);

/*!
    Enable (power-on) FPGA.
*/
void io_fpga_enable(void);

/*!
   Initialize I/O ports & wires.
 */
void io_init(void);

/*!
   Disable (power-off) FPGA.
 */
void io_fpga_disable(void);

/*!
   Initialize test mode selection pin (I/O).
 */
void io_init_fpga_tmd(void);

/*!
   Initialize test enable pin (I/O).
 */
void io_init_fpga_tst(void);

/*!
   Databus task. All databus related function runs here.
   @param[in]  *p    pointer to argument struct
 */
void task_databus(void *p);

/*!
   Write data to FPGA register. This function writes a byte of data to an 8-bit register address of the FPGA.
   @param[in]  addr  The destination register's address.
   @param[in]  data  The data byte.
 */
void io_fpga_register_write(uint8_t addr, uint8_t data);

/*!
   FPGA databus testcase.
 */
uint8_t io_fpga_bus_test(void);

static inline void io_fpga_bus_dir_input(void);
static inline void io_fpga_bus_dir_output(void);

// 1V5 DC-DC enable for FPGA core voltage
#define FPGA_CORE_EN_PIN    18
#define FPGA_CORE_EN_PORT   0
#define FPGA_CORE_EN        (1 << FPGA_CORE_EN_PIN)
#define FPGA_CORE_EN_DIR    LPC_GPIO0->FIODIR
#define FPGA_CORE_EN_MASK   LPC_GPIO0->FIOMASK
#define FPGA_CORE_EN_SET    LPC_GPIO0->FIOSET
#define FPGA_CORE_EN_CLR    LPC_GPIO0->FIOCLR
#define FPGA_CORE_EN_PRT    LPC_GPIO0->FIOPIN
#define fpga_pwr_enable_core_low()    ( FPGA_CORE_EN_CLR = FPGA_CORE_EN)
#define fpga_pwr_enable_core_high()   ( FPGA_CORE_EN_SET = FPGA_CORE_EN)
#define fpga_pwr_enable_core_toggle() ( FPGA_CORE_EN_PRT ^= FPGA_CORE_EN)

// 3V3 DC-DC enable for FPGA I/O voltage
#define FPGA_IO_EN_PIN    17
#define FPGA_IO_EN_PORT   0
#define FPGA_IO_EN        (1 << FPGA_IO_EN_PIN)
#define FPGA_IO_EN_DIR    LPC_GPIO0->FIODIR
#define FPGA_IO_EN_MASK   LPC_GPIO0->FIOMASK
#define FPGA_IO_EN_SET    LPC_GPIO0->FIOSET
#define FPGA_IO_EN_CLR    LPC_GPIO0->FIOCLR
#define FPGA_IO_EN_PRT    LPC_GPIO0->FIOPIN
#define fpga_pwr_enable_io_low()    ( FPGA_IO_EN_CLR = FPGA_IO_EN)
#define fpga_pwr_enable_io_high()   ( FPGA_IO_EN_SET = FPGA_IO_EN)
#define fpga_pwr_enable_io_toggle() ( FPGA_IO_EN_PRT ^= FPGA_IO_EN)

// databus clock - currently using TXD3
#define FPGA_DCLKPIN    0
#define FPGA_DCLKPORT   0
#define FPGA_DCLK       (1 << FPGA_DCLKPIN)
#define FPGA_DCLKDIR    LPC_GPIO0->FIODIR
#define FPGA_DCLKMASK   LPC_GPIO0->FIOMASK
#define FPGA_DCLKSET    LPC_GPIO0->FIOSET
#define FPGA_DCLKCLR    LPC_GPIO0->FIOCLR
#define FPGA_DCLKPRT    LPC_GPIO0->FIOPIN
#define fpga_bus_dataclock_low()    ( FPGA_DCLKCLR = FPGA_DCLK)
#define fpga_bus_dataclock_high()   ( FPGA_DCLKSET = FPGA_DCLK)
#define fpga_bus_dataclock_toggle() ( FPGA_DCLKPRT ^= FPGA_DCLK)

// databus write enable - currently using RXD3
#define FPGA_DWEPIN     1
#define FPGA_DWEPORT    0
#define FPGA_DWE        (1 << FPGA_DWEPIN)
#define FPGA_DWEDIR     LPC_GPIO0->FIODIR
#define FPGA_DWEMASK    LPC_GPIO0->FIOMASK
#define FPGA_DWESET     LPC_GPIO0->FIOSET
#define FPGA_DWECLR     LPC_GPIO0->FIOCLR
#define FPGA_DWEPRT     LPC_GPIO0->FIOPIN
#define fpga_dwe_low()  ( FPGA_DWECLR =  FPGA_DWE )
#define fpga_dwe_high() ( FPGA_DWESET =  FPGA_DWE )

// FPGA 16bit bus - data portion
#define FPGA_BUS_DATA_PORT    1
#define FPGA_BUS_DATA_DIR     LPC_GPIO1->FIODIR
#define FPGA_BUS_DATA_MASK    LPC_GPIO1->FIOMASK
#define FPGA_BUS_DATA_SET     LPC_GPIO1->FIOSET
#define FPGA_BUS_DATA_CLR     LPC_GPIO1->FIOCLR
#define FPGA_BUS_DATA_PRT     LPC_GPIO1->FIOPIN

#define FPGA_BUS_DATA_PIN_HIGH        23  // msb
#define FPGA_BUS_DATA_PIN_LOW         16  // lsb
#define FPGA_BUS_DATA_PINMASK        ((~(-1 * (1<<((FPGA_BUS_DATA_PIN_HIGH - FPGA_BUS_DATA_PIN_LOW) + 1))))<<FPGA_BUS_DATA_PIN_LOW)

// FPGA 16bit bus - address portion
#define FPGA_BUS_ADDR_PORT    2
#define FPGA_BUS_ADDR_DIR     LPC_GPIO2->FIODIR
#define FPGA_BUS_ADDR_MASK    LPC_GPIO2->FIOMASK
#define FPGA_BUS_ADDR_SET     LPC_GPIO2->FIOSET
#define FPGA_BUS_ADDR_CLR     LPC_GPIO2->FIOCLR
#define FPGA_BUS_ADDR_PRT     LPC_GPIO2->FIOPIN

#define FPGA_BUS_ADDR_PIN_HIGH        7  // msb
#define FPGA_BUS_ADDR_PIN_LOW         0  // lsb
#define FPGA_BUS_ADDR_PINMASK        ((~(-1 * (1<<((FPGA_BUS_ADDR_PIN_HIGH - FPGA_BUS_ADDR_PIN_LOW) + 1))))<<FPGA_BUS_ADDR_PIN_LOW)


// SysRst on UART 2 TX
#define FPGA_RSTPIN     10
#define FPGA_RSTPORT    0
#define FPGA_RST        (1 << FPGA_RSTPIN)
#define FPGA_RSTDIR     LPC_GPIO0->FIODIR
#define FPGA_RSTMASK    LPC_GPIO0->FIOMASK
#define FPGA_RSTSET     LPC_GPIO0->FIOSET
#define FPGA_RSTCLR     LPC_GPIO0->FIOCLR
#define FPGA_RSTPRT     LPC_GPIO0->FIOPIN
#define fpga_rst_low()  ( FPGA_RSTCLR =  FPGA_RST )
#define fpga_rst_high() ( FPGA_RSTSET =  FPGA_RST )


// TestEn on UART 2 RX
#define FPGA_TSTPIN     11
#define FPGA_TSTPORT    0
#define FPGA_TST        (1 << FPGA_TSTPIN)
#define FPGA_TSTDIR     LPC_GPIO0->FIODIR
#define FPGA_TSTMASK    LPC_GPIO0->FIOMASK
#define FPGA_TSTSET     LPC_GPIO0->FIOSET
#define FPGA_TSTCLR     LPC_GPIO0->FIOCLR
#define FPGA_TSTPRT     LPC_GPIO0->FIOPIN
#define fpga_tst_low()  ( FPGA_TSTCLR =  FPGA_TST )
#define fpga_tst_high() ( FPGA_TSTSET =  FPGA_TST )


// TestMode on UART 1 TX
#define FPGA_TMDPIN     15
#define FPGA_TMDPORT    0
#define FPGA_TMD        (1 << FPGA_TMDPIN)
#define FPGA_TMDDIR     LPC_GPIO0->FIODIR
#define FPGA_TMDMASK    LPC_GPIO0->FIOMASK
#define FPGA_TMDSET     LPC_GPIO0->FIOSET
#define FPGA_TMDCLR     LPC_GPIO0->FIOCLR
#define FPGA_TMDPRT     LPC_GPIO0->FIOPIN
#define fpga_tmd_low()  ( FPGA_TMDCLR =  FPGA_TMD )
#define fpga_tmd_high() ( FPGA_TMDSET =  FPGA_TMD )

#endif