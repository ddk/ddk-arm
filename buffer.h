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
 * File: buffer.h
 * Description: header & definitions of all buffer related functions.
 */

#ifndef _BUFFER_H
#define _BUFFER_H

#include <LPC17xx.h>

// XXX remove me
#define PCONP_PCGPIO    0x00008000

#define BUF1PIN   26
#define BUF1      (1 << BUF1PIN)
#define BUF1PORT  0
#define BUF1DIR   LPC_GPIO0->FIODIR
#define BUF1MASK  LPC_GPIO0->FIOMASK
#define BUF1SET   LPC_GPIO0->FIOSET
#define BUF1CLR   LPC_GPIO0->FIOCLR
#define BUF1PRT   LPC_GPIO0->FIOPIN

#define BUF2PIN   25
#define BUF2      (1 << BUF2PIN)
#define BUF2PORT  0
#define BUF2DIR   LPC_GPIO0->FIODIR
#define BUF2MASK  LPC_GPIO0->FIOMASK
#define BUF2SET   LPC_GPIO0->FIOSET
#define BUF2CLR   LPC_GPIO0->FIOCLR
#define BUF2PRT   LPC_GPIO0->FIOPIN

#define BUF3PIN   24
#define BUF3      (1 << BUF3PIN)
#define BUF3PORT  0
#define BUF3DIR   LPC_GPIO0->FIODIR
#define BUF3MASK  LPC_GPIO0->FIOMASK
#define BUF3SET   LPC_GPIO0->FIOSET
#define BUF3CLR   LPC_GPIO0->FIOCLR
#define BUF3PRT   LPC_GPIO0->FIOPIN

#define BUF4PIN   23
#define BUF4      (1 << BUF4PIN)
#define BUF4PORT  0
#define BUF4DIR   LPC_GPIO0->FIODIR
#define BUF4MASK  LPC_GPIO0->FIOMASK
#define BUF4SET   LPC_GPIO0->FIOSET
#define BUF4CLR   LPC_GPIO0->FIOCLR
#define BUF4PRT   LPC_GPIO0->FIOPIN

#define BUF5PIN   12
#define BUF5      (1 << BUF5PIN)
#define BUF5PORT  2
#define BUF5DIR   LPC_GPIO2->FIODIR
#define BUF5MASK  LPC_GPIO2->FIOMASK
#define BUF5SET   LPC_GPIO2->FIOSET
#define BUF5CLR   LPC_GPIO2->FIOCLR
#define BUF5PRT   LPC_GPIO2->FIOPIN

#define BUF6PIN   11
#define BUF6      (1 << BUF6PIN)
#define BUF6PORT  2
#define BUF6DIR   LPC_GPIO2->FIODIR
#define BUF6MASK  LPC_GPIO2->FIOMASK
#define BUF6SET   LPC_GPIO2->FIOSET
#define BUF6CLR   LPC_GPIO2->FIOCLR
#define BUF6PRT   LPC_GPIO2->FIOPIN

#define BUF7PIN   20
#define BUF7      (1 << BUF7PIN)
#define BUF7PORT  0
#define BUF7DIR   LPC_GPIO0->FIODIR
#define BUF7MASK  LPC_GPIO0->FIOMASK
#define BUF7SET   LPC_GPIO0->FIOSET
#define BUF7CLR   LPC_GPIO0->FIOCLR
#define BUF7PRT   LPC_GPIO0->FIOPIN

#define BUF8PIN   19
#define BUF8      (1 << BUF8PIN)
#define BUF8PORT  0
#define BUF8DIR   LPC_GPIO0->FIODIR
#define BUF8MASK  LPC_GPIO0->FIOMASK
#define BUF8SET   LPC_GPIO0->FIOSET
#define BUF8CLR   LPC_GPIO0->FIOCLR
#define BUF8PRT   LPC_GPIO0->FIOPIN

/*!
   Enable buffer 1.
 */
#define buf1_enable()       ( BUF1SET |=  BUF1)

/*!
   Disable buffer 1.
 */
#define buf1_disable()      ( BUF1CLR |=  BUF1)

/*!
   Enable buffer 2.
 */
#define buf2_enable()       ( BUF2SET |=  BUF2)

/*!
   Disable buffer 2.
 */
#define buf2_disable()      ( BUF2CLR |=  BUF2)

/*!
   Enable buffer 3.
 */
#define buf3_enable()       ( BUF3SET |=  BUF3)

/*!
   Disable buffer 3.
 */
#define buf3_disable()      ( BUF3CLR |=  BUF3)

/*!
   Enable buffer 4.
 */
#define buf4_enable()       ( BUF4SET |=  BUF4)
/*!
   Disable buffer 4.
 */
#define buf4_disable()      ( BUF4CLR |=  BUF4)

/*!
   Enable buffer 5.
 */
#define buf5_enable()       ( BUF5SET |=  BUF5)
/*!
   Disable buffer 5.
 */
#define buf5_disable()      ( BUF5CLR |=  BUF5)

/*!
   Enable buffer 6.
 */
#define buf6_enable()       ( BUF6SET |=  BUF6)
/*!
   Disable buffer 6.
 */
#define buf6_disable()      ( BUF6CLR |=  BUF6)

/*!
   Enable buffer 7.
 */
#define buf7_enable()       ( BUF7SET |=  BUF7)
/*!
   Disable buffer 7.
 */
#define buf7_disable()      ( BUF7CLR |=  BUF7)

/*!
   Enable buffer 8.
 */
#define buf8_enable()       ( BUF8SET |=  BUF8)
/*!
   Disable buffer 8.
 */
 #define buf8_disable()     ( BUF8CLR |=  BUF8)

/*!
   Initialize buffer 1 enable line.
 */
void init_buf1(void);

/*!
   Initialize buffer 2 enable line.
 */
void init_buf2(void);

/*!
   Initialize buffer 3 enable line.
 */
void init_buf3(void);

/*!
   Initialize buffer 4 enable line.
 */
void init_buf4(void);

/*!
   Initialize buffer 5 enable line.
 */
void init_buf5(void);

/*!
   Initialize buffer 6 enable line.
 */
void init_buf6(void);

/*!
   Initialize buffer 7 enable line.
 */
void init_buf7(void);

/*!
   Initialize buffer 8 enable line.
 */
void init_buf8(void);

/*!
   Initialize buffer enable lines.
 */
void buffer_init(void);

#endif