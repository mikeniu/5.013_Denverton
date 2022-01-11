//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SbRunSmm.c
    This file contains code for SouthBridge runtime SMM
    protocol

*/

// Module specific Includes

#include "Efi.h"
#include "token.h"
#include <AmiDxeLib.h>
#include <Library/AmiChipsetRuntimeServiceLib.h>

// Denverton Aptiov Override Start - EIP#210294 
#include <PchAccess.h>
#include <Protocol/PchReset.h>
#include <AmiCspLib.h>

VOID
EFIAPI
SmmRtResetSystem (
  IN PCH_RESET_TYPE      PchResetType,
  IN EFI_STATUS          ResetStatus,
  IN UINTN               DataSize,
  IN CHAR16              *ResetData OPTIONAL
  )
{
  UINT16              ABase;
  UINT8               OutputData;
  UINT32              Data32;
  UINT16              Data16;
  UINTN               PmcBaseAddress;

  PmcBaseAddress    = CSP_PCIE_CFG_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, 0);
  ABase             = MmioRead16 (PmcBaseAddress + R_PCH_PMC_ACPI_BASE) & B_PCH_PMC_ACPI_BASE_BAR;

  switch (PchResetType) {
  case WarmReset:
    IoWrite8 ((UINTN) R_PCH_RST_CNT, (UINT8) V_PCH_RST_CNT_SOFTSTARTSTATE);
    OutputData = V_PCH_RST_CNT_SOFTRESET;
    break;

  case ColdReset:
    IoWrite8 ((UINTN) R_PCH_RST_CNT, (UINT8) V_PCH_RST_CNT_HARDSTARTSTATE);
    OutputData = V_PCH_RST_CNT_HARDRESET;
    break;

  case ShutdownReset:
    ///
    /// Firstly, ACPI decode must be enabled
    ///
    MmioOr8 (
      PmcBaseAddress + R_PCH_PMC_ACPI_CNT,
      (UINT8) (B_PCH_PMC_ACPI_CNT_ACPI_EN)
      );

    ///
    /// Then, GPE0_EN should be disabled to avoid any GPI waking up the system from S5
    ///
    IoWrite32 ((UINTN) (ABase + R_PCH_ACPI_GPE0_EN_127_96), 0);

    ///
    /// Secondly, PwrSts register must be cleared
    ///
    /// Write a "1" to bit[8] of power button status register at
    /// (PM_BASE + PM1_STS_OFFSET) to clear this bit
    ///
    Data16 = B_PCH_ACPI_PM1_STS_PWRBTN;
    IoWrite16 ((UINTN) (ABase + R_PCH_ACPI_PM1_STS), Data16);

    //
    // Disable SMI on S5 sleep state. This allows ayatem to go to S5.
    //
	
	// Denverton Aptiov Override Start - EIP#364364
	// To avoid disabling of SMI on S5 sleep state and thus avoiding system entering into S5 state
 	// This mechanism allows all the handlers registered for SMI to be serviced and then system is 
  	// put into S5 sleep state.
	
    // IoWrite32 (ABase + R_PCH_SMI_EN,(UINT32)(IoRead32 (ABase + R_PCH_SMI_EN) & (~B_PCH_SMI_EN_ON_SLP_EN)));
	// Denverton Aptiov Override End - EIP#364364
  
    ///
    /// Finally, transform system into S5 sleep state
    ///
    Data32  = IoRead32 ((UINTN) (ABase + R_PCH_ACPI_PM1_CNT));

    Data32  = (UINT32) ((Data32 &~(B_PCH_ACPI_PM1_CNT_SLP_TYP + B_PCH_ACPI_PM1_CNT_SLP_EN)) | V_PCH_ACPI_PM1_CNT_S5);

    IoWrite32 ((UINTN) (ABase + R_PCH_ACPI_PM1_CNT), Data32);

    Data32 = Data32 | B_PCH_ACPI_PM1_CNT_SLP_EN;

    IoWrite32 ((UINTN) (ABase + R_PCH_ACPI_PM1_CNT), Data32);

    return;

  case PowerCycleReset:
  case GlobalReset:
  case GlobalResetWithEc:
    ///
    /// PCH BIOS Spec Section 4.6 GPIO Reset Requirement
    ///

    if ((PchResetType == GlobalReset) || (PchResetType == GlobalResetWithEc)) {
      MmioOr32 (
        PmcBaseAddress + R_PCH_PMC_ETR3,
        (UINT32) (B_PCH_PMC_ETR3_CF9GR)
        );
    }

    OutputData = V_PCH_RST_CNT_FULLRESET;
    break;

  default:
    return;
  }

  DEBUG ((DEBUG_ERROR, "Resetting the platform (%02x)...\n", OutputData));
  IoWrite8 ((UINTN) R_PCH_RST_CNT, OutputData);
  ///
  /// Waiting for system reset
  ///
  CpuDeadLoop ();

  return;
}
// Denverton Aptiov Override End - EIP#210294 
/**
    Submit runtime services both SMM and runtime.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS
SbRuntimeServiceInit (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
 // Denverton Aptiov Override Start - EIP#210294 
    EFI_GUID                 SmmRsTableGuid = EFI_SMM_RUNTIME_SERVICES_TABLE_GUID;
    UINT8                    Index;
    EFI_RUNTIME_SERVICES 	 *SmmRuntimeVar = NULL;
    EFI_SMM_BASE2_PROTOCOL   *mSmmBase2 = NULL;
    EFI_SMM_SYSTEM_TABLE2    *mSmst = NULL;
    EFI_STATUS               Status; 
  	
    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &mSmmBase2);
   
    if (EFI_ERROR(Status)) {
      return Status;
    }
    
    Status = mSmmBase2->GetSmstLocation (mSmmBase2, &mSmst);
	    
    if (EFI_ERROR(Status)) {
      return Status;
    }

    for (Index = 0; Index < mSmst->NumberOfTableEntries; ++Index) {
      if (guidcmp(&mSmst->SmmConfigurationTable[Index].VendorGuid, \
                           &SmmRsTableGuid) == 0) {
            break;
        }
    }
    
    if (Index != mSmst->NumberOfTableEntries) {
      SmmRuntimeVar =(EFI_RUNTIME_SERVICES *) mSmst->SmmConfigurationTable[Index].VendorTable;
    }
    
    if(SmmRuntimeVar){
       //Initializing SMM Runtime Services
      SmmRuntimeVar->ResetSystem = SmmRtResetSystem;
    }

/*    
    pRS->ResetSystem    = AmiChipsetResetSystem;
    pRS->GetTime        = AmiChipsetGetTime;
    pRS->SetTime        = AmiChipsetSetTime;
    pRS->GetWakeupTime  = AmiChipsetGetWakeupTime;
    pRS->SetWakeupTime  = AmiChipsetSetWakeupTime;
*/
    return Status;
 // Denverton Aptiov Override End - EIP#210294 
}

/**
    This function is the entry point for this SMM. This function
    installs the runtime services related to SB in SMM.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

**/
EFI_STATUS
SbRuntimeSmmInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
    return InitSmmHandler(
            ImageHandle, SystemTable, SbRuntimeServiceInit, NULL );
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
