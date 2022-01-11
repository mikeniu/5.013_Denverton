//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SbRun.c
    This file contains code for SouthBridge runtime
    protocol

*/

// Module specific Includes

#include "Efi.h"
#include "token.h"
#include <AmiDxeLib.h>
#include <Library/AmiChipsetRuntimeServiceLib.h>

// Produced Protocols
#include <Protocol/Metronome.h>
#include <Protocol/RealTimeClock.h>
#include <Library/SbCspLib.h>
#include <Library/PcdLib.h>

#ifdef AMI_MODULE_PKG_VERSION
#if AMI_MODULE_PKG_VERSION >= 28
#include <Guid/AmiResetSystemEvent.h>
#endif
#endif
//----------------------------------------------------------------------------
//          Variable Declaration
//----------------------------------------------------------------------------
EFI_EVENT gAmiSystemResetEvent;
static BOOLEAN gExitBs = FALSE;

#ifdef AMI_MODULE_PKG_VERSION
#if AMI_MODULE_PKG_VERSION < 28
#define AMI_RESET_SYSTEN_EVENT_GUID { 0x62da6a56, 0x13fb, 0x485a, 0xa8, 0xda, 0xa3, 0xdd, 0x79, 0x12, 0xcb, 0x6b }
EFI_GUID gAmiResetSystemEventGuid = AMI_RESET_SYSTEN_EVENT_GUID;
#endif
#endif
// Denverton AptioV override start - EIP#238625
/*
// Function Prototypes
EFI_STATUS
WaitForTick (
    IN  EFI_METRONOME_ARCH_PROTOCOL     *This,
    IN  UINT32                          TickNumber
);

// Architectural Protocol Definitions
EFI_METRONOME_ARCH_PROTOCOL mMetronomeProtocol = {
    WaitForTick,
    //
    // The value of TickPeriod in EFI_METRONOME_ARCH_PROTOCOL on any
    // platform must be at least 10 uS and must not exceed 200 uS.
    // The unit of TickPeriod is 100ns.
    // 100 = 10uS/100ns
    // The value of TickPeriod should be between 100 and 2000.
    //
    TICK_PERIOD
};

// Function Definitions

//----------------------------------------------------------------------------
//   USUALLY NO PORTING REQUIRED FOR THE FOLLOWING ROUTINES
//----------------------------------------------------------------------------

/**
    This function calculates the time needed to delay and then
    calls a library function to delay that amount of time

    @param This Pointer to the instance of the Metronome Arch Protocol
    @param TickNumber Number of ticks needed based off of tick period
                      defined in Protocol Definition

    @retval Return Status based on errors that occurred while waiting for
            time to expire.

/
EFI_STATUS
WaitForTick (
    IN  EFI_METRONOME_ARCH_PROTOCOL     *This,
    IN  UINT32                          TickNumber
    )
{
    EFI_STATUS          Status;
    UINT32              TotalTime;

    // Manipulate TickNumber into a valid value for the library function call
    // the Current Resolution is 10us.
    // The Library uses Microseconds to count delayed time.
    TotalTime = (TickNumber * This->TickPeriod) / 10;

    // Make Library Function call here
    Status = CountTime(TotalTime, PcdGet16 (PcdAcpiIoPortBaseAddress));

    return Status;
}

/**
    This function is the interface for the reset function.

    @param ResetType - Type of reset to perform
    @param ResetStatus - System status that caused the reset.  if part
                         of normal operation then this should be
                         EFI_SUCCESS, Otherwise it should reflect the
                         state of the system that caused it
    @param DataSize - Size in bytes of the data to be logged
    @param ResetData - Pointer to the data buffer that is to be logged

    @retval None, Even though it should never get that far

**/
EFI_STATUS
AmiChipsetRuntimeResetSystem (
    IN EFI_RESET_TYPE   ResetType,
    IN EFI_STATUS       ResetStatus,
    IN UINTN            DataSize,
    IN CHAR16           *ResetData OPTIONAL
    )
{
    EFI_STATUS              Status;

    if (!gExitBs) {
      //
      // Signal AmiResetSystem Event
      //
      pBS->SignalEvent(gAmiSystemResetEvent);
      pBS->CloseEvent(gAmiSystemResetEvent);
    }

    Status = AmiChipsetResetSystem (ResetType, ResetStatus, DataSize, ResetData);

    // This should not get here
    return Status;
}

