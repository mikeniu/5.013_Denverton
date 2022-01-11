/** @file
    Source code file for Silicon Init Post Memory module.

@copyright
 Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification
**/

#include "SiInit.h"
#include <Library/PerformanceLib.h>
#include <Library/PcdLib.h>

/**
  Silicon Init End of PEI callback function. This is the last change before entering DXE and OS when S3 resume.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_STATUS       - Always return EFI_SUCCESS
**/
EFI_STATUS
SiInitOnEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
  );

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_NOTIFY_DESCRIPTOR  mSiInitNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiEndOfPeiSignalPpiGuid,
    SiInitOnEndOfPei
  }
};

/**
  Silicon Init End of PEI callback function. This is the last change before entering DXE and OS when S3 resume.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_STATUS       - Always return EFI_SUCCESS
**/
EFI_STATUS
SiInitOnEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
  )
{
  PCIE_IP_GLOBAL_POLICY_PPI       *PcieIpGlobalPolicy;
  IQAT_IP_GLOBAL_POLICY_PPI       *IqatIpGlobalPolicy;
#ifndef FSP_FLAG
  RAS_GLOBAL_POLICY_PPI           *RasGlobalPolicyPpi;
#endif //!FSP_FLAG
  EFI_STATUS                      Status;
  DEBUG ((DEBUG_INFO, "SiInitOnEndOfPei - Start\n"));

  //
  // Locate installed PCIE Policy PPI
  //
  Status = PeiServicesLocatePpi (
         &gPcieIpPolicyPpiGuid,
         0,
         NULL,
         (VOID **) &PcieIpGlobalPolicy
         );
  ASSERT_EFI_ERROR( Status  );
  PcieIpPolicySetHob ( PcieIpGlobalPolicy );

  //
  // Locate installed IQAT Policy PPI
  //
  Status = PeiServicesLocatePpi (
         &gIqatIpPolicyPpiGuid,
         0,
         NULL,
         (VOID **) &IqatIpGlobalPolicy
         );
  ASSERT_EFI_ERROR( Status  );
  IqatIpPolicySetHob ( IqatIpGlobalPolicy );

#ifndef FSP_FLAG
  //
  // Locate installed RAS Policy PPI
  //
  Status = PeiServicesLocatePpi (
     &gRasPolicyPpiGuid,
     0,
     NULL,
     (VOID **) &RasGlobalPolicyPpi
     );
  ASSERT_EFI_ERROR( Status  );
  PeiRasPolicySetHob ( RasGlobalPolicyPpi );
#endif //!FSP_FLAG


  //
  // Initializes PCH after End of Pei
  //
  PchOnEndOfPei ();
  Status = PmcStPgLock ();
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "SiInitOnEndOfPei - End\n"));

  return EFI_SUCCESS;
}

/**
  The function configures the active cores in the system.

  @param[in]     ActiveCores            Indicate how many cores needs to be enabled via the setup option
  @param[in]     CoreExistsMask         Indicate how many cores exits in the SoC
  @param[in out] CoreDisableMask        This variable has the actual number of cores enabled in the system

  @retval        EFI_SUCCESS            The required settings programmed successfully
**/
VOID
SetCoreDisableMask (
IN       UINT8   ActiveCores,
IN       UINT32  CoreExistsMask,
IN  OUT  UINT32  *CoreDisableMask
  )
{
  UINT32 ActiveCoresMask;
  UINT32 CoreCounter;
  UINT32 BitMask;
  UINT8  Odd = 1;
  UINT8  Even = 2;
  UINTN  PtrMCHBAR;

  PtrMCHBAR = 0;
  BitMask = Odd;
  ActiveCoresMask = 0;
  for ( CoreCounter = 0; CoreCounter < ActiveCores;) {
    if (CoreExistsMask & BitMask) {
      ActiveCoresMask |= BitMask;
      CoreCounter++;
    }
    if ( BitMask == 0x4000 ) {
      BitMask = Even;
    } else if ( BitMask == 0x8000 ) {
      BitMask = Odd;
    } else {
      BitMask <<= 2;
    }
  }

  PchMchBarBaseGet(&PtrMCHBAR);
  ASSERT((UINTN)NULL != PtrMCHBAR);
  *CoreDisableMask = 0xFFFF & (~ActiveCoresMask);
  MmioWrite32( (UINTN) (PtrMCHBAR + PCH_PCR_CORE_DISABLE_MASK_MCHBAR), *CoreDisableMask);
  DEBUG ((DEBUG_INFO | DEBUG_INIT, "Set PCH_PCR_CORE_DISABLE_MASK_MCHBAR = %08x\n", (UINT32)MmioRead32((UINTN)(PtrMCHBAR + PCH_PCR_CORE_DISABLE_MASK_MCHBAR))));
}

