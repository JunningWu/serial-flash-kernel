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
 * Email  : junning.wu@mail.haawking.com
 * FILE   : main.c
 *****************************************************************/
 
#include <stdio.h>
#include "util.h"
#include <assert.h>
#include "mem_map.h"
#include "IQmathLib.h"
#include "Flash28034_API_Library.h"

#ifdef DSC28027
  #include "DSC28027_Device.h"
  #include "DSC28027_Examples.h"
#endif
  
#ifdef DSC28034
  #include "DSC28034_Device.h"
  #include "DSC28034_Examples.h"
#endif

//
// Globals
//
extern Uint32 SCI_Boot();
extern void InitFlash_Kernel(void);

void  WatchDogDisable()
{
   EALLOW;
   P_SysCtrlRegs->WDCR.all = 0x0068;               // Disable watchdog module
   EDIS;
}

void OSC_Cal(){
	EALLOW;
	P_SysCtrlRegs->PLLSTS.bit.DIVSEL = 3;
	P_SysCtrlRegs->INTOSC1TRIM.bit.FINETRIM = 0;
	EDIS;

}

void GPIO_INIT(){
  EALLOW;
  GpioCtrlRegs.GPBDIR.bit.GPIO41 = 1;
  EDIS;
}
int main(void)
{
	int temp, in;
	volatile unsigned int i;

	Uint16 VersionHex;
	Uint32 EntryAddr;
	WatchDogDisable();
/*	GPIO_INIT();

	for(in=0;in<10;in++){
	    GpioDataRegs.GPBDAT.bit.GPIO41 = 0;
	    for(i=0;i<12000;i++);
	    GpioDataRegs.GPBDAT.bit.GPIO41 = 1;
	    for(i=0;i<12000;i++);
	}*/

	EALLOW;
	P_SysCtrlRegs->PCLKCR0.bit.SCIAENCLK=0;
	EDIS;
	asm volatile (".align 2;RPTI 20,4;NOP");
	//OSC_Cal();

	InitFlash_Kernel();

	EntryAddr = SCI_Boot();
    asm volatile ("li ra,0x73FFE8");
    asm volatile ("ret");

	return 0;
}

// ----------------------------------------------------------------------------

