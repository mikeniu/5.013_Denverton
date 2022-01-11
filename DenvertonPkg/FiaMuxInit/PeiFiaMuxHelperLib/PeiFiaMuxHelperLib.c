/** @file

  This file is a library for FIA MUX PEI phase Helper library.

@copyright
 Copyright (c) 2016 Intel Corporation. All rights reserved
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
#include <Library/Hoblib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/PeiDxeSmmFiaMuxHelperLib.h>

STATIC
ME_FIA_MUX_LANE_CONFIG
GetFiaMuxLaneConfig(
IN UINT32 LaneNum,
IN ME_FIA_CONFIG MeFiaMuxConfig
)
/**
Get the specific Lane's FIA MUX Configuration.

@param[in] LaneNum              - Lane number
@param[in] MeFiaMuxConfig       - The FIA MUX configuration.

@retval ME_FIA_MUX_LANE_CONFIG  - The specific lane's FIA MUX Configuration.
**/
{	
  return (ME_FIA_MUX_LANE_CONFIG)((UINT32)(RShiftU64(MeFiaMuxConfig.MuxConfiguration.MeFiaMuxLaneConfig, 2 * LaneNum)) & ME_FIA_MUX_LANE_MUX_SEL_MASK); 
}

EFI_STATUS
FiaMuxConfigLanesQueryMultiMatch(
IN UINT32 StartLaneNum,
IN UINT32 EndLaneNum,
IN ME_FIA_MUX_LANE_CONFIG FiaMuxLaneConfig
)
/**
Query if FIA MUX multi lanes configuration as required type.

@param[in] StartLaneNum           - Start of lane number
@param[in] EndLaneNum             - End of Lane number
@param[in] FiaMuxLaneConfig       - Required FIA MUX lane configure type

@retval EFI_SUCCESS           - The multi lanes' FIA MUX Configuration are equal to the input configure type
@retval EFI_INVALID_PARAMETER - The lanes number or FIA MUX configuration is invalid.
@retval EFI_ABORTED           - At least one lane's configure is different with the input configure type
**/
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINT32                      LaneNum;
  FIA_MUX_GLOBAL_POLICY_PPI   *FiaMuxGlobalPolicy;

  //
  // Check if the lane number is a valid number.
  //
  if ((StartLaneNum >= ME_FIA_MUX_LANE_NUM_MAX) || (EndLaneNum >= ME_FIA_MUX_LANE_NUM_MAX)
	  || (StartLaneNum > EndLaneNum) || (FiaMuxLaneConfig > ME_FIA_MUX_LANE_XHCI)) {
    return EFI_INVALID_PARAMETER;
  }

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

  //
  // Return the specific lane config.
  //
  for (LaneNum = StartLaneNum; LaneNum <= EndLaneNum; LaneNum++) {
    if (FiaMuxLaneConfig != GetFiaMuxLaneConfig(LaneNum, FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig)) {
      Status =  EFI_ABORTED;
      break;
    }
  }

  return Status;
}

EFI_STATUS
FiaMuxConfigLanesQuerySingleMatch(
IN UINT32 StartLaneNum,
IN UINT32 EndLaneNum,
IN ME_FIA_MUX_LANE_CONFIG FiaMuxLaneConfig
)
/**
Query if at least one lane's FIA MUX configuration as required type.

@param[in] StartLaneNum           - Start of lane number
@param[in] EndLaneNum             - End of Lane number
@param[in] FiaMuxLaneConfig       - Required FIA MUX lane configure type

@retval EFI_SUCCESS           - At least one lane's FIA MUX Configuration is equal to the input configure type
@retval EFI_INVALID_PARAMETER - The lanes number or FIA MUX configuration is invalid.
@retval EFI_ABORTED           - None of multi lane's configure is equal to the input configure type
**/
{
  EFI_STATUS                  Status;
  UINT32                      LaneNum;
  FIA_MUX_GLOBAL_POLICY_PPI   *FiaMuxGlobalPolicy;

  //
  // Check if the lane number is a valid number.
  //
  if ((StartLaneNum >= ME_FIA_MUX_LANE_NUM_MAX) || (EndLaneNum >= ME_FIA_MUX_LANE_NUM_MAX)
	  || (StartLaneNum > EndLaneNum) || (FiaMuxLaneConfig > ME_FIA_MUX_LANE_XHCI)) {
    return EFI_INVALID_PARAMETER;
  }

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
  Status = EFI_ABORTED;
  //
  // Return the specific lane config.
  //
  for (LaneNum = StartLaneNum; LaneNum <= EndLaneNum; LaneNum++) {
    if (FiaMuxLaneConfig == GetFiaMuxLaneConfig(LaneNum, FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig)) {
      Status =  EFI_SUCCESS;
      break;
    }
  }

  return Status;
}

EFI_STATUS
FiaMuxConfigLaneQuery(
IN UINT32 LaneNum,
OUT ME_FIA_MUX_LANE_CONFIG *pFiaMuxLaneConfig
)
/**
Query FIA MUX Lane Configuration.

@param[in] LaneNum            - Lane number

@param[out] *pFiaMuxLaneConfig - On success, contains the lane's FIA MUX configuration.

@retval EFI_SUCCESS           - The lane's FIA MUX Configuration is returned to pFiaMuxLaneConfig
@retval EFI_INVALID_PARAMETER - The Lane number or FIA MUX configuration is invalid.
**/
{
  EFI_STATUS             Status = EFI_SUCCESS;
  FIA_MUX_GLOBAL_POLICY_PPI       *FiaMuxGlobalPolicy;

  //
  // Check if the lane number is a valid number.
  //
  if ((LaneNum >= ME_FIA_MUX_LANE_NUM_MAX) || (pFiaMuxLaneConfig == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

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

  //
  // Return the specific lane config.
  //
  *pFiaMuxLaneConfig = GetFiaMuxLaneConfig(LaneNum, FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig);

  DEBUG((DEBUG_INFO, "FIA Mux Lane 0x%x config is 0x%x\n", (UINT32)LaneNum, (ME_FIA_MUX_LANE_CONFIG)*pFiaMuxLaneConfig));

  return Status;
}

EFI_STATUS
FiaMuxConfigGetNumLanes(
OUT UINT32 *pLanesAllowed
)
/**
Get the number of configurable FIA MUX lanes allowed by the SKU.

@param[out] *pLanesAllowed     - On success, contains the number of allowed lanes

@retval EFI_SUCCESS           - The number of FIA MUX configurable lane returned.
@retval EFI_INVALID_PARAMETER - The Lane number pointer is invalid.
**/
{
  EFI_STATUS             Status = EFI_SUCCESS;
  FIA_MUX_GLOBAL_POLICY_PPI       *FiaMuxGlobalPolicy;

  //
  // Check if the pointer is valid.
  //
  if (pLanesAllowed == NULL) {
    return EFI_INVALID_PARAMETER;
  }

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

  //
  // Return the number of lanes allowed.
  //
  *pLanesAllowed = FiaMuxGlobalPolicy->FiaMuxConfig.SkuNumLanesAllowed;

  DEBUG((DEBUG_INFO, "NumLanesAllowed is 0x%x\n", *pLanesAllowed));

  return Status;
}
