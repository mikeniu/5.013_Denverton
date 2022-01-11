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

  Ppm.h

Abstract:

  IST/PPM reference code definitions.

  Acronyms:
    PPM   Platform Power Management
    GV    Geyserville
    TM    Thermal Monitor
    IST   Intel(R) Speedstep technology
    HT    Hyper-Threading Technology

--*/

#ifndef _PPM_H_
#define _PPM_H_

//
// Statements that include other files
//
#include "PiDxe.h"
#include "PpmCommon.h"
#include "Protocol/PpmProcessorSupport2.h"

//
// This is not a standard protocol, as it is never published.
// It is more of a dynamic internal library.
//

#define R_PCH_LPC_ACPI_BASE                       0x40 // ABASE, 16bit
#define B_PCH_LPC_ACPI_BASE_BAR                   0x0000FF80 // Base Address, 128 Bytes

#define PPM_IO_TRAP_LENGTH                0x10

#define PM_PM1_STS                        R_PCH_ACPI_PM1_STS

#define PM_PM1_CNT                        R_PCH_ACPI_PM1_CNT
  #define PM_BM_RELOAD                    B_PCH_ACPI_PM1_CNT_BM_RLD

#define PM_CST_LVL2                       0x14

#define SMI_PSTATE_LATENCY                110
#define NATIVE_PSTATE_LATENCY             10
#define PSTATE_BM_LATENCY                 10

#define FADT_C2_LATENCY                   1
#define FADT_C2_LATENCY_DISABLED          101
#define FADT_C3_LATENCY                   57
#define FADT_C3_LATENCY_DISABLED          1001

#define CSTATE_TRANSITION_C2              2
#define CSTATE_TRANSITION_C3              3
#define CSTATE_TRANSITION_C4              4


#define C3_RESIDENCY_TIMER_ROLLOVER_BIT   (1 << 24)

#define ICHX_C3_RES                       0x54

//
// MSR definitions
//

#define EFI_MSR_TIME_STAMP_COUNTER        0x10

#define MSR_PM_CFG_CTRL                   0xE2
  #define TM1_ENABLE                      (1 << 3)

#define MSR_PREFETCHERS_DISABLE           0x1a4
  #define B_L2_PREFETCH_DISABLE           (1 << 0)
  #define B_L1_PREFETCH_DISABLE           (1 << 2)

#ifndef MSR_MISC_PWR_MGMT
  #define MSR_MISC_PWR_MGMT               0x1aa
#endif
  #define B_LOCK_THERM_INT                (1 << 22)

#ifndef MSR_POWER_CTL
  #define MSR_POWER_CTL                   0x1fc
#endif
  #define B_PROCHOT_OUTPUT_MODE           (1 << 21)
  #define B_PROCHOT_RESPONSE              (1 << 22)
  #define B_VR_THERM_ALERT_DISABLE        (1 << 24)
  #define B_PROCHOT_FREQUENCY_RESPONSE    (1 << 26)

#define MSR_IA32_PERF_STS                 0x198
  #define IA32_PERF_STS_VIP               (1 << 17)

#define MSR_IA32_PERF_CTL                 0x199
  #define VID_BYTE_MASK                   0x3F
  #define RATIO_BYTE_MASK                 0x1F
  #define RATIO_N2                        (1 << 14)
  #define DYNAMIC_FSB_CONTROL_ENABLE      (1 << 15)
  #define PSTATE_CTL_MASK                 (DYNAMIC_FSB_CONTROL_ENABLE | RATIO_N2 | (RATIO_BYTE_MASK << 8) | VID_BYTE_MASK)

//#define EFI_MSR_IA32_MISC_ENABLE        0x1A0
  #define FERR_MUX_ENABLE                 (1 << 10)

//
// Save-state definitions
//
#define SYNC_SMI_BIT                      (1 << 0)

