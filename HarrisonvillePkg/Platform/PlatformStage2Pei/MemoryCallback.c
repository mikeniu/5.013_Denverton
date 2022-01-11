/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  MemoryCallback.c

Abstract:

  PEIM to provide the platform support functionality.

--*/
#include "PlatformStage2.h"
#include "Platform.h"
#include "PlatformDefinitions.h"
#include "PlatformFlashMap.h"
#include <Guid/FirmwareFileSystem2.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Library/HobLib.h>
#include <Ppi/Cache.h>
#include <Guid/SmramMemoryReserve.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
// Denverton Aptiov override start - EIP#220693
#include "SaRegs.h"
#include "SocRegs.h"
// Denverton Aptiov override end - EIP#220693
#include "CpuRegs.h"

#include "MmrcData.h"
extern EFI_GUID gEfiMemoryConfigDataGuid;

// Denverton Aptiov override start - EIP#220849
// Memory space reserved using RSVCHIPSET programming
/*
MEMORY_MAP                ReservedMap [] = {
      {_PCD_VALUE_PcdPciExpressBaseAddress, PCIEX_LENGTH},

};

MEMORY_MAP                MmioMap [] = {
      {EFI_APIC_IO_ADDRESS_PCH, APIC_IO_MMIO_SIZE},
      {HPET_ADDRESS_0,          HPET_MMIO_SIZE},
      {PCH_PCR_BASE_ADDRESS,    PCH_PCR_MMIO_SIZE},
      {PCH_PWRM_BASE_ADDRESS,   PCH_PWRM_MMIO_SIZE},
      {PCH_SPI_BASE_ADDRESS,   PCH_SPI_MMIO_SIZE},
//      {EFI_APIC_LOCAL_ADDRESS,  APIC_LOCAL_MMIO_SIZE}
};

MEMORY_MAP                SAMap [] = {
      {MCH_BASE_ADDRESS, MCH_MMIO_SIZE},
      {GDXC_BASE_ADDRESS, GDXC_MMIO_SIZE}
};
*/
// Denverton Aptiov override end - EIP#220849
static EFI_PEI_FIRMWARE_VOLUME_INFO_PPI mFvMainInfPpi = {
  EFI_FIRMWARE_FILE_SYSTEM2_GUID,
  (VOID *) (UINTN) FixedPcdGet32 (PcdFlashFvMainBase),
  (UINTN) FixedPcdGet32 (PcdFlashFvMainSize),
  NULL,
  NULL
};

static EFI_PEI_PPI_DESCRIPTOR mFvInfoPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiFirmwareVolumeInfoPpiGuid,
  &mFvMainInfPpi
};

static EFI_PEI_NOTIFY_DESCRIPTOR  mMtrrNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  EndOfPeiCallback
};

VOID
SetCacheMtrr (
  IN EFI_PEI_SERVICES    **PeiServices
  );

EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
/*++

Routine Description:

  Install Firmware Volume Hob's once there is main memory

Arguments:

  PeiServices       General purpose services available to every PEIM.
  NotifyDescriptor  Notify that this module published.
  Ppi               PPI that was installed.

Returns:

  EFI_SUCCESS     The function completed successfully.

--*/
{
  EFI_STATUS                            Status;
  EFI_BOOT_MODE                         BootMode;
  UINTN                                 Index;
  UINT8                                 PhysicalAddressBits;
  UINT32                                RegEax;

  Index = 0;

  Status = (**PeiServices).GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);

  SetCacheMtrr (PeiServices);

  AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
  if (RegEax >= 0x80000008) {
    AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);
    PhysicalAddressBits = (UINT8) RegEax;
  } else {
    PhysicalAddressBits = 36;
  }

  //
  // Create a CPU hand-off information
  //
  BuildCpuHob (PhysicalAddressBits, 16);
  ASSERT_EFI_ERROR (Status);

  //
  // If S3 resume, then we are done
  //
  if (BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }
  //
  // Build HOB for DXE
  //
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    //
    // Prepare the recovery service
    //
    // DNV_TODO: recovery
    // Status = PeimInitializeRecovery (PeiServices);
    //ASSERT_EFI_ERROR ( Status);
	// Denverton AptioV override Start - EIP#252720
    // ASSERT(FALSE);
	// Denverton AptioV override End - EIP#252720
  } else {

    Status = (**PeiServices).InstallPpi (PeiServices, &mFvInfoPpiList);
    ASSERT_EFI_ERROR ( Status);
  }
  // Denverton Aptiov override start - EIP#220849
  // The following resource allocation is done using RSVChipset programming in Sb.sdl
  /*
  for (Index = 0; Index < sizeof (SAMap) / (sizeof (MEMORY_MAP)); Index++) {
    BuildResourceDescriptorHob (
             EFI_RESOURCE_MEMORY_MAPPED_IO,
             (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
              EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
              EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
             SAMap[Index].BaseAddress,
             SAMap[Index].Length
             );
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Build Reserved Resource which is used to build E820 Table in LegacyBios.
  //
  for (Index = 0; Index < sizeof (ReservedMap) / (sizeof (MEMORY_MAP)); Index++){
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_RESERVED,
      (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_TESTED |
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
      ReservedMap[Index].BaseAddress,
      ReservedMap[Index].Length
      );
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Build Memory Mapped IO Resource which is used to build E820 Table in LegacyBios.
  //
  for (Index = 0; Index < sizeof (MmioMap) / (sizeof (MEMORY_MAP)); Index++){
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_MAPPED_IO,
      (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
      MmioMap[Index].BaseAddress,
      MmioMap[Index].Length
      );
    ASSERT_EFI_ERROR (Status);
  }

  BuildResourceDescriptorHob (
             EFI_RESOURCE_FIRMWARE_DEVICE,
             (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
              EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
              EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
              FLASH_BASE,
              FLASH_SIZE
             );
  ASSERT_EFI_ERROR (Status);
  */
  // Denverton Aptiov override end - EIP#220849  
  return Status;
}

