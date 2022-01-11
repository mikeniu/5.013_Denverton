/** @file
  This file is a library for PCIE Platform specific Initalization.

@copyright
 Copyright (c) 2015 - 2017 Intel Corporation. All rights reserved
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
#include <PcieIpPlatformLib.h>
#include <PcieIpPlatform.h>
#include <PcieIpPolicy.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MmPciBaseLib.h>
#include <PcieIpRegs.h>
#include <library/S3BootScriptLib.h>
#include <Library/PeiDxeSmmFiaMuxHelperLib.h>
#include <Library/MeFiaMuxLib.h>
#include <PchPmcStPg.h>

/* ***************************** private functions  ***************************** */
EFI_STATUS
PeiPcieSetLanes(
  IN PMC_ST_PG_FUNCTION function,
  IN BOOLEAN boolValue
  )
{
	EFI_STATUS Status = EFI_SUCCESS;
	if ( boolValue == TRUE ) {
      Status = PmcStPgEnableFunction( function );
      DEBUG ((DEBUG_INFO, "Enabled PCIE cluster %x!!\n", function ));
	} else {
      DEBUG ((DEBUG_INFO, "Disable PCIE cluster %x!!\n", function ));
	  Status = PmcStPgDisableFunction( function );
	}
	return Status;
}

EFI_STATUS
PeiPcieValidateFirstCluster ( )
{
  EFI_STATUS Status = EFI_SUCCESS;
  Status = FiaMuxConfigLanesQuerySingleMatch( 0, 7, ME_FIA_MUX_LANE_PCIE );
  return Status;
}

EFI_STATUS
PeiPcieValidateSecondCluster ( )
{
  EFI_STATUS Status = EFI_SUCCESS;
  Status = FiaMuxConfigLanesQuerySingleMatch( 8, 15, ME_FIA_MUX_LANE_PCIE );
  return Status;
}

