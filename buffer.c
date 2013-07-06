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
 * File: buffer.c
 * Description: Implementation of all GPIO related functions.
 */

#include "buffer.h"

void init_buf1(void)
{
   BUF1DIR  |=  BUF1;    // output
   BUF1MASK &= ~BUF1;    // in/out mask for reads and writes
   BUF1SET   =  BUF1;    // turn it off
   return;
}

void init_buf2(void)
{
   BUF2DIR  |=  BUF2;    // output
   BUF2MASK &= ~BUF2;    // in/out mask for reads and writes
   BUF2SET   =  BUF2;    // turn it off
   return;
}

void init_buf3(void)
{
   BUF3DIR  |=  BUF3;    // output
   BUF3MASK &= ~BUF3;    // in/out mask for reads and writes
   BUF3SET   =  BUF3;    // turn it off
   return;
}

void init_buf4(void)
{
   BUF4DIR  |=  BUF4;    // output
   BUF4MASK &= ~BUF4;    // in/out mask for reads and writes
   BUF4SET   =  BUF4;    // turn it off
   return;
}

void init_buf5(void)
{
   BUF5DIR  |=  BUF5;    // output
   BUF5MASK &= ~BUF5;    // in/out mask for reads and writes
   BUF5SET   =  BUF5;    // turn it off
   return;
}

void init_buf6(void)
{
   BUF6DIR  |=  BUF6;    // output
   BUF6MASK &= ~BUF6;    // in/out mask for reads and writes
   BUF6SET   =  BUF6;    // turn it off
   return;
}

void init_buf7(void)
{
   BUF7DIR  |=  BUF7;    // output
   BUF7MASK &= ~BUF7;    // in/out mask for reads and writes
   BUF7SET   =  BUF7;    // turn it off
   return;
}

void init_buf8(void)
{
   BUF8DIR  |=  BUF8;    // output
   BUF8MASK &= ~BUF8;    // in/out mask for reads and writes
   BUF8SET   =  BUF8;    // turn it off
   return;
}


void buffer_init(void)
{
    LPC_SC->PCONP |= PCONP_PCGPIO;

    init_buf1();
    init_buf2();
    init_buf3();
    init_buf4();
    init_buf5();
    init_buf6();
    init_buf7();
    init_buf8();

    return;
}


