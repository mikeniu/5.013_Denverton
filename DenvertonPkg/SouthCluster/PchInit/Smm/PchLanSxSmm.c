/** @file
  PCH LAN Sx handler implementation.

@copyright
 Copyright (c) 2013 - 2017 Intel Corporation. All rights reserved
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
**/

#include <Library/TimerLib.h>
#include "PchInitSmm.h"


/**
  Enable paltfom wake from LAN when in DeepSx if platform supports it.
  Called upon Sx entry.
**/
VOID
GbeConfigureDeepSxWake (
  VOID
  )
{
  UINT32 PchPwrmBase;
  UINTN  XhciPciMmBase;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  PchPwrmBaseGet (&PchPwrmBase);
  XhciPciMmBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_XHCI,
                    PCI_FUNCTION_NUMBER_PCH_XHCI
                    );

  MmioOr32 ( XhciPciMmBase + R_PCH_XHCI_XHCLKGTEN, (UINT32)BIT2 );
  DEBUG ((EFI_D_INFO, "\tXHCLKGTEN = 0x%08x\tValue = 0x%08x\n",XhciPciMmBase + R_PCH_XHCI_XHCLKGTEN,MmioRead32 ( XhciPciMmBase + R_PCH_XHCI_XHCLKGTEN ) ));
  MmioOr16 ( XhciPciMmBase + R_PCH_XHCI_PCE, (UINT16)BIT2 );
  DEBUG ((EFI_D_INFO, "\tPCE_REG = 0x%08x\tValue = 0x%08x\n",XhciPciMmBase + R_PCH_XHCI_PCE,(UINT16)MmioRead16 ( XhciPciMmBase + R_PCH_XHCI_PCE ) ));
  IoOr32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0_EN_127_96), (UINT32) B_PCH_ACPI_GPE0_EN_127_96_LAN_WAKE);
  IoOr32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0_EN_127_96), (UINT32) B_PCH_ACPI_GPE0_EN_127_96_PME_B0);
  IoOr32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0_EN_127_96), (UINT32) B_PCH_ACPI_GPE0_EN_127_96_USB_CON_DSX);
  DEBUG ((EFI_D_INFO, "\tGPE0_EN_127_96 = 0x%08x\tValue = 0x%08x\n",(mAcpiBaseAddr + R_PCH_ACPI_GPE0_EN_127_96),IoRead32(mAcpiBaseAddr + R_PCH_ACPI_GPE0_EN_127_96) ));


  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}

/**
  GbE Sx entry handler
**/
VOID
PchLanSxCallback (
  VOID
  )
{
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  GbeConfigureDeepSxWake ();
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

}