EFI_STATUS
PeiPcieFiaLanes(
  IN PCIE_IP_GLOBAL_POLICY_PPI  *PcieIpGlobaIpolicy
)
/*++

Routine Description:

  Verify if FIA and PCIE bifurcation are in sync

Arguments:

  pPcieIpGlobaIpolicy    pointer to array of Root port base policy.
  pFiaConfig             pointer to FIA configuration

--*/
{
  EFI_STATUS Status = EFI_SUCCESS;
  Status = PeiPcieValidateFirstCluster( );
#ifndef FSP_FLAG
  if ( Status == EFI_ABORTED ) {
#else
  if ( Status == EFI_ABORTED || PcieIpGlobaIpolicy->PcieIpClusterPolicy[0].PcieEnable == 0) {
#endif //FSP_FLAG
	  if ( PeiPcieSetLanes( PMC_ST_PG_PCIE0 , FALSE ) != EFI_SUCCESS ) {
		DEBUG ((DEBUG_ERROR, "First PCIE cluster couldn't be disabled!! \n" ));
	  } else {
		DEBUG ((DEBUG_ERROR, "First PCIE cluster was disabled!! \n" ));
	  }
  } else {
	  if ( PeiPcieSetLanes( PMC_ST_PG_PCIE0 , TRUE ) != EFI_SUCCESS ) {
		DEBUG ((DEBUG_ERROR, "First PCIE cluster couldn't be Enabled!! \n" ));
	  } else {
		DEBUG ((DEBUG_ERROR, "First PCIE cluster was enabled!! \n" ));
	  }
  }
  Status = PeiPcieValidateSecondCluster( );
#ifndef FSP_FLAG
  if ( Status == EFI_ABORTED ) {
#else
  if ( Status == EFI_ABORTED || PcieIpGlobaIpolicy->PcieIpClusterPolicy[1].PcieEnable == 0 ) {
#endif //FSP_FLAG
	  if ( PeiPcieSetLanes( PMC_ST_PG_PCIE1 , FALSE ) != EFI_SUCCESS ) {
		DEBUG ((DEBUG_ERROR, "Second PCIE cluster couldn't be disabled!! \n" ));
	  } else {
		DEBUG ((DEBUG_ERROR, "Second PCIE cluster was disabled!! \n" ));
	  }
  } else {
	  if ( PeiPcieSetLanes( PMC_ST_PG_PCIE1 , TRUE ) != EFI_SUCCESS ) {
		DEBUG ((DEBUG_ERROR, "Second PCIE cluster couldn't be enabled!! \n" ));
	  } else {
		DEBUG ((DEBUG_ERROR, "Second PCIE cluster was enabled!! \n" ));
	  }
  }
  return Status;
}

EFI_STATUS
PcieIpPlatHidePort(
  PCIE_IP_PORT_POLICY *PciIpPortPolicy
)
{
  return 0;
}

EFI_STATUS
PcieIpPlatPllHandoff(
  PCIE_IP_GLOBAL_POLICY_PPI *PciIpGlobalPolicy
)
{
  return 0;
}

EFI_STATUS
PcieIpPlatPowerGateController()
{
  return 0;
}

/**
  Get Pch Maximum Pcie Root Port Number
  @retval Pch Maximum Pcie Root Port Number
**/
UINT8
EFIAPI
PcieIPGetPchMaxPciePortNum (
  VOID
  )
/**
  Get count of standard and virtual root port ( temporal )

  Arguments:

  Returns:
  @retval count 			- Count standard and virtual root port
**/
{
	return MAX_PCIE_ROOT_PORTS;
}

UINTN
EFIAPI
PeiPcieIpGetRootPortBase (
  PCIE_IP_BDF_MAP_ENTRY *RootPort
  )
/**
  Get standard  root port base address

  Arguments:
  @param[in] RootPort - Root port information

  Returns:
  @retval base address 						- The function completed successfully.
  @retval V_ERROR_PCI_DEVICE_IS_NOT_PRESENT - Device is not present
**/
{
  if ( RootPort != NULL) {
    if ( MmioRead16 ( MmPciBase (  RootPort->BusNum,
		                         RootPort->DevNum,
								 RootPort->FuncNum ) + R_PCIE_VENDOR_ID ) == (UINT16) ~0)
    {
	  //
	  // Indicates rp is not present
	  //
      return (UINTN) V_ERROR_PCI_DEVICE_IS_NOT_PRESENT;
    } else {
      return  (UINTN) MmPciBase ( RootPort->BusNum, RootPort->DevNum, RootPort->FuncNum );
    }
  } else {
    return (UINTN)V_ERROR_PCI_DEVICE_IS_NOT_PRESENT;
  }
}

/**
  Get PCH PWRM base address.

  @param[out] Address                   Address of PWRM base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid pointer passed.
**/
EFI_STATUS
EFIAPI
PcieIpPlatformPchPwrmBaseGet (
  OUT UINT32                            *Address
  )
{
  UINTN                                 PmcBase;
  PmcBase      = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_PMC,
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );
  if (MmioRead16 (PmcBase) == 0xFFFF) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }
  *Address = MmioRead32 (PmcBase + R_PCH_PMC_PWRM_BASE) & B_PCH_PMC_PWRM_BASE_BAR;
  return EFI_SUCCESS;
}

/* *********************************** public functions ************************************** */

EFI_STATUS
PcieIpPlatformPortBdfMap(
  IN PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobalPolicy
	)
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
{
  UINT8 Index;
  UINT32  devmap[]={
   V_DEVICE_NUMBER_PCIE_ROOT_PORT_0
  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_1
  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_2
  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_3
  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_4
  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_5
  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_6
  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_7
  ,V_DEVICE_NUMBER_PCIE_VIRTUAL_ROOT_PORT_0
  ,V_DEVICE_NUMBER_PCIE_VIRTUAL_ROOT_PORT_1
  ,V_DEVICE_NUMBER_PCIE_VIRTUAL_ROOT_PORT_2
  };
  EFI_STATUS Status = RETURN_INVALID_PARAMETER;

  if ( PcieIpGlobalPolicy == NULL ) {
    return Status;
  }

  for( Index = 0; Index < MAX_PCIE_ROOT_PORTS ; Index++ ) {
	  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].PortNumber = Index;
	  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].PortType = PcieIpGlobalPolicy->PcieIpPortPolicy[ Index ].VirtualRootPort;
	  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].Enabled = TRUE;
	  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].BusNum = 0;
	  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].DevNum = devmap[ Index ];
	  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].FuncNum = 0;
	  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].RPBase = PeiPcieIpGetRootPortBase( &PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ] );
	  PcieIpGlobalPolicy->PcieIpPortPolicy[ Index ].RPBase = PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].RPBase;
	  DEBUG ((DEBUG_INFO, "RPBase %d -> 0x%08X, port %d, type: %d, bus %d, device %d \n",
			  Index,
			  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].RPBase,
			  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].PortNumber,
			  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].PortType,
			  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].BusNum,
			  PcieIpGlobalPolicy->PcieIpPortBdfMap[ Index ].DevNum
			  ));
  }
  Status = EFI_SUCCESS;
  return Status;
}

