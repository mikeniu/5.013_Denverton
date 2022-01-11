/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
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

  BootMode.c
   
Abstract:

  PEIM to provide the platform support functionality on the Sonoma.

--*/
#include "PlatformStage1.h"
#include "Guid/SetupVariable.h"

#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/Capsule.h>
// Denverton Aptiov override start
// #include <Library/CmosAccessLib.h>

UINT8 ReadCmos(
    IN UINT8 Index
);

// Denverton Aptiov override end
#include <CmosMap.h>
#include <Guid/BootState.h>

#define BOOT_SPECIAL_MASK                             0x80

// Denverton AptioV override Start - EIP#252720
// Defined in OEMPort.c
BOOLEAN
IsRecovery(
    EFI_PEI_SERVICES  **PeiServices,
    EFI_PEI_PCI_CFG_PPI  *PciCfg,
    EFI_PEI_CPU_IO_PPI  *CpuIo
);
// Denverton AptioV override End - EIP#252720

//
// Priority of our boot modes, highest priority first
//
EFI_BOOT_MODE mBootModePriority[] = {
  BOOT_IN_RECOVERY_MODE,
  BOOT_WITH_DEFAULT_SETTINGS,
  BOOT_ON_FLASH_UPDATE,
  BOOT_ON_S2_RESUME,
  BOOT_ON_S3_RESUME,
  BOOT_ON_S4_RESUME,
  BOOT_WITH_MINIMAL_CONFIGURATION,
  BOOT_ASSUMING_NO_CONFIGURATION_CHANGES,
  BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS,
  BOOT_WITH_FULL_CONFIGURATION,
  BOOT_ON_S5_RESUME,
  BOOT_SPECIAL_MASK 
};

/*
EFI_PEI_PPI_DESCRIPTOR mPpiListSStateBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiSStateBootModePpiGuid,
  NULL
};
*/

EFI_PEI_PPI_DESCRIPTOR mPpiListRecoveryBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBootInRecoveryModePpiGuid,
  NULL
};

EFI_PEI_PPI_DESCRIPTOR mPpiBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  NULL
};

EFI_PEI_NOTIFY_DESCRIPTOR mBootModeNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiCapsulePpiGuid,
  PlatformStage1UpdateBootMode 
};

EFI_STATUS
PrioritizeBootMode (
  IN OUT EFI_BOOT_MODE    *CurrentBootMode,
  IN EFI_BOOT_MODE        NewBootMode
  );

#pragma warning (push)
#pragma warning (disable : 4090)

