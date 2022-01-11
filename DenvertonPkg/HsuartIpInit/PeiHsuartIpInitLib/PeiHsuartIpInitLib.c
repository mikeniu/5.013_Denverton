/** @file
  This file is a library for PCIE Ip specific Initalization.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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

@par Specification Reference:

**/

#include <PeiHsuartIpInitLib.h>
#include <IndustryStandard/Pci.h>
#include <PchAccess.h>
#include <Library/PchSbiAccessLib.h>

EFI_STATUS
HsuartIpInit (
  IN HSUART_IP_GLOBAL_POLICY_PPI *HsuartIpGlobalPolicy
  )
/**
  Initialize IP PCIE

  @param[in] HsuartIpGlobalPolicy - This is the global Hsuart policy


  @retval EFI_SUCCESS           - The function completed successfully.
**/
{
  EFI_STATUS Status;
  UINT8      Index;
  UINT8      Response;

  DEBUG ((DEBUG_INFO, "HsuartIpInit Start\n"));
  Status = EFI_SUCCESS;

  for(Index=0;Index<HsuartIpGlobalPolicy->HsuartNum;Index++){
	  // Configure the class code for HSUART x to 0x070002
	  Status = PchSbiExecutionEx ( V_HSUART_PID,
			                       R_REVISION_ID,
			                       V_OPCODE_WRITE,
			                       FALSE,
			                       V_SBI_FBE,
			                       V_SBI_BAR,
			                       ((HsuartIpGlobalPolicy->DeviceNum<<SHIFT_LEFT_3)|Index),
			                       &HsuartIpGlobalPolicy->DefaultClassCode,
			                       &Response
			                       );

    // Set interrupt pin
    MmioAndThenOr8(MmPciBase(V_HSUART_BUS_NUM, HsuartIpGlobalPolicy->DeviceNum, Index) + PCI_INT_PIN_OFFSET,
                             (UINT8)0, HsuartIpGlobalPolicy->InterruptPin[Index]);
  }

  for(Index=0;Index<HsuartIpGlobalPolicy->HsuartNum;Index++){
	  //Set PLKCTL.CL
	  MmioAndThenOr32(MmPciBase(V_HSUART_BUS_NUM, HsuartIpGlobalPolicy->DeviceNum, Index) + R_PLKCTL,
                                (UINT32)B_CL_MASK, (UINT32)B_CL_BIT);
  }

  DEBUG ((DEBUG_INFO, "HsuartIpInit End\n"));
  return Status;
}