VOID
PcieIpPlatformInitDefault (
  IN  PCIE_IP_GLOBAL_POLICY_PPI 	*PcieIpGlobalPolicyPpi
  )
/**
  Fill PCIE policy with default values

  @param[in] PcieIpGlobalPolicyPpi - The pointer to PCIE IP Policy PPI instance

  @retval EFI_SUCCESS              - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES     - Insufficient resources to create buffer
**/
{
  UINT8 Index;
  //
  // PCI express config, endpoints
  //
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.DeviceCtlConfig.MaxPayload                 = V_PCIE_MAX_PAYLOAD_SIZE_DEFAULT;
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.DeviceCtlConfig.MaxReadRequest             = V_PCIE_MAX_READ_REQUEST_SIZE_DEFAULT;
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.DeviceCtlConfig.ExtTagField                = V_PCIE_EXT_TAG_FIELD_DEFAULT;
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.DeviceCtlConfig.RelaxedOrdering            = V_PCIE_RELAXED_ORDERING_DEFAULT;
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.ExtSync                                    = V_PCIE_EXTENDED_SYNCH_DEFAULT;
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.Aspm                                       = V_PCIE_ASPM_L1_DEFAULT;

  //
  // PCI express config, root port
  //
  for( Index = 0 ; Index < MAX_PCIE_ROOT_PORTS ; Index++ ) {
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].ExtTagField                      = V_PCIE_EXT_TAG_FIELD_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].DeviceCtlConfig.MaxPayload       = V_PCIE_MAX_PAYLOAD_SIZE_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].DeviceCtlConfig.MaxReadRequest   = V_PCIE_MAX_READ_REQUEST_SIZE_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].DeviceCtlConfig.EnableSLD        = V_PCIE_SURPRISE_LINK_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].DeviceCtlConfig.RelaxedOrdering  = V_PCIE_RELAXED_ORDERING_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].WheaPcieErrInj                   = V_PCIE_ERROR_INJECTION_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].CtlLaneReversal                  = V_PCIE_REVERSEVAL_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].ExtSync                          = V_PCIE_EXTENDED_SYNCH_DEFAULT;
	PcieIpGlobalPolicyPpi->PcieIpPortBdfMap[ Index ].SlotImplemented 	              = 0;
    PcieIpGlobalPolicyPpi->PcieIpPortBdfMap[ Index ].Enabled                          = TRUE;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].PmSci 				              = V_PCIE_PMSCI_DEFAULT;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].Aspm                             = V_PCIE_ASPM_L1_DEFAULT;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].HotPlug                          = V_PCIE_HOT_PLUG_DEFAULT;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].UnsupportedRequestReport         = V_PCIE_UNSUPPORTED_REQUEST_DEFAULT;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].TransmitterHalfSwing             = 0;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].PcieSpeed                        = V_PCIE_MAXIMUM_LINK_SPEED_DEFAULT;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].DeEmphasis                       = V_PCIE_DE_EMPHASIS_DEFAULT;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].ClockGatingEn                    = V_PCIE_CLOCK_GATING_ENABLE_DEFAULT;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].VirtualRootPort                  = FALSE;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].CompletionTimeout                = V_PCIE_COMPLETION_TIMEOUT_DEFAULT;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].CompletionTimeoutRange           = V_PCIE_COMPLETION_TIMEOUT_RANGE_DEFAULT;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].DeviceCtlConfig.LtrEnable        = V_PCIE_LTR_ENABLE_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].Lock							  = V_PCIE_LOCK_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].StopScream                       = V_PCIE_STOP_SCREAM_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].CtlLaneReversal                  = V_PCIE_LANE_REVERSAL_DEFAULT;
  }
  PcieIpGlobalPolicyPpi->EnableMphyMsg                                                = V_PCIE_DISABLE_MPHY_DEFAULT;

  for (Index = 0; Index < MAX_PCIE_CLUSTERS; Index++) {			//PCH_MAX_PCIE_CONTROLLERS what about it? = clusters?
    PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[ Index ].Bifurcation                   = V_PCIE_BIFURCATION_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[ Index ].Straps                        = V_PCIE_STRAP_DEFAULT;
    PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[ Index ].ReplyTimerTimeout                = V_PCIE_REPLY_TIMER_TIMEOUT_DEFAULT;
  }
}

