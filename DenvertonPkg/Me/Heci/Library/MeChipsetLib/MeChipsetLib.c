/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2004 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MeChipsetLib.c

@brief:
  Me Chipset Lib implementation.

**/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <PiDxe.h>
#include <Library/IoLib.h>
#include <Library/PciExpressLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include "MeAccess.h"
#include "PchAccess.h"
#include "HeciRegs.h"
#include "MeSetup.h"


/**

  @brief
  Enable/Disable Me devices

  @param[in] WhichDevice          Select of Me device
  @param[in] DeviceFuncCtrl       Function control


**/
VOID
MeDeviceControl (
  IN  ME_DEVICE             WhichDevice,
  IN  ME_DEVICE_FUNC_CTRL   DeviceFuncCtrl
  )
{
  UINT16                    DevPsfBase;
  UINT32                    PchPwrmBase;
  UINTN                     PciBaseAdd;

  DevPsfBase = 0;
  PciBaseAdd = 0;
  PchPwrmBaseGet (&PchPwrmBase);

  switch (WhichDevice) {
    case HECI1:
    case HECI2:
    case IDER:
    case SOL:
    case HECI3:
      if (WhichDevice == HECI1) {
        DevPsfBase = R_PCH_PCR_PSF2_T0_SHDW_HECI1_REG_BASE;
      } else if (WhichDevice == HECI2) {
        DevPsfBase = R_PCH_PCR_PSF2_T0_SHDW_HECI2_REG_BASE;
      } else if (WhichDevice == HECI3) {
        DevPsfBase = R_PCH_PCR_PSF2_T0_SHDW_HECI3_REG_BASE;
      } else if (WhichDevice == IDER) {
        DevPsfBase = R_PCH_PCR_PSF2_T0_SHDW_IDER_REG_BASE;
        PciBaseAdd = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, ME_DEVICE_NUMBER, IDER_FUNCTION_NUMBER);
      } else if (WhichDevice == SOL) {
        DevPsfBase = R_PCH_PCR_PSF2_T0_SHDW_KT_REG_BASE;
        PciBaseAdd = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, ME_DEVICE_NUMBER, SOL_FUNCTION_NUMBER);
      } else {
        break;
      }

      switch (DeviceFuncCtrl) {
        case SETUP_HECI_DEVICE_ENABLED:
        case SETUP_HECI_DEVICE_AUTO:
          // Show device (in case it is hidden)
          PchPcrAndThenOr32 (
            PID_PSF2, DevPsfBase + R_PCH_PCR_PSFX_T0_SHDW_CFG_DIS,
            (UINT32)~B_PCH_PCR_PSFX_T0_SHDW_CFG_DIS_CFGDIS,
            0
            );
          DEBUG ((DEBUG_INFO, "[ME] Enabling HECI%d\n", (UINT8)WhichDevice));
          PchPcrAndThenOr32 (
            PID_PSF2, DevPsfBase + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
            (UINT32)~B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS,
            0
            );

          break;
        case SETUP_HECI_DEVICE_DISABLED:
          DEBUG ((DEBUG_INFO, "[ME] Disabling HECI%d\n", (UINT8)WhichDevice));

          PchPcrAndThenOr32 (
            PID_PSF2, DevPsfBase + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
            (UINT32)~0,
            B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS
            );
          break;
        case SETUP_HECI_DEVICE_HIDE:
          // Enable device (in case it is disabled)
          PchPcrAndThenOr32 (
            PID_PSF2, DevPsfBase + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
            (UINT32)~B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS,
            0
            );
          DEBUG ((DEBUG_INFO, "[ME] Hiding HECI%d\n", (UINT8)WhichDevice));
          PchPcrAndThenOr32 (
            PID_PSF2, DevPsfBase + R_PCH_PCR_PSFX_T0_SHDW_CFG_DIS,
            (UINT32)~0,
            B_PCH_PCR_PSFX_T0_SHDW_CFG_DIS_CFGDIS
            );
          break;
          default:
            DEBUG ((DEBUG_WARN, "[ME] Unknown device %d operation (%d)\n", (UINT8)WhichDevice, DeviceFuncCtrl));
          break;
      }
      break;
    ///
    /// Function Disable SUS well lockdown
    ///
    case FDSWL:
      if (DeviceFuncCtrl) {
        MmioOr32 (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1, (UINT32)(B_PCH_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK));
      } else {
        MmioAnd32 (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1, (UINT32)(~B_PCH_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK));
      }
      break;
    default:
    break;
  }
}
