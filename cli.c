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
 * File: cli.c
 * Description: Implementation of Command Line Interface.
 *
 * *********************************************************************** * * * * * * * * * * *
 * Parts of the argument dispatching routines were taken from the LPC2148 demo code v1.44
 *   available at http://jcwren.com/arm/. Credits and thanks to jcwren _-at-_ jcwren.com -
 *   i didn't even know her/his real name. -- Thorsten, Berlin, Sept. 8th 2010
 * *********************************************************************** * * * * * * * * * * *
 */

#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "main.h"
#include "cli.h"
#include "helper.h"
#include "dpuser.h"
#include "io.h"
#include "buffer.h"
#include "led.h"
#include "tests.h"
#include "uart.h"

void vPortUsedMem(int *, int *, int *);

commandList_t *activeCommandList;

extern size_t read(int, unsigned char *, size_t );

static int __attribute__ ((unused)) prv_cli_led_set (int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_led_clr (int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_fpga(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_fpga_halt(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_fpga_reset(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_fpga_off(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_fpga_on(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_buf_en(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_buf_dis(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_buf_all_en(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_buf_all_dis(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_adv_erase(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_adv_test(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_adv_prog(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_led_ctl(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_led_ctl_all(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_data_write(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_data_read(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_buf_on(int , portCHAR **);
static int __attribute__ ((unused)) prv_cli_buf_off(int , portCHAR **);

static int prv_cli_help(int , portCHAR **);
static int prv_cli_version(int , portCHAR **);
static int prv_cli_reboot(int , portCHAR **);
static int prv_cli_credits(int , portCHAR **);

static int prv_mem_task(int , portCHAR **);
static int prv_mem_free(int , portCHAR **);

/* CLI sub-command example... */
static const commandList_t commandListMem [] =
{
  { "help",     0,  0, CMDTYPE_FUNCTION,  { prv_cli_help        }, "This help list",               "'help' has no parameters" },
  { "task",     0,  0, CMDTYPE_FUNCTION,  { prv_mem_task        }, "Show FreeRTOS tasks & stacks", "'task' has no parameters" },
  { "free",     0,  0, CMDTYPE_FUNCTION,  { prv_mem_free        }, "Show heap memory statistics",  "'heap' has no parameters" },
  { NULL,       0,  0, CMDTYPE_FUNCTION,  { NULL                }, NULL,                           NULL },
};

static const commandList_t commandListLED [] =
{
  { "help",     0,  0, CMDTYPE_FUNCTION,  { prv_cli_help        }, "This help list",               "'help' has no parameters" },
  { "set",      0,  1, CMDTYPE_FUNCTION,  { prv_cli_led_set     }, "set [led] - if no led is specified, all LEDs are set.", "'set' has 1 parameters" },
  { "clear",    0,  1, CMDTYPE_FUNCTION,  { prv_cli_led_clr     }, "clear [led] - if no led is specified, all LEDs are cleared.",  "'clear' has 1 parameters" },
  { NULL,       0,  0, CMDTYPE_FUNCTION,  { NULL                }, NULL,                           NULL },
};

static const commandList_t commandListFPGA [] =
{
  { "help",     0,  0, CMDTYPE_FUNCTION,  { prv_cli_help        }, "This help list",               "'help' has no parameters" },
  { "reset",    0,  0, CMDTYPE_FUNCTION,  { prv_cli_fpga_reset  }, "reset - reset FPGA by pulling reset high and low", "'reset' has no parameters" },
  { "halt",     0,  0, CMDTYPE_FUNCTION,  { prv_cli_fpga_halt   }, "halt - pull reset line low",  "'halt' has no parameters" },
  { "on",       0,  0, CMDTYPE_FUNCTION,  { prv_cli_fpga_on     }, "on - power on FPGA",  "'on' has no parameters" },
  { "off",      0,  0, CMDTYPE_FUNCTION,  { prv_cli_fpga_off    }, "off - power off FPGA",  "'off' has no parameters" },
  { NULL,       0,  0, CMDTYPE_FUNCTION,  { NULL                }, NULL,                           NULL },
};

static const commandList_t commandListBUF [] =
{
  { "help",        0,  0, CMDTYPE_FUNCTION,  { prv_cli_help     }, "This help list",               "'help' has no parameters" },
  { "enable",      0,  1, CMDTYPE_FUNCTION,  { prv_cli_buf_on   }, "enable [buf] - if no buffer is specified, all buffers are enabled.", "'enable' has 1 parameters" },
  { "disable",     0,  1, CMDTYPE_FUNCTION,  { prv_cli_buf_off  }, "disable [buf] - if no buffer is specified, all buffers are disabled.",  "'disable' has 1 parameters" },
  { NULL,          0,  0, CMDTYPE_FUNCTION,  { NULL             }, NULL,                           NULL },
};

static const commandList_t commandListADV [] =
{
  { "help",        0,  0, CMDTYPE_FUNCTION,  { prv_cli_help     }, "This help list",               "'help' has no parameters" },
  { "test",        0,  0, CMDTYPE_FUNCTION,  { prv_cli_adv_test }, "manually perform HW checks.", "'test' has no parameters" },
  { "prog",        0,  0, CMDTYPE_FUNCTION,  { prv_cli_adv_prog }, "Force reprogramming of the FPGA flash ROM. (!!! RTFM !!!)", "'prog' has no parameters" },
  { "erase",       0,  0, CMDTYPE_FUNCTION,  { prv_cli_adv_erase}, "Force reasure of the FPGA flash ROM. (!!! RTFM !!!)", "'erase' has no parameters" },
  { NULL,          0,  0, CMDTYPE_FUNCTION,  { NULL             }, NULL,                           NULL },
};

static const commandList_t commandList [] =
{
    { "mem",     0,  1, CMDTYPE_CMDLIST,   { commandListMem     }, "Various memory functions",          "'mem help' for help list" },
    { "led",     1,  2, CMDTYPE_CMDLIST,   { commandListLED     }, "LED control commands",              "'led help' for help list" },
    { "fpga",    0,  0, CMDTYPE_CMDLIST,   { commandListFPGA    }, "FPGA control commands",             "'fpga help' for help list" },
    { "buffer",  0,  1, CMDTYPE_CMDLIST,   { commandListBUF     }, "Buffer/channel control commands",   "'buffer help' for help list" },
    { "adv",     0,  0, CMDTYPE_CMDLIST,   { commandListADV     }, "Advanced commands",                 "'adv help' for help list" },

    { "help",    0,  0, CMDTYPE_FUNCTION,  { prv_cli_help       }, "This help list",                    "'help' has no parameters" },
    { "credits", 0,  0, CMDTYPE_FUNCTION,  { prv_cli_credits    }, "Display credits, greets & shoutz",  "'version' has no parameters" },
    { "reboot",  0,  0, CMDTYPE_FUNCTION,  { prv_cli_reboot     }, "Reset Datenkrake",                  "'reboot' has no parameters" },
// aliases
    { "R",      0,  0, CMDTYPE_FUNCTION,  { prv_cli_fpga_reset  }, "FPGA reset",                        "'R' reset FPGA" },
    { "H",      0,  0, CMDTYPE_FUNCTION,  { prv_cli_fpga_halt   }, "FPGA halt (reset high)",            "'H' halt FPGA - no params" },
    { "o",      0,  0, CMDTYPE_FUNCTION,  { prv_cli_fpga_off    }, "Power off FPGA",                    "'o' Power off FPGA - no params" },
    { "O",      0,  0, CMDTYPE_FUNCTION,  { prv_cli_fpga_on     }, "Power on FPGA",                     "'O' Power on FPGA - no params" },
    { "e",      1,  1, CMDTYPE_FUNCTION,  { prv_cli_buf_en      }, "Buffer enable",                     "'e <num>' enable buffer <num>" },
    { "d",      1,  1, CMDTYPE_FUNCTION,  { prv_cli_buf_dis     }, "Buffer disable",                    "'d <num>' disable buffer <num>" },
    { "E",      0,  0, CMDTYPE_FUNCTION,  { prv_cli_buf_all_en  }, "Buffer enable all",                 "'E' enable all - no params" },
    { "D",      0,  0, CMDTYPE_FUNCTION,  { prv_cli_buf_all_dis }, "Buffer disable all",                "'D' disable all - no params" },
    { "0",      0,  0, CMDTYPE_FUNCTION,  { prv_cli_adv_erase   }, "Advanced - FPGA erase flash ROM",   "'0' Erase FPGA - no params" },
    { "t",      0,  0, CMDTYPE_FUNCTION,  { prv_cli_adv_test    }, "Advanced - HW test",                "'t' HW tests - no params" },
    { "p",      0,  0, CMDTYPE_FUNCTION,  { prv_cli_adv_prog    }, "Advanced - Program FPGA flash ROM", "'p' program FPGA - no params" },
    { "l",      2,  2, CMDTYPE_FUNCTION,  { prv_cli_led_ctl     }, "LED control single",                "'l <num> <status>' control LEDs - status = 0 or 1" },
    { "L",      1,  1, CMDTYPE_FUNCTION,  { prv_cli_led_ctl_all }, "LED control all",                   "'L <status>' - control all LEDs - status = 0 or 1" },
    { "w",      2,  2, CMDTYPE_FUNCTION,  { prv_cli_data_write  }, "Data write",                        "'w <reg> <data>' write 8-bit data <data> to register <reg>" },
    { "r",      2,  1, CMDTYPE_FUNCTION,  { prv_cli_data_read   }, "Data read",                         "'r' read data - not implemented" },
    { NULL,     0,  0, CMDTYPE_FUNCTION,  { NULL                }, NULL,                                NULL },
};

// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_buf_on (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    unsigned int buf = 0;

    if(argc < 1) {
        buf1_enable();
        buf2_enable();
        buf3_enable();
        buf4_enable();
        buf5_enable();
        buf6_enable();
        buf7_enable();
        buf8_enable();
        puts("All buffers enabled.");
    } else {
        buf = strtol(argv[0], NULL, 10) & 0xff;
        switch(buf) {
            case 1:
                buf1_enable(); break;
            case 2:
                buf2_enable(); break;
            case 3:
                buf3_enable(); break;
            case 4:
                buf4_enable(); break;
            case 5:
                buf5_enable(); break;
            case 6:
                buf6_enable(); break;
            case 7:
                buf7_enable(); break;
            case 8:
                buf8_enable(); break;
            default:
                break;
        }
        printf("Buffer %d enabled.\n", buf);
    }

    return 0;
}

// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_buf_off (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    unsigned int buf = 0;

    if(argc < 1) {
        buf1_disable();
        buf2_disable();
        buf3_disable();
        buf4_disable();
        buf5_disable();
        buf6_disable();
        buf7_disable();
        buf8_disable();
        puts("All buffers disabled.");
    } else {
        buf = strtol(argv[0], NULL, 10) & 0xff;
        switch(buf) {
            case 1:
                buf1_disable(); break;
            case 2:
                buf2_disable(); break;
            case 3:
                buf3_disable(); break;
            case 4:
                buf4_disable(); break;
            case 5:
                buf5_disable(); break;
            case 6:
                buf6_disable(); break;
            case 7:
                buf7_disable(); break;
            case 8:
                buf8_disable(); break;
            default:
                break;
        }
        printf("Buffer %d disabled.\n", buf);
    }

    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_led_set (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    unsigned int led = 0;

    if(argc < 1) {
        led1_set();
        led2_set();
        led3_set();
        led4_set();
    } else {
        led = strtol(argv[0], NULL, 10) & 0xff;
        switch(led) {
            case 1:
                led1_set();
                break;
            case 2:
                led2_set();
                break;
            case 3:
                led3_set();
                break;
            case 4:
                led4_set();
                break;
            default:
                break;
        }
    }

    return 0;
}

// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_led_clr (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    unsigned int led = 0;

    if(argc < 1) {
        led1_clr();
        led2_clr();
        led3_clr();
        led4_clr();
    } else {
        led = strtol(argv[0], NULL, 10) & 0xff;
        switch(led) {
            case 1:
                led1_clr();
                break;
            case 2:
                led2_clr();
                break;
            case 3:
                led3_clr();
                break;
            case 4:
                led4_clr();
                break;
            default:
                break;
        }
    }

    return 0;
}



// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_fpga (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
   printf("ERR: Implement me.\n");
   return 0;
}




// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_fpga_reset (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    fpga_rst_high();
    vTaskDelay(100 / portTICK_RATE_MS);
    fpga_rst_low();
    puts("FPGA reset done.");

    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_fpga_halt (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    fpga_rst_high();

    puts("FPGA halted.");
    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_fpga_off (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
   io_fpga_disable();
   puts("FPGA powered off");
   return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_fpga_on (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
   io_fpga_enable();
   puts("FPGA powered on");
   return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_buf_en (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{

    unsigned int buf = strtol(argv[0], NULL, 10) & 0xff;

    switch(buf) {
        case 1:
            buf1_enable(); break;
        case 2:
            buf2_enable(); break;
        case 3:
            buf3_enable(); break;
        case 4:
            buf4_enable(); break;
        case 5:
            buf5_enable(); break;
        case 6:
            buf6_enable(); break;
        case 7:
            buf7_enable(); break;
        case 8:
            buf8_enable(); break;
        default:
            break;
    }
    printf("Buffer %d enabled.\n", buf);
    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_buf_dis (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{

    unsigned int buf = strtol(argv[0], NULL, 10) & 0xff;

    switch(buf) {
        case 1:
            buf1_disable(); break;
        case 2:
            buf2_disable(); break;
        case 3:
            buf3_disable(); break;
        case 4:
            buf4_disable(); break;
        case 5:
            buf5_disable(); break;
        case 6:
            buf6_disable(); break;
        case 7:
            buf7_disable(); break;
        case 8:
            buf8_disable(); break;
        default:
            break;
    }
    printf("Buffer %d disabled.\n", buf);
    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_buf_all_en (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    buf1_enable();
    buf2_enable();
    buf3_enable();
    buf4_enable();
    buf5_enable();
    buf6_enable();
    buf7_enable();
    buf8_enable();
    puts("All buffers enabled.");
    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_buf_all_dis (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    buf1_disable();
    buf2_disable();
    buf3_disable();
    buf4_disable();
    buf5_disable();
    buf6_disable();
    buf7_disable();
    buf8_disable();
    puts("All buffers disabled.");
    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_adv_erase (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    puts("Erasing FPGA flash ROM.");
    dp_erase_main();
    puts("FPGA flash ROM erased.");
    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_adv_test (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    ddk_manual_tests();
    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_adv_prog (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    dp_force_prog_main();
    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_led_ctl (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    unsigned int led = strtol(argv[0], NULL, 10) & 0xff;
    unsigned int val = strtol(argv[1], NULL, 10) & 0xff;

    printf("Setting LED %d to %d\n", led, val);

    switch(led) {
        case 1:
            if(val)
                led1_set();
            else
                led1_clr();
            break;
        case 2:
            if(val)
                led2_set();
            else
                led2_clr();
            break;
        case 3:
            if(val)
                led3_set();
            else
                led3_clr();
            break;
        case 4:
            if(val)
                led4_set();
            else
                led4_clr();
            break;
        default:
            break;
    }

    return 0;
}

// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_led_ctl_all (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    unsigned int val = strtol(argv[0], NULL, 10) & 0xff;

    if(val) {
        led1_set();
        led2_set();
        led3_set();
        led4_set();
    } else {
        led1_clr();
        led2_clr();
        led3_clr();
        led4_clr();
    }

    printf("All LEDs switched %s.\n", (val ? "on" : "off") );
    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_data_write (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
    unsigned char reg = 0;
    unsigned char val = 0;

    reg = strtol(argv[0], NULL, 16) & 0xff;
    val = strtol(argv[1], NULL, 16) & 0xff;

    io_fpga_register_write(reg, val);
    return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_cli_data_read (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
   printf("ERR: Implement me.\n");
   return 0;
}



/* *** MEMORY Command Handler *** */
static int prv_mem_task(int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
#if configUSE_TRACE_FACILITY == 1
  signed portCHAR buffer[TASKHANDLE_LAST * 42];
  int bytesFree;
  int bytesUsed;
  int blocksFree;

  vTaskList (buffer);
  vPortUsedMem (&bytesFree, &bytesUsed, &blocksFree);

  //
  //  Stack is the minimum amount of stack there has been available at any time since the task started executing.
  //
  printf ("Task\t\tState\tPrty\tStack\tTask#\n");
  printf ("%s\n", buffer);
#else
  printf ("Not implemented (requires configUSE_TRACE_FACILITY in FreeRTOSConfig.h)\n");
#endif

  return 0;
}

static int prv_mem_free(int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
#if configUSE_TRACE_FACILITY == 1
  int bytesFree;
  int bytesUsed;
  int blocksFree;

  vPortUsedMem (&bytesFree, &bytesUsed, &blocksFree);

  //
  //  Stack is the minimum amount of stack there has been available at any time since the task started executing.
  //
  printf ("Heap size=%ld, used=%d, free=%d (%d blocks)\n", configTOTAL_HEAP_SIZE, bytesUsed, bytesFree, blocksFree);
#else
  printf ("Not implemented (requires configUSE_TRACE_FACILITY in FreeRTOSConfig.h)\n");
#endif

  return 0;
}


// ******************************************************** ** ** **  **    *       *         *               *
static int __attribute__ ((unused)) prv_dummy_lala (int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
   printf("ERR: Implement me.\n");
   return 0;
}
// ******************************************************** ** ** **  **    *       *         *               *


/* *** CLI Command Handler *** */
static int prv_cli_version(int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
   printf("Die Datenkrake Version 1.0\n");
   return 0;
}

static int prv_cli_credits(int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
   printf("******************** *** **  **    **    *       *         *               *\n");
   printf("* Die Datenkrake was developed by Dmitry Nedospasov & Thorsten Schroeder   *\n");
   printf("* web: http://datenkrake.org -- twitter: @DieDatenkrake                    *\n");
   printf("* mail: datenkrake at dev dot io -- irc: #DDK on freenode                  *\n");
   printf("******************** *** **  **    **    *       *         *               *\n");
   printf("* 3rd party code or ideas that were used in this project:                  *\n");
   printf("* > FreeRTOS 7, Real Time Kernel by Richard Barry                          *\n");
   printf("*          Copyright (C) 2012 Real Time Engineers Ltd.                     *\n");
   printf("*          -> http://www.freertos.org/                                     *\n");
   printf("* > CLI command handling basics were developed & published by              *\n");
   printf("*          J.C.Wren <jcwren-at-jcwren.com> in his LPC2148 demo code v1.44  *\n");
   printf("*          -> http://jcwren.com/arm/                                       *\n");
   printf("**** THANK YOU! **** *** **  **    **    *       *         *               *\n");
   printf("* Special thanks & greetings to:                                           *\n");
   printf("*    Hugo Fortier & REcon crew                                             *\n");
   printf("*    Colleagues at SECT & modzero AG                                       *\n");
   printf("*    Daniel Mack, Joachim Steiger, Felix von Leitner                       *\n");
   printf("*    Microsemi Corporation - http://www.microsemi.com/                     *\n");
   printf("******************** *** **  **    **    *       *         *               *\n");
   return 0;
}

static int prv_cli_reboot(int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
   printf("Rebooting now...\n");
   uart0_shutdown();
   NVIC_SystemReset();
   return 0;
}


static int prv_cli_help(int argc __attribute__ ((unused)), portCHAR **argv __attribute__ ((unused)))
{
  unsigned int i;
  int t;
  int longestCmd;
  portCHAR spaces [32];

  memset (spaces, ' ', sizeof (spaces));

  for (longestCmd = 0, i = 0; activeCommandList [i].command; i++)
    if ((t = strlen (activeCommandList [i].command)) > longestCmd)
      longestCmd = t;

  spaces [longestCmd] = '\0';

  for (i = 0; activeCommandList [i].command; i++)
  {
    const commandList_t *cl = &activeCommandList [i];

    printf ("%s%s -- %s\n", cl->command, &spaces [strlen (cl->command)], cl->description);
  }

  printf ("\nUse '<command> ?' for details on parameters to command\n");

  return 0;
}









//
//  bufferLength includes the space reserved for the \0
//
int argsGetLine (int fd __attribute__ ((unused)), unsigned char *buffer, int bufferLength, const char *prompt)
{
  unsigned char *p;

  p = buffer;
  *p = '\0';

  printf ("%s", prompt);

  while (1)
  {
    unsigned char c;

    c = getchar();
    //if(read(0, &c, sizeof(c)) ==sizeof(c))

    if (c != EOF)
    {
      switch (c)
      {
        case '\n' :
        case '\r' :
          printf ("\n");
          return strlen ((char *) buffer);

        // treat DEL as BS
        case 0x7f:
          if (p > buffer)
            *--p = '\0';
          printf ("\x08 \x08");
          break;

        // BS
        case 0x08:
          if (p > buffer)
            *--p = '\0';
          printf ("\x08 \x08");
          break;

        case 0x15 : // CTRL-U
          while (p != buffer)
          {
            printf ("\b \b");
            --p;
          }
          *p = '\0';
          break;

        case 0xfe :
        case 0xff :
          *buffer++ = c;
          *buffer = '\0';
          return 1;

        default :
          if (p < buffer + bufferLength - 1 && c >= ' ' && c < 0x7f)
          {
            *p++ = c;
            *p = '\0';
            printf ("%c", c);
          }
          else
            printf ("%c", c);

          break;
      }
    }
  }

  return 0;
}

//
//
//
typedef enum
{
  P_EATWHITESPACE = 0,
  P_GETCHARFIRST,
  P_GETCHAR,
  P_QUOTEDGETCHAR
}
PSTATE;

int argsParse (char *cmd, char **argv, int sizeofArgv, int *argc)
{
  int maxArgs = (sizeofArgv / sizeof (argv [0])) - 1;
  char *s = strtrim (cmd);
  PSTATE pstate = P_EATWHITESPACE;

  *argc = 0;
  memset (argv, 0, sizeofArgv);

  while (*s)
  {
    switch (pstate)
    {
      case P_EATWHITESPACE :
        {
          if (!isspace (*s))
            pstate = P_GETCHARFIRST;
          else
            s++;
        }
        break;

      case P_GETCHARFIRST :
        {
          *argv++ = s;

          if (++*argc == maxArgs)
            return 1;
          if (*s == '"')
            pstate = P_QUOTEDGETCHAR;
          else
            pstate = P_GETCHAR;

          s++;
        }
        break;

      case P_GETCHAR :
        {
          if (isspace (*s)) {
            pstate = P_EATWHITESPACE;
            *s = '\0';
          }
          else if (*s == '"')
            pstate = P_QUOTEDGETCHAR;

          s++;
        }
        break;

      case P_QUOTEDGETCHAR :
        {
          if (*s == '"')
            pstate = P_GETCHAR;

          s++;
        }
        break;
    }
  }

  return 0;
}



int argsDispatch (const commandList_t *cl, int argc, char **argv)
{
   int ret = 0;
   activeCommandList = (commandList_t *)cl;

   while (cl->command) {
      if (!strcmp (cl->command, argv [0])) {
         ret = 0;

      if ((argc == 2) && !strcmp (argv [1], "?"))
        printf ("%s\n", cl->parameters);
      else if ((argc - 1) < cl->minArgs)
        printf ("Too few arguments to command (%d expected)\n", cl->minArgs);
      else if (cl->cmdType == CMDTYPE_CMDLIST)
        argsDispatch (cl->commandList, argc - 1, &argv [1]);
      else if ((argc - 1) > cl->maxArgs)
        printf ("Too many arguments to command (%d maximum)\n", cl->maxArgs);
      else
        ret = (*cl->handler) (argc - 1, &argv [1]);

      return ret;
    }

    cl++;
  }

  if (!cl->command)
    printf ("Unknown command \"%s\"\n", argv [0]);

  return 0;
}

//-----------------------------------------------------------------------------
// Main task for CLI
//-----------------------------------------------------------------------------
void vCLITask( void *pvParameters )
{
   const portTickType xDelay = 100 / portTICK_RATE_MS;

   static unsigned char buffer[256];
   static portCHAR *argv[34];
   int argc;

   /* Just to prevent compiler warnings about the unused parameter. */
   ( void ) pvParameters;

   printf("[*] Starting CLI task.\n");


   for (;;) {
      int l;

      if ((l = argsGetLine (0, buffer, sizeof (buffer), PROMPT))) {

         if (argsParse ((char *) buffer, argv, sizeof (argv), &argc))
            printf ("Too many arguments (max %ld)\n", sizeof (argv));
         else if (argv[0])
            argsDispatch (commandList, argc, &argv [0]);
      }
      vTaskDelay( xDelay );

   }


   for(;;) {

      vTaskDelay( xDelay );

   }
}


