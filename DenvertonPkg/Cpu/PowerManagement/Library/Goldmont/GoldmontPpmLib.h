/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  GoldmontPpmLib.h
    
Abstract:

  This library contains power management configuration functions specific to 
  Denverton processors.

  Acronyms:
    PPM   Processor Power Management
    GV    Geyserville
    TM    Thermal Monitor
    IST   Intel(R) Speedstep technology
    HT    Hyper-Threading Technology
    CMP   Core Multi-Processing

--*/

#ifndef _GOLDMONT_PPM_LIB_H_
#define _GOLDMONT_PPM_LIB_H_

//
// Include files
//
#include <GoldmontPpmDefines.h>
#include <PiDxe.h>
#include <PpmCommon.h>
#include <Protocol/MpService.h>

#include <Protocol/PpmPlatformPolicy.h>
#include <Protocol/PpmProcessorSupport2.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>


extern EFI_MP_SERVICES_PROTOCOL  *mMpServices;

typedef struct {
  UINT32  RegEax;
  UINT32  RegEbx;
  UINT32  RegEcx;
  UINT32  RegEdx;
} EFI_CPUID_REGISTER;
//
// Stall period in microseconds
//
#define PPM_WAIT_PERIOD  15

//
// Function prototypes
//
/*++

Routine Description:

  Initializes the platform power management library.  This must be called
  prior to any of the library functions being used.

  At this time, we don't properly publish the PPM processor support protocol,
  we simply return it if this library implements the protocol.

  If the processor is not supported, the input will not be modified.

Arguments:
  
  This          Pointer to the PPM support protocol instance

Returns:

  EFI_SUCCESS         Library was initialized successfully
  EFI_NOT_SUPPORTED   The library does not support the current processor

--*/
EFI_STATUS
InitializePowerManagementLib (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   **This
  );

STATIC 
VOID 
ConfigureFreqLimit (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy
  );

STATIC
VOID
ConfigureRaplLimit (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy
  );

UINT8
Time2TAU (
  IN UINT32 PackageRaplTimeWindow
  );

STATIC
EFI_STATUS 
SetPpmFlags (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );

STATIC
EFI_STATUS
EnableCStates (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );


VOID
ApSafeEnableCStates (
  IN OUT VOID                               *Buffer
  );

STATIC
EFI_STATUS
InitThermal (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy
  );

VOID
ApSafeInitThermal (
  IN OUT VOID                               *Buffer
  );

STATIC
EFI_STATUS
EnableTm (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );

VOID
ApSafeEnableTm (
  IN OUT VOID                               *Buffer
  );

STATIC
EFI_STATUS
EnableProcHot (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );

STATIC
EFI_STATUS
EnableTsc (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT FVID_TABLE                         *FvidPointer,
  IN     UINT16                             iFSBFrequency,
  IN OUT UINT8                              *PpmCstTmrFlags,
  IN OUT UINTN                              *PpmTscCorrFactor,
  IN OUT UINTN                              *PpmTscCorrFactorRem
  );

STATIC
EFI_STATUS
S3SaveMsr (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );

STATIC
EFI_STATUS
S3RestoreMsr (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );

VOID
ApSafeRestoreMsr (
  IN OUT VOID                               *Buffer
  );

STATIC
EFI_STATUS
EnableCStateIoRedirection (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );

VOID
ApSafeEnableCStateIoRedirection (
  IN OUT VOID                               *Buffer
  );

STATIC
EFI_STATUS
DisableCStateIoRedirection (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );

VOID
ApSafeDisableCStateIoRedirection (
  IN OUT VOID                               *Buffer
  );

STATIC
EFI_STATUS
EnablePStateHardwareCoordination (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );

VOID
ApSafeEnablePStateHardwareCoordination (
  IN OUT VOID                               *Buffer
  );

STATIC
EFI_STATUS
DisablePStateHardwareCoordination (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );

VOID
ApSafeDisablePStateHardwareCoordination (
  IN OUT VOID                               *Buffer
  );

STATIC
EFI_STATUS
InitFvidTable (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT FVID_TABLE                         *FvidPointer,
  IN     UINT16                             MaxNumberOfStates,
  IN     UINT16                             MinStepSize,
  IN     BOOLEAN                            CreateDefaultTable
  );

STATIC
VOID
ApSafeEnableTurboMode (
  IN OUT VOID                               *Buffer 
  );

STATIC
EFI_STATUS
CreateFvidTable (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT FVID_TABLE                         *FvidPointer,
  IN     UINT16                             MaxNumberOfStates
  );

STATIC
VOID
CreateDefaultFvidTable (
  IN OUT FVID_TABLE                         *FvidPointer
  );

VOID
InitEist (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy,
  IN OUT FVID_TABLE                         *FvidPointer
  );

VOID
ApSafeEnableEist (
  IN OUT VOID                               *Buffer
  );

STATIC
EFI_STATUS
RunOnAllLogicalProcessors (
  IN OUT EFI_AP_PROCEDURE   Procedure,
  IN OUT VOID               *Buffer
  );
/*++

Routine Description:

  Runs the specified procedure on all logical processors, passing in the
  parameter buffer to the procedure.

Arguments:
  
  Procedure     The function to be run.
  Buffer        Pointer to a parameter buffer.

Returns:

  EFI_SUCCESS

--*/
STATIC
EFI_STATUS
SetBootPState (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  )
/*++

Routine Description:

  Set processor P state to HFM or LFM

Arguments:
  
  This          Pointer to the protocol instance

Returns:
  EFI_UNSUPPORTED EIST not supported.
  EFI_SUCCESS     Processor P state has been set.

--*/
;

/**
  The function makes the crash dump from the PUnit through the B2P mailbox only
  when this log is supported and available, ergo, the log was generated because an
  error occurred in the previous boot (i.e system crashed or hung).

  @param[in]      None
  @param[in out]  None

  @retval         EFI_NOT_FOUND           Crash log doesn't exist due there was not a crashed or hung system
                  EFI_INVALID_PARAMETER   Intended address to access is not a valid address
                  EFI_UNSUPPORTED         Crash dump is not supported by the B2P mailbox or the PUnit
                  EFI_END_OF_MEDIA        End of the Crash log was reached
                  EFI_SUCCESS             Crash dump was successfully made
**/
EFI_STATUS
GetCrashLogDump (
  );

VOID
ApSafeSetBootPState (
  IN OUT VOID *Buffer
  )
/*++

Routine Description:

  Set processor P state to HFM or LFM.

Arguments:
  
  Buffer          Unused

Returns:

  EFI_SUCCESS   Processor MSR setting is saved.

--*/
;
#endif
