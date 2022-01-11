/** @file 
  MemoryInit.c
  Framework PEIM to initialize memory on a Memory Controller.
 
  Copyright (c) 2005-2017 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "MemoryInit.h"
#include "DetectDimms.h"
#include "Ppi\PchPolicy.h"

#ifdef APTIO_BUILD
#define VFRCOMPILE
#include <Include/Setup.h>
#else
#if SMM_SUPPORT
#include <Guid\SmramMemoryReserve.h>
#endif
#endif
#include <Guid\AcpiS3Context.h>
#if defined(IQAT_RESERVED_MEMORY_REGION_SIZE) && IQAT_RESERVED_MEMORY_REGION_SIZE
#include <Guid\IqatIpReservedBase.h>
#include <Ppi/IqatIpPolicyPpi.h>
#endif
#ifdef FSP_FLAG
#include <FspEas.h>
#include <Guid\GuidHobFspEas.h>
#include <Library/FspCommonLib.h>
#include <IndustryStandard/SmBios.h>
#include <FspmUpd.h>
extern EFI_GUID gFspSmbiosMemoryInfoHobGuid;
extern EFI_GUID gFspFastBootCheckerHobGuid;
#endif //FSP_FLAG
#include "NvdimmHooks.h"

// Denverton Aptiov override Start - EIP#373905
#include <Ppi/Capsule.h>
// Denverton Aptiov override End - EIP#373905

// Denverton AptioV Override Start
#include "Ppi/NbPpi.h"
// Denverton AptioV Override End

#define RESERVED_ACPI_S3_VARIABLE_SET_OFFSET (EFI_PAGE_SIZE - sizeof (ACPI_VARIABLE_SET))
#define RESERVED_CPU_S3_SAVE_OFFSET (RESERVED_ACPI_S3_VARIABLE_SET_OFFSET - sizeof (SMM_S3_RESUME_STATE))

//
// mMmioSize is 1MB Aligned
//
UINT16  mMmioSize[3] = {
  0x800,  // By default
  0x400,  // 1024MB
  0xC00,  // 3072MB
};

//
// Defined for MRC Reset Test loop forever.
//
#define MRC_RESET_TEST_HOOKED 0

static EFI_PEI_PPI_DESCRIPTOR mBeforeMemoryDiscoveredPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBeforeMemoryDiscoveredPpiGuid,
  NULL
};

// Denverton AptioV Override Start
static EFI_PEI_PPI_DESCRIPTOR mAmiPeiBeforeMrcDesc[] = {
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), \
      &gAmiPeiBeforeMrcGuid, \
      NULL }
};

static EFI_PEI_PPI_DESCRIPTOR mAmiPeiCompelteMrcDesc[] = {
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), \
      &gAmiPeiAfterMrcGuid, \
      NULL }
};

static EFI_PEI_PPI_DESCRIPTOR mAmiPeiEndOfMrcDesc[] = {
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), \
      &gAmiPeiEndOfMemDetectGuid, \
      NULL }
};
// Denverton AptioV Override End 

/**
  This function is the Framework PEIM entry point to initialize memory on a Memory Controller.

  @param[in]       FileHandle      File Handler.
  @param[in]       PeiServices     PEI Services Table.
  @retval          EFI_SUCCESS     If memory training was successful
**/
EFI_STATUS
PeimMemoryInit (
  IN          EFI_PEI_FILE_HANDLE  FileHandle,
  IN  CONST   EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS          Status;
  EFI_BOOT_MODE       BootMode;
  MMRC_DATA           MrcData;
  UINT8               Cmos8b;
  MRC_FLOW_FLAGS      MrcFlowFlags;
  REGISTER_ACCESS     Register;
#if BDAT
  BDAT_MEMORY_DATA_2_STRUCTURE    BdatMemoryData2Structure;
#endif

#ifdef FSP_FLAG
  SetFspMeasurePoint (FSP_PERF_ID_MRC_INIT_ENTRY);
#endif //FSP_FLAG
  _asm out 98h, al;
  // Denverton AptioV Override Start
  // Install the Before Mrc Notify PPI
  Status = (*PeiServices)->InstallPpi(PeiServices, &mAmiPeiBeforeMrcDesc[0]);
  ASSERT_EFI_ERROR (Status);
  // Denverton AptioV Override End
  //
  // Disable PCH Watchdog timer at SB_RCBA+0x3410
  //
  Mmio32Write (PBASE + 0x08, Mmio32Read (PBASE + 0x08) | 0x10);

  //
  //Install Ppi to trigger Events that registered for Before Memory Init.
  //
  Status = (*PeiServices)->InstallPpi (PeiServices, &mBeforeMemoryDiscoveredPpi);

  //
  // Determine boot mode
  //
  Status = (*PeiServices)->GetBootMode (
             PeiServices,
             &BootMode
           );

  ASSERT_EFI_ERROR (Status);
  //
  // Initialize params before configure memory
  //
  MrcInitializeParameters (&MrcData);
  //
  // Execute DetectDimms
  //
  if (BootMode == BOOT_ON_S3_RESUME) {
    MrcData.MrcParameters.BootMode = S3;
  }
  //
  // Restore MRC Parameters from NVRAM.
  //
  Status = MrcParamsRestore (
             (EFI_PEI_SERVICES **) PeiServices,
             &MrcData
           );
#if ME_SUPPORT_FLAG
  //
  // Read and return the amount of ME UMA requested by ME ROM from the HECI Device.
  //
  OemGetSetMeUma ((EFI_PEI_SERVICES **) PeiServices, &MrcData);
#endif        

  Status = ConfigureMemory (&MrcData);
  if (Status != EFI_SUCCESS) {
    MrcData.ErrorCode = MrcErrConfigureMemory;
    MrcDeadLoop();
  }
  
#ifdef FSP_FLAG
  BuildFspSmbiosMemoryInfoHob (&MrcData);
#endif

   // Denverton AptioV Override Start
   // Install the Complete of Mrc Notify PPI
   Status = (*PeiServices)->InstallPpi(PeiServices, &mAmiPeiCompelteMrcDesc[0]);
   ASSERT_EFI_ERROR (Status);
   // Denverton AptioV Override end
  //
  // Install memory
  //
  MspDebugPrint ((MSP_DBG_MIN, "Install EFI Memory.\n"));
  if (BootMode == BOOT_ON_S3_RESUME) {
    MspDebugPrint ((MSP_DBG_MIN, "Following BOOT_ON_S3_RESUME boot path.\n"));
    Status = InstallS3Memory ((EFI_PEI_SERVICES **) PeiServices, &MrcData);
    ASSERT_EFI_ERROR (Status);
  }
  Status = InstallEfiMemory ((EFI_PEI_SERVICES **) PeiServices, BootMode, &MrcData);
#if BDAT
  //
  // BDAT 4.0
  //
  FillBdatStructure (&MrcData, &BdatMemoryData2Structure);
  //
  //  Save the BDAT structure to a Hob.
  //
  MrcBdatSaveHob ((EFI_PEI_SERVICES **) PeiServices, &MrcData, &BdatMemoryData2Structure);
#endif // BDAT
  //
  //  Save the Mrc Version to a Hob.
  //
  MrcVersionSaveHob ((EFI_PEI_SERVICES **) PeiServices, MRC_VERSION);
  if (BootMode != BOOT_ON_S3_RESUME) {
    MrcParamsSave ((EFI_PEI_SERVICES **) PeiServices, &MrcData);
    MspDebugPrint ((MSP_DBG_MIN, "Save MRC params.\n"));
  }
  if (Status != EFI_SUCCESS) {
    MspDebugPrint ((MSP_DBG_MIN, "MemoryInit Not Installed.\n"));
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "MemoryInit Installed.\n"));
  }
#if ME_SUPPORT_FLAG
  //
  // Send indication to ME FW to start using allocated ME UMA Space. 
  //
  OemSendDramInitDoneCommand ((EFI_PEI_SERVICES **) PeiServices, &Status);
#endif

#if IE_SUPPORT
  //
  // Send indication to IE FW
  //
  OemSendIEDramInitDoneCommand ((EFI_PEI_SERVICES **) PeiServices, &Status);
#endif //IE_SUPPORT


  //
  // SSKPD0_DUNIT_REG[1:0]= 1 MRC started; 2 MRC finished
  //
  Register.Offset   = SSKPD0_DUNIT_COMMON_REG;
  Register.Mask     = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  MrcFlowFlags.Data = (UINT32) MemRegRead (DUNIT_COMMON, 0, 0, Register);
  MrcFlowFlags.Bits.MrcFlowStatus = MmrcFinished;
  MemRegWrite (DUNIT_COMMON, 0, 0, Register, MrcFlowFlags.Data, 0xF);

  if (MrcData.ResetLoop == 1) {
    Cmos8b = ReadCmosMrc (RESET_LOOP_ADDR);
    if (Cmos8b < MrcData.ResetLoopCycles) {
      Cmos8b++;
    }
    WriteCmosMrc (RESET_LOOP_ADDR, Cmos8b);
    if (MrcData.ResetLoopCycles == MRC_RESET_TEST_HOOKED) {
      MspDebugPrint ((MSP_DBG_MAX, "PeiMemoryInit::TimesReset Loop Forever...\n"));
    } else {
      MspDebugPrint ((MSP_DBG_MAX, "PeiMemoryInit::TimesReset = %d\n", Cmos8b));
    }
    //
    // Write 0x0E to IO 0xCF9 to trigger power cycling reset
    //
    if ((Cmos8b != MrcData.ResetLoopCycles) || (MrcData.ResetLoopCycles == MRC_RESET_TEST_HOOKED)) {
      IoOut8 (0xCF9, 0x0);
      IoOut8 (0xCF9, 0xE);
      _asm hlt
    }
  } else {
    WriteCmosMrc (RESET_LOOP_ADDR, 0);
  }

  _asm out 0edh, al;
   // Denverton AptioV Override Start
   // Install the End of Mrc Notify PPI
   Status = (*PeiServices)->InstallPpi(PeiServices, &mAmiPeiEndOfMrcDesc[0]);
   ASSERT_EFI_ERROR (Status);
   // Denverton AptioV Override End

#ifdef FSP_FLAG
  SetFspMeasurePoint (FSP_PERF_ID_MRC_INIT_EXIT);
#endif //FSP_FLAG
  return EFI_SUCCESS;
}