//
// ASL SSDT structure layout
//
#pragma pack (1)
typedef struct {
  UINT8     NameOp;           // First opcode is a NameOp.
  UINT32    NameString;       // 'TDSS'   ; Name of object.
  UINT8     PackageOp;        // db 12h   ; Sixth OpCode is PackageOp.
  UINT16    PackageLen;       // dw 0h    ; Seventh/Eighth OpCode is PackageLength.
  UINT8     PackageEntryNum;  // db 0Ch   ; Ninth OpCode is number of package entries.
  UINT8     StringPrefix1;    // 0Dh
  UINT64    Cpu0IstStr;       // 00h
  UINT8     StringNull1;      // 00h
  UINT8     DwordPrefix1a;    // 0Ch
  UINT32    Cpu0IstAddr;      // 00h
  UINT8     DwordPrefix1b;    // 0Ch
  UINT32    Cpu0IstLen;       // 00h

  UINT8     StringPrefix2;    // 0Dh
  UINT64    Cpu1IstStr;       // 00h
  UINT8     StringNull2;      // 00h
  UINT8     DwordPrefix2a;    // 0Ch
  UINT32    ApIstAddr;        // 00h
  UINT8     DwordPrefix2b;    // 0Ch
  UINT32    ApIstLen;         // 00h

  UINT8     StringPrefix3;    // 0Dh
  UINT64    Cpu0CstStr;       // 00h
  UINT8     StringNull3;      // 00h
  UINT8     DwordPrefix3a;    // 0Ch
  UINT32    Cpu0CstAddr;      // 00h
  UINT8     DwordPrefix3b;    // 0Ch
  UINT32    Cpu0CstLen;       // 00h

  UINT8     StringPrefix4;    // 0Dh
  UINT64    ApCstStr;         // 00h
  UINT8     StringNull4;      // 00h
  UINT8     DwordPrefix4a;    // 0Ch
  UINT32    ApCstAddr;        // 00h
  UINT8     DwordPrefix4b;    // 0Ch
  UINT32    ApCstLen;         // 00h
} SSDT_LAYOUT;
#pragma pack ()

//
// ASL CFGD structure layout
//
#pragma pack (1)
typedef struct {
  UINT8     NameOp;       //  Byte [0]=0x08:NameOp.
  UINT32    NameString;   //  Byte [4:1]=Name of object.
  UINT8     DWordPrefix;  //  Byte [5]=0x0C:DWord Prefix.
  UINT32    Value;        //  0   ; Value of named object.
} CFGD_LAYOUT;
#pragma pack()

//
// ASL PCTP structure layout
//
#pragma pack (1)
typedef struct {
  UINT8     RegDes;       //  Byte [0]=0x82:Register descriptor code
  UINT16    RegLen;       //  Byte [2:1]=0x0C:Register descriptor length
  UINT8     RegType;      //  Byte [3]=0x01:Register type (System IO)
  UINT8     RegWidth;     //  Byte [4]=0x10:Register width (16-bit)
  UINT8     RegBitOffst;  //  Byte [5]=0x00:Register bit offset (0)
  UINT8     RegAccSize;   //  Byte [6]=0x00:Register access size (0)
  UINT64    RegAddress;   //  Byte [14:7]=Register address
  UINT16    EndTag;       //  Byte [16:15]=End tag
} PCTP_LAYOUT;


#pragma pack()
//
// Function prototypes
//

