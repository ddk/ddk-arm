/*
 * Copyright (c) 2013, The DDK Project
 *    Dmitry Nedospasov <dmitry at nedos dot net>
 *    Thorsten Schroeder <ths at modzero dot ch>
 *    Andrew Karpow <andy at ndyk dot de>
 *
 * All rights reserved.
 *
 * This file is part of Die Datenkrake (DDK).
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <dmitry at nedos dot net>, <ths at modzero dot ch> and <andy at ndyk dot de>
 *  wrote this file. As long as you retain this notice you can do whatever 
 * you want with this stuff. If we meet some day, and you think this stuff is 
 * worth it, you can buy us a beer in return. Die Datenkrake Project.
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
 * File: syscalls.c
 * Description: Newlib syscall wrapper for libc compatibility
 *
 */

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#include "uart.h"

#undef errno
extern int errno;

int _read(int file, char *ptr, int len)
{
    char c;
    int  i;

    for (i = 0; i < len; i++)
    {
        c = __getchar();

        *ptr++ = c;
        __putchar(c);

        if(c == '\r' && i < len - 1)
        {
            *ptr = '\n';
            __putchar(*ptr);
            i++;
            break;
        }
    }
    return i;
}

int _write(int file, char *ptr, int len)
{
    int i;

    for (i = 0; i < len; i++)
        __putchar(*ptr++);

    return len;
}

int _close(int file) {
    return 0;
}


int _lseek(int file, int ptr, int dir)
{
    return 0;
}

int _isatty(int file) {
    return 1;
}

int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

extern char __cs3_heap_end[]; /* end is set in the linker command   */
static char *heap_ptr;        /* Points to current end of the heap. */

void *_sbrk(ptrdiff_t nbytes)
{
    char *base;   /* errno should be set to  ENOMEM on error */

    if (!heap_ptr) {        /* Initialize if first time through. */
        heap_ptr = __cs3_heap_end;
    }
    base = heap_ptr;        /* Point to end of heap. */
    heap_ptr += nbytes;     /* Increase heap. */

    return base;            /* Return pointer to start of new heap area. */
}