/**
  This function installs the EFI Memory

  @param[in]       PeiServices     PEI Services Table.
  @param[in]       BootMode        Indicates the boot mode selected.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          EFI_SUCCESS     When no errors when installing memory
**/
EFI_STATUS
InstallEfiMemory (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN        EFI_BOOT_MODE         BootMode,
  IN  OUT   MMRC_DATA             *MrcData
  )
{
  EFI_STATUS                      Status;

  UINT8                           RemapEnable;
  EFI_PHYSICAL_ADDRESS            UpperTotalMemory;
  UINT32                          MmioAllocation;
  EFI_PHYSICAL_ADDRESS            HostIOBoundaryHi;
  EFI_PHYSICAL_ADDRESS            HostIOBoundary;
  EFI_PHYSICAL_ADDRESS            TSegBase;
  UINT64                          TSegSize;
  UINT64			  RsvdRegionLength 	= 0;
  UINT8			          NumberOfOnes		= 0;

  EFI_PHYSICAL_ADDRESS            dPeiMemBase;
  UINT64                          PeiMemSize;
#if ME_SUPPORT_FLAG
  EFI_PHYSICAL_ADDRESS            MeUmaAddr;
#endif
#if defined(IQAT_RESERVED_MEMORY_REGION_SIZE) && IQAT_RESERVED_MEMORY_REGION_SIZE
  EFI_PHYSICAL_ADDRESS            IqatReservedBase;
  IQAT_IP_GLOBAL_POLICY_PPI       *IqatIpGlobalPolicy;
#endif
  EFI_PHYSICAL_ADDRESS            NonVolatileHighMemoryAddr = 0;
  EFI_PHYSICAL_ADDRESS            NonVolatileHighMemorySize = 0;
  EFI_PHYSICAL_ADDRESS            Sub4GMemAddr;
  EFI_PHYSICAL_ADDRESS            Above4GMemAddr;
  EFI_PHYSICAL_ADDRESS            LowSystemMemoryBase;
  UINT64                          LowSystemMemorySize;
  EFI_PHYSICAL_ADDRESS            HighSystemMemoryBase;
  UINT64                          HighSystemMemorySize;

  TOUUD_HI_BUNITMEM_STRUCT        TouudHi;
  TOUUD_LO_BUNITMEM_STRUCT        TouudLo; 
  TOLUD_BUNITMEM_STRUCT           Tolud;
  TSEGMB_BUNITMEM_STRUCT          TsegMB;
  REGISTER_ACCESS                 Register;


  EFI_PEI_HOB_POINTERS            Hob;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK  *SmramHobDescriptorBlock;
  EFI_SMRAM_DESCRIPTOR            DescriptorAcpiVariable;
  // Denverton Aptiov override Start - EIP#373905
  PEI_CAPSULE_PPI                 *Capsule;
  VOID                            *CapsuleBuffer;
  UINTN                           CapsuleBufferLength;
  EFI_STATUS                      CapsulePpiStatus;
  EFI_PHYSICAL_ADDRESS            CapsuleReservedBase;
  // Denverton Aptiov override End - EIP#373905

#ifdef FSP_FLAG
  UINT32  BootLoaderTolumSize = ((FSPM_UPD *)GetFspMemoryInitUpdDataPointer ())->FspmArchUpd.BootLoaderTolumSize;
#endif

  // Denverton Aptiov override Start - EIP#373905
  Capsule             = NULL;
  CapsulePpiStatus    = EFI_NOT_FOUND; // Set Fail status as default.
  CapsuleBuffer       = NULL;
  CapsuleBufferLength = CAPSULE_RESERVED_MEMORY_REGION_SIZE;
  // Denverton Aptiov override End - EIP#373905
  
  Register.Offset   = 0xFFFFFFFF;
  Register.Mask     = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  Status =  MMRC_SUCCESS;
  RemapEnable = 0;
  UpperTotalMemory = 0;
  MmioAllocation = MrcData->OemMrcData.MmioSize;
  dPeiMemBase = 0x200000;
  PeiMemSize = 0;
  Sub4GMemAddr = 0;
  Above4GMemAddr = 0;
  LowSystemMemoryBase = 0;
  LowSystemMemorySize = 0;
  HighSystemMemoryBase = 0;
  HighSystemMemorySize = 0;

  UpperTotalMemory  = (EFI_PHYSICAL_ADDRESS) LShiftU64 (MrcData->MrcParameters.SystemMemorySize, 20);
  Register.Offset   = TOUUD_LO_BUNITMEM_REG;
  Register.Mask     = 0xFFFFFFFF;
  TouudLo.Data      = MemRegRead (BUNITMEM, 0, 0, Register);
  Register.Offset   = TOUUD_HI_BUNITMEM_REG;
  TouudHi.Data      = MemRegRead (BUNITMEM, 0, 0, Register);
  HostIOBoundaryHi  = (EFI_PHYSICAL_ADDRESS) TouudLo.Data;
  HostIOBoundaryHi  += (EFI_PHYSICAL_ADDRESS) LShiftU64 (TouudHi.Data, 32);
  MrcData->MrcParameters.SaveRestore.Touud = HostIOBoundaryHi;

  Register.Offset   = TOLUD_BUNITMEM_REG; 
  Tolud.Data        = MemRegRead (BUNITMEM, 0, 0, Register);
  MrcData->MrcParameters.SaveRestore.Tolud = HostIOBoundary  = (EFI_PHYSICAL_ADDRESS) Tolud.Data;

  Register.Offset   = TSEGMB_BUNITMEM_REG;
  TsegMB.Data       = MemRegRead (BUNITMEM, 0, 0, Register); 
  TSegBase          = (EFI_PHYSICAL_ADDRESS) TsegMB.Data;
  TSegSize          = HostIOBoundary - TSegBase;

  MspDebugPrint ((MSP_DBG_MIN, "MmioAllocation:   0x%08X\n", MmioAllocation));
  MspDebugPrint ((MSP_DBG_MIN, "UpperTotalMemory: 0x%08X",   UpperTotalMemory >> 32));
  MspDebugPrint ((MSP_DBG_MIN, "%08X\n",                     UpperTotalMemory));
  MspDebugPrint ((MSP_DBG_MIN, "HostIOBoundary:   0x%08X\n", HostIOBoundary));
  MspDebugPrint ((MSP_DBG_MIN, "HostIOBoundaryHi: 0x%08X",   HostIOBoundaryHi >> 32));
  MspDebugPrint ((MSP_DBG_MIN, "%08X\n", HostIOBoundaryHi));
  MspDebugPrint ((MSP_DBG_MIN, "TSegBase:         0x%08X\n", TSegBase));
  MspDebugPrint ((MSP_DBG_MIN, "TSegSize:         0x%08X\n", TSegSize));

  HostIOBoundary = (EFI_PHYSICAL_ADDRESS) LShiftU64 ((MIN (0x1000 - MmioAllocation, MrcData->MrcParameters.SystemMemorySize) ), 20);

  //
  // If we're using upper memory, recalculate the UpperTotalMemory, taking
  // into consideration the MMIO hole.
  //
  if (HostIOBoundaryHi > 0x100000000) {
    RemapEnable = 1;
    UpperTotalMemory = UpperTotalMemory - HostIOBoundary + 0x100000000;
    MspDebugPrint ((MSP_DBG_MIN, "UpperTotalMemory after remap: 0x%08X", UpperTotalMemory >> 32));
    MspDebugPrint ((MSP_DBG_MIN, "%08X\n", UpperTotalMemory));
  }

  Sub4GMemAddr = TSegBase;
  if (RemapEnable != 0) {
    Above4GMemAddr = UpperTotalMemory - 0x100000000;
  }

#if ME_SUPPORT_FLAG
  //
  // Setting ME UMA Base Address
  //
  if (MrcData->MeSeg && (MrcData->MeUmaSize != 0)) {
    Sub4GMemAddr -= MrcData->MeUmaSize;
    MeUmaAddr = (UINT32) Sub4GMemAddr;
  }
#endif

#if defined(IQAT_RESERVED_MEMORY_REGION_SIZE) && IQAT_RESERVED_MEMORY_REGION_SIZE
  //
  // Locate installed IQAT Policy PPI
  //
  Status = PeiServicesLocatePpi (
         &gIqatIpPolicyPpiGuid,
         0,
         NULL,
         (VOID **) &IqatIpGlobalPolicy
         );
  ASSERT_EFI_ERROR (Status);

  if (IqatIpGlobalPolicy->IqatIpConfig.Enable) {
    Sub4GMemAddr -= (IQAT_RESERVED_MEMORY_REGION_SIZE);
    IqatReservedBase = Sub4GMemAddr;
  }
#endif

  // Denverton Aptiov override Start - EIP#373905
  if (BootMode == BOOT_ON_FLASH_UPDATE) {

    CapsulePpiStatus = PeiServicesLocatePpi (
               &gPeiCapsulePpiGuid,
               0,
               NULL,
               (VOID **) &Capsule
               );
    ASSERT_EFI_ERROR (CapsulePpiStatus);
	
    if (CapsulePpiStatus == EFI_SUCCESS) {
        Sub4GMemAddr -= CAPSULE_RESERVED_MEMORY_REGION_SIZE;
        CapsuleReservedBase = Sub4GMemAddr;
        CapsuleBuffer = (VOID *) ((UINTN) CapsuleReservedBase);

        Status = Capsule->Coalesce ((EFI_PEI_SERVICES **)PeiServices, &CapsuleBuffer, &CapsuleBufferLength);
        MspDebugPrint ((MSP_DBG_MIN, "BOOT_ON_FLASH_UPDATE: Capsule (coalesce) - Status = %r \n", Status));

      // If it fails, then NULL out capsule PPI pointer so that the capsule
      // HOB does not get created below.

      if (Status != EFI_SUCCESS) {
        Capsule = NULL;
        MspDebugPrint ((MSP_DBG_MIN, "BOOT_ON_FLASH_UPDATE: Capsule memory Hob creation FAILED \n"));
      }
    }
   }
   // Denverton Aptiov override End - EIP#373905
 
  //
  // Setting Non-Volatile Memory Base Address if ADR, C2F or NVDIMM is enabled
  //
  if (0 != MrcData->NonVolMemMode) {
    if (NVDIMM_MODE == MrcData->NonVolMemMode) {
      UINT8                  ch;
      UINT8                  dimm;

      MrcData->NonVolMemSize = 0;

      // Find the size of all NVDIMMS
      for (ch = 0; ch < MAX_CHANNELS; ch++) {
        if ( 1 == MrcData->MrcParameters.ChEnabled[ch] ) {
          for (dimm = 0; dimm < MAX_DIMMS; dimm++) {
            if ( (1 == MrcData->MrcParameters.DimmPresent[ch][dimm]) && (0 != MrcData->NvDimmType[ch][dimm]) ) {
              MrcData->NonVolMemSize += MrcData->MrcParameters.SlotMem[ch][dimm];
              MspDebugPrint ((MSP_DBG_MIN, "[NVDIMM]%d [%d][%d]:%x => NonVolMemSize:0x%x\n", MrcData->NvDimmType[ch][dimm], ch, dimm, MrcData->D_Size[ch][dimm], MrcData->NonVolMemSize));
            }//Channel->DimmPresent and NVDIMM
          }//dimm
        } //Channel->Enabled
      } //ch
    } //NVDIMM_MODE

    if (MrcData->NonVolMemSize) {
      NonVolatileHighMemorySize = (EFI_PHYSICAL_ADDRESS) LShiftU64 (MrcData->NonVolMemSize, 20);;
      if (Above4GMemAddr < NonVolatileHighMemorySize ) {
        //
        // limit persistent memory to address space above 4G
        //
        NonVolatileHighMemorySize = Above4GMemAddr;
      }
      Above4GMemAddr -= NonVolatileHighMemorySize;
      UpperTotalMemory -= NonVolatileHighMemorySize;
      NonVolatileHighMemoryAddr = UpperTotalMemory;
    }
    MspDebugPrint ((MSP_DBG_MIN, "[NVDIMM] Above4GMem:0x%lx...0x%lx  NonVolatileMem: 0x%lx + 0x%x\n", Above4GMemAddr, UpperTotalMemory, NonVolatileHighMemoryAddr, MrcData->NonVolMemSize));
  }
  
   // Denverton Aptiov override Start - EIP#373905
   if (Capsule != NULL){

      BuildResourceDescriptorHob (
      EFI_RESOURCE_SYSTEM_MEMORY,
      MEM_DET_COMMON_MEM_ATTR,
      CapsuleReservedBase,
      CAPSULE_RESERVED_MEMORY_REGION_SIZE
      );

      MspDebugPrint ((
      MSP_DBG_MIN,
         "Capsule (RSVD) Mem: 0x%lx -- 0x%lx\n",
         (UINT64) CapsuleReservedBase,
         (UINT64) CapsuleReservedBase + CAPSULE_RESERVED_MEMORY_REGION_SIZE - 1
         ));
  }
  // Denverton Aptiov override End - EIP#373905


  //
  // Report first 640KB of memory
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    MEM_DET_COMMON_MEM_ATTR,
    (EFI_PHYSICAL_ADDRESS) (0),
    (UINT64) (0xA0000)
  );
  //
  // Report first 0A0000h - 0FFFFFh as RESERVED memory
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_RESERVED,
    MEM_DET_COMMON_MEM_ATTR,
    (EFI_PHYSICAL_ADDRESS) (0xA0000),
    (UINT64) (0x60000)
  );
  //
  // Default Values
  //

  LowSystemMemoryBase = 0x100000;
  LowSystemMemorySize = Sub4GMemAddr - LowSystemMemoryBase;
  if (RemapEnable != 0) {
    HighSystemMemoryBase = 0x100000000;
    HighSystemMemorySize = Above4GMemAddr;
  }
  //
  //Check that RSVD Memory region has power-of-two length.
  //
  RsvdRegionLength = MrcData->MrcParameters.SaveRestore.Tolud - (LowSystemMemoryBase + LowSystemMemorySize);
  while(RsvdRegionLength > 0) {
    if(RsvdRegionLength & 1) {
  	  NumberOfOnes++;
    }
    RsvdRegionLength >>= 1;
  }

  //
  //If length of RSVD region not power-of-two aligned we will cover as UC little bit more
  //
  if(NumberOfOnes > 1) {
    MrcData->MrcParameters.SaveRestore.RsvdMem = MrcData->MrcParameters.SaveRestore.Tolud - (GetPowerOfTwo64(MrcData->MrcParameters.SaveRestore.Tolud - (LowSystemMemoryBase + LowSystemMemorySize)) << 1);
    RsvdRegionLength = (LowSystemMemoryBase + LowSystemMemorySize) - MrcData->MrcParameters.SaveRestore.RsvdMem;

    MspDebugPrint ((MSP_DBG_MIN, "RSVD memory region has been not power-of-two aligned. New RSVD Memory Base: 0x%lx  Length: 0x%lx\n", MrcData->MrcParameters.SaveRestore.RsvdMem, (MrcData->MrcParameters.SaveRestore.Tolud - MrcData->MrcParameters.SaveRestore.RsvdMem)));

    BuildResourceDescriptorHob (
  	  EFI_RESOURCE_MEMORY_RESERVED,
  	  MEM_DET_COMMON_MEM_ATTR,
	  MrcData->MrcParameters.SaveRestore.RsvdMem,
	  RsvdRegionLength
    );

    Sub4GMemAddr 		-= RsvdRegionLength;
    LowSystemMemorySize = MrcData->MrcParameters.SaveRestore.RsvdMem - LowSystemMemoryBase;
  }

  //
  // Change PeiMemory location for EFI-complaint Grub Bootloader, from tsegbase with length 64M. If RSVD memory region has (originally) power-of-two length, RsvdRegionLength variable has ZERO value
  //
