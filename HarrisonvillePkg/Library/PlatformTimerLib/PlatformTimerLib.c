/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  PCH ACPI Timer implements one instance of PlatformTimer Library.

Copyright (c) 2012 - 2013, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#include <Base.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PciExpressLib.h>
#include <Library/PcdLib.h>
#include <Library/MmPciLib.h>
#include <PchAccess.h>

/**
  Initialize ACPI timer
**/
VOID
EFIAPI
AcpiTimerInit (
  VOID
  )
{
  UINTN PmcBaseAddress;
  PmcBaseAddress = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_PMC,
                    PCI_FUNCTION_NUMBER_PCH_PMC
                    );

  if ((MmioRead8 (PmcBaseAddress + R_PCH_PMC_ACPI_CNT) & B_PCH_PMC_ACPI_CNT_ACPI_EN) == 0) {
    //
    // If ACPI I/O space is not enabled, program ACPI I/O base address and enable it.
    //
      MmioWrite16 (
        PmcBaseAddress + R_PCH_PMC_ACPI_BASE,
        PcdGet16 (PcdAcpiBaseAddress)
        ); 
      
      MmioOr8 (
        PmcBaseAddress + R_PCH_PMC_ACPI_CNT,
        B_PCH_PMC_ACPI_CNT_ACPI_EN
        );
  }
}