VOID
SetDefaultMemoryCacheAsWB (
  VOID
  )
{
   UINT64 TempQword;

   TempQword = AsmReadMsr64 (EFI_CACHE_IA32_MTRR_DEF_TYPE);
   //
   // Write Back as default memory atrribute
   //
   TempQword = TempQword & ~0xff | EFI_CACHE_WRITEBACK;

   AsmWriteMsr64 (EFI_CACHE_IA32_MTRR_DEF_TYPE, TempQword);
   return;

}


VOID
SetCacheMtrr (
  IN EFI_PEI_SERVICES    **PeiServices
  )
/*++

Routine Description:

  Set Cache Mtrr.
Arguments:

  PeiServices      -  General purpose services available to every PEIM.

Returns:

  None

--*/
{
  EFI_STATUS                  Status;
  EFI_PEI_HOB_POINTERS        Hob;
  PEI_CACHE_PPI               *CachePpi;
  UINT64                      MemoryBase;
  UINT64                      MemoryLength;
  UINT64                      LowMemoryLength;
  UINT64                      HighMemoryLength;
  EFI_BOOT_MODE               BootMode;

  //
  // Set low to 1 MB. Since 1MB cacheability will always be set
  // until override by CSM.
  // Initialize high memory to 0.
  //
  LowMemoryLength   = 0x100000;
  HighMemoryLength  = 0;

  Status            = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) {
        if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000000ULL) {
          HighMemoryLength += Hob.ResourceDescriptor->ResourceLength;
        } else if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000) {
          LowMemoryLength += Hob.ResourceDescriptor->ResourceLength;
        }
      }
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  Status = (**PeiServices).GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // W/A: SystemMemoryLength in AcpiS3Save and MemoryInit is not consistent, AcpiS3Save covers
  // memory > 4GB, MemoryInit does not cover memory > 4GB.
  // when SystemMemoryLength assigned in AcpiS3Save driver covers memory size > 4GB,
  // MemoryInit will build a hob for 0 ~ above 4GB, so substract > 4GB memory size.
  //
  if (BootMode == BOOT_ON_S3_RESUME) {
    LowMemoryLength -= HighMemoryLength;
  }

  DEBUG(( DEBUG_INFO, "Memory Length (Below 4GB) = %lx.\n", LowMemoryLength));
  DEBUG(( DEBUG_INFO, "Memory Length (Above 4GB) = %lx.\n", HighMemoryLength));

  //
  // Load Cache PPI
  //
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gPeiCachePpiGuid,  // GUID
                            0,                  // Instance
                            NULL,               // PEI_PPI_DESCRIPTOR
                            &CachePpi           // PPI
                            );

  if (!EFI_ERROR (Status)) {
    Status = (**PeiServices).NotifyPpi (PeiServices, &mMtrrNotifyList);
    ASSERT_EFI_ERROR (Status);

    //
    // It will reset all MTRR setting.
    //
    CachePpi->ResetCache (
                PeiServices,
                CachePpi
                );
    //
    // Cache the Flash area as WP to boost performance
    //
    CachePpi->SetCache (
                PeiServices,
                CachePpi,
                FLASH_BASE,
                FLASH_SIZE,
                EFI_CACHE_WRITEPROTECTED
                );

    //
    // Assume size of main memory is multiple of 256MB
    //
    MemoryLength = (LowMemoryLength + 0xFFFFFFF) & 0xF0000000;
    MemoryBase = 0;
    CachePpi->SetCache (
                PeiServices,
                CachePpi,
                MemoryBase,
                MemoryLength,
                EFI_CACHE_WRITEBACK
                );

    MemoryBase = LowMemoryLength;
    MemoryLength -= LowMemoryLength;
    CachePpi->SetCache (
                PeiServices,
                CachePpi,
                MemoryBase,
                MemoryLength,
                EFI_CACHE_UNCACHEABLE
                );
  //
  // Disable NEM, Update MTRR setting from MTRR buffer
  //
  //DNV_TODO: investigate ActivateCache and enable the equivalent
//  CachePpi->ActivateCache (PeiServices, CachePpi);
  }

  return ;
}