#ifndef FSP_FLAG
  PeiMemSize = Sub4GMemAddr - dPeiMemBase;
#else  //FSP_FLAG
  PeiMemSize = PcdGet32(PcdFspReservedMemoryLength);
  dPeiMemBase = Sub4GMemAddr - PeiMemSize - BootLoaderTolumSize;
  LowSystemMemorySize -= (PeiMemSize + BootLoaderTolumSize);
#endif  //FSP_FLAG

  // Report the memory to EFI
  //
  if (BootMode != BOOT_ON_S3_RESUME) {
    Status = (*PeiServices)->InstallPeiMemory (PeiServices, dPeiMemBase, PeiMemSize);
    if (Status != EFI_SUCCESS) {
      MrcData->ErrorCode = MrcErrInstallPeiMemory;
      MrcDeadLoop ();
    }
  }

  MspDebugPrint ((MSP_DBG_MIN, "Low Memory Space Available  =0x%lx\n", Sub4GMemAddr));
  MspDebugPrint ((MSP_DBG_MIN, "High Memory Space Available =0x%lx\n", Above4GMemAddr));
  MspDebugPrint ((MSP_DBG_MIN, "-------------------Memory Mapping----------------\n"));

  MspDebugPrint ((
    MSP_DBG_MIN,
    "PeiMemory:       0x%lx -- 0x%lx\n",
    (UINT64) dPeiMemBase,
    (UINT64) (dPeiMemBase + PeiMemSize - 1)
  ));
  MspDebugPrint ((MSP_DBG_MIN, "System Memory:   0x%lx -- 0x%lx\n", (UINT64) 0x0, (UINT64) (0xA0000 - 1) ));

  MspDebugPrint ((
    MSP_DBG_MIN,
    "Reserved Memory: 0x%lx -- 0x%lx\n",
    (UINT64) 0xA0000,
    (UINT64) (0xA0000 + 0x60000 - 1)
  ));

  //
  // Report 1MB to TOLUD - (TSEG size) - (Punit FW size) - (NorthPeak memory Size) - (Iqat Reserved region) as system memory
  //
  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    MEM_DET_COMMON_MEM_ATTR,
    LowSystemMemoryBase,
    LowSystemMemorySize
  );

  MspDebugPrint ((
    MSP_DBG_MIN,
    "System Memory:   0x%lx -- 0x%lx\n",
    (UINT64) LowSystemMemoryBase,
    (UINT64) LowSystemMemoryBase + LowSystemMemorySize - 1
  ));

#ifdef FSP_FLAG
  //
  // HOB that describes FSP reserved memory below BootLoader TOLUM.
  //
  BuildResourceDescriptorWithOwnerHob (
    EFI_RESOURCE_MEMORY_RESERVED,
    MEM_DET_COMMON_MEM_ATTR,
    LowSystemMemoryBase + LowSystemMemorySize,
    PcdGet32(PcdFspReservedMemoryLength),
    &gFspReservedMemoryResourceHobGuid
  );

  //
  // Build HOB that describes at BootLoader TOLUM Resource reserved at top of main memory below 4GB.
  //
  if (BootLoaderTolumSize > 0) {
    BuildResourceDescriptorWithOwnerHob (
      EFI_RESOURCE_MEMORY_RESERVED,            // MemoryType,
      MEM_DET_COMMON_MEM_ATTR,
      LowSystemMemoryBase + LowSystemMemorySize + PcdGet32(PcdFspReservedMemoryLength),
      BootLoaderTolumSize,
      &gFspBootLoaderTolumHobGuid
      );
  }

  MspDebugPrint ((
    MSP_DBG_MIN,
    "Reserved Memory: 0x%lx -- 0x%lx\n",
    (UINT64) LowSystemMemoryBase + LowSystemMemorySize,
    (UINT64) LowSystemMemoryBase + LowSystemMemorySize + PeiMemSize - 1
  ));
#endif  //FSP_FLAG

 // Denverton Aptiov override Start - EIP#373905
 // Capsule PPI, if found (and didn't have errors), then
 // call the capsule PEIM to allocate memory for the capsule.

 if (Capsule != NULL) {
     Status = Capsule->CreateState ((EFI_PEI_SERVICES **) PeiServices, CapsuleBuffer, CapsuleBufferLength);
     MspDebugPrint ((MSP_DBG_MIN, "BOOT_ON_FLASH_UPDATE: Capsule in memory - Status = %r \n", Status));
 }

 // Denverton Aptiov override End - EIP#373905

#if ME_SUPPORT_FLAG
 if (MrcData->MeSeg && (MrcData->MeUmaSize != 0)){
    //
    // Report ME UMA region as reserved.
    //
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_RESERVED,
      MEM_DET_COMMON_MEM_ATTR,
      MeUmaAddr,
      MrcData->MeUmaSize
    );
    MspDebugPrint ((
      MSP_DBG_MIN,
      "ME UMA Region:    0x%lx -- 0x%lx\n",
      (UINT64) MeUmaAddr,
      (UINT64) MeUmaAddr + MrcData->MeUmaSize - 1
    ));
  }
#endif

