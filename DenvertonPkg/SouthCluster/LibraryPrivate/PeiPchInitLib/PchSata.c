/** @file
  Configures PCH Sata Controller for PCH-H

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
#include "PchInitPei.h"
/**
  Perform PCH SKU detection to check if the RST PCIe Storage Remapping feature is supported
  Prior to any PCIe device enumeration, initial the RST PCIe Storage "Cycle Router Enable (CRE)" bit to 1
  If the CRE bit value after the write is 1, it indicates the bit is RW,and is enabled by fuse
  else, it indicates the bit is RO only and is disabled by fuse
  The value after the write is stored in RST PCIe Storage Cycle Router Global Control regiser, and then CRE bit is cleared

  @retval None
**/
VOID
PchRstPcieStorageCreEarlyInit (
  )
{
  UINTN       i;
  UINT8       Data8;
  UINTN       PciSataRegBase;

  DEBUG ((DEBUG_INFO, "PchRstPcieStorageCreEarlyInit() - Start\n"));

  PciSataRegBase  = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_SATA,
                      PCI_FUNCTION_NUMBER_PCH_SATA
                      );

  ///
  /// Read Vendor ID to check if Sata exists
  /// Return if Sata is not exist
  ///
  if (MmioRead16 (PciSataRegBase + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    return;
  }

  ///
  /// Prior to any PCIe device enum, BIOS needs to detect the PCH SKU to
  /// make sure the RST PCIe Storage Remapping feature is supported
  ///
  for (i = 0; i < PCH_MAX_RST_PCIE_STORAGE_CR; i++) {
    ///
    /// Set the related RST PCIe Storage Cycle Router
    ///
    MmioWrite8 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_CRGC, (UINT8) i);

    ///
    /// Set the "Port Configuration Check Disable" bit, Sata PCI offset 304h [31]
    ///
    MmioOr32 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_GSR, B_PCH_RST_PCIE_STORAGE_GSR_PCCD);

    ///
    /// Set RST PCIe Storage's Cycle Router Enabled Bit for CR#i
    ///
    MmioOr8 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_GCR, (UINT8) B_PCH_RST_PCIE_STORAGE_GCR_CRE);

    ///
    /// Write the value into SATA PCI offset FC0h[16+i], where i = 0 for RST PCIe Storage Cycle Router 1 and so on
    ///
    Data8 = MmioRead8 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_GCR) & B_PCH_RST_PCIE_STORAGE_GCR_CRE;
    MmioOr32 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_CRGC, (UINT32) (Data8 << (16+i)));

    ///
    /// Clear RST PCIe Storage's Cycle Router Enabled Bit for CR#i
    ///
    MmioAnd8 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_GCR, (UINT8) ~(B_PCH_RST_PCIE_STORAGE_GCR_CRE));

    ///
    /// Clear the "Port Configuration Check Disable" bit, Sata PCI offset 304h [31]
    ///
    MmioAnd32 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_GSR, (UINT32) ~B_PCH_RST_PCIE_STORAGE_GSR_PCCD);

  }


  DEBUG ((DEBUG_INFO, "PchRstPcieStorageCreEarlyInit() - End\n"));
}

/**
  Check if any lanes are assigned to SATA

  @retval TRUE                    Sata lanes exist
  @retval FALSE                   Sata lanes does not exist
**/
BOOLEAN
SataLaneExist (
  VOID
  )
{
  UINT8                 Index;
  UINT8                 PortId;
  UINT8                 LaneOwner;
  UINT8                 MaxSataPorts;
  UINT8                 LaneNum;

  MaxSataPorts = GetPchMaxSataPortNum ();
    for (Index = 0; Index < MaxSataPorts; Index++) {
      PchGetSataLaneNum (Index, &LaneNum);
      PchGetLaneInfo (LaneNum, &PortId, &LaneOwner);
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        return TRUE;
      }
    }
  return FALSE;
}

/**
  //
  // Program the HSIO Settings Specific to the SPT Implementation - Elastic Buffer Configuration
  //
  @retval None
**/
VOID
ProgramSataTestMode (
  VOID
  )
{
  UINT32                Data32And;
  UINT32                Data32Or;
  UINT16                Offset;
  HSIO_LANE             HsioLane;
  UINT8                 LaneOwner;
  UINT8                 PortId;
  UINT8                 CurrLane;

  //
  // If Loopback mode, the EB inside the PHY logic is in 'half-empty' mode.
  // Each SATA port/lane in the HSIO space must be written with these values.if Sata Test Mode is enabled
  //    i.)  cri_rxeb_lowater[4:0]   = 5'h0A
  //    ii.) cri_rxeb_ptr_init[4:0]  = 5'h10
  //

  Data32And = (UINT32)~(B_PCH_HSIO_PCS_DWORD8_CRI_RXEB_PTR_INIT_4_0 | B_PCH_HSIO_PCS_DWORD8_CRI_RXEB_LOWATER_4_0);
  Data32Or  = (UINT32)(0x100A0000);


  for(CurrLane = 4; CurrLane <=19; CurrLane++) {
    if (PchGetLaneInfo (CurrLane, &PortId, &LaneOwner) == EFI_SUCCESS) {
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        PchPcrAndThenOr32 (PortId, R_PCH_H_HSIO_LANE18_PCS_DWORD8, Data32And, Data32Or);
        HsioGetLane (CurrLane, &HsioLane);
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
          (UINT32) (5 << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
      }
    }
  }
}

