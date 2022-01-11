/** @file
  Pch common library for PCH INIT PEI/DXE/SMM modules

@copyright
  Copyright (c) 2013 - 2014 Intel Corporation. All rights reserved
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

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include <PchAccess.h>
#include <PchPolicyCommon.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchP2sbLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/PchInfoLib.h>
#include <Library/S3BootScriptLib.h>

#include "SimicsDebug.h"

/**
  This function lock down the P2sb SBI before going into OS.
  This only apply to PCH B0 onward.

  @param[in] P2sbConfig                 The PCH policy for P2SB configuration
**/
VOID
ConfigureP2sbSbiLock (
  IN  CONST PCH_P2SB_CONFIG             *P2sbConfig
  )
{
  UINTN                                 P2sbBase;
  BOOLEAN                               P2sbOrgStatus;
  PCH_STEPPING                          PchStep;
  PCH_SERIES                            PchSeries;

  PchStep     = PchStepping ();
  PchSeries   = GetPchSeries ();

  if ((P2sbConfig->SbiUnlock == FALSE) &&
      (((PchSeries == PchLp) && (PchStep >= PchLpB0)) || ((PchSeries == PchH) && (PchStep >= PchHB0))))
  {
    P2sbBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_P2SB, PCI_FUNCTION_NUMBER_PCH_P2SB);
    PchRevealP2sb (P2sbBase, &P2sbOrgStatus);

    ///
    /// Set P2SB PCI Offset 0xE0[31] to 1 to lock down SBI interface.
    /// @note: SbiLock is write once. Please make sure it's not touched earlier.
    ///
    MmioOr8 (P2sbBase + R_PCH_P2SB_E0 + 3, BIT7);

    if (!P2sbOrgStatus) {
      PchHideP2sb (P2sbBase);
    }
  }
}

/**
  Bios will remove the host accessing right to PSF register range
  prior to any 3rd party code execution.

  1) Set P2SB PCI offset C4h [29, 28, 27, 26] to [1, 1, 1, 1]
  2) Set the "Endpoint Mask Lock!", P2SB PCI offset E2h bit[1] to 1.

  @param[in] P2sbConfig                 The PCH policy for P2SB configuration
**/
VOID
RemovePsfAccess (
  IN  CONST PCH_P2SB_CONFIG             *P2sbConfig
  )
{
  UINTN                                 P2sbBase;
  BOOLEAN                               P2sbOrgStatus;

  if (P2sbConfig->PsfUnlock == TRUE) {
    return;
  }

  P2sbBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_P2SB, PCI_FUNCTION_NUMBER_PCH_P2SB);
  PchRevealP2sb (P2sbBase, &P2sbOrgStatus);

  MmioOr32 (P2sbBase + R_PCH_P2SB_EPMASK1, BIT4);  // PSF0: 0x24
  MmioOr32 (P2sbBase + R_PCH_P2SB_EPMASK5, BIT9);  // PSF1: 0xA9
  MmioOr32 (P2sbBase + R_PCH_P2SB_EPMASK5, BIT0);  // PSF2: 0xA0
  MmioOr32 (P2sbBase + R_PCH_P2SB_EPMASK6, BIT6);  // PSF3: 0xC6
  MmioOr32 (P2sbBase + R_PCH_P2SB_EPMASK5, BIT23); // PSFS0: 0xB7
  MmioOr32 (P2sbBase + R_PCH_P2SB_EPMASK5, BIT24); // PSFS1: 0xB8
  MmioOr32 (P2sbBase + R_PCH_P2SB_EPMASK7, BIT3);  // MEPSF: 0xE3
  MmioOr32 (P2sbBase + R_PCH_P2SB_EPMASK7, BIT19); // IEPSF: 0xF3
  MmioOr8 (P2sbBase + R_PCH_P2SB_E0 + 2, BIT1);

  if (!P2sbOrgStatus) {
    PchHideP2sb (P2sbBase);
  }
}

/**
  Configure PMC static function disable lock
**/
VOID
ConfigurePmcStaticFunctionDisableLock (
  VOID
  )
{
  UINT32                                PchPwrmBase;
  PchPwrmBaseGet (&PchPwrmBase);

  MmioOr32 (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1, B_PCH_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK);
}

