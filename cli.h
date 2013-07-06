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
 * File: cli.h
 * Description: Header & definitions of Command Line Interface.
 *
 * *********************************************************************** * * * * * * * * * * *
 * Parts of the argument dispatching routines were taken from the LPC2148 demo code v1.44
 *   available at http://jcwren.com/arm/. Credits and thanks to jcwren _-at-_ jcwren.com -
 *   i didn't even know her/his real name. -- Thorsten, Berlin, Sept. 8th 2010
 * *********************************************************************** * * * * * * * * * * *
 */

#ifndef _CLI_H
#define _CLI_H

#define PROMPT "# "

void vCLITask( void *);

typedef enum
{
  CMDTYPE_CMDLIST = 0,
  CMDTYPE_FUNCTION
} cmdType_e;

typedef struct commandList_s
{
  const portCHAR *command;
  portCHAR minArgs;
  portCHAR maxArgs;
  cmdType_e cmdType;
  union
  {
    const void *trickGCC;
    int (*handler) (int argc, portCHAR **argv);
    struct commandList_s *commandList;
  };
  const portCHAR *description;
  const portCHAR *parameters;
}
commandList_t;

int argsGetLine (int fd, unsigned char *buffer, int bufferLength, const char *prompt);
int argsParse (char *cmd, char **argv, int sizeofArgv, int *argc);

int argsDispatch (const commandList_t *cl, int argc, char **argv);


#endif