VOID
PcieIpPlatformLTR(
  IN UINTN RPBase,
  IN BOOLEAN LtrEnable
		)
{
  UINT32                            Data32;
//  UINT16                            Data16;
  Data32 = MmioRead32 (RPBase + R_PCH_PCIE_DCAP2);
  ///
  /// PCH BIOS Spec Rev 0.5.5, Section 8.14.1 Power Optimizer Configuration
  /// Step 1
  /// Enable support Latency Tolerance Reporting (LTR)
  /// Step 1.1
  /// Program B0:D28:F0~F7 + 400h to 883C883Ch for ports which has a PCIe
  /// device attached to it.
  /// Done in PcieSetPm()
  /// Step 1.2
  /// Program B0:D28:F0~F7 + 404h [1:0] = 11b for ports which has a PCIe device
  /// device attached to it.
  /// Done in PcieSetPm()
  /// Step 1.3
  /// Program B0:D28:F0-F7 + 64h [11] = 1b
  ///
  if (LtrEnable == TRUE) {
	Data32 |= BIT11;
  } else {
	Data32 &= (UINT32) ~(BIT11);
  }
  MmioWrite32 (RPBase + R_PCH_PCIE_DCAP2, Data32);
  S3BootScriptSaveMemWrite (
	S3BootScriptWidthUint32,
	(UINTN) (RPBase + R_PCH_PCIE_DCAP2),
	1,
	&Data32
	);
}


VOID
PcieIpPlatformSlotImplemented(
  IN UINTN RPBase,
  IN UINTN PhysicalSlotNumber,
  IN BOOLEAN HotPlug
		)
