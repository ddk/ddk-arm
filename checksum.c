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
 * <andy at ndyk dot de> wrote this file. As long as you retain this notice 
 * you can do whatever you want with this stuff.
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
 * File: checksum.c
 * Description: LPC 17xx checksum insertion program
 */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    int32_t fd, i, *addr;

    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if((fd = open(argv[1], O_RDWR)) == -1 ||
       (addr = mmap(NULL, 8 * sizeof(uint32_t), PROT_READ | 
                    PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        fprintf(stderr, "Error opening '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    addr[7] = 0;
    for(i = 0; i < 7; i++)
        addr[7] += addr[i];

    addr[7] = -addr[7];

    return EXIT_SUCCESS;
}
