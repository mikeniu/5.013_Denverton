/** @file
  PCH HSIO Library.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

@copyright
 Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <PchAccess.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchHsioLib.h>



/**
  The function returns the Port Id and lane owner for the specified lane

  @param[in]  PhyMode             Phymode that needs to be checked
  @param[out] PortId              Common Lane End Point ID
  @param[out] LaneOwner           Lane Owner

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid lane number
**/
EFI_STATUS
EFIAPI
PchGetLaneInfo (
  IN  UINT32                            LaneNum,
  OUT UINT8                             *PortId,
  OUT UINT8                             *LaneOwner
  )
{
  PCH_SERIES  PchSeries;
  UINT32 Los1;
  UINT32 Los2;
  UINT32 Los3;

  Los1 = 0;
  Los2 = 0;
  Los3 = 0;

  PchSeries = GetPchSeries ();

  if ( (LaneNum > 19) || (PchSeries != PchH)) {
    return EFI_INVALID_PARAMETER;
  }
  if (LaneNum < 8) {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
    *PortId = PID_MODPHY0;
  } else if (LaneNum < 16) {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);
    *PortId = PID_MODPHY1;
  } else if (LaneNum < 24) {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
    *PortId = PID_MODPHY2;
  }


  if (PchSeries == PchLp) {
    switch (LaneNum) {
      case 0:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L0O));
        *PortId = PID_MODPHY0;
        break;
      case 1:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L1O)>>4);
        *PortId = PID_MODPHY0;
        break;
      case 2:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L2O)>>8);
        *PortId = PID_MODPHY0;
        break;
      case 3:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L3O)>>12);
        *PortId = PID_MODPHY0;
        break;
      case 4:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L4O)>>16);
        *PortId = PID_MODPHY0;
        break;
      case 5:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L5O)>>20);
        *PortId = PID_MODPHY0;
        break;
      case 6:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L6O)>>24);
        *PortId = PID_MODPHY1;
        break;
      case 7:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L7O)>>28);
        *PortId = PID_MODPHY1;
        break;
      case 8:
        *LaneOwner = (UINT8) (Los2 & B_PCH_PCR_FIA_L8O);
        *PortId = PID_MODPHY1;
        break;
      case 9:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L9O)>>4);
        *PortId = PID_MODPHY1;
        break;
      case 10:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L10O)>>8);
        *PortId = PID_MODPHY2;
        break;
      case 11:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L11O)>>12);
        *PortId = PID_MODPHY2;
        break;
      case 12:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L12O)>>16);
        *PortId = PID_MODPHY2;
        break;
      case 13:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L13O)>>20);
        *PortId = PID_MODPHY2;
        break;
      case 14:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L14O)>>24);
        *PortId = PID_MODPHY2;
        break;
      case 15:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L15O)>>28);
        *PortId = PID_MODPHY2;
        break;
    }
  } else {
    switch (LaneNum) {
      case 0:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L0O));
        break;
      case 1:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L1O)>>4);
        break;
      case 2:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L2O)>>8);
        break;
      case 3:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L3O)>>12);
        break;
      case 4:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L4O)>>16);
        break;
      case 5:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L5O)>>20);
        break;
      case 6:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L6O)>>24);
        break;
      case 7:
        *LaneOwner = (UINT8) ((Los1 & B_PCH_PCR_FIA_L7O)>>28);
        break;
      case 8:
        *LaneOwner = (UINT8) (Los2 & B_PCH_PCR_FIA_L8O);
        break;
      case 9:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L9O)>>4);
        break;
      case 10:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L10O)>>8);
        break;
      case 11:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L11O)>>12);
        *PortId = PID_MODPHY1;
        break;
      case 12:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L12O)>>16);
        break;
      case 13:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L13O)>>20);
        break;
      case 14:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L14O)>>24);
        break;
      case 15:
        *LaneOwner = (UINT8) ((Los2 & B_PCH_PCR_FIA_L15O)>>28);
        break;
      case 16:
        *LaneOwner = (UINT8) (Los3 & B_PCH_PCR_FIA_L16O);
        break;
      case 17:
        *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L17O)>>4);
        break;
      case 18:
        *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L18O)>>8);
        break;
      case 19:
        *LaneOwner = (UINT8) ((Los3 & B_PCH_PCR_FIA_L19O)>>12);
        break;
    default:
      ASSERT(FALSE);
    }
  }
  return EFI_SUCCESS;
}