#if defined(IQAT_RESERVED_MEMORY_REGION_SIZE) && IQAT_RESERVED_MEMORY_REGION_SIZE
  if (IqatIpGlobalPolicy->IqatIpConfig.Enable) {
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_RESERVED,
      MEM_DET_COMMON_MEM_ATTR,
      IqatReservedBase,
      IQAT_RESERVED_MEMORY_REGION_SIZE
      );
  
    BuildGuidDataHob (
      &gIqatIpReservedBaseHobGuid,
      &IqatReservedBase,
      sizeof (EFI_PHYSICAL_ADDRESS)
      );

    MspDebugPrint ((
       MSP_DBG_MIN,
       "Iqat (RSVD) Mem: 0x%lx -- 0x%lx\n",
       (UINT64) IqatReservedBase,
       (UINT64) IqatReservedBase + IQAT_RESERVED_MEMORY_REGION_SIZE - 1
     ));
   }
#endif

  if(RsvdRegionLength) {
	MspDebugPrint ((
	  MSP_DBG_MIN,
	  "Reserved Memory: 0x%lx -- 0x%lx\n",
	  MrcData->MrcParameters.SaveRestore.RsvdMem,
	  (MrcData->MrcParameters.SaveRestore.RsvdMem + RsvdRegionLength) - 1
    ));
  }

  //
  // Report (TOM-TSEG_SIZE) to TSEG_SIZE as reserved memory (SMRAM TSEG)
  //
  if (TSegSize != 0) {

    Hob.Raw = BuildGuidHob (
                &gEfiSmmPeiSmramMemoryReserveGuid,
                sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK) + sizeof (EFI_SMRAM_DESCRIPTOR)
              );
    ASSERT (Hob.Raw);
    SmramHobDescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) (Hob.Raw);
    SmramHobDescriptorBlock->NumberOfSmmReservedRegions = (UINT32) (1);
    SmramHobDescriptorBlock->Descriptor[0].PhysicalStart = (EFI_PHYSICAL_ADDRESS) (TSegBase);
    SmramHobDescriptorBlock->Descriptor[0].CpuStart      = (EFI_PHYSICAL_ADDRESS) (TSegBase);
    SmramHobDescriptorBlock->Descriptor[0].PhysicalSize  = (UINT64) TSegSize;
    SmramHobDescriptorBlock->Descriptor[0].RegionState   = (UINT64) (EFI_SMRAM_CLOSED);

  //
  // Build a HOB with the location of the reserved memory range.
  //
  CopyMem(&DescriptorAcpiVariable, &SmramHobDescriptorBlock->Descriptor[0], sizeof(EFI_SMRAM_DESCRIPTOR));
  DescriptorAcpiVariable.CpuStart += RESERVED_CPU_S3_SAVE_OFFSET;
  BuildGuidDataHob (
    &gEfiAcpiVariableGuid,
    &DescriptorAcpiVariable,
    sizeof (EFI_SMRAM_DESCRIPTOR)
    );
  

#ifndef FSP_FLAG
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_RESERVED,
      MEM_DET_COMMON_MEM_ATTR,
      (EFI_PHYSICAL_ADDRESS) (TSegBase),
      TSegSize
    );
#else //FSP_FLAG
    BuildResourceDescriptorWithOwnerHob (
      EFI_RESOURCE_MEMORY_RESERVED,
      MEM_DET_COMMON_MEM_ATTR,
      (EFI_PHYSICAL_ADDRESS) (TSegBase),
      TSegSize,
      &gFspReservedMemoryResourceHobTsegGuid
    );
#endif //FSP_FLAG
    MspDebugPrint ((
      MSP_DBG_MIN,
      "dTSeg:           0x%lx -- 0x%lx\n",
      (UINT64) TSegBase,
      (UINT64) TSegBase + TSegSize - 1
    ));
  }// TSegSize

  if (RemapEnable != 0) {
    if (HighSystemMemorySize > 0) {
      BuildResourceDescriptorHob (
        EFI_RESOURCE_SYSTEM_MEMORY,
        MEM_DET_COMMON_MEM_ATTR,
        HighSystemMemoryBase,
        HighSystemMemorySize
      );
      MspDebugPrint ((
        MSP_DBG_MIN,
        "UpperMemory:     0x%lx -- 0x%lx\n",
        (UINT64) HighSystemMemoryBase,
        (UINT64) (HighSystemMemoryBase) + HighSystemMemorySize - 1
      ));
    }
    if (0 != NonVolatileHighMemorySize)  {
      //
      // Setting Non-Volatile High Memory Base Address
      //
      BuildResourceDescriptorHob(
        EFI_RESOURCE_SYSTEM_MEMORY,
        MEM_DET_COMMON_MEM_ATTR | EFI_RESOURCE_ATTRIBUTE_PERSISTENT | EFI_RESOURCE_ATTRIBUTE_PERSISTABLE,
        NonVolatileHighMemoryAddr,
        NonVolatileHighMemorySize
        );
      MspDebugPrint((
        MSP_DBG_MIN,
        "ADR UpperMemory: 0x%lx -- 0x%lx\n",
        (UINT64)NonVolatileHighMemoryAddr,
        (UINT64)(NonVolatileHighMemoryAddr)+NonVolatileHighMemorySize - 1
        ));

      HighSystemMemorySize = Above4GMemAddr;
    }
  } // RemapEnable

  return EFI_SUCCESS;
}

/**
  This function installs the EFI Memory when S3 path is executed

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          EFI_SUCCESS     When no errors when installing memory
**/
EFI_STATUS
InstallS3Memory (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   MMRC_DATA             *MrcData
  )
{
  EFI_STATUS                      Status;
  UINTN                           VarSize;
  UINTN                           VarAttrib;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVariable;
  UINT64                          AcpiVariableSet64;
  ACPI_VARIABLE_SET               *AcpiVariableSet;
  UINTN                           S3MemoryBase;
  UINTN                           S3MemorySize;

  Status          = EFI_SUCCESS;
  AcpiVariableSet = NULL;
  VarSize         = sizeof (AcpiVariableSet64);
  VarAttrib       = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;

  MspDebugPrint ((MSP_DBG_MIN, "InstallS3Memory()\n"));

  //
  // Get necessary PPI
  //
  Status = (*PeiServices)->LocatePpi (
             PeiServices,
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             &PeiVariable
           );
  ASSERT_EFI_ERROR (Status);

  Status = PeiVariable->GetVariable (
             PeiVariable,
             ACPI_GLOBAL_VARIABLE,
             &gEfiAcpiVariableGuid,
             &VarAttrib,
             &VarSize,
             &AcpiVariableSet64
           );

  AcpiVariableSet = (ACPI_VARIABLE_SET *) (UINTN) AcpiVariableSet64;

  if (EFI_ERROR (Status) || (AcpiVariableSet == NULL) ) {
    return EFI_OUT_OF_RESOURCES;
  }

  S3MemoryBase = (UINTN) (AcpiVariableSet->AcpiReservedMemoryBase);
  S3MemorySize = (UINTN) (AcpiVariableSet->AcpiReservedMemorySize);

  //
  // Report Memory to EFI
  //
  Status = (*PeiServices)->InstallPeiMemory (PeiServices, S3MemoryBase, S3MemorySize);
  ASSERT_EFI_ERROR (Status);


  return Status;
}

/**
  This function restores the MRC parameters previously saved and 
  configured in the Setup Options 

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          EFI_SUCCESS     MRC parameters restored correctly.
**/
EFI_STATUS
MrcParamsRestore (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   MMRC_DATA             *MrcData
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVariable;
  //
  // GetVariable is returning more data that was saved in the Memory Config Hob so creating a temp buffer to hold the returned data
  //
  UINT8                           MrcParametersHob[sizeof(MRC_PARAMETERS)];
  SI_SA_POLICY_PPI                *SiSaPolicyPpi;
  MEMORY_CONFIGURATION            *MemConfig;
  NV_MEMORY_CONFIGURATION         *NvMemConfig;
  UINTN                           VariableSize;
#if LEAKY_BUCKET == 1 || PASS_GATE_TEST == 1
  UINT8                           Channel;
  UINT8                           Rank;
#endif
  // Denverton AptioV Override Start - EIP#235760
  //PCH_POLICY_PPI                  *PchPolicyPpi;
  // Denverton AptioV Override End - EIP#235760
  UINT8                           Dimm;
  UINT16                          SpdOffset;
  UINT8                           Index;
  UINT8                           TargetChannel;
  UINT8                           TargetRank;
  UINT8                           Bank;
  UINT8                           BankGroup;
  UINT8                           DramDevice;
  UINT32                          TargetRow;

  VariableSize = sizeof (MRC_PARAMETERS);

  //
  // Get necessary PPI
  //
  Status = (*PeiServices)->LocatePpi (
             PeiServices,
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             &PeiVariable
           );
  ASSERT_EFI_ERROR (Status);
  //
  // Get all the parameters for MRC_PARAMETERS
  //
  Status = PeiVariable->GetVariable (
             PeiVariable,
             L"MemoryConfig",
             &gEfiMemoryConfigDataGuid,
             NULL,
             &VariableSize,
             &MrcParametersHob
           );
  MspDebugPrint ((MSP_DBG_MAX, "%a() needed size:%x, after adjust %x after get %x.\n", __FUNCTION__, sizeof (MRC_SAVE_RESTORE), sizeof(MrcParametersHob), VariableSize));

  if (EFI_ERROR (Status) ) {
    MspDebugPrint ((MSP_DBG_MIN, "MRC Parameters not valid. Status is %x\n", Status));
    //
    // Final check - if we are executing any sort of fast path through MRC
    // which relies on restoring register values from ones which were saved
    // on a previous boot, make sure those saved values are valid before
    // attempting to restore. If they are not, force a powergood reset and
    // retrain memory from scratch.
    //
    if (MrcData->MrcParameters.BootMode != S5) {
      //
      // Do cold boot
      //
#ifndef SIM
      MspDebugPrint ((MSP_DBG_MIN, "Forcing power good reset. Variables are not valid\n"));
      IoOut8 (0xCF9, 0x0);
      IoOut8 (0xCF9, 0xE);
      _asm hlt
#else
      return EFI_FAILURE;
#endif
    }
  } else {
    //
    // Only override the SaveRestore parameters
    // into the MmrcData structure
    //
    CopyMem (
      MrcData,
      &MrcParametersHob,
      sizeof (MRC_SAVE_RESTORE)
    );
    MrcData->MrcParameters.SaveRestore.MrcParamsValidFlag = 1;
  }

  VariableSize = sizeof (UINT32);

  Status = PeiVariable->GetVariable (
             PeiVariable,
             L"MemCeil.",
             &gEfiGlobalVariableGuid,
             NULL,
             &VariableSize,
             (UINT32 *)&MrcData->MemoryCeiling
           );
  if (EFI_ERROR (Status) ) {
    MrcData->MemoryCeiling = 0;
  }

  Status = PeiServicesLocatePpi (
             &gSiSaPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiSaPolicyPpi
           );
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_STRUCT *)SiSaPolicyPpi, (UINT16)EnumMemConfigId, (VOID *)&MemConfig);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status) || MemConfig->ValidSetup == 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Platform Policy couldn't be read successfully: %x or the setup values are not valid: %d\n", Status, MemConfig->ValidSetup));
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "Platform Policy read successfully\n", Status));

    MrcData->FastBootEnable    = MemConfig->Fastboot;

    switch (MemConfig->MrcDebugMsg) {
    case 0:
      MrcData->MspData.DebugMsgLevel = 0;
      break;
    case 1:
      MrcData->MspData.DebugMsgLevel = MSP_DBG_MIN;
      break;
    case 2:
      MrcData->MspData.DebugMsgLevel = MSP_DBG_TYPICAL;
      break;
    case 3:
      MrcData->MspData.DebugMsgLevel = MSP_DBG_VERBOSE;
      break;
    default:
      MrcData->MspData.DebugMsgLevel = MSP_DBG_TYPICAL;
      // Denverton AptioV Override Start - EIP#235760
      /*
      Status = PeiServicesLocatePpi (
                 &gPchPolicyPpiGuid,
                 0,
                 NULL,
                 (VOID **) &PchPolicyPpi
               );
      ASSERT_EFI_ERROR (Status);
      switch ( PchPolicyPpi->PchConfig.DebugLevel ) {
      case MSP_DBG_NONE:
        MrcData->MspData.DebugMsgLevel = MSP_DBG_NONE;
        break;
      case DEBUG_ERROR:
        MrcData->MspData.DebugMsgLevel = MSP_DBG_MED;
        break;
      case DEBUG_WARN:
        MrcData->MspData.DebugMsgLevel = MSP_DBG_MAX;
        break;
      case (DEBUG_ERROR|DEBUG_INFO|DEBUG_WARN):       //0x80000042:
        MrcData->MspData.DebugMsgLevel = MSP_DBG_VERBOSE;
        break;
      case (DEBUG_ERROR|DEBUG_INFO|DEBUG_WARN|DEBUG_VERBOSE): //0x80400042:
        MrcData->MspData.DebugMsgLevel = MSP_DBG_MIN | MSP_DBG_MED | MSP_DBG_MAX;
        break;
      default:
        MrcData->MspData.DebugMsgLevel = MSP_DBG_MIN | MSP_DBG_MED | MSP_DBG_MAX;
      }
      */
      // Denverton AptioV Override End - EIP#235760
      break;
    }
    MrcData->MspData.DebugMsgLevelRequested = MrcData->MspData.DebugMsgLevel;
    // Mrc Regresion
