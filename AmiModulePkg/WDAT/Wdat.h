//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2013, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Pkwy, Norcross, GA 30093                 **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

/** @file Wdat.h 
    Entry point for WDAT initialization. 

**/

#ifndef __WDAT_H__
#define __WDAT_H__

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <token.h>
#include <ACPI30.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

///
/// WDAT Signature
///
#define WDAT_SIG    EFI_SIGNATURE_32('W', 'D', 'A', 'T')

///--------------------------------------------------------------
///Watchdog(WD) Flag Value FOR WDIE_20 STRUCT USE
///--------------------------------------------------------------
#define WD_ENABLED                  0x1
#define WD_STOPPED_IN_SLEEP_STATE   0x80

///--------------------------------------------------------------
///Watchdog Actions (WDA) Value FOR WDIE_20 STRUCT USE
///--------------------------------------------------------------
#define WDA_RESET                           0x1
#define WDA_QUERY_CURRENT_COUNTDOWN_PERIOD  0x4
#define WDA_QUERY_COUNTDOWN_PERIOD          0x5
#define WDA_SET_COUNTDOWN_PERIOD            0x6
#define WDA_QUERY_RUNNING_STATE             0x8
#define WDA_SET_RUNNING_STATE               0x9
#define WDA_QUERY_STOPPED_STATE             0xA
#define WDA_SET_STOPPED_STATE               0xB
#define WDA_QUERY_REBOOT                    0x10
#define WDA_SET_REBOOT                      0x11
#define WDA_QUERY_SHUTDOWN                  0x12
#define WDA_SET_SHUTDOWN                    0x13
#define WDA_QUERY_WATCHDOG_STATUS           0x20
#define WDA_SET_WATCHDOG_STATUS             0x21

///--------------------------------------------------------------
///Watchdog Instruction(WDI) Flags Value
///--------------------------------------------------------------
#define WDI_READ_VALUE          0x0
#define WDI_READ_COUNTDOWN      0x1
#define WDI_WRITE_VALUE         0x2
#define WDI_WRITE_COUNTDOWN     0x3
#define WDI_PRESERVE_REGISTER   0x80

// Macro Definition(s)
#define EFI_SIGNATURE_16(A, B)        ((A) | (B << 8))
#define EFI_SIGNATURE_32(A, B, C, D) \
    (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))

// Type Definition(s)
#pragma pack(1)
///----------------------------------------------------------------
///WDAT_HDR = Watchdog Header Structure
///----------------------------------------------------------------
typedef struct _WDAT_Header
{
    UINT32  WatchdogHeaderLength;
    UINT16  PciSegment;
    UINT8   PciBusNumber;
    UINT8   PciDeviceNumber;
    UINT8   PciFuncitonNumber;
    UINT8   Reserved1[3];   
    UINT32  TimerPeriod;
    UINT32  MaximumCount;
    UINT32  MinimumCount;
    UINT8   WatchdogFlags;
    UINT8   Reserved2[3];
    UINT32  NumberWactchdogInstructionEntries;
}WDAT_HDR;

///------------------------------------------------------------------------------------
/// WDAT_INST_ENTRY = Watchdog Instruction Entries Structure->FOR EDAT PARAMENTER USE
///------------------------------------------------------------------------------------
typedef struct _Watchdog_Instruction_Entries
{
    UINT8   WatchdogAction;
    UINT8   InstructionFlags;
    UINT16  Reserved1;
    GAS_30  RegisterRegion;
    UINT32  Value;
    UINT32  Mask;
}WDAT_INST_ENTRY;

///-----------------------------------------------------------------
///WDAT = Watchdog Action Table
///-----------------------------------------------------------------
typedef struct _WDA_TABLE
{
    ACPI_HDR        AcpiHeader;   //0..35
    WDAT_HDR        WatchdogHeader;
}WDA_TABLE;
#pragma pack()

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------  


//=============================================================

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2013, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Pkwy, Norcross, GA 30093                 **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
