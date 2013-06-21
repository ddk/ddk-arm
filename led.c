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
 * File: led.c
 * Description: Implementation of LED related functions.
 *
 */


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"

#include "led.h"

void led_init(void)
{
    LPC_SC->PCONP |= PCONP_PCGPIO;

    init_led1();
    init_led2();
    init_led3();
    init_led4();
    
    return;
}

void init_led4(void)
{
   LED4DIR  |=  LED4;    // output
   LED4MASK &= ~LED4;    // in/out mask for reads and writes
   LED4SET   =  LED4;    // turn it off
   return;
}

void init_led3(void)
{
   LED3DIR  |=  LED3;    // output
   LED3MASK &= ~LED3;    // in/out mask for reads and writes
   LED3CLR   =  LED3;    // turn it off
   return;
}

void init_led2(void)
{
   LED2DIR  |=  LED2;    // output
   LED2MASK &= ~LED2;    // in/out mask for reads and writes
   LED2SET   =  LED2;    // turn it off
   return;
}

void init_led1(void)
{
   LED1DIR  |=  LED1;    // output
   LED1MASK &= ~LED1;    // in/out mask for reads and writes
   LED1SET   =  LED1;    // turn it off
   return;
}

void vBlinkTask(void *p)
{  
    unsigned int v=0;
    const portTickType xDelay = 10 / portTICK_RATE_MS;

    ( void ) p;

    // do nothing useful
    while (1) {

        led4_set();
        vTaskDelay( FLASH_MS / portTICK_RATE_MS );
        led4_clr();
        vTaskDelay( 2000 / portTICK_RATE_MS );

/*
        led1_set();
        vTaskDelay( FLASH_MS / portTICK_RATE_MS );
        led1_clr();
        vTaskDelay( WAIT_MS / portTICK_RATE_MS );

        led2_set();
        vTaskDelay( FLASH_MS / portTICK_RATE_MS );
        led2_clr();
        vTaskDelay( WAIT_MS / portTICK_RATE_MS );

        led3_set();
        vTaskDelay( FLASH_MS / portTICK_RATE_MS );
        led3_clr();
        vTaskDelay( WAIT_MS / portTICK_RATE_MS );

        led4_set();
        vTaskDelay( FLASH_MS / portTICK_RATE_MS );
        led4_clr();
        vTaskDelay( WAIT_MS / portTICK_RATE_MS );

        led4_set();
        vTaskDelay( FLASH_MS / portTICK_RATE_MS );
        led4_clr();
        vTaskDelay( WAIT_MS / portTICK_RATE_MS );

        led3_set();
        vTaskDelay( FLASH_MS / portTICK_RATE_MS );
        led3_clr();
        vTaskDelay( WAIT_MS / portTICK_RATE_MS );

        led2_set();
        vTaskDelay( FLASH_MS / portTICK_RATE_MS );
        led2_clr();
        vTaskDelay( WAIT_MS / portTICK_RATE_MS );

        led1_set();
        vTaskDelay( FLASH_MS / portTICK_RATE_MS );
        led1_clr();
        vTaskDelay( WAIT_MS / portTICK_RATE_MS );

        //ctl_timeout_wait(ctl_get_current_time() + 1000);

        // task logic goes here      
        v++;
     */
    }  
}



