/** @file
  Header file for FIA Mux Helper PEI library.

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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

#ifndef _PEI_DXE_SMM_FIA_MUX_HELPER_LIB_H_
#define _PEI_DXE_SMM_FIA_MUX_HELPER_LIB_H_

#include <Ppi/FiaMuxPolicyPpi.h>
#include <Guid/FiaMuxHobGuid.h>

/**
Query if FIA MUX multi lanes configuration as required type.

@param[in] StartLaneNum           - Start of lane number
@param[in] EndLaneNum             - End of Lane number
@param[in] FiaMuxLaneConfig       - Required FIA MUX lane configure type

@retval EFI_SUCCESS           - The multi lanes' FIA MUX Configuration are equal to the input configure type
@retval EFI_INVALID_PARAMETER - The lanes number or FIA MUX configuration is invalid.
@retval EFI_ABORTED           - At least one lane's configure is different with the input configure type
**/
EFI_STATUS
FiaMuxConfigLanesQueryMultiMatch(
IN UINT32 StartLaneNum,
IN UINT32 EndLaneNum,
IN ME_FIA_MUX_LANE_CONFIG FiaMuxLaneConfig
);

/**
Query if at least one lane's FIA MUX configuration as required type.

@param[in] StartLaneNum           - Start of lane number
@param[in] EndLaneNum             - End of Lane number
@param[in] FiaMuxLaneConfig       - Required FIA MUX lane configure type

@retval EFI_SUCCESS           - At least one lane's FIA MUX Configuration is equal to the input configure type
@retval EFI_INVALID_PARAMETER - The lanes number or FIA MUX configuration is invalid.
@retval EFI_ABORTED           - None of multi lane's configure is equal to with the input configure type
**/
EFI_STATUS
FiaMuxConfigLanesQuerySingleMatch(
IN UINT32 StartLaneNum,
IN UINT32 EndLaneNum,
IN ME_FIA_MUX_LANE_CONFIG FiaMuxLaneConfig
);

/**
Query FIA MUX Lane Configuration.

@param[in] LaneNum            - Lane number

@param[out] *pFiaMuxLaneConfig - On success, contains the lane's FIA MUX configuration.

@retval EFI_SUCCESS           - The lane's FIA MUX Configuration is returned to pFiaMuxLaneConfig
@retval EFI_INVALID_PARAMETER - The Lane number or FIA MUX configuration is invalid.
**/
EFI_STATUS
FiaMuxConfigLaneQuery(
IN UINT32 LaneNum,
OUT ME_FIA_MUX_LANE_CONFIG *pFiaMuxLaneConfig
);

/**
Get the number of configurable FIA MUX lanes allowed by the SKU.

@param[out] *pLanesAllowed     - On success, contains the number of allowed lanes

@retval EFI_SUCCESS           - The number of FIA MUX configurable lane returned.
@retval EFI_INVALID_PARAMETER - The Lane number pointer is invalid.
**/
EFI_STATUS
FiaMuxConfigGetNumLanes(
OUT UINT32 *pLanesAllowed
);

#endif  //_PEI_DXE_SMM_FIA_MUX_HELPER_LIB_H_
