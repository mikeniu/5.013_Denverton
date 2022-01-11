/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c)2009-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SouthFabricErrorHandler.c

Abstract:

  SMM Error Logging.

------------------------------------------------------------------*/
#include "CommonErrorHandlerIncludes.h"

PCH_SBI_PID PsfPortIdTable [] = {
  PID_PSF0,
  PID_PSF1,
  PID_PSF2,
  PID_PSF3,
  PID_PSFS0,
  PID_PSFS1,
  PID_MEPSF,
  PID_IEPSF
};

UINT16 PsfPortIdTableSize = sizeof(PsfPortIdTable)/sizeof(PsfPortIdTable[0]);

DNV_PCIE_RP  DnvPCIeRootPorts [DNV_PCIE_RP_COUNT] = {
  {RPX16_PCIE_BUS, RPX16_PCIE_DEVICE, RPX16_PCIE_FUNC, PCIE_RP},
  {RPX16_PCIE_BUS, RPX16_PCIE_DEVICE + 1, RPX16_PCIE_FUNC, PCIE_RP},
  {RPX16_PCIE_BUS, RPX16_PCIE_DEVICE + 2, RPX16_PCIE_FUNC, PCIE_RP},
  {RPX16_PCIE_BUS, RPX16_PCIE_DEVICE + 3, RPX16_PCIE_FUNC, PCIE_RP},
  {RPX4_PCIE_BUS, RPX4_PCIE_DEVICE, RPX4_PCIE_FUNC, PCIE_RP},
  {RPX4_PCIE_BUS, RPX4_PCIE_DEVICE + 1, RPX4_PCIE_FUNC, PCIE_RP},
  {RPX4_PCIE_BUS, RPX4_PCIE_DEVICE + 2, RPX4_PCIE_FUNC, PCIE_RP},
  {RPX4_PCIE_BUS, RPX4_PCIE_DEVICE + 3, RPX4_PCIE_FUNC, PCIE_RP},
  {vRP_PCIE_BUS, vRP0_PCIE_DEVICE, vRP_PCIE_FUNC, PCIE_vRP},
  {vRP_PCIE_BUS, vRP1_PCIE_DEVICE, vRP_PCIE_FUNC, PCIE_vRP},
  {vRP_PCIE_BUS, vRP2_PCIE_DEVICE, vRP_PCIE_FUNC, PCIE_vRP}
};

// Denverton AptioV Override Start- EIP#280221
VOID SwitchAlternateAccessMode (
    IN BOOLEAN      Switch
);
// Denverton AptioV Override Start- EIP#280221

VOID
ClearRCECErros(
  VOID
  )
/*++

Routine Description:

  Clear all RCEC and PCIe Root complex Integrated End points (IEH and SMBus) error status register .

Arguments:

  None

Returns:

  none

--*/
{
  UINT32    Data32;
  UINT16    Data16;

  DEBUG((EFI_D_ERROR, "Clearing RCEC Errors\n"));

  //
  //Clear IEH Error Status register
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_ERRUNCSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_ERRUNCSTS), Data32);

  //
  //Clear SMBus AER Uncorrectable error status register
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER, R_PCH_SMBUS_HOST_AER_ERRUNCSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER, R_PCH_SMBUS_HOST_AER_ERRUNCSTS), Data32);

  //
  //Clear SMBus AER correctable error status register
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER, R_PCH_SMBUS_HOST_AER_ERRCORSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER, R_PCH_SMBUS_HOST_AER_ERRCORSTS), Data32);

  //
  // Clear the SMBus PCIe device status register
  //
  Data16 = PciExpressRead16 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER, R_PCH_SMBUS_HOST_DEVSTS));
  PciExpressWrite16 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER, R_PCH_SMBUS_HOST_DEVSTS), Data16);

  //
  //Clear SMBus Error status register
  //
  Data16 = PciExpressRead16 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER, R_PCH_SMBUS_HOST_PCISTS));
  PciExpressWrite16 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER, R_PCH_SMBUS_HOST_PCISTS), Data16);

  //
  //Clear RCEC AER Uncorrectable error status register
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ERRUNCSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ERRUNCSTS), Data32);

  //
  //Clear RCEC AER correctable error status register
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ERRCORSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ERRCORSTS), Data32);

  //
  //Clear RCEC AER Root error status register
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ROOTERRSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ROOTERRSTS), Data32);

  //
  // Clear the RCEC PCIe device status register
  //
  Data16 = PciExpressRead16 (PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_DEVSTS));
  PciExpressWrite16 (PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_DEVSTS), Data16);

  //
  //Clear RCEC Error status register
  //
  Data16 = PciExpressRead16 (PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_PCISTS));
  PciExpressWrite16 (PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_PCISTS), Data16);
}

VOID
ClearGlobalErrors(
  VOID
  )
