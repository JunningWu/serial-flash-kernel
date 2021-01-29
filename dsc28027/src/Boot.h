/*
 * Copyright (c) 2020 Beijing Haawking Technology Co.,Ltd
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Junning Wu
 * Email: junning.wu@mail.haawking.com
 * update: 2020-08-13: 
 *                    IQMath Src V0.0.3
 *                    Drivers：Rev105
 *                    FlashAPI：V0.04
 */


#ifndef DSC28027_BOOT_H
#define DSC28027_BOOT_H

#include "util.h"
#include <assert.h>
#include "mem_map.h"
#include "DSC28027_Device.h"

#define CODE_SECTION(v) __attribute__((section(v)))

//
// Define for the length of the programming buffer
//
#define PROG_BUFFER_LENGTH 0x100


//---------------------------------------------------------------------------
// Boot Modes
//
//           GPIO37    GPIO34   TRSTn
//            TDO      CMP2OUT
// Mode EMU    x         x        1      Emulator connected
// Mode 0      0         0        0      Flash Boot
// Mode 1      0         1        0      SCI
// Mode 2      1         0        0      wait
// Mode 3      1         1        0      "Get Mode"

//Store in the Highest for Bytes of OTP
#define OTP_KEY             0x7A3BF8
#define OTP_BMODE           0x7A3BFA
#define KEY_VAL             0x55AA

#define PARALLEL_BOOT       0x0000
#define SCI_BOOT            0x0001
#define WAIT_BOOT           0x0002
#define GET_BOOT            0x0003

#define SPI_BOOT            0x0004
#define I2C_BOOT            0x0005
#define OTP_BOOT            0x0006
#define RAM_BOOT            0x000A
#define FLASH_BOOT          0x000B

//---------------------------------------------------------------------------
// Fixed boot entry points:
//
#define FLASH_ENTRY_POINT 0x7DFFE8
#define OTP_ENTRY_POINT   0x7A0000
#define RAM_ENTRY_POINT   0x012000

#define DIVSEL_BY_4             0
#define DIVSEL_BY_2             2
#define DIVSEL_BY_1             3

#define ERROR                   1
#define NO_ERROR                0
#define EIGHT_BIT               8
#define SIXTEEN_BIT            16
#define EIGHT_BIT_HEADER   0x08AA
#define SIXTEEN_BIT_HEADER 0x10AA

//---------------------------------------------------------------------------
//
typedef Uint16 (* uint16fptr)();
extern  uint16fptr GetWordData;
extern  uint16fptr GetOnlyWordData;
extern  void (*Flash_CallbackPtr) (void);
extern  Uint32 Flash_CPUScaleFactor;
//#define Device_cal (void   (*)(void))0x3D7C80
//#define Get_mode   (Uint16 (*)(void))0x3D7CC0

//---------------------------------------------------------------------------
// Configured by the boot ROM
//
//#define BORTRIM (Uint16 *)0x0986

#endif  // end of DSC28027_BOOT_H definition
