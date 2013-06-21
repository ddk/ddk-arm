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
 * File: uart.c
 * Description: Implementation of all UART related functions.
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"

#include "lpc_types.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctl_api.h>

#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"

#include "uart.h"
#include "helper.h"


#define CONSOLE_UART_PORT       LPC_UART0
#define UART0_FUNCNUM           1
#define UART0_PORTNUM           0
#define UART0_TX_PINNUM         2
#define UART0_RX_PINNUM         3

#define FPGA_UART1_PORT       LPC_UART1
#define UART1_FUNCNUM           1
#define UART1_PORTNUM           0
#define UART1_TX_PINNUM         15
#define UART1_RX_PINNUM         16

#define FPGA_UART_PORT          LPC_UART2
#define UART2_FUNCNUM           1
#define UART2_PORTNUM           0
#define UART2_TX_PINNUM         10
#define UART2_RX_PINNUM         11

#define FPGA_UART3_PORT         LPC_UART3
#define UART3_FUNCNUM           2
#define UART3_PORTNUM           0
#define UART3_TX_PINNUM         0
#define UART3_RX_PINNUM         1



/* ************************************************************ uart tasks */

/* UART0 */

void vUART0Task(void *p)
{  
    unsigned int v=0;
    uint8_t testchar = 1;
    
    const portTickType xDelay = 10 / portTICK_RATE_MS;

    
    ( void ) p;

    //puts("sending bytes 00h-ffh:");
    while (1) {
        int c = getchar_nb(); // testchar++; //
        if(c>=0) {
            putchar2(c);
            //putchar(c);
        }
        vTaskDelay( xDelay );
        
        // stop here
        if(c == 0) {

        }

        v++;
    }  
}

/* UART1 - Communication with FPGA */

void vUART1Task(void *p)
{  
    unsigned int v=0;
    const portTickType xDelay = 10 / portTICK_RATE_MS;

    ( void ) p;

    while (1) {
        int c = getchar1_nb();
        if(c > 0) {
            printf("u1 read: ");
            print_hex_byte(c);
            puts("");
        }

        vTaskDelay( xDelay );
   
        v++;
    }  
}

/* UART2 - Communication with FPGA */

void vUART2Task(void *p)
{  
    unsigned int v=0;
    const portTickType xDelay = 10 / portTICK_RATE_MS;

    ( void ) p;

    while (1) {
        int c = getchar2_nb();
        if(c > 0) {
            printf("u2 read: ");
            print_hex_byte(c);
            puts("");
        }

        vTaskDelay( xDelay );
   
        v++;
    }  
}

/* UART3 - currently disabled and used as i/o */

void vUART3Task(void *p)
{  
    unsigned int v=0;
    const portTickType xDelay = 10 / portTICK_RATE_MS;

    ( void ) p;

    while (1) {
        int c = getchar3_nb();
        if(c > 0) {
            //putchar(c);
            printf("u3: %02x\n", c & 0xff);
        }

        vTaskDelay( xDelay );
   
        v++;
    }  
}


/* ************************************************************ uart init functions */

void uart0_init(const uint32_t BaudRate, const bool DoubleSpeed)
{
    UART_CFG_Type UARTConfigStruct;
    PINSEL_CFG_Type PinCfg;

    PinCfg.Portnum = UART0_PORTNUM;
    PinCfg.Pinnum = UART0_TX_PINNUM;
    PinCfg.Funcnum = UART0_FUNCNUM;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = UART0_RX_PINNUM;
    PINSEL_ConfigPin(&PinCfg);

    /* Initialize UART Configuration parameter structure to default state:
     * Baudrate = 9600bps
     * 8 data bit
     * 1 Stop bit
     * None parity
     */
    UART_ConfigStructInit(&UARTConfigStruct);
    // Re-configure baudrate
    UARTConfigStruct.Baud_rate = BaudRate;

    // Initialize DEBUG_UART_PORT peripheral with given to corresponding parameter
    UART_Init((LPC_UART_TypeDef *)CONSOLE_UART_PORT, &UARTConfigStruct);

    // Enable UART Transmit
    UART_TxCmd((LPC_UART_TypeDef *)CONSOLE_UART_PORT, ENABLE);
    
    UART_IntConfig((LPC_UART_TypeDef *)CONSOLE_UART_PORT, UART_INTCFG_RBR, ENABLE);
    NVIC_EnableIRQ(UART0_IRQn);
}

