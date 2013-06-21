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
 * File: main.h
 * Description: Header and definitions of main.
 */

#ifndef _MAIN_H
#define _MAIN_H
void Delay (unsigned long);
void ddk_init(void);

#include "helper.h"
#define Delay_us _DELAY_US


#define PROJECT_NAME "Die Datenkrake "
#define PROJECT_REVISION "1.0.0 (20130614)"

#define STR(x) (signed char *)((x))
#define _STR (signed char *)
#define _BIN (uint8_t *)


// config stuff:


typedef enum __task_handles {
   TASK_MAIN,
   TASK_CLI_MAIN,
   TASK_BLINK_MAIN,
   TASK_UART0_MAIN,
   TASK_UART1_MAIN,
   TASK_UART2_MAIN,
   TASK_UART3_MAIN,
   TASK_DATABUS_MAIN,
   TASK_FUNCTONTEST,
   TASKHANDLE_LAST
} TaskHandle_t;

#define mainCHECK_DELAY                         ( ( portTickType ) 5000 / portTICK_RATE_MS )

#define mainBASIC_BLINK_STACK_SIZE           ( configMINIMAL_STACK_SIZE   + 10   )
#define mainBASIC_UART0_STACK_SIZE           ( configMINIMAL_STACK_SIZE*2 + 20   )
#define mainBASIC_UART1_STACK_SIZE           ( configMINIMAL_STACK_SIZE*2 + 20   )
#define mainBASIC_UART2_STACK_SIZE           ( configMINIMAL_STACK_SIZE*2 + 20   )
#define mainBASIC_UART3_STACK_SIZE           ( configMINIMAL_STACK_SIZE*2 + 20   )
#define mainBASIC_DATABUS_STACK_SIZE         ( configMINIMAL_STACK_SIZE*2 + 20   )
#define mainBASIC_TESTMODE_STACK_SIZE        ( configMINIMAL_STACK_SIZE*2 + 20   )

#define mainBASIC_CLI_STACK_SIZE             ( configMINIMAL_STACK_SIZE*6 + 20   )



#endif

