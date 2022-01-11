/** @file
  Initializes PCH SCC Devices

@copyright
  Copyright (c) 2012 - 2014 Intel Corporation. All rights reserved
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
#include "PchInit.h"
#ifndef FSP_FLAG
#include "Protocol/GlobalNvsArea.h"
#endif  //FSP_FLAG
#include <PchAccess.h>

EFI_STATUS
ConfigureScc (
  IN     PCH_POLICY_HOB       *PchPlatformPolicy,
  IN OUT UINT32                            *FuncDisableReg

  )
/*++

Routine Description:

  Configure SCC devices.

Arguments:

  PchPlatformPolicy       The PCH Platform Policy protocol instance
  FuncDisableReg          Function Disable Register

Returns:

  EFI_SUCCESS             The function completed successfully

--*/

{
  EFI_STATUS            Status;
  UINTN                 SccPciMmBase = 0;
  EFI_PHYSICAL_ADDRESS  SccMmioBase0 = 0;

  ///
  /// Enable IOSF Snoop
  ///
  
  PchPcrAndThenOr32 (
    PCH_SCC_EP_PORT_ID,
    R_PCH_SCC_EP_IOSFCTL,
    0xFFFFFF7F,
    B_PCH_SCC_EP_IOSFCTL_NSNPDIS
  ); 

  // set PCI_IRQ[27:20]=0x10
  PchPcrAndThenOr32 (
    PCH_SCC_EP_PORT_ID,
    R_PCH_SCC_EP_PCICFGCTR1,
    M_PCH_SCC_EP_PCICFGCTR1_PCI_IRQ1,
    B_PCH_SCC_EP_PCICFGCTR1_PCI_IRQ1
  );

  ///
  /// SCC eMMC
  ///
 
#if !VP_FLAG //zchai
  SccPciMmBase = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_SCC_SDIO_0,
                   PCI_FUNCTION_NUMBER_PCH_SCC_SDIO
                 );

  if (PchPlatformPolicy->eMMCEnabled == 0) {
    DEBUG ((EFI_D_INFO, "Putting SCC eMMC into D3 Hot State.\n"));
    MmioOr32 ((UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_PCS), B_PCH_SCC_SDIO_PCS_PS);
    S3BootScriptSaveMemWrite(
      EfiBootScriptWidthUint32,
      (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_PCS),
      1,
      (VOID *) (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_PCS)
      );
    *FuncDisableReg |= B_PCH_PMC_FUNC_DIS_SDIO1;
  } else {
    ///
    /// Check if device present
    ///
    if (MmioRead32 (SccPciMmBase) != 0xFFFFFFFF) {
      ///
      /// Disable BAR1 when device is working in PCI mode.
      ///
	   if (PchPlatformPolicy->eMMCEnabled == 0) {
        PchPcrAndThenOr32 (
          PCH_SCC_EP_PORT_ID,
          R_PCH_SCC_EP_PCICFGCTR1,
          0xFFFFFFFF,
          (B_PCH_SCC_EP_PCICFGCTR1_B1D1)
        ); 
      }
	  
      ///
      /// Allocate temporary base address
      ///
#ifndef FSP_FLAG
      Status = gDS->AllocateMemorySpace (
                      EfiGcdAllocateAnySearchBottomUp,
                      EfiGcdMemoryTypeMemoryMappedIo,
                      N_PCH_SCC_SDIO_BAR_ALIGNMENT,
                      V_PCH_SCC_SDIO_BAR_SIZE,
                      &SccMmioBase0,
                      mImageHandle,
                      NULL
                      );
#else  //FSP_FLAG
      SccMmioBase0 = MmioRead32 (SccPciMmBase + R_PCH_SCC_SDIO_BAR) & B_PCH_SCC_SDIO_BAR_BA;
      Status = EFI_SUCCESS;
#endif  //FSP_FLAG
      ASSERT_EFI_ERROR (Status);
      if (!EFI_ERROR (Status)) {
        ///
        /// Disable Bus Master Enable & Memory Space Enable
        ///
        MmioAnd32 ((UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD), (UINT32) ~(B_PCH_SCC_SDIO_STSCMD_BME | B_PCH_SCC_SDIO_STSCMD_MSE));
        S3BootScriptSaveMemWrite(
          EfiBootScriptWidthUint32,
          (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD),
          1,
          (VOID *) (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD)
          );
        ///
        /// Program BAR 0
        ///
        ASSERT (((SccMmioBase0 & B_PCH_SCC_SDIO_BAR_BA) == SccMmioBase0) && (SccMmioBase0 != 0));
        MmioWrite32 ((UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_BAR), (UINT32) (SccMmioBase0 & B_PCH_SCC_SDIO_BAR_BA));
        S3BootScriptSaveMemWrite(
          EfiBootScriptWidthUint32,
          (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_BAR),
          1,
          (VOID *) (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_BAR)
          );
        ///
        /// Bus Master Enable & Memory Space Enable
        ///
        MmioOr32 ((UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD), (UINT32) (B_PCH_SCC_SDIO_STSCMD_BME | B_PCH_SCC_SDIO_STSCMD_MSE));
        S3BootScriptSaveMemWrite(
          EfiBootScriptWidthUint32,
          (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD),
          1,
          (VOID *) (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD)
          );
        ASSERT (MmioRead32 ((UINTN) SccMmioBase0) != 0xFFFFFFFF);
        ///
        /// Set Maximum Timeout to 0x0E
        ///
        MmioWrite8 ((UINTN) (SccMmioBase0 + R_PCH_SCC_SDIO_MEM_TIMEOUT_CTL), (UINT8) 0x0E);
        ///
        /// Disable Bus Master Enable & Memory Space Enable
        ///
        MmioAnd32 ((UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD), (UINT32) ~(B_PCH_SCC_SDIO_STSCMD_BME | B_PCH_SCC_SDIO_STSCMD_MSE));
        S3BootScriptSaveMemWrite(
          EfiBootScriptWidthUint32,
          (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD),
          1,
          (VOID *) (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD)
          );
        ///
        /// Clear BAR0
        ///
        MmioWrite32 ((UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_BAR), (UINT32) (0x00));
#ifndef FSP_FLAG
        gDS->FreeMemorySpace (SccMmioBase0, (UINT64) V_PCH_SCC_SDIO_BAR_SIZE);
#endif  //FSP_FLAG
      }
    } else {
      DEBUG ((EFI_D_ERROR, "SCC eMMC not present, skipping.\n"));
      //PchPlatformPolicy->eMMCEnabled = PchDisabled;
      *FuncDisableReg |= B_PCH_PMC_FUNC_DIS_SDIO1;
    }
  }
