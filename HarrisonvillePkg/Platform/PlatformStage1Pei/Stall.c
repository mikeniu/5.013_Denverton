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
  
  Stall.c
   
Abstract:
  
  Produce Stall Ppi.
  
--*/


#include "PlatformStage1.h"

EFI_STATUS
EFIAPI
Stall (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN CONST EFI_PEI_STALL_PPI    *This,
  IN UINTN                      Microseconds
  )
/*++

Routine Description:

  Waits for at least the given number of microseconds.

Arguments:

  PeiServices     General purpose services available to every PEIM.
  This            PPI instance structure.
  Microseconds    Desired length of time to wait.

Returns: 

  EFI_SUCCESS     If the desired amount of time was passed.

--*/
{
  UINTN                 Ticks;
  UINTN                 Counts;
  UINT32                CurrentTick;
  UINT32                OriginalTick;
  UINT32                RemainingTick;
  
  if (Microseconds == 0) {
    return EFI_SUCCESS;
  }

  OriginalTick = ((**PeiServices).CpuIo->IoRead32) (PeiServices, (**PeiServices).CpuIo, (UINT64)PCH_ACPI_TIMER_ADDR);
  OriginalTick &= (PCH_ACPI_TIMER_MAX_VALUE - 1);
  CurrentTick = OriginalTick;
  
  //
  // The timer frequency is 3.579545MHz, so 1 microsecond corresponds to 3.58 clocks
  //
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;
  
  //
  // The loops needed for timer overflow
  //
  Counts = Ticks / PCH_ACPI_TIMER_MAX_VALUE;
  
  //
  // Remaining clocks within one loop
  //
  RemainingTick = (UINT32)Ticks % PCH_ACPI_TIMER_MAX_VALUE;  
  
  //
  // Do not intend to use TMROF_STS bit of register PM1_STS, because this add extra
  // one I/O operation, and may generate SMI
  //
  
  while (Counts != 0) {
    CurrentTick = ((**PeiServices).CpuIo->IoRead32) (PeiServices, (**PeiServices).CpuIo, (UINT64)PCH_ACPI_TIMER_ADDR);
    CurrentTick &= (PCH_ACPI_TIMER_MAX_VALUE - 1);
    if (CurrentTick <= OriginalTick) {
      Counts--;
    }
    OriginalTick = CurrentTick;
  }

  while ((RemainingTick > CurrentTick) && (OriginalTick <= CurrentTick) ) {
    OriginalTick = CurrentTick;
    CurrentTick = ((**PeiServices).CpuIo->IoRead32) (PeiServices, (**PeiServices).CpuIo, (UINT64)PCH_ACPI_TIMER_ADDR);
    CurrentTick &= (PCH_ACPI_TIMER_MAX_VALUE - 1);
  }        

  return EFI_SUCCESS;
}


