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