//###########################################################################
//
// FILE:    SCI_Boot.c
//
// TITLE:   SCI Boot mode routines
//
// Functions:
//
//     Uint32 SCI_Boot(void)
//     inline void SCIA_Init(void)
//     inline void SCIA_AutobaudLock(void)
//     Uint32 SCIA_GetWordData(void)
//
// Notes:
//
//###########################################################################
// $TI Release: F2802x Support Library v3.02.00.00 $
// $Release Date: Fri May 24 03:35:50 CDT 2019 $
// $Copyright:
// Copyright (C) 2009-2019 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

//
// Included Files
//
#include "Boot.h"

//
// Function Prototypes
//
void SCIA_Init(void);
void SCIA_AutobaudLock(void);
Uint16 SCIA_GetWordData(void);
Uint16 SCIA_GetOnlyWordData(void);

//
// External functions
//
extern void CopyData(void);
Uint32 GetLongData(void);
extern void ReadReservedFn(void);
extern void ReadCSMKeys(void);
extern unsigned int checksum;
extern Uint32 CSMKEY0, CSMKEY1, CSMKEY2, CSMKEY3;
//
// SCI_Boot - This module is the main SCI boot routine. It will load code
// via the SCI-A port. It will return a entry point address back to the 
// InitBoot routine which in turn calls the ExitBoot routine.
//
Uint32
SCI_Boot()
{
    Uint32 EntryAddr;

    //
    // Assign GetWordData to the SCI-A version of the function. GetOnlyWordData
    // is a pointer to a function. This version doesn't send echo back each 
    // character.
    //
    GetOnlyWordData = SCIA_GetOnlyWordData;

    SCIA_Init();
    SCIA_AutobaudLock();
    checksum = 0;

    //
    // If the KeyValue was invalid, abort the load and return the flash entry
    // point.
    //
    if (SCIA_GetOnlyWordData() != 0x08AA)
    {
        return FLASH_ENTRY_POINT;
    }


    //ReadCSMKeys();
    ReadReservedFn();

    EntryAddr = GetLongData();
    CopyData();

    return EntryAddr;
}

//
// SCIA_Init -  Initialize the SCI-A port for communications with the host
//
void SCIA_Init()
{
    // Enable the SCI-A clocks
    EALLOW;
    P_SysCtrlRegs->PCLKCR0.bit.SCIAENCLK=1;
    P_SysCtrlRegs->LOSPCP.all = 0x0002;
  
    //LCR[7]=1, enable, ����16550 ����ʲôʱ�򶼿��Զ�д
	   P_SciaRegs->SCILCR.bit.DLAB = 1;
	  //��Ƶ��������8λ ��ʼֵΪ0
	   P_SciaRegs->SCIDLH_SCIIER.SCIDLH_bits.DLH=0;
	   //P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCIDLL_bits.DLL=0;
	  //�л�����Ҫд��ļĴ���
	   P_SciaRegs->SCILCR.bit.DLAB = 0;
	  //��Ҫ����8λ���ݴ��� ��Ȼ��������
	   P_SciaRegs->SCILCR.bit.DLS = 3;
    
    // Enable pull-ups on SCI-A pins
    // P_GpioCtrlRegs->GPAPUD.bit.GPIO28 = 0;
    // P_GpioCtrlRegs->GPAPUD.bit.GPIO29 = 0;
    P_GpioCtrlRegs->GPAPUD.all &= 0xCFFFFFFF;
    // Enable the SCI-A pins   
    // P_GpioCtrlRegs->GPAMUX2.bit.GPIO28 = 1;
    // P_GpioCtrlRegs->GPAMUX2.bit.GPIO29 = 1;
    P_GpioCtrlRegs->GPAMUX2.all |= 0x05000000;
    // Input qual for SCI-A RX is asynch
    P_GpioCtrlRegs->GPAQSEL2.bit.GPIO28 = 3;
    EDIS;
    return;
}

