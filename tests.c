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
 * File: tests.c
 * Description: Implementation of all hardware function test related functions.
 */

#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"

#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#include "main.h"
#include "led.h"
#include "buffer.h"
#include "uart.h"
#include "io.h"
#include "jtag_1149.h"
#include "tests.h"
#include "dpuser.h"
#include "main.h"

/*-----------------------------------------------------------*/

/* The time between cycles of the 'check' functionality (defined within the
tick hook). */
#define mainCHECK_DELAY      ( ( portTickType ) 5000 / portTICK_RATE_MS )


#define TEST_V1 0x3F
#define TEST_V2 0x2A
#define TEST_V3 0x15
#define TEST_V4 0x00



extern xTaskHandle task_handles[TASKHANDLE_LAST];
extern unsigned int g_test_done;

static uint8_t test_fpga_jtag()
{
    uint8_t i = 0;
    printf("test_fpga_jtag - not implemented\n");

    return i;
}

static   PINSEL_CFG_Type PinCfg;

static void hw_test_init(void);

static uint32_t test_vector_read(uint32_t port)
{
    uint32_t result = 0;
    uint32_t temp = 0;

    if(!port) { // p1.16 - p1.21
        temp = GPIO_ReadValue(1);
        result = (temp >> 16) & 0x3f;

    } else { // 1.22, p1.23, p2.3-p2.0
        result = GPIO_ReadValue(2) & 0x0f;

        result = result << 2;
        temp = GPIO_ReadValue(1);

        result |= (temp >> 22) & 3;
    }

    return result;
}

static uint32_t test_vector_set(uint32_t val)
{
    val &= 0x3f;

    GPIO_ClearValue(0, 0x03);
    GPIO_ClearValue(2, 0xf0);

    GPIO_SetValue(0, val >> 4);
    GPIO_SetValue(2, (val & 0xf) << 4);

    return val;
}


/**
 * DDK databus main task.
 */
void ddk_functiontests(void)
{
    unsigned int dir=0,v=0;
    uint32_t i = 0;
    uint32_t error = 0;

    puts("[*] HW function tests.");
    dp_prog_main();

    if(g_test_done) 
        return;

    buf1_enable();
    buf2_enable();
    buf3_enable();
    buf4_enable();
    buf5_enable();
    buf6_enable();
    buf7_enable();
    buf8_enable();

    // set configure and use TXD3 as test vector input bit 5
    io_init_fpga_dclk();

    // set configure and use TXD3 as test vector input bit 4
    io_init_fpga_dwe();

    for(v=7; v>3; v--) {
        PinCfg.Funcnum      = 0;    // GPIO
        PinCfg.OpenDrain    = 0;
        PinCfg.Pinmode      = 0;
        PinCfg.Portnum      = 2;
        PinCfg.Pinnum       = v;

        PINSEL_ConfigPin(&PinCfg);
        GPIO_SetDir(2, 1<<v, POUT);
    }

    // vector input ports

    for(v=0; v<4; v++) {
        PinCfg.Funcnum      = 0;    // GPIO
        PinCfg.OpenDrain    = 0;
        PinCfg.Pinmode      = 0;
        PinCfg.Portnum      = 2;
        PinCfg.Pinnum       = v;

        PINSEL_ConfigPin(&PinCfg);
        GPIO_SetDir(2, 1<<v, PIN);
    }

    for(v=16; v<24; v++) {
        PinCfg.Funcnum      = 0;    // GPIO
        PinCfg.OpenDrain    = 0;
        PinCfg.Pinmode      = 0;
        PinCfg.Portnum      = 1;
        PinCfg.Pinnum       = v;

        PINSEL_ConfigPin(&PinCfg);
        GPIO_SetDir(1, 1<<v, PIN);
    }

    fpga_rst_low();
    fpga_tst_high();

    
    for(dir=0; dir<2; dir++) {

        if(dir == 1) {
            fpga_tmd_high();
            puts("[*] Testing CH5-8");
        } else {
            fpga_tmd_low();
            puts("[*] Testing CH1-4");
        }
        
        printf("[+] test vector: %x: ", test_vector_set(TEST_V1));

        if(test_vector_read(dir) == TEST_V1)
            puts("OK");
        else {
            puts("FAILED");
            error = 1;
        }
        printf("[+] test vector: %x: ", test_vector_set(TEST_V2));

        if(test_vector_read(dir) == TEST_V2)
            puts("OK");
        else {
            puts("FAILED");
            error = 1;
        }

        printf("[+] test vector: %x: ", test_vector_set(TEST_V3));

        if(test_vector_read(dir) == TEST_V3)
            puts("OK");
        else {
            puts("FAILED");
            error = 1;
        }

        printf("[+] test vector: %x: ", test_vector_set(TEST_V4));

        if(test_vector_read(dir) == TEST_V4)
            puts("OK");
        else {
            puts("FAILED");
            error = 1;
        }
    }
    
    if(error == 1)
        g_test_done = 0;
    else
        g_test_done = 1;

    return;
}