/*++

Routine Description:

  Clear all Global error status register.

Arguments:

  None

Returns:

  none

--*/
{
  UINT32    Data32;

  //
  //Clear Global corrected errors status
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GCOERRSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GCOERRSTS), Data32);

  //
  //Clear Global Non Fatal errors status
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GNFERRSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GNFERRSTS), Data32);

  //
  //Clear Global Fatal errors status
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GFAERRSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GFAERRSTS), Data32);

  //
  //Clear Global System event status (RO)
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GSYSEVTSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GSYSEVTSTS), Data32);

  //
  //Clear errors pin status
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_ERRPINSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_ERRPINSTS), Data32);

}

VOID
ClearSouthComplexErrors(
  VOID
  )
/*++

Routine Description:

  Clear all South complex errors before enabling them.

Arguments:

  None

Returns:

  none

--*/
{

  //Clear RCEC Error
  ClearRCECErros ();

  //PcieErrors will be cleared during enabling PCIe error reporting capability routine "EnableElogRPPCIe"

  //Clear Global errors status
  ClearGlobalErrors ();

}

VOID
PSFEnableCmdParity (
  PCH_SBI_PID    PortId
  )
/*++

Routine Description:

  This function Enables Command parity in PSF Global Configuration register.

Arguments:

  PortId  -  Sideband access port number.

Returns:

  none

--*/
{

  PchPcrAndThenOr32 (PortId, R_PSF_GLOBAL_CONFIG, (UINT32)~(B_ERR_MSI_EN | B_EN_CMD_PARITY_DETECTION), (UINT32)(B_ERR_MSI_EN | B_EN_CMD_PARITY_DETECTION));

}


VOID
EnableIEHError(
  VOID
  )
/*++

Routine Description:

  This function Enables or disables the IEH and PSF IPs internal errors .

Arguments:

  none

Returns:

  none

--*/
{

  UINT8     Index;

  //
  //Enable command parity detection in all PSF IPs.
  //
  if (!DXE_IN_SIMICS) {
    for (Index = 0; Index < PsfPortIdTableSize; Index++) {
      PSFEnableCmdParity (PsfPortIdTable[Index]);
    }
  }
}

VOID
EnableElogP2sbRCEC(
  VOID
  )
/*++

Routine Description:

  This function Enables P2SB RCEC Error reporting to IEH, but before doing so
  clears any existing RCEC Error which may have been present during
  PCIe Enum.

Arguments:

  none

Returns:

  none

--*/
{
  UINT8      Data8;
  UINT16     Data16;
  UINT32     Data32;
  UINTN      Address;

  DEBUG ((DEBUG_INFO, "Enabling RCEC error reporting after PCIe Enum...\n" ));

  //
  // Enable IEH Error Forwarding
  //
  Address = PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ERRCORMSK);
  Data16  = (PciExpressRead16 (Address) & ~B_RCEC_ERRCORMSK_ANFEM);
  PciExpressWrite16 (Address, Data16);

  Address = PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ERRUNCMSK);
  Data32  = (PciExpressRead32 (Address) & ~(B_RCEC_ERRUNCMSK_UCEM   |
                                            B_RCEC_ERRUNCMSK_MTLPEM |
//                                            B_RCEC_ERRUNCMSK_UREM   | //Denverton AptioV override - EIP#348617 To avoid logging of unsupported error by IEH after PCI Enumeration   
                                            B_RCEC_ERRUNCMSK_UIEM));
  PciExpressWrite32 (Address, Data32);

  Address = PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ROOTCTL);
  Data8   = (PciExpressRead8 (Address) | B_RCEC_ROOTCTL_SECEE  |
                                         B_RCEC_ROOTCTL_SENFEE |
                                         B_RCEC_ROOTCTL_SEFEE);
  PciExpressWrite8 (Address, Data8);

  Address = PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_DEVCTL);
  Data16  = (PciExpressRead16 (Address) | B_RCEC_DEVCTL_CERE  |
                                          B_RCEC_DEVCTL_NFERE |
                                          B_RCEC_DEVCTL_FERE  |
                                          B_RCEC_DEVCTL_UREE);
  PciExpressWrite16 (Address, Data16);

  Address = PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ERRUNCSEV);
  Data32  = (PciExpressRead32 (Address) | B_RCEC_ERRUNCSEV_UIES);
  PciExpressWrite32 (Address, Data32);
  //Denverton AptioV override Start - EIP#348617 
  //To avoid logging of unsupported error by IEH after PCI Enumeration   
/*
  Address = PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ERRUNCMSK);
  Data32  = (PciExpressRead32 (Address) & ~B_RCEC_ERRUNCMSK_UREM);
  PciExpressWrite32 (Address, Data32);
*/
  //Denverton AptioV override End - EIP#348617 
}

VOID
EnableElogRCEC(
  VOID
  )
