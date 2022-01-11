/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2007 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PcieErrorHandler.c

Abstract:

  Whea generic implementation for error logging.

--*/
#include "CommonErrorHandlerIncludes.h"

BOOLEAN
PciErrLibIsDevicePresent (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function checks if a PCI or PCIe device exists at the specified PCI address.

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number
    
Returns: 
    TRUE     - Device present at the specified address
    FALSE    - Device not present at the specified address

--*/
{
  UINT16    Data16;

  //
  // Check if a device is present at this location...
  //
  Data16 = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_VENDOR_ID_OFFSET));
  if (Data16 == 0xFFFF) {
    return FALSE;
  }

  return TRUE;
}

BOOLEAN
PciErrLibDeviceOnBus (
  IN UINT8   Bus
  )
/*++

Routine Description:
    This function checks if a PCI device is present on the specified bus.

Arguments:
    Bus      - PCI bus number to check
    
Returns: 
    TRUE     - A PCI device was found on the bus
    FALSE    - A PCI device was not found on the bus

--*/
{
  UINT8   Device;
  UINT8   Function;

  for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
    for (Function = 0; Function <= PCI_MAX_FUNC; Function++) {

      if (PciErrLibIsDevicePresent (Bus, Device, Function)) {
        return TRUE;
      }
    }
  }

  return FALSE;
}

BOOLEAN
PciErrLibIsPciBridgeDevice (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function checks if a PCI-to-PCI bridge exists at the specified PCI address.

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number
    
Returns: 
    TRUE     - P2P present at the specified address
    FALSE    - P2P not present at the specified address

--*/
{
  UINT32  Data32;

  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_REVISION_ID_OFFSET));
  Data32 &= 0xFFFFFF00;
  if ( (Data32 == V_PCI_BRIDGE_DEV_CLASS_CODE) || 
       (Data32 == V_PCI_BRIDGE_SUBTRACTIVE_DEV_CLASS_CODE ) ) {
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
PciErrLibIsPcieDevice (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function checks if a specified device is a PCI Express device.

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number
    
Returns: 
    TRUE     - PCIe device present at the specified address
    FALSE    - PCIe device not present at the specified address

--*/
{
  UINT8   CapabilitiesOffset;

  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Bus, Device, Function);
  if (CapabilitiesOffset == 0) {
    return FALSE;
  }

  return TRUE;
}

VOID
PciErrLibEnableElogDeviceAndSubDevices (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function initializes and enables error reporting in the specified PCI or PCIe
    device and all subordinate PCI or PCIe devices.

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number
    
Returns: 
    Nothing

--*/
{
  UINT8 SecondaryBusNumber;
  UINT8 SubordinateBusNumber;

  //
  // Enable error logging in the current device...
  //
  PciErrLibEnableElogDevice (Bus, Device, Function);

  //
  // Enable error logging in the devices behind a bridge...
  //

  if (PciErrLibIsPciBridgeDevice (Bus, Device, Function)) {

    SecondaryBusNumber = PciExpressRead8 (
                          PCI_PCIE_ADDR(Bus,
                          Device,
                          Function,
                          R_PCI_BRIDGE_SECONDARY_BUS_OFFSET
                          ));

    SubordinateBusNumber = PciExpressRead8 (
                            PCI_PCIE_ADDR(Bus,
                            Device,
                            Function,
                            R_PCI_BRIDGE_SUBORDINATE_BUS_OFFSET
                            ));

    if((SecondaryBusNumber ==0) && (SubordinateBusNumber == 00)) return;

    for (Bus = SecondaryBusNumber; Bus <= SubordinateBusNumber; Bus++) {
      for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
        for (Function = 0; Function <= PCI_MAX_FUNC; Function++) {
          PciErrLibEnableElogDevice (Bus, Device, Function);
        }
      }
    }
  }


}

VOID
PciErrLibEnableElogDevice (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function initializes and enables error reporting in the specified PCI or PCIe device.

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number
    
Returns: 
    Nothing

--*/
{
  if (PciErrLibIsDevicePresent (Bus, Device, Function)) {

    PciErrLibClearLegacyPciDeviceStatus (Bus, Device, Function);

    PciErrLibEnablePciSerrPerrGeneration (Bus, Device, Function);

    //
    // Check if this is a PCIe device...
    //

    if (PciErrLibIsPcieDevice (Bus, Device, Function)) {
      PciErrLibEnablePcieELogCapabilities (Bus, Device, Function);
    }

    if (PciErrLibIsPciBridgeDevice (Bus, Device, Function)) {
      //
      // Enable SERR and PERR Error Logging in Bridge Control...
      // Serves both PCI and PCIe bridges...
      //
      PciErrLibEnableBridgeControl (Bus, Device, Function);
    }
  }
}

VOID
PciErrLibClearLegacyPciDeviceStatus (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function clears the specified device's primary and secondary (if appropriate) 
    legacy PCI status registers.

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number
    
Returns: 
    Nothing

--*/
{
  UINT16  StatusReg16;

  StatusReg16 = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_PRIMARY_STATUS_OFFSET));
  PciExpressWrite16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_PRIMARY_STATUS_OFFSET), StatusReg16);

  if (PciErrLibIsPciBridgeDevice (Bus, Device, Function)) {
    StatusReg16 = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_BRIDGE_STATUS_OFFSET));
    PciExpressWrite16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_BRIDGE_STATUS_OFFSET), StatusReg16);
  }
}

