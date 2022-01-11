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

  PpmProcessorSupport2.h
    
Abstract:

  This protocol provides Platform Power Management support functionality and definitions.

  Acronyms:
    PPM   Platform Power Management
    GV    Geyserville
    TM    Thermal Monitor
    IST   Intel(R) Speedstep technology
    HT    Hyper-Threading Technology
    CMP   Core Multi-Processing

--*/

#ifndef _PPM_PROCESSOR_SUPPORT_PROTOCOL_2_H_
#define _PPM_PROCESSOR_SUPPORT_PROTOCOL_2_H_

#include "Protocol/PpmPlatformPolicy.h"
//
// PPM Processor Support Protocol GUID
//
#define PPM_PROCESSOR_SUPPORT_PROTOCOL_2_GUID \
{ 0x8a140b82, 0xf44c, 0x43ed, 0x86, 0xc1, 0x59, 0xaf, 0xd7, 0x65, 0xf, 0x42 }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gPpmProcessorSupportProtocol2Guid;

//
// Forward reference for pure ANSI compatability
//
typedef struct _PPM_PROCESSOR_SUPPORT_PROTOCOL_2 PPM_PROCESSOR_SUPPORT_PROTOCOL_2;

//
// Protocol member function prototypes
//

typedef
EFI_STATUS
(EFIAPI *SET_PPM_FLAGS) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Set the PPM flags supported by this processor.
  PpmFlags format is defined in the GlobalNvsArea protocol.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   PpmFlags updated with the features supported by the processor

--*/

typedef
EFI_STATUS
(EFIAPI *ENABLE_C_STATES) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This

  );
/*++

Routine Description:

  Enable C state support as specified by the input flags

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor C state support configured successfully.

--*/

typedef
EFI_STATUS
(EFIAPI *INIT_THERMAL) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy
  );
/*++

Routine Description:

  Initialize general thermal management for the processor

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor thermal support configured successfully.

--*/

typedef
EFI_STATUS
(EFIAPI *ENABLE_TM) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Enable TM1 for the processor

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor TM1 support configured successfully.

--*/

typedef
EFI_STATUS
(EFIAPI *ENABLE_TM2) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT FVID_TABLE                         *FvidPointer
  );
/*++

Routine Description:

  Enable TM2 for the processor

Arguments:
  
  This          Pointer to the protocol instance
  FvidPointer   Pointer to a table to be updated

Returns:

  EFI_SUCCESS   Processor TM2 support configured successfully.

--*/

typedef
EFI_STATUS
(EFIAPI *ENABLE_PROC_HOT) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Enable the bi-directional PROCHOT# signal

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor PROCHOT# support configured successfully.

--*/

typedef
EFI_STATUS
(EFIAPI *ENABLE_TSC) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT FVID_TABLE                         *FvidPointer,
  IN     UINT16                             FrequencyCorrectionFactor,
  IN OUT UINT8                              *PpmCstTmrFlags,
  IN OUT UINTN                              *PpmTscCorrFactor,
  IN OUT UINTN                              *PpmTscCorrFactorRem
  );
/*++

Routine Description:

  Initialize the TSC support.

Arguments:
  
  This                                 Pointer to the protocol instance
  FvidPointer                          Pointer to a table to be updated
  FrequencyCorrectionFactor            Applicable frequency correction factor, the processor core frequency.
  PpmCstTmrFlags                       Flag of update TSC MSR if C3 or C4  
  PpmTscCorrFactor                     TSC correction factor 
  PpmTscCorrFactorRem                  TSC correction factor remainder

Returns:

  EFI_SUCCESS   Processor TSC support configured successfully.

--*/


typedef
EFI_STATUS
(EFIAPI *S3_SAVE_MSR) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Save processor MSR runtime settings for S3.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor MSR setting is saved.

--*/

typedef
EFI_STATUS
(EFIAPI *S3_RESTORE_MSR) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Restore processor MSR runtime settings for S3.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor MSR setting is restored.

--*/

typedef
EFI_STATUS
(EFIAPI *ENABLE_C_STATE_IO_REDIRECTION) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Enable IO redirection C state control.
  This should only be enabled if the OS and processor driver support
  independent C state control for C2 and higher.

  This will cause the processor to capture LVL_X reads and perform the requested
  C state transition.

  This cannot be enabled when SMI based C state coordination is enabled, 
  so this function will disable SMI based C state coordination.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor IO redirection C state control enabled.

--*/

typedef
EFI_STATUS
(EFIAPI *DISABLE_C_STATE_IO_REDIRECTION) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Disable IO redirection C state control.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor IO redirection C state control disabled.

--*/

typedef
EFI_STATUS
(EFIAPI *ENABLE_C_STATE_SMI_COORDINATION) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Enable SMI based C state coordination.
  This should only be enabled if the OS and processor driver do not support
  independent C state control for C2 and higher.
  This will cause an SMI to be generated when both processors enter autohalt state.

  This cannot be enabled when IO redirection control is enabled, so this function
  will disable IO redirection control.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor SMI based C state coordination enabled.

--*/

typedef
EFI_STATUS
(EFIAPI *DISABLE_C_STATE_SMI_COORDINATION) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Disable SMI based C state coordination.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor SMI based C state coordination disabled.

--*/

typedef
EFI_STATUS
(EFIAPI *ENABLE_P_STATE_HARDWARE_COORDINATION) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Enable the Geyserville hardware coordination of P-states.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Hardware coordination enabled.

--*/

typedef
EFI_STATUS
(EFIAPI *DISABLE_P_STATE_HARDWARE_COORDINATION) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Disable the Geyserville hardware coordination of P-states.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Hardware coordination disabled.

--*/