/*++

Routine Description:

  This function Enables or disables the root complex ingrated device errors (IEH and SMBus) .

Arguments:

  none

Returns:

  none

--*/
{

  UINT32     Data32;

  if ((mErrLogSetupData.SystemErrorEn != ENABLED) || (!mErrLogSetupData.RCECErrorLog))
    return;


  EnableIEHError();

  //
  // Enable P2SB RCEC error reporting
  //
  EnableElogP2sbRCEC();

  //
  //Enable SMBUS HOST Data Parity and Command parity Error
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER, R_PCH_SMBUS_HOST_AER_DEFEATURE));

  Data32 &= ~(B_PCH_SMBUS_HOST_AER_CPERR | B_PCH_SMBUS_HOST_AER_DPERR);

  PciExpressWrite32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER, R_PCH_SMBUS_HOST_AER_DEFEATURE), Data32);



  //
  // Enable Error logging in SMBus HOST Device Control
  //
  PciErrLibEnableElogDevice (DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER);

  //
  // Enable Error logging in RCEC Root Control and Device Control
  //
  PciErrLibEnableElogDevice (RCEC_BUS, RCEC_DEVICE, RCEC_FUNC);

}

VOID
EnableElogRPPCIe(
  VOID
  )
/*++

Routine Description:

  This function Enables or disables the PCIe RP and virtual RP errors.

Arguments:

  none

Returns:

  none

--*/
{
  UINT8   Index;
  UINT32  Data32;

  if ((mErrLogSetupData.SystemErrorEn != ENABLED) || (!mErrLogSetupData.PCIeErrorLog))
    return;

  //
  //Enable Error reporting in All DNV root ports
  //
  for(Index = 0; Index < DNV_PCIE_RP_COUNT; Index++) {

    if (!PciErrLibIsDevicePresent (DnvPCIeRootPorts[Index].Bus, DnvPCIeRootPorts[Index].Dev, DnvPCIeRootPorts[Index].Func))
      continue;

    //
    //Unmask Uncorrectable Internal Error in AER mask register
    //
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(DnvPCIeRootPorts[Index].Bus, DnvPCIeRootPorts[Index].Dev, DnvPCIeRootPorts[Index].Func, R_PCIE_AER_ERRUNCMSK));
    Data32 &= ~(B_PCIE_ERRUNCMSK_UIEM |  B_PCIE_ERRUNCMSK_PTLPEBM);
    PciExpressWrite32 (PCI_PCIE_ADDR(DnvPCIeRootPorts[Index].Bus,DnvPCIeRootPorts[Index].Dev, DnvPCIeRootPorts[Index].Func, R_PCIE_AER_ERRUNCMSK), Data32);

    //
    //Unmask Uncorrectable Internal Error in AER mask register
    //
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(DnvPCIeRootPorts[Index].Bus, DnvPCIeRootPorts[Index].Dev, DnvPCIeRootPorts[Index].Func, R_PCIE_AER_ERRCORMSK));
    Data32 &= ~(B_PCIE_AER_ERRCORMSK_CIEM |  B_PCIE_AER_ERRCORMSK_HLOEM);
    PciExpressWrite32 (PCI_PCIE_ADDR(DnvPCIeRootPorts[Index].Bus,DnvPCIeRootPorts[Index].Dev, DnvPCIeRootPorts[Index].Func, R_PCIE_AER_ERRCORMSK), Data32);
    //
    //Enable IOSF parity checking in all Root ports
    //
    PciErrLibEnableIOSFParity (DnvPCIeRootPorts[Index].Bus, DnvPCIeRootPorts[Index].Dev, DnvPCIeRootPorts[Index].Func, DnvPCIeRootPorts[Index].PortType);
    //
    // Enable error reporting in the root port and all subordinate devices.
    //
    PciErrLibEnableElogDeviceAndSubDevices (DnvPCIeRootPorts[Index].Bus, DnvPCIeRootPorts[Index].Dev, DnvPCIeRootPorts[Index].Func);

  }
}

VOID
EnableElogLegacyPci(
  VOID
	)