VOID
PciErrLibEnableBridgeControl (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function enables SERR and Parity error reporting in the P2P bridge's
    bridge control register.

Arguments:
    Bus      - P2P Bridge Device's bus number
    Device   - P2P Bridge Device's device number
    Function - P2P Bridge Device's function number
    
Returns: 
    Nothing

--*/
{
  UINT8 Data8;

  Data8 = PciExpressRead8 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_BRIDGE_CONTROL_OFFSET));

  Data8 &= ~(B_PCI_BRIDGE_CONTROL_SERR | B_PCI_BRIDGE_CONTROL_PARITY_ERROR_RESPONSE);

  //
  // Enable SERR if selected in Setup
  //
  if (mErrLogSetupData.PCIeSystemErrEnable) {
    Data8 |= B_PCI_BRIDGE_CONTROL_SERR;
  }

  //
  // enable PERR if selected in Setup
  //
  if (mErrLogSetupData.PCIeParityErrEnable) {
    Data8 |= B_PCI_BRIDGE_CONTROL_PARITY_ERROR_RESPONSE;
  }

  PciExpressWrite8 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_BRIDGE_CONTROL_OFFSET), Data8);
}

VOID
PciErrLibEnablePciSerrPerrGeneration (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function enables SERR and Parity error reporting in the specified device's 
    legacy PCI command register.

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number
    
Returns: 
    Nothing

--*/
{
  UINT16  Data16;

  //
  // Check if Assert on SERR is enabled in setup
  //
  Data16 = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_COMMAND_OFFSET));

  Data16 &= ~(B_PCI_COMMAND_SERR | B_PCI_COMMAND_PARITY_ERROR_RESPOND);

  if (mErrLogSetupData.PCIeSystemErrEnable) {
    Data16 |= B_PCI_COMMAND_SERR;
  }

  //
  // Check if Assert NMI on PERR is enabled in setup
  //
  if (mErrLogSetupData.PCIeParityErrEnable) {
    Data16 |= B_PCI_COMMAND_PARITY_ERROR_RESPOND;
  }

  PciExpressWrite16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_COMMAND_OFFSET), Data16);
}

UINT8 
PciErrLibGetCapabilitiesOffset (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function obtains the PCI configuration space register offset of the specified device's
    PCI Express Capabilities register set.  Note: this is not the offset to the Advanced
    Error Reporting register set.

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number
    
Returns: 
    0        - PCIe capability ID not found
    non-zero - Offset of the PCIe capability register set

--*/
{
  UINT8   PciPrimaryStatus;
  UINT8   CapabilityOffset;
  UINT8   CapId;

  PciPrimaryStatus = PciExpressRead8 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_PRIMARY_STATUS_OFFSET));

  if (PciPrimaryStatus & B_PCI_STATUS_CAPABILITY_LIST_ENABLE) {

    CapabilityOffset = PciExpressRead8 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_CAPABILITY_PTR));

    do {

      CapId = PciExpressRead8 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilityOffset));
      if (CapId == V_PCI_CAPABILITY_ID_PCIEXP) {
        break;
      }
      CapabilityOffset = PciExpressRead8 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilityOffset + 1));

    } while (CapabilityOffset != 0);

  } else {
    CapabilityOffset = 0;
  }

  return CapabilityOffset;
}


BOOLEAN
PCIeIsRootPort(
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function will verify whether a port is PCIe root port

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number

Returns:
    True     - if PCIe is root port
    False    - if PCIe is not root port
--*/
{
  BOOLEAN RootPort;
  UINT16  PcieCapabilities;
  UINT16  CapabilitiesOffset;

  RootPort = FALSE;

  if (!PciErrLibIsDevicePresent (Bus, Device, Function))
     return FALSE;
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Bus, Device, Function);
  if (CapabilitiesOffset == 0) {
    return FALSE;
  }

  PcieCapabilities = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, (CapabilitiesOffset + R_PCIE_CAPABILITY_OFFSET)));
  if ((PcieCapabilities & 0xF0) == V_PCIE_CAPABILITY_DPT_ROOT_PORT) {
    RootPort = TRUE;
  } else {
    RootPort = FALSE;
  }
  return RootPort;
}