BOOLEAN
IsPreviousBootSuccessful(
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
/*++

Routine Description:

  Check CMOS register bit to determine if previous boot was successful
  
Arguments:

  PeiServices   - pointer to the PEI Service Table

Returns:

  TRUE          - Previous Boot was success
  FALSE         - Previous Boot wasn't success
  
--*/
{
  EFI_STATUS                  Status;
  UINT8						  PreviousBootCompletionFlag;
  BOOLEAN                     BootState;
  UINTN                       DataSize;
  CHAR16                      VarName[] = BOOT_STATE_VARIABLE_NAME;
  EFI_PEI_READ_ONLY_VARIABLE_PPI *PeiVar;

  Status = (**PeiServices).LocatePpi (
                                PeiServices,
                                &gEfiPeiReadOnlyVariablePpiGuid,
                                0,
                                NULL,
                                &PeiVar
                                );
  ASSERT_EFI_ERROR (Status);
  
  //
  // Get last Boot State Variable to confirm that it is not a first boot .
  //
  

  DataSize = sizeof (BOOLEAN);
  Status = PeiVar->PeiGetVariable (
                            PeiServices,
                            VarName,
                            &gEfiBootStateGuid,
                            NULL,
                            &DataSize,
                            &BootState
                            );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  PreviousBootCompletionFlag = 0;

  PreviousBootCompletionFlag = ReadCmos (CMOS_FAST_BOOT_REG); // Denverton AptioV override Start
  PreviousBootCompletionFlag &= FAST_BOOT_OR_MASK;

  if(PreviousBootCompletionFlag){
    DEBUG ((DEBUG_INFO, "Previous Boot was not successful to boot to OS\n"));
    return FALSE;
  }
  DEBUG ((DEBUG_INFO, "Previous boot cycle successfully completed handover to OS\n"));
  return TRUE;
}

#pragma warning (pop)

BOOLEAN
IsTimeOutOccurred(
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
/*++

Routine Description:

  Check Fast Boot 30-second WDT (TCO Timer) timeout has occurred
  
Arguments:

  PeiServices   - pointer to the PEI Service Table

Returns:

  TRUE           - Timeout has occurred
  FALSE          - Timeout didnt occur
  
--*/
{
  EFI_PEI_CPU_IO_PPI *CpuIo;
  
  CpuIo    = (**PeiServices).CpuIo;

  if ((CpuIo->IoRead8) (PeiServices, CpuIo, PCH_TCO_BASE_ADDRESS + R_PCH_TCO_WDCNT) == FAST_BOOT_WDT_RESET_SIGNATURE) {
    (CpuIo->IoWrite8) (PeiServices, CpuIo, PCH_TCO_BASE_ADDRESS + R_PCH_TCO_WDCNT, 0); 
    DEBUG ((DEBUG_INFO, "WDT occurred in previous boot\n"));
    return TRUE;
  }
  
  DEBUG ((DEBUG_INFO, "WDT did not occur in previous boot\n"));
  return FALSE;
}

BOOLEAN
IsPowerButtonOverride(
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
/*++

Routine Description:

  Check if this system boot is due to 4sec power button override has occurred
  
Arguments:

  PeiServices   - pointer to the PEI Service Table

Returns:

  TRUE         - Power Button Override occurred in last system boot
  FALSE        - Power Button Override didnt occur
  
--*/
{
  EFI_PEI_CPU_IO_PPI *CpuIo;
  
  CpuIo    = (**PeiServices).CpuIo;

  if (((CpuIo->IoRead16) (PeiServices, CpuIo, PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_STS) & B_PCH_ACPI_PM1_STS_PRBTNOR) != 0) {
    DEBUG ((DEBUG_INFO, "Power Button Override occurred in last system boot\n"));
    return TRUE;
  }

  DEBUG ((DEBUG_INFO, "Power Button Override did not occur\n"));
  return FALSE;
}

BOOLEAN 
IsBootWithNoChange (
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
/*++

Routine Description:

  Query PCH to determine if Intrusion Detection set
  
Arguments:

  PeiServices   - pointer to the PEI Service Table

Returns:

  TRUE          - No Change
  FALSE         - Change

  
--*/
{
  EFI_STATUS                  Status;
  UINT16                      TcoBase;
  UINT16                      Tco2Status;
  BOOLEAN                     BoxOpen;
  BOOLEAN                     BootState;
  UINTN                       DataSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVar;
  EFI_PEI_CPU_IO_PPI          *CpuIo;
  CHAR16                      VarName[] = BOOT_STATE_VARIABLE_NAME;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  CpuIo    = (**PeiServices).CpuIo;

  Status = (**PeiServices).LocatePpi (
                                PeiServices,
                                &gEfiPeiReadOnlyVariable2PpiGuid,
                                0,
                                NULL,
                                &PeiVar
                                );
  ASSERT_EFI_ERROR (Status);
  
  //
  // Get last Boot State Variable From NVRAM
  //
  DataSize = sizeof (BOOLEAN);
  Status = PeiVar->GetVariable (
                            PeiVar,
                            VarName,
                            &gEfiBootStateGuid,
                            NULL,
                            &DataSize,
                            &BootState
                            );
  if (EFI_ERROR (Status)) {
    // If there's no variable, just report the state of the hardware
    BootState = TRUE;
  }
  
  //
  // Read the TCO registers
  //
  TcoBase     = (UINT16)PCH_TCO_BASE_ADDRESS;
  DEBUG ((EFI_D_INFO, "\tTSTS2 = 0x%08x\tValue = 0x%08x\n",
    TcoBase + R_PCH_TCO2_STS,
    (CpuIo->IoRead16) (PeiServices, CpuIo, TcoBase + R_PCH_TCO2_STS)));
  Tco2Status  = (CpuIo->IoRead16) (PeiServices, CpuIo, TcoBase + R_PCH_TCO2_STS);

  //
  // This is the state of the hardware
  //
  BoxOpen = (BOOLEAN) (Tco2Status & B_PCH_TCO2_STS_INTRD_DET);
  if (BoxOpen) {
    DEBUG ((DEBUG_INFO, "\tINTRD_DET is set to %d\n",BoxOpen));
    DEBUG ((EFI_D_INFO, "\tTSTS2 = 0x%08x\tValue = 0x%08x\n",
      TcoBase + R_PCH_TCO2_STS,
      (CpuIo->IoRead16) (PeiServices, CpuIo, TcoBase + R_PCH_TCO2_STS)));

    //
    // Since it was OPEN, return that it cannot be in "no config. change boot"
    //
    DEBUG ((DEBUG_INFO, "Boot with Full configuration\n"));
    DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
    return FALSE;
  } else {
    //
    // Since it was CLOSED, return that can be "no config. change boot"
    // Need variable to know that memory was tested AT LEAST one time!
    //
    if (!BootState) {
      DEBUG ((DEBUG_INFO, "Boot assuming no configuration changes\n"));
      DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
      return TRUE;
    } else {
      DEBUG ((DEBUG_INFO, "Boot with Full configuration\n"));
      DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
      return FALSE;
    }
  }
}

VOID
PlatformStage1InitBootMode (
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  EFI_STATUS                    Status;
  UINT32                        SleepType;
  EFI_BOOT_MODE                 BootMode;
  EFI_PEI_CPU_IO_PPI            *CpuIo;

  PLATFORM_INFO_HOB             *PlatformInfoHob;


  CpuIo = (**PeiServices).CpuIo;

  // Denverton AptioV override Start - EIP#252720    
  (*PeiServices)->GetBootMode (PeiServices, &BootMode);

  if (IsRecovery((EFI_PEI_SERVICES**)PeiServices, (EFI_PEI_PCI_CFG_PPI *)(*PeiServices)->PciCfg, CpuIo)){
    // IsRecovery returns TRUE - Recovery, FALSE - Not in Recovery
    //if (Status == EFI_SUCCESS) {
    BootMode = BOOT_IN_RECOVERY_MODE;
    Status = (**PeiServices).InstallPpi (PeiServices, &mPpiListRecoveryBootMode);
    ASSERT_EFI_ERROR (Status);
  } else {
    /*if (Status == EFI_UNSUPPORTED) {
      BootMode = BOOT_WITH_DEFAULT_SETTINGS;
    }*/
    // Denverton AptioV override End - EIP#252720
    if (GetSleepTypeAfterWakeup (&SleepType)) {
      switch (SleepType) {
       case V_PCH_ACPI_PM1_CNT_S3:
        BootMode = BOOT_ON_S3_RESUME;
        break;
       case V_PCH_ACPI_PM1_CNT_S4:
        BootMode = BOOT_ON_S4_RESUME;
        break;
       case V_PCH_ACPI_PM1_CNT_S5:
        BootMode = BOOT_ON_S5_RESUME;
        break;
      }
    }
    


	GetPlatformInfo (&PlatformInfoHob);

    if (NULL == PlatformInfoHob) {
      DEBUG(( DEBUG_ERROR, "FAIL to get Platform info\n"));
    } else {
      if (PlatformInfoHob->RtcWeak) {
        //
        // Report RTC battery failure
        //
        DEBUG(( DEBUG_ERROR, "RTC Power failure !! Could be due to a weak or missing battery.\n"));
        DEBUG ((DEBUG_ERROR, "BootMode is set to BOOT_WITH_DEFAULT_SETTINGS\n"));
        BootMode = BOOT_WITH_DEFAULT_SETTINGS;
      }
      if (PlatformInfoHob->MfgMode) {
        DEBUG(( DEBUG_ERROR, "BootMode set in BOOT_WITH_MFG_MODE_SETTINGS\n"));
        BootMode = BOOT_WITH_MFG_MODE_SETTINGS;
      }
      if (PlatformInfoHob->NvClear) {
        DEBUG(( DEBUG_ERROR, "BootMode set in BOOT_WITH_DEFAULT_SETTINGS\n"));
        BootMode = BOOT_WITH_DEFAULT_SETTINGS;
      }
    }
  }
  (**PeiServices).SetBootMode (PeiServices, BootMode);

  //
  // Register notify to update BootMode later
  //
  Status = (**PeiServices).NotifyPpi (PeiServices, &mBootModeNotifyList);
  ASSERT_EFI_ERROR (Status);

  return ;
}

#pragma warning (push)
#pragma warning (disable : 4090)

EFI_STATUS
EFIAPI
PlatformStage1UpdateBootMode (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Interface
  )
{
  EFI_STATUS                    Status;
  PEI_CAPSULE_PPI               *Capsule;
  EFI_BOOT_MODE                 BootMode;
  UINT8                         FastBootRegData;
#ifdef RAPID_START_FLAG
  UINT8                         RapidStartMode;
#endif

  FastBootRegData   = 0;

  (*PeiServices)->GetBootMode (PeiServices, &BootMode);

  //
  // Determine if we're in capsule update mode
  //
  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gPeiCapsulePpiGuid,
                             0,
                             NULL,
                             (VOID **)&Capsule
                             );
  if (Status == EFI_SUCCESS) {
    Status = Capsule->CheckCapsuleUpdate (PeiServices);
    if (Status == EFI_SUCCESS) {
      PrioritizeBootMode (&BootMode, BOOT_ON_FLASH_UPDATE);
    }
  }

  if ((BootMode != BOOT_ON_S5_RESUME) && IsBootWithNoChange (PeiServices)) {
    PrioritizeBootMode (&BootMode, BOOT_ASSUMING_NO_CONFIGURATION_CHANGES);
  } else {
    PrioritizeBootMode (&BootMode, BOOT_WITH_FULL_CONFIGURATION);
  }

  Status = (**PeiServices).SetBootMode (PeiServices, BootMode);
  ASSERT_EFI_ERROR (Status);
    
  //
  // Signal possible dependent modules that there has been a 
  // final boot mode determination, it is used to build BIST
  // Hob for Dxe use.
  //
  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiBootMode);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

#pragma warning (pop)

EFI_STATUS
PrioritizeBootMode (
  IN OUT EFI_BOOT_MODE    *CurrentBootMode,
  IN EFI_BOOT_MODE        NewBootMode
  )
/*++

Routine Description:

  Given the current boot mode, and a proposed new boot mode, determine
  which has priority. If the new boot mode has higher priority, then
  make it the current boot mode.
  
Arguments:

  CurrentBootMode   - pointer to current planned boot mode
  NewBootMode       - proposed boot mode

Returns:

  EFI_NOT_FOUND     - if either boot mode is not recognized
  EFI_SUCCESS       - if both boot mode values were recognized and
                      were processed.
  
--*/
{
  UINT32 CurrentIndex;
  UINT32 NewIndex;
  
  //
  // Find the position of the current boot mode in our priority array
  //
  for ( CurrentIndex = 0; 
        CurrentIndex < sizeof (mBootModePriority) / sizeof (mBootModePriority[0]); 
        CurrentIndex++) {
    if (mBootModePriority[CurrentIndex] == *CurrentBootMode) {
      break;
    }
  }
  if (CurrentIndex >= sizeof (mBootModePriority) / sizeof (mBootModePriority[0])) {
    return EFI_NOT_FOUND;
  }
  //
  // Find the position of the new boot mode in our priority array
  //
  for ( NewIndex = 0; 
        NewIndex < sizeof (mBootModePriority) / sizeof (mBootModePriority[0]); 
        NewIndex++) {
    if (mBootModePriority[NewIndex] == NewBootMode) {
      //
      // If this new boot mode occurs before the current boot mode in the
      // priority array, then take it.
      //
      if (NewIndex < CurrentIndex) {
        *CurrentBootMode = NewBootMode;
      }
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

