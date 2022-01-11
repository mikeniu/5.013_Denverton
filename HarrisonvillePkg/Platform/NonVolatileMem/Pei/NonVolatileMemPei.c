/*++
 This file contains an 'Intel Peripheral Driver' and is
 licensed for Intel CPUs and chipsets under the terms of your
 license agreement with Intel or your vendor.  This file may
 be modified by the user, subject to additional terms of the
 license agreement
 --*/
/*++

 @copyright
 Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor.  This file may be modified by the user, subject to
 the additional terms of the license agreement


 Module Name:

 NonVolatileMemPei.c

 Abstract:

 Do platform specific stage2 initializations.

 --*/

#include "NonVolatileMemPei.h"
#include "Base.h"
#include "C2F.h"
#include <Library/DebugLib.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/NonVolatileMemPolicy.h>
#include <Guid/NonVolatileMemoryHob.h>
#include <Library/HobLib.h>
#include <Library/C2fAhciLib.h>
#include <Library/AdrCfgLib.h>
#include <Include/PlatformDefinitions.h>
#include <Library/PciLib.h>
#include <Library/GpioCtrlrLib.h>
#include <Ppi/SaPolicy.h>
#include <Ppi/PchPolicy.h>
#include <Library/ConfigBlockLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Library/PeiPlatformHookLib.h>

#ifndef VOLATILE
#define VOLATILE    volatile
#endif
typedef UINT32 VOLATILE * UINT32_REG;

static VOID UpdateAdrRecoveryDetected(
IN CONST EFI_PEI_SERVICES **PeiServices,
    IN NON_VOLATILE_MEM_POLICY_PPI * NonVolPpi);
static EFI_STATUS PublishNonVolatileMemHob( IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *Ppi);
static EFI_STATUS EFIAPI AdrCallback( IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *Ppi);

// Denverton Aptiov override start
// #define BitAt(data,bit)               (0 < (data & (1<<bit)))/*(0<0)=0 (0<1 or greater)=1*/
#define BitAt(data,bit)               (0 < (data & (LShiftU64 ( 1, bit )))) 	// Denverton AptioV Override - EIP#293004 
// Denverton Aptiov override end
#define ADR_EN                        (0xf0)
#define P_CR_ADR_COMMAND_0_0_0_MCHBAR (0x712C)
#define L2_FLUSH_BIT                  (0)
#define CKE_LATCH_GPIO                (gpio_GPIO_2) //dnvtodo - find the real GPIO
#define ADR_MODE                      (1)
#define C2F_ADR_MODE                  (2)
#define ADR_FEAT_EN                   (1)

static EFI_PEI_NOTIFY_DESCRIPTOR mC2fNotifyList[] = { {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid, C2FCallback } };

static EFI_PEI_NOTIFY_DESCRIPTOR mAdrNotifyList[] = { {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid, AdrCallback } };

static EFI_PEI_NOTIFY_DESCRIPTOR mHobNotifyList[] = { {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), &gEfiEndOfPeiSignalPpiGuid,
    PublishNonVolatileMemHob } };

static CPLD_OP_MODES NVMtoCpldModeMap[(NONVOL_NVDIMM + 1)] =
//   NONVOL_DISABLED, NONVOL_ADR, NONVOL_ADR_C2F, NONVOL_SMI_C2F, NONVOL_NVDIMM
    { NVM_DISABLE, ADR_BBU, C2F, SMI_ONLY, NVDIMM };

EFI_STATUS
ConfigurePmc( IN NON_VOLATILE_MEM_POLICY_PPI * NonVolPpi);