//    MrcData->MspData.DebugMsgLevel = MSP_DBG_MIN | MSP_DBG_MED | MSP_DBG_MAX;
    MrcData->EnableParityCheck        = MemConfig->EnableParityCheck;
    MrcData->MrcParameters.EccEnabled[0]            = ((BOOLEAN) MemConfig->EccSupport);
    MrcData->MrcParameters.EccEnabled[1]            = ((BOOLEAN) MemConfig->EccSupport);
    if ((MrcData->MrcParameters.EccEnabled[0] == FALSE) || (MrcData->MrcParameters.EccEnabled[1] == FALSE)) {
      MrcData->PatrolScrubEnable        = 0;
      MrcData->PatrolScrubPeriod        = 0;
      MrcData->DemandScrub              = 0;
    } else {
      MrcData->PatrolScrubEnable        = MemConfig->PatrolSupport;
      MrcData->PatrolScrubPeriod        = MemConfig->PatrolPeriod;
      MrcData->DemandScrub              = MemConfig->DemandScrub;
    }
    MrcData->EnableScrambler          = MemConfig->EnableScrambler;
//    MrcData->EnableSlowPowerDownExit  = MemConfig->EnableSlowPowerDownExit;
    MrcData->MemoryPreservation       = MemConfig->MemoryPreservation;
    MrcData->HalfWidthEnable          = MemConfig->HalfWidthEnable;
    MrcData->TclIdle                  = MemConfig->TclIdle;
    MrcData->EnableParallelTraining   = MemConfig->EnableParallelTraining;
    MrcData->DynSR                    = MemConfig->DynamicSelfRefresh;
    MrcData->DynPmop                  = MemConfig->DynamicPowerModeOpcode;
    MrcData->SelectRefreshRate        = MemConfig->SelectRefreshRate;
    MrcData->CorrectErrThreshold      = MemConfig->CorrectErrThreshold;
    MrcData->MrcParameters.DdrFrequency             = MemConfig->DdrFreq;
    MrcData->TotalChannels            = MemConfig->DdrChannels;
    MrcData->CommandAddressParity     = MemConfig->CommandAddressParity;
    MrcData->SpdWriteDisable          = MemConfig->SpdWriteDisable;
    if (MemConfig->MrcRmtSupport == 1) {
      MrcData->MrcRmtSupport          |= RMT_SUPPORT_ENABLED;
    } else {
      MrcData->MrcRmtSupport          &= ~RMT_SUPPORT_ENABLED;
    }
//    MrcData->MrcRmtSupport          |= RMT_SUPPORT_ENABLED;
    MrcData->OpenPolicyTimer          = MemConfig->OpenPolicyTimer;
    MrcData->OemMrcData.TsegSize      = MemConfig->SmmSize;
//    MrcData->OutOfOrderAgingThreshold = MemConfig->OutOfOrderAgingThreshold;
    //MrcData->BankXorMapEnable       = MemConfig->BankXorMapEnable;

    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      MrcData->TwoClkRdPreamble[Channel] = MemConfig->TwoClkRdPreamble;
      MrcData->TwoClkWrPreamble[Channel] = MemConfig->TwoClkWrPreamble;
      MrcData->WriteDataEarlyEnable[Channel] = MemConfig->WriteDataEarlyEnable;
    }

    //
    // Since out of order and new request bypass are both negative logic
    // in the DUNIT, we invert the positive logic setup questions here.
    //
    if (MemConfig->EnableOutOfOrderProcessing == 1) {
      MrcData->OutOfOrderDisable        = 0;
    } else {
      MrcData->OutOfOrderDisable        = 1;
    }
    if (MemConfig->EnableNewRequestBypass == 1) {
//      MrcData->NewRequestBypassDisable  = 0;
    } else {
//      MrcData->NewRequestBypassDisable  = 1;
    }
    MrcData->CkePowerDownDisable      = MemConfig->CkePowerDownDisable;
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      MrcData->Rk2Rk[Channel] = MemConfig->Rk2RkEn;
    }
    MrcData->MaxScrubDebitCount       = MemConfig->MaxScrubDebitCount;
    MrcData->AbSegToDram              = MemConfig->AbSegToDram;
    MrcData->ESegToDram               = MemConfig->ESegToDram;
    MrcData->FSegToDram               = MemConfig->FSegToDram;
    if (MrcData->MrcParameters.SaveRestore.MrcParamsValidFlag == 0) {
      MrcData->MrcParameters.SaveRestore.SreDly = MemConfig->SelfRefreshDelay;
    }
    //
    // Enable to find the center for CMD and CTL signals relative to CLK.
    //
    //MrcData->CmdCtlTraining           = MemConfig->MrcCmdCtlTraining;
    MrcData->MemoryTestLoop           = MemConfig->MrcMemoryTestLoop;
    MrcData->BramParityEnable         = MemConfig->BramParityEnable;

    if (MemConfig->MrcHaltSystemOnMemTestError) {
      MrcData->HaltSystemOnMemTestError = MrcHaltSystemOnError;
    } else {
      MrcData->HaltSystemOnMemTestError = MrcNoHaltSystemOnError;
    }
    MrcData->ResetLoop = MemConfig->MrcResetLoop;
    MrcData->ResetLoopCycles = MemConfig->MrcResetLoopCycles;

    if (MrcData->MemoryCeiling != 0) {
      MrcData->OemMrcData.MmioSize = (UINT16) GetPowerOfTwo (0x1000 - (MrcData->MemoryCeiling >> 20) );
    } else {
      MrcData->OemMrcData.MmioSize = mMmioSize[MemConfig->MmioSize];
    }
    MrcData->MrcTrainingCpgcExpLoopCnt    = MemConfig->MrcTrainingCpgcExpLoopCntValue;
    MrcData->MrcCpgcNumBursts             = MemConfig->MrcTrainingCpgcNumBursts;
    MrcData->MrcRmtCpgcExpLoopCnt         = MemConfig->MrcRmtCpgcExpLoopCntValue;
    MrcData->MrcRmtCpgcNumBursts          = MemConfig->MrcRmtCpgcNumBursts;
    //
    // PerBit Support.
    // 
    MrcData->ReadPerBitEnable  = MemConfig->ReadPerBitEnable;
    MrcData->WritePerBitEnable = MemConfig->WritePerBitEnable;
    //
    // LoopCount for memory test range from 1 to 0xffff
    //
    MrcData->LoopCount = (UINT32) MemConfig->LoopCount;
#if INTERLEAVING_SUPPORT == 1
    MrcData->InterleaveMode = MemConfig->InterleaveMode;
#endif
#if ME_SUPPORT_FLAG
    MrcData->MeSeg                        = (BOOLEAN) MemConfig->MeSeg;
#endif       
    MrcData->VrefOverrideEnable           = MemConfig->VrefOverrideEnable;
    MrcData->VrefOverrideValue            = MemConfig->VrefOverrideValue;
    MrcData->ZqcEnable = (BOOLEAN) MemConfig->ZqcEnable;


    Status = GetConfigBlock ((CONFIG_BLOCK_STRUCT *)SiSaPolicyPpi, (UINT16)EnumNvMemConfigId, (VOID *)&NvMemConfig);
    ASSERT_EFI_ERROR (Status);

    //
    // Persistent Memory
    //
    MrcData->NonVolMemMode         = NvMemConfig->Mode;
    MrcData->NonVolMemInterleaving = NvMemConfig->Interleaving ;
    MrcData->NonVolMemSize         = NvMemConfig->Size;
    MrcData->AdrRecoveryDetected   = NvMemConfig->AdrRecoveryDetected;
    //Set MemoryPreservation if ADR recover is in process
    MrcData->MemoryPreservation    = MemConfig->MemoryPreservation | NvMemConfig->AdrRecoveryDetected;
    MrcData->eraseArmNVDIMMS       = NvMemConfig->eraseArmNVDIMMS;
    MrcData->restoreNVDIMMS        = NvMemConfig->restoreNVDIMMS;
    MrcData->CkeLatchGpioCtrl      = NvMemConfig->CkeLatchGpioCtrl;