VOID
PciErrLibEnablePcieELogCapabilities (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function performs the following operations on the specified PCIe device:
      1.  Programs the Uncorrectable Error Mask register if applicable
      2.  Programs the Uncorrectable Error Severity register if applicable
      3.  Programs the Correctable Error Mask register if applicable
      4.  Clears the standard PCIe status registers
      5.  Disables legacy SERR reporting if appropriate (no AER registers)
      6.  Enables error reporting for root ports and regular devices

Arguments:
    Bus      - PCIe Device's bus number
    Device   - PCIe Device's device number
    Function - PCIe Device's function number
    
Returns: 
    Nothing

--*/
{
  UINT8   CapabilitiesOffset;
  UINT16  PcieCapabilities;
  UINT16  DeviceControl;
  UINT16  RootControl;
  UINT16  StatusReg16;
  UINT32  StatusReg32;
  BOOLEAN RootPort;
  
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Bus, Device, Function);
  if (CapabilitiesOffset == 0) {
    return ;
  }

  PcieCapabilities = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_CAPABILITY_OFFSET));
  if ((PcieCapabilities & 0xF0) == V_PCIE_CAPABILITY_DPT_ROOT_PORT) {
    RootPort = TRUE;
  } else {
    RootPort = FALSE;
  }

  //
  // Clear the PCIe device status register
  //
  StatusReg16 = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_STATUS_OFFSET));
  PciExpressWrite16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_STATUS_OFFSET), StatusReg16);


  //
  //Clear AER Uncorrectable error status register
  //
  StatusReg32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_ERRUNCSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_ERRUNCSTS), StatusReg32);

  //
  //Clear AER correctable error status register
  //
  StatusReg32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_ERRCORSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_ERRCORSTS), StatusReg32);


  //
  // If this is a PCIe Root Port of the PCIe root complex, then configure its
  // Root Control Register also...
  //
  if (RootPort) {

    //
    //Clear AER Root error status register
    //
    StatusReg32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_RES));
    PciExpressWrite32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_RES), StatusReg32);

    RootControl = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_ROOT_CONTROL_OFFSET));
    //
    // Enable SERR on Fatal, Non-Fatal, and Correctable errors for error reporting.
    // An NMI will not be generated for correctable errors.
    //
    RootControl &= ~(B_PCIE_ROOT_CONTROL_FAT_ERR_EN | B_PCIE_ROOT_CONTROL_NF_ERR_EN |B_PCIE_ROOT_CONTROL_COR_ERR_EN);
    if (mErrLogSetupData.PCIeCorrErrorLog)
      RootControl |= B_PCIE_ROOT_CONTROL_COR_ERR_EN;
    if (mErrLogSetupData.PCIeNFErrorLog)
      RootControl |= B_PCIE_ROOT_CONTROL_NF_ERR_EN;
    if (mErrLogSetupData.PCIeFatalErrorLog)
      RootControl |= B_PCIE_ROOT_CONTROL_FAT_ERR_EN;

    PciExpressWrite16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_ROOT_CONTROL_OFFSET), RootControl);
  }

  //
  // Enable Error logging in Device Control...
  //
  DeviceControl = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_CONTROL_OFFSET));

  DeviceControl &= ~(B_PCIE_DEVICE_CONTROL_COR_ERR_EN | B_PCIE_DEVICE_CONTROL_NF_ERR_EN | B_PCIE_DEVICE_CONTROL_FAT_ERR_EN);
  if (mErrLogSetupData.PCIeCorrErrorLog)
    DeviceControl |= B_PCIE_DEVICE_CONTROL_COR_ERR_EN;
  if (mErrLogSetupData.PCIeNFErrorLog)
    DeviceControl |= B_PCIE_DEVICE_CONTROL_NF_ERR_EN;
  if (mErrLogSetupData.PCIeFatalErrorLog)
    DeviceControl |= B_PCIE_DEVICE_CONTROL_FAT_ERR_EN;

  DeviceControl &= ~(B_PCIE_DEVICE_CONTROL_UNSUP_REQ_EN);
  PciExpressWrite16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_CONTROL_OFFSET), DeviceControl);
}

VOID
PciErrLibClearPcieDeviceStatus (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function clears the legacy and standard PCI Express status registers for the
    specified PCI Express device.

Arguments:
    Bus      - PCIe Device's bus number
    Device   - PCIe Device's device number
    Function - PCIe Device's function number
    
Returns: 
    Nothing

--*/
{
  UINT8   CapabilitiesOffset;
  UINT16  PcieCapabilities;
  UINT16  StatusReg16;
  UINT32  StatusReg32;
  BOOLEAN RootPort;

  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Bus, Device, Function);
  if (CapabilitiesOffset == 0) {
    return ;
  }

  PcieCapabilities = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_CAPABILITY_OFFSET));
  if ((PcieCapabilities & 0xF0) == V_PCIE_CAPABILITY_DPT_ROOT_PORT) {
    RootPort = TRUE;
  } else {
    RootPort = FALSE;
  }

  //
  //Clear AER Uncorrectable error status register
  //
  StatusReg32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_ERRUNCSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_ERRUNCSTS), StatusReg32);

  //
  //Clear AER correctable error status register
  //
  StatusReg32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_ERRCORSTS));
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_ERRCORSTS), StatusReg32);

  if (RootPort) {
    //
	//Clear AER Root error status register
    //
	StatusReg32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_RES));
	PciExpressWrite32 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_RES), StatusReg32);
  }

  //
  // Clear the PCIe device status register
  //
  StatusReg16 = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_STATUS_OFFSET));
  if(StatusReg16 & B_PCIE_DEVICE_STATUS_FAT_ERR)  {
    StatusReg16 |= (B_PCIE_DEVICE_STATUS_FAT_ERR);
  }

  if(StatusReg16 & B_PCIE_DEVICE_STATUS_COR_ERR)  {
    StatusReg16 |= (B_PCIE_DEVICE_STATUS_COR_ERR);
  }

  if((StatusReg16 & B_PCIE_DEVICE_STATUS_NF_ERR) && (mAllowClearingOfUEStatus == FALSE))  {
    StatusReg16 &= ~(B_PCIE_DEVICE_STATUS_NF_ERR);
  }

  PciExpressWrite16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_STATUS_OFFSET), StatusReg16);

  PciErrLibClearLegacyPciDeviceStatus (Bus, Device, Function);
}


BOOLEAN
PciErrLibIsRootPortErrReportingEnabled (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function checks to see if PCIe error reporting is enabled in the specified 
    PCIe root port device.

Arguments:
    Bus      - PCIe root port device's bus number
    Device   - PCIe root port device's device number
    Function - PCIe root port device's function number
    
Returns: 
    TRUE     - Error reporting is enabled
    FALSE    - Error reporting is not enabled or it is under OS control

--*/
{
  UINT8     CapabilitiesOffset;
  UINT16    PcieCapabilities;
  UINT16    RootControl;
  BOOLEAN   IsEnabled = FALSE;


  //
  // Check if a device is present at the specified location.
  //
  if (!PciErrLibIsDevicePresent (Bus, Device, Function)) {
    return FALSE;
  }

  //
  // Get the capabilities offset and verify this is a root port
  //
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Bus, Device, Function);
  if (CapabilitiesOffset == 0) {
    return FALSE;
  }

  PcieCapabilities = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_CAPABILITY_OFFSET));
  if ((PcieCapabilities & 0xF0) != V_PCIE_CAPABILITY_DPT_ROOT_PORT) {
    return FALSE;
  }

  RootControl = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_ROOT_CONTROL_OFFSET));
  if ( RootControl & (B_PCIE_ROOT_CONTROL_FAT_ERR_EN | B_PCIE_ROOT_CONTROL_NF_ERR_EN | B_PCIE_ROOT_CONTROL_COR_ERR_EN) ) {
    IsEnabled = TRUE;
  }

  return IsEnabled;
}