EFI_STATUS EFIAPI AdrCallback( IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *Ppi) {
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 PwrmBase;
  UINT32_REG PmSts;
  NON_VOLATILE_MEM_POLICY_PPI *NonVolatileMemPpi = NULL;

  DEBUG((DEBUG_INFO, "%a() NonVolatile memory support\n", __FUNCTION__ ));
  AdrCfg_Deactivate(); //Clear signal that ADR recover is in process

  Status = (**PeiServices).LocatePpi(PeiServices, &gNonVolatileMemPpiGuid, 0,
  NULL, (VOID **) &NonVolatileMemPpi);
  if ((EFI_ERROR(Status)) || (NonVolatileMemPpi == NULL)) {
    DEBUG(
        (DEBUG_ERROR, "%a() - Failed to find NonVolatileMem PPI\n",__FUNCTION__));
    ASSERT_EFI_ERROR(Status);
    ASSERT(FALSE);
  }
//  ConfigurePmc(NonVolatileMemPpi);

  Status = PchPwrmBaseGet(&PwrmBase);
  ASSERT_EFI_ERROR(Status);
  PmSts = (UINT32_REG)( PwrmBase + PCH_PM_STS );
  // Denverton Aptiov override start
  //DEBUG ((DEBUG_INFO, "%a() - PWRMBASE:%x->PCH_PM_STS:%x(%x).ADR_RST_STS(%x) ADR_EN:%x(%x) \n", __FUNCTION__, PwrmBase, PmSts, *PmSts, (*PmSts & (1 << ADR_RST_STS)), ( PwrmBase + ADR_EN ), *(UINT32_REG)( PwrmBase + ADR_EN )));
  //*PmSts |= (1 << ADR_RST_STS);
  DEBUG ((DEBUG_INFO, "%a() - PWRMBASE:%x->PCH_PM_STS:%x(%x).ADR_RST_STS(%x) ADR_EN:%x(%x) \n", __FUNCTION__, PwrmBase, PmSts, *PmSts, (*PmSts & LShiftU64 ( 1, ADR_RST_STS )), ( PwrmBase + ADR_EN ), *(UINT32_REG)( PwrmBase + ADR_EN )));
  *PmSts |= LShiftU64 ( 1, ADR_RST_STS );
  // Denverton Aptiov override end
  return Status;
}

EFI_STATUS PublishNonVolatileMemHob( IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *Ppi)
/*++

 Routine Description:

 Sets up the data being passed to the DXE NonVolMem driver

 Arguments:

 PeiServices - General purpose services available to every PEIM.
 NonVolPpi - Nonvolatile memory data being maintained

 Returns:

 Status -  EFI_SUCCESS if the interface could be successfully installed.
 EFI_NOT_FOUND if required PPI not found.

 --*/
{
  EFI_STATUS Status = EFI_SUCCESS;
  NON_VOLATILE_MEM_HOB NonVolMemHob;
  NON_VOLATILE_MEM_POLICY_PPI *NonVolPpi = NULL;
  const UINT8 MAX_NVDIMMS = sizeof(NonVolMemHob.NVDIMM_SMBUS_ADDR)
      / sizeof(NonVolMemHob.NVDIMM_SMBUS_ADDR[0]);
  VOID *Hob;
  UINT8 i;

  Status = (**PeiServices).LocatePpi(PeiServices, &gNonVolatileMemPpiGuid, 0,
  NULL, (VOID **) &NonVolPpi);
  ASSERT_EFI_ERROR(Status);

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  NonVolMemHob.NonVolMemMode = NonVolPpi->NonVolMemMode;
  NonVolMemHob.AdrRecoveryDetected = NonVolPpi->AdrRecoveryDetected;
  NonVolMemHob.C2fLowStart = NonVolPpi->C2fLowStart;
  NonVolMemHob.C2fLowLength = NonVolPpi->C2fLowLength;
  NonVolMemHob.C2fHighStart = NonVolPpi->C2fHighStart;
  NonVolMemHob.C2fHighLength = NonVolPpi->C2fHighLength;
  NonVolMemHob.C2fLbaLocMethod = NonVolPpi->NonVolMemC2fLbaLocMethod;
  NonVolMemHob.C2fLba = NonVolPpi->NonVolMemC2fLba;
  for (i = 0; i < MAX_NVDIMMS; i++) {
    NonVolMemHob.NVDIMM_SMBUS_ADDR[i] = NonVolPpi->NVDIMM_SMBUS_ADDR[i];
  }

  //
  // Publish NonVol Memory data for DXE driver usage
  //
  Hob = BuildGuidDataHob(&gNonVolatileMemHobGuid, &(NonVolMemHob),
      sizeof(NonVolMemHob));
  //
  // Cannot Build HOB
  //
  if (Hob == NULL) {
    DEBUG((DEBUG_ERROR, "%a() - Failed to create HOB\n",__FUNCTION__));
    ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
  }

  return Status;
}