#if LEAKY_BUCKET == 1
    //
    // Leaky Bucket
    //
    if (MemConfig->LeakyBucketUnits != 0) {
      MrcData->LeakyBucketParameters.RateUnits = ONE_DAY_IN_PS;
    } else {
      MrcData->LeakyBucketParameters.RateUnits = ONE_US_IN_PS;
    }
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        MrcData->LeakyBucketParameters.Rate[Channel][Rank] = MemConfig->LeakyBucketRate[Channel][Rank];
        MrcData->LeakyBucketParameters.CorrectableErrorThreshold[Channel][Rank] = MemConfig->CorrectableErrorThreshold[Channel][Rank];
      }
    }
#endif
    //
    // Setting Memory Thermal Throttling Variables.
    //
    MrcData->MemoryThermalEnable   = MemConfig->MemoryThermalEnable;
    MrcData->ThermalThrottlingType = MemConfig->ThermalThrottlingType;
    MrcData->ClttMode              = MemConfig->ClttMode;
    MrcData->TempHi                = MemConfig->TempHi;
    MrcData->TempMid               = MemConfig->TempMid;
    MrcData->TempLo                = MemConfig->TempLo;
    MrcData->BwLevelOnThrtCrit     = MemConfig->BwLevelOnThrtCrit;
    MrcData->BwLevelOnThrtHi       = MemConfig->BwLevelOnThrtHi;
    MrcData->BwLevelOnThrtMid      = MemConfig->BwLevelOnThrtMid;
    MrcData->OlttThrtBw            = MemConfig->OlttThrtBw;
    //
    // Setting MEMHOT variable
    //
    MrcData->MemHotThrotLvl        = MemConfig->MemHotThrotLvl;
    //
    // Setting MEMTRIP variables
    //
    MrcData->EnMemTrip             = MemConfig->EnMemTrip;
    MrcData->TempMemTrip           = MemConfig->TempMemTrip;
    //
    // RxTxSkewCtl ToDo: Connect the setup option 0: No skew 1: +2 2: -2 ticks
    //
    MrcData->RxSkewCtl             = MemConfig->RxSkewCtl;
    MrcData->TxSkewCtl             = MemConfig->TxSkewCtl;
    if (MrcData->MrcParameters.SaveRestore.MrcParamsValidFlag == 1) {
      MrcData->MrcParameters.SaveRestore.InputFineDdrVoltage   = MemConfig->InputFineDdrVoltage;
    }
    //
    // Performance profile for DMAP 
    //    Options:
    //       1: 17_19_13_18
    //       2: 17_19_6_18
    //       3: 17_19_6_7
    //
    MrcData->PerformanceProfile = MemConfig->PerformanceProfile;
    //
    // Override Settings
    //
//    MrcData->OverrideClocks.CmdOffsetValue = MemConfig->OverrideClocks.CmdOffsetValue;
//    MrcData->OverrideClocks.ClkOffsetValue = MemConfig->OverrideClocks.ClkOffsetValue;
//    MrcData->OverrideClocks.CsOffsetValue  = MemConfig->OverrideClocks.CsOffsetValue;
//    MrcData->OverrideClocks.CkeOffsetValue = MemConfig->OverrideClocks.CkeOffsetValue;
//    MrcData->OverrideClocks.OdtOffsetValue = MemConfig->OverrideClocks.OdtOffsetValue;
    //
    // Command Mode Override
    //
    MrcData->CommandMode           = MemConfig->CommandMode;

#if DRAM_RAPL == 1
    //
    // Dram RAPL
    //
    MrcData->DramRaplParameters.TimeWindow = MemConfig->DramRaplTimeWindow;
    MrcData->DramRaplParameters.PowerLimit = MemConfig->DramRaplPowerLimit;
    MrcData->DramRaplParameters.PowerLimitEnable = MemConfig->DramRaplPowerLimitEnable;
    MrcData->DramRaplParameters.PowerLimitLock = MemConfig->DramRaplLimitLock;
#endif // DRAM_RAPL
#if MITIGATION_FEATURE == 1
    MrcData->MitigationProbability    = MemConfig->MitigationProbability;
#endif // MITIGATION_FEATURE
#if PASS_GATE_TEST == 1
    MrcData->PassGateParameters.EnableTest                = (BOOLEAN) MemConfig->InputPassGateEnabled;
    MrcData->PassGateParameters.EnablePeriodicSelfRefresh = (BOOLEAN) MemConfig->InputPassGateEnableSelfRefresh;
    MrcData->PassGateParameters.Direction                 = MemConfig->InputPassGateDirection;
    MrcData->PassGateParameters.ActivationsCount          = MemConfig->InputPassGateRepetitionCountRange * 1000;
    MrcData->PassGateParameters.IterationOnRow            = MemConfig->InputPassGateIterationOnRow;
    MrcData->PassGateParameters.SwizzleMode               = MemConfig->InputPassGateSwizzle;
    MrcData->PassGateParameters.Pattern                   = MemConfig->InputPassGatePattern;
    MrcData->PassGateParameters.TargetPattern             = MemConfig->InputPassGateTargetPattern;
    MrcData->PassGateParameters.SpeedSelect               = MemConfig->InputSpeedSelected;

    MrcData->PassGateParameters.EnablePartialTest         = (BOOLEAN) MemConfig->InputPassGatePartial;
    MrcData->PassGateParameters.PartialRowBankMin         = MemConfig->InputPassGateRowBankMin;
    MrcData->PassGateParameters.PartialRowBankMax         = MemConfig->InputPassGateRowBankMax;

    MrcData->PassGateParameters.EnableMontecarloSearch    = (BOOLEAN) MemConfig->InputPassGateMonteCarloEnable;
    MrcData->PassGateParameters.MontecarloMaxFailures     = MemConfig->InputMontecarloMaxFailures;
    MrcData->PassGateParameters.MontecarloStartRepetition = MemConfig->InputMontecarloStartRepetition * 1000;
    MrcData->PassGateParameters.MontecarloDecrement       = MemConfig->InputMontecarloDecrement * 1000;

    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      for (Rank = 0; Rank < MAX_RANKS; Rank++) {
        MrcData->PassGateParameters.RankTest[Channel][Rank] = MemConfig->InputPassGateRankEnable[Channel][Rank];
      }
    }
#endif // PASS_GATE_TEST
    //
    // Override timing parameters from BIOS setup.
    //
    Status = GetTimingOverrides (PeiServices, MrcData);
    if (Status == EFI_SUCCESS) {
      OverrideTimingParams (MrcData);
    }

    //
    // Override DIMM SPD Address Configurations
    //
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
        if (MemConfig->SpdAddressTable[Channel][Dimm]){
          // Override DIMM SPD address if non-zero
          MrcData->OemMrcData.SpdAddressTable[Channel][Dimm] = MemConfig->SpdAddressTable[Channel][Dimm];
        }
      }
    }

    //
    // Override Memory Down Configurations
    //
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
        if (MemConfig->MemDownConfiguration.EnableMemoryDown[Channel][Dimm] == MEMORY_DOWN_ENABLE) {
          MrcData->OemMrcData.EnableMemoryDown[Channel][Dimm] = MemConfig->MemDownConfiguration.EnableMemoryDown[Channel][Dimm];
          if (MemConfig->MemDownConfiguration.MaxSpdSize > MAX_SPD_ADDR) {
            MrcData->ErrorCode = MrcErrSpdBufferOverrun;
            MrcDeadLoop ();
          }
          for (SpdOffset = 0; SpdOffset < MemConfig->MemDownConfiguration.MaxSpdSize; SpdOffset++) {
            MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SpdOffset] = MemConfig->MemDownConfiguration.MemoryDownSpdData[Channel][Dimm][SpdOffset];
          }

        }
      }
    }
    //
    // Memory Scrub Segments
    //
    if (MemConfig->MemoryScrubSegments.MemorySegmentationEnabled) {
      MrcData->OemMrcData.MemoryScrubSegments.MemorySegmentationEnabled  = MemConfig->MemoryScrubSegments.MemorySegmentationEnabled;
      MrcData->OemMrcData.MemoryScrubSegments.NumberOfSegments           = MIN(MAX_MRC_SCRUB_SEGMENTS, MemConfig->MemoryScrubSegments.NumberOfSegments);
      if (MAX_MRC_SCRUB_SEGMENTS < MemConfig->MemoryScrubSegments.NumberOfSegments) {
        MspDebugPrint ((MSP_DBG_MIN, "MAX_MRC_SCRUB_SEGMENTS < MemConfig->MemoryScrubSegments.NumberOfSegments\n"));
      }
      for (Index = 0; Index < MAX_MRC_SCRUB_SEGMENTS; Index++) {
        MrcData->OemMrcData.MemoryScrubSegments.ScrubSegment[Index].Low  = MemConfig->MemoryScrubSegments.ScrubSegment[Index].Low;
        MrcData->OemMrcData.MemoryScrubSegments.ScrubSegment[Index].High = MemConfig->MemoryScrubSegments.ScrubSegment[Index].High;
      }
    }

    //
    // Fast Boot Checker options
    //
    MrcData->MrcFastBootChecker.EnableFastBootTester                = MemConfig->FastBootChecker.EnableFastBootTester;
    MrcData->MrcFastBootChecker.Critical.Margin[MrcFastBootRxNDqs]  = MemConfig->FastBootChecker.Critical.MarginRxDelay;
    MrcData->MrcFastBootChecker.Critical.Margin[MrcFastBootRxPDqs]  = MemConfig->FastBootChecker.Critical.MarginRxDelay;
    MrcData->MrcFastBootChecker.Critical.Margin[MrcFastBootTxDq]    = MemConfig->FastBootChecker.Critical.MarginTxDelay;
    MrcData->MrcFastBootChecker.Critical.Margin[MrcFastBootRxVref]  = MemConfig->FastBootChecker.Critical.MarginRxVoltage;
    MrcData->MrcFastBootChecker.Critical.Margin[MrcFastBootTxVref]  = MemConfig->FastBootChecker.Critical.MarginTxVoltage;

    MrcData->MrcFastBootChecker.Warning.Margin[MrcFastBootRxNDqs]   = MemConfig->FastBootChecker.Warning.MarginRxDelay;
    MrcData->MrcFastBootChecker.Warning.Margin[MrcFastBootRxPDqs]   = MemConfig->FastBootChecker.Warning.MarginRxDelay;
    MrcData->MrcFastBootChecker.Warning.Margin[MrcFastBootTxDq]     = MemConfig->FastBootChecker.Warning.MarginTxDelay;
    MrcData->MrcFastBootChecker.Warning.Margin[MrcFastBootRxVref]   = MemConfig->FastBootChecker.Warning.MarginRxVoltage;
    MrcData->MrcFastBootChecker.Warning.Margin[MrcFastBootTxVref]   = MemConfig->FastBootChecker.Warning.MarginTxVoltage;

    for (Index = 0; Index < 8; Index++) {
      if (MemConfig->PprEnable[Index]) {
        TargetChannel = MemConfig->TargetChannel[Index];
        TargetRank    = MemConfig->TargetRank[Index];
        Bank          = MemConfig->Bank[Index];
        BankGroup     = MemConfig->BankGroup[Index];
        DramDevice    = MemConfig->DramDevice[Index];
        TargetRow     = MemConfig->Row[Index];

        MrcData->PprParameters.Repair[TargetChannel][TargetRank][BankGroup]       = TRUE;
        MrcData->PprParameters.RowAddress[TargetChannel][TargetRank][BankGroup]   = TargetRow;
        MrcData->PprParameters.BankAddress[TargetChannel][TargetRank][BankGroup]  = Bank;
        MrcData->PprParameters.DramDevice[TargetChannel][TargetRank][BankGroup]   = DramDevice;

        MspDebugPrint ((
          MSP_DBG_MIN,
          "C%d-R%d-B%d-BG%d-DD%d - Row = 0x%06x\n",
          TargetChannel,
          TargetRank,
          Bank,
          BankGroup,
          DramDevice,
          TargetRow
        ));
      }
    }
  } //  Status for Read Setup variables was done OK