typedef
EFI_STATUS
(EFIAPI *ENABLE_EMTTM) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT FVID_TABLE                         *FvidPointer
  );
/*++

Routine Description:

  This function enables EMTTM support in the processor.

Arguments:
  
  This                Pointer to the protocol instance
  FvidPointer         Pointer to a table to be updated

Returns:

  EFI_SUCCESS         EMTTM enabled in the processor.

--*/

typedef
EFI_STATUS
(EFIAPI *P_STATE_TRANSITION) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN     FVID_TABLE                         *FvidPointer,
  IN     UINTN                              CpuNumber,
  IN     UINTN                              NextState
  );
/*++

Routine Description:

  This function causes the specified processor to transition to a new P state.

  The function may be provided by either the protocol producer or consumer.
  If the protocol producer provides this function, it should take precedence over
  a consumer version.

  E.G. If the protocol producer does not provide the function, it will be NULL in
  the protocol structure.  The consumer would detect this and update the protocol
  with a function to provide this service.
  If the protocol producer does provide this service, the consumer should not 
  update the protocol.

Arguments:
  
  This          Pointer to the protocol instance
  FvidPointer   Pointer to a table containing P state information
  CpuNumber     The processor thread to perform the transition on
  NextState     The desired state (zero based) from the provided FVID table

Returns:

  EFI_SUCCESS     The specified processor was transitioned to the requested state.

--*/

typedef
VOID
(EFIAPI *INIT_EIST) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy,
  IN OUT FVID_TABLE                         *FvidPointer
  );
/*++

Routine Description:

  Initializes required structures for P-State table creation and enables GV3
  support in the processor.

Arguments:
  
  This          Pointer to the protocol instance
  FvidPointer   Pointer to a table to be updated

Returns:

  EFI_SUCCESS

--*/

typedef
EFI_STATUS
(EFIAPI *ENABLE_C6_RESIDENCY) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );
/*++

Routine Description:

  Enable C6 residency under Windows XP SP1 and SP2.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS

--*/

typedef
EFI_STATUS
(EFIAPI *POWER_SOURCE_CHANGE) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN BOOLEAN                                PowerSource
  );

/*++

Routine Description:

  Set processor P state when Eist is disabled.

Arguments:
  
  This          Pointer to the protocol instance

Returns:

  EFI_SUCCESS   Processor MSR setting is saved.

--*/
typedef
EFI_STATUS
(EFIAPI *PPM_SET_BOOT_P_STATE) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This
  );

typedef
EFI_STATUS
(EFIAPI *GET_CRASH_LOG_DUMP) (
  );

/*++

Routine Description:

  The function configures Ratio Limits & Core Counts to be used to limit frequency (MSR 0x1AD & MSR 0x1AE). 

Arguments:

  This          Pointer to the protocol instance
  PPMPolicy     Pointer to policy protocol instance

Returns:

  None

--*/
typedef
VOID
(EFIAPI *CONFIGURE_FREQ_LIMIT) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy
  );

/**
  The function configures RAPL limits for PL1 & PL2 (MSR 0x610).

  @param[in]      None
  @param[in out]  None

  @retval         EFI_SUCESS
**/
typedef
VOID
(EFIAPI *CONFIGURE_RAPL_LIMIT) (
  IN OUT PPM_PROCESSOR_SUPPORT_PROTOCOL_2   *This,
  IN OUT PPM_PLATFORM_POLICY_PROTOCOL       *PPMPolicy
  );

/*++

Routine Description:

  Perform any processor configuration required on power source change event.

Arguments:
  
  This          Pointer to the protocol instance
  PowerSource   Current power source, TRUE = AC power, FALSE = Battery power

Returns:

  EFI_SUCCESS

--*/


//
// Protocol data structure definition
//
typedef struct _PPM_PROCESSOR_SUPPORT_PROTOCOL_2 {
  UINT32                                        PpmFlags;
  UINT32                                        PpmFeatureFlags;

  SET_PPM_FLAGS                                 SetPpmFlags;
  ENABLE_C_STATES                               EnableCStates;
  INIT_EIST                                     InitEist;
  CONFIGURE_FREQ_LIMIT                          ConfigureFreqLimit;
  CONFIGURE_RAPL_LIMIT                          ConfigureRaplLimit;
  INIT_THERMAL                                  InitThermal;
  ENABLE_TM                                     EnableTm;
  ENABLE_PROC_HOT                               EnableProcHot;
  ENABLE_TSC                                    EnableTsc;
  ENABLE_EMTTM                                  EnableEmttm;                        OPTIONAL
  S3_SAVE_MSR                                   S3SaveMsr;
  S3_RESTORE_MSR                                S3RestoreMsr;
  PPM_SET_BOOT_P_STATE                          SetBootPState;
  GET_CRASH_LOG_DUMP                            GetCrashLogDump;
  ENABLE_C_STATE_IO_REDIRECTION                 EnableCStateIoRedirection;          OPTIONAL
  DISABLE_C_STATE_IO_REDIRECTION                DisableCStateIoRedirection;         OPTIONAL
  ENABLE_P_STATE_HARDWARE_COORDINATION          EnablePStateHardwareCoordination;   OPTIONAL
  DISABLE_P_STATE_HARDWARE_COORDINATION         DisablePStateHardwareCoordination;  OPTIONAL
  ENABLE_C6_RESIDENCY                           EnableC6Residency;                  OPTIONAL

  //
  // If this function is not-null, consumers should use the processor specific transition code
  // Otherwise, consumer code may update this with a P state transition function
  //
  P_STATE_TRANSITION                            PStateTransition;                   OPTIONAL

  POWER_SOURCE_CHANGE                           PowerSourceChange;                  OPTIONAL

} PPM_PROCESSOR_SUPPORT_PROTOCOL_2;

#endif