#endif //zchai
 
  DEBUG ((EFI_D_INFO, "ConfigureScc() End\n"));
  return EFI_SUCCESS;
}


EFI_STATUS
ConfigureSccAtBoot (
  IN PCH_POLICY_HOB      *PchPlatformPolicy
  )

/**

  @brief
  Hide PCI config space of SCC devices and do any final initialization.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance

  @retval EFI_SUCCESS             The function completed successfully

**/
{

  UINTN                         SccPciMmBase;
  UINT32                        SccMmioBase0;
  UINT32                        SccMmioBase1;
#ifndef FSP_FLAG
  EFI_STATUS                    Status;
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsArea;
#endif  //FSP_FLAG


  DEBUG ((EFI_D_INFO, "ConfigureSccAtBoot() Start\n")); 
//
  ///
  /// Initialize Variables
  ///
  SccPciMmBase     = 0;
  SccMmioBase0     = 0;
  SccMmioBase1     = 0;
  ///
  /// Update SCC devices ACPI variables
  ///
#ifndef FSP_FLAG
  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &GlobalNvsArea
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
#endif  //FSP_FLAG
  ///
  /// SCC eMMC
  ///
//#if !VP_FLAG //zchai
  if (PchPlatformPolicy->eMMCEnabled == 0) {
    DEBUG ((EFI_D_INFO, "Switching SCC eMMC into ACPI Mode.\n"));	
    SccPciMmBase = MmPciBase ( DEFAULT_PCI_BUS_NUMBER_PCH,
                               PCI_DEVICE_NUMBER_PCH_SCC_SDIO_0,
                               PCI_FUNCTION_NUMBER_PCH_SCC_SDIO
                              );
    SccMmioBase0 = MmioRead32 ((UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_BAR)) & B_PCH_SCC_SDIO_BAR_BA;
    SccMmioBase1 = MmioRead32 ((UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_BAR1)) & B_PCH_SCC_SDIO_BAR1_BA;
    ///
    /// Disable PCI Interrupt, Bus Master Enable & Memory Space Enable
    ///
    MmioOr32 (
      (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD),
      (UINT32) (B_PCH_SCC_SDIO_STSCMD_INTRDIS | B_PCH_SCC_SDIO_STSCMD_BME | B_PCH_SCC_SDIO_STSCMD_MSE)
    );
    S3BootScriptSaveMemWrite(
      EfiBootScriptWidthUint32,
      (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD),
      1,
      (VOID *) (UINTN) (SccPciMmBase + R_PCH_SCC_SDIO_STSCMD)
      );
    ///
    /// Update BAR and length in ACPI table.
    ///
#ifndef FSP_FLAG
    GlobalNvsArea->Area->eMMCAddr  = SccMmioBase0;
    GlobalNvsArea->Area->eMMCLen   = V_PCH_SCC_SDIO_BAR_SIZE;
    GlobalNvsArea->Area->eMMC1Addr = SccMmioBase1;
    GlobalNvsArea->Area->eMMC1Len  = V_PCH_SCC_SDIO_BAR1_SIZE;
#endif  //FSP_FLAG
    ///
    /// Switch to ACPI Mode
    ///
	
    PchPcrAndThenOr32 (
      PCH_SCC_EP_PORT_ID,
      R_PCH_SCC_EP_PCICFGCTR1,
      0xFFFFFFFF,
      (B_PCH_SCC_EP_PCICFGCTR1_ACPI_INT_EN1 | B_PCH_SCC_EP_PCICFGCTR1_PCI_CFG_DIS1)
    ); 
  }
  DEBUG ((EFI_D_INFO, "ConfigureSccAtBoot() End\n"));
  return EFI_SUCCESS;
}