void uart0_shutdown(void)
{

    puts("");

    UART_CFG_Type UARTConfigStruct;
    PINSEL_CFG_Type PinCfg;

    PinCfg.Portnum = UART0_PORTNUM;
    PinCfg.Pinnum = UART0_TX_PINNUM;
    PinCfg.Funcnum = 0;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = UART0_RX_PINNUM;
    PINSEL_ConfigPin(&PinCfg);

    GPIO_SetDir(UART0_PORTNUM, (1<<UART0_TX_PINNUM), 1);
    GPIO_SetValue(UART0_PORTNUM, (1<<UART0_TX_PINNUM));

    UART_TxCmd((LPC_UART_TypeDef *)CONSOLE_UART_PORT, DISABLE);
    
    UART_IntConfig((LPC_UART_TypeDef *)CONSOLE_UART_PORT, UART_INTCFG_RBR, DISABLE);
    NVIC_DisableIRQ(UART0_IRQn);
}


volatile unsigned char g_u0char_available = 0;
volatile unsigned char g_u0char = 0;

void UART0_IRQHandler(void)
{
    g_u0char_available=0;
    g_u0char = 0;

    unsigned int id = UART_GetIntId((LPC_UART_TypeDef *)CONSOLE_UART_PORT);
    unsigned int temp = 0;

    switch(id) {
        case 6:
            // RX Line Status / Error
            temp = CONSOLE_UART_PORT->LSR;
            break;
            
        case 4:
            g_u0char = UART_ReceiveByte((LPC_UART_TypeDef *)CONSOLE_UART_PORT);
            g_u0char_available=1;
            break;
        
        case 0xc:
            // Character Time-out indication
            temp = CONSOLE_UART_PORT->RBR;
            break;
        case 2:
            // THRE
            temp = CONSOLE_UART_PORT->IIR;
            break;
        default:
            break;
    }
   
   return;
}

/* *** ********************************************************** */

int getchar_nb(void)
{
    // XXX TODO: add queue in rx interrupt routine
    
    if (g_u0char_available) {
        g_u0char_available=0;
        return g_u0char;
    }
    
    g_u0char_available=0;
    return -1;

}

int __getchar(void)
{
   int ch = 0;

   while(!g_u0char_available)
      ;

   ch = g_u0char;
   g_u0char_available = 0;
   
   return ch;
}

void __putchar(int ch)
{
   unsigned char buf[2] = {0};

   buf[0] = ch & 0xff;

   if (ch == 0xa) {
      buf[0] = 0xa;
      buf[1] = 0;
      //UART_Send((LPC_UART_TypeDef *)CONSOLE_UART_PORT, (unsigned char *)buf, 2, BLOCKING);
      UART_Send((LPC_UART_TypeDef *)CONSOLE_UART_PORT, (unsigned char *)buf, 1, BLOCKING);

      buf[0] = 0xd;
      buf[1] = 0;
      UART_Send((LPC_UART_TypeDef *)CONSOLE_UART_PORT, (unsigned char *)buf, 1, BLOCKING);

   } else {
      UART_Send((LPC_UART_TypeDef *)CONSOLE_UART_PORT, (unsigned char *)buf, 1, BLOCKING);
   }
}

/* *** ********************************************************** */

void uart1_init(const uint32_t BaudRate, const bool DoubleSpeed)
{
    UART_CFG_Type UARTConfigStruct;
    PINSEL_CFG_Type PinCfg;

    PinCfg.Portnum      = UART1_PORTNUM;
    PinCfg.Pinnum       = UART1_TX_PINNUM;
    PinCfg.Funcnum      = 0; // XXX UART1_FUNCNUM;
    PinCfg.OpenDrain    = 0;
    PinCfg.Pinmode      = 0;

    PINSEL_ConfigPin(&PinCfg);

    PinCfg.Pinnum       = UART1_RX_PINNUM;
    PinCfg.Funcnum      = UART1_FUNCNUM;
    PINSEL_ConfigPin(&PinCfg);

    /* Initialize UART Configuration parameter structure to default state:
     * Baudrate = 9600bps
     * 8 data bit
     * 1 Stop bit
     * None parity
     */
    UART_ConfigStructInit(&UARTConfigStruct);
    // Re-configure baudrate
    UARTConfigStruct.Baud_rate = BaudRate;

    // Initialize DEBUG_UART_PORT peripheral with given to corresponding parameter
    UART_Init((LPC_UART_TypeDef *)FPGA_UART1_PORT, &UARTConfigStruct);

    // Enable UART Transmit
    // XXX UART_TxCmd((LPC_UART_TypeDef *)FPGA_UART1_PORT, ENABLE);
    
    UART_IntConfig((LPC_UART_TypeDef *)FPGA_UART1_PORT, UART_INTCFG_RBR, ENABLE);
    NVIC_EnableIRQ(UART1_IRQn);
}