/*++

Routine Description:

  This function Enables PCI device (SATA, USB, LPC and UART) errors.

Arguments:

  None

Returns:

  None

--*/
{
  UINT32    Data32;
  UINT8     SecBus;

  //
  //Enable SATA0 Data Phase Parity and Command parity error
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SATA0_DEVICE_NUMBER, PCH_SATA_FUNCTION_NUMBER, R_PCH_SATAGC));

  Data32 |= (B_PCH_SATAGC_CPEE | B_PCH_SATAGC_DPPEE);

  PciExpressWrite32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SATA0_DEVICE_NUMBER, PCH_SATA_FUNCTION_NUMBER, R_PCH_SATAGC), Data32);

  //
  //Enable SATA PERR and SERR
  //
  PciErrLibEnableElogDevice (DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SATA0_DEVICE_NUMBER, PCH_SATA_FUNCTION_NUMBER);

  //
  //Enable SATA1 Data Phase Parity and Command parity error
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SATA1_DEVICE_NUMBER, PCH_SATA_FUNCTION_NUMBER, R_PCH_SATAGC));

  Data32 |= (B_PCH_SATAGC_CPEE | B_PCH_SATAGC_DPPEE);

  PciExpressWrite32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SATA1_DEVICE_NUMBER, PCH_SATA_FUNCTION_NUMBER, R_PCH_SATAGC), Data32);

  //
  //Enable SATA PERR and SERR
  //
  PciErrLibEnableElogDevice (DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SATA1_DEVICE_NUMBER, PCH_SATA_FUNCTION_NUMBER);

  //
  //Enable USB Data Parity and Command parity Check
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_USB_DEVICE_NUMBER, PCH_USB_FUNCTION_NUMBER, R_PCH_USB_XHCC3));

  Data32 &= ~(B_PCH_USB_XHCC3_DDPC | B_PCH_USB_XHCC3_DCPC);

  PciExpressWrite32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_USB_DEVICE_NUMBER, PCH_USB_FUNCTION_NUMBER, R_PCH_USB_XHCC3), Data32);

  //
  //Enable USB PERR and SERR
  //
  PciErrLibEnableElogDevice (DEFAULT_PCI_BUS_NUMBER_PCH, PCH_USB_DEVICE_NUMBER, PCH_USB_FUNCTION_NUMBER);

  //
  //Enable HSUART Data Parity and Command parity Error
  //
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_HSUART_DEVICE_NUMBER, PCH_HSUART_FUNCTION_NUMBER, R_PCH_HSUART_DEFEATURE));

  Data32 &= ~(B_PCH_HSUART_DEFEATURE_CPERR_MSK | B_PCH_HSUART_DEFEATURE_DPERR_MSK);

  PciExpressWrite32 (PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH, PCH_HSUART_DEVICE_NUMBER, PCH_HSUART_FUNCTION_NUMBER, R_PCH_HSUART_DEFEATURE), Data32);

  //
  //Enable HSUART PERR and SERR
  //
  PciErrLibEnableElogDevice (DEFAULT_PCI_BUS_NUMBER_PCH, PCH_HSUART_DEVICE_NUMBER, PCH_HSUART_FUNCTION_NUMBER);

  //
  //Enable nCPM Parity Check. nCPM is behind the vRP0, get secondary  bus number and enable parity check.
  //
  SecBus = PciExpressRead8 (PCI_PCIE_ADDR(vRP_PCIE_BUS, vRP0_PCIE_DEVICE, vRP_PCIE_FUNC, R_PCI_BRIDGE_SECONDARY_BUS_OFFSET));

  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(SecBus, PCH_NCPM_DEVICE_NUMBER, PCH_NCPM_FUNCTION_NUMBER, R_PCH_NCPM_PFCGCIOSFPRIR));
  Data32 |= B_PCH_NCPM_PFCGCIOSFPRIR_PARCHK;
  PciExpressWrite32 (PCI_PCIE_ADDR(SecBus, PCH_NCPM_DEVICE_NUMBER, PCH_NCPM_FUNCTION_NUMBER, R_PCH_NCPM_PFCGCIOSFPRIR), Data32);

}


VOID
EnableElogGlobalError(
  VOID
  )
/*++

Routine Description:

  This function Enables or disables the Global errors.

Arguments:

  None

Returns:

  None

--*/
{
  UINT32                          Data32;

  if (mErrLogSetupData.SystemErrorEn != ENABLED) {

    //
    //Mask Global error reporting of error detected by the DNV local devices
    //
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GERRMSK));
    Data32 |= B_MSK_ALL;
    PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GERRMSK), Data32);

    //
    //Mask Global System Event
    //
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GSYSEVTMSK));
    Data32 |= (B_CO_SYSEV_M | B_NF_SYSEV_M | B_FA_SYSEV_M);
    PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GSYSEVTMSK), Data32);

    //
    // Clear System Event Map for no system event generation
    //
    Data32 = (B_CO_SYSEV_MAP_NE | B_NF_SYSEV_MAP_NE | B_FA_SYSEV_MAP_NE);
    PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GSYSEVTMAP), Data32);

  } else {

    //
    //Mask reporting of error detected by the DNV local devices
    //
    Data32 = B_MSK_ALL;

    //
    //Un Mask Memory error from DUNIT
    //
    if (mErrLogSetupData.MemoryErrorLog)
      Data32 &= ~(B_DUNIT0_MSK | B_DUNIT1_MSK);

    //
    //Un Mask RCEC error from PCIe Integrated end point devices
    //
    if (mErrLogSetupData.RCECErrorLog)
      Data32 &= ~B_RCEC_MSK;
    //
    //Un Mask PCIe error from PCIe Root port and Virtual Root port.
    //
    if (mErrLogSetupData.PCIeErrorLog)
      Data32 &= ~(B_PCIERP0_MSK | B_PCIERP1_MSK | B_PCIERP2_MSK | B_PCIERP3_MSK | B_PCIERP4_MSK | B_PCIERP5_MSK | B_PCIERP6_MSK | B_PCIERP7_MSK |\
                  B_VRP0_MSK | B_VRP1_MSK | B_VRP2_MSK);

    //
    //Un Mask AUNIT, BUNIT, SMBus, LPC, USB, SATA, SPI and HSUART errors
    //
    Data32 &= ~(B_AUNIT_MSK | B_BUNIT_MSK | B_SMBUS_MSK | B_LPC_MSK | B_USB_MSK | B_SATA0_MSK | B_SATA1_MSK | B_HSUART_MSK | B_SPIESPI_MSK);

    PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GERRMSK), Data32);

    //
    //UnMask System Event
    //
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GSYSEVTMSK));
    Data32 &= ~(B_CO_SYSEV_M | B_NF_SYSEV_M | B_FA_SYSEV_M);
    PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GSYSEVTMSK), Data32);

    //
    // System Event Map for the errors detected by DNV CORR_ERR->SMI / NON-FATAL_ERR->SMI / FATAL_ERR->SMI
    //
    Data32 = (B_CO_SYSEV_MAP_SMI | B_NF_SYSEV_MAP_SMI | B_FA_SYSEV_MAP_SMI);
    PciExpressWrite32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GSYSEVTMAP), Data32);

  }
}


