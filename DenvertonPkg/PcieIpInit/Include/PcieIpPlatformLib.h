/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

/*++

Copyright (c)  2015 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PcieIpPlatformLib.h

Abstract:

  Driver for Pcie Platform specific library.

Revision History

--*/

#ifndef _PCIE_IP_PLATFORM_LIB_H_
#define _PCIE_IP_PLATFORM_LIB_H_

#include <Ppi/PeiPcieIpPolicyPpi.h>
#include <PcieIpPlatform.h>

EFI_STATUS
PcieIpPlatHidePort(
  PCIE_IP_PORT_POLICY *PciIpPortPolicy
);

EFI_STATUS
PcieIpPlatPllHandoff(
  PCIE_IP_GLOBAL_POLICY_PPI *PciIpGlobalPolicy
);

EFI_STATUS
PcieIpPlatPowerGateController();

/**
  Initialize Pcie IP Platform Port Bus/device/function mapping

  Arguments:
  @param[in] PcieIpGlobalPolicy - This is the global PCIE policy

  Returns:
  @retval EFI_SUCCESS 						- The function completed successfully.
  @retval V_ERROR_INVALID_ROOT_PORT_NUMBER	- error in root port base address
  @retval V_ERROR_PCI_DEVICE_IS_NOT_PRESENT - error in root port base address
  @retval RETURN_INVALID_PARAMETER			- Invalid address of policy
**/
EFI_STATUS
PcieIpPlatformPortBdfMap(
  IN PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobalPolicy
);

/**
  Fill PCIE policy with default values

  @param[in] PcieIpGlobalPolicyPpi - The pointer to PCIE IP Policy PPI instance

  @retval EFI_SUCCESS              - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES     - Insufficient resources to create buffer
**/
VOID
PcieIpPlatformInitDefault (
  IN  PCIE_IP_GLOBAL_POLICY_PPI 	*PcieIpGlobalPolicyPpi
);

UINTN
PcieIpPlatformGetVirtualRootPortBase (
  UINT8 VRpIndex
  );

UINTN
EFIAPI
PeiPcieIpGetRootPortBase (
  PCIE_IP_BDF_MAP_ENTRY *RootPort
  );

UINT8
PcieIpPlatformGetPchMaxPcieVirtPortNum (
  VOID
  );

VOID
PcieIpPlatformLTR(
  IN UINTN RPBase,
  IN BOOLEAN LtrEnable
		);

VOID
PcieIpPlatformSlotImplemented(
  IN UINTN RPBase,
  IN UINTN PhysicalSlotNumber,
  IN BOOLEAN HotPlug
		);

BOOLEAN
EFIAPI
PcieIpPlatformPchIsPciePresent (
  VOID
  );

VOID
PcieIpPlatformHotPlug(
  IN UINTN RPBase
  );

UINT8
EFIAPI
PcieIPGetPchStandardPciePortNum (
  VOID
	  );

EFI_STATUS
PcieIpPlatformFIA(
  IN PCIE_IP_GLOBAL_POLICY_PPI  *PcieIpGlobaIpolicy
	);


#endif