/**
 * DDK manual function tests.
 */
void ddk_manual_tests(void)
{

    unsigned int dir=0,v=0;
    uint32_t i = 0;
    uint32_t error = 0;

    hw_test_init();
    jtag_init();

    buf1_enable();
    buf2_enable();
    buf3_enable();
    buf4_enable();
    buf5_enable();
    buf6_enable();
    buf7_enable();
    buf8_enable();

    // set configure and use TXD3 as test vector input bit 5
    io_init_fpga_dclk();

    // set configure and use TXD3 as test vector input bit 4
    io_init_fpga_dwe();

    for(v=7; v>3; v--) {
        PinCfg.Funcnum      = 0;    // GPIO
        PinCfg.OpenDrain    = 0;
        PinCfg.Pinmode      = 0;
        PinCfg.Portnum      = 2;
        PinCfg.Pinnum       = v;

        PINSEL_ConfigPin(&PinCfg);
        GPIO_SetDir(2, 1<<v, POUT);
    }

    // vector input ports

    for(v=0; v<4; v++) {
        PinCfg.Funcnum      = 0;    // GPIO
        PinCfg.OpenDrain    = 0;
        PinCfg.Pinmode      = 0;
        PinCfg.Portnum      = 2;
        PinCfg.Pinnum       = v;

        PINSEL_ConfigPin(&PinCfg);
        GPIO_SetDir(2, 1<<v, PIN);
    }

    for(v=16; v<24; v++) {
        PinCfg.Funcnum      = 0;    // GPIO
        PinCfg.OpenDrain    = 0;
        PinCfg.Pinmode      = 0;
        PinCfg.Portnum      = 1;
        PinCfg.Pinnum       = v;

        PINSEL_ConfigPin(&PinCfg);
        GPIO_SetDir(1, 1<<v, PIN);
    }

    fpga_rst_low();
    fpga_tst_high();

    
    for(dir=0; dir<2; dir++) {

        if(dir == 1) {
            fpga_tmd_high();
            puts("[*] Testing CH5-8");
        } else {
            fpga_tmd_low();
            puts("[*] Testing CH1-4");
        }
        
        printf("[+] test vector: %x: ", test_vector_set(TEST_V1));

        if(test_vector_read(dir) == TEST_V1)
            puts("OK");
        else {
            puts("FAILED");
            error = 1;
        }
        printf("[+] test vector: %x: ", test_vector_set(TEST_V2));

        if(test_vector_read(dir) == TEST_V2)
            puts("OK");
        else {
            puts("FAILED");
            error = 1;
        }

        printf("[+] test vector: %x: ", test_vector_set(TEST_V3));

        if(test_vector_read(dir) == TEST_V3)
            puts("OK");
        else {
            puts("FAILED");
            error = 1;
        }

        printf("[+] test vector: %x: ", test_vector_set(TEST_V4));

        if(test_vector_read(dir) == TEST_V4)
            puts("OK");
        else {
            puts("FAILED");
            error = 1;
        }
    }
    
    if(error == 1)
        puts("An error occured!");

    uart0_shutdown();
    NVIC_SystemReset();

    return;
}


static void hw_test_init(void)
{
    // 1. first, bring buffer enable into well defined state
    buffer_init();

    buf1_disable();
    buf2_disable();
    buf3_disable();
    buf4_disable();
    buf5_disable();
    buf6_disable();
    buf7_disable();
    buf8_disable();
    fpga_rst_high();

    // 2. visuals
    led_init();

    led1_clr();
    led2_clr();
    led3_clr();
    led4_clr();

    // 3. comms
    //uart0_init(115200, 0);
    //uart1_init(115200, 0);

    // 5. init misc i/o
    io_init();
    io_fpga_disable();

    // 6. start fpga

    // init some i/o lines for test mode
    io_init_fpga_tmd();
    io_init_fpga_rst();
    fpga_rst_high();
    io_init_fpga_tst();
    io_init_fpga_dwe();

    // enable test mode
    fpga_tst_high();

    // power-on FPGA
    io_fpga_enable();

    // enable systick interrupt. (1msec)
    SysTick_Config(SystemCoreClock/1000 - 1); /* Generate interrupt each 1 ms   */
    SYSTICK_Cmd(DISABLE);

    fpga_rst_low();
}


int hw_test_main(void)
{
    portBASE_TYPE ret = 0;
    unsigned int dir = 0;

    unsigned int v=0;

    hw_test_init();
    jtag_init();
    
    ddk_functiontests();

    fpga_tst_low();
    io_fpga_disable();
    puts("Tests done");
    
    io_init();
    io_fpga_enable();

    return 0;
}
