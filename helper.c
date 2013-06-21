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
 * File: helper.c
 * Description: Implementation of helper functions.
 */


#include <string.h>
#include <ctype.h>

#include <stdio.h>

#ifndef build_date
const char *build_date = __DATE__;
#endif
#ifndef build_time
const char *build_time = __TIME__;
#endif

#include "helper.h"
#include "uart.h"
#include "main.h"


#ifndef putc
#define putc(a,b) putchar((b))
#endif

#ifndef stdout
#define stdout 0
#endif

// bin to hexascii nibble
static const unsigned char _tab[] = "0123456789ABCDEF";

void print_hex_byte(char b)
{
    putchar( _tab[ (b >> 4) & 0xF ] );
    putchar( _tab[ (b >> 0) & 0xF ] );
}

void print_hex_word(int16_t w)
{
    putchar( _tab[ (w >> 12) & 0xF ] );
    putchar( _tab[ (w >>  8) & 0xF ] );
    putchar( _tab[ (w >>  4) & 0xF ] );
    putchar( _tab[ (w >>  0) & 0xF ] );
}


void print_hex_dword(int32_t d)
{
    putchar( _tab[ (d >> 28) & 0xF ] );
    putchar( _tab[ (d >> 24) & 0xF ] );
    putchar( _tab[ (d >> 20) & 0xF ] );
    putchar( _tab[ (d >> 16) & 0xF ] );
    putchar( _tab[ (d >> 12) & 0xF ] );
    putchar( _tab[ (d >>  8) & 0xF ] );
    putchar( _tab[ (d >>  4) & 0xF ] );
    putchar( _tab[ (d >>  0) & 0xF ] );
}

void print_hex_buf(char *s, uint32_t l)
{
    
    while(l--) {
        putchar( _tab[ (*s >> 4) & 0xF ] );
        putchar( _tab[ (*s >> 0) & 0xF ] );
        s++;
    }
}


/*
 * print data in rows of 16 bytes: offset   hex   ascii
 *
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 */
void print_hex_ascii_line(const unsigned char *payload, int slen, int offset)
{

   int i;
   int gap, len;
   const unsigned char *ch, *ch2;

   len = 16;
   ch2 = ch = payload;

  do {
      
      /* offset */
      //printf("%05x   ", offset);
      print_hex_word(offset & 0xffff);
      putchar(' ');

      /* hex */
      if(slen < len)
         len=slen;

      for(i = 0; i < len; i++) {
         
         //printf("%02x ", *ch);

         print_hex_byte(*ch);
         putchar(' ');

         ch++;
         /* print extra space after 8th byte for visual aid */
         if (i == 7)
            putchar(' ');
      }
      /* print space to handle line less than 8 bytes */
      if (len < 8)
         putchar(' ');
      
      /* fill hex gap with spaces if not full line */
      if (len < 16) {
         gap = 16 - len;
         for (i = 0; i < gap; i++) {
            printf("   ");
         }
      }          
      printf("   ");
      
      /* ascii (if printable) */
      for(i = 0; i < len; i++) {
         if (isprint(*ch2)){
            printf("%c", *ch2);
         } else
            putchar(' ');
         ch2++;
      }

      puts("");
      slen -= 16;
      offset += len;
   } while( slen > 0 ) ;

  
   return;
}



//-----------------------------------------------------------------------------
// delay
//-----------------------------------------------------------------------------

void _Delay (volatile unsigned long a) { while (--a!=0); }
   


//-----------------------------------------------------------------------------
// the most important function is implemented right here:
//-----------------------------------------------------------------------------
static void banner(void)
{
   int cols = 70;
   int rows = 25;
   int i = 0;
   
   putchar('\n');
   
   for (i=0;i<rows;i++)
      putchar('\n');
   
   for (i=0;i<cols;i++)
      putchar('*');
      putchar('\n');

   printf("* Booting  %s %s\n", 
      PROJECT_NAME, PROJECT_REVISION);
   printf("* Built: %s %s\n", 
      build_date, build_time);

   printf("* datenkrake (at) dev . io -- ://datenkrake.org/\n");
   printf("* Oh hai!\n");
         
   for (i=0;i<cols;i++)
      putchar('*');
      putchar('\n');

#ifdef _ROWLEY_CTL
   printf("* Information:\n");
   printf("*   Internal Flash Memory Size (Byte):        %d\n", liblpc1000_get_internal_flash_size() );
   printf("*   Internal Local SRAM Memory Size (Byte):   %d\n", liblpc1000_get_local_sram_size() );
   printf("*   AHB SRAM Memory Size (Byte):              %d\n", liblpc1000_get_ahb_sram_size() );
   printf("*   Oscillator Clock Frequency (Hz):          %d\n", (OSCILLATOR_CLOCK_FREQUENCY) );
   printf("*   Current CPU Clock Frequency (Hz):         %d\n", liblpc1000_lpc17xx_get_cclk(OSCILLATOR_CLOCK_FREQUENCY) );
   printf("*   Current Timer Ticks Per Second:           %d\n", ctl_get_ticks_per_second());
#endif
   
   for (i=0;i<cols;i++)
      putchar('*');
      putchar('\n');
}


char *strtrim (char *s)
{
  char *t = s + strlen (s) - 1;

  while (t >= s && *t && isspace (*t))
    *t-- = '\0';

  while (*s && isspace (*s))
    s++;

  return s;
}



char * index(char *str, char c)
{
   while(*str != c) 
      if(*str == 0)
         return str;
      else
         str++;

   return str;
}