///
/// Set the Slot Implmemented Bit.  Note that this must be set before
/// presence is valid.
/// PCH BIOS Spec Rev 0.5.0 section 8.2.2, The System BIOS must
/// initialize the "Slot Implemented" bit of the PCI Express* Capabilities Register,
/// XCAP D28:F0~7:Reg 42h[8] of each available and enabled downstream root port.
/// Setting this bit will indicate that the PCI Express* link associated with this
/// port is connected to a slot (as compared to being connected to an integrated
/// device component).
///
{
  UINT32                            Data16Or;
  UINT32                            Data16And;
  UINT32                            Data32Or;
  UINT32                            Data32And;
    ///
    /// Slot Implemented enabled earlier. Here will only save this register for enabled ports
    ///
    Data16Or  = BIT8;
    Data16And = 0xFFFF;
    S3BootScriptSaveMemReadWrite (
      S3BootScriptWidthUint16,
      (UINTN) (RPBase + R_PCH_PCIE_XCAP),
      &Data16Or,  // Data to be ORed
      &Data16And  // Data to be ANDed
      );
    ///
    /// For Root Port Slots Numbering on the CRBs.
    ///
    Data32Or  = 0;
    Data32And = (UINT32) (~(B_PCH_PCIE_SLCAP_SLV | B_PCH_PCIE_SLCAP_SLS | B_PCH_PCIE_SLCAP_PSN));
    ///
    /// PCH BIOS Spec Rev 0.5.0 section 8.8.2.1
    /// Note: If Hot Plug is supported, then write a 1 to the Hot Plug Capable (bit6) and Hot Plug
    /// Surprise (bit5) in the Slot Capabilities register, D28:F0~7:Reg 54h. Otherwise,
    /// write 0 to the bits PCIe Hot Plug SCI Enable
    ///
    Data32And &= (UINT32) (~(B_PCH_PCIE_SLCAP_HPC | B_PCH_PCIE_SLCAP_HPS));
    if (HotPlug) {
      Data32Or |= B_PCH_PCIE_SLCAP_HPC | B_PCH_PCIE_SLCAP_HPS;
    }
    ///
    /// Get the width from LCAP
    /// Slot Type  X1  X4/X8  X16
    /// Default     10W   25W  75W
    /// The slot power consumption and allocation is platform specific. Please refer to the
    /// "PCI Express* Card Electromechanical (CEM) Spec" for details.
    /// bugbug what's the default setting for X2
    ///
    if ((((MmioRead32 (RPBase + R_PCH_PCIE_LCAP)) & B_PCH_PCIE_LCAP_MLW) >> 4) == 0x01) {
      Data32Or |= (UINT32) (100 << 7);
      Data32Or |= (UINT32) (1 << 15);
    } else if ((((MmioRead32 (RPBase + R_PCH_PCIE_LCAP)) & B_PCH_PCIE_LCAP_MLW) >> 4) >= 0x04) {
      Data32Or |= (UINT32) (250 << 7);
      Data32Or |= (UINT32) (1 << 15);
    }

    Data32Or |= (UINT32) (PhysicalSlotNumber << 19);
    MmioAndThenOr32 (RPBase + R_PCH_PCIE_SLCAP, Data32And, Data32Or);
    S3BootScriptSaveMemReadWrite (
      S3BootScriptWidthUint32,
      (UINTN) (RPBase + R_PCH_PCIE_SLCAP),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
}

VOID
PcieIpPlatformHotPlug(
  IN UINTN RPBase
  )
{
  UINT16                            Data16Or;
  UINT16                            Data16And;
  UINT32                            Data32Or;
  UINT32                            Data32And;
  UINT32                            PwrmBase = 0;
  ///
  /// PCH BIOS Spec Rev 0.5.0 section 8.8.2.1
  /// Step 1
  /// Clear following status bits, by writing 1b to them, in the Slot
  /// Status register at offset 1Ah of PCI Express Capability structure:SLOTCTL (SLOTCTL) - @0x000058
  /// Attention Button Pressed (bit0)
  /// Presence Detect Changed (bit3)
  ///
  Data16And = 0xFFFF;
  Data16Or  = (BIT3 | BIT0);
  MmioOr16 (RPBase + R_PCH_PCIE_SLCTL, Data16Or);
  S3BootScriptSaveMemReadWrite (
    S3BootScriptWidthUint16,
    (UINTN) (RPBase + R_PCH_PCIE_SLCTL),
    &Data16Or,  // Data to be ORed
    &Data16And  // Data to be ANDed
    );
  ///
  /// Step 2
  /// Program the following bits in Slot Control register at offset 18h
  /// of PCI Express* Capability structure: SLOTCTL (SLOTCTL) - @0x000058
  /// Attention Button Pressed Enable (bit0) = 1b
  /// Presence Detect Changed Enable (bit3) = 1b
  /// Hot Plug Interrupt Enable (bit5) = 1b
  ///
  Data16And = 0xFFFF;
  Data16Or  = (BIT5| BIT3 | BIT0);
  MmioAndThenOr16 (RPBase + R_PCH_PCIE_SLCTL, Data16And, Data16Or);
  S3BootScriptSaveMemReadWrite (
    S3BootScriptWidthUint16,
    (UINTN) (RPBase + R_PCH_PCIE_SLCTL),
    &Data16Or,  // Data to be ORed
    &Data16And  // Data to be ANDed
    );
  ///
  /// Step 3
  /// Program Misc Port Config (MPC) register at PCI config space offset
  /// D8h as follows:
  /// Hot Plug SCI Enable (HPCE, bit1) = 1b  SCICSR (SCICSR) - @0x0000CC  bit 1
  /// Hot Plug SMI Enable (HPME, bit1) = 0b  SMICSR (SMICSR) - @0x0000C8   bit 1 default option
  ///
  Data32And = 0xFFFFFFFF;
  Data32Or  = B_PCH_PCIE_SMICSR_HPDLLSCS | B_PCH_PCIE_SCICSR_HPCE;
  MmioAndThenOr32 (RPBase + R_PCH_PCIE_SCICSR, Data32And, Data32Or);
  S3BootScriptSaveMemReadWrite (
    EfiBootScriptWidthUint32,
    (UINTN) (RPBase + R_PCH_PCIE_SCICSR),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );
  ///
  /// Step 4
  /// Clear GPE0 Register (PMBase+20h), bit1, HOT_PLUG_STS by writing 1
  ///
  if ( PcieIpPlatformPchPwrmBaseGet (&PwrmBase) == EFI_SUCCESS ) {
	IoWrite32 (PwrmBase + R_PCH_ACPI_GPE0_STS_127_96, (UINT32) B_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG);
  } else {
	DEBUG ((DEBUG_INFO, "HotPlug has error\n" ));
  }
  ///
  /// Step 5
  /// Clear Hot Plug DLLSC SCI Status - HPLSCSCISTS (SCICSR, bit18) by writing 1
  ///
  if(MmioRead32(RPBase + R_PCH_PCIE_SCICSR) & B_PCH_PCIE_SMICSR_HPDLLSCS) {
    Data32And = 0xFFFFFFFF;
    Data32Or  = B_PCH_PCIE_SMICSR_HPDLLSCS;
    MmioAndThenOr32 (RPBase + R_PCH_PCIE_SCICSR, Data32And, Data32Or);
    S3BootScriptSaveMemReadWrite (
      EfiBootScriptWidthUint32,
      (UINTN) (RPBase + R_PCH_PCIE_SCICSR),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
    ); 
  }
  ///
  /// Step 6
  /// Set Slot Implemented - SI (EXPCAP, bit8) = 1
  ///
  Data16And = 0xFFFF;
  Data16Or  = (B_PCH_PCIE_XCAP_SI);
  MmioAndThenOr16 (RPBase + R_PCH_PCIE_XCAP, Data16And, Data16Or);
  S3BootScriptSaveMemReadWrite (
    S3BootScriptWidthUint16,
    (UINTN) (RPBase + R_PCH_PCIE_XCAP),
    &Data16Or,  // Data to be ORed
    &Data16And  // Data to be ANDed
    );
}

BOOLEAN
EFIAPI
PcieIpPlatformPchIsPciePresent (
  VOID
  )
/**
  Check if PCIE is present using FUSES

  @retval TRUE                    PCIE is supported in this chipset
  @retval FALSE                   PCIE is not supported
**/
{
  UINT32  PwrmBase;
  UINT32  FuseDis2State;
  if ( PcieIpPlatformPchPwrmBaseGet (&PwrmBase) == EFI_SUCCESS ) {
	FuseDis2State = MmioRead32 ((UINTN) (PwrmBase + R_PCH_PWRM_STPG_FUSE_SS_DIS_RD_2));
	return ( (FuseDis2State & B_PCH_PWRM_PCIE1_FUSE_SS_DIS) == 0) &&
	                ((FuseDis2State & B_PCH_PWRM_PCIE0_FUSE_SS_DIS) == 0)  ;
  } else {
	  return FALSE;
  }
}

UINT8
EFIAPI
PcieIPGetPchStandardPciePortNum (
	  VOID
	  )
/**
	  Get count of only standard root port

	  Arguments:

	  Returns:
	  @retval count             - Count standard root port
**/
{
	    return PCIE_IP_STANDARD_ROOT_PORT;
}

EFI_STATUS
PcieIpPlatformFIA(
  IN PCIE_IP_GLOBAL_POLICY_PPI  *PcieIpGlobaIpolicy
  )
/**
  Verify if the PCIE clusters will be used, if not disable it

  Arguments:
  IN PcieIpGlobaIpolicy  PCIE policy

  Returns:
  @retval EFI_STATUS  - EFI_SUCCESS= FIA and PCIE are in sync, was disabled the right PCIE clusters
  	  	  	  	  	  - EFI_ABORTED= There was problems when FIA and PCIE try to sync
**/

{
  EFI_STATUS Status;

  Status= PeiPcieFiaLanes(PcieIpGlobaIpolicy);
  if ( Status == EFI_SUCCESS ) {
	DEBUG ((DEBUG_INFO, "FIA & PCIE are in sync\n" ));
  } else {
	DEBUG ((DEBUG_ERROR, "FIA & PCIE are NOT in sync\n" ));
  }
  return Status;
}
