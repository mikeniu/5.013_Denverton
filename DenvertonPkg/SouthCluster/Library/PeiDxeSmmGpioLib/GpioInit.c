/** @file
  This file contains routines for GPIO initialization

@copyright
 Copyright (c) 2013 - 2017 Intel Corporation. All rights reserved
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
#include <Library/BaseLib.h>

#include "GpioLibrary.h"



/**
  This procedure will handle requirement on SATA DEVSLPx pins.

  @param[in]     GpioPad        GPIO pad
  @param[in]     PadMode        GPIO PadMode value
  @param[in out] Dw0Reg         Value for PADCFG_DW0 register
  @param[in out] Dw0RegMask     Mask of bits which will change in PADCFG_DWO register

  @retval None

**/
static
VOID
GpioHandleSataDevSlpPad (
  IN GPIO_PAD                  GpioPad,
  IN UINT32                    PadMode,
  IN OUT UINT32                *Dw0Reg,
  IN OUT UINT32                *Dw0RegMask
  )
{
  //
  // For SATA DEVSLPx pins if used in native 1 mode then ensure that PadRstCfg
  // is set to "00" - Powergood
  //
  if (GpioIsPadASataDevSlpPin (GpioPad, PadMode)) {
    //
    // Set PadRstCfg to Powergood
    //
    *Dw0RegMask |= B_PCH_GPIO_RST_CONF;
    *Dw0Reg |= ((GpioResetPwrGood >> 1) << N_PCH_GPIO_RST_CONF);
  }
}