BOOLEAN
PciErrLibIsErrOnPcieRootPort (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function checks the Root Error Status register to see if one of the enabled errors
    has occurred.

Arguments:
    Bus      - PCIe root port device's bus number
    Device   - PCIe root port device's device number
    Function - PCIe root port device's function number
    
Returns: 
    TRUE     - An error was detected
    FALSE    - An error was not detected, or the device is not a root port

--*/
{
  UINT8     CapabilitiesOffset;
  UINT16    PcieCapabilities;
  UINT8     AERGlobalStatus;
  BOOLEAN   IsError                            = FALSE;


  //
  // Get the capabilities offset and verify this is a root port
  //
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Bus, Device, Function);
  if (CapabilitiesOffset == 0) {
    return FALSE;
  }

  PcieCapabilities = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_CAPABILITY_OFFSET));
  if ((PcieCapabilities & 0xF0) != V_PCIE_CAPABILITY_DPT_ROOT_PORT) {
    return FALSE;
  }

  AERGlobalStatus = PciExpressRead8 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_RES));
  if (AERGlobalStatus & (B_PCIE_RES_CR | B_PCIE_RES_ENR | B_PCIE_RES_MCR | B_PCIE_RES_MENR)) {
    PciExpressWrite8 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCIE_AER_RES), AERGlobalStatus);
    IsError   = TRUE;      
  }

  return IsError;

}


BOOLEAN
PciErrLibPcieRootPortErrorHandler (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    PCI Express error handler for ESB2 and BNB PCI Express Root Ports.

Arguments:
    Bus      - PCIe root port device's bus number
    Device   - PCIe root port device's device number
    Function - PCIe root port device's function number
    
Returns: 
    TRUE     - An error was detected
    FALSE    - An error was not detected

--*/
{
  UINT8     RootPortBus      = Bus;
  UINT8     RootPortDevice   = Device;
  UINT8     RootPortFunction = Function;
  BOOLEAN   ErrorDetected    = FALSE;
  UINT8     SecondaryBusNumber;
  UINT8     SubordinateBusNumber;

  // 
  // Check for errors on subordinate devices
  //
  if (PciErrLibIsPciBridgeDevice (Bus, Device, Function)) {

    SecondaryBusNumber = PciExpressRead8 ( PCI_PCIE_ADDR(Bus,
                                           Device,
                                           Function,
                                           R_PCI_BRIDGE_SECONDARY_BUS_OFFSET
                                         ));

    SubordinateBusNumber = PciExpressRead8 ( PCI_PCIE_ADDR(Bus,
                                             Device,
                                             Function,
                                             R_PCI_BRIDGE_SUBORDINATE_BUS_OFFSET
                                           ));

    for (Bus = SecondaryBusNumber; Bus <= SubordinateBusNumber; Bus++) {
      if((SecondaryBusNumber == 00) && (SubordinateBusNumber == 00)) break;

      for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
        for (Function = 0; Function <= PCI_MAX_FUNC; Function++) {

          if ( PciErrLibIsDevicePresent (Bus, Device, Function) ) {

            if ( PciErrLibIsPcieDevice (Bus, Device, Function) ) {

              if ( PciErrLibPcieDeviceErrorHandler (Bus, Device, Function) ) {
                ErrorDetected = TRUE;
              }

            } else {

              if ( PciErrLibPciDeviceErrorHandler (Bus, Device, Function) ) {
                ErrorDetected = TRUE;
              }

              PciErrLibClearLegacyPciDeviceStatus (Bus, Device, Function);
            }
          }
        }
      }
    }
  }

  //
  // Check for a Root Port error only if we did not find any subordinate devices with an error
  //
  if ( !ErrorDetected ) {
    ErrorDetected = PciErrLibPcieDeviceErrorHandler (RootPortBus, RootPortDevice, RootPortFunction);
  }

  // 
  // Clear the Root Port status (especially the Root Port Error Message Status)
  //
  if (mAllowClearingOfPcieDeviceStatus) {
    PciErrLibClearPcieDeviceStatus (RootPortBus, RootPortDevice, RootPortFunction);
  }

  return ErrorDetected;
}