EFI_STATUS ConfigurePmc( IN NON_VOLATILE_MEM_POLICY_PPI * NonVolPpi)
/*++

 Routine Description:

 Sets up the PMC to support ADR and enables internal resets to initiate ADR flows

 Arguments:

 NonVolPpi - Nonvolatile memory data being maintained

 Returns:

 Status -  EFI_SUCCESS if the interface could be successfully installed.
 EFI_NOT_FOUND if required PPI not found.

 --*/
{
  UINT32 AdrSourceEnables;
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN const PmcXramAddr = PCH_PWRM_BASE_ADDRESS;
  UINT32 Pch_Pm_Sts;
  UINT32 SocAdrRecovery;

  //
  // Setup the PMC registers and get ADR enable/internal sources settings
  //
  Status = PchPwrmBaseSet(PmcXramAddr);
  ASSERT_EFI_ERROR(Status);
  if (NonVolPpi->NonVolMemMode) {
    if (NonVolPpi->NonVolAdrSourcesSet) {
      AdrSourceEnables = NonVolPpi->NonVolAdrSources;
    } else {
      AdrSourceEnables = *((UINT32 volatile *) (PmcXramAddr + ADR_EN));
    }
    AdrSourceEnables |= ADR_FEAT_EN;

    Pch_Pm_Sts = *((UINT32 volatile *) (PmcXramAddr + PCH_PM_STS));
    SocAdrRecovery = (0 != (Pch_Pm_Sts & (1 << ADR_RST_STS)));

    DEBUG(
        (DEBUG_INFO, "%a()  PCH_PM_STS:%8x; ADR_EN: %x<=%x ",__FUNCTION__, Pch_Pm_Sts, *((UINT32 volatile *)(PmcXramAddr+ADR_EN)), AdrSourceEnables));
// //duncan   if (0 == SocAdrRecovery) {
//    if(!(AdrCfg_Active())){ //duncan
      *((UINT32 volatile *) (PmcXramAddr + ADR_EN)) = AdrSourceEnables;
//    } else {
//      DEBUG((DEBUG_INFO, "%a()  skip setting ADR_FEAT_EN\n",__FUNCTION__ ));
//    }
      DEBUG(
          (DEBUG_INFO, "..  %x\n", *((UINT32 volatile *)(PmcXramAddr+ADR_EN)) ));
  }

  DEBUG((DEBUG_INFO, "%a()end\n", __FUNCTION__));
  return Status;
}

EFI_STATUS ConfigureCacheFlush( IN NON_VOLATILE_MEM_POLICY_PPI * NonVolPpi)
/*++

 Routine Description:

 Sets up the PMC to support ADR and enables internal resets to initiate ADR flows

 Arguments:

 NonVolPpi - Non-volatile memory data being maintained

 Returns:

 Status -  EFI_SUCCESS if the interface could be successfully installed.
 EFI_NOT_FOUND if required PPI not found.

 --*/
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN PtrMCHBAR;
  UINT32 AdrCommand;

  //
  // Get MCHBAR
  //
  PchMchBarBaseGet(&PtrMCHBAR);
  ASSERT((UINTN)NULL != PtrMCHBAR);
  DEBUG((EFI_D_INFO, "%a() PtrMCHBAR: %x \n", __FUNCTION__,PtrMCHBAR));

  //
  // Tell PUnit's P_CR_ADR_COMMAND_0_0_0_MCHBAR the caches to be flushed
  //
  AdrCommand = *(UINT32*)(PtrMCHBAR + P_CR_ADR_COMMAND_0_0_0_MCHBAR);
  DEBUG((EFI_D_INFO, "%a() %x => PtrMCHBAR[%x]: %x ", __FUNCTION__, NonVolPpi->NonVolMemCacheFlushing, P_CR_ADR_COMMAND_0_0_0_MCHBAR, AdrCommand));
  if ( BitAt(AdrCommand, L2_FLUSH_BIT) != NonVolPpi->NonVolMemCacheFlushing ) {
    // Denverton Aptiov override start
    //  AdrCommand &= ~(UINT32)(1<<L2_FLUSH_BIT);
    //  AdrCommand |= (NonVolPpi->NonVolMemCacheFlushing & 1) << L2_FLUSH_BIT;
    AdrCommand &= ~(UINT32)(LShiftU64 ( 1, L2_FLUSH_BIT ));
    AdrCommand |= LShiftU64 ((NonVolPpi->NonVolMemCacheFlushing & 1), L2_FLUSH_BIT);
    // Denverton Aptiov override end
    *(UINT32*)(PtrMCHBAR + P_CR_ADR_COMMAND_0_0_0_MCHBAR) = AdrCommand;
  }
  DEBUG((EFI_D_INFO, "=> %x %x\n", AdrCommand, (*(UINT32*)(PtrMCHBAR + P_CR_ADR_COMMAND_0_0_0_MCHBAR)) ));

  return Status;
}