VOID
ProgramSouthComplexErrorSignals(
  VOID
	)
/*++

Routine Description:

  This function Enables or disables the Root Fabric, PCIe RP and PCIe end node (GBe Sata and SMBus etc.,) errors.

Arguments:

  None

Returns:

  None

--*/
{

  EnableElogRCEC();

  EnableElogRPPCIe();

  EnableElogLegacyPci();

  EnableElogGlobalError();
}

BOOLEAN
CheckForSystemError(
  VOID
  )
/*++

Routine Description:

  This function checks for system errors from global erros status register.

Arguments:

  None

Returns:

  None

--*/
{
  UINT32  Global_Evt_Sts;
  UINT32  Global_Evt_Mask;

  //
  //Global System event status (RO)
  //
  Global_Evt_Sts = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GSYSEVTSTS));

  if((Global_Evt_Sts & (B_CO_SYSEV_STS | B_NF_SYSEV_STS | B_FA_SYSEV_STS)) != 0) {

    //
    //check Global System event Mask register
    //
    Global_Evt_Mask = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GSYSEVTMSK));

    if ( !(Global_Evt_Sts & (Global_Evt_Mask & B_CO_SYSEV_M)) ||
         !(Global_Evt_Sts & (Global_Evt_Mask & B_NF_SYSEV_M)) ||
         !(Global_Evt_Sts & (Global_Evt_Mask & B_FA_SYSEV_M))
    ) {
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN
ElogRPPCIeErrorHandler
(
  VOID
  )
/*++

Routine Description:
    PCIe Root port SMI handler

Arguments:
    None

Returns:
    None

--*/
{

  UINT8    Device;
  BOOLEAN  ErrorDetected = FALSE;
  BOOLEAN  RootPortErrReportingEnabled = FALSE;

  //check 4pX16 root ports
  for (Device = RPX16_PCIE_DEVICE; Device < (RPX16_PCIE_DEVICE + RPX16_PCIE_MAX_DEVICE); Device++) {

    RootPortErrReportingEnabled = PciErrLibIsRootPortErrReportingEnabled (RPX16_PCIE_BUS, Device, RPX16_PCIE_FUNC);

    if(RootPortErrReportingEnabled){
      if (PciErrLibIsErrOnPcieRootPort (RPX16_PCIE_BUS, Device, RPX16_PCIE_FUNC)) {
      //
      // Error flagged on this root port, process and log the error.
      //
      ErrorDetected |= PciErrLibPcieRootPortErrorHandler (RPX16_PCIE_BUS, Device, RPX16_PCIE_FUNC);
      }
    }
  }
  RootPortErrReportingEnabled = FALSE;
  //check 4pX4 root ports
  for (Device = RPX4_PCIE_DEVICE; Device < (RPX4_PCIE_DEVICE + RPX4_PCIE_MAX_DEVICE); Device++) {

    RootPortErrReportingEnabled = PciErrLibIsRootPortErrReportingEnabled (RPX4_PCIE_BUS, Device, RPX4_PCIE_FUNC);

    if(RootPortErrReportingEnabled){
      if (PciErrLibIsErrOnPcieRootPort (RPX4_PCIE_BUS, Device, RPX4_PCIE_FUNC)) {
        //
        // Error flagged on this root port, process and log the error.
        //
        ErrorDetected |= PciErrLibPcieRootPortErrorHandler (RPX4_PCIE_BUS, Device, RPX4_PCIE_FUNC);
      }
    }
  }

  //check virtual root port 0
  if (PciErrLibIsRootPortErrReportingEnabled (vRP_PCIE_BUS, vRP0_PCIE_DEVICE, vRP_PCIE_FUNC)) {
    if (PciErrLibIsErrOnPcieRootPort (vRP_PCIE_BUS, vRP0_PCIE_DEVICE, vRP_PCIE_FUNC)) {
      //
      // Error flagged on this root port, process and log the error.
      //
      ErrorDetected |= PciErrLibPcieRootPortErrorHandler (vRP_PCIE_BUS, vRP0_PCIE_DEVICE, vRP_PCIE_FUNC);
    }
  }

  //check virtual root port 1
  if (PciErrLibIsRootPortErrReportingEnabled (vRP_PCIE_BUS, vRP1_PCIE_DEVICE, vRP_PCIE_FUNC)) {
    if (PciErrLibIsErrOnPcieRootPort (vRP_PCIE_BUS, vRP1_PCIE_DEVICE, vRP_PCIE_FUNC)) {
      //
      // Error flagged on this root port, process and log the error.
      //
      ErrorDetected |= PciErrLibPcieRootPortErrorHandler (vRP_PCIE_BUS, vRP1_PCIE_DEVICE, vRP_PCIE_FUNC);
    }
  }

  //check virtual root port 2
  if (PciErrLibIsRootPortErrReportingEnabled (vRP_PCIE_BUS, vRP2_PCIE_DEVICE, vRP_PCIE_FUNC)) {
    if (PciErrLibIsErrOnPcieRootPort (vRP_PCIE_BUS, vRP2_PCIE_DEVICE, vRP_PCIE_FUNC)) {
      //
      // Error flagged on this root port, process and log the error.
      //
      ErrorDetected |= PciErrLibPcieRootPortErrorHandler (vRP_PCIE_BUS, vRP2_PCIE_DEVICE, vRP_PCIE_FUNC);
    }
  }
  return ErrorDetected;
}



UINTN mNonStdDevErrLookupEntries = 0;
NON_STANDARD_DEV_ERROR_INFO mNonStdDevErrLookup[] = {
//ErrorType, PortType
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_CFG_PARITY, IEH },
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_CMD_PARITY, IEH },
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_DATA_PARITY, IEH },
  { PLATFORM_NON_STANDARD_DEV_ERROR_P2SB_NDT, IEH },
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_CMD_PARITY, PSF0},
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_UC, PSF0},
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_UR, PSF0},
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_CMD_PARITY, PSF1},
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_UC, PSF1},
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_UR, PSF1},
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_CMD_PARITY, PSF2},
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_UC, PSF2},
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_UR, PSF2},
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_CMD_PARITY, PSF3},
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_UC, PSF3},
  { PLATFORM_NON_STANDARD_DEV_ERROR_PSF_UR, PSF3},
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_CMD_PARITY, IEH}, //PSFS0
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_UC, IEH},
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_UR, IEH},
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_CMD_PARITY, IEH}, //PSFS1
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_UC, IEH},
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_UR, IEH},
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_CMD_PARITY, IEH}, //PSFCSME
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_UC, IEH},
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_UR, IEH},
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_CMD_PARITY, IEH}, //PSFIE
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_UC, IEH},
  { PLATFORM_NON_STANDARD_DEV_ERROR_IEH_UR, IEH},
};