volatile unsigned char g_u1char = 0;
volatile unsigned char g_u1char_available = 0;

void UART1_IRQHandler(void)
{
    g_u1char_available=0;
    g_u1char = 0;

    unsigned int id = UART_GetIntId((LPC_UART_TypeDef *)FPGA_UART1_PORT);
    unsigned int temp = 0;

    switch(id) {
        case 6:
            // RX Line Status / Error
            temp = FPGA_UART1_PORT->LSR;
            break;
            
        case 4:
            g_u1char = UART_ReceiveByte((LPC_UART_TypeDef *)FPGA_UART1_PORT);
            g_u1char_available=1;
            break;
        
        case 0xc:
            // Character Time-out indication
            temp = FPGA_UART1_PORT->RBR;
            break;
        case 2:
            // THRE
            temp = FPGA_UART1_PORT->IIR;
            break;
        default:
            break;
    }
   
   return;
}

int getchar1_nb(void)
{

    // XXX TODO: add queue in rx interrupt routine

    if (g_u1char_available) {
        g_u1char_available=0;
        return g_u1char;
    }
    
    g_u1char_available=0;
    return -1;
}


int getchar1(void)
{
   int ch = 0;

   while(!g_u1char_available)
      ;

   ch = g_u1char_available;
   g_u1char_available = 0;
   
   return ch;
}




/* *** ********************************************************** */

void uart2_init(const uint32_t BaudRate, const bool DoubleSpeed)
{
    UART_CFG_Type UARTConfigStruct;
    PINSEL_CFG_Type PinCfg;

    PinCfg.Portnum = UART2_PORTNUM;
    PinCfg.Pinnum = UART2_TX_PINNUM;
    PinCfg.Funcnum = UART2_FUNCNUM;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = UART2_RX_PINNUM;
    PINSEL_ConfigPin(&PinCfg);

    /* Initialize UART Configuration parameter structure to default state:
     * Baudrate = 9600bps
     * 8 data bit
     * 1 Stop bit
     * None parity
     */
    UART_ConfigStructInit(&UARTConfigStruct);
    // Re-configure baudrate
    UARTConfigStruct.Baud_rate = BaudRate;

    // Initialize DEBUG_UART_PORT peripheral with given to corresponding parameter
    UART_Init((LPC_UART_TypeDef *)FPGA_UART_PORT, &UARTConfigStruct);

    // Enable UART Transmit
    UART_TxCmd((LPC_UART_TypeDef *)FPGA_UART_PORT, ENABLE);
    
    UART_IntConfig((LPC_UART_TypeDef *)FPGA_UART_PORT, UART_INTCFG_RBR, ENABLE);
    NVIC_EnableIRQ(UART2_IRQn);
}

volatile unsigned char g_u2char = 0;
volatile unsigned char g_u2char_available = 0;

void UART2_IRQHandler(void)
{
    g_u2char_available=0;
    g_u2char = 0;

    unsigned int id = UART_GetIntId((LPC_UART_TypeDef *)FPGA_UART_PORT);
    unsigned int temp = 0;

    switch(id) {
        case 6:
            // RX Line Status / Error
            temp = FPGA_UART_PORT->LSR;
            break;
            
        case 4:
            g_u2char = UART_ReceiveByte((LPC_UART_TypeDef *)FPGA_UART_PORT);
            g_u2char_available=1;
            break;
        
        case 0xc:
            // Character Time-out indication
            temp = FPGA_UART_PORT->RBR;
            break;
        case 2:
            // THRE
            temp = FPGA_UART_PORT->IIR;
            break;
        default:
            break;
    }
   
   return;
}





/* *** ********************************************************** */



