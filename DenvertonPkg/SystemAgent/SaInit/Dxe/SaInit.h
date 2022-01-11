/** @file
  Header file for SA Initialization Driver.

@copyright
  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _SA_INITIALIZATION_DRIVER_H_
#define _SA_INITIALIZATION_DRIVER_H_

///
/// External include files do NOT need to be explicitly specified in real EDKII
/// environment
///
#ifdef ECP_FLAG
#include <EdkIIGlueDxe.h>
#include <EfiScriptLib.h>
#else
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/PciLib.h>
#include <Library/PchPcrLib.h>
#include <Protocol/AcpiTable.h>
#include <Guid/EventGroup.h>
#ifndef FSP_FLAG
#include <Uefi.h>
#include <IndustryStandard/Acpi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Framework/FrameworkInternalFormRepresentation.h>
#include <Guid/DataHubRecords.h>
#include <Protocol/FirmwareVolume.h>
#endif  //FSP_FLAG
#endif
#ifndef FSP_FLAG
#include "VTd.h"
#include "LegacyRegion.h"
#endif  //FSP_FLAG
#include <Library/HobLib.h>
#include <SaDataHob.h>
#include <CpuRegs.h>
#include <SaAccess.h>
#include <Library/MmPciLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchP2sbLib.h>
#include "SimicsDebug.h"

///
/// Data definitions
///
#define CRID_DATA 0x69
#define CRID_LOCK 0x17

typedef struct {
  UINT64  BaseAddr;
  UINT32  Offset;
  UINT32  AndMask;
  UINT32  OrMask;
} BOOT_SCRIPT_REGISTER_SETTING;

typedef struct {
  UINT64                Address;
  EFI_BOOT_SCRIPT_WIDTH Width;
  UINT32                Value;
} BOOT_SCRIPT_PCI_REGISTER_SAVE;

typedef struct {
  UINT8 DeviceNumber;
  UINT8 FunctionNumber;
  UINT8 SvidRegOffset;
} SA_SVID_SID_INIT_ENTRY;

typedef struct {
  UINT64  Base;
  UINT64  Length;
  BOOLEAN Enabled;
} SA_MEM_DEFINITION;

typedef struct {
  UINT64  Base;
  UINT64  Length;
  UINT8   IMRNumber;
} SA_IMR_DEFINITION;

typedef struct {
  SA_MEM_DEFINITION  Prmrr;
  SA_MEM_DEFINITION  Bgsm;
  SA_MEM_DEFINITION  Mot;
  SA_MEM_DEFINITION  Tseg;
  SA_IMR_DEFINITION  GenericIMR[N_SA_MCHBAR_BIMR_NUM_OF_IMRS];
  UINT8              GenericIMRCount;
} SA_IMR_REGIONS;

#define LENGTH_FROM_MASK(Mask, Start, Shift)  ((Mask) + (1 << (Shift)) - (Start))
#define DO_NUMBERS_OVERLAP(Start1, End1, Start2, End2)  ((End1 >= Start2) && (Start1 <= End2))

///
/// Function Prototype
///
VOID
EFIAPI
SaPciEnumCompleteCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
/**
  This function gets registered as a callback to perform SA 
  initialization before EndOfDxe 

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.

  @retval EFI_SUCCESS   - Always.

  **/
;

EFI_STATUS
EFIAPI
SaEndOfDxeCallback (
#ifndef FSP
  IN EFI_EVENT    Event,
  IN VOID         *Context
#else
  IN EFI_PEI_SERVICES                  **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR         *NotifyDescriptor,
  IN VOID                              *Ppi
#endif
  )
/**
  This function gets registered as a callback to perform SA configuration security lock

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.

  @retval EFI_SUCCESS      - When event completes successfully. 

  **/
;

VOID
SaGetImrRegions (
  IN OUT SA_IMR_REGIONS    *IMRRegions,
  IN     UINTN              MchBar
  )
/**
  This function sets up various IMR regions in SA_IMR_REGIONS 
  structure. 

  @param[in/out] IMRRegions  - Pointer to SA_IMR_REGIONS struct.
  @param[in]     MchBar      - Base of MCH.

  **/
;

EFI_STATUS
SaCheckImrForOverlap (
  IN OUT SA_IMR_REGIONS    *IMRRegions,
  IN     UINT8              IMRNumber,
  IN     UINT32             IMRBase,
  IN     UINT32             IMRLength
  )
/**
  This function tests IMRs for overlap with each other and other 
  reserved memory regions. 

  @param[in/out] IMRRegions  - Pointer to SA_IMR_REGIONS struct. 
  @param[in]     IMRNumber   - IMR number being tested. 
  @param[in]     IMRBase     - Base of IMR to test. 
  @param[in]     IMRLength   - Length of IMR to test. 
   
  @retval EFI_SUCCESS      - When no overlaps are detected. 
   EFI_INVALID_PARAMETER   - When a parameter is invalid.
   EFI_SECURITY_VIOLATION  - When an overlap is detected.
   

  **/
;


VOID
SaMiscellaneousRegisters (
#ifndef FSP_FLAG
  IN EFI_HANDLE      ImageHandle,
#endif
  IN SA_DATA_HOB     *SaDataHob
  )
/**
  Configure SA Registers: A-Unit, B-Unit and T-Unit.

  @param[in] DxePlatformSaPolicy            SA DxePlatformPolicy protocol

**/
;

#endif