/**
    Submit runtime services both SMM and runtime.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
            time to expire.

**/
EFI_STATUS
SbRuntimeServiceInit (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
    pRS->ResetSystem    = AmiChipsetRuntimeResetSystem;
	// RTC functionalities are from RC drivers
    /*
    pRS->GetTime        = AmiChipsetGetTime;
    pRS->SetTime        = AmiChipsetSetTime;
    pRS->GetWakeupTime  = AmiChipsetGetWakeupTime;
    pRS->SetWakeupTime  = AmiChipsetSetWakeupTime;
    */
    return EFI_SUCCESS;
}

/**
    A dummy function.

    @param Event   Event of callback
    @param Context Pointer to context of callback

    @retval VOID

**/
VOID SbRunDummyFunction (
    IN EFI_EVENT  Event,
    IN VOID       *Context
    )
{
}

/**
    Update the global variable ExitBs before exiting boot services.

    @param Event   Event of callback
    @param Context Pointer to context of callback

    @retval VOID

**/

VOID SbExitBs (
    IN EFI_EVENT  Event,
    IN VOID       *Context
    )
{
    gExitBs = TRUE;
}

/**
    This function is the entry point for this DXE. This function
    installs the runtime services related to SB

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
            time to expire.

**/
EFI_STATUS
SbRuntimeInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    EFI_HANDLE              Handle = NULL;
    EFI_EVENT               ExitBsEvent;

    InitAmiRuntimeLib(ImageHandle, SystemTable, NULL, NULL);

    PROGRESS_CODE(DXE_SBRUN_INIT);
    //
    // Install runtime services
    //
    Status = SbRuntimeServiceInit(ImageHandle, SystemTable);
    ASSERT_EFI_ERROR(Status);

    Status = pBS->InstallMultipleProtocolInterfaces (
                    &Handle,
                    &gEfiResetArchProtocolGuid, NULL,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);

    /*
    Status = pBS->InstallProtocolInterface (
                    &ImageHandle, \
                    &gEfiMetronomeArchProtocolGuid, \
                    EFI_NATIVE_INTERFACE, \
                    &mMetronomeProtocol
                    );
    ASSERT_EFI_ERROR(Status);
    */
    //
    // Create the event for AmiResetSystem
    //
    Status = pBS->CreateEventEx(
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    SbRunDummyFunction,
                    NULL,
                    &gAmiResetSystemEventGuid,
                    &gAmiSystemResetEvent
                    );
    //
    // Do not signal AmiResetSystem Event while it is failed
    // to create the event for AmiResetSystem.
    //
    if (EFI_ERROR (Status)) {
      gExitBs = TRUE;
    }
    //
    // Update the global variable ExitBs before exiting boot services.
    //
    Status = pBS->CreateEvent(
                    EVT_SIGNAL_EXIT_BOOT_SERVICES,
                    TPL_CALLBACK,
                    SbExitBs,
                    NULL,
                    &ExitBsEvent
                    );
    //
    // Do not signal AmiResetSystem Event while it is failed
    // to update the global variable ExitBs.
    //
    if (EFI_ERROR (Status)) {
      gExitBs = TRUE;
    }
    /*
    //
    // This protocol is to notify core that the Runtime Table has been
    // updated, so it can update the runtime table CRC.
    //
    Handle = NULL;
    return pBS->InstallMultipleProtocolInterfaces (
                  &Handle, \
                  &gEfiRealTimeClockArchProtocolGuid, \
                  NULL, \
                  NULL
                  );
    */
    return EFI_SUCCESS;
}
// Denverton AptioV override end - EIP#238625
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