/**
 Routine Description:

 This function sets the MRC's pointer to the CKE latch control GPIO.

 Arguments:

 PeiServices   General purpose services available to every PEIM.
 SaPolicyPpi    The SA Platform Policy PPI instance

 Returns:

 EFI_SUCCESS   Operation success.
 **/
EFI_STATUS PassCkeLatchGpioToMrc(
IN CONST EFI_PEI_SERVICES **PeiServices) {
  EFI_STATUS Status = EFI_SUCCESS;
  SI_SA_POLICY_PPI *SiSaPolicyPpi;
  NV_MEMORY_CONFIGURATION *NonVolMemConfig;

  ///
  /// Get platform policy settings through the SaPolicy PPI
  ///
  Status = (*PeiServices)->LocatePpi(PeiServices, &gSiSaPolicyPpiGuid, 0,
  NULL, &SiSaPolicyPpi);
  ASSERT_EFI_ERROR(Status);

  Status = GetConfigBlock((CONFIG_BLOCK_STRUCT *) SiSaPolicyPpi,
      (UINT16) EnumNvMemConfigId, (VOID *) &NonVolMemConfig);
  ASSERT_EFI_ERROR(Status);

  NonVolMemConfig->CkeLatchGpioCtrl = gpioPadCfgBaseOf(CKE_LATCH_GPIO);
  DEBUG((DEBUG_INFO, "%a() CkeLatchGpioCtrl base:%x \n", __FUNCTION__, NonVolMemConfig->CkeLatchGpioCtrl ));

  return Status;
}

/**
 Routine Description:

 This function transfers the Setup menu settings to the NonVolatile Memory Policy.

 Arguments:

 PeiServices   General purpose services available to every PEIM.
 SaPolicyPpi    The SA Platform Policy PPI instance

 Returns:

 EFI_SUCCESS   Operation success.
 **/
VOID UpdateAdrRecoveryDetected(
IN CONST EFI_PEI_SERVICES **PeiServices,
    IN NON_VOLATILE_MEM_POLICY_PPI * NonVolPpi

    ) {
  //
  // PMC indication if ADR reset has occurred is in PCH_PM_STS.ADR_RST_STS
  //
  UINT32 PwrmBase;
  UINT32_REG PmSts;
  UINT8 SocAdrRecovery;
  UINT8 CpldAdrRecovery;
  EFI_STATUS Status = EFI_SUCCESS;
  SI_SA_POLICY_PPI *SiSaPolicyPpi;
  NV_MEMORY_CONFIGURATION *NonVolMemConfig;
  PLATFORM_INFO_HOB *PlatformInfoHob = NULL;

  ///
  /// Get platform policy settings through the SaPolicy PPI
  ///
  Status = (*PeiServices)->LocatePpi(PeiServices, &gSiSaPolicyPpiGuid, 0,
  NULL, &SiSaPolicyPpi);
  ASSERT_EFI_ERROR(Status);

  Status = GetConfigBlock((CONFIG_BLOCK_STRUCT *) SiSaPolicyPpi,
      (UINT16) EnumNvMemConfigId, (VOID *) &NonVolMemConfig);
  ASSERT_EFI_ERROR(Status);

  CpldAdrRecovery = (0 != AdrCfg_Active());
  DEBUG(
      (DEBUG_INFO, "%a() - CpldAdrRecovery:%x \n", __FUNCTION__, CpldAdrRecovery ));

  Status = PchPwrmBaseGet(&PwrmBase);
  ASSERT_EFI_ERROR(Status);
  PmSts = (UINT32_REG) (PwrmBase + PCH_PM_STS);
  SocAdrRecovery = (0 != (*PmSts & (1 << ADR_RST_STS)));
  DEBUG(
      (DEBUG_INFO, "%a() - PWRMBASE:%x->PCH_PM_STS:%x(%x).ADR_RST_STS(%x) ADR_EN:%x(%x) SocAdrRecovery:%x\n", __FUNCTION__, PwrmBase, PmSts, *PmSts, (*PmSts & (1 << ADR_RST_STS)), ( PwrmBase + ADR_EN ), *(UINT32_REG)( PwrmBase + ADR_EN ), SocAdrRecovery ));

  {
    //
    // Obtain Platform Info from HOB.
    //
    Status = GetPlatformInfo(&PlatformInfoHob);
    ASSERT_EFI_ERROR(Status);

    if (PlatformInfoHob == NULL) {
      DEBUG ((EFI_D_ERROR, "UpdateAdrRecoveryDetected() - PlatformInfoHob pointer is null!\n"));
      return;
    }

    if (BOARD_ID_CormorantLake !=  PlatformInfoHob->BoardId){
      //all boards but ComorantLake must use the SoC ADR status
      NonVolPpi->NonVolMemAdrStatusSrc = 1;
    }

    if ( 0 == NonVolPpi->NonVolMemAdrStatusSrc ) {
      NonVolMemConfig->AdrRecoveryDetected = CpldAdrRecovery;
    } else {
      NonVolMemConfig->AdrRecoveryDetected = SocAdrRecovery;
    }

     if (1 == NonVolPpi->NonVolMemTestMode){
       NonVolMemConfig->AdrRecoveryDetected = 1;
     }
   }
  DEBUG(
      (DEBUG_INFO, "%a() - AdrRecoveryDetected:%x\n", __FUNCTION__, NonVolMemConfig->AdrRecoveryDetected));

}

