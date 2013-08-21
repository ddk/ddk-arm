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
 * File: uart.h
 * Description: header & definitions of all uart related functions.
 */

#ifndef _UART_H
#define _UART_H

#include <stdbool.h>
#include <stdint.h>

#define UART0_BAUDRATE  115200
#define UART1_BAUDRATE  115200
#define UART2_BAUDRATE  115200
#define UART3_BAUDRATE  115200

void vUART0Task(void *p);
void vUART1Task(void *p);
void vUART2Task(void *p);
void vUART3Task(void *p);

void uart0_init(const uint32_t, const bool);
void uart0_shutdown(void);
void UART0_IRQHandler(void);
int getchar_nb(void);
// for stdio (printf, scanf, etc) - blocking!
int __getchar(void);
void __putchar(int);
#ifndef __GNUC__
void putchar(int);
int getchar(void);
#endif

void uart1_init(const uint32_t , const bool);
void UART1_IRQHandler(void);
int getchar1_nb(void);
int getchar1(void);
void putchar1(int);

void uart2_init(const uint32_t , const bool);
void UART2_IRQHandler(void);
int getchar2_nb(void);
int getchar2(void);
void putchar2(int);

void uart3_init(const uint32_t , const bool);
void UART3_IRQHandler(void);
int getchar3_nb(void);
int getchar3(void);
void putchar3(int);


#endif