/**
  Program AHCI PI register for Enabled ports
  Handle hotplug, and interlock switch setting,
  and config related GPIOs.

  @param[in] SataConfig           The PCH Policy for SATA configuration
  @param[in] AhciBarAddress       The Base Address for AHCI BAR

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigurePchSataAhci (
#ifdef PCH_PO_FLAG
  IN  CONST UINT32                    SataSafeRegister,
#endif // PCH_PO_FLAG
  IN  CONST PCH_SATA_CONFIG           *SataConfig,
  IN  UINT32                          AhciBarAddress
  ,IN UINT8                            SataDeviceNumber
  )
{
  UINT32                AhciBar;
  UINT32                Data32And;
  UINT32                Data32Or;
  UINT32                PxCMDRegister;
  UINT16                SataPortsEnabled;
  UINT8                 Index;
  UINTN                 PciSataRegBase;
  UINT16                WordReg;
  UINT8                 PortSataControl;
  UINT32                CapRegister;
  PCH_SERIES            PchSeries;
  UINT8                 PortxEnabled;
  UINTN                 PortxEnabledOffset;
  UINT16                PortxEnabledMask;

  DEBUG ((DEBUG_INFO, "ConfigurePchSataAhci() Start\n"));


  DEBUG ((EFI_D_INFO,"SataDeviceNumber: %x\n",SataDeviceNumber));
  PciSataRegBase  = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, SataDeviceNumber, PCI_FUNCTION_NUMBER_PCH_SATA);


  ///
  /// Set the AHCI BAR
  ///
  AhciBar = AhciBarAddress;
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_AHCI_BAR, AhciBar);

  ///
  /// Enable command register memory space decoding
  ///
  MmioOr16 (PciSataRegBase + PCI_COMMAND_OFFSET, (UINT16) EFI_PCI_COMMAND_MEMORY_SPACE);

  ///
  /// Get Port Settings
  ///
  PchSeries = GetPchSeries ();
  if (PchSeries == PchH) {
    SataPortsEnabled    = MmioRead16 (PciSataRegBase + R_PCH_H_SATA_PCS);
    SataPortsEnabled    &= (UINT16) (B_PCH_H_SATA_PCS_PXE_MASK);
    PortxEnabled        = B_PCH_H_SATA_PCS_P0E;


  } else {
    SataPortsEnabled    = MmioRead16 (PciSataRegBase + R_PCH_LP_SATA_PCS);
    SataPortsEnabled    &= (UINT16) (B_PCH_LP_SATA_PCS_PXE_MASK);
    PortxEnabled        = B_PCH_LP_SATA_PCS_P0E;
    PortxEnabledOffset  = R_PCH_LP_SATA_PCS;
    PortxEnabledMask    = B_PCH_LP_SATA_PCS_PXE_MASK;
  }

  //
  // Read the default value of the Host Capabilites Register
  // NOTE: many of the bits in this register are R/WO (Read/Write Once)
  //
  CapRegister = MmioRead32 (AhciBar + R_PCH_SATA_AHCI_CAP);
  CapRegister &= (UINT32)~(B_PCH_SATA_AHCI_CAP_SIS | B_PCH_SATA_AHCI_CAP_SSS | B_PCH_SATA_AHCI_CAP_SALP |
                           B_PCH_SATA_AHCI_CAP_PMS | B_PCH_SATA_AHCI_CAP_SSC | B_PCH_SATA_AHCI_CAP_PSC |
                           B_PCH_SATA_AHCI_CAP_SXS | B_PCH_SATA_AHCI_CAP_SSNTF);

  for (Index = 0; Index < GetPchMaxSataPortNum (); Index++) {
    //
    // Check PCS.PxE to know if the SATA Port x is disabled.
    //
    if ((SataPortsEnabled & (PortxEnabled << Index)) == 0) {
      continue;
    }

    if (SataConfig->PortSettings[Index].InterlockSw == TRUE) {
      //
      // Mechanical Presence Switch is Enabled for at least one of the ports
      //
      CapRegister |= B_PCH_SATA_AHCI_CAP_SIS;
    }

    if ((SataConfig->PortSettings[Index].SpinUp == TRUE) ||
        (SataConfig->PortSettings[Index].External == TRUE)) {
      //
      // PCH BIOS Spec Section 14.1.4 Initialize Registers in AHCI Memory-Mapped Space
      // Step 1.4
      // Set SSS (ABAR + 00h[27]) to enable SATA controller supports staggered
      // spin-up on its ports, for use in balancing power spikes
      // SATA Port Spin up is supported at least one of the ports
      // If this is an extra eSATA port. It needs to be hotpluggable but it's usually empty anyway
      // so LPM is not available but Listen Mode is available, so it will have good power management.
      // If Sata Test Mode enabled, then uncoditonally clear SSS (ABAR + 00h[27])
      // to resolve Spin-donw issue with the test equiepment
      //
      if (SataConfig->TestMode == FALSE ) {
        CapRegister |= B_PCH_SATA_AHCI_CAP_SSS;
      }
    }

    if (SataConfig->PortSettings[Index].External == TRUE) {
      //
      // External SATA is supported at least one of the ports
      //
      CapRegister |= B_PCH_SATA_AHCI_CAP_SXS;
    }
  }


  //
  //Sata LPM and SALP configuration
  //
  if(SataConfig->LpmSupport == TRUE)
  {
    CapRegister |= (B_PCH_SATA_AHCI_CAP_SSC | B_PCH_SATA_AHCI_CAP_PSC);
    DEBUG ((EFI_D_INFO,"SATA: LPM enable\n"));
    if (SataConfig->SalpSupport == TRUE)
    {
      DEBUG ((EFI_D_INFO,"SATA: SALP enable\n"));
      CapRegister |= B_PCH_SATA_AHCI_CAP_SALP;
    }
    else
    {
      DEBUG ((EFI_D_INFO,"SATA: SALP disable\n"));
      CapRegister &= ~B_PCH_SATA_AHCI_CAP_SALP;
    }
  }
  else
  {
    DEBUG ((EFI_D_INFO,"SATA: LPM disable\n"));
    DEBUG ((EFI_D_INFO,"SATA: SALP disable\n"));
    CapRegister &= ~(B_PCH_SATA_AHCI_CAP_SSC | B_PCH_SATA_AHCI_CAP_PSC);
    CapRegister &= ~B_PCH_SATA_AHCI_CAP_SALP;
  }
  //
  // Support Command List Override & PIO Multiple DRQ Block
  //
  CapRegister |= (B_PCH_SATA_AHCI_CAP_SCLO | B_PCH_SATA_AHCI_CAP_PMD);

  //
  // Configure for the max speed support 1.5Gb/s, 3.0Gb/s and 6.0Gb/s.
  //
  CapRegister &= ~B_PCH_SATA_AHCI_CAP_ISS_MASK;

  switch (SataConfig->SpeedLimit) {
  case PchSataSpeedGen1:
    CapRegister |= (V_PCH_SATA_AHCI_CAP_ISS_1_5_G << N_PCH_SATA_AHCI_CAP_ISS);
    break;

  case PchSataSpeedGen2:
    CapRegister |= (V_PCH_SATA_AHCI_CAP_ISS_3_0_G << N_PCH_SATA_AHCI_CAP_ISS);
    break;

  case PchSataSpeedGen3:
  case PchSataSpeedDefault:
    CapRegister |= (V_PCH_SATA_AHCI_CAP_ISS_6_0_G << N_PCH_SATA_AHCI_CAP_ISS);
    break;
  }


  //
  //Configure Supports Port Multiplier (SPM)
  //
  if(SataConfig->PortMultiplier == TRUE)
  {
    CapRegister |= B_PCH_SATA_AHCI_CAP_PMS;
  }
  else
  {
    CapRegister &= ~B_PCH_SATA_AHCI_CAP_PMS;
  }
  //
  // Update the Host Capabilites Register
  // NOTE: Many of the bits in this register are R/WO (Read/Write Once)
  //
  MmioWrite32 (AhciBar + R_PCH_SATA_AHCI_CAP, CapRegister);

  ///
  /// Enable SATA ports by setting all PI bits to 1b
  /// If BIOS accesses any of the port specific AHCI address range before setting PI bit,
  /// BIOS is required to read the PI register before the initial write to the PI register.
  /// NOTE: The read before initial write to PI register is done by the MmioAndThenOr32 routine.
  /// NOTE: many of the bits in this register are R/WO (Read/Write Once)
  ///
  MmioOr32 (AhciBar + R_PCH_SATA_AHCI_PI, SataPortsEnabled);

  ///
  /// After BIOS issues initial write to this register, BIOS is requested to issue two
  /// reads to this register.
  ///
  Data32Or = MmioRead32 (AhciBar + R_PCH_SATA_AHCI_PI);
  Data32Or = MmioRead32 (AhciBar + R_PCH_SATA_AHCI_PI);

  ///
  /// Set ABAR + 24h[5] to 1b
  /// Set ABAR + 24h[4:2] to 111b
  ///
  Data32Or = 0;
  MmioOr32 (AhciBar + R_PCH_SATA_AHCI_CAP2, Data32Or);

  ///
  /// Program all PCS "Port X Enabled" to all 0b,
  /// then program them again to all 1b in order to trigger COMRESET
  ///
  MmioAnd16 (PciSataRegBase + R_PCH_H_SATA_PCS, (UINT16) ~B_PCH_H_SATA_PCS_PXE_MASK);
  MmioOr16 (PciSataRegBase + R_PCH_H_SATA_PCS, (UINT16) SataPortsEnabled);
  ///
  /// At any time that BIOS or software is clearing PCS.PxE from 1 to 0, due to time needed for port
  /// staggering hardware process (up to 6 ports) to complete, BIOS and software shall delay the write
  /// to set the TM.PCD register by 1.4us
  ///
  MicroSecondDelay (0x02);
  ///
  /// Set Sata Port Clock Disable bits, SATA PCI offset 90h[7:0] for disabled ports
  ///
  MmioAnd16 (PciSataRegBase + R_PCH_H_SATA_MAP, (UINT16) (~B_PCH_H_SATA_MAP_PCD));
  MmioOr16 (PciSataRegBase + R_PCH_H_SATA_MAP, (UINT16) (~SataPortsEnabled));

  ///
  /// Port[Max:0] Command Register update
  /// NOTE: this register must be updated after Port Implemented and Capabilities register,
  /// Many of the bits in this register are R/WO (Read/Write Once)
  ///
  for (Index = 0; Index < GetPchMaxSataPortNum (); Index++) {
    ///
    /// Check PCS.PxE to know if the SATA Port x is disabled.
    ///
    if ((SataPortsEnabled & (PortxEnabled << Index)) == 0) {
      continue;
    }
    ///
    /// Initial to zero first
    ///
    PxCMDRegister = 0;

    if (SataConfig->PortSettings[Index].HotPlug == TRUE) {
      if (SataConfig->PortSettings[Index].External == FALSE) {
        ///
        /// Hot Plug of this port is enabled
        ///
        PxCMDRegister |= B_PCH_SATA_AHCI_PxCMD_HPCP;
        if (SataConfig->PortSettings[Index].InterlockSw == TRUE) {
          ///
          /// Mechanical Switch of this port is Attached
          ///
          PxCMDRegister |= B_PCH_SATA_AHCI_PxCMD_MPSP;

          ///
          /// Check the GPIO Pin is set as used for native function not a normal GPIO
          ///
          if (GpioIsSataResetPortInGpioMode (Index)) {
            DEBUG ((DEBUG_ERROR,
                    "BIOS must set the SATA%0xGP GPIO pin to native function if Inter Lock Switch is enabled!\n",
                    Index));
            ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
          }

        }
      }
    } else {
      ///
      /// When "Mechanical Switch Attached to Port" (PxCMD[19]) is set, it is expected that HPCP (PxCMD[18]) is also set.
      ///
      if (SataConfig->PortSettings[Index].InterlockSw == TRUE) {
        DEBUG ((DEBUG_ERROR, "Hot-Plug function of Port%d should be enabled while the Inter Lock Switch of it is enabled!\n",
        Index));
      }
    }

    if (SataConfig->PortSettings[Index].External == TRUE) {
      PxCMDRegister |= B_PCH_SATA_AHCI_PxCMD_ESP;
    }

    #ifdef PCH_PO_FLAG
    if (SataSafeRegister) {
      ///
      /// Leave as default
      ///
    } else {
    #endif // PCH_PO_FLAG
    ///
    /// Set ALPE
    ///
    PxCMDRegister |= (B_PCH_SATA_AHCI_PxCMD_ALPE);
    #ifdef PCH_PO_FLAG
    }
    #endif // PCH_PO_FLAG

    ///
    /// If the SATA controller supports staggerred spin-up (SSS in AHCI_CAP is set to 1b),
    /// then set the SUD bit in Port[Max:0] Command Register to 1b
    ///
    if ((CapRegister & B_PCH_SATA_AHCI_CAP_SSS) != 0) {
      PxCMDRegister |= B_PCH_SATA_AHCI_PxCMD_SUD;
    }

    MmioAndThenOr32 (
      AhciBar + (R_PCH_SATA_AHCI_P0CMD + (0x80 * Index)),
      (UINT32)~(B_PCH_SATA_AHCI_PxCMD_MASK),
      (UINT32) PxCMDRegister
      );

    ///
    /// DevSleep programming
    /// Set ABAR + 144h[1], ABAR + 1C4h[1], ABAR + 244h[1], ABAR + 2C4[1] to 0b as default
    /// Board rework is required to enable DevSlp.
    /// POR settings are ABAR + 144h[1], ABAR + 1C4h[1], ABAR + 244h[1] = 1b, ABAR + 2C4[1] to 0b
    ///
    if (SataConfig->PortSettings[Index].DevSlp == TRUE) {
      Data32And = (UINT32)~(B_PCH_SATA_AHCI_PxDEVSLP_DITO_MASK | B_PCH_SATA_AHCI_PxDEVSLP_DM_MASK);
      Data32Or  = (B_PCH_SATA_AHCI_PxDEVSLP_DSP | V_PCH_SATA_AHCI_PxDEVSLP_DM_16 | V_PCH_SATA_AHCI_PxDEVSLP_DITO_625);

      if (SataConfig->PortSettings[Index].EnableDitoConfig == TRUE) {
        Data32Or &= Data32And;
        Data32Or |= ((SataConfig->PortSettings[Index].DitoVal << 15) | (SataConfig->PortSettings[Index].DmVal << 25));
      }

      MmioAndThenOr32 (
        AhciBar + (R_PCH_SATA_AHCI_P0DEVSLP + (0x80 * Index)),
        Data32And,
        Data32Or
        );
    } else {
      MmioAnd32 (
        AhciBar + (R_PCH_SATA_AHCI_P0DEVSLP + (0x80 * Index)),
        (UINT32) ~(B_PCH_SATA_AHCI_PxDEVSLP_DSP)
        );
    }

    //
    // eSATA port support only up to Gen2.
    // When enabled, BIOS will configure the PxSCTL.SPD to 2 to limit the eSATA port speed.
    // Please be noted, this setting could be cleared by HBA reset, which might be issued
    // by EFI AHCI driver when POST time, or by SATA inbox driver/RST driver after POST.
    // To support the Speed Limitation when POST, the EFI AHCI driver should preserve the
    // setting before and after initialization. For support it after POST, it's dependent on
    // driver's behavior.
    //
    if ((SataConfig->eSATASpeedLimit == TRUE) &&
        (SataConfig->PortSettings[Index].External == TRUE)) {
      PortSataControl = MmioRead8 (AhciBar + (R_PCH_SATA_AHCI_P0SCTL + (0x80 * Index)));
      PortSataControl &= (UINT8) ~(B_PCH_SATA_AHCI_PXSCTL_SPD);
      PortSataControl |= (UINT8) V_PCH_SATA_AHCI_PXSCTL_SPD_2;
      MmioWrite8 (AhciBar + (R_PCH_SATA_AHCI_P0SCTL + (0x80 * Index)), PortSataControl);
    }
  }

  /// PCH BIOS Spec Section 14.1.5.2 Enable Flexible RAID OROM Features
  /// Lynx Point with RAID capable sku is able to customize the RAID features through setting the
  /// Intel RST Feature Capabilities (RSTF) register before loading the RAID Option ROM. The RAID
  /// OROM will enable the desired features based on the setting in that register, please refer to
  /// PCH EDS for more details.
  /// For example, if the platform desired features are RAID0, RAID1, RAID5, RAID10 and
  /// RST Smart Storage caching. System BIOS should set RSTF (ABAR + C8h [15:0]) to 022Fh before
  /// loading RAID OROM.
  ///
  WordReg = 0;

  if (SataConfig->Rst.HddUnlock == TRUE) {
    ///
    /// If set to "1", indicates that the HDD password unlock in the OS is enabled
    /// while SATA is configured as RAID mode.
    ///
    WordReg |= B_PCH_SATA_AHCI_RSTF_HDDLK;
  }

  if (SataConfig->Rst.LedLocate == TRUE) {
    ///
    /// If set to "1", indicates that the LED/SGPIO hardware is attached and ping to
    /// locate feature is enabled on the OS while SATA is configured as RAID mode.
    ///
    WordReg |= B_PCH_SATA_AHCI_RSTF_LEDL;
  }

  if (SataConfig->SataMode == PchSataModeRaid) {
    if (SataConfig->Rst.Raid0 == TRUE) {
      ///
      /// If set to "1", then RAID0 is enabled in Flexible RAID Option ROM.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_R0E;
    }

    if (SataConfig->Rst.Raid1 == TRUE) {
      ///
      /// If set to "1", then RAID1 is enabled in FD-OROM.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_R1E;
    }

    if (SataConfig->Rst.Raid10 == TRUE) {
      ///
      /// If set to "1", then RAID10 is enabled in FD-OROM.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_R10E;
    }

    if (SataConfig->Rst.Raid5 == TRUE) {
      ///
      /// If set to "1", then RAID5 is enabled in FD-OROM.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_R5E;
    }

    if (SataConfig->Rst.Irrt == TRUE) {
      ///
      /// If set to "1", then Intel Rapid Recovery Technology is enabled.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_RSTE;
    }

    if (SataConfig->Rst.OromUiBanner == TRUE) {
      ///
      /// If set to "1" then the OROM UI is shown.  Otherwise, no OROM banner or information
      /// will be displayed if all disks and RAID volumes are Normal.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_IRSTOROM;
    }

    if (SataConfig->Rst.IrrtOnly == TRUE) {
      ///
      /// If set to "1", then only IRRT volumes can span internal and eSATA drives. If cleared
      /// to "0", then any RAID volume can span internal and eSATA drives.
      ///
      WordReg |= B_PCH_SATA_AHCI_RSTF_IROES;
    }
    ///
    /// Enable the RST Smart Storage caching bit to support Smart Storage caching.
    ///
    if (SataConfig->Rst.SmartStorage == TRUE) {
      WordReg |= B_PCH_SATA_AHCI_RSTF_SEREQ;
    }
    ///
    /// Program the delay of the OROM UI Splash Screen in a normal status.
    ///
    WordReg |= (UINT16) (SataConfig->Rst.OromUiDelay << N_PCH_SATA_AHCI_RSTF_OUD);
  }

  ///
  /// RSTF(RST Feature Capabilities) is a Write-Once register.
  ///
  MmioWrite16 (AhciBar + R_PCH_SATA_AHCI_RSTF, WordReg);

  ///
  /// Disable command register memory space decoding
  ///
  MmioAnd16 (PciSataRegBase + PCI_COMMAND_OFFSET, (UINT16) ~EFI_PCI_COMMAND_MEMORY_SPACE);

  ///
  /// Set Ahci Bar to zero
  ///
  AhciBar = 0;
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_AHCI_BAR, AhciBar);

  ///
  /// if in test mode enable, perform misc programming for test mode
  ///
  if (SataConfig->TestMode == TRUE) {
    ProgramSataTestMode();
  }

  DEBUG ((DEBUG_INFO, "ConfigurePchSataAhci() End\n"));
  return EFI_SUCCESS;
}


/**
  Disable Sata Controller for PCH-H

  @param[in] AhciBarAddress       The Base Address for AHCI BAR

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
DisablePchHSataController (
  IN UINT32                             AhciBarAddress
  ,IN UINT8                              SataDeviceNumber
  )
{
  UINTN                                 PciSataRegBase;
  UINT32                                AhciBar;
  UINT32                                Data32And;
  UINT32                                Data32Or;
  UINT32                                PchPwrmBase;
  UINTN                                 Index;

  EFI_STATUS Status;
  PciSataRegBase  = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, SataDeviceNumber, PCI_FUNCTION_NUMBER_PCH_SATA);

  DEBUG ((DEBUG_INFO, "DisablePchHSataController: DisablePchHSataController() Started\n"));
  DEBUG ((DEBUG_INFO, "Sata Controller Device Number: 0x%x\n", SataDeviceNumber));

  ///
  /// Set the AHCI BAR
  ///
  AhciBar = AhciBarAddress;
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_AHCI_BAR, AhciBar);

  ///
  /// Enable command register memory space decoding
  ///
  MmioOr16 (PciSataRegBase + PCI_COMMAND_OFFSET, (UINT16) EFI_PCI_COMMAND_MEMORY_SPACE);

  ///
  /// PCH BIOS Spec Section 14.3 SATA Controller Disabling
  /// Step 1 - Disable all ports
  /// Set SATA PCI offset 94h[7:0] to all 0b
  ///
  MmioAnd16 (PciSataRegBase + R_PCH_H_SATA_PCS, (UINT16) ~(B_PCH_H_SATA_PCS_PXE_MASK));

  ///
  /// Step 2 - Disable all ports
  /// Clear PI register, ABAR + 0Ch
  ///
  Data32And = (UINT32) (~B_PCH_H_SATA_PORT_MASK);
  MmioAnd32 (AhciBar + R_PCH_SATA_AHCI_PI, Data32And);

  ///
  /// After BIOS issues initial write to this register, BIOS is requested to
  /// issue two reads to this register.
  ///
  Data32Or = MmioRead32 (AhciBar + R_PCH_SATA_AHCI_PI);
  Data32Or = MmioRead32 (AhciBar + R_PCH_SATA_AHCI_PI);

  ///
  /// Step 3
  /// Clear MSE and IOE, SATA PCI offset 4h[1:0] = 00b
  ///
  MmioAnd16 (PciSataRegBase + PCI_COMMAND_OFFSET, (UINT16) ~(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_IO_SPACE));

  ///
  /// Step 4
  /// Set Sata Port Clock Disable bits, SATA PCI offset 90h[7:0] to all 1b
  ///
  MmioOr32 (PciSataRegBase + R_PCH_H_SATA_MAP, (UINT32) B_PCH_H_SATA_MAP_PCD);

  ///
  /// Step 5 Enable all SATA dynamic clock gating and dynamic power gating features:
  ///  a. SATA SIR offset 9Ch bit[29, 23] = [1, 1]
  ///  b. SATA SIR offset 8Ch bit[23:16, 7:0] to all 1b
  ///  c. SATA SIR offset A0h bit[15] to [1]
  ///  d. SATA SIR offset 84h bit[23:16] to all 1b
  ///  e. SATA SIR offset A4h bit[26, 25, 24, 14, 5] = [0, 0, 0, 1, 1]
  ///  f. SATA SIR offset A8h bit[17, 16] = [1, 1]
  ///
  /// Step 5a
  /// SATA SIR offset 9Ch bit[29, 23] = [1, 1]
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_9C);
  MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, (UINT32) (BIT29 | BIT23));

  ///
  /// Step 5b
  /// SATA SIR offset 8Ch bit[23:16, 7:0] to all 1b
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_8C);
  MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00FF00FF);

  ///
  /// Step 5c
  /// SATA SIR offset A0h bit[15] to [1]
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A0);
  MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, (UINT32) (BIT15));

  ///
  /// Step 5d
  /// SATA SIR offset 84h bit[23:16] to all 1b
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_84);
  MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00FF0000);

  ///
  /// Step 5e
  /// SATA SIR offset A4h bit[26, 25, 24, 14, 5] = [0, 0, 0, 1, 1]
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A4);
  Data32And = (UINT32)~(BIT26 | BIT25 | BIT24 | BIT14 | BIT5);
  Data32Or  = (UINT32) (BIT14 | BIT5);
  MmioAndThenOr32 (
    PciSataRegBase + R_PCH_SATA_STRD,
    Data32And,
    Data32Or
    );

  ///
  /// Step 5f
  /// SATA SIR offset A8h bit[17, 16] = [1, 1]
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A8);
  Data32And = (UINT32)~(BIT17 | BIT16);
  Data32Or  = (UINT32) (BIT17 | BIT16);
  MmioAndThenOr32 (
    PciSataRegBase + R_PCH_SATA_STRD,
    Data32And,
    Data32Or
    );
  ///
  /// Step 5g For each RST PCIe Storage Cycle Router, program the "Extended General Configuration Register" field,
  ///
  for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
    MmioWrite8 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_CRGC, (UINT8) Index);
    MmioOr32 (PciSataRegBase + R_PCH_RST_PCIE_STORAGE_EGCR,(UINT32) (B_PCH_RST_PCIE_STORAGE_EGCR_CRDCGE | B_PCH_RST_PCIE_STORAGE_EGCR_CRTCGE | B_PCH_RST_PCIE_STORAGE_EGCR_ICAS | B_PCH_RST_PCIE_STORAGE_EGCR_TSCAS));
  }
  ///
  /// Step 6
  /// Disabling SATA Device by programming SATA SCFD, SATA PCI offset 9Ch[10] = 1
  ///
  MmioOr32 (PciSataRegBase + R_PCH_SATA_SATAGC, (UINT32) (BIT10));

  ///
  /// Step 7
  /// Set PCR[PSF1] AUD PCIEN[8] to 1
  ///
  Data32And = (UINT32)~0;
  Data32Or  = B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS;
  if (SataDeviceNumber == PCI_DEVICE_NUMBER_PCH_SATA) {
    PchPcrAndThenOr32 (
      PID_PSF2, R_PCH_PCR_PSF2_T0_SHDW_SATA0_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
      Data32And,
      Data32Or
    );
  } else {
    PchPcrAndThenOr32 (
      PID_PSF2, R_PCH_PCR_PSF2_T0_SHDW_SATA1_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
      Data32And,
      Data32Or
    );
  }

  ///
  /// Set Ahci Bar to zero (Note: MSE and IO has been disabled)
  ///
  AhciBar = 0;
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_AHCI_BAR, AhciBar);

  ///
  /// Set PWRMBASE + 0x628 [22] = 1b to disable SATA Controller in PMC
  /// No need to clear this non-static PG bit while IP re-enabled since it's reset to 0 when PLTRST.
  ///
  PchPwrmBaseGet (&PchPwrmBase);

  if(SataDeviceNumber == PCI_DEVICE_NUMBER_PCH_SATA)
  {
    Status = PmcStPgDisableFunction(PMC_ST_PG_SATA0);
  }else
  if(SataDeviceNumber == PCI_DEVICE_NUMBER_PCH_SATA2)
  {
    Status = PmcStPgDisableFunction(PMC_ST_PG_SATA1);
  }else
  {
    Status = EFI_INVALID_PARAMETER;
  }
  ASSERT_EFI_ERROR(Status);


  DEBUG ((DEBUG_INFO, "DisablePchHSataController: DisablePchHSataController() Ended\n"));

  return EFI_SUCCESS;
}


/**
  Configures Sata Controller for PCH-H

  @param[in]      SataConfig                The PCH Policy for SATA configuration
  @param[in] RstPcieStorageRemapEnabled     A Boolean to indicate if RST PCIe Storage Remapping is enabled
  @param[in] AhciBarAddress                 The Base Address for AHCI BAR

  @retval EFI_SUCCESS                       The function completed successfully
**/
EFI_STATUS
ConfigurePchHSata (
#ifdef PCH_PO_FLAG
  IN  CONST UINT32                            SataSafeRegister,
#endif // PCH_PO_FLAG
  IN  CONST PCH_SATA_CONFIG                   *SataConfig,
  IN UINT8                                    SataDeviceNumber,
  IN BOOLEAN                                  RstPcieStorageRemapEnabled,
  IN UINT32                                   AhciBarAddress
  )
{
  EFI_STATUS              Status;
  UINT16                  SataGcReg;
  UINTN                   PciSataRegBase;
  UINTN                   PciLpcRegBase;
  UINT16                  LpcDeviceId;
  UINT32                  Data32And;
  UINT32                  Data32Or;
  UINTN                   MaxSataPorts;

  UINT16                  SataPortsEnabled;
  UINT8                   Index;


  DEBUG ((DEBUG_INFO, "ConfigurePchHSata() Start\n"));
  MaxSataPorts = GetPchMaxSataPortNum ();
  DEBUG ((EFI_D_INFO,"SataDeviceNumber: 0x%x\n",SataDeviceNumber));
  PciSataRegBase    = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, SataDeviceNumber, PCI_FUNCTION_NUMBER_PCH_SATA);
  PciLpcRegBase     = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC);
  LpcDeviceId       = MmioRead16 (PciLpcRegBase + PCI_DEVICE_ID_OFFSET);
  Status          = EFI_SUCCESS;

  ///
  /// If Sata is disabled or no lanes assigned to SATA when in AHCI mode
  /// perform the disabling steps to function disable the SATA Controller
  ///

  if (SataConfig->Enable == FALSE){
    DisablePchHSataController (AhciBarAddress, SataDeviceNumber);
  }
  else
  {
    //
    // Set message to enable SATA in PMC.
    //
    DEBUG ((EFI_D_INFO,"Setting PMC message to enable controller 0x%x\n",SataDeviceNumber));
    if (SataDeviceNumber == PCI_DEVICE_NUMBER_PCH_SATA)
    {
      Status = PmcStPgEnableFunction(PMC_ST_PG_SATA0);
    }
    else if (SataDeviceNumber == PCI_DEVICE_NUMBER_PCH_SATA2)
    {
      Status = PmcStPgEnableFunction(PMC_ST_PG_SATA1);
    }
    else
    {
      Status = EFI_INVALID_PARAMETER;
    }
    ASSERT_EFI_ERROR(Status);
  }


  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_98);
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_STRD, 0x8183);

  //Set MSI-X  to SATACR0.Next
  MmioWrite32(PciSataRegBase + R_PCH_SATA_CR0, (MmioRead32(PciSataRegBase + R_PCH_SATA_CR0) & ~B_PCH_SATA_CR0_NEXT) | ( V_PCH_SATA_CR0_NEXT_D0 << N_PCH_SATA_CR0_NEXT));


  ///
  /// Step 2
  /// Set SATA SIR Index A4h = 0x190065
  /// These bits should be restored while resuming from a S3 sleep state
  ///
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A4);
  MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, 0x190065);

  ///
  /// Step 3
  /// System BIOS must program SATA Hsio table as stated in Table 7-7 to 7-8 BEFORE the SATA
  /// ports are enabled.
  ///
MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_9C);
MmioWrite32 (PciSataRegBase + R_PCH_SATA_STRD, 0x209C0224);
  ///
  /// Step 5
  /// Program SATA PCI offset 9Ch[5] to 1b
  /// Note: Bits 7:0 are RWO, perform byte write here, program the 9Ch[31] to 1b later in PchOnEndOfDxe ()
  ///
  SataGcReg = MmioRead16 (PciSataRegBase + R_PCH_SATA_SATAGC);
  SataGcReg |= BIT5;


  ///
  /// If RstPcieStorageRemapEnabled is TRUE, program Sata PCI offset 9Ch, bit[4:3] and bit[2:0] to [00'b] and [110'b]
  /// else, program Sata PCI offset 9Ch, bit[4:3] and bit[2:0] to [10'b] and [000'b]
  ///
  if (RstPcieStorageRemapEnabled) {
    SataGcReg |= V_PCH_SATA_SATAGC_ASSEL_512K;
    SataGcReg &= ~(B_PCH_SATA_SATAGC_MSS);
  } else {
    SataGcReg &= ~B_PCH_SATA_SATAGC_ASSEL;
    SataGcReg |= V_PCH_SATA_SATAGC_MSS_8K << N_PCH_SATA_SATAGC_MSS;
  }


  //
  //Enable error bits
  //
  SataGcReg |= B_PCH_SATA_SATAGC_DPPEE | B_PCH_SATA_SATAGC_CPEE | B_PCH_SATA_SATAGC_URRE;

  //
  //Requested as safe value of emulation
  //
  SataGcReg |= V_PCH_SATA_SATAGC_WRRSELMPS_64K;
  SataGcReg |= B_PCH_SATA_SATAGC_REGLOCK;
  ///
  /// Unconditional write is necessary to lock the register
  ///
  MmioWrite16 (PciSataRegBase + R_PCH_SATA_SATAGC, SataGcReg);

  ///
  /// Program PCS "Port X Enabled", SATA PCI offset 94h[7:0] to all 1b.
  ///

  SataPortsEnabled  = 0;
  for (Index = 0; Index < MaxSataPorts; Index++) {
    SataPortsEnabled |= (SataConfig->PortSettings[Index].Enable << Index);
  }
  MmioAndThenOr16(PciSataRegBase + R_PCH_H_SATA_PCS,(UINT16)~B_PCH_H_SATA_PCS_PXE_MASK, SataPortsEnabled);


  ///
  /// PCH BIOS Spec section 19.10
  /// Step 4.2
  /// After configuring Port and Control Status (PCS) Register Port x Enabled (PxE) bits accordingly,
  /// wait 1.4 micro second
  ///
  MicroSecondDelay (0x02);

  ///
  /// Set Sata Port Clock Disable bits, SATA PCI offset 90h[7:0] for disabled ports
  ///
  MmioAnd16 (PciSataRegBase + R_PCH_H_SATA_MAP, (UINT16) (~B_PCH_H_SATA_MAP_PCD));
  MmioOr16 (PciSataRegBase + R_PCH_H_SATA_MAP, (UINT16) (~SataPortsEnabled));

  ///
  /// Configure AHCI
  ///
  Status = ConfigurePchSataAhci (
             #ifdef PCH_PO_FLAG
             SataSafeRegister,
             #endif // PCH_PO_FLAG
             SataConfig,
             AhciBarAddress
            ,SataDeviceNumber
             );


  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A8);
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00000020);


  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_A0);
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00588000);

  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_80);
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00000000);

  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_84);
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00FF00FF);

  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_88);
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00);

  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_8C);
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_STRD, 0x00FF00FF);

  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_90);
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_STRD, 0xFF);

  Data32And = (UINT32)~(BIT2);
  Data32Or  = (UINT32)(BIT1 | BIT3);
  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_C8);
  MmioAndThenOr32 (PciSataRegBase + R_PCH_SATA_STRD, Data32And, Data32Or);

  MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_D4);
  MmioWrite32 (PciSataRegBase + R_PCH_SATA_STRD, 0x2C1E1108);
  ///
  /// Initialize the SATA mode to be in AHCI, then check the SATA mode from the policy
  /// If RAID mode is selected and the SKU supports RAID feature, set the SATA Mode Select to 1b (RAID)
  ///
  MmioAnd8 (PciSataRegBase + R_PCH_SATA_SATAGC + 2,(UINT8) ~(BIT0));

  if (SataConfig->SataMode == PchSataModeRaid) {
    if (IS_PCH_RAID_AVAILABLE (LpcDeviceId)) {
      MmioAndThenOr8 (
        PciSataRegBase + R_PCH_SATA_SATAGC + 2,
        (UINT8) ~(BIT0),
        (UINT8) (V_PCH_H_SATA_SATAGC_SMS_RAID)
        );
    } else {
      DEBUG ((DEBUG_INFO, "PCH Device ID : 0x%x\n", LpcDeviceId));
      DEBUG ((DEBUG_ERROR, "This SKU doesn't support RAID feature. Set to AHCI mode.\n"));
    }
  }