EFI_STATUS
EFIAPI
EndOfPeiCallback (
  IN EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                            Status;
  PEI_CACHE_PPI                         *CachePpi;
  EFI_BOOT_MODE                         BootMode;

  VOID              *HobList;
  MRC_PARAMETERS    *MrcParameters;
  EFI_HOB_GUID_TYPE *GuidHob;
  VOID              *HobData;

  HobList = GetHobList ();
  ASSERT (HobList != NULL);

  GuidHob = GetNextGuidHob (&gEfiMemoryConfigDataGuid, HobList);
  if (GuidHob != NULL) {
    HobData = GET_GUID_HOB_DATA (GuidHob);
    MrcParameters = (MRC_PARAMETERS *) HobData;
  }
  else {
	ASSERT(FALSE);
    return EFI_NOT_FOUND;
  }

  Status = (**PeiServices).GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }

  //
  // Load Cache PPI
  //
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gPeiCachePpiGuid,  // GUID
                            0,                  // Instance
                            NULL,               // PEI_PPI_DESCRIPTOR
                            &CachePpi           // PPI
                            );

  if (!EFI_ERROR (Status)) {
    //
    // Clear the CAR Settings
    //
    Status = CachePpi->ResetCache (
                         PeiServices,
                         CachePpi
                         );
    ASSERT_EFI_ERROR (Status);

    /*
     * This function will provide following configuration:
     * 1) Default MTRR type is UC.
	 * 2) MTRR0: Covers range 0x0 - TOUUD. Overlaps with the Fixed MTRR regions and Variable MTRR's (UC) for RSVD and TraceHub (NPK) frames. Fixed MTRR regions override variable MTRR regions where they overlap. Variable MTRR type of UC will override a variable MTRR type of WB where they overlap. SMRR type (WB) will always override variable MTRR regions where they overlap.
     * 3) Next available MTRRs to set reserved memory regions to UC and provide a safe UC region to expand regions needed to be UC.  MRC should ensure this is always a power of two!
	 * 4) Remain MTRR's: For NPK - allocated directly by PCH RC PchTraceHubSetMemoryAsUncacheable().  Could potentially be collected under MTRR1 (3) but we have enough MTRRs available without the additional code impacts.
	*/

    CachePpi->SetCache (
    				PeiServices,
    				CachePpi,
    				0x0,
    				MrcParameters->SaveRestore.Touud,
    				EFI_CACHE_WRITEBACK
    				);
    ASSERT_EFI_ERROR (Status);

	CachePpi->SetCache (
				PeiServices,
				CachePpi,
				MrcParameters->SaveRestore.RsvdMem,
				MEM_4GB - MrcParameters->SaveRestore.RsvdMem,
				EFI_CACHE_UNCACHEABLE
				);
    ASSERT_EFI_ERROR (Status);

    //
    // Set fixed cache for memory range below 1MB
    //

	Status = CachePpi->SetCache (
				 PeiServices,
				 CachePpi,
				 0x0,
				 0xA0000,
				 EFI_CACHE_WRITEBACK
				 );
	ASSERT_EFI_ERROR (Status);

    Status = CachePpi->SetCache (
                 PeiServices,
                 CachePpi,
                 0xA0000,
                 0x20000,
                 EFI_CACHE_UNCACHEABLE
                 );
    ASSERT_EFI_ERROR ( Status);

	Status = CachePpi->SetCache (
				PeiServices,
				CachePpi,
				0xC0000,
				0x40000,
				EFI_CACHE_WRITEPROTECTED
				);
	ASSERT_EFI_ERROR ( Status);
  }

  return Status;
}
