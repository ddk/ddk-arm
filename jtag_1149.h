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
 * File: jtag_1149.h
 * Description: Header & definitions of JTAG related functions (for programming the FPGA).
 *
 */

#ifndef _JTAG1149_H
#define _JTAG1149_H

#include <LPC17xx.h>
#include <stdint.h>


#define PIN     0       // input
#define POUT    1       // output

#define TCK_PERIOD  10


void jtag_init(void);

typedef enum __cmd_signal {
    TMS     = 1,
    TDI     = 2,
    TRST    = 4,
    TDO     = 8
} jtag_cmd_signal_t;


/*
test setup, using olimex stk-1766:

target                  olimex
=================================================
TRST = 3 = rot          UEXT9 / 0.7     (SCK1)
TDI  = 5 = orange       UEXT5 / 0.11    (SCL2)
TMS  = 7 = gelb         UEXT8 / 0.9     (MOSI1)
TCK  = 9 = gruen        UEXT7 / 0.8     (MISO1)
TDO = 13 = blau         UEXT6 / 0.10    (SDA2)
GND                     UEXT2


UEXT:

    +----------------+
    |  2  4  6  8 10 |
    |  1  3  5  7  9 |
    +------    ------+

*/


#define JTAG_PORT       1

#define JTAG_TDOPIN     1
#define JTAG_TDOPORT    1
#define JTAG_TDO        (1 << JTAG_TDOPIN)
#define JTAG_TDODIR     LPC_GPIO1->FIODIR
#define JTAG_TDOMASK    LPC_GPIO1->FIOMASK
#define JTAG_TDOSET     LPC_GPIO1->FIOSET
#define JTAG_TDOCLR     LPC_GPIO1->FIOCLR
#define JTAG_TDOPRT     LPC_GPIO1->FIOPIN

#define jtag_TDO_low()    ( JTAG_TDOCLR |= JTAG_TDO)
#define jtag_TDO_high()   ( JTAG_TDOSET |= JTAG_TDO)
#define jtag_TDO_toggle() ( JTAG_TDOPRT ^= JTAG_TDO)

#define JTAG_TDIPIN     8
#define JTAG_TDIPORT    1
#define JTAG_TDI        (1 << JTAG_TDIPIN)
#define JTAG_TDIDIR     LPC_GPIO1->FIODIR
#define JTAG_TDIMASK    LPC_GPIO1->FIOMASK
#define JTAG_TDISET     LPC_GPIO1->FIOSET
#define JTAG_TDICLR     LPC_GPIO1->FIOCLR
#define JTAG_TDIPRT     LPC_GPIO1->FIOPIN

#define jtag_TDI_low()    ( JTAG_TDICLR |= JTAG_TDI)
#define jtag_TDI_high()   ( JTAG_TDISET |= JTAG_TDI)
#define jtag_TDI_toggle() ( JTAG_TDIPRT ^= JTAG_TDI)

#define JTAG_TMSPIN     4
#define JTAG_TMSPORT    1
#define JTAG_TMS        (1 << JTAG_TMSPIN)
#define JTAG_TMSDIR     LPC_GPIO1->FIODIR
#define JTAG_TMSMASK    LPC_GPIO1->FIOMASK
#define JTAG_TMSSET     LPC_GPIO1->FIOSET
#define JTAG_TMSCLR     LPC_GPIO1->FIOCLR
#define JTAG_TMSPRT     LPC_GPIO1->FIOPIN

#define jtag_TMS_low()    ( JTAG_TMSCLR |= JTAG_TMS)
#define jtag_TMS_high()   ( JTAG_TMSSET |= JTAG_TMS)
#define jtag_TMS_toggle() ( JTAG_TMSPRT ^= JTAG_TMS)

#define JTAG_TRSTPIN    9
#define JTAG_TRSTPORT   1
#define JTAG_TRST       (1 << JTAG_TRSTPIN)
#define JTAG_TRSTDIR    LPC_GPIO1->FIODIR
#define JTAG_TRSTMASK   LPC_GPIO1->FIOMASK
#define JTAG_TRSTSET    LPC_GPIO1->FIOSET
#define JTAG_TRSTCLR    LPC_GPIO1->FIOCLR
#define JTAG_TRSTPRT    LPC_GPIO1->FIOPIN

#define jtag_TRST_low()    ( JTAG_TRSTCLR |= JTAG_TRST)
#define jtag_TRST_high()   ( JTAG_TRSTSET |= JTAG_TRST)
#define jtag_TRST_toggle() ( JTAG_TRSTPRT ^= JTAG_TRST)

#define JTAG_TCKPIN     0
#define JTAG_TCKPORT    1
#define JTAG_TCK        (1 << JTAG_TCKPIN)
#define JTAG_TCKDIR     LPC_GPIO1->FIODIR
#define JTAG_TCKMASK    LPC_GPIO1->FIOMASK
#define JTAG_TCKSET     LPC_GPIO1->FIOSET
#define JTAG_TCKCLR     LPC_GPIO1->FIOCLR
#define JTAG_TCKPRT     LPC_GPIO1->FIOPIN

#define jtag_TCK_low()    ( JTAG_TCKCLR |= JTAG_TCK)
#define jtag_TCK_high()   ( JTAG_TCKSET |= JTAG_TCK)
#define jtag_TCK_toggle() ( JTAG_TCKPRT ^= JTAG_TCK)

#endif
