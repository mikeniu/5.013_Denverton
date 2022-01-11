/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  CpuPpmLib.h

Abstract:

  IST/PPM reference code definitions.

  Acronyms:
    PPM   Platform Power Management
    GV    Geyserville
    TM    Thermal Monitor
    IST   Intel(R) Speedstep technology

--*/

#ifndef _CPU_PPM_LIB_H_
#define _CPU_PPM_LIB_H_

//
// Statements that include other files
//
#include <PiDxe.h>

#include <PpmCommon.h>
#include <Protocol/PpmProcessorSupport2.h>

//
// CPUID definitions
//
#define CPUID_EXT_FAMILY                  0x0FF00000
#define CPUID_EXT_MODEL                   0x000F0000
#define CPUID_FAMILY                      0x00000F00
#define CPUID_MODEL                       0x000000F0
#define CPUID_STEPPING                    0x0000000F
#define CPUID_FAMILY_MODEL                0x00000FF0
#define CPUID_FAMILY_MODEL_STEPPING       0x00000FFF
#define CPUID_FULL_FAMILY_MODEL           0x0FFF0FF0
#define CPUID_FULL_FAMILY_MODEL_STEPPING  0x0FFF0FFF
#define CPUID_FULL_FAMILY_MODEL_VALLEVIEW 0x00030670  // Valleyview
//
// Structure declarations
//

//
// UINT64 workaround
//
// The MS compiler doesn't handle QWORDs very well.  I'm breaking
// them into DWORDs to circumvent the problems.  Converting back
// shouldn't be a big deal.
//
#pragma pack(1)
typedef union _MSR_REGISTER {
  UINT64  Qword;

  struct _DWORDS {
    UINT32  Low;
    UINT32  High;
  } Dwords;

  struct _BYTES {
    UINT8 FirstByte;
    UINT8 SecondByte;
    UINT8 ThirdByte;
    UINT8 FouthByte;
    UINT8 FifthByte;
    UINT8 SixthByte;
    UINT8 SeventhByte;
    UINT8 EighthByte;
  } Bytes;

} MSR_REGISTER;
#pragma pack()

typedef struct _EFI_MSR_VALUES{
  UINT16  Index;
  UINT64  Value;
} EFI_MSR_VALUES;

//
// Function prototypes
//
EFI_STATUS
InitializePenrynPowerManagementLib (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   **This
  );
/*++

Routine Description:

  Initializes the processor power management library.  This must be called
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
InitializeMeromPowerManagementLib (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   **This
  );
/*++

Routine Description:

  Initializes the processor power management library.  This must be called
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
InitializeYonahPowerManagementLib (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   **This
  );
/*++

Routine Description:

  Initializes the processor power management library.  This must be called
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
InitializeDothanPowerManagementLib (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   **This
  );
/*++

Routine Description:

  Initializes the processor power management library.  This must be called
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
InitializeValleyviewPowerManagementLib (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   **This
  );

/*Routine Description:

  Initializes the processor power management library for Valleyview processors.
  This must be called  prior to any of the library functions being used.

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

/*Routine Description:

  Initializes the processor power management library for Denverton processors.
  This must be called  prior to any of the library functions being used.

  At this time, we don't properly publish the PPM processor support protocol,
  we simply return it if this library implements the protocol.

  If the processor is not supported, the input will not be modified.

Arguments:

  This          Pointer to the PPM support protocol instance

Returns:

  EFI_SUCCESS         Library was initialized successfully
  EFI_NOT_SUPPORTED   The library does not support the current processor

--*/

#endif
