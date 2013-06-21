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
 * File: main.c
 * Description: Implementation of main.
 */


#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"

#include "main.h"
#include "led.h"
#include "buffer.h"
#include "uart.h"
#include "io.h"
#include "tests.h"
#include "cli.h"


#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_systick.h"

#include "jtag_1149.h"


/*-----------------------------------------------------------*/

/* The time between cycles of the 'check' functionality (defined within the
tick hook). */
#define mainCHECK_DELAY      ( ( portTickType ) 5000 / portTICK_RATE_MS )


/************************** PRIVATE VARIABLES *************************/
/* SysTick Counter */
volatile unsigned long SysTickCnt;

/************************** PRIVATE FUNCTIONS *************************/
void SysTick_Handler (void);
void xPortSysTickHandler(void);
void Delay (unsigned long tick);


/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware( void );


xTaskHandle task_handles[TASKHANDLE_LAST];
unsigned int g_test_done = 0;

void ddk_init(void);

#define MEMORY_BARRIER __asm__ volatile (                         \
      "" :      /* inline assembly code: effectively nothing */   \
      :         /* list of outputs: empty */                      \
      :         /* list of inputs: empty */                       \
      "memory"  /* clobbered data: memory */                      \
   )

static inline void ddk_delay(unsigned long microseconds)
{
    unsigned long ticks = microseconds * 100;
    while(ticks--)
        MEMORY_BARRIER;
}

int main(void)
{
    portBASE_TYPE ret = 0;

    unsigned int v=0;

    ddk_init();

    // call HW function tests and FPGA ROM programming. This will program the FPGA bitstream
    // into flash ROM if FPGA is uninitialized.
    hw_test_main();

    led1_clr();
    led2_clr();
    led3_clr();
    led4_clr();


    // error
    while(g_test_done != 1) {
        led1_set();
        led2_set();
        led3_set();
        led4_set();
        ddk_delay(50000);
        led1_clr();
        led2_clr();
        led3_clr();
        led4_clr();
        ddk_delay(50000);
    }
    
    
    puts("[*] init done.");

    ret = xTaskCreate( vBlinkTask,
        _STR "BLINK",
        mainBASIC_BLINK_STACK_SIZE,
        ( void * ) NULL,
        tskIDLE_PRIORITY,
        &task_handles[TASK_BLINK_MAIN]
        );

    if(ret != pdTRUE)
    for(;;);

/*
    ret = xTaskCreate( vUART0Task,
        _STR "UART0",
        mainBASIC_UART0_STACK_SIZE,
        ( void * ) NULL,
        tskIDLE_PRIORITY,
        &task_handles[TASK_UART0_MAIN]
        );

    if(ret != pdTRUE)
    for(;;);

    ret = xTaskCreate( vUART2Task,
        _STR "UART2",
        mainBASIC_UART2_STACK_SIZE,
        ( void * ) NULL,
        tskIDLE_PRIORITY,
        &task_handles[TASK_UART2_MAIN]
        );

    if(ret != pdTRUE)
    for(;;);
*/

#ifdef WITH_UART3
    ret = xTaskCreate( vUART3Task,
        _STR "UART3",
        mainBASIC_UART3_STACK_SIZE,
        ( void * ) NULL,
        tskIDLE_PRIORITY,
        &task_handles[TASK_UART3_MAIN]
        );

    if(ret != pdTRUE)
    for(;;);
#endif


   ret = xTaskCreate( vCLITask,  
      _STR"CLI", 
      mainBASIC_CLI_STACK_SIZE, 
      ( void * ) NULL,       
      tskIDLE_PRIORITY,
      &task_handles[TASK_CLI_MAIN] );


   if(ret != pdTRUE)
      for(;;);


    vTaskStartScheduler();

    while (1) {
      // power down can go here if supported
      v++;
    }

    return 0;
}

void ddk_init(void)
{
    // 1. first, bring buffer enable into well defined state
    buffer_init();
    buf1_enable();
    buf2_enable();
    buf3_disable();
    buf4_disable();
    buf5_disable();
    buf6_disable();
    buf7_disable();
    buf8_disable();

    // 2. visuals
    led_init();
    led1_clr();
    led2_clr();
    led3_clr();
    led4_clr();

    // 3. comms
    uart0_init(115200, 0);
    uart2_init(115200, 0);

#ifdef WITH_UART3
    uart3_init(115200, 0); // temporarily disabled and used as i/o
#endif
    // 4. enable certain buffers if necessary
    //buf1_enable();
    //buf2_enable();

    // 5. init misc i/o
    io_init();

    // 6. start fpga
    io_fpga_enable();
    //io_fpga_disable();

    // enable systick interrupt. (1msec)
    SysTick_Config(SystemCoreClock/1000 - 1); /* Generate interrupt each 1 ms   */
    SYSTICK_Cmd(DISABLE);
}

void SysTick_Handler (void) {
    //vApplicationTickHook();
    xPortSysTickHandler();
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief     Delay function
 * @param[in]  tick - number milisecond of delay time
 * @return     None
 **********************************************************************/
void Delay (unsigned long tick)
{
  unsigned long systickcnt;

  systickcnt = SysTickCnt;
  while ((SysTickCnt - systickcnt) < tick);
}

/*-----------------------------------------------------------*/
void vApplicationTickHook( void )
{
   SysTickCnt++;

#if 0
   static unsigned long ulTicksSinceLastDisplay = 0;

   /* Called from every tick interrupt as described in the comments at the top
   of this file.

   Have enough ticks passed to make it   time to perform our health status
   check again? */
   ulTicksSinceLastDisplay++;
   if( ulTicksSinceLastDisplay >= mainCHECK_DELAY )
   {
      /* Reset the counter so these checks run again in mainCHECK_DELAY
      ticks time. */
      ulTicksSinceLastDisplay = 0;
   }
#endif
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
   /* This function will get called if a task overflows its stack. */

   ( void ) pxTask;
   ( void ) pcTaskName;

   for( ;; );
}
/*-----------------------------------------------------------*/

#if 0
void v_ConfigureTimerForRunTimeStats( void )
{
const unsigned long TCR_COUNT_RESET = 2, CTCR_CTM_TIMER = 0x00, TCR_COUNT_ENABLE = 0x01;

   /* This function configures a timer that is used as the time base when
   collecting run time statistical information - basically the percentage
   of CPU time that each task is utilising.  It is called automatically when
   the scheduler is started (assuming configGENERATE_RUN_TIME_STATS is set
   to 1). */

   /* Power up and feed the timer. */
   LPC_SC->PCONP |= 0x02UL;
   LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & (~(0x3<<2))) | (0x01 << 2);

   /* Reset Timer 0 */
   LPC_TIM0->TCR = TCR_COUNT_RESET;

   /* Just count up. */
   LPC_TIM0->CTCR = CTCR_CTM_TIMER;

   /* Prescale to a frequency that is good enough to get a decent resolution,
   but not too fast so as to overflow all the time. */
   LPC_TIM0->PR =  ( configCPU_CLOCK_HZ / 10000UL ) - 1UL;

   /* Start the counter. */
   LPC_TIM0->TCR = TCR_COUNT_ENABLE;
}
/*-----------------------------------------------------------*/
#endif