/**
  Print registers value

  @param[in] PrintMmioBase       Mmio base address
  @param[in] PrintSize           Number of registers
  @param[in] OffsetFromBase      Offset from mmio base address

  @retval None
**/
VOID
PrintRegisters (
  IN  UINTN        PrintMmioBase,
  IN  UINT32       PrintSize,
  IN  UINT32       OffsetFromBase
  )
{
  UINT32  Offset;
  DEBUG ((DEBUG_VERBOSE, "       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"));
  for (Offset = 0; Offset < PrintSize; Offset++) {
    if ((Offset % 16) == 0) {
      DEBUG ((DEBUG_VERBOSE, "\n %04X: ", (Offset + OffsetFromBase) & 0xFFF0));
    }
    DEBUG ((DEBUG_VERBOSE, "%02X ", MmioRead8 (PrintMmioBase + Offset)));
  }
  DEBUG ((DEBUG_VERBOSE, "\n"));
}

VOID
PrintPchPciConfigSpace (
  VOID
  )
{
  if (!PEI_IN_EMULATION) {
    DEBUG((DEBUG_VERBOSE, "[ 0 - 00 - 0 ] Host Bridge\n")); 
    PrintRegisters(MmPciBase( 0,0,0 ), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 04(0x04) - 0 ] RAS\n"));
    PrintRegisters(MmPciBase(00,  4, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 05(0x05) - 0 ] RCEC\n"));
    PrintRegisters(MmPciBase(00,  5, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 06(0x06) - 0 ] VRP2\n"));
    PrintRegisters(MmPciBase(00,  6, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 09(0x09) - 0 ] PCI0\n"));
    PrintRegisters(MmPciBase(00,  9, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 10(0x0A) - 0 ] PCI1\n"));
    PrintRegisters(MmPciBase(00, 10, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 11(0x0B) - 0 ] PCI2\n"));
    PrintRegisters(MmPciBase(00, 11, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 12(0x0C) - 0 ] PCI3\n"));
    PrintRegisters(MmPciBase(00, 12, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 14(0x0E) - 0 ] PCI4\n"));
    PrintRegisters(MmPciBase(00, 14, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 15(0x0F) - 0 ] PCI5\n"));
    PrintRegisters(MmPciBase(00, 15, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 16(0x10) - 0 ] PCI6\n"));
    PrintRegisters(MmPciBase(00, 16, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 17(0x11) - 0 ] PCI7\n"));
    PrintRegisters(MmPciBase(00, 17, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 19(0x13) - 0 ] SATA0\n"));
    PrintRegisters(MmPciBase(00, 19, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 20(0x14) - 0 ] SATA1\n"));
    PrintRegisters(MmPciBase(00, 20, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 21(0x15) - 0 ] XHCI Controller\n"));
    PrintRegisters(MmPciBase(00, 21, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 22(0x16) - 0 ] VRP0\n"));
    PrintRegisters(MmPciBase(00, 22, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 23(0x17) - 0 ] VRP1\n"));
    PrintRegisters(MmPciBase(00, 23, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 24(0x18) - 0 ] ME HECI\n"));
    PrintRegisters(MmPciBase(00, 24, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 24(0x18) - 1 ] ME HECI2\n"));
    PrintRegisters(MmPciBase(00, 24, 01), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 24(0x18) - 3 ] MEKT\n"));
    PrintRegisters(MmPciBase(00, 24, 03), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 24(0x18) - 4 ] ME HECI3\n"));
    PrintRegisters(MmPciBase(00, 24, 04), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 18(0x12) - 0 ] SMBUS1\n"));
    PrintRegisters(MmPciBase(00, 18, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 26(0x1A) - 0 ] UART0\n"));
    PrintRegisters(MmPciBase(00, 26, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 26(0x1A) - 1 ] UART1\n"));
    PrintRegisters(MmPciBase(00, 26, 01), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 26(0x1A) - 2 ] UART2\n"));
    PrintRegisters(MmPciBase(00, 26, 02), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 30(0x1E) - 0 ] EMMC\n"));
    PrintRegisters(MmPciBase(00, 30, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 31(0x1F) - 0 ] ISA Bridge\n"));
    PrintRegisters(MmPciBase(00, 31, 00), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 31(0x1F) - 1 ] P2SB\n"));
    PrintRegisters(MmPciBase(00, 31, 01), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 31(0x1F) - 2 ] PMC\n"));
    PrintRegisters(MmPciBase(00, 31, 02), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 31(0x1F) - 4 ] SMBus Controller\n"));
    PrintRegisters(MmPciBase(00, 31, 04), 256, 0);
    DEBUG ((DEBUG_VERBOSE, "[ 0 - 31(0x1F) - 5 ] SPI Controller\n"));
    PrintRegisters(MmPciBase(00, 31, 05), 256, 0);
  }

  DEBUG ((DEBUG_VERBOSE, "\n\n"));
}