BOOLEAN
ElogIEHErrorHandler
(
  VOID
  )
/*++

Routine Description:
    IEH SMI handler

Arguments:
    None

Returns:
    None

--*/
{

  BOOLEAN                        ErrorDetected = FALSE;
  UINT32                         Data32;
  NON_STANDARD_DEV_ERROR_INFO    NonStdDevErrInfo;
  UINT8                          i;
  i = 0;
  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_ERRUNCSTS));
  mNonStdDevErrLookupEntries = sizeof(mNonStdDevErrLookup) / sizeof(NON_STANDARD_DEV_ERROR_INFO);
  while(Data32 != 0) {
    ErrorDetected = TRUE;
    if((Data32 & 0x1) == 1) {
      if(i < mNonStdDevErrLookupEntries) {
        NonStdDevErrInfo.ErrorType = mNonStdDevErrLookup[i].ErrorType;
        NonStdDevErrInfo.PortType = mNonStdDevErrLookup[i].PortType;
        mWheaPlatformSupport->WheaPlatformElogNonStandardDev (
                                        mWheaPlatformSupport,
                                        GEN_ERR_SEV_NON_STANDARD_DEV_FATAL,
                                        &NonStdDevErrInfo
                                        );
      }
    }
    Data32 >>= 1;
    i++;
  }
  return ErrorDetected;
}

BOOLEAN
ElogSMBusErrorHandler
(
  VOID
  )
/*++

Routine Description:
    SMBus SMI handler

Arguments:
    None

Returns:
    None

--*/
{

  BOOLEAN  ErrorDetected = FALSE;

  //
  //check for a SMBus Uncorrectable FATAL/NON FATAL error
  //
  if ( PciErrLibIsFatalPcieDeviceError (DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER) ||
	   PciErrLibIsNonfatalPcieDeviceError (DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER))
  {
	  PciErrLibHandleUncorrectablePcieDeviceError (DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER);
	  ErrorDetected = TRUE;
  }

  //
  //check for a SMBus Correctable error
  //
  if (PciErrLibIsCorrectablePcieDeviceError (DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER))
  {
    PciErrLibHandleCorrectablePcieDeviceError (DEFAULT_PCI_BUS_NUMBER_PCH, PCH_SMBUS_HOST_DEVICE_NUMBER, PCH_SMBUS_HOST_FUNCTION_NUMBER);
	ErrorDetected = TRUE;
  }

  return ErrorDetected;
}


BOOLEAN
ElogRCECErrorHandler
(
  VOID
  )