VOID
InitializePpm (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Initialize the processor power management based on hardware capabilities
  and user configuration settings.

  This includes creating FVID table, updating ACPI tables,
  and updating processor and chipset hardware configuration.

  This should be called prior to any Px, Cx, Tx activity.

Arguments:

  This                Pointer to the protocol instance

Returns:

  None

--*/

VOID
PpmPreInit (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Initialize global variables used during PPM init
    miFSBFrequency - CDV iFSB Freqeuncy
    mCpuid01 - Set to CPUID01 values
    mFvidPointer - Set FVID states
    mAcpiSupport - Set ACPI support protocol instance

Arguments:

  This                Pointer to the protocol instance

Returns:

  None

--*/

VOID
SetUserConfigurationPpmFlagsMask (
  IN OUT UINT32 *PpmFlagsMask
  );
/*++

Routine Description:

  Set the PPM flags based on current user configuration obtained from PPM platform policy protocol

Arguments:

  PpmFlagsMask  Mask of feature options to be enabled as specified by the policy

Returns:

  None

--*/

VOID
AcpiPatchSpssNpss (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Patch the SMI _PSS (SPSS) package with performance values and the native _PSS (NPSS) package with the GV3 values

  Uses ratio/VID values from the FVID table to fix up the control values in the NPSS and SPSS.

  (1) Find NPSS or SPSS package:
    (1.1) Find the _PR_CPU0 scope.
    (1.2) Save a pointer to the package length.
    (1.3) Find the NPSS or SPSS AML name object.
  (2) Resize the NPSS and SPSS package.
  (3) Fix up the NPSS and SPSS package entries
    (3.1) Check Turbo mode support.
    (3.2) Check Dynamic FSB support.
  (4) Fix up the Processor block and \_PR_CPU0 Scope length.
  (5) Update SSDT Header with new length.
  (6) Renames PSS packages for ACPI utility evaluation

Arguments:

  This                Pointer to the protocol instance

Returns:

  None

--*/


VOID
PpmPostInit (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Completes processor power management initialization
    (1) Initializes the TSC update variables.
    (2) Initializes the GV state for processors.
    (3) Saves MSR state for S3
    (4) Adds a callback (SMI) in S3 resume script to restore the MSR

Arguments:
  
  This                Pointer to the protocol instance

Returns:

  None

--*/

EFI_STATUS
InitializePpmRuntime (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Initialize any runtime required data for runtime PPM services.  This should be called prior to invoking runtime functions.

Arguments:

  This                Pointer to the protocol instance

Returns:

  EFI_SUCCESS   PPM Runtime initialized.

--*/

EFI_STATUS
EFIAPI
PpmTransition (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN     FVID_TABLE                         *FvidPointer,
  IN     UINTN                              CpuNumber,
  IN     UINTN                              NextState
  );
/*++

Routine Description:

  Transition a single processor thread to a P state.

Arguments:

  This          Unused
  FvidPointer   Pointer to a table to be updated
  CpuNumber     The processor thread to perform the transition on
  NextState     The desired state (zero based) from the provided FVID table

Returns:

  EFI_SUCCESS             State transition success.
  EFI_INVALID_PARAMETER   Requested state is not supported.

--*/

EFI_STATUS
WaitForPpmTransitionComplete (
  IN  VOID        *Buffer
  );
/*++

Routine Description:

  This function will poll for the completion of a voltage transition for a core.
  This function must be MP safe.

Arguments:

  CpuNumber     The processor thread to poll

Returns:

  EFI_SUCCESS             State transition completed.

--*/

EFI_STATUS
PpmCStateTransition (
  UINT8         CState
  );
/*++

Routine Description:

  Perform a C state transition

Arguments:

  CState    C state desired (1-based)

Returns:

  EFI_SUCCESS             The function completed successfully
  EFI_INVALID_PARAMETER   Unsupported C state requested

--*/

VOID
CreateCustomFvidTable (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT FVID_TABLE                         *FvidPointer
  );
/*++

Routine Description:

  Create a custom FVID table based on setup options.
  Caller is responsible for providing a large enough table.

Arguments:

  This                Pointer to the protocol instance
  FvidPointer   Table to update, must be initialized.

Returns:

  None

--*/

VOID
ProcessorCmpCState (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Process a C state change request (SMI coordination method uses this).
    Determine best C state possible
    Save the PM timer and C3 timer entry times
    Transition to lowest GV state if soft C2 desired
    Update TSC MSR if C3 or C4

Arguments:

  This                Pointer to the protocol instance

Returns:

  None

--*/

EFI_STATUS
PpmUpdateTimeStampCounter (
  IN  VOID        *Buffer
  );
/*++

Routine Description:

  This function will update the Time Stamp Counter MSR with mPpmCstTscTicks, which
  contains the timer tick offset for timer ticks during C-states.
  This function must be MP safe.

Arguments:

  Buffer              Unused

Returns:

  EFI_SUCCESS             TSC updated

--*/

VOID
InitializePpmAcpiTable(
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Locate the PPM ACPI tables data file and read ACPI SSDT tables.
  Publish the appropriate SSDT based on current configuration and capabilities.

Arguments:

  This                Pointer to the protocol instance

Returns:

  None

--*/

#endif