BOOLEAN
PciErrLibPcieDeviceErrorHandler (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function determines if the specified device is reporting an error and handles the
    error if one is detected.

Arguments:
    Bus      - PCIe device's bus number
    Device   - PCIe device's device number
    Function - PCIe device's function number
    
Returns: 
    TRUE     - An error was detected
    FALSE    - An error was not detected

--*/
{
  BOOLEAN   ErrorDetected = FALSE;

  //
  // Check for an Uncorrectable PCIe error
  //
  if ( PciErrLibIsFatalPcieDeviceError (Bus, Device, Function) ||
       PciErrLibIsNonfatalPcieDeviceError (Bus, Device, Function) ) {

    ErrorDetected = TRUE;
    //
    // Do not report this device as having an error if this is a P2P bridge
    // and there is a subordinate device indicating an error occurred.
    //
    if (!PciErrLibIsPciBridgeDevice (Bus, Device, Function) ||
        !PciErrLibIsSubordinateDeviceError (Bus, Device, Function) ) {

      PciErrLibHandleUncorrectablePcieDeviceError (Bus, Device, Function);
    }
  } 

  //
  // Check for a Correctable PCIe error
  //
  if ( PciErrLibIsCorrectablePcieDeviceError (Bus, Device, Function) ) {

    ErrorDetected = TRUE;

    //
    // Do not report this device as having an error if this is a P2P bridge
    // and there is a subordinate device indicating an error occurred.
    //
    if (!PciErrLibIsPciBridgeDevice (Bus, Device, Function) ||
        !PciErrLibIsSubordinateDeviceError (Bus, Device, Function) ) {

      PciErrLibHandleCorrectablePcieDeviceError (Bus, Device, Function);
    }
  } 

  if ( ErrorDetected && mAllowClearingOfPcieDeviceStatus ) {
    PciErrLibClearPcieDeviceStatus (Bus, Device, Function);
  }
  
  return ErrorDetected;
}

BOOLEAN
PciErrLibIsFatalPcieDeviceError (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function checks the SERR and PCIe device status to see if a PCIe Uncorrectable Fatal
    error has occurred.

Arguments:
    Bus      - PCIe device's bus number
    Device   - PCIe device's device number
    Function - PCIe device's function number
    
Returns: 
    TRUE     - A PCIe Uncorrectable Fatal error was detected
    FALSE    - A PCIe Uncorrectable Fatal error was not detected

--*/
{
  UINT8     CapabilitiesOffset;
  UINT16    DeviceStatus;
  UINT16    DeviceControl;
  UINT16    PrimaryStatus;
  BOOLEAN   SERREnabled;


  //
  // Get the capabilities offset
  //
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Bus, Device, Function);
  if (CapabilitiesOffset == 0) {
    return FALSE;
  }

  // 
  // Check if the device status indicates that a FATAL error occurred
  //
  DeviceStatus = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_STATUS_OFFSET));
  if ( !(DeviceStatus & B_PCIE_DEVICE_STATUS_FAT_ERR) ) {
    //
    // No FATAL error reported
    //
    return FALSE;
  }


  // 
  // FATAL error status set, now check if FATAL error reporting is enabled.
  // FATAL error reporting is enabled if SERR is enabled or FATAL reporting is enabled
  // in the PCIe device control register.
  //
  if (PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_COMMAND_OFFSET)) & B_PCI_COMMAND_SERR) {
    SERREnabled = TRUE;
  } else {
    SERREnabled = FALSE;
  }

  DeviceControl = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_CONTROL_OFFSET));

  if ( !SERREnabled && !(DeviceControl & B_PCIE_DEVICE_CONTROL_FAT_ERR_EN) ) {
    //
    // FATAL error reporting is not enabled
    //
    return FALSE;
  }

  //
  // FATAL status and FATAL reporting enabled.
  // The PCIe error status could be set even if the error was masked.
  // The PCIe error status could be set even if there are not bits set in the AER status register.
  // Check SERR status to see if a FATAL/NON-FATAL error message was sent.
  //
  if (SERREnabled) {

    PrimaryStatus = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_PRIMARY_STATUS_OFFSET));
    if (PrimaryStatus & B_PCI_STATUS_SIGNALED_SYSTEM_ERROR) {
      //
      // SERR was signaled (FATAL or Non-Fatal error occurred)
      //
      return TRUE;
    } else {
      return FALSE;
    }

  } else {
    //
    // SERR not enabled, but PCIe reporting is enabled and status indicates an error.
    //
    return TRUE;
  }
}

BOOLEAN
PciErrLibIsNonfatalPcieDeviceError (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function checks the SERR and PCIe device status to see if a Uncorrectable Non-Fatal
    PCIe error has occurred.

Arguments:
    Bus      - PCIe device's bus number
    Device   - PCIe device's device number
    Function - PCIe device's function number
    
Returns: 
    TRUE     - A PCIe Uncorrectable Non-Fatal error was detected
    FALSE    - A PCIe Uncorrectable Non-Fatal error was not detected

--*/
{
  UINT8     CapabilitiesOffset;
  UINT16    DeviceStatus;
  UINT16    DeviceControl;
  UINT16    PrimaryStatus;
  BOOLEAN   SERREnabled;
  BOOLEAN   UnsupportedRequestDetected = FALSE;


  //
  // Get the capabilities offset
  //
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Bus, Device, Function);
  if (CapabilitiesOffset == 0) {
    return FALSE;
  }

  // 
  // Check if the device status indicates that a NON-FATAL error occurred
  //
  DeviceStatus = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_STATUS_OFFSET));
  if ( !(DeviceStatus & B_PCIE_DEVICE_STATUS_NF_ERR) ) {
    //
    // No NON-FATAL error reported
    //
    return FALSE;
  }
  if (DeviceStatus & B_PCIE_DEVICE_STATUS_UNSUP_REQ) {
    UnsupportedRequestDetected = TRUE;
  }

  // 
  // NON-FATAL error status set, now check if NON-FATAL error reporting is enabled.
  // NON-FATAL error reporting is enabled if SERR is enabled or NON-FATAL reporting is enabled
  // in the PCIe device control register.
  //
  if (PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_COMMAND_OFFSET)) & B_PCI_COMMAND_SERR) {
    SERREnabled = TRUE;
  } else {
    SERREnabled = FALSE;
  }

  DeviceControl = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_CONTROL_OFFSET));

  if ( !SERREnabled && !(DeviceControl & B_PCIE_DEVICE_CONTROL_NF_ERR_EN) ) {
    //
    // NON-FATAL error reporting is not enabled
    //
    return FALSE;
  }

  //
  // NON-FATAL status and NON-FATAL reporting enabled.
  // The PCIe status could be set even if the error was masked.
  // The PCIe status could be set even if there are not bits set in the AER status register.
  // Check SERR status to see if a FATAL/NON-FATAL error message was sent.
  //
  if (SERREnabled) {

    PrimaryStatus = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_PRIMARY_STATUS_OFFSET));
    if (PrimaryStatus & B_PCI_STATUS_SIGNALED_SYSTEM_ERROR) {
      //
      // SERR was signaled (FATAL or Non-Fatal error occurred)
      //
      return TRUE;
    } else {
      return FALSE;
    }

  } else {
    //
    // SERR not enabled, but PCIe reporting is enabled and status indicates an error.
    // This is likely an Unsupported Request error that occurred during PCI bus probe.
    //
    if (UnsupportedRequestDetected) {
      //
      // Even if we incorrectly ignore this error we will NMI (if appropriate) 
      // because of the Uncorrectable error check in PciErrLibIsErrOnPcieRootPort().
      //
	  UnsupportedRequestDetected = FALSE;
	  return FALSE;
    } else {
      return TRUE;
    }
  }
}