/**
  The function performs Core Disable programming.

  @param[in] PchPolicyPpi       The PCH Policy PPI instance

  @retval    EFI_SUCCESS        The required settings programmed successfully
**/
EFI_STATUS
SetActiveCores (
  IN PCH_POLICY_PPI               *PchPolicyPpi
  )
{
  //
  // Variables definitions
  //
  UINT32             CoreExistsMask;
  UINT32             CoreDisableMask;
  UINT32             ActiveCoresMask;
  UINT32             BitMask;
  UINT8              ActiveCoresSetupValue;
  UINT8              ActiveCores;
  UINT8              ExistingCores;
  UINT8              Counter;
#ifndef FSP_FLAG
  BOOLEAN            IsFirstBoot;
#endif
  UINTN              PtrMCHBAR;
  PCH_RESET_PPI      *mPchReset;
  EFI_STATUS         Status;
  UINT32             OrigCoreDisableMask;
  UINT32             OrigActiveCoresMask;

  //
  // Variables initialization
  //
#ifndef FSP_FLAG
  IsFirstBoot = PcdGetBool(PcdBootState);
#endif
  PtrMCHBAR   = 0;
  OrigCoreDisableMask = 0xDEAD;
  OrigActiveCoresMask = 0xBEEF;
  CoreDisableMask = 0;
  //
  // Get MCHBAR
  //
  PchMchBarBaseGet(&PtrMCHBAR);
  ASSERT((UINTN)NULL != PtrMCHBAR);

  //
  // Get PCH RESET policy.
  //
  Status = PeiServicesLocatePpi(&gPchResetPpiGuid, 0, NULL, &mPchReset);
  ASSERT_EFI_ERROR (Status);
  //
  // Get Active Cores Setup Option Value
  //
  ActiveCoresSetupValue = PchPolicyPpi->PmConfig.ActiveCoreCount;
  //
  // Get Core Exist Mask
  //
  CoreExistsMask = (UINT32)MmioRead32( (UINTN) (PtrMCHBAR + PCH_PCR_CORE_EXISTS_VECTOR_MCHBAR) );
  if ( PEI_IN_SIMICS ) {
    if (CoreExistsMask == 0) 
      CoreExistsMask |= 0x0000FFFF;  // Simics Overide for testing
  }
  
#ifdef FSP_FLAG
  // NOTE: IsFirstBoot/PcdGetBool is a BIOS artifact that requires persistent storage between successive boots,
  //       which FSP does not have.  Additional code has been added as a substitute to determine if a change
  //       is required to the core disable mask register. (See OrigCoreDisableMask and OrigActiveCoresMask)
  //
  // ActiveCores is calculated here in FSP only code as it gets clobbered by SetCoreDisableMask() call in 
  // IsFirstBoot logic.
  //
  // Get # of currently active cores
  //
  CoreDisableMask =  MmioRead32( (UINTN) (PtrMCHBAR + PCH_PCR_CORE_DISABLE_MASK_MCHBAR));
  ActiveCoresMask = (~CoreDisableMask) & CoreExistsMask;
  for (BitMask = 1, ActiveCores = 0, Counter = 0; Counter < (MAX_NUM_MODULES * MAX_NUM_CORES_PMODULE); Counter++ ) {
    if ( ActiveCoresMask & BitMask ) {
      ActiveCores++;
    }
    BitMask <<= 1;
  }
  DEBUG ((DEBUG_INFO | DEBUG_INIT, __FUNCTION__ ": ActiveCores:%X\n", ActiveCores));
#endif

  //
  // Calculate Number of Existing Cores
  //
  for (BitMask = 1, ExistingCores = 0, Counter = 0; Counter < (MAX_NUM_MODULES * MAX_NUM_CORES_PMODULE); Counter++ ) {
    if ( CoreExistsMask & BitMask ) {
      ExistingCores++;
    }
    BitMask <<= 1;
  }
  DEBUG ((DEBUG_INFO, __FUNCTION__": ExistingCores:%X\n", ExistingCores));
  //
  // Verify if the Active Cores Setup Option Value is valid
  //
  if (ActiveCoresSetupValue > ExistingCores) {
    //
    // Adjusting Setup Option value locally to the maximum possible
    //
    ActiveCoresSetupValue = ExistingCores;
    DEBUG ((DEBUG_INFO, "WARNING: Chosen value for setup option 'Active Processor Cores' exceeds the number of\n"));
    DEBUG ((DEBUG_INFO, "         existing cores in the SoC. Existing Processor Cores = 0x%x\n", ExistingCores));
#ifndef FSP_FLAG
    if (!IsFirstBoot) {
      return EFI_INVALID_PARAMETER;
    }
#else
    return EFI_INVALID_PARAMETER;
#endif
    }
// Denverton AptioV Override Start - EIP#292932    
// IsFirstBoot is not being updated correctly
// Comment code that checks this item and use the FSP method above
// That code was already after this check
/*
#ifndef FSP_FLAG
  //
  // If First Boot: Set Default Values
  //
  if (IsFirstBoot) {
    DEBUG ((DEBUG_INFO, __FUNCTION__ ": First Boot found: Set Default Core Disable Mask Value\n"));
    //
    // Set Default Core Disable Mask Value
    //
    SetCoreDisableMask(ExistingCores,CoreExistsMask,&CoreDisableMask);
    //
    // Determine Current Active Cores Mask based on register values
    //
    ActiveCoresMask = (~CoreDisableMask) & CoreExistsMask;
    //
    // Calculate Number of Active Cores
    //
    for (BitMask = 1, ActiveCores = 0, Counter = 0; Counter < (MAX_NUM_MODULES * MAX_NUM_CORES_PMODULE); Counter++ ) {
      if ( ActiveCoresMask & BitMask ) {
        ActiveCores++;
      }
      BitMask <<= 1;
    }
    return EFI_SUCCESS;
  }
#endif
*/
// Denverton AptioV Override End - EIP#292932    

  //
  // Get Core Disable Mask
  //
  CoreDisableMask = MmioRead32( (UINTN) (PtrMCHBAR + PCH_PCR_CORE_DISABLE_MASK_MCHBAR) );
  //
  // Determine Current Active Cores Mask based on register values
  //
  ActiveCoresMask = (~CoreDisableMask) & CoreExistsMask;
  DEBUG ((DEBUG_INFO, __FUNCTION__ ": Initial CoreDisableMask:%X  ActiveCoresMask:%X\n", CoreDisableMask, ActiveCoresMask));
  //
  // Save original values for later use to bypass reset
  //
  OrigCoreDisableMask = CoreDisableMask;
  OrigActiveCoresMask = ActiveCoresMask;
  
  //
  // Calculate Number of Active Cores
  //
  for (BitMask = 1, ActiveCores = 0, Counter = 0; Counter < (MAX_NUM_MODULES * MAX_NUM_CORES_PMODULE); Counter++ ) {
    if ( ActiveCoresMask & BitMask ) {
      ActiveCores++;
    }
    BitMask <<= 1;
  }
  DEBUG ((DEBUG_INFO, __FUNCTION__ ": ActiveCores:%X\n", ActiveCores));
  //
  // Verify if there was a change in the Active Cores Setup Option
  //
  if (ActiveCores != ActiveCoresSetupValue) {
    if (!ActiveCoresSetupValue) {
      DEBUG ((DEBUG_INFO, "Setup option 'Active Processor Cores' is %x. All existing cores are active\n", ActiveCoresSetupValue));
      ActiveCoresSetupValue = ExistingCores;
    } else {
      DEBUG ((DEBUG_INFO, __FUNCTION__ ": Change detected! ActiveCores:%X  != ActiveCoresSetupValue:%X\n", ActiveCores, ActiveCoresSetupValue));
    }
    //
    // Set New Core Disable Mask Value
    //
    SetCoreDisableMask(ActiveCoresSetupValue,CoreExistsMask,&CoreDisableMask);
    //
    // Determine New Active Cores based on Register values
    //
    ActiveCoresMask = (~CoreDisableMask) & CoreExistsMask;
    //
    // Calculate New Number of Active Cores
    //
    for (BitMask = 1, ActiveCores = 0, Counter = 0; Counter < (MAX_NUM_MODULES * MAX_NUM_CORES_PMODULE); Counter++ ) {
      if ( ActiveCoresMask & BitMask ) {
        ActiveCores++;
      }
      BitMask <<= 1;
    } 
    DEBUG ((DEBUG_INFO, __FUNCTION__ ": New # of ActiveCores:%X  ActiveCoresSetupValue:%X\n", ActiveCores, ActiveCoresSetupValue));
    //
    // Trigger a Power Cycling Reset By Writing 0x0E to IO 0xCF9
    //
    DEBUG ((DEBUG_INFO, __FUNCTION__ ": Requesting a Reset to make changes have effect\n"));
    if ( !PEI_IN_SIMICS ) {
      if (ActiveCoresSetupValue != ExistingCores) {
        DEBUG ((DEBUG_INFO | DEBUG_INIT, __FUNCTION__ ": Change detected! ColdReset - ActiveCoresSetupValue:%X  CoreDisableMask:%X\n", ActiveCoresSetupValue, CoreDisableMask));
        mPchReset->ResetEndOfPei (mPchReset, ColdReset);
      } else {
        if ( (OrigActiveCoresMask != ActiveCoresMask) && (OrigCoreDisableMask != 0) ) {
          DEBUG ((DEBUG_INFO | DEBUG_INIT, __FUNCTION__ ": Change detected! ColdReset - ActiveCoresSetupValue:%X  CoreDisableMask:%X\n", ActiveCoresSetupValue, CoreDisableMask));
          DEBUG ((DEBUG_INFO | DEBUG_INIT, __FUNCTION__ ": OrigCoreDisableMask:%X  OrigActiveCoresMask:%X\n", OrigCoreDisableMask, OrigActiveCoresMask));
          mPchReset->ResetEndOfPei (mPchReset, ColdReset);
        } else {
          DEBUG ((DEBUG_INFO | DEBUG_INIT, __FUNCTION__ ": Change detected! BYPASS ColdReset - ActiveCoresSetupValue:%X  CoreDisableMask:%X\n", ActiveCoresSetupValue, CoreDisableMask));
        }
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  Silicon Initializes after memory services initialized

  @param[in] FileHandle           The file handle of the file, Not used.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             The function completes successfully
  @retval EFI_UNSUPPORTED         The function failed to locate SiPolicy
**/
EFI_STATUS
EFIAPI
SiInit (
  IN  EFI_PEI_FILE_HANDLE               FileHandle,
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  SI_POLICY_PPI      *SiPolicy;
  PCH_POLICY_PPI     *PchPolicy;
  VOID               *HobPtr;
  EFI_STATUS         Status;
  BOOLEAN            StatusBit;

  PCIE_IP_GLOBAL_POLICY_PPI   *PcieIpGlobalPolicy;
  HSUART_IP_GLOBAL_POLICY_PPI *HsuartIpGlobalPolicy;
  SI_SA_POLICY_PPI            *SiSaPolicyPpi;
#ifdef ME_TESTMENU_FLAG
  PEI_ME_PLATFORM_POLICY_PPI  *PeiMePlatformPolicyPpi;
  BOOLEAN                     FiaMuxMessageEnabled = TRUE;
#endif
  VTD_CONFIG                  *VtdConfig;
  BOOLEAN                     GlobalResetRequired;
  PCH_RESET_PPI               *mPchReset;
  DEBUG ((DEBUG_INFO, "SiInit () - Start\n"));
  //
  // Locate installed Silicon Policy PPI
  //
  Status = PeiServicesLocatePpi (
             &gSiPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPolicy
             );
  if (Status != EFI_SUCCESS) {
    //
    // SI_POLICY_PPI must be installed at this point
    //
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  //
  // Locate installed PCH Policy PPI
  //
  Status = PeiServicesLocatePpi (
             &gPchPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &PchPolicy
             );
  if (Status != EFI_SUCCESS) {
    //
    // PCH_POLICY_PPI must be installed at this point
    //
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }


  HobPtr = BuildGuidDataHob (&gSiPolicyHobGuid, SiPolicy, sizeof (SI_POLICY_PPI));
  ASSERT (HobPtr != 0);

  //
  // Get PCH RESET policy.
  //
  Status = PeiServicesLocatePpi(&gPchResetPpiGuid, 0, NULL, &mPchReset);
  ASSERT_EFI_ERROR (Status);

  //
  // Locate installed PCIE Policy PPI
  //
  Status = PeiServicesLocatePpi (
             &gPcieIpPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &PcieIpGlobalPolicy
             );

  DEBUG ((DEBUG_INFO, "Locate PcieIpGlobalPolicy Start\n"));
  if (Status != EFI_SUCCESS) {
    //
    // PCH_POLICY_PPI must be installed at this point
    //
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }
  DEBUG ((DEBUG_INFO, "PcieIpInit Phase0\n"));
  PcieIpInit (PcieIpGlobalPolicy, Phase0);

#ifdef ME_TESTMENU_FLAG
    Status = PeiServicesLocatePpi (
              &gPeiMePlatformPolicyPpiGuid,
              0,
              NULL,
              &PeiMePlatformPolicyPpi
              );

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Cannot get ME Policy PPI. Use module default.\n"));
    } else {
      FiaMuxMessageEnabled = (BOOLEAN)PeiMePlatformPolicyPpi->FiaMuxMessageEnabled;
      DEBUG ((DEBUG_INFO, "FiaMuxConfigMessaging state: %d\n", PeiMePlatformPolicyPpi->FiaMuxMessageEnabled));
    }
#endif

#ifdef ME_TESTMENU_FLAG
  if (FiaMuxMessageEnabled) {
#endif
    //
    // Init FIA MUX in PEI phase
    //
    Status = PeiFiaMuxConfigInit(&GlobalResetRequired);
    if (Status == EFI_UNSUPPORTED) {
      DEBUG((DEBUG_ERROR, "FiaMuxConfig Get or Set command unsupported by current FW\n"));
    }
    else {
      ASSERT_EFI_ERROR (Status);
    }

    if (GlobalResetRequired) {
      //
      // Reset system after
      //
      if ( !PEI_IN_SIMICS ) {
        mPchReset->ResetEndOfPei (mPchReset, ColdReset);
      }
    }
#ifdef ME_TESTMENU_FLAG
  } else {
    DEBUG ((DEBUG_INFO, "FiaMuxConfig disabled \n"));
  }
#endif


  //
  // Locate installed HSUART Policy PPI
  //
   Status = PeiServicesLocatePpi (
              &gHsuartIpPolicyPpiGuid,
              0,
              NULL,
              (VOID **) &HsuartIpGlobalPolicy
              );
   if (Status != EFI_SUCCESS) {
      //
      // PCH_POLICY_PPI must be installed at this point
      //
      ASSERT (FALSE);
      return EFI_UNSUPPORTED;
    }
   HsuartIpInit (HsuartIpGlobalPolicy);

   //
   // Init IQAT in PEI phase
   //
   PeiIqatIpInit ();

  SetActiveCores(PchPolicy);
  
  //
  // Initializes PCH after memory services initialized
  //
  
  // Denverton AptioV override Start - EIP#278393
  // To correct the -ve values in AMIPRD report, by passing same 'identifier' value for START and END macro
  
  // PERF_START_EX (&gPerfPchPostMemGuid,NULL, NULL, AsmReadTsc(), 0x4020);
  PERF_START_EX (NULL, "SiPol", NULL, AsmReadTsc(), 0x4020);
  PchInit (SiPolicy, PchPolicy);
  // PERF_END_EX (&gPerfPchPostMemGuid, NULL, NULL, AsmReadTsc(), 0x4021);
  PERF_END_EX (NULL, "SiPol", NULL, AsmReadTsc(), 0x4020);

  // Denverton AptioV override End - EIP#278393 
  
  DEBUG ((DEBUG_INFO, "PcieIpInit Phase1\n"));
  PcieIpInit (PcieIpGlobalPolicy, Phase1);

  Status = PeiServicesLocatePpi (
             &gSiSaPolicyPpiGuid,
             0,
             NULL,
             (VOID **)&SiSaPolicyPpi
             );
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_STRUCT *)SiSaPolicyPpi, (UINT16)EnumVtdConfigId, (VOID *)&VtdConfig);
  ASSERT_EFI_ERROR (Status);

  VtdInit (VtdConfig);


#ifdef ME_SUPPORT_FLAG
  //
  // Perform ME post mem init
  //
  
  // Denverton AptioV override Start - EIP#278393
  // To correct the -ve values in AMIPRD report, by passing same 'identifier' value for START and END macro
   
  // PERF_START_EX (&gPerfMePostMemGuid, NULL, NULL, AsmReadTsc(), 0x40A0);
  PERF_START_EX (NULL, "MePost", NULL, AsmReadTsc(), 0x40A0);
  MePostMemInit ();
  // PERF_END_EX (&gPerfMePostMemGuid, "KDB", NULL, AsmReadTsc(), 0x40A1);
  PERF_END_EX (NULL, "MePost", NULL, AsmReadTsc(), 0x40A0);
  
  // Denverton AptioV override End - EIP#278393 
  
#endif

  //
  // Performing ST PG of SouthCluster IP blocks
  //
  Status = PmcStPgConfig ();
  ASSERT_EFI_ERROR (Status);
  Status = PmcStPgResetRequired (&StatusBit);
  if (Status == EFI_SUCCESS && StatusBit) {
    if ( !PEI_IN_SIMICS ) {
      mPchReset->ResetEndOfPei (mPchReset, GlobalReset);
    }
  }

  //
  // Install EndOfPei callback function.
  //
  Status = PeiServicesNotifyPpi (mSiInitNotifyList);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "SiInit () - End\n"));
  return EFI_SUCCESS;
}
