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
 * File: helper.h
 * Description: Declaration & Documentation of helper functions.
 */

#ifndef _HELPER_H
#define _HELPER_H

#include <stdint.h>

/*!
 * print buffer as hex-dump. The input buffer is printed as hex-dump, with 
 * an offset column first. The second column is the hexadecimal dump of the binary data.
 * Third column is the ascii representation of the input data.
 * Example:
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 *
 * @param[in] s source buffer
 * @param[in] l source buffer length
 * @param[in] o offset
*/
void print_hex_ascii_line(const unsigned char *s, int l, int o);

/*!
 * print byte as hex. Prints input byte b as hex string.
 * @param[in]  b    byte
*/
void print_hex_byte(char b);

/*!
 * print 16bit word as hex. Prints input word w as hex string.
 * @param[in]  w word
*/
void print_hex_word(int16_t w);

/*!
 * print 32bit dword as hex. Prints input dword d as hex string.
 * @param[in]  d    dword
*/
void print_hex_dword(int32_t d);

/*!
 * print buffer as hex. Prints input byte buffer s as hex string.
 * @param[in] s source buffer
 * @param[in] l source buffer length
*/
void print_hex_buf(char *s, uint32_t l);



char *strtrim (char *);
char * index(char *, char);



// ************************************************

void Delay (volatile unsigned long a);

#define _DELAY_MS(s) _DELAY_US( ((s)*1000) )
#define _DELAY_US(s)   do {volatile int i=0;for(i=0;i<((s)*10)-2;i++);}while(0);   // 100 cyc => 1usec, needs 10 instructions
#define _DELAY_500N()  do {volatile int i=0;for(i=0;i<(5)-2;i++);}while(0);      // 1 cyc => 10nsec

#endif