//
// SCIA_AutobaudLock - Perform autobaud lock with the host. Note that if 
// autobaud never occurs the program will hang in this routine as there
// is no timeout mechanism included.
//
void SCIA_AutobaudLock()
{
    Uint16 byteData;

    EALLOW;
	
	//ʹ���Զ�������
	  P_SciaRegs->SCIAUTOBAUD.bit.CDC=1;
	//�������Զ������ж�
	  P_SciaRegs->SCIAUTOBAUD.bit.INTEN=0;
	  P_SciaRegs->SCIAUTOBAUD.bit.DONE = 0;
    // Wait until we correctly read an 
    // 'A' or 'a' and lock    
    while(P_SciaRegs->SCIAUTOBAUD.bit.DONE != 1) {}
    // After autobaud lock, clear the CDC bits
    P_SciaRegs->SCIAUTOBAUD.bit.CDC=0;
    P_SciaRegs->SCIAUTOBAUD.bit.DONE = 0;
    //write_reg32(MB_ADDR,P_SciaRegs->SCIAUTOBAUD.bit.REV);
    //�Զ������ʼ�������üĴ���������������Ϊ8-bitģʽ
    P_SciaRegs->SCILCR.bit.DLS = 3;
    
    P_SciaRegs->SCILCR.bit.DLAB=0;

    //�ȴ����ͼĴ���Ϊ�գ������ʼ��ɹ����ش���λ�����յ�������
    while(0 == P_SciaRegs->SCILSR.bit.THRE);
	  //���ݷ��ͼĴ���
	  byteData = P_SciaRegs->SCIAUTOBAUD.bit.REV;
    P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCITHR_bits.THR = byteData;
    
    //�����ʼ��ɹ��󣬽����µ����ݣ����ش���λ��
    while(P_SciaRegs->SCILSR.bit.DR != 1) { } 
    byteData = P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCIRBR_bits.RBR;
    //write_reg32(MB_ADDR,byteData);

    //�ȴ����ͼĴ���Ϊ��
    while(0 == P_SciaRegs->SCILSR.bit.THRE);
	  //���ݷ��ͼĴ���
    P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCITHR_bits.THR = byteData;
    EDIS;
    return;  
}

//
// SCIA_GetWordData - This routine fetches two bytes from the SCI-A port and 
// puts them together to form a single 16-bit value.  It is assumed that the 
// host is sending the data in the order LSB followed by MSB.
//
Uint16 SCIA_GetWordData()
{
   Uint16 wordData;
   Uint16 byteData;
  
   wordData = 0x0000;
   byteData = 0x0000;
   
   // Fetch the LSB and verify back to the host
   while(P_SciaRegs->SCILSR.bit.DR != 1) { } 
   wordData =  (Uint16)P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCIRBR_bits.RBR;
   while(0 == P_SciaRegs->SCILSR.bit.THRE);
	//���ݷ��ͼĴ���
   P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCITHR_bits.THR = wordData;

   // Fetch the MSB and verify back to the host
   while(P_SciaRegs->SCILSR.bit.DR != 1) { }  
   byteData =  (Uint16)P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCIRBR_bits.RBR;
   while(0 == P_SciaRegs->SCILSR.bit.THRE);
   P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCITHR_bits.THR = byteData;
   
   checksum += wordData + byteData;
   // form the wordData from the MSB:LSB
   wordData |= (byteData << 8);
   return wordData;
}

//
// SCIA_GetOnlyWordData - 
//
Uint16 SCIA_GetOnlyWordData()
{
    Uint16 wordData;
   Uint16 byteData;
  
   wordData = 0x0000;
   byteData = 0x0000;
   
   // Fetch the LSB and verify back to the host
   while(P_SciaRegs->SCILSR.bit.DR != 1) { } 
   wordData =  (Uint16)P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCIRBR_bits.RBR;
   //while(0 == P_SciaRegs->SCILSR.bit.THRE);
	//���ݷ��ͼĴ���
   //P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCITHR_bits.THR = wordData;

   // Fetch the MSB and verify back to the host
   while(P_SciaRegs->SCILSR.bit.DR != 1) { }  
   byteData =  (Uint16)P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCIRBR_bits.RBR;
   //while(0 == P_SciaRegs->SCILSR.bit.THRE);
   //P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCITHR_bits.THR = byteData;
   
   checksum += wordData + byteData;
   // form the wordData from the MSB:LSB
   wordData |= (byteData << 8);
   return wordData;
}

//
// End of File
//