EFI_STATUS
EFIAPI
NonVolatileMemPeiInit( IN EFI_PEI_FILE_HANDLE FfsHeader,
IN CONST EFI_PEI_SERVICES **PeiServices)
/*++

 Routine Description:

 Perform the platform specific initializations.

 Arguments:

 FfsHeader   - FFS file header pointer of this driver.
 PeiServices - General purpose services available to every PEIM.

 Returns:

 Status -  EFI_SUCCESS if the interface could be successfully installed.
 EFI_NOT_FOUND if required PPI not found.

 --*/
{
  EFI_STATUS Status = EFI_SUCCESS;
  NON_VOLATILE_MEM_POLICY_PPI *NonVolatileMemPpi = NULL;

  DEBUG((DEBUG_INFO, "%a()\n",__FUNCTION__));
  Status = (**PeiServices).LocatePpi(PeiServices, &gNonVolatileMemPpiGuid, 0,
  NULL, (VOID **) &NonVolatileMemPpi);
  ASSERT_EFI_ERROR(Status);

  if (NonVolatileMemPpi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ASSERT(NONVOL_INVALID > NonVolatileMemPpi->NonVolMemMode);
  AdrCfg_Arm(NVMtoCpldModeMap[NonVolatileMemPpi->NonVolMemMode]);

  Status = (**PeiServices).NotifyPpi(PeiServices, mHobNotifyList);
  ASSERT_EFI_ERROR(Status);

  //
  // done if none of the Non-volatile modes are enabled
  //
  if (NONVOL_DISABLED == NonVolatileMemPpi->NonVolMemMode) {
    return Status;
  }

  ConfigurePmc(NonVolatileMemPpi);
  ConfigureCacheFlush(NonVolatileMemPpi);
  PassCkeLatchGpioToMrc(PeiServices);
  UpdateAdrRecoveryDetected(PeiServices, NonVolatileMemPpi);

  //
  // Only need to be notified that MRC is complete in C2F mode
  //
  if (NONVOL_C2F_MODE(NonVolatileMemPpi->NonVolMemMode)) {
    // Setup call backs after memory is available
    Status = (**PeiServices).NotifyPpi(PeiServices, mC2fNotifyList);
    ASSERT_EFI_ERROR(Status);
    DEBUG(
        (DEBUG_INFO, "%a() - Scheduled post C2F memory init handler\n",__FUNCTION__));
  }

  //
  // Only need to be notified that MRC is complete in Non-volatile memeory mode is enabled
  //
  if (NONVOL_DISABLED != NonVolatileMemPpi->NonVolMemMode) {
    // Setup call backs after memory is available
    Status = (**PeiServices).NotifyPpi(PeiServices, mAdrNotifyList);
    ASSERT_EFI_ERROR(Status);
    DEBUG(
        (DEBUG_INFO, "%a() - Scheduled post ADR memory init handler\n",__FUNCTION__));
  }

  return Status;
}

