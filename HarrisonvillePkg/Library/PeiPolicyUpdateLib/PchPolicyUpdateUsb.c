/** @file

@copyright
 Copyright (c) 2012 - 2017 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

//
// EDK and EDKII have different GUID formats
//
#include <Uefi/UefiBaseType.h>
#include <Ppi/PchPolicy.h>
#include <PlatformBoardId.h>
#include <SetupVariable.h>
#include <Library/PchInfoLib.h>
#include <Guid/PlatformInfo.h>
#include <Library/DebugLib.h>

/**
  This function performs PCH USB Platform Policy initialization

  @param[in] PchUsbConfig         Pointer to PCH_USB_CONFIG data buffer
  @param[in] PchSetup             Pointer to PCH_SETUP data buffer
  @param[in] PlatformType         PlatformType specified
  @param[in] PlatformFlavor       PlatformFlavor specified
  @param[in] BoardType            BoardType specified
  @param[in] BoardId              BoardId specified
**/
VOID
UpdatePchUsbConfig (
  IN PCH_USB_CONFIG            *PchUsbConfig,
  IN PCH_SETUP                 *PchSetup,
  IN UINT8                     PlatformType,
  IN UINT8                     PlatformFlavor,
  IN UINT8                     BoardType,
  IN UINT16                    BoardId
  )
{
  UINTN              PortIndex;
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  	  PchUsbConfig->IIL1e = PchSetup->IIL1e;
  	  PchUsbConfig->XhciL1e = PchSetup->XhciL1e;
  PchUsbConfig->UsbPrecondition = PchSetup->UsbPrecondition;
  PchUsbConfig->DisableComplianceMode = PchSetup->DisableComplianceMode;

  for (PortIndex = 0; PortIndex < GetPchXhciMaxUsb2PortNum (); PortIndex++) {
    if (PchSetup->PchUsbHsPort[PortIndex] == 1) {
      PchUsbConfig->PortUsb20[PortIndex].Enable = TRUE;
    } else {
      PchUsbConfig->PortUsb20[PortIndex].Enable = FALSE;
    }
  }
  for (PortIndex = 0; PortIndex < GetPchXhciMaxUsb3PortNum (); PortIndex++) {
    if (PchSetup->PchUsbSsPort[PortIndex] == 1) {
      PchUsbConfig->PortUsb30[PortIndex].Enable = TRUE;
    } else {
      PchUsbConfig->PortUsb30[PortIndex].Enable = FALSE;
    }
  }

  for (PortIndex = 0; PortIndex < GetPchUsbMaxPhysicalPortNum (); PortIndex++)
  {
    PchUsbConfig->PortUsb20[PortIndex].Afe.Petxiset     = 0x4;
    PchUsbConfig->PortUsb20[PortIndex].Afe.Txiset       = 0x0;
    PchUsbConfig->PortUsb20[PortIndex].Afe.Predeemp     = 0x3;
    PchUsbConfig->PortUsb20[PortIndex].Afe.Pehalfbit    = 0x0;
  }



  switch (BoardId) {
  default:
    // OC Map for USB2 Ports
    for (PortIndex = 0; PortIndex < GetPchXhciMaxUsb2PortNum (); PortIndex++)
    {
      if (PchUsbConfig->PortUsb20[PortIndex].Enable == TRUE) {
        PchUsbConfig->PortUsb20[PortIndex].OverCurrentPin = PchUsbOverCurrentPin0;
      } else {
        PchUsbConfig->PortUsb20[PortIndex].OverCurrentPin = PchUsbOverCurrentPinSkip;
      }
    }
    // OC Map for USB3 Ports
    for (PortIndex = 0; PortIndex < GetPchXhciMaxUsb3PortNum (); PortIndex++)
    {
      if (PchUsbConfig->PortUsb30[PortIndex].Enable == TRUE) {
        PchUsbConfig->PortUsb30[PortIndex].OverCurrentPin = PchUsbOverCurrentPin0;
      } else {
        PchUsbConfig->PortUsb30[PortIndex].OverCurrentPin = PchUsbOverCurrentPinSkip;
      }
    }
    break;
  }
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}
