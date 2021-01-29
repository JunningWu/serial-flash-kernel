//###########################################################################
//
// FILE:    Shared_Boot.c
//
// TITLE:   Boot loader shared functions
//
// Functions:
//
//     void   CopyData(void)
//     Uint32 GetLongData(void)
//     void ReadReservedFn(void)
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
#include "Flash28034_API_Library.h"

//
// GetWordData is a pointer to the function that interfaces to the peripheral.
// Each loader assigns this pointer to it's particular GetWordData function.
//
uint16fptr GetOnlyWordData;

//
// Function prototypes
//
Uint32 GetLongData();
Uint16 CsmUnlock_Kernel();
void   CopyData(void);
void ReadReservedFn(void);

//
// After flash_program, send checksum to PC
//
void SendCheckSum();
void ReadCSMKeys();
//
// Programming Buffer
//
Uint32 progBuf[PROG_BUFFER_LENGTH];

//
// Flash Status Structure
//
FLASH_ST FlashStatus;
FLASH_ST FlashProgStatus,FlashEraseStatus;

//extern Uint32 Flash_CPUScaleFactor;
//extern void (*Flash_CallbackPtr) (void);
unsigned int checksum;

Uint32 CSMKEY0, CSMKEY1, CSMKEY2, CSMKEY3;


//
// CsmUnlock -  This function unlocks the CSM. User must replace 0xFFFF's with 
// current password for the DSP. Returns 1 if unlock is successful.
//
#define FAIL          1
#define SUCCESS       0
Uint16
CsmUnlock_Kernel()
{
    volatile Uint32 temp;

    //
    // Load the key registers with the current password. The 0xFFFF's are dummy
    // passwords.  User should replace them with the correct password for the 
    // DSP.
    //
    //EALLOW;
    //CsmRegs.KEY0 = 0xFFFFFFFF;
    //CsmRegs.KEY1 = 0xFFFFFFFF;
    //CsmRegs.KEY2 = 0xFFFFFFFF;
    //CsmRegs.KEY3 = 0xFFFFFFFF;
    //CsmRegs.KEY0 = CSMKEY0;
    //CsmRegs.KEY1 = CSMKEY1;
    //CsmRegs.KEY2 = CSMKEY2;
    //CsmRegs.KEY3 = CSMKEY3;
    //EDIS;

    //
    // Perform a dummy read of the password locations if they match the key 
    // values, the CSM will unlock
    //
    temp = P_CsmPwl->PSWD0;
    temp = P_CsmPwl->PSWD1;
    temp = P_CsmPwl->PSWD2;
    temp = P_CsmPwl->PSWD3;

    //
    // If the CSM unlocked, return succes, otherwise return failure.
    //
    if (CsmRegs.CSMSCR.bit.SECURE == 0)
    {
        return SUCCESS;
    }
    else
    {
        return FAIL;
    }
}

//
// CopyData - This routine copies multiple blocks of data from the host to the 
// specified RAM locations.  There is no error checking on any of the 
// destination addresses. That is it is assumed all addresses and block size
// values are correct. Multiple blocks of data are copied until a block
// size of 00 00 is encountered.
//
void
CopyData()
{
    struct HEADER 
    {
        Uint16 BlockSize;
        Uint32 DestAddr;
        Uint32 ProgBuffAddr;
    } BlockHeader;

    Uint32 wordData;
    Uint16 status;
    Uint16 i,j;

    //
    // Make sure code security is disabled
    //
    CsmUnlock_Kernel();

    //EALLOW;
    //Flash_CPUScaleFactor = SCALE_FACTOR;
    //Flash_CallbackPtr = NULL;
    //EDIS;
    for(i=0;i<128;i++){
       status = (*Flash28034_Erase)(i, &FlashStatus);
       //success == 0
       //status = Status = (*Flash28027_ChipErase)(&FlashEraseStatus);
       if(status != SUCCESS)
       {
        //
        // TODO fix so that it returns a serial error and reboot device
        //
        return;
       }
    }



    //
    // After Flash Erase, send the checksum to PC program.
    //
    SendCheckSum();
    
    //
    // Get the size in words of the first block
    //
    BlockHeader.BlockSize = (*GetOnlyWordData)()/4;

    //
    // While the block size is > 0 copy the data to the DestAddr. There is no 
    // error checking as it is assumed the DestAddr is a valid memory location
    //
    while(BlockHeader.BlockSize != (Uint16)0x0000)
    {
        if(BlockHeader.BlockSize > PROG_BUFFER_LENGTH)
        {
            //
            // Block is to big to fit into our buffer so we must program it in 
            // chunks
            //
            BlockHeader.DestAddr = GetLongData();
            
            //
            // Program as many full buffers as possible
            //
            for(j = 0; j < (BlockHeader.BlockSize / PROG_BUFFER_LENGTH); j++)
            {
                BlockHeader.ProgBuffAddr = (Uint32)progBuf;
                for(i = 1; i <= PROG_BUFFER_LENGTH; i++)
                {
                    wordData = (*GetOnlyWordData)();
                    wordData = ((*GetOnlyWordData)()<<16) | wordData;
                    *(Uint32 *)BlockHeader.ProgBuffAddr = wordData;
                    BlockHeader.ProgBuffAddr +=4;
                }
                status = (*Flash28034_Program)((Uint32 *) BlockHeader.DestAddr,
                        (Uint32 *)progBuf, PROG_BUFFER_LENGTH, &FlashStatus);
                if(status != SUCCESS)
                {
                    return;
                }
                BlockHeader.DestAddr += PROG_BUFFER_LENGTH*4;
                
                //
                // After Flash program, send the checksum to PC program.
                //
                SendCheckSum();
            }
            
            //
            // Program the leftovers
            //
            BlockHeader.ProgBuffAddr = (Uint32)progBuf;
            for(i = 1; i <= (BlockHeader.BlockSize % PROG_BUFFER_LENGTH); i++)
            {
                wordData = (*GetOnlyWordData)();
                wordData = ((*GetOnlyWordData)()<<16) | wordData;
                *(Uint32 *)BlockHeader.ProgBuffAddr = wordData;
                BlockHeader.ProgBuffAddr +=4;
            }
            status = (*Flash28034_Program)((Uint32 *) BlockHeader.DestAddr,
                                   (Uint32 *)progBuf, (BlockHeader.BlockSize % 
                                   PROG_BUFFER_LENGTH), &FlashStatus);
            if(status != SUCCESS)
            {
                return;
            }
            
            //
            // After Flash program, send the checksum to PC program.
            //
            SendCheckSum();
        }
        
        else
        {
            //
            // Block will fit into our buffer so we'll program it all at once
            //
            BlockHeader.DestAddr = GetLongData();
            BlockHeader.ProgBuffAddr = (Uint32)progBuf;
            for(i = 1; i <= BlockHeader.BlockSize; i++)
            {
                wordData = (*GetOnlyWordData)();
                wordData = ((*GetOnlyWordData)()<<16) | wordData;
                *(Uint32 *)BlockHeader.ProgBuffAddr = wordData;
                BlockHeader.ProgBuffAddr +=4;
            }
            status = (*Flash28034_Program)((Uint32 *) BlockHeader.DestAddr, (Uint32 *)progBuf, BlockHeader.BlockSize, &FlashStatus);
            if(status != SUCCESS)
            {
                return;
            }
            
            //
            // After Flash program, send the checksum to PC program.
            //
            SendCheckSum();
        }

        //
        // Get the size of the next block
        //
        BlockHeader.BlockSize = (*GetOnlyWordData)()/4;
    }
    return;
}