#ifdef FSP_FLAG
  //
  // If FSP build is enabled, don't perform dynamic port disable
  // Set the Sata Port Enable/Disable based on policy only
  //
  SataPortsEnabled  = 0;
  for (Index = 0; Index < MaxSataPorts; Index++) {
    SataPortsEnabled |= (SataConfig->PortSettings[Index].Enable << Index);
  }

  //
  // Set MAP."Sata PortX Disable", SATA PCI offset 90h[23:16] to 1b if SATA Port 0/1/2/3/4/5/6/7 is disabled
  //
  MmioOr32 (PciSataRegBase + R_PCH_H_SATA_MAP, ((~SataPortsEnabled << N_PCH_H_SATA_MAP_SPD) & (UINT32) B_PCH_H_SATA_MAP_SPD));

  //
  // Program PCS "Port X Enabled", SATA PCI offset 94h[7:0] = Port 0~7 Enabled bit as per SataPortsEnabled value.
  //
  MmioOr16 (PciSataRegBase + R_PCH_H_SATA_PCS, SataPortsEnabled);

  //
  // Program SATA PCI offset 9Ch [31] to 1b
  //
  MmioOr32 ((UINTN) (PciSataRegBase + R_PCH_SATA_SATAGC), BIT31);
#endif // FSP_FLAG

  DEBUG ((DEBUG_INFO, "ConfigurePchHSata() End\n"));

  return Status;
}