/**
  Determine the lane number of a specified port

  @param[out] LaneNum                   GBE Lane Number

  @retval EFI_SUCCESS                   Lane number valid.
  @retval EFI_UNSUPPORTED               Incorrect input device port
**/
EFI_STATUS
PchGetGbeLaneNum (
  UINT8               *LaneNum
  ) 
{
  PCH_SERIES  PchSeries;
  UINT32 Los1;
  UINT32 Los2;
  UINT32 Los3;

  PchSeries = GetPchSeries ();
  PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);

  if (PchSeries == PchLp) {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
    if (((Los1 & B_PCH_PCR_FIA_L6O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 6;
      return EFI_SUCCESS;
    } else if (((Los1 & B_PCH_PCR_FIA_L7O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 7;
      return EFI_SUCCESS;
    } else if ((Los2 & B_PCH_PCR_FIA_L8O) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 8;
      return EFI_SUCCESS;
    } else if (((Los2 & B_PCH_PCR_FIA_L12O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 12;
      return EFI_SUCCESS;
    } else if (((Los2 & B_PCH_PCR_FIA_L13O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 13;
      return EFI_SUCCESS;
    }     
  } else {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
    if (((Los2 & B_PCH_PCR_FIA_L9O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 9;
      return EFI_SUCCESS;
    } else if (((Los2 & B_PCH_PCR_FIA_L10O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 10;
      return EFI_SUCCESS;
    } else if (((Los3 & B_PCH_PCR_FIA_L18O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 18;
      return EFI_SUCCESS;
    } else if (((Los3 & B_PCH_PCR_FIA_L21O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 21;
      return EFI_SUCCESS;
    } else if (((Los3 & B_PCH_PCR_FIA_L22O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_GBE) {
      *LaneNum = 22;
      return EFI_SUCCESS;
    }
  }
  return EFI_UNSUPPORTED;
}

/**
  Determine the lane number of a specified port

  @param[in]  Usb3LaneIndex             USB3 Lane Index
  @param[out] LaneNum                   Lane Number

  @retval EFI_SUCCESS                   Lane number valid.
  @retval EFI_UNSUPPORTED               Incorrect input device port
**/
EFI_STATUS
PchGetUsb3LaneNum (
  UINT32              Usb3LaneIndex,
  UINT8               *LaneNum
  ) 
{
  PCH_SERIES  PchSeries;
  UINT32 Los1;
  UINT32 Los2;
  UINT32 Los3;
  UINT32 Los4;

  PchSeries = GetPchSeries ();
  PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
  PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);

  if (PchSeries == PchLp) {
    switch (Usb3LaneIndex) {
      case 0:
        if ((Los1 & B_PCH_PCR_FIA_L0O) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 0;
          return EFI_SUCCESS;
        }
        break;
      case 1:
        if (((Los1 & B_PCH_PCR_FIA_L1O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 1;
          return EFI_SUCCESS;
        }
        break;
      case 2:
        if (((Los1 & B_PCH_PCR_FIA_L2O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 2;
          return EFI_SUCCESS;
        }
        break;
      case 3:
        if (((Los1 & B_PCH_PCR_FIA_L3O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 3;
          return EFI_SUCCESS;
        }
        break;
      case 4:
        if (((Los1 & B_PCH_PCR_FIA_L4O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 4;
          return EFI_SUCCESS;
        }
        break;
      case 5:
        if (((Los1 & B_PCH_PCR_FIA_L5O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 5;
          return EFI_SUCCESS;
        }
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported USB3 Lane Index"));
        ASSERT (FALSE);
        return EFI_UNSUPPORTED;
        break;
    }
  } else {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS4_REG_BASE, &Los4);
    switch (Usb3LaneIndex) {
      case 0:
        if ((Los1 & B_PCH_PCR_FIA_L0O) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 0;
          return EFI_SUCCESS;
        }
        break;
      case 1:
        if (((Los1 & B_PCH_PCR_FIA_L1O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 1;
          return EFI_SUCCESS;
        }
        break;
      case 2:
        if (((Los1 & B_PCH_PCR_FIA_L2O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 2;
          return EFI_SUCCESS;
        }
        break;
      case 3:
        if (((Los1 & B_PCH_PCR_FIA_L3O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 3;
          return EFI_SUCCESS;
        }
        break;
      case 4:
        if (((Los1 & B_PCH_PCR_FIA_L4O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 4;
          return EFI_SUCCESS;
        }
        break;
      case 5:
        if (((Los1 & B_PCH_PCR_FIA_L5O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 5;
          return EFI_SUCCESS;
        }
        break;
      case 6:
        if (((Los1 & B_PCH_PCR_FIA_L6O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 6;
          return EFI_SUCCESS;
        }
        break;
      case 7:
        if (((Los1 & B_PCH_PCR_FIA_L7O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 7;
          return EFI_SUCCESS;
        }
        break;
      case 8:
        if ((Los2 & B_PCH_PCR_FIA_L8O) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 8;
          return EFI_SUCCESS;
        }
        break;
      case 9:
        if (((Los2 & B_PCH_PCR_FIA_L9O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_USB3) {
          *LaneNum = 9;
          return EFI_SUCCESS;
        }
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported USB3 Lane Index"));
        ASSERT (FALSE);
        return EFI_UNSUPPORTED;
        break;
    }
  }
  return EFI_UNSUPPORTED;
}

/**
  Determine the lane number of a specified port

  @param[in]  SataLaneIndex             Sata Lane Index
  @param[out] LaneNum                   Lane Number

  @retval EFI_SUCCESS                   Lane number valid.
  @retval EFI_UNSUPPORTED               Incorrect input device port
**/
EFI_STATUS
PchGetSataLaneNum (
  UINT32              SataLaneIndex,
  UINT8               *LaneNum
  ) 
{
  PCH_SERIES  PchSeries;
  UINT32 Los1;
  UINT32 Los2;
  UINT32 Los3;

  PchSeries = GetPchSeries ();
  PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
  PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);

  if (PchSeries != PchH) {
      DEBUG ((DEBUG_ERROR, "Unsupported PchSeries"));
      ASSERT (FALSE);
  } else {
	PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
    switch (SataLaneIndex) {
      case 0:
        if (((Los1 & B_PCH_PCR_FIA_L4O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 4;
        return EFI_SUCCESS;
        }
        break;
      case 1:
        if (((Los1 & B_PCH_PCR_FIA_L5O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 5;
        return EFI_SUCCESS;
        }
        break;
      case 2:
        if (((Los1 & B_PCH_PCR_FIA_L6O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 6;
        return EFI_SUCCESS;
        }
        break;
      case 3:
        if (((Los1 & B_PCH_PCR_FIA_L7O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 7;
        return EFI_SUCCESS;
        }
        break;
      case 4:
        if (((Los2 & B_PCH_PCR_FIA_L8O) >> 0) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 8;
        return EFI_SUCCESS;
        }
        break;
      case 5:
        if (((Los2 & B_PCH_PCR_FIA_L9O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 9;
        return EFI_SUCCESS;
        }
        break;
      case 6:
        if (((Los2 & B_PCH_PCR_FIA_L10O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 10;
        return EFI_SUCCESS;
        }
        break;
      case 7:
        if (((Los2 & B_PCH_PCR_FIA_L11O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 11;
        return EFI_SUCCESS;
        }
        break;
      case 8:
        if (((Los2 & B_PCH_PCR_FIA_L12O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 12;
        return EFI_SUCCESS;
        }
        break;
      case 9:
        if (((Los2 & B_PCH_PCR_FIA_L13O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 13;
        return EFI_SUCCESS;
        }
        break;
      case 10:
        if (((Los2 & B_PCH_PCR_FIA_L14O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 14;
        return EFI_SUCCESS;
        }
        break;
      case 11:
        if (((Los2 & B_PCH_PCR_FIA_L15O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 15;
        return EFI_SUCCESS;
        }
        break;
      case 12:
        if (((Los3 & B_PCH_PCR_FIA_L16O) >> 0) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 16;
        return EFI_SUCCESS;
        }
        break;
      case 13:
        if (((Los3 & B_PCH_PCR_FIA_L17O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 17;
        return EFI_SUCCESS;
        }
        break;
      case 14:
        if (((Los3 & B_PCH_PCR_FIA_L18O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 18;
        return EFI_SUCCESS;
        }
        break;
      case 15:
        if (((Los3 & B_PCH_PCR_FIA_L19O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        *LaneNum = 19;
        return EFI_SUCCESS;
        }
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported SATA Lane Index"));
        ASSERT (FALSE);
    }
  }
  return EFI_UNSUPPORTED;
}

/**
  Determine the lane number of a specified port

  @param[in]  PcieLaneIndex             PCIE Root Port Lane Index
  @param[out] LaneNum                   Lane Number

  @retval EFI_SUCCESS                   Lane number valid.
  @retval EFI_UNSUPPORTED               Incorrect input device port
**/
EFI_STATUS
PchGetPcieLaneNum (
  UINT32              PcieLaneIndex,
  UINT8               *LaneNum
  ) 
{
  PCH_SERIES  PchSeries;
  UINT32 Los1;
  UINT32 Los2;
  UINT32 Los3;

  PchSeries = GetPchSeries ();
  if (PchSeries != PchH) {
      DEBUG ((DEBUG_ERROR, "Unsupported PchSeries for PCIE!!"));
      ASSERT (FALSE);
  } else {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
	PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
    switch (PcieLaneIndex) {
      case 0:
        if ( (Los1 & B_PCH_PCR_FIA_L0O)  == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 0;
          return EFI_SUCCESS;
        }
        break;
      case 1:
        if (((Los1 & B_PCH_PCR_FIA_L1O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 1;
          return EFI_SUCCESS;
        }
        break;
      case 2:
        if (((Los1 & B_PCH_PCR_FIA_L2O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 2;
          return EFI_SUCCESS;
        }
        break;
      case 3:
        if (((Los1 & B_PCH_PCR_FIA_L3O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 3;
          return EFI_SUCCESS;
        }
        break;
      case 4:
        if (((Los1 & B_PCH_PCR_FIA_L4O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 4;
          return EFI_SUCCESS;
        }
        break;
      case 5:
        if (((Los1 & B_PCH_PCR_FIA_L5O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 5;
          return EFI_SUCCESS;
        }
        break;
      case 6:
        if (((Los1 & B_PCH_PCR_FIA_L6O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 6;
          return EFI_SUCCESS;
        }
        break;
      case 7:
        if (((Los1 & B_PCH_PCR_FIA_L7O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 7;
          return EFI_SUCCESS;
        }
        break;
      case 8:
        if ( (Los2 & B_PCH_PCR_FIA_L8O)  == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 8;
          return EFI_SUCCESS;
        }
        break;
      case 9:
        if (((Los2 & B_PCH_PCR_FIA_L9O) >> 4) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 9;
          return EFI_SUCCESS;
        }
        break;
      case 10:
        if (((Los2 & B_PCH_PCR_FIA_L10O) >> 8) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 10;
          return EFI_SUCCESS;
        }
        break;
      case 11:
        if (((Los2 & B_PCH_PCR_FIA_L11O) >> 12) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 11;
          return EFI_SUCCESS;
        }
        break;
      case 12:
        if (((Los2 & B_PCH_PCR_FIA_L12O) >> 16) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 12;
          return EFI_SUCCESS;
        }
        break;
      case 13:
        if (((Los2 & B_PCH_PCR_FIA_L13O) >> 20) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 13;
          return EFI_SUCCESS;
        }
        break;
      case 14:
        if (((Los2 & B_PCH_PCR_FIA_L14O) >> 24) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 14;
          return EFI_SUCCESS;
        }
        break;
      case 15:
        if (((Los2 & B_PCH_PCR_FIA_L15O) >> 28) == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
          *LaneNum = 15;
          return EFI_SUCCESS;
        }
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported PCIE Root Port Lane Index"));
        ASSERT (FALSE);
        return EFI_UNSUPPORTED;
    }
  }

  return EFI_UNSUPPORTED;
}

/**
  Get HSIO lane representation needed to perform any operation on the lane.

  @param[in]  LaneIndex  Number of the HSIO lane
  @param[out] HsioLane   HSIO lane representation
**/
VOID
HsioGetLane (
  IN   UINT8       LaneIndex,
  OUT  HSIO_LANE   *HsioLane
  )
{
	  // DNV - H
	  // iolane  0 -  7 : 0xB9 -
	  // iolane  8 - 15 : 0xBA -
	  // iolane 16 - 23 : 0xBB -

  static UINT8 IoLanesH[] = { 0, 8, 16, 24 };
  static UINT8 Pids[] = { PID_MODPHY0, PID_MODPHY1, PID_MODPHY2 };
  UINT8 *IoLanes;
  UINT8  PidMax;
  UINT32 Index;

  ASSERT (HsioLane != NULL);

  if(HsioLane == NULL){
	DEBUG ((EFI_D_ERROR, "\nERROR!! HsioGetLane() - HsioLane pointer is null!\n"));
	return;
  }

    IoLanes = IoLanesH;
    PidMax = 3;
    ASSERT (LaneIndex < IoLanes[PidMax]);
  
  for (Index = 0; Index < PidMax; ++Index) {
    if (LaneIndex < IoLanes[Index + 1]) {
      HsioLane->Index = LaneIndex;
      HsioLane->Pid = Pids[Index];
      HsioLane->Base = (LaneIndex * PCH_HSIO_iolaneX_RX) + PCH_HSIO_iolane0_RX;
      return;
    }
  }
  ASSERT (FALSE);
}