int getchar2_nb(void)
{

    // XXX TODO: add queue in rx interrupt routine

    if (g_u2char_available) {
        g_u2char_available=0;
        return g_u2char;
    }
    
    g_u2char_available=0;
    return -1;
}


int getchar2(void)
{
   int ch = 0;

   while(!g_u2char_available)
      ;

   ch = g_u2char_available;
   g_u2char_available = 0;
   
   return ch;
}

void putchar2(int ch)
{
   unsigned char buf[2] = {0};

   buf[0] = ch;

   if (ch == '\n') {
      buf[1] = '\n';
      buf[0] = '\r';
      UART_Send((LPC_UART_TypeDef *)FPGA_UART_PORT, (unsigned char *)buf, 2, BLOCKING);
   } else {
      UART_Send((LPC_UART_TypeDef *)FPGA_UART_PORT, (unsigned char *)buf, 1, BLOCKING);
   }
}

/* *** ********************************************************** */

void uart3_init(const uint32_t BaudRate, const bool DoubleSpeed)
{
    UART_CFG_Type UARTConfigStruct;
    PINSEL_CFG_Type PinCfg;

    PinCfg.Portnum = UART3_PORTNUM;
    PinCfg.Pinnum  = UART3_RX_PINNUM;
    PinCfg.Funcnum = UART3_FUNCNUM;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PINSEL_ConfigPin(&PinCfg);
    
    // leave TX3 untouched, we are currently using TX3 as i/o!
    //PinCfg.Pinnum = UART3_TX_PINNUM;
    //PINSEL_ConfigPin(&PinCfg);

    /* Initialize UART Configuration parameter structure to default state:
     * Baudrate = 9600bps
     * 8 data bit
     * 1 Stop bit
     * None parity
     */
    UART_ConfigStructInit(&UARTConfigStruct);
    // Re-configure baudrate
    UARTConfigStruct.Baud_rate = BaudRate;

    // Initialize DEBUG_UART_PORT peripheral with given to corresponding parameter
    UART_Init((LPC_UART_TypeDef *)FPGA_UART3_PORT, &UARTConfigStruct);

    // Enable UART Transmit
    //UART_TxCmd((LPC_UART_TypeDef *)FPGA_UART3_PORT, ENABLE);
    
    UART_IntConfig((LPC_UART_TypeDef *)FPGA_UART3_PORT, UART_INTCFG_RBR, ENABLE);
    NVIC_EnableIRQ(UART3_IRQn);
}

volatile unsigned char g_u3char = 0;
volatile unsigned char g_u3char_available = 0;

void UART3_IRQHandler(void)
{
    g_u3char_available=0;
    g_u3char = 0;

    unsigned int id = UART_GetIntId((LPC_UART_TypeDef *)FPGA_UART3_PORT);
    unsigned int temp = 0;

    switch(id) {
        case 6:
            // RX Line Status / Error
            temp = FPGA_UART3_PORT->LSR;
            break;
            
        case 4:
            g_u3char = UART_ReceiveByte((LPC_UART_TypeDef *)FPGA_UART3_PORT);
            g_u3char_available=1;
            break;
        
        case 0xc:
            // Character Time-out indication
            temp = FPGA_UART3_PORT->RBR;
            break;
        case 2:
            // THRE
            temp = FPGA_UART3_PORT->IIR;
            break;
        default:
            break;
    }
   
   return;
}





/* *** ********************************************************** */



int getchar3_nb(void)
{

    // XXX TODO: add queue in rx interrupt routine

    if (g_u3char_available) {
        g_u3char_available=0;
        return g_u3char;
    }
    
    g_u3char_available=0;
    return -1;
}


int getchar3(void)
{
   int ch = 0;

   while(!g_u3char_available)
      ;

   ch = g_u3char_available;
   g_u3char_available = 0;
   
   return ch;
}

void putchar3(int ch)
{
   unsigned char buf[2] = {0};

   buf[0] = ch;

   if (ch == '\n') {
      buf[1] = '\n';
      buf[0] = '\r';
      UART_Send((LPC_UART_TypeDef *)FPGA_UART3_PORT, (unsigned char *)buf, 2, BLOCKING);
   } else {
      UART_Send((LPC_UART_TypeDef *)FPGA_UART3_PORT, (unsigned char *)buf, 1, BLOCKING);
   }
}