//
// GetLongData - This routine fetches a 32-bit value from the peripheral input 
// stream.
//
Uint32
GetLongData()
{
    Uint32 longData;

    //
    // Fetch the upper 1/2 of the 32-bit value
    //
    longData = ( (Uint32)(*GetOnlyWordData)() << 16);

    //
    // Fetch the lower 1/2 of the 32-bit value
    //
    longData |= (Uint32)(*GetOnlyWordData)();

    return longData;
}

//
// Read_ReservedFn - This function reads 8 reserved words in the header. None 
// of these reserved words are used by the this boot loader at this time, they 
// may be used in future devices for enhancements.  Loaders that use these 
// words use their own read function.
//
void
ReadReservedFn()
{
    Uint16 i;
    
    //
    // Read and discard the 8 reserved words.
    //
    for(i = 1; i <= 8; i++)
    {
        GetOnlyWordData();
    }
    return;
}

//
// ReadCSMKeys - This function reads 4 reserved Uint32 in the header.  
// These are CSM Keys, CSMKEY0,CSMKEY1,CSMKEY2,CSMKEY3
//
void
ReadCSMKeys()
{
    Uint32 temp;
    
    
    temp = GetOnlyWordData();
    CSMKEY0 = (temp<<16) | GetOnlyWordData();
    temp = GetOnlyWordData();
    CSMKEY1 = (temp<<16) | GetOnlyWordData();
    temp = GetOnlyWordData();
    CSMKEY2 = (temp<<16) | GetOnlyWordData();
    temp = GetOnlyWordData();
    CSMKEY3 = (temp<<16) | GetOnlyWordData();
    
    
    return;
}


//
// SendCheckSum - This function sends checksum to PC program. After flash 
// memory erases or writes something, this functions will be running
//
void
SendCheckSum()
{
    while(0 == P_SciaRegs->SCILSR.bit.THRE)
    {
        
    }
    P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCITHR_bits.THR = checksum & 0xFF;

    while(0 == P_SciaRegs->SCILSR.bit.THRE)
    {
        
    }
    P_SciaRegs->SCIRBR_SCIDLL_SCITHR.SCITHR_bits.THR = (checksum >> 8) & 0xFF;

    checksum = 0;

    return;
}

//
// InitFlash -This function initializes the Flash Control registers
//                   CAUTION
// This function MUST be executed out of RAM. Executing it
// out of OTP/Flash will yield unpredictable results
//
void
InitFlash_Kernel(void)
{
    EALLOW;
    
    //
    // Enable Flash Pipeline mode to improve performance of code executed from
    // Flash.
    //
    P_FlashRegs->FOPT.bit.ENPIPE = 1;
    
    EDIS;

    //
    // Force a pipeline flush to ensure that the write to the last register 
    // configured occurs before returning.
    //
    asm volatile (".align 2;RPTI 40,4;NOP");
}

//
// End of File
//

