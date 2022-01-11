/** @file
  This file contains SKL specific GPIO information

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
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
#include "GpioLibrary.h"



  //Community       PadOwnOffset                    HostOwnOffset                    GpiIsOffset                   GpiIeOffset                   GpiGpeStsOffset                    GpiGpeEnOffset                    SmiStsOffset                   SmiEnOffset                   NmiStsOffset                   NmiEnOffset                   PadCfgLockOffset                  PadCfgLockTxOffset                  PadCfgOffset                         PadPerGroup
GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_INFO mPchHGpioGroupInfo[] = {
  {PID_GPIOCOM0,    R_PCH_PCR_GPIO_NC_PAD_OWN,     R_PCH_PCR_GPIO_NC_HOSTSW_OWN,     R_PCH_PCR_GPIO_NC_GPI_IS,     R_PCH_PCR_GPIO_NC_GPI_IE,     R_PCH_PCR_GPIO_NC_GPI_GPE_STS,     R_PCH_PCR_GPIO_NC_GPI_GPE_EN,     R_PCH_PCR_GPIO_NC_SMI_STS,     R_PCH_PCR_GPIO_NC_SMI_EN,     R_PCH_PCR_GPIO_NC_NMI_STS,     R_PCH_PCR_GPIO_NC_NMI_EN,     R_PCH_PCR_GPIO_NC_PADCFGLOCK,     R_PCH_PCR_GPIO_NC_PADCFGLOCKTX,     R_PCH_PCR_GPIO_NC_PADCFG_OFFSET,     V_PCH_GPIO_NC_PAD_MAX},           //DNV NORTH_ALL
  {PID_GPIOCOM1,    R_PCH_PCR_GPIO_SC_DFX_PAD_OWN, R_PCH_PCR_GPIO_SC_DFX_HOSTSW_OWN, R_PCH_PCR_GPIO_SC_DFX_GPI_IS, R_PCH_PCR_GPIO_SC_DFX_GPI_IE, R_PCH_PCR_GPIO_SC_DFX_GPI_GPE_STS, R_PCH_PCR_GPIO_SC_DFX_GPI_GPE_EN, NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,     NO_REGISTER_FOR_PROPERTY,      NO_REGISTER_FOR_PROPERTY,     R_PCH_PCR_GPIO_SC_DFX_PADCFGLOCK, R_PCH_PCR_GPIO_SC_DFX_PADCFGLOCKTX, R_PCH_PCR_GPIO_SC_DFX_PADCFG_OFFSET, V_PCH_GPIO_SC_DFX_PAD_MAX},       //DNV SOUTH_DFX
  {PID_GPIOCOM1,    R_PCH_PCR_GPIO_SC0_PAD_OWN,    R_PCH_PCR_GPIO_SC0_HOSTSW_OWN,    R_PCH_PCR_GPIO_SC0_GPI_IS,    R_PCH_PCR_GPIO_SC0_GPI_IE,    R_PCH_PCR_GPIO_SC0_GPI_GPE_STS,    R_PCH_PCR_GPIO_SC0_GPI_GPE_EN,    R_PCH_PCR_GPIO_SC0_SMI_STS,    R_PCH_PCR_GPIO_SC0_SMI_EN,    R_PCH_PCR_GPIO_SC0_NMI_STS,    R_PCH_PCR_GPIO_SC0_NMI_EN,    R_PCH_PCR_GPIO_SC0_PADCFGLOCK,    R_PCH_PCR_GPIO_SC0_PADCFGLOCKTX,    R_PCH_PCR_GPIO_SC0_PADCFG_OFFSET,    V_PCH_GPIO_SC0_PAD_MAX},          //DNV South Community 0
  {PID_GPIOCOM1,    R_PCH_PCR_GPIO_SC1_PAD_OWN,    R_PCH_PCR_GPIO_SC1_HOSTSW_OWN,    R_PCH_PCR_GPIO_SC1_GPI_IS,    R_PCH_PCR_GPIO_SC1_GPI_IE,    R_PCH_PCR_GPIO_SC1_GPI_GPE_STS,    R_PCH_PCR_GPIO_SC1_GPI_GPE_EN,    R_PCH_PCR_GPIO_SC1_SMI_STS,    R_PCH_PCR_GPIO_SC1_SMI_EN,    R_PCH_PCR_GPIO_SC1_NMI_STS,    R_PCH_PCR_GPIO_SC1_NMI_EN,    R_PCH_PCR_GPIO_SC1_PADCFGLOCK,    R_PCH_PCR_GPIO_SC1_PADCFGLOCKTX,    R_PCH_PCR_GPIO_SC1_PADCFG_OFFSET,    V_PCH_GPIO_SC1_PAD_MAX},          //DNV South Community 1
};

//
// SATA reset port to GPIO pin mapping
// SATAGP_x -> GPIO pin y
//
GPIO_PAD_NATIVE_FUNCTION mPchLpSataPortResetToGpioMap[PCH_LP_AHCI_MAX_PORTS] =
{
  {GPIO_SKL_LP_GPP_E0, GpioPadModeNative2},
  {GPIO_SKL_LP_GPP_E1, GpioPadModeNative2},
  {GPIO_SKL_LP_GPP_E2, GpioPadModeNative2}
};

GPIO_PAD_NATIVE_FUNCTION mPchHSataPortResetToGpioMap[PCH_H_AHCI_MAX_PORTS]  =
{
  {GPIO_SKL_H_GPP_E0, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_E1, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_E2, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_F0, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_F1, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_F2, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_F3, GpioPadModeNative2},
  {GPIO_SKL_H_GPP_F4, GpioPadModeNative2}
};

//
// SATADevSlpPin to GPIO pin mapping
// SATA_DEVSLP_x -> GPIO pin y
//
GPIO_PAD_NATIVE_FUNCTION mPchLpSataDevSlpPinToGpioMap[PCH_LP_AHCI_MAX_PORTS] =
{
  {GPIO_SKL_LP_GPP_E4, GpioPadModeNative1},
  {GPIO_SKL_LP_GPP_E5, GpioPadModeNative1},
  {GPIO_SKL_LP_GPP_E6, GpioPadModeNative1}
};

GPIO_PAD_NATIVE_FUNCTION mPchHSataDevSlpPinToGpioMap[PCH_H_AHCI_MAX_PORTS] =
{
  {GPIO_SKL_H_GPP_E4, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_E5, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_E6, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_F5, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_F6, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_F7, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_F8, GpioPadModeNative1},
  {GPIO_SKL_H_GPP_F9, GpioPadModeNative1}
};