BOOLEAN
PciErrLibIsCorrectablePcieDeviceError (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function checks the PCIe device status and the correctable error registers to see 
    if a PCIe Correctable error has occurred.

Arguments:
    Bus      - PCIe device's bus number
    Device   - PCIe device's device number
    Function - PCIe device's function number
    
Returns: 
    TRUE     - A PCIe Correctable error was detected
    FALSE    - A PCIe Correctable error was not detected

--*/
{
  UINT8     CapabilitiesOffset;
  UINT16    DeviceStatus;
  UINT16    DeviceControl;

  //
  // Get the capabilities offset
  //
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Bus, Device, Function);
  if (CapabilitiesOffset == 0) {
    return FALSE;
  }

  // 
  // Check if the device status indicates that a CORRECTABLE error occurred
  //
  DeviceStatus = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_STATUS_OFFSET));
  if ( !(DeviceStatus & B_PCIE_DEVICE_STATUS_COR_ERR) ) {
    //
    // No CORRECTABLE error
    //
    return FALSE;
  }
  // 
  // Now check if CORRECTABLE error reporting is enabled
  //
  DeviceControl = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_CONTROL_OFFSET));
  if ( !(DeviceControl & B_PCIE_DEVICE_CONTROL_COR_ERR_EN) ) {
    //
    // CORRECTABLE error reporting not enabled
    //
    return FALSE;
  }
  return TRUE;
}

BOOLEAN
PciErrLibIsSubordinateDeviceError (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function determines if there is a device reporting an error on the specified 
    P2P bridge's secondary bus or on a subordinate bus.

Arguments:
    Bus      - P2P bridge's bus number
    Device   - P2P bridge's device number
    Function - P2P bridge's function number
    
Returns: 
    TRUE     - An error was detected
    FALSE    - An error was not detected

--*/
{
  UINT8     SecondaryBusNumber;
  UINT8     SubordinateBusNumber;


  if (PciErrLibIsPciBridgeDevice (Bus, Device, Function)) {

    SecondaryBusNumber = PciExpressRead8 ( PCI_PCIE_ADDR(Bus,
                                           Device,
                                           Function,
                                           R_PCI_BRIDGE_SECONDARY_BUS_OFFSET
                                         ));

    SubordinateBusNumber = PciExpressRead8 ( PCI_PCIE_ADDR(Bus,
                                             Device,
                                             Function,
                                             R_PCI_BRIDGE_SUBORDINATE_BUS_OFFSET
                                           ));

    for (Bus = SecondaryBusNumber; Bus <= SubordinateBusNumber; Bus++) {
      if((SecondaryBusNumber == 00) && (SubordinateBusNumber == 00)) break;

      for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
        for (Function = 0; Function <= PCI_MAX_FUNC; Function++) {

          if ( PciErrLibIsDevicePresent (Bus, Device, Function) ) {

            if ( PciErrLibIsPcieDevice (Bus, Device, Function) ) {

              if ( PciErrLibIsFatalPcieDeviceError (Bus, Device, Function)      ||
                   PciErrLibIsNonfatalPcieDeviceError (Bus, Device, Function)   ||
                   PciErrLibIsCorrectablePcieDeviceError (Bus, Device, Function)   ) {

                return TRUE;
              }

            } else {

              if ( PciErrLibIsPciDeviceError (Bus, Device, Function) ) {
                return TRUE;
              }
            }
          }
        }
      }
    }
  }

  return FALSE;
}

BOOLEAN
PciErrLibIsPciDeviceError (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function checks for errors on standard PCI devices.

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number
    
Returns: 
    TRUE     - An error was detected
    FALSE    - An error was not detected

--*/
{
  UINT16                PrimaryStatus;
  UINT16                ErrorMap = B_PCI_STATUS_SIGNALED_SYSTEM_ERROR      |
                                   B_PCI_STATUS_DETECTED_PARITY_ERROR      |
                                   B_PCI_STATUS_DATA_PARITY_ERROR_DETECTED |
                                   B_PCI_STATUS_RECEIVED_TARGET_ABORT;


  if (!PciErrLibIsDevicePresent (Bus, Device, Function)) {
    return FALSE;
  }

  PrimaryStatus = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_PRIMARY_STATUS_OFFSET));
  if (!(PrimaryStatus & ErrorMap)) {
    return FALSE;
  }

  return TRUE;
}

