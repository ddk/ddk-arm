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
 * File: led.h
 * Description: Header & definitions of LED related functions.
 *
 */


#ifndef _LED_H
#define _LED_H

#include <LPC17xx.h>


#define FLASH_MS        30
#define WAIT_MS         5

void vBlinkTask(void *p);
void led_init(void);
void init_led4(void);
void init_led3(void);
void init_led2(void);
void init_led1(void);

// XXX remove me
#define PCONP_PCGPIO    0x00008000

#define LED1      (1 << 6 )
#define LED1PIN   6
#define LED1DIR   LPC_GPIO0->FIODIR
#define LED1MASK  LPC_GPIO0->FIOMASK
#define LED1SET   LPC_GPIO0->FIOSET
#define LED1CLR   LPC_GPIO0->FIOCLR
#define LED1PRT   LPC_GPIO0->FIOPIN

#define LED2      (1 << 7)
#define LED2PIN   7
#define LED2DIR   LPC_GPIO0->FIODIR
#define LED2MASK  LPC_GPIO0->FIOMASK
#define LED2SET   LPC_GPIO0->FIOSET
#define LED2CLR   LPC_GPIO0->FIOCLR
#define LED2PRT   LPC_GPIO0->FIOPIN

#define LED3      (1 << 8)
#define LED3PIN   8
#define LED3DIR   LPC_GPIO0->FIODIR     // GPIO1->FIODIR
#define LED3MASK  LPC_GPIO0->FIOMASK    // GPIO1->FIOMASK
#define LED3SET   LPC_GPIO0->FIOSET
#define LED3CLR   LPC_GPIO0->FIOCLR
#define LED3PRT   LPC_GPIO0->FIOPIN

#define LED4      (1 << 9)
#define LED4PIN   9
#define LED4DIR   LPC_GPIO0->FIODIR     // GPIO1->FIODIR
#define LED4MASK  LPC_GPIO0->FIOMASK    // GPIO1->FIOMASK
#define LED4SET   LPC_GPIO0->FIOSET
#define LED4CLR   LPC_GPIO0->FIOCLR
#define LED4PRT   LPC_GPIO0->FIOPIN



#define led1_set()      ( LED1CLR |=  LED1) 
#define led1_clr()      ( LED1SET |=  LED1)

#define led2_set()      ( LED2CLR |=  LED2)
#define led2_clr()      ( LED2SET |=  LED2)

#define led3_set()      ( LED3CLR |=  LED3)
#define led3_clr()      ( LED3SET |=  LED3)

#define led4_set()      ( LED4CLR |=  LED4)
#define led4_clr()      ( LED4SET |=  LED4)

#define led1_toggle()      ( LED1PRT ^=  LED1)
#define led2_toggle()      ( LED2PRT ^=  LED2)
#define led3_toggle()      ( LED3PRT ^=  LED3)
#define led4_toggle()      ( LED4PRT ^=  LED4)

#endif

