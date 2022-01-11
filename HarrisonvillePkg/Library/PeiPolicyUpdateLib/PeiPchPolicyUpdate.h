/*++
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement 
--*/
/*++

Copyright (c) 2009 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PchPlatformPolicyUpdatePei.h

Abstract:

  Header file for PEI PchPlatformPolicyUpdate.

--*/
#ifndef _PCH_PLATFORM_POLICY_UPDATE_PEI_H_
#define _PCH_PLATFORM_POLICY_UPDATE_PEI_H_

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
#include <PiPei.h>

#include <Guid/PlatformInfo.h>

#include <PlatformBoardId.h>
// #include <PlatformDefinitions.h> // Denverton AptioV override
#include <Include/SetupVariable.h>
#include <Library/PeiPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/BootState.h>
#include <PchAccess.h>
#include <Ppi/PchPolicy.h>
#include <Library/MmPciLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
//pcie
#include <PcieIpInit/Include/PeiPcieIpPolicyLib.h>
#include <PcieIpInit/Include/PcieIpPlatformLib.h>
#include <PcieIpInit/Include/PcieIpPolicy.h>
extern EFI_GUID gEfiCapsuleVendorGuid;

/**
  This function performs PCH USB Platform Policy initialization

  @param[in]  PchUsbConfig     Pointer to PCH_USB_CONFIG data buffer
  @param[in]  PchSetup         Pointer to PCH_SETUP data buffer
  @param[in]  PlatformType     PlatformType specified
  @param[in]  PlatformFlavor   PlatformFlavor specified
  @param[in]  BoardType        BoardType specified
  @param[in]  BoardId          BoardId specified
**/
VOID
UpdatePchUsbConfig (
  IN PCH_USB_CONFIG            *PchUsbConfig,
  IN PCH_SETUP                 *PchSetup,
  IN UINT8                     PlatformType,
  IN UINT8                     PlatformFlavor,
  IN UINT8                     BoardType,
  IN UINT16                    BoardId
  );

#endif
