//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformStage2.h

Abstract:

  Platform Stage2 header file

--*/
#ifndef _EFI_PLATFORM_STAGE2_H_
#define _EFI_PLATFORM_STAGE2_H_

#include "Base.h"
#include "Uefi.h"
#include <Library/DebugLib.h>
#include <Uefi/UefiBaseType.h>
#include <Pi/PiFirmwareFile.h>
#include <Ppi/PlatformMemorySize.h>
#include <Ppi/PlatformMemoryRange.h>
#include <Platform.h>
//#include <Ppi/SaPolicyUpdate.h>
#include <Ppi/Smbus.h>
#include <Ppi/CpuIo.h>
#include <Ppi/Stall.h>
#include <Ppi/MemoryDiscovered.h>
#include <SetupVariable.h>
#include <Ppi/ReadOnlyVariable2.h>
//#include <Ppi/SaPlatformPolicy.h>
#include <Library/PchInfoLib.h>
#include <Guid/GlobalVariable.h>
#include <Guid/PlatformInfo.h>
#include <Library/PeiPlatformHookLib.h>
#include <Library/IoLib.h>
#include "ClockGeneratorLib.h"
//#include <Library/CpuPlatformLib.h>
#include <Library/PeiPlatformLib.h>
#include <Library/PciLib.h>
//#include <CpuDataStruct.h>
#include <Library/GpioLib.h>
#include <Library/PeiServicesLib.h>
#include <IndustryStandard/pci22.h>

#include "CpuRegs.h"
#include <Library/MmPciLib.h>
#include <Ppi/PchPolicy.h>

#define IA32_MISC_ENABLE_MSR  0x01A0
#define IA32_PMC0_MSR         0xC1
#define IA32_PERFEVTSEL0_MSR  0x186

#define EC_EXTRA_IO_PORT_1    0x06A0
#define BMC_KCS_IO_PORT_1     0x0CA0

//
//Define values for AG3E setup option
//
#define S0_AFTER_G3       0
#define S5_AFTER_G3       1
#define SS_AFTER_G3       2

typedef enum {
  IGD = 0,
  PEG,
  PCI,
  DISPLAY_DEVICE_MAX
} DISPLAY_DEVICE;

typedef enum {
  VBIOS_DEFAULT = 0,
  CRT,
  LFP,
  CRT_LFP,
  TV,
  LFPSDVO,
  EFP,
  TVSDVO,
  CRT_LFPSDVO,
  CRT_EFP,
  IGD_BOOT_TYPE_MAX
} IGD_BOOT_TYPE;

typedef enum {
  GMS_FIXED = 0,
  GMS_DVMT,
  GMS_FIXED_DVMT,
  GMS_MAX
} GRAPHICS_MEMORY_SELECTION;

typedef struct {
  EFI_PHYSICAL_ADDRESS    BaseAddress;
  UINT64                  Length;
} MEMORY_MAP;

extern EFI_GUID mSystemConfigurationGuid;

//
// Function Prototypes
//

EFI_STATUS
EFIAPI
PeiInitPlatformStage2 (
  IN EFI_PEI_FILE_HANDLE    FfsHeader,
  IN CONST EFI_PEI_SERVICES **PeiServices
  );

EFI_STATUS
EFIAPI
GetPlatformMemorySize (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN      PEI_PLATFORM_MEMORY_SIZE_PPI           *This,
  IN OUT  UINT64                                 *MemorySize
  );

EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  );

EFI_STATUS
EFIAPI
WarmResetCallback (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  );

EFI_STATUS
EFIAPI
PeimInitializeDimm (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *SmbusPpi
  );

EFI_STATUS
EFIAPI
ChoosePlatformMemoryRange (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN PEI_PLATFORM_MEMORY_RANGE_PPI        *This,
  IN OUT PEI_MEMORY_RANGE_OPTION_ROM      *OptionRomMask,
  IN OUT PEI_MEMORY_RANGE_SMRAM           *SmramMask,
  IN OUT PEI_MEMORY_RANGE_GRAPHICS_MEMORY *GraphicsMemoryMask,
  IN OUT PEI_MEMORY_RANGE_PCI_MEMORY      *PciMemoryMask
  );

EFI_STATUS
EFIAPI
PeimInitializeRecovery (
  IN CONST EFI_PEI_SERVICES               **PeiServices
  );

EFI_STATUS
ClearIchSmiAndWake (
  IN VOID
  );

EFI_STATUS
ConfigurePlatformClocks (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *SmbusPpi
  );

EFI_STATUS
EFIAPI
EndOfPeiCallback (
  IN EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
SetTheStateToGoAfterG3 ( IN UINT8 StateAfterG3 );

EFI_STATUS
CheckForceResetAfterAcRemoval (
  IN CONST EFI_PEI_SERVICES                **PeiServices,
  IN       EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices
  );

VOID
EcForceResetAfterAcRemoval (
  IN CONST EFI_PEI_SERVICES **PeiServices,
  EFI_PEI_CPU_IO_PPI    *CpuIo,
  EFI_PEI_STALL_PPI     *StallPpi,
  UINT8                  ForceResetAfterAcRemovalVar
  );

EFI_STATUS
PlatformPchInit (
  IN CONST EFI_PEI_SERVICES          **PeiServices
  );

EFI_STATUS
PlatformSaInit (
  IN CONST EFI_PEI_SERVICES          **PeiServices
  );

VOID
InitTxt (
  IN CONST EFI_PEI_SERVICES          **PeiServices
  );

EFI_STATUS
ConfigureIoExpander (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *SmbusPpi
  );

EFI_STATUS
InitIoExpander (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *MemoryInstalled
  );

#endif