#ifdef FSP_FLAG
  if (MemConfig->SkipMemoryTest) {
    MrcData->SkipTrainingAlgos[LMemoryTest] = TRUE;
  } else {
    MrcData->SkipTrainingAlgos[LMemoryTest] = FALSE;
  }
#endif

  return EFI_SUCCESS;
}


#if BDAT
/**
  This function saves the BDAT structure to Hob for later use. 

  @param[in]       PeiServices         PEI Services Table.
  @param[in, out]  MrcData             Host structure for all data related to MMRC.
  @param[in] BdatMemoryData2Structure  Pointer to the BDAT data structure.
  @retval          EFI_SUCCESS         Hob is successfully built
**/
EFI_STATUS
MrcBdatSaveHob (
  IN        EFI_PEI_SERVICES                **PeiServices,
  IN        MMRC_DATA                       *MrcData,
  IN        BDAT_MEMORY_DATA_2_STRUCTURE    *BdatMemoryData2Structure
  )
{
  BDAT_MEMORY_DATA_2_STRUCTURE  *BdatMemoryData2Hob;
  EFI_PEI_HOB_POINTERS          Hob;
  UINTN                         BufferSize;

  BufferSize = sizeof (BDAT_MEMORY_DATA_2_STRUCTURE);

  MspDebugPrint ((MSP_DBG_MIN, "Max Data for a HOB = %d Bytes\n", 0xFFF8 - sizeof (EFI_HOB_GUID_TYPE)));
  MspDebugPrint ((MSP_DBG_MIN, "BDAT_MEMORY_DATA_2_STRUCTURE = %d Bytes\n", BufferSize));

  Hob.Raw = BuildGuidHob (
              &gEfiBdatMemoryData2Guid,
              BufferSize
            );
  ASSERT (Hob.Raw);

  BdatMemoryData2Hob = (BDAT_MEMORY_DATA_2_STRUCTURE *) Hob.Raw;

  CopyMem (
    BdatMemoryData2Hob,
    BdatMemoryData2Structure,
    sizeof (BDAT_MEMORY_DATA_2_STRUCTURE)
    );

  MspDebugPrint ((MSP_DBG_MIN, "BdatMemoryData2Hob = 0x%08x\n", BdatMemoryData2Hob));

  return EFI_SUCCESS;
}
#endif

/**
  This function saves the MRC Parameters to Hob for later use.

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          EFI_SUCCESS     Hob is successfully built.
**/
EFI_STATUS
MrcParamsSave (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   MMRC_DATA             *MrcData
  )
{
  EFI_PEI_HOB_POINTERS      Hob;
  UINTN                     BufferSize;

  BufferSize = sizeof (MRC_PARAMETERS);
  Hob.Raw = BuildGuidDataHob (&gEfiMemoryConfigDataGuid, MrcData, BufferSize );
  ASSERT (Hob.Raw);
  MspDebugPrint ((MSP_DBG_MIN, "%a  Hob:%x BufferSize:0x%08x MrcParamsHob:%x\n", __FUNCTION__, Hob.Raw, BufferSize, sizeof (MRC_PARAMETERS)));

#ifdef FSP_FLAG
  // The gEfiMemoryConfigDataGuid HOB only contains the MRC_PARAMETERS structure
  // (located at the start of MMRC_DATA). It will not contain the result of the
  // fast boot checker, so build a new HOB to report the result of this test to
  // the boot loader.
  UINT32 FastBootCheckerResult;

  if (MrcData->MrcFastBootChecker.EnableFastBootTester) {
    FastBootCheckerResult = MrcData->MrcFastBootChecker.CriteriaTestResult;
    BuildGuidDataHob (&gFspFastBootCheckerHobGuid,
    &FastBootCheckerResult,
    sizeof(FastBootCheckerResult));
  }
#endif


  return EFI_SUCCESS;
}

/**
  This function saves the MRC Version to Hob for later use.

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcVersion      MRC Version.
  @retval          EFI_SUCCESS     Hob is successfully built.
**/
EFI_STATUS
MrcVersionSaveHob (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   UINT32                MrcVersion
  )
{
  UINT32                   *MrcVersionHob;
  EFI_PEI_HOB_POINTERS      Hob;
  UINTN                     BufferSize;

  BufferSize = sizeof (UINT32);

  Hob.Raw = BuildGuidHob (
              &gEfiMrcVersionGuid,
              BufferSize
            );
  ASSERT (Hob.Raw);

  MrcVersionHob = (UINT32 *) Hob.Raw;

  CopyMem (
    MrcVersionHob,
    &MrcVersion,
    BufferSize
    );

  return EFI_SUCCESS;
}

/**
  This overrides the MRC Parameters set in DetectDimms with 
  setup values. 

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          EFI_SUCCESS     Setup correctly done.
**/
EFI_STATUS
GetTimingOverrides (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   MMRC_DATA             *MrcData
  )
{
  UINT8                           Channel;
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVariable;
  SI_SA_POLICY_PPI                *SiSaPolicyPpi;
  MEMORY_CONFIGURATION            *MemConfig;
  //
  // Get necessary PPI
  //
  Status = (*PeiServices)->LocatePpi (
             PeiServices,
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             &PeiVariable
           );
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesLocatePpi (
             &gSiSaPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiSaPolicyPpi
           );
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_STRUCT *)SiSaPolicyPpi, (UINT16)EnumMemConfigId, (VOID *)&MemConfig);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status) || MemConfig->ValidSetup == 0) {
    MspDebugPrint ((MSP_DBG_MIN, "Platform Policy couldn't be read successfully: %x or the setup values are not valid: %d\n", Status, MemConfig->ValidSetup));
    return MMRC_SUCCESS;
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "Platform Policy read successfully\n", Status));
  }

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel]) {
      continue;
    }
    if (MemConfig->TclTiming != 0) {
      MrcData->TimingDataOverrides[Channel][TimingTcl][FreqA]  = MemConfig->TclTiming;
    }
    if (MemConfig->TRcdTiming != 0) {
      MrcData->TimingDataOverrides[Channel][TimingTrcd][FreqA] = MemConfig->TRcdTiming;
    }
    if (MemConfig->TRpTiming != 0) {
      MrcData->TimingDataOverrides[Channel][TimingTrp][FreqA]  = MemConfig->TRpTiming;
    }
    if (MemConfig->TRasTiming != 0) {
      MrcData->TimingDataOverrides[Channel][TimingTras][FreqA] = MemConfig->TRasTiming;
    }
    if (MemConfig->TRtpTiming != 0) {
      MrcData->TimingDataOverrides[Channel][TimingTrtp][FreqA] = MemConfig->TRtpTiming;
    }
    if (MemConfig->TRrdTiming != 0) {
      MrcData->TimingDataOverrides[Channel][TimingTrrd][FreqA] = MemConfig->TRrdTiming;
    }
    if (MemConfig->TFawTiming!= 0) {
      MrcData->TimingDataOverrides[Channel][TimingTfaw][FreqA] = MemConfig->TFawTiming;
    }
    if (MemConfig->TCcdTiming != 0) {
      MrcData->TimingDataOverrides[Channel][TimingTccd][FreqA] = MemConfig->TCcdTiming;
    }
    if (MemConfig->TWtpTiming != 0) {
      MrcData->TimingDataOverrides[Channel][TimingTwtp][FreqA] = MemConfig->TWtpTiming;
    }
    if (MemConfig->TWclTiming != 0) {
      MrcData->TimingDataOverrides[Channel][TimingTwcl][FreqA] = MemConfig->TWclTiming;
    }
  } // for (Channel = 0; Channel < MAX_CHANNELS; Channel++)

  return EFI_SUCCESS;
}
#if ME_SUPPORT_FLAG
/**
  This function read and returns the amount of ME UMA requested 
  by ME ROM from the HECI Device. Skips using ME UMA PPI if not 
  located. 

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          NONE
**/
VOID
OemGetSetMeUma (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   MMRC_DATA              *MrcData
  )
{
  ME_UMA_PPI   *MeUma;
  EFI_STATUS   Status; 

  Status = (*PeiServices)->LocatePpi (
              PeiServices, 
              &gMeUmaPpiGuid,
              0,
              NULL,
              &MeUma
           );  
  if (Status != EFI_SUCCESS) {
    MspDebugPrint ((MSP_DBG_MIN, "UMA: UMA_PPI could not be located.\n"));
    return;
  }
  //
  // Get ME UMA size only if MeSeg and Isoc are Enabled
  //  
  if (MrcData->MeSeg /*&& !BDebug0.Bits.disable_badmit_urgent_isoc*/) {
    //
    // UMA Size in MB units.
    //
    if (MeUma->isMeUmaEnabled()) {
      if (MrcData->MrcParameters.BootMode == S5) {
        MrcData->MeUmaSize = (MeUma->MeSendUmaSize(NULL) << 20);
      }
    } 
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "UMA: ME UMA size set to 0. Isoc is Disabled.\n")); 
  }
  return;  
}

