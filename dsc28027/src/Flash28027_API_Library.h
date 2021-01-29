#ifndef FLASH28027_API_LIBRARY_H
#define FLASH28027_API_LIBRARY_H

#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------
 API Status Messages

 The following status values are returned from the API to the calling
 program.  These can be used to determine if the API function passed
 or failed.  
---------------------------------------------------------------------------*/

// The CSM is preventing the function from performing its operation
#define STATUS_FAIL_CSM_LOCKED               10

// Device REVID does not match that required by the API
#define STATUS_FAIL_REVID_INVALID            11
    
// Invalid address passed to the API
#define STATUS_FAIL_ADDR_INVALID             12

// Incorrect PARTID
// For example the F2806 API was used on a F2808 device. 
#define STATUS_FAIL_INCORRECT_PARTID         13

// API/Silicon missmatch.  An old version of the
// API is being used on silicon it is not valid for
// Please update to the latest API. 
#define STATUS_FAIL_API_SILICON_MISMATCH     14

// ---- Erase Specific errors ---- 
#define STATUS_FAIL_NO_SECTOR_SPECIFIED      20
#define STATUS_FAIL_PRECONDITION             21
#define STATUS_FAIL_ERASE                    22
#define STATUS_FAIL_COMPACT                  23
#define STATUS_FAIL_PRECOMPACT               24

// ---- Program Specific errors ----  
#define STATUS_FAIL_PROGRAM                  30
#define STATUS_FAIL_ZERO_BIT_ERROR           31

// ---- Verify Specific errors ----
#define STATUS_FAIL_VERIFY                   40

// Busy is set by each API function before it determines
// a pass or fail condition for that operation.  
// The calling function will will not receive this 
// status condition back from the API
#define STATUS_BUSY                999    

/*---------------------------------------------------------------------------
 Flash sector mask definitions

 The following macros can be used to form a mask specifying which sectors
 will be erased by the erase API function.
 
---------------------------------------------------------------------------*/

#define SECTOR00   (Uint32)0x00000001
#define SECTOR01   (Uint32)0x00000002
#define SECTOR02   (Uint32)0x00000004
#define SECTOR03   (Uint32)0x00000008
#define SECTOR04   (Uint32)0x00000010
#define SECTOR05   (Uint32)0x00000020
#define SECTOR06   (Uint32)0x00000040
#define SECTOR07   (Uint32)0x00000080
#define SECTOR08   (Uint32)0x00000100
#define SECTOR09   (Uint32)0x00000200
#define SECTOR10   (Uint32)0x00000400
#define SECTOR11   (Uint32)0x00000800
#define SECTOR12   (Uint32)0x00001000
#define SECTOR13   (Uint32)0x00002000
#define SECTOR14   (Uint32)0x00004000
#define SECTOR15   (Uint32)0x00008000
#define SECTOR16   (Uint32)0x00010000
#define SECTOR17   (Uint32)0x00020000
#define SECTOR18   (Uint32)0x00040000
#define SECTOR19   (Uint32)0x00080000
#define SECTOR20   (Uint32)0x00100000
#define SECTOR21   (Uint32)0x00200000
#define SECTOR22   (Uint32)0x00400000
#define SECTOR23   (Uint32)0x00800000
#define SECTOR24   (Uint32)0x01000000
#define SECTOR25   (Uint32)0x02000000
#define SECTOR26   (Uint32)0x04000000
#define SECTOR27   (Uint32)0x08000000
#define SECTOR28   (Uint32)0x10000000
#define SECTOR29   (Uint32)0x20000000
#define SECTOR30   (Uint32)0x40000000
#define SECTOR31   (Uint32)0x80000000


typedef struct{
	Uint32 *StartAddr;
	Uint32 *EndAddr;
	}SECTOR;
	
#define FLASH_START_ADDR 0x7C0000
#define FLASH_END_ADDR   0x7E0000




#define Flash28027_APIVersionHex (Uint16 (*)(void))0x7fe450
#define Flash28027_Program (Uint16 (*)(Uint32 *FlashAddr, Uint32*BufAddr, Uint32 Length, FLASH_ST *FProgStatus))0x7fde00
#define Flash28027_ChipErase (Uint16 (*)(FLASH_ST *FEraseStat))0x7fe25e
#define Flash28027_ChipEraseLast (Uint16 (*)(FLASH_ST *FEraseStat))0x7fe33e
#define Flash28027_Verify (Uint16 (*)(Uint32 *FlashAddr, Uint32 *BufAddr, Uint32 Length, FLASH_ST *FVerifyStat))0x7fdf24
#define Flash28027_ProgVerify (Uint16 (*)(Uint32 *FlashAddr, Uint32 Length, FLASH_ST *FVerifyStat))0x7fdef6
#define Flash28027_Erase (Uint16 (*)(Uint64 SectorMask, FLASH_ST *FEraseStat))0x7fe09a
#define Flash28027_EraseVerify (Uint16 (*)(Uint32 *FlashAddr, Uint32 Length, FLASH_ST *FVerifyStat))0x7fe06C


// All sectors on an F2802x - Sectors 0 - 31
#define SECTOR_F28027 (0xFFFFFFFF)


/*---------------------------------------------------------------------------
 API Status Structure
 
 This structure is used to pass debug data back to the calling routine.
 Note that the Erase API function has 3 parts: precondition, erase and
 and compaction. Erase and compaction failures will not populate 
 the expected and actual data fields.  
---------------------------------------------------------------------------*/

typedef struct {
    Uint32  FirstFailAddr;
    Uint16  ExpectedData;
    Uint16  ActualData;
}FLASH_ST;


/*---------------------------------------------------------------------------
   Frequency Scale factor:
   The calling program must provide this global parameter used
   for frequency scaling the algo's.
----------------------------------------------------------------------------*/

extern Uint32 Flash_CPUScaleFactor;

/*---------------------------------------------------------------------------
   Callback Function Pointer:
   A callback function can be specified.  This function will be called
   at safe times during erase, program and verify.  This function can
   then be used to service an external watchdog or send a communications
   packet.
   
   Note: 
   THE FLASH AND OTP ARE NOT AVAILABLE DURING THIS FUNCTION CALL.
   THE FLASH/OTP CANNOT BE READ NOR CAN CODE EXECUTE FROM IT DURING THIS CALL
   DO NOT CALL ANY OF THE THE FLASH API FUNCTIONS DURING THIS CALL
----------------------------------------------------------------------------*/
extern void (*Flash_CallbackPtr) (void);


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif // -- end FLASH2802x_API_LIBRARY_H 

// --------- END OF FILE ----------------------------------