/*++

Routine Description:
    RCEC SMI handler

Arguments:
    None

Returns:
    None

--*/
{

  UINT32   Data32;
  BOOLEAN  ErrorDetected = FALSE;

  //
  //check for a RCEC Uncorrectable FATAL/NON FATAL error
  //
  if ( PciErrLibIsFatalPcieDeviceError (RCEC_BUS, RCEC_DEVICE, RCEC_FUNC) ||
     PciErrLibIsNonfatalPcieDeviceError (RCEC_BUS, RCEC_DEVICE, RCEC_FUNC))
  {
    //Check for IEH internal Error
    Data32 = PciExpressRead32 (PCI_PCIE_ADDR(RCEC_BUS, RCEC_DEVICE, RCEC_FUNC, R_RCEC_ERRUNCSTS));
  if (Data32 & B_RCEC_ERRUNCSTS_UIE) {  //Uncorrectable Internal Error (IEH)
    ErrorDetected |= ElogIEHErrorHandler ();
  }

  ErrorDetected |= ElogSMBusErrorHandler ();

  if (!ErrorDetected)
    PciErrLibHandleUncorrectablePcieDeviceError (RCEC_BUS, RCEC_DEVICE, RCEC_FUNC);
    ErrorDetected = TRUE;
  }

  //
  //check for a RCEC Correctable error
  //
  if (PciErrLibIsCorrectablePcieDeviceError (RCEC_BUS, RCEC_DEVICE, RCEC_FUNC))
  {

  ErrorDetected |= ElogSMBusErrorHandler ();

  if (!ErrorDetected)
    PciErrLibHandleCorrectablePcieDeviceError (RCEC_BUS, RCEC_DEVICE, RCEC_FUNC);
    ErrorDetected = TRUE;
  }

  return ErrorDetected;
}


BOOLEAN
ElogSouthComplexErrorHandler
(
  VOID
  )
/*++

Routine Description:
    South Complex Elog SMI handler to handle PCIe Root port, virtual RP and PCIe end device (SATA, GBe) errors

Arguments:
    None

Returns:
    None

--*/
{
  UINT32   Global_CO_Status;
  UINT32   Global_NF_Status;
  UINT32   Global_FA_Status;
  BOOLEAN  ErrorDetected = FALSE;


  //
  //Global corrected errors status
  //
  Global_CO_Status = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GCOERRSTS));

  //
  //Global Non Fatal errors status
  //
  Global_NF_Status = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GNFERRSTS));

  //
  //Global Fatal errors status
  //
  Global_FA_Status = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GFAERRSTS));

  if ((Global_CO_Status & (B_PCIERP0_CO_STS | B_PCIERP1_CO_STS | B_PCIERP2_CO_STS | B_PCIERP3_CO_STS | B_PCIERP4_CO_STS | B_PCIERP5_CO_STS |
                           B_PCIERP6_CO_STS | B_PCIERP7_CO_STS | B_VRP0_CO_STS | B_VRP1_CO_STS | B_VRP2_CO_STS)) ||
      (Global_NF_Status & (B_PCIERP0_NF_STS | B_PCIERP1_NF_STS | B_PCIERP2_NF_STS | B_PCIERP3_NF_STS | B_PCIERP4_NF_STS | B_PCIERP5_NF_STS | \
                           B_PCIERP6_NF_STS | B_PCIERP7_NF_STS | B_VRP0_NF_STS | B_VRP1_NF_STS | B_VRP2_NF_STS)) ||
      (Global_FA_Status & (B_PCIERP0_FA_STS | B_PCIERP1_FA_STS | B_PCIERP2_FA_STS | B_PCIERP3_FA_STS | B_PCIERP4_FA_STS | B_PCIERP5_FA_STS | \
                           B_PCIERP6_FA_STS | B_PCIERP7_FA_STS | B_VRP0_FA_STS | B_VRP1_FA_STS | B_VRP2_FA_STS))) {

    ErrorDetected |= ElogRPPCIeErrorHandler ();
  }

  if ((Global_CO_Status & B_RCEC_CO_STS) || (Global_NF_Status & B_RCEC_NF_STS) || (Global_FA_Status & B_RCEC_FA_STS))
  {
    ErrorDetected |= ElogRCECErrorHandler ();
  }

  return ErrorDetected;

}

VOID
ElogGenerateNmiNow (
  VOID
  )