VOID
PciErrLibHandleUncorrectablePcieDeviceError (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function handles PCIe Uncorrectable (Fatal and Non-Fatal) errors.
    An error is reported and the PCIe error detection status is updated.

Arguments:
    Bus      - PCIe device's bus number
    Device   - PCIe device's device number
    Function - PCIe device's function number
    
Returns: 
    Nothing

--*/
{
  UINT8                    CapabilitiesOffset;
  UINT16                   PCIeCapabilities;
  UINT8                    DeviceType;
  BOOLEAN                  PCIeToPCIX;
  UINT16                   SecondaryStatus;
  UINT16                   ErrorMap;
  UINT8                    PciErrorType;
  UINT16                   DeviceStatus;

  //
  // If PCIe-to-PCI-X bridge, check secondary status register for a PCI error
  //
  //
  // Get the capabilities offset
  //
  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Bus, Device, Function);
  if (CapabilitiesOffset == 0) {
    return;
  }

  PCIeCapabilities = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_CAPABILITY_OFFSET));
  DeviceType = (UINT8)(PCIeCapabilities >> 4) & 0x0F;

  PCIeToPCIX = (DeviceType == 0x07) ? TRUE : FALSE;

  if ( PCIeToPCIX ) {

    ErrorMap = B_PCI_STATUS_SIGNALED_SYSTEM_ERROR | B_PCI_STATUS_DETECTED_PARITY_ERROR |
               B_PCI_STATUS_DATA_PARITY_ERROR_DETECTED | B_PCI_STATUS_RECEIVED_TARGET_ABORT;

    SecondaryStatus = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_BRIDGE_STATUS_OFFSET));
    if ((SecondaryStatus & ErrorMap) != 0) {

      //
      // Error signaled on the secondary bus
      //
      if (SecondaryStatus & B_PCI_STATUS_SIGNALED_SYSTEM_ERROR) {
        PciErrorType = SERROR_TYPE;
      } else {
        PciErrorType = PERROR_TYPE;
      }

      //
      // Report the error
      //
      PciErrLibReportError (Bus, Device, Function, PciErrorType);

      //
      // Clear the error bits
      //
      PciExpressWrite16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_BRIDGE_STATUS_OFFSET), ErrorMap);

      return;
    }
  }
  //
  // Not a PCI error, report a FATAL PCIe error
  //
  // 
  // Check if the device status indicates that a FATAL error or uncorrected error occurred
  //
  DeviceStatus = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_STATUS_OFFSET));

  if ( (DeviceStatus & B_PCIE_DEVICE_STATUS_FAT_ERR) ) {
    //
    // FATAL error reported
    //
     if( mPciXPUnsupportedRequest != 01){
       PciErrLibReportError (Bus, Device, Function, FATAL_ERROR_TYPE);
     }
  }else{
     if( mPciXPUnsupportedRequest != 01){
	   PciErrLibReportError (Bus, Device, Function, NONFATAL_ERROR_TYPE);
     }
  }

}

VOID
PciErrLibHandleCorrectablePcieDeviceError (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function handles PCIe Correctable errors.
    An error is reported if the correctable error threshold has not been reached.
    If the threshold has been reached, correctable errors are masked in the device.

Arguments:
    Bus      - PCIe device's bus number
    Device   - PCIe device's device number
    Function - PCIe device's function number
    
Returns: 
    Nothing

--*/
{
  UINT8              CapabilitiesOffset;
  UINT16             PcieCapabilities;
  UINT16             DeviceControl;
  UINT16             RootControl;

  //
  // Check if the correctable-error reporting limit has already been reached.
  //
  if (mNumCorrectablePcieErrs < PCIE_CORRECTABLE_ERROR_COUNT_LIMIT) {
    //
    // Report the correctable error.
    //
    PciErrLibReportError (Bus, Device, Function, COR_ERROR_TYPE);
    mNumCorrectablePcieErrs++;
    return;
  }

  // 
  // Error limit has been reached.  Mask correctable errors in the device.
  //

  CapabilitiesOffset = PciErrLibGetCapabilitiesOffset (Bus, Device, Function);
  if (CapabilitiesOffset == 0) {
    return;
  }

  DeviceControl  = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_CONTROL_OFFSET));
  DeviceControl &= ~(B_PCIE_DEVICE_CONTROL_COR_ERR_EN);
  PciExpressWrite16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_DEVICE_CONTROL_OFFSET), DeviceControl);

  //
  // Update the Root Control register if this is a root port.
  //
  PcieCapabilities = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_CAPABILITY_OFFSET));
  if ((PcieCapabilities & 0xF0) == V_PCIE_CAPABILITY_DPT_ROOT_PORT) {
    RootControl  = PciExpressRead16 ( PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_ROOT_CONTROL_OFFSET));
    RootControl &= ~(B_PCIE_ROOT_CONTROL_COR_ERR_EN);
    PciExpressWrite16 (PCI_PCIE_ADDR(Bus, Device, Function, CapabilitiesOffset + R_PCIE_ROOT_CONTROL_OFFSET), RootControl);
  }
}

BOOLEAN
PciErrLibPciDeviceErrorHandler (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function
  )
