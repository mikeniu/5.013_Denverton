/** @file
  This file is a library for FIA MUX specific Initalization.

@copyright
 Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
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

#include <Library/IoLib.h>
#include <Library/Hoblib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/PeiFiaMuxPolicyInitLib.h>
#include <Library/PeiFiaMuxConfigInitLib.h>
#include <Library/MeFiaMuxLib.h>
#include <Library/MmPciLib.h>
#include <SimicsDebug.h>

EFI_STATUS
PeiFiaMuxConfigInit (
  OUT BOOLEAN *GlobalResetRequired
)
/**
  Initialize FIA MUX configuration.

  Assumption:
  The FIA MUX Policy and FIA MUX HOB structures are created and
  initialized prior to entry into this function. They contain the
  current FIA MUX configuration read from ME.

  Arguments:
  None

  Returns:
  @retval EFI_SUCCESS 				- The function completed successfully.
  @retval EFI_INVALID_PARAMETER 	- invalid parameter
  @retval EFI_NOT_READY Returned if this API is called before ME signals init
                        complete, or after END_OF_POST was sent.
  @retval EFI_DEVICE_ERROR Returned if HECI transport is not available.
  @retval EFI_PROTOCOL_ERROR Unexpected protocol error returned by ME.
  @retval EFI_UNSUPPORTED Function not supported on current firmware
  @retval EFI_OUT_OF_RESOURCES SKU is invalid for current configuration
  @retval EFI_ABORTED Configuration invalidated
**/
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  FIA_MUX_CONFIG_HOB              *FiaMuxConfigHob;
  FIA_MUX_GLOBAL_POLICY_PPI       *FiaMuxGlobalPolicy;

  DEBUG ((DEBUG_INFO, "PeiFiaMuxConfigInit Start\n"));

  *GlobalResetRequired = FALSE;

  //
  // Locate FIA MUX HOB
  //
  FiaMuxConfigHob = FiaMuxPolicyGetHob();
  ASSERT_EFI_ERROR(FiaMuxConfigHob != NULL);

  //
  // Locate installed FIA MUX Policy PPI
  //
  Status = PeiServicesLocatePpi(
	  &gFiaMuxPolicyPpiGuid,
	  0,
	  NULL,
	  (VOID **)&FiaMuxGlobalPolicy
	  );
  ASSERT_EFI_ERROR(Status);

  if (FiaMuxGlobalPolicy->FiaMuxConfigStatus.FiaMuxConfigGetStatus == EFI_SUCCESS) {
    //
    // Check if requested FIA MUX configuration same as the current one.
    //
      if (CompareMem (&FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfigRequest, &FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig, sizeof(ME_FIA_CONFIG))) { 	
      //
      // Set FiaMuxConfigSetRequired to TRUE
      //
      FiaMuxGlobalPolicy->FiaMuxConfigStatus.FiaMuxConfigSetRequired = TRUE;

      Status = MeFiaMuxConfigSet(&(FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfigRequest));

      DEBUG((DEBUG_INFO, "FiaMuxConfigSet() status = %r, MuxConfiguration 0x%lx\n", Status, (UINT64)FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfigRequest.MuxConfiguration.MeFiaMuxLaneConfig));
      DEBUG((DEBUG_INFO, "    SataLaneConfiguration 0x%lx, PcieRootPortsConfiguration 0x%lx\n", (UINT64)FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfigRequest.SataLaneConfiguration.MeFiaSataLaneConfig,
          (UINT64)FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfigRequest.PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig));
      
      FiaMuxGlobalPolicy->FiaMuxConfigStatus.FiaMuxConfigSetStatus = Status;

      switch (Status) {
      case EFI_SUCCESS:
        //
        // Update the FIA MUX configuration structure with the new configuration.
        //
        CopyMem(&FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig, &FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfigRequest, sizeof(ME_FIA_CONFIG));

        //
        // Set reset flag.
        //
        *GlobalResetRequired = TRUE;

        break;

      case EFI_OUT_OF_RESOURCES:
        //
      	// Configure ME FIA MUX configuration fail
        //
      	DEBUG((DEBUG_ERROR, "PeiFiaMuxConfigInit: Configure ME FIA MUX configuration fail with status = %r\n", Status));
        FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig.MuxConfiguration.MeFiaMuxLaneConfig = (UINT64)ME_FIA_MUX_LANE_XHCI_ONLY;
        FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig.SataLaneConfiguration.MeFiaSataLaneConfig = (UINT64)ME_FIA_SATA_LANE_XHCI_ONLY;
        FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig = (UINT64)ME_FIA_PCIE_ROOT_CONFIG_XHCI_ONLY;       
        break;

      // Denverton AptioV override Start - EIP#337485
      // Intel ME notion of EOP status bit was set.
      // If the BIOS encounters this bit set to 1 during POST, it signals an error in platform power flow.
      // To clean this, BIOS may consider performing Global Platform Reset
      case EFI_NOT_READY:
        *GlobalResetRequired = TRUE;
         break;
      // Denverton AptioV override End - EIP#337485

      case EFI_ABORTED: 
        //
        // SPS returned, that configuration is invalid. Send SetFiaMuxConfiguration with invalidate field set to TRUE.
        //
        DEBUG((DEBUG_WARN, "PeiFiaMuxConfigInit: SPS reports configuration is invalid. Sending invalidate configuration message"));
        Status = MeFiaMuxConfigSet(NULL);
        if (EFI_ERROR(Status)) {
          DEBUG((DEBUG_ERROR, "PeiFiaMuxConfigInit: Sending invalidate configuration message failed, status = %r\n", Status));
          Status = EFI_ABORTED;
          FiaMuxGlobalPolicy->FiaMuxConfigStatus.FiaMuxConfigSetStatus = Status;
          break;
        }
        *GlobalResetRequired = TRUE;

        //
        // Intentionally no break here
        //
      default:
        //
    	// Configure ME FIA MUX configuration fail
        //
    	DEBUG((DEBUG_ERROR, "PeiFiaMuxConfigInit: Configure ME FIA MUX configuration fail with status = %r\n", Status));
        break;
      }
    }
  } else {
    //
    // Request ME FIA MUX configuration fail
    //
    DEBUG((DEBUG_ERROR, "PeiFiaMuxConfigInit: Request ME FIA MUX configuration fail with status = %r\n", Status));
  }

  //
  // Setting HOB with existing FIA MUX configuration
  //
  CopyMem((VOID *)&(FiaMuxConfigHob->FiaMuxConfig), &(FiaMuxGlobalPolicy->FiaMuxConfig), sizeof(FIA_MUX_CONFIG));

  DEBUG ((DEBUG_INFO, "PeiFiaMuxConfigInit End\n"));

  return Status;
}