/*++

Routine Description:
    This function generates an NMI  to force an entry to the NMI handler..

Arguments:
    N/A

Returns:
    N/A

--*/
{

  UINT8             Data;
  UINT16           TcoBase;
  UINT8             Save_Nmi2Smi_En;
  UINT8             Save_Port70;
  UINT16            Data16;

  //
  // Get the TCO Base Address
  //
  TcoBase = PciExpressRead16(PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH,
                                           PCI_DEVICE_NUMBER_PCH_SMBUS,
                                           PCI_FUNCTION_NUMBER_PCH_SMBUS,
                                           R_PCH_SMBUS_TCOBASE
                                           ));

  TcoBase = (UINT16) (TcoBase & B_PCH_SMBUS_TCOBASE_BAR);

  //
  // Read the NMI2SMI_EN bit, save it for future restore
  //
  Save_Nmi2Smi_En = IoRead8 (TcoBase + R_PCH_TCO1_CNT + 1);

  //
  // Set the NMI2SMI_EN bit to 0
  //
  Data = (UINT8) (Save_Nmi2Smi_En & (~BIT1));
  IoWrite8 ((TcoBase + R_PCH_TCO1_CNT + 1), Data);

  //
  // Enable NMI_EN
  //
  Save_Port70 = IoRead8 (R_PCH_NMI_EN_ALIAS);
  Data = (UINT8) (Save_Port70 & (~BIT7));
  IoWrite8 (R_PCH_NMI_EN, Data);

  //
  // Set NMI_NOW = 1
  //
  Data = IoRead8 (TcoBase + R_PCH_TCO1_CNT + 1);
  Data = (UINT8) (Data | BIT0);
  IoWrite8 ((TcoBase + R_PCH_TCO1_CNT + 1), Data);

  //
  // Clear NMI_NOW = 0 by writing 1 to NMI_NOW bit
  //
  Data = IoRead8 (TcoBase + R_PCH_TCO1_CNT + 1);
  Data = (UINT8) (Data | BIT0);
  IoWrite8 ((TcoBase + R_PCH_TCO1_CNT + 1), Data);

  //
  // Restore NMI2SMI_EN
  //
  IoWrite8 ((TcoBase + R_PCH_TCO1_CNT + 1), Save_Nmi2Smi_En);

  //
  // Clear the NMI2SMI_STS bit if set
  //
  Data16 = IoRead16 (TcoBase + R_PCH_TCO1_STS);
  if (Data16 & BIT0) {
    //
    // check port 0x61
    //
    Data = IoRead8 (R_PCH_NMI_SC);
    if (Data & B_PCH_NMI_SC_SERR_NMI_STS) {
      Data = IoRead8 (R_PCH_NMI_SC);
      Data  = (UINT8) (Data | B_PCH_NMI_SC_PCI_SERR_EN);
      Data  = (UINT8) (Data & 0x0F);
      IoWrite8 (R_PCH_NMI_SC, Data);
      Data = (UINT8) (Data & 0x0B);
      IoWrite8 (R_PCH_NMI_SC, Data);
    }
  }
  //
  // Restore NMI_EN
  //
  IoWrite8 (R_PCH_NMI_EN, Save_Port70);
  return ;

}

EFI_STATUS
EnableAsyncSmiLogic (
  VOID
  )
{
  UINT8                       Data;
  UINT16                      Data16;
  UINT32                      Data32;
  UINT16                      PmBase;

  //
  // Get the PM Base Address
  //
  PmBase = PciExpressRead16(PCI_PCIE_ADDR(DEFAULT_PCI_BUS_NUMBER_PCH,
                                                                          PCI_DEVICE_NUMBER_PCH_PMC,
                                                                          PCI_FUNCTION_NUMBER_PCH_PMC,
                                                                          R_PCH_PMC_ACPI_BASE
                                                                          ));

  PmBase = (UINT16) (PmBase & B_PCH_PMC_ACPI_BASE_BAR);

  //
  // Clear all Spurious Sources of the SMI at the SMI_STS register offset PMBASE + 34
  //
  Data32 = IoRead32 ((PmBase + R_PCH_SMI_STS));
  IoWrite32 ((PmBase + R_PCH_SMI_STS), Data32);

  //
  // enable PCH error generation
  // Setup the GBL_SMI_EN=1 to only cause SMI.
  // Set bit 13 TCO_EN and bit 0 GBL_SMI_EN
  //
  Data16 = IoRead16 ((PmBase + R_PCH_SMI_EN));
  Data16 = (UINT16) (Data16 | B_PCH_SMI_EN_TCO | B_PCH_SMI_EN_GBL_SMI );
  IoWrite16 ((PmBase + R_PCH_SMI_EN), Data16);

  //
  // Setup NMI_EN = 0;
  //
  // NMI is accessible thru the RTC/NMI register, mapped to I/O address 0x70 and also aliased to
  // I/O address 0x74.
  // Bit 7 controls NMI operation...
  //
  
  SwitchAlternateAccessMode(TRUE);     // Denverton AptioV Override - EIP#280221
  
  Data = IoRead8 (R_PCH_NMI_EN);
  Data = (UINT8) (Data & (~BIT7));
  IoWrite8 (R_PCH_NMI_EN, Data);
  
  SwitchAlternateAccessMode(FALSE);    // Denverton AptioV Override - EIP#280221

  //
  // Disable PCI_SERR_EN to generate NMIs
  //
  Data = IoRead8 (R_PCH_NMI_SC);
  Data &= (UINT8) (~B_PCH_NMI_SC_PCI_SERR_EN);
  Data |= B_PCH_NMI_SC_PCI_SERR_EN;
  IoWrite8 (R_PCH_NMI_SC, Data);

  return EFI_SUCCESS;
}