/**
  This SKL PCH specific procedure will initialize multiple SKL PCH GPIO pins

  @param[in] NumberofItem               Number of GPIO pads to be updated
  @param[in] GpioInitTableAddress       GPIO initialization table

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid group or pad number
**/
// Denverton Aptiov Override start
// Changes added to suppress build errors for >> and <<
static
EFI_STATUS
GpioConfigureSklPch (
  IN UINT32                    NumberOfItems,
  IN GPIO_INIT_CONFIG          *GpioInitTableAddress,
  IN UINT8              LockGpioConfig
  )
{
  UINT32               Index;
  UINT32               Dw0Reg;
  UINT32               Dw0RegMask;
  UINT32               Dw1Reg;
  UINT32               Dw1RegMask;
  UINT32               PadCfgReg;
  UINT64               HostSoftOwnReg[V_PCH_GPIO_GROUP_MAX];
  UINT64               HostSoftOwnRegMask[V_PCH_GPIO_GROUP_MAX];
  UINT64               GpiGpeEnReg[V_PCH_GPIO_GROUP_MAX];
  UINT64               GpiGpeEnRegMask[V_PCH_GPIO_GROUP_MAX];
  UINT64               PadsToLock[V_PCH_GPIO_GROUP_MAX];
  UINT64               PadsToLockTx[V_PCH_GPIO_GROUP_MAX];
  GPIO_GROUP_INFO      *GpioGroupInfo;
  UINTN                GpioGroupInfoLength;
  GPIO_GROUP           GpioGroupOffset;
  UINT32               NumberOfGroups;
  GPIO_PAD_OWN         PadOwnVal;
  GPIO_INIT_CONFIG     *GpioData;
  GPIO_GROUP           Group;
  UINT32               GroupIndex;
  UINT32               PadNumber;
  UINT32               FinalValue;
  UINT32               Data32;
  UINT32               PadMode1, PadMode2;
  PCH_SERIES           PchSeries;

  PchSeries = GetPchSeries ();
  PadOwnVal = GpioPadOwnHost;

  ZeroMem (HostSoftOwnReg, sizeof (HostSoftOwnReg));
  ZeroMem (HostSoftOwnRegMask, sizeof (HostSoftOwnRegMask));
  ZeroMem (GpiGpeEnReg, sizeof (GpiGpeEnReg));
  ZeroMem (GpiGpeEnRegMask, sizeof (GpiGpeEnRegMask));
  ZeroMem (PadsToLock, sizeof (PadsToLock));
  ZeroMem (PadsToLockTx, sizeof (PadsToLockTx));

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  GpioGroupOffset = GpioGetLowestGroup ();
  NumberOfGroups = GpioGetNumberOfGroups ();

  for (Index = 0; Index < NumberOfItems; Index ++) {

    Dw0RegMask = 0;
    Dw0Reg = 0;
    Dw1RegMask = 0;
    Dw1Reg = 0;

    GpioData = &GpioInitTableAddress[Index];

    Group = GpioGetGroupFromGpioPad (GpioData->GpioPad);
    GroupIndex = GpioGetGroupIndexFromGpioPad (GpioData->GpioPad);
    PadNumber = GpioGetPadNumberFromGpioPad (GpioData->GpioPad);

    if (GroupIndex >= V_PCH_GPIO_GROUP_MAX) {
      DEBUG ((DEBUG_ERROR, "GPIO ERROR: Invalid Group Index (GroupIndex=%d, Pad=%d)!\n", GroupIndex, PadNumber));
// @todo SKL PCH: one platform code ready we should add this back      ASSERT (FALSE);
      continue;
    }

DEBUG_CODE_BEGIN();
    if (!(((PchSeries == PchH) && (GPIO_GET_CHIPSET_ID(GpioData->GpioPad) == GPIO_SKL_H_CHIPSET_ID)) ||
        ((PchSeries == PchLp) && (GPIO_GET_CHIPSET_ID(GpioData->GpioPad) == GPIO_SKL_LP_CHIPSET_ID)))) {
      DEBUG ((DEBUG_ERROR, "GPIO ERROR: Incorrect GpioPad define used on this chipset (Group=%d, Pad=%d)!\n", GroupIndex, PadNumber));
      ASSERT (FALSE);
      return EFI_UNSUPPORTED;
    }
DEBUG_CODE_END();

    //
    // Check if group argument exceeds GPIO group range
    //
    if ((Group < GpioGroupOffset) || (Group >= NumberOfGroups + GpioGroupOffset)) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // Check if legal pin number
    //
    if (PadNumber >= GpioGroupInfo[GroupIndex].PadPerGroup){
      return EFI_INVALID_PARAMETER;
    }

DEBUG_CODE_BEGIN();
    //
    // Check if selected GPIO Pad is not owned by CSME/ISH
    //
    GpioGetPadOwnership (GpioData->GpioPad, &PadOwnVal);

    if (PadOwnVal != GpioPadOwnHost) {
      DEBUG ((DEBUG_ERROR, "GPIO WARNING: Accessing pad not owned by host (Group=%d, Pad=%d)! ", GroupIndex, PadNumber));
      if(PadOwnVal==GpioPadOwnCsme)
        DEBUG ((DEBUG_ERROR, "The owner is CSME \n"));
      else if(PadOwnVal==GpioPadOwnIsh)
        DEBUG ((DEBUG_ERROR, "The owner is ISH \n"));
      DEBUG ((DEBUG_ERROR, "** Please make sure the GPIO usage in sync between CSME and BIOS configuration. \n"));
      DEBUG ((DEBUG_ERROR, "** All the GPIO occupied by CSME should not do any configuration by BIOS.\n"));
      continue;
    }

DEBUG_CODE_END();

    //
    // Configure Reset Type (PadRstCfg)
    //
  Dw0RegMask |= ((RShiftU64((GpioData->GpioConfig.PowerConfig & GPIO_CONF_RESET_MASK) , GPIO_CONF_RESET_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_RST_CONF);
  Dw0Reg |= LShiftU64(RShiftU64((GpioData->GpioConfig.PowerConfig & GPIO_CONF_RESET_MASK) , (GPIO_CONF_RESET_BIT_POS + 1)) , N_PCH_GPIO_RST_CONF);

  //
  // Configure how interrupt is triggered (RxEvCfg)
  //
  Dw0RegMask |= ((RShiftU64((GpioData->GpioConfig.InterruptConfig & GPIO_CONF_INT_TRIG_MASK) , GPIO_CONF_INT_TRIG_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_RX_LVL_EDG);
  Dw0Reg |= LShiftU64(RShiftU64((GpioData->GpioConfig.InterruptConfig & GPIO_CONF_INT_TRIG_MASK) , (GPIO_CONF_INT_TRIG_BIT_POS + 1)) , N_PCH_GPIO_RX_LVL_EDG);


  //
  // Configure interrupt generation (GPIRoutIOxAPIC/SCI/SMI/NMI)
  //
  Dw0RegMask |= ((RShiftU64((GpioData->GpioConfig.InterruptConfig & GPIO_CONF_INT_ROUTE_MASK) , GPIO_CONF_INT_ROUTE_BIT_POS) == GpioHardwareDefault)  ? 0x0 : (B_PCH_GPIO_RX_NMI_ROUTE | B_PCH_GPIO_RX_SCI_ROUTE | B_PCH_GPIO_RX_SMI_ROUTE | B_PCH_GPIO_RX_APIC_ROUTE));
  Dw0Reg |= LShiftU64(RShiftU64((GpioData->GpioConfig.InterruptConfig & GPIO_CONF_INT_ROUTE_MASK) , (GPIO_CONF_INT_ROUTE_BIT_POS + 1)) , N_PCH_GPIO_RX_NMI_ROUTE);

  //If CFIO is not Working as GPIO mode, Don't move TxDisabe and RxDisable
  if ( GpioData->GpioConfig.PadMode == GpioPadModeGpio) {
  //
  // Configure GPIO direction (GPIORxDis and GPIOTxDis)
  //
  Dw0RegMask |= ((RShiftU64((GpioData->GpioConfig.Direction & GPIO_CONF_DIR_MASK) , GPIO_CONF_DIR_BIT_POS) == GpioHardwareDefault) ? 0x0 : (B_PCH_GPIO_RXDIS | B_PCH_GPIO_TXDIS));
  Dw0Reg |= LShiftU64(RShiftU64((GpioData->GpioConfig.Direction & GPIO_CONF_DIR_MASK) , (GPIO_CONF_DIR_BIT_POS + 1)) , N_PCH_GPIO_TXDIS);
  }
  //
  // Configure GPIO input inversion (RXINV)
  //
  Dw0RegMask |= ((RShiftU64((GpioData->GpioConfig.Direction & GPIO_CONF_INV_MASK) , GPIO_CONF_INV_BIT_POS) == GpioHardwareDefault) ?  0x0 : B_PCH_GPIO_RXINV);
  Dw0Reg |= LShiftU64(RShiftU64((GpioData->GpioConfig.Direction & GPIO_CONF_INV_MASK) , (GPIO_CONF_INV_BIT_POS + 1)) , N_PCH_GPIO_RXINV);

  //
  // Configure GPIO output state (GPIOTxState)
  //
  Dw0RegMask |= ((RShiftU64((GpioData->GpioConfig.OutputState & GPIO_CONF_OUTPUT_MASK) , GPIO_CONF_OUTPUT_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_TX_STATE);
  Dw0Reg |= LShiftU64(RShiftU64((GpioData->GpioConfig.OutputState & GPIO_CONF_OUTPUT_MASK) , (GPIO_CONF_OUTPUT_BIT_POS + 1)) , N_PCH_GPIO_TX_STATE);

  //
  // Configure GPIO RX raw override to '1' (RXRAW1)
  //
  Dw0RegMask |= ((RShiftU64((GpioData->GpioConfig.OtherSettings & GPIO_CONF_RXRAW_MASK) , GPIO_CONF_RXRAW_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_RX_RAW1);
  Dw0Reg |= LShiftU64(RShiftU64((GpioData->GpioConfig.OtherSettings & GPIO_CONF_RXRAW_MASK) , (GPIO_CONF_RXRAW_BIT_POS + 1)) , N_PCH_GPIO_RX_RAW1);

  //
  // Configure GPIO Pad Mode (PMode)
  //
  Dw0RegMask |= ((RShiftU64((GpioData->GpioConfig.PadMode & GPIO_CONF_PAD_MODE_MASK) , GPIO_CONF_PAD_MODE_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_PAD_MODE);
  Dw0Reg |= LShiftU64(RShiftU64((GpioData->GpioConfig.PadMode & GPIO_CONF_PAD_MODE_MASK) , (GPIO_CONF_PAD_MODE_BIT_POS + 1)) , N_PCH_GPIO_PAD_MODE);

  //
  // Configure GPIO termination (Term)
  //
  Dw1RegMask |= ((RShiftU64((GpioData->GpioConfig.ElectricalConfig & GPIO_CONF_TERM_MASK) , GPIO_CONF_TERM_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_TERM);
  Dw1Reg |= LShiftU64(RShiftU64((GpioData->GpioConfig.ElectricalConfig & GPIO_CONF_TERM_MASK) , (GPIO_CONF_TERM_BIT_POS + 1)) , N_PCH_GPIO_TERM);

  //
  // Configure GPIO pad tolerance (padtol)
  //
  Dw1RegMask |= ((RShiftU64((GpioData->GpioConfig.ElectricalConfig & GPIO_CONF_PADTOL_MASK) , GPIO_CONF_PADTOL_BIT_POS) == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_PADTOL);
  Dw1Reg |= LShiftU64(RShiftU64((GpioData->GpioConfig.ElectricalConfig & GPIO_CONF_PADTOL_MASK) , (GPIO_CONF_PADTOL_BIT_POS + 1)) , N_PCH_GPIO_PADTOL);

    //
    // Check for additional requirements on setting PADCFG register
    //

    //
    // Create PADCFG register offset using group and pad number
    //
    PadCfgReg = 0x8 * PadNumber + GpioGroupInfo[GroupIndex].PadCfgOffset;
    Data32 = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, PadCfgReg));

    FinalValue = ((Data32 & (~Dw0RegMask) | Dw0Reg));

    PadMode1 = (Data32 & B_PCH_GPIO_PAD_MODE)>> N_PCH_GPIO_PAD_MODE;
    PadMode2 = (Dw0Reg & B_PCH_GPIO_PAD_MODE)>> N_PCH_GPIO_PAD_MODE;

    if (((Data32 & B_PCH_GPIO_PAD_MODE) != (FinalValue & B_PCH_GPIO_PAD_MODE)) || (PadMode2 == 0)) {
        DEBUG ((DEBUG_INFO, "Changing GpioPad PID: %x Offset: 0x%x PadModeP1: %d P2: %d ", GpioGroupInfo[GroupIndex].Community, PadCfgReg, PadMode1, PadMode2));
        DEBUG ((DEBUG_INFO, "R: 0x%08x Fx%08x !\n", Data32, FinalValue));
        //
        // Write PADCFG DW0 register
        //
        MmioAndThenOr32 (
          (UINTN)PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, PadCfgReg),
          ~(UINT32)Dw0RegMask,
          (UINT32)Dw0Reg
        );
    }

    Data32 = MmioRead32(PCH_PCR_ADDRESS(GpioGroupInfo[GroupIndex].Community, PadCfgReg + 0x4));
    FinalValue = ((Data32 & (~Dw1RegMask)) | Dw1Reg);
    if (Data32 != FinalValue) {
        //
        // Write PADCFG DW1 register
        //
        MmioAndThenOr32 (
          (UINTN)PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, PadCfgReg + 0x4),
          ~(UINT32)Dw1RegMask,
          (UINT32)Dw1Reg
        );
    }

    //
    // Update value to be programmed in HOSTSW_OWN register
    //
    HostSoftOwnRegMask[GroupIndex] |= LShiftU64((UINT64)GpioData->GpioConfig.HostSoftPadOwn & 0x1, PadNumber);
    HostSoftOwnReg[GroupIndex] |= LShiftU64((UINT64) RShiftU64(GpioData->GpioConfig.HostSoftPadOwn , 0x1),PadNumber);

    //
    // Update value to be programmed in GPI_GPE_EN register
    //
    GpiGpeEnRegMask[GroupIndex] |= LShiftU64((UINT64)(GpioData->GpioConfig.InterruptConfig & 0x1), PadNumber);
    GpiGpeEnReg[GroupIndex] |= LShiftU64((UINT64)RShiftU64((GpioData->GpioConfig.InterruptConfig & GpioIntSci) , 3), PadNumber);

    //
    // Update information on Pad Configuration Lock
    //
    PadsToLock[GroupIndex] |= LShiftU64((UINT64)((RShiftU64(GpioData->GpioConfig.LockConfig , 0x1)) & 0x1), PadNumber);

    //
    // Update information on Pad Configuration Lock Tx
    //
    PadsToLockTx[GroupIndex] |= LShiftU64((UINT64)((RShiftU64(GpioData->GpioConfig.LockConfig , 0x2)) & 0x1), PadNumber);
  }
// Denverton Aptiov Override end

  //
  // SKL PCH BWG 16.6. PCH-LP GPP_A_12 Special Handling
  //
  if (PchSeries == PchLp) {
    GpioGetPadOwnership (GPIO_SKL_LP_GPP_A12, &PadOwnVal);

    //
    // If the pad is host-own, BIOS has to always lock this pad after being initialized
    //
    if (PadOwnVal == GpioPadOwnHost) {
      //
      // Set PadCfgLock for GPP_A_12
      //
      GpioLockPadCfg (GPIO_SKL_LP_GPP_A12);
    }
  }

  for (Index = 0; Index < NumberOfGroups; Index++) {
    //
    // Write HOSTSW_OWN registers
    //
    if (GpioGroupInfo[Index].HostOwnOffset != NO_REGISTER_FOR_PROPERTY) {
      MmioAndThenOr32 (
        (UINTN)PCH_PCR_ADDRESS (GpioGroupInfo[Index].Community, GpioGroupInfo[Index].HostOwnOffset),
        ~(UINT32)(HostSoftOwnRegMask[Index] & 0xFFFFFFFF),
        (UINT32)(HostSoftOwnReg[Index] & 0xFFFFFFFF)
        );
      MmioAndThenOr32 (
        (UINTN)PCH_PCR_ADDRESS (GpioGroupInfo[Index].Community, GpioGroupInfo[Index].HostOwnOffset + 0x4),
        ~(UINT32)(RShiftU64(HostSoftOwnRegMask[Index],32) ),
        (UINT32)( RShiftU64(HostSoftOwnReg[Index], 32) )
        );
    }

    //
    // Write GPI_GPE_EN registers
    //
    if (GpioGroupInfo[Index].GpiGpeEnOffset != NO_REGISTER_FOR_PROPERTY) {
      MmioAndThenOr32 (
        (UINTN)PCH_PCR_ADDRESS (GpioGroupInfo[Index].Community, GpioGroupInfo[Index].GpiGpeEnOffset),
        ~(UINT32)(GpiGpeEnRegMask[Index] & 0xFFFFFFFF),
        (UINT32)(GpiGpeEnReg[Index] & 0xFFFFFFFF)
        );
      MmioAndThenOr32 (
        (UINTN)PCH_PCR_ADDRESS (GpioGroupInfo[Index].Community, GpioGroupInfo[Index].GpiGpeEnOffset + 0x4),
        ~(UINT32)(RShiftU64 (GpiGpeEnRegMask[Index], 32)),
        (UINT32)(RShiftU64 (GpiGpeEnReg[Index], 32))
        );
    }
    //
    // Write Pad Configuration Lock
    //
    if (LockGpioConfig == TRUE && PadsToLock[Index] != 0) {
      GpioLockPadCfgForGroupDw (Index + GpioGroupOffset, 0, (UINT32)(PadsToLock[Index] & 0xFFFFFFFF));
      if(GpioGroupInfo[Index].PadPerGroup > 32)
         GpioLockPadCfgForGroupDw (Index + GpioGroupOffset, 1, (UINT32)(RShiftU64 (PadsToLock[Index], 32)));
    }

    //
    // Write Pad Configuration Lock Tx
    //
    if (LockGpioConfig == TRUE && PadsToLockTx[Index] != 0) {
      GpioLockPadCfgTxForGroupDw (Index + GpioGroupOffset, 0, (UINT32)(PadsToLockTx[Index] & 0xFFFFFFFF) );
      if(GpioGroupInfo[Index].PadPerGroup > 32)
         GpioLockPadCfgTxForGroupDw (Index + GpioGroupOffset, 1, (UINT32)(RShiftU64 (PadsToLockTx[Index], 32)));
    }
  }
  return EFI_SUCCESS;
}

/**
  This procedure will clear all status bits of any GPIO interrupts.

  @param[in] none

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid group or pad number
**/
static
EFI_STATUS
GpioClearAllGpioInterrupts (
  VOID
  )
{
  GPIO_GROUP           Group;
  GPIO_GROUP_INFO      *GpioGroupInfo;
  GPIO_GROUP           GpioGroupLowest;
  GPIO_GROUP           GpioGroupHighest;
  UINT32               GroupIndex;
  UINTN                GpioGroupInfoLength;
  UINT32               DwOffset;
  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);

  GpioGroupLowest = GpioGetLowestGroup ();
  GpioGroupHighest = GpioGetHighestGroup ();

  for (Group = GpioGroupLowest; Group <= GpioGroupHighest; Group++) {
    GroupIndex = GpioGetGroupIndexFromGroup (Group);
    //
    // Check if group has GPI IS register
    //
    if (GpioGroupInfo[Group].GpiIsOffset != NO_REGISTER_FOR_PROPERTY) {
      //
      // Clear all GPI_IS Status bits by writing '1'
      //
      for(DwOffset = 0; DwOffset < GpioGetMaxDwOffsetFromGroup(Group); DwOffset++) {
          MmioWrite32 (
            PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiIsOffset + 4 * DwOffset),
            (UINT32)0xFFFFFFFF
            );
      }
    }

    //
    // Check if group has GPI_GPE_STS register
    //
    if (GpioGroupInfo[GroupIndex].GpiGpeStsOffset != NO_REGISTER_FOR_PROPERTY) {
      //
      // Clear all GPI_GPE_STS Status bits by writing '1'
      //
      for(DwOffset = 0; DwOffset < GpioGetMaxDwOffsetFromGroup(Group); DwOffset++) {
          MmioWrite32 (
            PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].GpiGpeStsOffset + 4 * DwOffset),
            (UINT32)0xFFFFFFFF
            );
      }
    }

    //
    // Check if group has SMI_STS register
    //
    if (GpioGroupInfo[GroupIndex].SmiStsOffset != NO_REGISTER_FOR_PROPERTY) {
      //
      // Clear all SMI_STS Status bits by writing '1'
      //
      for(DwOffset = 0; DwOffset < GpioGetMaxDwOffsetFromGroup(Group); DwOffset++) {
          MmioWrite32 (
            PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].SmiStsOffset + 4 * DwOffset),
            (UINT32)0xFFFFFFFF
            );
      }
    }

    //
    // Check if group has NMI_STS register
    //
    if (GpioGroupInfo[GroupIndex].NmiStsOffset != NO_REGISTER_FOR_PROPERTY) {
      //
      // Clear all NMI_STS Status bits by writing '1'
      //
      for(DwOffset = 0; DwOffset < GpioGetMaxDwOffsetFromGroup(Group); DwOffset++) {
          MmioWrite32 (
            PCH_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, GpioGroupInfo[GroupIndex].NmiStsOffset + 4 * DwOffset),
            (UINT32)0xFFFFFFFF
            );

      }
    }

  }
  return EFI_SUCCESS;
}

/**
  This procedure will initialize multiple GPIO pins. Use GPIO_INIT_CONFIG structure.
  Structure contains fields that can be used to configure each pad.
  Pad not configured using GPIO_INIT_CONFIG will be left with hardware default values.
  Separate fields could be set to hardware default if it does not matter, except
  GpioPad and PadMode.
  Some GpioPads are configured and switched to native mode by RC, those include:
  SerialIo pins, ISH pins, ClkReq Pins

  @param[in] NumberofItem               Number of GPIO pads to be updated
  @param[in] GpioInitTableAddress       GPIO initialization table

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_INVALID_PARAMETER         Invalid group or pad number
**/
EFI_STATUS
GpioConfigurePads (
  IN UINT32                    NumberOfItems,
  IN GPIO_INIT_CONFIG          *GpioInitTableAddress,
  IN UINT8                     LockGpioConfig
)
{
  EFI_STATUS   Status;
  Status =  GpioConfigureSklPch (NumberOfItems, GpioInitTableAddress, LockGpioConfig);
  GpioClearAllGpioInterrupts ();
  return Status;
}
