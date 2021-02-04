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
 * Update: change to FlashAPI V0.04, 20200728, wujn
 */
#include "util.h"
#include <assert.h>
#include "mem_map.h"
#include "DSC28027_Device.h"
#include "DSC28027_Examples.h"
#include "Flash28027_API_Library.h"

void  WatchDogDisable()
{
   EALLOW;
   SysCtrlRegs.WDCR.all = 0x0068;               // Disable watchdog module
   EDIS;
}

//---------------------------------------------------------------
// This module enables the watchdog timer.
//---------------------------------------------------------------

void  WatchDogEnable()
{
   EALLOW;
   SysCtrlRegs.WDCR.all = 0x0028;               // Enable watchdog module
   SysCtrlRegs.WDKEY = 0x55;                // Clear the WD counter
   SysCtrlRegs.WDKEY = 0xAA;
   EDIS;
}
void GPIO_INIT(){
  EALLOW;
  GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;
  EDIS;
}


//
// Globals
//
extern Uint32 SCI_Boot();
extern void InitFlash_Kernel(void);

int main( int argc, char* argv[] )
{
  Uint16 VersionHex;
  volatile unsigned int i,j;
  Uint32 EntryAddr;
  WatchDogDisable();
  //while(1);
//  GPIO_INIT();
//
//  for(i=0;i<10;i++){
//	  GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
//	  for(j=0;j<12000;j++);
//	  GpioDataRegs.GPASET.bit.GPIO7 = 1;
//	  for(j=0;j<12000;j++);
//  }

  EALLOW;
  P_SysCtrlRegs->PCLKCR0.bit.SCIAENCLK=0;
  EDIS;
  asm volatile (".align 2;RPTI 20,4;NOP");

  InitFlash_Kernel();

  EntryAddr = SCI_Boot();
  asm volatile ("li ra,0x7DFFE8");
  asm volatile ("ret");
  
  return 0;
}

