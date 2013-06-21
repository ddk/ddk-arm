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
 * File: stats.c
 * Description: Implementation of all memory & scheduler statistics related functions.
 */

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"

#include "stats.h"


void vPortUsedMem(int *, int *, int *);


static char buffer[256];
static int bytesFree;
static int bytesUsed;
static int blocksFree;
static void stats_fail(int);

int stats_print_memusage(void)
{
#if configUSE_TRACE_FACILITY == 1
   vPortUsedMem (&bytesFree, &bytesUsed, &blocksFree);
   printf ("Heap size=%ld, used=%d, free=%d (%d blocks)\n", configTOTAL_HEAP_SIZE, bytesUsed, bytesFree, blocksFree);
#else
   stats_fail(0);
#endif
   return 0;
}



int stats_print_tasklist(void)
{
#if configUSE_TRACE_FACILITY == 1
   
   vTaskList (buffer);
   printf ("Task\t\tState\tPrty\tStack\tTask#\n");
   printf ("%s\n", buffer);
#else
   stats_fail(0);
#endif
   return 0;
}

static void stats_fail(int e)
{
   switch(e) {
      case 0:
         #ifdef DEBUG
         printf("[e] stats: Not implemented (requires configUSE_TRACE_FACILITY in FreeRTOSConfig.h)\n");
         #endif
         break;
      case 1:
      default:
         printf("[e] stats: unknown error.\n");
         break;
   }
   return;
}