/**
  MeConfigDidReg     - Send indication to ME FW to start using allocated ME UMA Space.
  HandleMeBiosAction - Will enforce the BIOS Action that was requested by ME FW
                       as part of the DRAM Init Done message
  Skips using ME UMA PPI if not located. 

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcStatus       Pointer to MRC Status.
  @retval          NONE
**/
VOID
OemSendDramInitDoneCommand (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   EFI_STATUS            *MrcStatus
  )
{
  ME_UMA_PPI   *MeUma;
  EFI_STATUS   Status;
  UINT8        NextStep;
  UINT8        InitStat;

  Status = (*PeiServices)->LocatePpi (
              PeiServices, 
              &gMeUmaPpiGuid,
              0,
              NULL,
              &MeUma
           );
  if (Status != EFI_SUCCESS) {
     return;
  }
  if (MeUma->isMeUmaEnabled()) {
    //
    // Send 0 if MemoryInit Installed, 1 if there were Errors, TODO: What else?? 
    //
    if (*MrcStatus != EFI_SUCCESS) {
      InitStat = 1;
    } else {
      InitStat = 0;
    }
    MspDebugPrint ((MSP_DBG_MIN, "Send DRAM_INIT_DONE to ME (InitStart = 0x%x).\n", InitStat));
    MeUma->MeConfigDidReg (NULL, InitStat, &NextStep);

    MspDebugPrint ((MSP_DBG_MIN, "MeDramInitDone Complete. Checking for reset...\n"));
    MeUma->HandleMeBiosAction (NextStep);
  }
  return;
}
#endif

#ifdef IE_SUPPORT

/**

  Send to IE FW information that memory is ready.

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcStatus       Pointer to MRC Status.
  @retval          NONE
**/
VOID
OemSendIEDramInitDoneCommand (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   EFI_STATUS            *MrcStatus
  )
{
  PEI_IE_HECI_PPI   *IeHeciPpi = NULL;
  EFI_STATUS        Status;
  UINT8             InitStat;

  //
  // Locate IeHeci PPI.
  //
  Status = (*PeiServices)->LocatePpi (PeiServices, &gPeiIeHeciPpiGuid, 0, NULL, &IeHeciPpi);

  ASSERT_EFI_ERROR (Status);
  if (Status == EFI_SUCCESS){
    UINT8 Action = 0;

    if (*MrcStatus != EFI_SUCCESS) {
        InitStat = 1;
    } else {
        InitStat = 0;
    }

    Status = IeHeciPpi->MemoryInitDoneNotify(IeHeciPpi, InitStat, &Action);
    switch (Status) {
      case EFI_SUCCESS:
        MspDebugPrint ((MSP_DBG_MIN, "[IE] IeMemoryInitDone Complete.\n"));
        break;
      case EFI_UNSUPPORTED:
        MspDebugPrint ((MSP_DBG_MIN, "[IE] IeMemoryInitDone disabled.\n"));
        break;
      default:
        MspDebugPrint ((MSP_DBG_MIN, "[IE] ERROR: IeMemoryInitDone FAILED (%r).\n", Status));
        break;
    }

  }
}

#endif //IE_SUPPORT

#ifdef FSP_FLAG
/**
  Build FSP SMBIOS memory info HOB

  @param[in] MrcData          Pointer to Mrc Parameters
**/
VOID
BuildFspSmbiosMemoryInfoHob (
  IN MMRC_DATA                *MrcData
  )
{
  FSP_SMBIOS_MEMORY_INFO      FspSmbiosMemoryInfo;
  UINT8                       ChannelIndex;
  UINT8                       ChannelCount;
  UINT8                       DimmIndex;
  UINT8                       DimmCount;
  DIMM_INFO                   *DimmInfo;

  //
  // Revision
  //
  FspSmbiosMemoryInfo.Revision = 0x01;
  MspDebugPrint ((MSP_DBG_MAX, "[FspSmbiosMemoryInfo] Revision: 0x%x\n", FspSmbiosMemoryInfo.Revision));

  //
  // DdrFrequency
  //
  switch (MrcData->MrcParameters.DdrFrequency) {
    case MMRC_800:
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 800;
      break;
    case MMRC_1066:
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 1066;
  	  break;
    case MMRC_1333:
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 1333;
  	  break;
    case MMRC_1600:
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 1600;
  	  break;
    case MMRC_1866:
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 1866;
  	  break;
    case MMRC_2133:
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 2133;
  	  break;
    case MMRC_2400:
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 2400;
  	  break;
    case MMRC_2666:
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 2666;
  	  break;
    case MMRC_2933:
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 2933;
  	  break;
    case MMRC_3200 :
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 3200 ;
  	  break;
    case MMRC_3777:
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 3777;
  	  break;
    default:
      FspSmbiosMemoryInfo.MemoryFrequencyInMHz = 0;
  	  break;
  }
  MspDebugPrint ((MSP_DBG_MAX, "[FspSmbiosMemoryInfo] MemoryFrequencyInMHz: %d\n", FspSmbiosMemoryInfo.MemoryFrequencyInMHz));

  //
  // ErrorCorrectionType
  //
  if (MrcData->MrcParameters.EccEnabled[0]) {
    FspSmbiosMemoryInfo.ErrorCorrectionType = MemoryErrorCorrectionSingleBitEcc;
  } else {
    FspSmbiosMemoryInfo.ErrorCorrectionType = MemoryErrorCorrectionNone;
  }
  MspDebugPrint ((MSP_DBG_MAX, "[FspSmbiosMemoryInfo] ErrorCorrectionType: 0x%x\n", FspSmbiosMemoryInfo.ErrorCorrectionType));

  //
  // ChannelInfo
  //
  ChannelCount = 0;
  for (ChannelIndex = 0; ChannelIndex < MAX_CH; ChannelIndex++) {

	if (MrcData->MrcParameters.ChEnabled[ChannelIndex] == 0)
	  continue;

    FspSmbiosMemoryInfo.ChannelInfo[ChannelIndex].ChannelId = ChannelIndex;
    FspSmbiosMemoryInfo.MemoryType = MrcData->MrcParameters.DramType[ChannelIndex];
    MspDebugPrint ((MSP_DBG_MAX, "[FspSmbiosMemoryInfo] [CHANNEL %x] MemoryType: 0x%x\n", FspSmbiosMemoryInfo.ChannelInfo[ChannelIndex].ChannelId, FspSmbiosMemoryInfo.MemoryType));

    DimmCount = 0;
    DimmInfo = FspSmbiosMemoryInfo.ChannelInfo[ChannelIndex].DimmInfo;

    //
    // DimmInfo
    //
    for (DimmIndex = 0; DimmIndex < MAX_DIMM; DimmIndex++) {
      DimmInfo[DimmIndex].DimmId = DimmIndex;
      if (MrcData->MrcParameters.DimmPresent[ChannelIndex][DimmIndex] == 1) {
        DimmInfo[DimmIndex].SizeInMb = MrcData->MrcParameters.SlotMem[ChannelIndex][DimmIndex];
    	MspDebugPrint ((MSP_DBG_MAX, "[FspSmbiosMemoryInfo] [CHANNEL %x] [DIMM %x] SizeInMb: 0x%x\n", FspSmbiosMemoryInfo.ChannelInfo[ChannelIndex].ChannelId, DimmInfo[DimmIndex].DimmId, DimmInfo[DimmIndex].SizeInMb));
    	if (MrcData->MrcParameters.DramType[ChannelIndex] == TypeDdr4) {
    	  DimmInfo[DimmIndex].MfgId = (MrcData->MrcParameters.SpdData[ChannelIndex][DimmIndex].Buffer[SPD_DDR4_MMIDL]) | (MrcData->MrcParameters.SpdData[ChannelIndex][DimmIndex].Buffer[SPD_DDR4_MMIDH] << 8);
      	  MspDebugPrint ((MSP_DBG_MAX, "[FspSmbiosMemoryInfo] [CHANNEL %x] [DIMM %x] MfgId: 0x%x\n", FspSmbiosMemoryInfo.ChannelInfo[ChannelIndex].ChannelId, DimmInfo[DimmIndex].DimmId, DimmInfo[DimmIndex].MfgId));
          CopyMem (&DimmInfo[DimmIndex].ModulePartNum[0],
                   &MrcData->MrcParameters.SpdData[ChannelIndex][DimmIndex].Buffer[SPD_DDR4_MODULEPN],
                   sizeof(DimmInfo[DimmIndex].ModulePartNum) / sizeof(DimmInfo[DimmIndex].ModulePartNum[0]));
    	} else {
          // TODO - DDR3
        }
        //
        // DataWidth
        //
        FspSmbiosMemoryInfo.DataWidth = MrcData->D_DataWidth[ChannelIndex][DimmIndex];
    	MspDebugPrint ((MSP_DBG_MAX, "[FspSmbiosMemoryInfo] [CHANNEL %x] [DIMM %x] DataWidth: 0x%x\n", FspSmbiosMemoryInfo.ChannelInfo[ChannelIndex].ChannelId, DimmInfo[DimmIndex].DimmId, FspSmbiosMemoryInfo.DataWidth));
        DimmCount++;
      }
    }

    //
    // DimmCount
    //
    FspSmbiosMemoryInfo.ChannelInfo[ChannelIndex].DimmCount = DimmCount;
	MspDebugPrint ((MSP_DBG_MAX, "[FspSmbiosMemoryInfo] [CHANNEL %x] DimmCount: 0x%x\n", FspSmbiosMemoryInfo.ChannelInfo[ChannelIndex].ChannelId, FspSmbiosMemoryInfo.ChannelInfo[ChannelIndex].DimmCount));
    ChannelCount++;
  }

  //
  // ChannelCount
  //
  FspSmbiosMemoryInfo.ChannelCount = ChannelCount;
  MspDebugPrint ((MSP_DBG_MAX, "[FspSmbiosMemoryInfo] ChannelCount: 0x%x\n", FspSmbiosMemoryInfo.ChannelCount));

  //
  // Build HOB for FspSmbiosMemoryInfo
  //
  BuildGuidDataHob (
    &gFspSmbiosMemoryInfoHobGuid,
    &FspSmbiosMemoryInfo,
    sizeof (FSP_SMBIOS_MEMORY_INFO)
    );
}
#endif