/*++

Routine Description:
    This function checks for, and handles, errors on standard PCI devices.

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number
    
Returns: 
    TRUE     - An error was detected
    FALSE    - An error was not detected

--*/
{
  UINT16                    PrimaryStatus;
  UINT8                     PciErrorType;

  UINT16                    ErrorMap = B_PCI_STATUS_DETECTED_PARITY_ERROR      |
                                       B_PCI_STATUS_SIGNALED_SYSTEM_ERROR      |
                                       B_PCI_STATUS_RECEIVED_TARGET_ABORT      |
                                       B_PCI_STATUS_DATA_PARITY_ERROR_DETECTED;


  if (!PciErrLibIsDevicePresent (Bus, Device, Function)) {
    return FALSE;
  }

  //
  // The standard behavior is outlined below:
  // 1. #SERR generation is controlled thru CMD.SEE and CMD.PERE.
  // 2. #SERR generation can also occur if the device is capable of being a bus master 
  //    on the primary bus and can detect parity errors. Optionally, #SERR can also be
  //    triggered by setting a chipset-specific policy for bridges and bus masters for 
  //    target and master aborts...
  //
  PrimaryStatus = PciExpressRead16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_PRIMARY_STATUS_OFFSET));
  if (!(PrimaryStatus & ErrorMap)) {
    return FALSE;
  }

  //
  // Error signaled on the primary bus...
  //
  if (PrimaryStatus & B_PCI_STATUS_SIGNALED_SYSTEM_ERROR) {
    PciErrorType = SERROR_TYPE;
  } else {
    PciErrorType = PERROR_TYPE;
  }

  //
  // Report the error...
  //
  PciErrLibReportError (Bus, Device, Function, PciErrorType);

  //
  // Clear the error bits...
  //
  PciExpressWrite16 (PCI_PCIE_ADDR(Bus, Device, Function, R_PCI_PRIMARY_STATUS_OFFSET), ErrorMap);

  return TRUE;
}

VOID
PciErrLibReportError (
  IN UINT8                    Bus,
  IN UINT8                    Device,
  IN UINT8                    Function,
  IN UINT8                    ErrorType
  )
/*++

Routine Description:
    This function logs the appropriate type of SEL error and updates the status
    of PCI error detection flags.

Arguments:
    Bus       - Device's bus number
    Device    - Device's device number
    Function  - Device's function number
    ErrorType - Type of the error to report (SERR/PERR/Correctable/...)

Returns: 
    Nothing

--*/
{
  // 
  // NV ELog support 
  // 
  PCI_PCIE_ERROR_LOCAL_STRUC            PciErrStruc;
  
  // Denverton Aptiov override Start - EIP#249150
  /*
  //
  // Whea support
  //
  PCIE_PCI_DEV_INFO                     PciDev;
  UINT16                                WheaErrType;

  PciDev.Segment      = 0;
  PciDev.Bus          = Bus;
  PciDev.Device       = Device;
  PciDev.Function     = Function;
  */
  // Denverton Aptiov override End - EIP#249150

  PciErrStruc.ErrorSource  = PciSource;
  PciErrStruc.ErrorType    = ErrorType;
  PciErrStruc.Segment      = 0;
  PciErrStruc.Bus          = Bus;
  PciErrStruc.Device       = Device;
  PciErrStruc.Function     = Function;
  
  // Denverton Aptiov override Start - EIP#249150
  
  //
  // Logging Error using Runtime Error Logging Listener
  //
  CommonErrorHandling((VOID *) &PciErrStruc); 
// Denverton AptioV Override Start - EIP#275412
 
  switch (ErrorType) {

    case SERROR_TYPE:

      mPciSerrDetected        = TRUE;
      break;

    case PERROR_TYPE:
      mPciPerrDetected        = TRUE;
      break;

    case FATAL_ERROR_TYPE:
      mPcieFatalErrDetected   = TRUE;
      break;

    default:
      return;
  }
  // Denverton AptioV Override End - EIP#275412
/*
  //
  // Whea support
  //
  if (mWheaPlatformSupportAvailable && mErrLogSetupData.WheaLogging) {
    if (WheaErrType == GEN_ERR_SEV_PCI_DEV_RECOVERABLE || WheaErrType == GEN_ERR_SEV_PCI_DEV_CORRECTED || \
        WheaErrType == GEN_ERR_SEV_PCI_DEV_FATAL || WheaErrType == GEN_ERR_SEV_PCI_DEV_INFORMATION) {
       mWheaPlatformSupport->WheaPlatformElogPciDev (mWheaPlatformSupport, WheaErrType, &PciDev);
    } else {
       mWheaPlatformSupport->WheaPlatformElogPcieRootDevBridge (mWheaPlatformSupport, WheaErrType, &PciDev);
    }
  }  */
  
  // Denverton Aptiov override End - EIP#249150
}

VOID
PciErrLibEnableIOSFParity (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function,
  IN UINT8   PortType
  )
/*++

Routine Description:
    This function initializes and enables IOSF Parity error reporting in the specified PCIe
    Root ports.

Arguments:
    Bus      - Device's bus number
    Device   - Device's device number
    Function - Device's function number
    PortType - Device's Type "Root port/Virtual Root port
    
Returns: 
    Nothing

--*/
{
  UINT32    Data32;
  UINT32    Register = R_PCIE_XPTDEF2;

  //
  //Enable IOSF Parity checking
  //
  if(PortType) {
    Register = R_vPCIE_XPTDEF2;
  }

  Data32 = PciExpressRead32 (PCI_PCIE_ADDR(Bus, Device, Function, Register));
  Data32 &= ~(B_PCIE_TCPAR_CHK_DIS | B_PCIE_TDPAR_CHK_DIS);
  PciExpressWrite32 (PCI_PCIE_ADDR(Bus, Device, Function, Register), Data32);
}
