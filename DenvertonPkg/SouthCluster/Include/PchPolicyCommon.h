/** @file
  PCH configuration based on PCH policy

@copyright
  Copyright (c) 2009 - 2017 Intel Corporation. All rights reserved
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
#ifndef _PCH_POLICY_COMMON_H_
#define _PCH_POLICY_COMMON_H_

#include "PchLimits.h"

#pragma pack (push,1)
//
// ---------------------------- PCH General Config -------------------------------
//

typedef struct {
  /**
    Subsystem Vendor ID and Subsystem ID of the PCH devices.
    This fields will be ignored if the value of SubSystemVendorId and SubSystemId
    are both 0.
  **/
  UINT16    SubSystemVendorId;          ///< Default Subsystem Vendor ID of the PCH devices. Default is <b>0x8086</b>
  UINT16    SubSystemId;                ///< Default Subsystem ID of the PCH devices. Default is <b>0x7270</b>
  /**
    This member describes whether or not the Compatibility Revision ID (CRID) feature
    of PCH should be enabled. <b>0: Disable</b>; 1: Enable
  **/
  UINT32    Crid            :  1;
  UINT32    RsvdBits0       : 31;       ///< Reserved bits
  UINT32    Rsvd0[2];                   ///< Reserved bytes
  UINT32    DebugLevel;						///< Debug message level
} PCH_GENERAL_CONFIG;


#define FORCE_ENABLE  1
#define FORCE_DISABLE 2
#define PLATFORM_POR  0
//
// ---------------------------- Reserved Page Config -----------------------------
//

enum PCH_RESERVED_PAGE_ROUTE {
  PchReservedPageToLpc,     ///< Port 80h cycles are sent to LPC.
  PchReservedPageToPcie     ///< Port 80h cycles are sent to PCIe.
};

//
// ---------------------------- PCI Express Config ----------------------
//

enum PCH_PCIE_SPEED {
  PchPcieAuto,
  PchPcieGen1,
  PchPcieGen2,
  PchPcieGen3
};

///
/// The values before AutoConfig match the setting of PCI Express Base Specification 1.1, please be careful for adding new feature
///
typedef enum  {
  PchPcieAspmDisabled,
  PchPcieAspmL0s,
  PchPcieAspmL1,
  PchPcieAspmL0sL1,
  PchPcieAspmAutoConfig,
  PchPcieAspmMax
} PCH_PCIE_ASPM_CONTROL;

/**
  Refer to PCH EDS for the PCH implementation values corresponding
  to below PCI-E spec defined ranges
**/
typedef enum {
  PchPcieL1SubstatesDisabled,
  PchPcieL1SubstatesL1_1,
  PchPcieL1SubstatesL1_2,
  PchPcieL1SubstatesL1_1_2,
  PchPcieL1SubstatesMax
} PCH_PCIE_L1SUBSTATES_CONTROL;

enum PCH_PCIE_COMPLETION_TIMEOUT {
  PchPcieCompletionTO_Default,
  PchPcieCompletionTO_50_100us,
  PchPcieCompletionTO_1_10ms,
  PchPcieCompletionTO_16_55ms,
  PchPcieCompletionTO_65_210ms,
  PchPcieCompletionTO_260_900ms,
  PchPcieCompletionTO_1_3P5s,
  PchPcieCompletionTO_4_13s,
  PchPcieCompletionTO_17_64s,
  PchPcieCompletionTO_Disabled
};

typedef enum {
  PchPcieEqDefault      = 0,  ///< Use reference code default (software margining)
  PchPcieEqHardware     = 1,  ///< Hardware equalization (experimental), note this requires PCH-LP C0 or PCH-H D0 or newer
  PchPcieEqSoftware     = 2,  ///< Use software margining flow
  PchPcieEqStaticCoeff  = 4   ///< Fixed equalization (requires Coefficient settings per lane)
} PCH_PCIE_EQ_METHOD;

/**
  Represent lane specific PCIe Gen3 equalization parameters.
**/
typedef struct {
  UINT8   Cm;                 ///< Coefficient C-1
  UINT8   Cp;                 ///< Coefficient C+1
  UINT8   Rsvd0[2];           ///< Reserved bytes
} PCH_PCIE_EQ_LANE_PARAM, PCH_PCIE_EQ_PARAM;

typedef struct {
  UINT8	  MaxPayload;
  UINT8   MaxReadRequest;
  UINT8	  ExtTagField;
  UINT8	  RelaxedOrdering;
  UINT8   EnableSLD;
}PCIE_DEVICE_LINK_CHAIN;
/**
  The PCH_PCI_EXPRESS_ROOT_PORT_CONFIG describe the feature and capability of each PCH PCIe root port.
**/
typedef struct {

  UINT32  HsioRxSetCtleEnable : 1;                ///< @deprecated, please use HsioRxSetCtleEnable from PCH_HSIO_PCIE_LANE_CONFIG
  UINT32  HsioRxSetCtle       : 6;                ///< @deprecated, please use HsioRxSetCtle from PCH_HSIO_PCIE_LANE_CONFIG
  //
  // Gen3 Equaliztion settings
  //
  UINT32  Uptp                : 4;                ///< <b>(Test)</b> Upstream Port Transmiter Preset used during Gen3 Link Equalization. Used for all lanes.  Default is <b>5</b>.
  UINT32  Dptp                : 4;                ///< <b>(Test)</b> Downstream Port Transmiter Preset used during Gen3 Link Equalization. Used for all lanes.  Default is <b>7</b>.
  UINT32  RsvdBits3           : 3;               ///< Reserved Bits
  UINT32  Rsvd2[16];                              ///< Reserved bytes
} PCH_PCIE_ROOT_PORT_CONFIG;

///
/// The PCH_PCIE_CONFIG block describes the expected configuration of the PCH PCI Express controllers
///
typedef struct {

  /// These members describe the configuration of each PCH PCIe root port.
  ///
  PCH_PCIE_ROOT_PORT_CONFIG         RootPort[PCH_MAX_PCIE_ROOT_PORTS];
  /**
    Compliance Test Mode shall be enabled when using Compliance Load Board.
    <b>0: Disable</b>, 1: Enable
  **/
  UINT32  ComplianceTestMode               :  1;
} PCH_PCIE_CONFIG;


#define PCH_PCIE_SWEQ_COEFFS_MAX    5

///
/// The PCH_PCIE_CONFIG2 block describes the additional configuration of the PCH PCI Express controllers
///
typedef struct {
  PCH_PCIE_EQ_PARAM  SwEqCoeffList[PCH_PCIE_SWEQ_COEFFS_MAX];          ///< List of coefficients used during software equalization
  PCH_PCIE_EQ_PARAM  Rsvd0[3];
  UINT32             Rsvd1[4];
} PCH_PCIE_CONFIG2;


/**
  The PCH_HSIO_PCIE_LANE_CONFIG describes HSIO settings for PCIe lane
**/
typedef struct {
  //
  // HSIO Rx Eq
  // Refer to the EDS for recommended values.
  // Note that these setting are per-lane and not per-port
  //
  UINT32  HsioRxSetCtleEnable           : 1;      ///< <b>0: Disable</b>; 1: Enable PCH PCIe Gen 3 Set CTLE Value
  UINT32  HsioRxSetCtle                 : 6;      ///< PCH PCIe Gen 3 Set CTLE Value
  UINT32  HsioTxGen1DownscaleAmpEnable  : 1;      ///< <b>0: Disable</b>; 1: Enable PCH PCIe Gen 1 TX Output Downscale Amplitude Adjustment value override
  UINT32  HsioTxGen1DownscaleAmp        : 6;      ///< PCH PCIe Gen 1 TX Output Downscale Amplitude Adjustment value
  UINT32  HsioTxGen2DownscaleAmpEnable  : 1;      ///< <b>0: Disable</b>; 1: Enable PCH PCIe Gen 2 TX Output Downscale Amplitude Adjustment value override
  UINT32  HsioTxGen2DownscaleAmp        : 6;      ///< PCH PCIe Gen 2 TX Output Downscale Amplitude Adjustment value
  UINT32  HsioTxGen3DownscaleAmpEnable  : 1;      ///< <b>0: Disable</b>; 1: Enable PCH PCIe Gen 3 TX Output Downscale Amplitude Adjustment value override
  UINT32  HsioTxGen3DownscaleAmp        : 6;      ///< PCH PCIe Gen 3 TX Output Downscale Amplitude Adjustment value
  UINT32  RsvdBits0                     : 4;      ///< Reserved Bits

  UINT32  HsioTxGen1DeEmphEnable        : 1;      ///< <b>0: Disable</b>; 1: Enable PCH PCIe Gen 1 TX Output De-Emphasis Adjustment Setting value override
  UINT32  HsioTxGen1DeEmph              : 6;      ///< PCH PCIe Gen 1 TX Output De-Emphasis Adjustment Setting
  UINT32  HsioTxGen2DeEmph3p5Enable     : 1;      ///< <b>0: Disable</b>; 1: Enable PCH PCIe Gen 2 TX Output -3.5dB Mode De-Emphasis Adjustment Setting value override
  UINT32  HsioTxGen2DeEmph3p5           : 6;      ///< PCH PCIe Gen 2 TX Output -3.5dB Mode De-Emphasis Adjustment Setting
  UINT32  HsioTxGen2DeEmph6p0Enable     : 1;      ///< <b>0: Disable</b>; 1: Enable PCH PCIe Gen 2 TX Output -6.0dB Mode De-Emphasis Adjustment Setting value override
  UINT32  HsioTxGen2DeEmph6p0           : 6;      ///< PCH PCIe Gen 2 TX Output -6.0dB Mode De-Emphasis Adjustment Setting
  UINT32  RsvdBits1                     : 11;     ///< Reserved Bits

  UINT32  Rsvd2[14];                              ///< Reserved bytes

} PCH_HSIO_PCIE_LANE_CONFIG;

///
/// The PCH_HSIO_PCIE_CONFIG block describes the configuration of the HSIO for PCIe lanes
///
typedef struct {
  ///
  /// These members describe the configuration of HSIO for PCIe lanes.
  ///
  PCH_HSIO_PCIE_LANE_CONFIG         Lane[PCH_MAX_PCIE_ROOT_PORTS];
  UINT32  Rsvd0[3];  ///< Reserved bytes

} PCH_HSIO_PCIE_CONFIG;

//
// ---------------------------- SATA Config -----------------------------
//

#define POLICY_SATA0_INDEX 0
#define POLICY_SATA1_INDEX 1

typedef enum  {
  PchSataModeAhci,
  PchSataModeRaid,
  PchSataModeMax
} PCH_SATA_MODE;

enum {
  PchSataOromDelay2sec,
  PchSataOromDelay4sec,
  PchSataOromDelay6sec,
  PchSataOromDelay8sec
} PCH_SATA_OROM_DELAY;

typedef enum {
  PchSataSpeedDefault,
  PchSataSpeedGen1,
  PchSataSpeedGen2,
  PchSataSpeedGen3
} PCH_SATA_SPEED;

/**
  This structure configures the features, property, and capability for each SATA port.
**/
typedef struct {
  /**
    Enable SATA port.
    It is highly recommended to disable unused ports for power savings
  **/
  UINT32  Enable           :  1;        ///< 0: Disable; <b>1: Enable</b>
  UINT32  HotPlug          :  1;        ///< <b>0: Disable</b>; 1: Enable
  UINT32  InterlockSw      :  1;        ///< <b>0: Disable</b>; 1: Enable
  UINT32  External         :  1;        ///< <b>0: Disable</b>; 1: Enable
  UINT32  SpinUp           :  1;        ///< <b>0: Disable</b>; 1: Enable the COMRESET initialization Sequence to the device
  UINT32  SolidStateDrive  :  1;        ///< <b>0: HDD</b>; 1: SSD
  UINT32  DevSlp           :  1;        ///< <b>0: Disable</b>; 1: Enable DEVSLP on the port
  UINT32  EnableDitoConfig :  1;        ///< <b>0: Disable</b>; 1: Enable DEVSLP Idle Timeout settings (DmVal, DitoVal)
  UINT32  DmVal            :  4;        ///< DITO multiplier. Default is <b>15</b>.
  UINT32  DitoVal          : 10;        ///< DEVSLP Idle Timeout (DITO), Default is <b>625</b>.
  /**
    Support zero power ODD <b>0: Disable</b>, 1: Enable.
    This is also used to disable ModPHY dynamic power gate.
  **/
  UINT32  ZpOdd            :  1;
  UINT32  RsvdBits0        :  9;        ///< Reserved fields for future expansion w/o protocol change

  UINT32  HsioRxEqBoostMagAdEnable     : 1;       ///< @deprecated, please use HsioRxGen3EqBoostMagEnable
  UINT32  HsioRxEqBoostMagAd           : 6;       ///< @deprecated, please use HsioRxGen3EqBoostMag

  UINT32  HsioTxGen1DownscaleAmpEnable : 1;       ///< @deprecated, please use HsioTxGen1DownscaleAmpEnable in PCH_HSIO_SATA_PORT_LANE
  UINT32  HsioTxGen1DownscaleAmp       : 6;       ///< @deprecated, please use HsioTxGen1DownscaleAmp in PCH_HSIO_SATA_PORT_LANE
  UINT32  HsioTxGen2DownscaleAmpEnable : 1;       ///< @deprecated, please use HsioTxGen2DownscaleAmpEnable in PCH_HSIO_SATA_PORT_LANE
  UINT32  HsioTxGen2DownscaleAmp       : 6;       ///< @deprecated, please use HsioTxGen2DownscaleAmp in PCH_HSIO_SATA_PORT_LANE
  UINT32  Rsvd0                        : 11;      ///< Reserved bits

} PCH_SATA_PORT_CONFIG;

/**
  Rapid Storage Technology settings.
**/
typedef struct {
  UINT32  RaidAlternateId :  1;         ///< <b>0: Disable</b>; 1: Enable RAID Alternate ID
  UINT32  Raid0           :  1;         ///< 0: Disable; <b>1: Enable</b> RAID0
  UINT32  Raid1           :  1;         ///< 0: Disable; <b>1: Enable</b> RAID1
  UINT32  Raid10          :  1;         ///< 0: Disable; <b>1: Enable</b> RAID10
  UINT32  Raid5           :  1;         ///< 0: Disable; <b>1: Enable</b> RAID5
  UINT32  Irrt            :  1;         ///< 0: Disable; <b>1: Enable</b> Intel Rapid Recovery Technology
  UINT32  OromUiBanner    :  1;         ///< 0: Disable; <b>1: Enable</b> OROM UI and BANNER
  UINT32  OromUiDelay     :  2;         ///< <b>00b: 2 secs</b>; 01b: 4 secs; 10b: 6 secs; 11: 8 secs (see: PCH_SATA_OROM_DELAY)
  UINT32  HddUnlock       :  1;         ///< 0: Disable; <b>1: Enable</b>. Indicates that the HDD password unlock in the OS is enabled
  UINT32  LedLocate       :  1;         ///< 0: Disable; <b>1: Enable</b>. Indicates that the LED/SGPIO hardware is attached and ping to locate feature is enabled on the OS
  UINT32  IrrtOnly        :  1;         ///< 0: Disable; <b>1: Enable</b>. Allow only IRRT drives to span internal and external ports
  UINT32  SmartStorage    :  1;         ///< 0: Disable; <b>1: Enable</b> RST Smart Storage caching Bit
  UINT32  RsvdBits0       : 19;         ///< Reserved Bits
} PCH_SATA_RST_CONFIG;

/**
  This structure describes the details of Intel RST for PCIe Storage remapping
  Note: In order to use this feature, Intel RST Driver is required
**/
typedef struct {
  /**
    This member describes whether or not the Intel RST for PCIe Storage remapping should be enabled. <b>0: Disable</b>; 1: Enable.
    Note 1: If Sata Controller is disabled, PCIe Storage Remapping should be disabled as well
    Note 2: If PCIe Storage remapping is enabled, the PCH integrated AHCI controllers Class Code is configured as RAID
  **/
  UINT32   Enable                 :  1;
  /**
    Intel RST for PCIe Storage remapping - PCIe Port Selection (1-based, <b>0 = autodetect</b>)
    The supported ports for PCIe Storage remapping is different depend on the platform and cycle router, the assignments are as below:
    SKL PCH-LP RST PCIe Storage Cycle Router Assignment:
    i.)   RST PCIe Storage Cycle Router 2 -> RP5 - RP8
    ii.)  RST PCIe Storage Cycle Router 3 -> RP9 - RP12

    SKL PCH-H RST PCIe Storage Cycle Router Assignment:
    i.)   RST PCIe Storage Cycle Router 1 -> RP9  - RP12
    ii.)  RST PCIe Storage Cycle Router 2 -> RP13 - RP16
    iii.) RST PCIe Storage Cycle Router 3 -> RP17 - RP20
  **/
  UINT32   RstPcieStoragePort     :  5;
  UINT32   RsvdBits0              :  2; ///< Reserved bit
  /**
    PCIe Storage Device Reset Delay in milliseconds (ms), which it guarantees such delay gap is fulfilled
    before PCIe Storage Device configuration space is accessed after an reset caused by the link disable and enable step.
    Default value is <b>100ms</b>.
  **/
  UINT32   DeviceResetDelay       :  8;
  UINT32   RsvdBits1              : 16; ///< Reserved bits

  UINT32   Rsvd0[2];                    ///< Reserved bytes
} PCH_RST_PCIE_STORAGE_CONFIG;

///
/// The PCH_SATA_CONFIG block describes the expected configuration of the SATA controllers.
///
typedef struct {
  ///
  /// This member describes whether or not the SATA controllers should be enabled. 0: Disable; <b>1: Enable</b>.
  ///
  UINT32                        Enable          :  1;
  UINT32                        TestMode        :  1;       ///< <b>(Test)</b> <b>0: Disable</b>; 1: Allow entrance to the PCH SATA test modes
  UINT32                        SalpSupport     :  1;       ///< 0: Disable; <b>1: Enable</b> Aggressive Link Power Management
  UINT32                        LpmSupport      :  1;       ///< 0: Disable; 1: Enable Link Power Management
  UINT32                        PortMultiplier  :  1;       ///< 0: Disable; 1: Enable Port Multiplier Capability
  /**
    eSATASpeedLimit
    When enabled, BIOS will configure the PxSCTL.SPD to 2 to limit the eSATA port speed.
    Please be noted, this setting could be cleared by HBA reset, which might be issued
    by EFI AHCI driver when POST time, or by SATA inbox driver/RST driver after POST.
    To support the Speed Limitation when POST, the EFI AHCI driver should preserve the
    setting before and after initialization. For support it after POST, it's dependent on
    driver's behavior.
    <b>0: Disable</b>; 1: Enable
  **/
  UINT32                        eSATASpeedLimit :  1;
  UINT32                        RsvdBits0       : 26;       ///< Reserved bits
  /**
    Determines the system will be configured to which SATA mode (PCH_SATA_MODE). Default is <b>PchSataModeAhci</b>.
  **/
  PCH_SATA_MODE                 SataMode;
  /**
    Indicates the maximum speed the SATA controller can support
    <b>0h: PchSataSpeedDefault</b>; 1h: 1.5 Gb/s (Gen 1); 2h: 3 Gb/s(Gen 2); 3h: 6 Gb/s (Gen 1)
  **/
  PCH_SATA_SPEED                SpeedLimit;
  /**
    This member configures the features, property, and capability for each SATA port.
  **/
  PCH_SATA_PORT_CONFIG          PortSettings[PCH_MAX_SATA_PORTS];
  PCH_SATA_RST_CONFIG           Rst;                        ///< Setting applicable to Rapid Storage Technology
  /**
    This member describes the details of implementation of Intel RST for PCIe Storage remapping (Intel RST Driver is required)
  **/
  PCH_RST_PCIE_STORAGE_CONFIG   RstPcieStorageRemap[PCH_MAX_RST_PCIE_STORAGE_CR];
  UINT32                        Rsvd0[4];               ///< Reserved fields for future expansion
} PCH_SATA_CONFIG;

/**
  The PCH_HSIO_SATA_PORT_LANE describes HSIO settings for SATA Port lane
**/
typedef struct {

  //
  // HSIO Rx Eq
  //
  UINT32  HsioRxGen1EqBoostMagEnable   : 1;       ///< <b>0: Disable</b>; 1: Enable Receiver Equalization Boost Magnitude Adjustment Value override
  UINT32  HsioRxGen1EqBoostMag         : 6;       ///< SATA 1.5 Gb/sReceiver Equalization Boost Magnitude Adjustment value
  UINT32  HsioRxGen2EqBoostMagEnable   : 1;       ///< <b>0: Disable</b>; 1: Enable Receiver Equalization Boost Magnitude Adjustment Value override
  UINT32  HsioRxGen2EqBoostMag         : 6;       ///< SATA 3.0 Gb/sReceiver Equalization Boost Magnitude Adjustment value
  UINT32  HsioRxGen3EqBoostMagEnable   : 1;       ///< <b>0: Disable</b>; 1: Enable Receiver Equalization Boost Magnitude Adjustment Value override
  UINT32  HsioRxGen3EqBoostMag         : 6;       ///< SATA 6.0 Gb/sReceiver Equalization Boost Magnitude Adjustment value

  //
  // HSIO Tx Eq
  //
  UINT32  HsioTxGen1DownscaleAmpEnable : 1;       ///< <b>0: Disable</b>; 1: Enable SATA 1.5 Gb/s TX Output Downscale Amplitude Adjustment value override
  UINT32  HsioTxGen1DownscaleAmp       : 6;       ///< SATA 1.5 Gb/s TX Output Downscale Amplitude Adjustment value
  UINT32  RsvdBits0                    : 4;       ///< Reserved bits

  UINT32  HsioTxGen2DownscaleAmpEnable : 1;       ///< <b>0: Disable</b>; 1: Enable SATA 3.0 Gb/s TX Output Downscale Amplitude Adjustment value override
  UINT32  HsioTxGen2DownscaleAmp       : 6;       ///< SATA 3.0 Gb/s TX Output Downscale Amplitude Adjustment
  UINT32  HsioTxGen3DownscaleAmpEnable : 1;       ///< <b>0: Disable</b>; 1: Enable SATA 6.0 Gb/s TX Output Downscale Amplitude Adjustment value override
  UINT32  HsioTxGen3DownscaleAmp       : 6;       ///< SATA 6.0 Gb/s TX Output Downscale Amplitude Adjustment
  UINT32  HsioTxGen1DeEmphEnable       : 1;       ///< <b>0: Disable</b>; 1: Enable SATA 1.5 Gb/s TX Output De-Emphasis Adjustment Setting value override
  UINT32  HsioTxGen1DeEmph             : 6;       ///< SATA 1.5 Gb/s TX Output De-Emphasis Adjustment Setting

  UINT32  HsioTxGen2DeEmphEnable       : 1;       ///< <b>0: Disable</b>; 1: Enable SATA 3.0 Gb/s TX Output De-Emphasis Adjustment Setting value override
  UINT32  HsioTxGen2DeEmph             : 6;       ///< SATA 3.0 Gb/s TX Output De-Emphasis Adjustment Setting
  UINT32  RsvdBits1                    : 4;       ///< Reserved bits

  UINT32  HsioTxGen3DeEmphEnable       : 1;       ///< <b>0: Disable</b>; 1: Enable SATA 6.0 Gb/s TX Output De-Emphasis Adjustment Setting value override
  UINT32  HsioTxGen3DeEmph             : 6;       ///< SATA 6.0 Gb/s TX Output De-Emphasis Adjustment Setting value override
  UINT32  RsvdBits2                    : 25;      ///< Reserved bits

  UINT32  Rsvd0[8];                               ///< Reserved bytes
} PCH_HSIO_SATA_PORT_LANE;


///
/// The PCH_HSIO_SATA_CONFIG block describes the HSIO configuration of the SATA controller.
///
typedef struct {
  ///
  /// These members describe the configuration of HSIO for SATA lanes.
  ///
  PCH_HSIO_SATA_PORT_LANE        PortLane[PCH_MAX_SATA_PORTS];
  UINT32                         Rsvd0[8];                               ///< Reserved bytes

} PCH_HSIO_SATA_CONFIG;

//
// --------------------------- IO APIC Config ------------------------------
//
/**
  The PCH_IOAPIC_CONFIG block describes the expected configuration of the PCH
  IO APIC, it's optional and PCH code would ignore it if the BdfValid bit is
  not TRUE. Bus:device:function fields will be programmed to the register
  P2SB IBDF(P2SB PCI offset R6Ch-6Dh), it's using for the following purpose:
  As the Requester ID when initiating Interrupt Messages to the processor.
  As the Completer ID when responding to the reads targeting the IOxAPI's
  Memory-Mapped I/O registers.
  This field defaults to Bus 0: Device 31: Function 0 after reset. BIOS can
  program this field to provide a unique Bus:Device:Function number for the
  internal IOxAPIC.
  The address resource range of IOAPIC must be reserved in E820 and ACPI as
  system resource.
**/
typedef struct {
  UINT32  BdfValid            :  1;     ///< Set to 1 if BDF value is valid, PCH code will not program these fields if this bit is not TRUE. <b>0: Disable</b>; 1: Enable.
  UINT32  RsvdBits0           :  7;     ///< Reserved bits
  UINT32  BusNumber           :  8;     ///< Bus/Device/Function used as Requestor / Completer ID. Default is <b>0xF0</b>.
  UINT32  DeviceNumber        :  5;     ///< Bus/Device/Function used as Requestor / Completer ID. Default is <b>0x1F</b>.
  UINT32  FunctionNumber      :  3;     ///< Bus/Device/Function used as Requestor / Completer ID. Default is <b>0x00</b>.
  UINT32  IoApicEntry24_119   :  1;     ///< 0: Disable; <b>1: Enable</b> IOAPIC Entry 24-119
  UINT32  RsvdBits1           :  7;     ///< Reserved bits
  UINT8   IoApicId;                     ///< This member determines IOAPIC ID. Default is <b>0x02</b>.
  UINT8   ApicRangeSelect;              ///< Define address bits 19:12 for the IOxAPIC range. Default is <b>0</b>
  UINT8   Rsvd0[2];                     ///< Reserved bytes
} PCH_IOAPIC_CONFIG;

//
// ---------------------------- HPET Config -----------------------------
//

/**
  The PCH_HPET_CONFIG block passes the bus/device/function value for HPET.
  The address resource range of HPET must be reserved in E820 and ACPI as
  system resource.
**/
typedef struct {
  /**
    Determines if enable HPET timer. 0: Disable; <b>1: Enable</b>.
    The HPET timer address decode is always enabled.
    This policy is used to configure the HPET timer count, and also the _STA of HPET device in ACPI.
    While enabled, the HPET timer is started, else the HPET timer is halted.
  **/
  UINT32   Enable             :  1;
  UINT32   BdfValid           :  1;     ///< Whether the BDF value is valid. <b>0: Disable</b>; 1: Enable.
  UINT32   RsvdBits0          :  6;     ///< Reserved bits
  UINT32   BusNumber          :  8;     ///< Bus Number HPETn used as Requestor / Completer ID. Default is <b>0xF0</b>.
  UINT32   DeviceNumber       :  5;     ///< Device Number HPETn used as Requestor / Completer ID. Default is <b>0x1F</b>.
  UINT32   FunctionNumber     :  3;     ///< Function Number HPETn used as Requestor / Completer ID. Default is <b>0x00</b>.
  UINT32   RsvdBits1          :  8;     ///< Reserved bits
  UINT32   Base;                        ///< The HPET base address. Default is <b>0xFED00000</b>.
} PCH_HPET_CONFIG;


//
// ------------------------------ LAN Config ---------------------------------
//

/**
  PCH intergrated LAN controller configuration settings.
**/
typedef struct {
  /**
    Determines if enable PCH internal LAN, 0: Disable; <b>1: Enable</b>.
    When Enable is changed (from disabled to enabled or from enabled to disabled),
    it needs to set LAN Disable regsiter, which might be locked by FDSWL register.
    So it's recommendated to issue a global reset when changing the status for PCH Internal LAN.
  **/
  UINT32  Enable          :  1;
  UINT32  LtrEnable       :  1;  ///< <b>0: Disable</b>; 1: Enable LTR capabilty of PCH internal LAN.
  UINT32  K1OffEnable     :  1;  ///< Use CLKREQ for GbE power management; 1: Enabled, <b>0: Disabled</b>;
  UINT32  RsvdBits0       :  4;  ///< Reserved bits
  UINT32  ClkReqSupported :  1;  ///< Indicate whether dedicated CLKREQ# is supported;  1: Enabled, <b>0: Disabled</b>;
  UINT32  ClkReqNumber    :  4;  ///< CLKREQ# used by GbE. Valid if ClkReqSupported is TRUE.
  UINT32  RsvdBits1       : 20;  ///< Reserved bits
  UINT32  Rsvd0;                 ///< Reserved bytes
} PCH_LAN_CONFIG;

//
// --------------------------- SMBUS Config ------------------------------
//

#define PCH_MAX_SMBUS_RESERVED_ADDRESS 128

///
/// The SMBUS_CONFIG block lists the reserved addresses for non-ARP capable devices in the platform.
///

enum {
	PchSMBusStandard80Khz,
	PchSMBusStandard100Khz,
	PchSMBusFastMode400Khz,
	PchSMBusFastModePlus1Mhz
} PCH_SMBUS_HOST_SPEED;

typedef struct {
  /**
    This member describes whether or not the SMBus controller of PCH should be enabled.
    0: Disable; <b>1: Enable</b>.
  **/
  UINT32  Enable           :  1;
  UINT32  ArpEnable        :  1;        ///< Enable SMBus ARP support, <b>0: Disable</b>; 1: Enable.
  UINT32  DynamicPowerGating :  1;      ///< <b>(Test)</b> <b>Disable</b> or Enable Smbus dynamic power gating.
  UINT32  RsvdBits0          : 29;      ///< Reserved bits
  UINT16  SmbusIoBase;                  ///< SMBUS Base Address (IO space). Default is <b>0xEFA0</b>.
  UINT8   Rsvd0;                        ///< Reserved bytes
  UINT8   NumRsvdSmbusAddresses;        ///< The number of elements in the RsvdSmbusAddressTable.
  /**
    Array of addresses reserved for non-ARP-capable SMBus devices.
  **/
  UINT8   RsvdSmbusAddressTable[PCH_MAX_SMBUS_RESERVED_ADDRESS];

  UINT8   SMBusIOSFClockGating;
  UINT8   SMBusHostSpeedConfig;
} PCH_SMBUS_CONFIG;

//
// --------------------------- Lock Down Config ------------------------------
//
/**
  The PCH_LOCK_DOWN_CONFIG block describes the expected configuration of the PCH
  for security requirement.
**/
typedef struct {
  /**
    <b>(Test)</b> Enable SMI_LOCK bit to prevent writes to the Global SMI Enable bit. 0: Disable; <b>1: Enable</b>.
  **/
  UINT32  GlobalSmi      :  1;
  /**
    <b>(Test)</b> Enable BIOS Interface Lock Down bit to prevent writes to the Backup Control Register
    Top Swap bit and the General Control and Status Registers Boot BIOS Straps. 0: Disable; <b>1: Enable</b>.
  **/
  UINT32  BiosInterface  :  1;
  /**
    <b>(Test)</b> Enable RTC lower and upper 128 byte Lock bits to lock Bytes 38h-3Fh in the upper
    and lower 128-byte bank of RTC RAM. 0: Disable; <b>1: Enable</b>.
  **/
  UINT32  RtcLock        :  1;
  /**
    Enable the BIOS Lock Enable (BLE) feature and set EISS bit (D31:F5:RegDCh[5])
    for the BIOS region protection. When it is enabled, the BIOS Region can only be
    modified from SMM after EndOfDxe protocol is installed.
    Note: When BiosLock is enabled, platform code also needs to update to take care
    of BIOS modification (including SetVariable) in DXE or runtime phase after
    EndOfDxe protocol is installed. <b>0: Disable</b>; 1: Enable.
  **/
  UINT32  BiosLock       :  1;
  /**
    Enable InSMM.STS (EISS) in SPI
    If this bit is set, then WPD must be a '1' and InSMM.STS must be '1' also
    in order to write to BIOS regions of SPI Flash. If this bit is clear,
    then the InSMM.STS is a don't care.
    The BIOS must set the EISS bit while BIOS Guard support is enabled.
    In recovery path, platform can temporary disable EISS for SPI programming in
    PEI phase or early DXE phase.
    0: Clear EISS bit; <b>1: Set EISS bit</b>.
  **/
  UINT32  SpiEiss        :  1;
  UINT32  GpioLockDown   :  1;
  UINT32  RsvdBits0      : 26;          ///< Reserved bits

} PCH_LOCK_DOWN_CONFIG;

//
// --------------------------- Thermal Config ------------------------------------
//
/**
  This structure lists PCH supported throttling register setting for custimization.
  When the SuggestedSetting is enabled, the customized values are ignored.
**/
typedef struct {
  UINT32 T0Level                  :  9; ///< Custimized T0Level value. If SuggestedSetting is used, this setting is ignored.
  UINT32 T1Level                  :  9; ///< Custimized T1Level value. If SuggestedSetting is used, this setting is ignored.
  UINT32 T2Level                  :  9; ///< Custimized T2Level value. If SuggestedSetting is used, this setting is ignored.
  UINT32 TTEnable                 :  1; ///< Enable the thermal throttle function. If SuggestedSetting is used, this settings is ignored.
  /**
    When set to 1 and the programmed GPIO pin is a 1, then PMSync state 13 will force at least T2 state.
    If SuggestedSetting is used, this setting is ignored.
  **/
  UINT32 TTState13Enable          :  1;
  /**
    When set to 1, this entire register (TL) is locked and remains locked until the next platform reset.
    If SuggestedSetting is used, this setting is ignored.
  **/
  UINT32 TTLock                   :  1;
  UINT32 SuggestedSetting         :  1; ///< 0: Disable; <b>1: Enable</b> suggested representative values.
  /**
    ULT processors support thermal management and cross thermal throttling between the processor package
    and LP PCH. The PMSYNC message from PCH to CPU includes specific bit fields to update the PCH
    thermal status to the processor which is factored into the processor throttling.
    Enable/Disable PCH Cross Throttling; 0: Disabled, 1: <b>Enabled</b>.
  **/
  UINT32 PchCrossThrottling       :  1;
  UINT32 Rsvd0;                      ///< Reserved bytes
} THERMAL_THROTTLE_LEVELS;

/**
  This structure allows to customize DMI HW Autonomous Width Control for Thermal and Mechanical spec design.
  When the SuggestedSetting is enabled, the customized values are ignored.
**/
typedef struct {
  UINT32  DmiTsawEn               :  1; ///< DMI Thermal Sensor Autonomous Width Enable
  UINT32  SuggestedSetting        :  1; ///< 0: Disable; <b>1: Enable</b> suggested representative values
  UINT32  RsvdBits0               :  6; ///< Reserved bits
  UINT32  TS0TW                   :  2; ///< Thermal Sensor 0 Target Width
  UINT32  TS1TW                   :  2; ///< Thermal Sensor 1 Target Width
  UINT32  TS2TW                   :  2; ///< Thermal Sensor 2 Target Width
  UINT32  TS3TW                   :  2; ///< Thermal Sensor 3 Target Width
  UINT32  RsvdBits1               : 16; ///< Reserved bits
} DMI_HW_WIDTH_CONTROL;

/**
  This structure lists PCH supported SATA thermal throttling register setting for custimization.
  The settings is programmed through SATA Index/Data registers.
  When the SuggestedSetting is enabled, the customized values are ignored.
**/
typedef struct {
  UINT32  P0T1M                   :  2; ///< Port 0 T1 Multipler
  UINT32  P0T2M                   :  2; ///< Port 0 T2 Multipler
  UINT32  P0T3M                   :  2; ///< Port 0 T3 Multipler
  UINT32  P0TDisp                 :  2; ///< Port 0 Tdispatch

  UINT32  P1T1M                   :  2; ///< Port 1 T1 Multipler
  UINT32  P1T2M                   :  2; ///< Port 1 T2 Multipler
  UINT32  P1T3M                   :  2; ///< Port 1 T3 Multipler
  UINT32  P1TDisp                 :  2; ///< Port 1 Tdispatch

  UINT32  P0Tinact                :  2; ///< Port 0 Tinactive
  UINT32  P0TDispFinit            :  1; ///< Port 0 Alternate Fast Init Tdispatch
  UINT32  P1Tinact                :  2; ///< Port 1 Tinactive
  UINT32  P1TDispFinit            :  1; ///< Port 1 Alternate Fast Init Tdispatch
  UINT32  SuggestedSetting        :  1; ///< 0: Disable; <b>1: Enable</b> suggested representative values
  UINT32  RsvdBits0               :  9; ///< Reserved bits
} SATA_THERMAL_THROTTLE;

/**
  This structure decides the settings of PCH Thermal throttling. When the Suggested Setting
  is enabled, PCH RC will use the suggested representative values.
**/
typedef struct {
  THERMAL_THROTTLE_LEVELS   TTLevels;
  DMI_HW_WIDTH_CONTROL      DmiHaAWC;
  SATA_THERMAL_THROTTLE     SataTT;
} PCH_THERMAL_THROTTLING;

/**
  This structure configures PCH memory throttling thermal sensor GPIO PIN settings
**/
typedef struct {
  /**
    GPIO PM_SYNC enable, 0:Diabled, 1:<b>Enabled</b>
    When enabled, RC will overrides the selected GPIO native mode.
    For GPIO_C, PinSelection 0: CPU_GP_0 (default) or 1: CPU_GP_1
    For GPIO_D, PinSelection 0: CPU_GP_3 (default) or 1: CPU_GP_2
    For SKL: CPU_GP_0 is GPP_E3, CPU_GP_1 is GPP_E7, CPU_GP_2 is GPP_B3, CPU_GP_3 is GPP_B4.
  **/
  UINT32  PmsyncEnable     :  1;
  UINT32  C0TransmitEnable :  1;        ///< GPIO Transmit enable in C0 state, 0:Disabled, 1:<b>Enabled</b>
  UINT32  PinSelection     :  1;        ///< GPIO Pin assignment selection, <b>0: default</b>, 1: secondary
  UINT32  RsvdBits0        : 29;
} TS_GPIO_PIN_SETTING;

enum PCH_PMSYNC_GPIO_X_SELECTION {
  TsGpioC,
  TsGpioD,
  MaxTsGpioPin
};

/**
  This structure supports an external memory thermal sensor (TS-on-DIMM or TS-on-Board).
**/
typedef struct {
  /**
   This will enable PCH memory throttling.
   While this policy is enabled, must also enable EnableExtts in SA policy.
   <b>0: Disable</b>; 1: Enable
  **/
  UINT32   Enable           :  1;
  UINT32   RsvdBits0        : 31;
  /**
    GPIO_C and GPIO_D selection for memory throttling.
    It's strongly recommended to choose GPIO_C and GPIO_D for memory throttling feature,
    and route EXTTS# accordingly.
  **/
  TS_GPIO_PIN_SETTING     TsGpioPinSetting[2];
} PCH_MEMORY_THROTTLING;

/**
  The PCH_THERMAL_CONFIG block describes the expected configuration of the PCH for Thermal.
**/
typedef struct {
  /**
    This field reports the status of Thermal Device. When it reports ThermalDevice
    is disabled, the PCI configuration space of thermal device will be hidden by
    setting TCFD and PCR[PSF2] TRH PCIEN[8] prior to end of POST.
  **/
  UINT32  ThermalDeviceEnable     :  2; ///< 0: Disabled, <b>1: Enabled in PCI mode</b>, 2: Enabled in ACPI mode
  UINT32  TsmicLock               :  1; ///< This locks down "SMI Enable on Alert Thermal Sensor Trip". 0: Disabled, 1: <b>Enabled</b>.
  UINT32  RsvdBits0               : 29;
  /**
    This field decides the settings of Thermal throttling. When the Suggested Setting
    is enabled, PCH RC will use the suggested representative values.
  **/
  PCH_THERMAL_THROTTLING    ThermalThrottling;
  /**
    Memory Thermal Management settings
  **/
  PCH_MEMORY_THROTTLING     MemoryThrottling;
  /**
    This field decides the temperature, default is <b>zero</b>.
    - 0x00 is the hottest
    - 0x1FF is the lowest temperature
  **/
  UINT16                    PchHotLevel;
  UINT8                     Rsvd0[6];
} PCH_THERMAL_CONFIG;


//
// ---------------------- Power Management Config --------------------------
//
/**
  This PCH_POWER_RESET_STATUS Specifies which Power/Reset bits need to be cleared by the PCH Init Driver.
  Usually platform drivers take care of these bits, but if not, let PCH Init driver clear the bits.
**/
typedef struct {
  UINT32  MeWakeSts           :  1;     ///< Clear the ME_WAKE_STS bit in the Power and Reset Status (PRSTS) register. 0: Disable; <b>1: Enable</b>.
  UINT32  MeHrstColdSts       :  1;     ///< Clear the ME_HRST_COLD_STS bit in the Power and Reset Status (PRSTS) register. 0: Disable; <b>1: Enable</b>.
  UINT32  MeHrstWarmSts       :  1;     ///< Clear the ME_HRST_WARM_STS bit in the Power and Reset Status (PRSTS) register. 0: Disable; <b>1: Enable</b>.
  UINT32  MeHostPowerDn       :  1;     ///< Clear the ME_HOST_PWRDN bit in the Power and Reset Status (PRSTS) register. <b>0: Disable</b>; 1: Enable.
  UINT32  WolOvrWkSts         :  1;     ///< Clear the WOL_OVR_WK_STS bit in the Power and Reset Status (PRSTS) register. 0: Disable; <b>1: Enable</b>.
  UINT32  RsvdBits0           : 27;
} PCH_POWER_RESET_STATUS;

/**
  This structure allows to customize PCH wake up capability from S5 or DeepSx by WOL, LAN, PCIE wake events.
**/
typedef struct {
  /**
    Corresponds to the PME_B0_S5_DIS bit in the General PM Configuration B (GEN_PMCON_B) register.
    When set to 1, this bit blocks wake events from PME_B0_STS in S5, regardless of the state of PME_B0_EN.
    When cleared (default), wake events from PME_B0_STS are allowed in S5 if PME_B0_EN = 1. <b>0: Disable</b>; 1: Enable.
  **/
  UINT32  PmeB0S5Dis         :  1;
  UINT32  WolEnableOverride  :  1;      ///< Corresponds to the "WOL Enable Override" bit in the General PM Configuration B (GEN_PMCON_B) register. 0: Disable; <b>1: Enable</b>.
  UINT32  Gp27WakeFromDeepSx :  1;      ///< @deprecated
  UINT32  PcieWakeFromDeepSx :  1;      ///< Determine if enable PCIe to wake from deep Sx. <b>0: Disable</b>; 1: Enable.
  UINT32  WoWlanEnable       :  1;      ///< Determine if WLAN wake from Sx, corresponds to the "HOST_WLAN_PP_EN" bit in the PWRM_CFG3 register. <b>0: Disable</b>; 1: Enable.
  UINT32  WoWlanDeepSxEnable :  1;      ///< Determine if WLAN wake from DeepSx, corresponds to the "DSX_WLAN_PP_EN" bit in the PWRM_CFG3 register. <b>0: Disable</b>; 1: Enable.
  UINT32  LanWakeFromDeepSx  :  1;      ///< Determine if enable LAN to wake from deep Sx. 0: Disable; <b>1: Enable</b>.
  UINT32  RsvdBits0          : 25;
} PCH_WAKE_CONFIG;

typedef enum {
  PchDeepSxPolDisable,
  PchDpS5BatteryEn,
  PchDpS5AlwaysEn,
  PchDpS4S5BatteryEn,
  PchDpS4S5AlwaysEn,
  PchDpS3S4S5BatteryEn,
  PchDpS3S4S5AlwaysEn
} PCH_DEEP_SX_CONFIG;

typedef enum {
  PchSlpS360us,
  PchSlpS31ms,
  PchSlpS350ms,
  PchSlpS32s
} PCH_SLP_S3_MIN_ASSERT;

typedef enum {
  PchSlpS4PchTime,     ///< The time defined in PCH EDS Power Sequencing and Reset Signal Timings table
  PchSlpS41s,
  PchSlpS42s,
  PchSlpS43s,
  PchSlpS44s
} PCH_SLP_S4_MIN_ASSERT;

typedef enum {
  PchSlpSus0ms,
  PchSlpSus500ms,
  PchSlpSus1s,
  PchSlpSus4s
} PCH_SLP_SUS_MIN_ASSERT;

typedef enum {
  PchSlpA0ms,
  PchSlpA4s,
  PchSlpA98ms,
  PchSlpA2s
} PCH_SLP_A_MIN_ASSERT;

/**
  The PCH_PM_CONFIG block describes expected miscellaneous power management settings.
  The PowerResetStatusClear field would clear the Power/Reset status bits, please
  set the bits if you want PCH Init driver to clear it, if you want to check the
  status later then clear the bits.
**/
typedef struct {
  /**
    Specify which Power/Reset bits need to be cleared by
    the PCH Init Driver.
    Usually platform drivers take care of these bits, but if
    not, let PCH Init driver clear the bits.
  **/
  PCH_POWER_RESET_STATUS  PowerResetStatusClear;
  PCH_WAKE_CONFIG         WakeConfig;                       ///< Specify Wake Policy
  PCH_DEEP_SX_CONFIG      PchDeepSxPol;                     ///< Deep Sx Policy. Default is <b>PchDeepSxPolDisable</b>.
  PCH_SLP_S3_MIN_ASSERT   PchSlpS3MinAssert;                ///< SLP_S3 Minimum Assertion Width Policy. Default is <b>PchSlpS350ms</b>.
  PCH_SLP_S4_MIN_ASSERT   PchSlpS4MinAssert;                ///< SLP_S4 Minimum Assertion Width Policy. Default is <b>PchSlpS44s</b>.
  PCH_SLP_SUS_MIN_ASSERT  PchSlpSusMinAssert;               ///< SLP_SUS Minimum Assertion Width Policy. Default is <b>PchSlpSus4s</b>.
  PCH_SLP_A_MIN_ASSERT    PchSlpAMinAssert;                 ///< SLP_A Minimum Assertion Width Policy. Default is <b>PchSlpA2s</b>.
  /**
    This member describes whether or not the PCI ClockRun feature of PCH should
    be enabled. <b>0: Disable</b>; 1: Enable
  **/
  UINT32                  PciClockRun          :  1;
  UINT32                  SlpStrchSusUp        :  1;        ///< <b>0: Disable</b>; 1: Enable SLP_X Stretching After SUS Well Power Up
  /**
    Enable/Disable SLP_LAN# Low on DC Power. 0: Disable; <b>1: Enable</b>.
    Configure On DC PHY Power Diable according to policy SlpLanLowDc.
    When this is enabled, SLP_LAN# will be driven low when ACPRESENT is low.
    This indicates that LAN PHY should be powered off on battery mode.
    This will override the DC_PP_DIS setting by WolEnableOverride.
  **/
  UINT32                  SlpLanLowDc          :  1;
  UINT32                  PwrBtnOverridePeriod :  3;        ///< PCH power button override period
  /**
    <b>(Test)</b>
    Disable/Enable PCH to CPU enery report feature. <b>0: Disable</b>; 1: Enable.
    Enery Report is must have feature. Wihtout Energy Report, the performance report
    by workloads/benchmarks will be unrealistic because PCH's energy is not being accounted
    in power/performance management algorithm.
    If for some reason PCH energy report is too high, which forces CPU to try to reduce
    its power by throttling, then it could try to disable Energy Report to do first debug.
    This might be due to energy scaling factors are not correct or the LPM settings are not
    kicking in.
  **/
  UINT32                  DisableEnergyReport  :  1;
  /**
    When set to Disable, PCH will internal pull down AC_PRESENT in deep SX and during G3 exit.
    When set to Enable, PCH will not pull down AC_PRESENT.
    This setting is ignored when DeepSx is not supported.
    Default is <b>0:Disable</b>
  **/
  UINT32                  DisableDsxAcPresentPulldown  :  1;
  /**
    <b>(Test)</b>
    When set to true, this bit disallows host reads to PMC XRAM.
    BIOS must set this bit (to disable and lock the feature) prior to passing control to OS
    0:Disable, <b>1:Enable</b>
  **/
  UINT32                  PmcReadDisable               :  1;
  /**
   This determines the type of reset issued during the capsule update process by UpdateCapsule().
   The default is <b>0:S3 Resume</b>, 1:Warm reset.
  **/
  UINT32                  CapsuleResetType             :  1;
  UINT32                  RsvdBits0                    : 22;
  /**
    Reset Power Cycle Duration could be customized in the unit of second. Please refer to EDS
    for all support settings. PCH HW default is 4 seconds, and range is 1~4 seconds, where
    <b>0 is default</b>, 1 is 1 second, 2 is 2 seconds, ... 4 is 4 seconds.
    And make sure the setting correct, which never less than the following register.
    - GEN_PMCON_B.SLP_S3_MIN_ASST_WDTH
    - GEN_PMCON_B.SLP_S4_MIN_ASST_WDTH
    - PWRM_CFG.SLP_A_MIN_ASST_WDTH
    - PWRM_CFG.SLP_LAN_MIN_ASST_WDTH
  **/
  UINT8                   PchPwrCycDur;
  /**
    Specifies the Pcie Pll Spread Spectrum Percentage
    The value of this policy is in 1/10th percent units.
    Valid spread range is 0-20. A value of 0xFF is reserved for AUTO.
    A value of 0 is SSC of 0.0%. A value of 20 is SSC of 2.0%
    The default is <b>0xFF: AUTO - No BIOS override</b>.
  **/
  UINT8                   PciePllSsc;
  UINT8                   Rsvd0[2];                             ///< Reserved bytes

  UINT8                   StateAfterG3;
  UINT8                   ActiveCoreCount;
} PCH_PM_CONFIG;

//
// ---------------------------- DMI Config -----------------------------
//

///
/// The PCH_DMI_CONFIG block describes the expected configuration of the PCH for DMI.
///
typedef struct {
  /**
    0: Disable; <b>1: Enable</b> ASPM on PCH side of the DMI Link.
    While DmiAspm is enabled, DMI ASPM will be set to Intel recommended value.
  **/
  UINT32     DmiAspm           :  1;
  UINT32     PwrOptEnable      :  1;    ///< <b>0: Disable</b>; 1: Enable DMI Power Optimizer on PCH side.
  UINT32     Rsvdbits          : 30;    ///< Reserved bits
  UINT32     Rsvd0[6];                  ///< Reserved bytes
} PCH_DMI_CONFIG;

//
// --------------------------- Serial IRQ Config ------------------------------
//

typedef enum {
  PchQuietMode,
  PchContinuousMode
} PCH_SIRQ_MODE;
///
/// Refer to PCH EDS for the details of Start Frame Pulse Width in Continuous and Quiet mode
///
typedef enum {
  PchSfpw4Clk,
  PchSfpw6Clk,
  PchSfpw8Clk
} PCH_START_FRAME_PULSE;

///
/// The PCH_LPC_SIRQ_CONFIG block describes the expected configuration of the PCH for Serial IRQ.
///
typedef struct {
  UINT32                SirqEnable    :  1;       ///< Determines if enable Serial IRQ. 0: Disable; <b>1: Enable</b>.
  UINT32                RsvdBits0     : 31;       ///< Reserved bits
  PCH_SIRQ_MODE         SirqMode;                 ///< Serial IRQ Mode Select. <b>0: quiet mode</b> 1: continuous mode.
  PCH_START_FRAME_PULSE StartFramePulse;          ///< Start Frame Pulse Width. Default is <b>PchSfpw4Clk</b>.
  UINT32                Rsvd0;                    ///< Reserved bytes
} PCH_LPC_SIRQ_CONFIG;

//
// --------------------------- Serial IO Config ------------------------------
//

/**
  The PCH_SERIAL_IO_CONFIG block provides the configurations to set the Serial IO controllers
  to Acpi devices or Pci controllers, and also set the interrupt type to Acpi or Pci
  through Policy. It also provides to configure the I2c0 and I2c1 voltage
  to 1.8v or 3.3v by platform setting.
  Please refer to PeiDxeSmmPchSerialIoLib.h for definition of device numbers and enum values for the below fields
**/
typedef struct {
  /**
    0: Disabled; 1: ACPI Mode; <b>2: PCI Mode</b>; 3: Hidden mode, 4: Legacy UART mode
    NOTE: Considering the PcdSerialIoUartDebugEnable and PcdSerialIoUartNumber for all SerialIo UARTx,
          the PCD is more meaningful to represent the board design. It means, if PcdSerialIoUartDebugEnable is TRUE,
          the board is designed to use the SerialIo UART for debug message and the PcdSerialIoUartNumber is dedicated
          to be Debug UART usage. Therefore, it should grayout the option from setup menu since no other options
          available for this UART controller on this board, and also override the policy default accordingly.
          While PcdSerialIoUartDebugEnable is FALSE, then it's allowed to configure the UART controller by policy.
  **/
  UINT8  DevMode[PCH_SERIALIO_MAX_CONTROLLERS];
  UINT8  Gpio;                                              ///< 0: Disabled; <b>1: Enabled</b>.
  UINT8  I2cVoltage[PCH_SERIALIO_MAX_I2C_CONTROLLERS];      ///< Selects the IO voltage for I2C controllers. It can be 1.8v or 3.3v. <b>0: PchSerialIoIs33V</b>; 1: PchSerialIoIs18V.
  UINT8  SpiCsPolarity[PCH_SERIALIO_MAX_SPI_CONTROLLERS];   ///< Selects SPI ChipSelect signal polarity, <b>0=active low</b>.
  UINT8  UartHwFlowCtrl[PCH_SERIALIO_MAX_UART_CONTROLLERS]; ///< Enables UART hardware flow control, CTS and RTS lines, <b>0:disabled</b>, 1:enabled
  UINT8  Rsvd0[1];                                          ///< Bytes reserved for reuse when new fields are added to struct
  UINT32 DebugUartNumber           :  2;                    ///< UART number for debug purpose. 0:UART0, 1: UART1, <b>2:UART2</b>
  UINT32 EnableDebugUartAfterPost  :  1;                    ///< Enable debug UART controller after post. 0: diabled, <b>1: enabled</b>
  UINT32 RsvdBits0                 : 29;
  UINT32 Rsvd1[3];                                          ///< Bytes reserved for reuse when new fields are added to struct
} PCH_SERIAL_IO_CONFIG;



typedef enum  {
  HsUartModePcie,
  HsUartModeLegacy,
  HsUartModeHide,
}PCH_HSUART_MODE;

#define HSUART_COM1_ADDRESSS 0x3f8
#define HSUART_COM2_ADDRESSS 0x2f8
#define HSUART_COM3_ADDRESSS 0x3e8
#define HSUART_COM4_ADDRESSS 0x2e8


typedef struct {
  PCH_HSUART_MODE HsUartMode; //0:Pcie, 1:Legacy, 2:Hide
  UINT16 HsUartCOM; //If Mode=Legacy, this value is assigned as IO base
} PCH_HSUART_CONFIG;



//
// --------------------- Interrupts Config ------------------------------
//
typedef enum {
  PchNoInt,        ///< No Interrupt Pin
  PchIntA,
  PchIntB,
  PchIntC,
  PchIntD
} PCH_INT_PIN;


typedef enum {
  PchPIRQA = 16,
  PchPIRQB, //17
  PchPIRQC, //18
  PchPIRQD, //19
  PchPIRQE, //20
  PchPIRQF, //21
  PchPIRQG, //22
  PchPIRQH  //23
} PCH_PIRQ_ROUTE;
///
/// The PCH_DEVICE_INTERRUPT_CONFIG block describes interrupt pin, IRQ and interrupt mode for PCH device.
///
typedef struct {
  UINT8        Device;                  ///< Device number
  UINT8        Function;                ///< Device function
  UINT8        IntX;                    ///< Interrupt pin: INTA-INTD (see PCH_INT_PIN)
  UINT8        Irq;                     ///< IRQ to be set for device.
} PCH_DEVICE_INTERRUPT_CONFIG;

#define PCH_MAX_DEVICE_INTERRUPT_CONFIG  64       ///< Number of all PCH devices
#define PCH_MAX_PXRC_CONFIG              8        ///< Number of PXRC registers in ITSS

///
/// The PCH_INTERRUPT_CONFIG block describes interrupt settings for PCH.
///
typedef struct {
  UINT8                        NumOfDevIntConfig;                               ///< Number of entries in DevIntConfig table
  UINT8                        Rsvd0[3];                                        ///< Reserved bytes, align to multiple 4.
  PCH_DEVICE_INTERRUPT_CONFIG  DevIntConfig[PCH_MAX_DEVICE_INTERRUPT_CONFIG];   ///< Array which stores PCH devices interrupts settings
  UINT8                        PxRcConfig[PCH_MAX_PXRC_CONFIG];                 ///< Array which stores interrupt routing for 8259 controller
  UINT8                        GpioIrqRoute;                                    ///< Interrupt routing for GPIO. Default is <b>14</b>.
  UINT8                        SciIrqSelect;                                    ///< Interrupt select for SCI. Default is <b>9</b>.
  UINT8                        TcoIrqSelect;                                    ///< Interrupt select for TCO. Default is <b>9</b>.
  UINT8                        TcoIrqEnable;                                    ///< Enable IRQ generation for TCO. <b>0: Disable</b>; 1: Enable.
} PCH_INTERRUPT_CONFIG;

//
// --------------------- TraceHub Config ------------------------------
//

///
/// The PCH_TRACE_HUB_CONFIG block describes TraceHub settings for PCH.
///
typedef struct {
  UINT8   TraceHubEnable;
  UINT8   TraceHubFwEnable;
  UINT8   TraceHubFwDestination;
  UINT8   TraceHubPtiMode;
  UINT8   TraceHubPtiTraining;
  UINT8   TraceHubPtiSpeed;
  UINT32  TraceHubMemBaseRegion0;
  UINT32  TraceHubMemBaseRegion1;
  UINT8   TraceHubMemoryEnabled;
  UINT32  EnableMode          :  2;      ///< 0 = Disable <b> 2 = Host Debugger enabled </b>
  UINT32  RsvdBits0           : 30;      ///< Reserved bits
  UINT32  MemReg0Size;                   ///< Default is <b>0 (none)</b>.
  UINT32  MemReg1Size;                   ///< Default is <b>0 (none)</b>.
} PCH_TRACE_HUB_CONFIG;

//
// ---------------------------- USB Config -----------------------------
//
///
/// Overcurrent pins, the values match the setting of PCH EDS, please refer to PCH EDS for more details
///
typedef enum {
  PchUsbOverCurrentPin0 = 0,
  PchUsbOverCurrentPin1,
  PchUsbOverCurrentPin2,
  PchUsbOverCurrentPin3,
  PchUsbOverCurrentPin4,
  PchUsbOverCurrentPin5,
  PchUsbOverCurrentPin6,
  PchUsbOverCurrentPin7,
  PchUsbOverCurrentPinSkip,
  PchUsbOverCurrentPinMax
} PCH_USB_OVERCURRENT_PIN;

///
/// The location of the USB connectors.  This information is use to decide eye diagram tuning value for Usb 2.0 motherboard trace.
///
enum PCH_USB_PORT_LOCATION{
  PchUsbPortLocationBackPanel = 0,
  PchUsbPortLocationFrontPanel,
  PchUsbPortLocationDock,
  PchUsbPortLocationMiniPciE,
  PchUsbPortLocationFlex,
  PchUsbPortLocationInternalTopology,
  PchUsbPortLocationSkip,
  PchUsbPortLocationMax
};


/**
  This structure configures per USB2 AFE settings.
  It allows to setup the port parameters.
**/
typedef struct {
/** Per Port HS Preemphasis Bias (PERPORTPETXISET)
  000b - 0mV
  001b - 11.25mV
  010b - 16.9mV
  011b - 28.15mV
  100b - 28.15mV
  101b - 39.35mV
  110b - 45mV
  111b - 56.3mV
**/
  UINT8   Petxiset;
/** Per Port HS Transmitter Bias (PERPORTTXISET)
  000b - 0mV
  001b - 11.25mV
  010b - 16.9mV
  011b - 28.15mV
  100b - 28.15mV
  101b - 39.35mV
  110b - 45mV
  111b - 56.3mV
**/
  UINT8   Txiset;
/**
  Per Port HS Transmitter Emphasis (IUSBTXEMPHASISEN)
  00b - Emphasis OFF
  01b - De-emphasis ON
  10b - Pre-emphasis ON
  11b - Pre-emphasis & De-emphasis ON
**/
  UINT8   Predeemp;
/**
  Per Port Half Bit Pre-emphasis (PERPORTTXPEHALF)
  1b - half-bit pre-emphasis
  0b - full-bit pre-emphasis
**/
  UINT8   Pehalfbit;
} PCH_USB20_AFE;

/**
  This structure configures per USB2 port physical settings.
  It allows to setup the port location and port length, and configures the port strength accordingly.
**/
typedef struct {
  UINT32        Enable             :  1;     ///< 0: Disable; <b>1: Enable</b>.
  UINT32        RsvdBits0          : 31;     ///< Reserved bits
  /**
    These members describe the specific over current pin number of USB 2.0 Port N.
    It is SW's responsibility to ensure that a given port's bit map is set only for
    one OC pin Description. USB2 and USB3 on the same combo Port must use the same
    OC pin (see: PCH_USB_OVERCURRENT_PIN).
  **/
  UINT8         OverCurrentPin;
  UINT8         Rsvd0[3];                    ///< Reserved bytes, align to multiple 4.
  PCH_USB20_AFE Afe;                         ///< USB2 AFE settings
  UINT32        Rsvd1[1];                    ///< Reserved bytes
} PCH_USB20_PORT_CONFIG;

/**
  This structure describes whether the USB3 Port N of PCH is enabled by platform modules.
**/
typedef struct {
  UINT32   Enable             :  1;     ///< 0: Disable; <b>1: Enable</b>.
  UINT32   RsvdBits0          : 31;     ///< Reserved bits
  /**
    These members describe the specific over current pin number of USB 3.0 Port N.
    It is SW's responsibility to ensure that a given port's bit map is set only for
    one OC pin Description. USB2 and USB3 on the same combo Port must use the same
    OC pin (see: PCH_USB_OVERCURRENT_PIN).
  **/
  UINT8    OverCurrentPin;
  UINT8    Rsvd0[3];                 ///< Reserved bytes, align to multiple 4

  UINT32 HsioTxDeEmphEnable           :  1; ///< Enable the write to USB 3.0 TX Output -3.5dB De-Emphasis Adjustment, <b>0: Disable</b>; 1: Enable.
  /**
    USB 3.0 TX Output -3.5dB De-Emphasis Adjustment Setting (ow2tapgen2deemph3p5)
    HSIO_TX_DWORD5[21:16]
    <b>Default = 29h</b> (approximately -3.5dB De-Emphasis)
  **/
  UINT32 HsioTxDeEmph                 :  6;

  UINT32 HsioTxDownscaleAmpEnable     :  1; ///< Enable the write to USB 3.0 TX Output Downscale Amplitude Adjustment, <b>0: Disable</b>; 1: Enable.
  /**
    USB 3.0 TX Output Downscale Amplitude Adjustment (orate01margin)
    HSIO_TX_DWORD8[21:16]
    <b>Default = 00h</b>
  **/
  UINT32 HsioTxDownscaleAmp           :  6;

  UINT32 RsvdBits1                    : 18; ///< Reserved bits
  UINT32 Rsvd1[1];                          ///< Reserved bytes
} PCH_USB30_PORT_CONFIG;

#define PCH_XHCI_MODE_OFF         0
#define PCH_XHCI_MODE_ON          1

/**
  These members describe some settings which are related to the SSIC ports.
**/
typedef struct {
  /**
    0: Disable; <b>1: Enable</b> SSIC support.
  **/
  UINT32  Enable      : 1;
  UINT32  RsvdBits1       : 31;
} PCH_XHCI_SSIC_PORT;
/**
  These members describe some settings which are related to the SSIC ports.
**/
typedef struct {
  PCH_XHCI_SSIC_PORT  SsicPort[PCH_XHCI_MAX_SSIC_PORT_COUNT];
} PCH_SSIC_CONFIG;

/**
  The PCH_XDCI_CONFIG block describes the configurations
  of the xDCI Usb Device controller.
**/
typedef struct {
  /**
    This member describes whether or not the xDCI controller should be enabled.
    0: Disable; <b>1: Enable</b>.
  **/
  UINT32  Enable              :  1;
  UINT32  RsvdBits0            : 31;     ///< Reserved bits
} PCH_XDCI_CONFIG;


/**
  This member describes the expected configuration of the PCH USB controllers,
  Platform modules may need to refer Setup options, schematic, BIOS specification
  to update this field.
  The Usb20OverCurrentPins and Usb30OverCurrentPins field must be updated by referring
  the schematic.
**/
typedef struct {
  /**
    This feature intends to reduce the necessary initialization time for USB HC
    and devices on root ports. It is assembled by PCHInit drivers in PEI and DXE phase.
    In PEI phase, the feature resets all USB HCs on PCH bus, including Intel EHCI
    and XHCI. After reset USB HC, continue the system initialization without waiting
    for the USB XHC reset ready. After running to DXE phase, the feature resets
    those USB devices installed on each USB HC root port in parallel, including
    any non USB3 speed devices on XHCI root port if XHCI is enabled.
    For USB3 protocol root port, USB3 speed devices will be advanced to
    enable state if link training succeeds after XHC reset.

    UsbPrecondition = Enable , Force USB Init happen in PEI as part of 2Sec Fast Boot bios optimization.
    UsbPrecondition = Disable, USB Init happen in DXE just like traditionally where it happen.
    Remark: With Precondition Enabled some USB2 devices which are not compliant with usb2 specification
    are not being detected if installed in the system during S4/S5.


    <b>0: Disable</b>; 1: Enable.
  **/
  UINT32                      UsbPrecondition     :  1;
  /**
    This policy will disable XHCI compliance mode on all ports. Complicance Mode should be default enabled.
    For the platform that support USB Type-C, it can disable Compliance Mode, and enable Compliance Mode when testing.
    <b>0:Disable</b> , 1: Enable
  **/
  UINT32                      DisableComplianceMode        :  1;
  UINT32                      RsvdBits0           : 30;     ///< Reserved bits
  /**
    These members describe whether the USB2 Port N of PCH is enabled by platform modules.
    Panel and Dock are used to describe the layout of USB port. Panel is only available for Desktop PCH.
    Dock is only available for Mobile LPT.
  **/
  PCH_USB20_PORT_CONFIG       PortUsb20[PCH_MAX_USB2_PORTS];
  /**
    These members describe whether the USB3 Port N of PCH is enabled by platform modules.
  **/
  PCH_USB30_PORT_CONFIG       PortUsb30[PCH_MAX_USB3_PORTS];

  UINT32                      Rsvd0[15];                    ///< Reserved bytes
  UINT8                       IIL1e;
  UINT8                       XhciL1e;
#ifdef FSP_FLAG
  UINT8                       XhciEnable;
#endif
} PCH_USB_CONFIG;

//
// ----------------------------- SCS Config --------------------------------
//
///
/// The PCH_SCS_CONFIG block describes Storage and Communication Subsystem (SCS) settings for PCH.
///

typedef enum {
  PchScsSdDisabled = 0,
  PchScsSdioMode,       ///< @deprecated
  PchScsSdcardMode
} PCH_SCS_DEV_SD_MODE;


//
// --------------------- ISH Config ------------------------------
//
///
/// The PCH_ISH_CONFIG block describes Integrated Sensor Hub device.
///
typedef struct {
  UINT32    Enable          :  1;   ///< ISH Controler 0: Disable; <b>1: Enable</b>.
  UINT32    SpiGpioAssign   :  1;   ///< ISH SPI GPIO pins assigned: <b>0: False</b> 1: True
  UINT32    Uart0GpioAssign :  1;   ///< ISH UART0 GPIO pins assigned: <b>0: False</b> 1: True
  UINT32    Uart1GpioAssign :  1;   ///< ISH UART1 GPIO pins assigned: <b>0: False</b> 1: True
  UINT32    I2c0GpioAssign  :  1;   ///< ISH I2C0 GPIO pins assigned: <b>0: False</b> 1: True
  UINT32    I2c1GpioAssign  :  1;   ///< ISH I2C1 GPIO pins assigned: <b>0: False</b> 1: True
  UINT32    I2c2GpioAssign  :  1;   ///< ISH I2C2 GPIO pins assigned: <b>0: False</b> 1: True
  UINT32    Gp0GpioAssign   :  1;   ///< ISH GP_0 GPIO pin assigned: <b>0: False</b> 1: True
  UINT32    Gp1GpioAssign   :  1;   ///< ISH GP_1 GPIO pin assigned: <b>0: False</b> 1: True
  UINT32    Gp2GpioAssign   :  1;   ///< ISH GP_2 GPIO pin assigned: <b>0: False</b> 1: True
  UINT32    Gp3GpioAssign   :  1;   ///< ISH GP_3 GPIO pin assigned: <b>0: False</b> 1: True
  UINT32    Gp4GpioAssign   :  1;   ///< ISH GP_4 GPIO pin assigned: <b>0: False</b> 1: True
  UINT32    Gp5GpioAssign   :  1;   ///< ISH GP_5 GPIO pin assigned: <b>0: False</b> 1: True
  UINT32    Gp6GpioAssign   :  1;   ///< ISH GP_6 GPIO pin assigned: <b>0: False</b> 1: True
  UINT32    Gp7GpioAssign   :  1;   ///< ISH GP_7 GPIO pin assigned: <b>0: False</b> 1: True
  UINT32    PdtUnlock       :  1;   ///< ISH PDT Unlock Msg: <b>0: False</b> 1: True
  UINT32    RsvdBits0       : 16;   ///< Reserved Bits
} PCH_ISH_CONFIG;

//
// --------------------------- Flash Protection Range Registers ------------------------------
//
/**
  The PCH provides a method for blocking writes and reads to specific ranges
  in the SPI flash when the Protected Ranges are enabled.
  PROTECTED_RANGE is used to specify if flash protection are enabled,
  the write protection enable bit and the read protection enable bit,
  and to specify the upper limit and lower base for each register
  Platform code is responsible to get the range base by PchGetSpiRegionAddresses routine,
  and set the limit and base accordingly.
**/
typedef struct {
  UINT32                WriteProtectionEnable     :  1;     ///< Write or erase is blocked by hardware. <b>0: Disable</b>; 1: Enable.
  UINT32                ReadProtectionEnable      :  1;     ///< Read is blocked by hardware. <b>0: Disable</b>; 1: Enable.
  UINT32                RsvdBits                  :  30;    ///< Reserved
  /**
    The address of the upper limit of protection
    This is a left shifted address by 12 bits with address bits 11:0 are assumed to be FFFh for limit comparison
  **/
  UINT16                ProtectedRangeLimit;
  /**
    The address of the upper limit of protection
    This is a left shifted address by 12 bits with address bits 11:0 are assumed to be 0
  **/
  UINT16                ProtectedRangeBase;
} PROTECTED_RANGE;

typedef struct {
  PROTECTED_RANGE       ProtectRange[PCH_FLASH_PROTECTED_RANGES];
} PCH_FLASH_PROTECTION_CONFIG;

//
// --------------------- WatchDog (WDT) Configuration ------------------------------
//
/**
  This policy clears status bits and disable watchdog, then lock the
  WDT registers.
  while WDT is designed to be disabled and locked by Policy,
  bios should not enable WDT by WDT PPI. In such case, bios shows the
  warning message but not disable and lock WDT register to make sure
  WDT event trigger correctly.
**/
typedef struct {
  UINT32    DisableAndLock    :  1;     ///< <b>(Test)</b> Set 1 to clear WDT status, then disable and lock WDT registers. <b>0: Disable</b>; 1: Enable.
  UINT32    RsvdBits          : 31;
} PCH_WDT_CONFIG;

//
// --------------------- P2SB Configuration ------------------------------
//
/**
  This structure contains the policies which are related to P2SB device.
**/
typedef struct {
  /**
    <b>(Test)</b>
    This unlock the SBI lock bit to allow SBI after post time. <b>0: Disable</b>; 1: Enable.
    NOTE: Do not set this policy "SbiUnlock" unless necessary.
  **/
  UINT32    SbiUnlock         :  1;
  /**
    <b>(Test)</b>
    The PSF registers will be locked before 3rd party code execution.
    This policy unlock the PSF space. <b>0: Disable</b>; 1: Enable.
    NOTE: Do not set this policy "PsfUnlock" unless necessary.
  **/
  UINT32    PsfUnlock         :  1;
  UINT32    RsvdBits          : 30;
} PCH_P2SB_CONFIG;

//
// --------------------- DCI Configuration ------------------------------
//
/**
  This structure contains the policies which are related to Direct Connection Interface (DCI).
**/
typedef struct {
  /**
    <b>(Test)</b> DCI enable (HDCIEN bit)
    when Enabled, allow DCI to be enabled. When Disabled, the host control is not enabling DCI feature.
    BIOS provides policy to enable or disable DCI, and user would be able to use BIOS option to change this policy.
    The user changing the setting from disable to enable, is taken as a consent from the user to enable this DCI feature.
    <b>0:Disabled</b>; 1:Enabled
  **/
  UINT32    DciEn           :  1;
  /**
    <b>(Test)</b> When set to Auto detect mode, it detects DCI being connected during BIOS post time and enable DCI.
    Else it disable DCI. This policy only apply when DciEn is disabled.
    NOTE: this policy should not be visible to end customer.
    0: Disable AUTO mode, <b>1: Enable AUTO mode</b>
  **/
  UINT32    DciAutoDetect   :  1;
  UINT32    RsvdBits        : 30;       ///< Reserved bits
} PCH_DCI_CONFIG;

//
// --------------------- LPC Configuration ------------------------------
//
/**
  This structure contains the policies which are related to LPC.
**/
typedef struct {
  /**
    Enhance the port 8xh decoding.
    Original LPC only decodes one byte of port 80h, with this enhancement LPC can decode word or dword of port 80h-83h.
    @note: this will occupy one LPC generic IO range register. While this is enabled, read from port 80h always return 0x00.
    0: Disable, <b>1: Enable</b>
  **/
  UINT32    EnhancePort8xhDecoding      :  1;
#if defined(FSP_FLAG) && defined(SERVER_BIOS_FLAG)
  UINT32    Port80RouteDisable          :  1;
  UINT32    RsvdBits                    : 30;     ///< Reserved bits
#else
  UINT32    RsvdBits                    : 31;     ///< Reserved bits
#endif
} PCH_LPC_CONFIG;

//
// --------------------- SPI Configuration ------------------------------
//
/**
  This structure contains the policies which are related to SPI.
**/
typedef struct {
  /**
    Force to show SPI controller.
    <b>0: FALSE</b>, 1: TRUE
    NOTE: For Windows OS, it MUST BE false. It's optional for other OSs.
  **/
  UINT32    ShowSpiController           :  1;
  UINT32    RsvdBits                    : 31;     ///< Reserved bits
} PCH_SPI_CONFIG;

//
// ---------------------------------------------------------------------
//

/**
  PCH Policy revision number
  Any backwards compatible changes to this structure will result in an update in the revision number
**/
#define PCH_POLICY_REVISION  12

/**
  The PCH Policy allows the platform code to publish a set of
  configuration information that the PCH drivers will use to configure the PCH hardware.
  The Revision field is used to accommodate backward compatible changes to the PPI/protocol.
  The Revision should be initialized to PCH_POLICY_REVISION_X
  by the PPI producer.
  The BusNumber field is used for platform to assign Bus number with multiple instances.

  All reserved/unused fields must be initialized with zeros.
**/
struct _PCH_POLICY {
  /**
    This member specifies the revision of the PCH policy PPI. This field is used to
    indicate backwards compatible changes to the protocol. Platform code that produces
    this PPI must fill with the correct revision value for the PCH reference code
    to correctly interpret the content of the PPI fields.

  Revision 1:   Original version
                - Add DciAutoDetect policy in PCH_GENERAL_CONFIG.
                - Add SbiUnlock policy in PCH_P2SB_CONFIG.
                - Add the following policies in PCH_ISH_CONFIG:
                  - SpiGpioAssign
                  - Uart0GpioAssign
                  - Uart1GpioAssign
                  - I2c0GpioAssign
                  - I2c1GpioAssign
                  - I2c2GpioAssign
                  - Gp0GpioAssign
                  - Gp1GpioAssign
                  - Gp2GpioAssign
                  - Gp3GpioAssign
                  - Gp4GpioAssign
                  - Gp5GpioAssign
                  - Gp6GpioAssign
                  - Gp7GpioAssign
                - Add ClkReqSupported and ClkReqDetect in PCH_PCIE_ROOT_PORT_CONFIG.
                - Add the following in PCH_SKYCAM_CIO2_CONFIG
                  - SkyCamPortATermOvrEnable
                  - SkyCamPortBTermOvrEnable
                  - SkyCamPortCTermOvrEnable
                  - SkyCamPortDTermOvrEnable
                - Add UartHwFlowCtrl in PCH_SERIAL_IO
                - Move DciEn and DciAutoDetect to PCH_DCI_CONFIG



  Revision 2:   Updated version
                - Add Enable policy in PCH_SSIC_CONFIG
                - Deprecated Target Debugger option of EnableMode in PCH_TRACE_HUB_CONFIG
                - Deprecated the following policies in PCH_TRACE_HUB_CONFIG
                  - MemReg0WrapEnable
                  - MemReg1WrapEnable
                  - TraceDestination
                  - PtiMode
                  - PtiSpeed
                  - PtiTraining
                - Deprecated the Usb3PinsTermination and ManualModeUsb30PerPinEnable in PCH_XHCI_CONFIG
                - Redefine the Enable policy in PCH_HPET_CONFIG
                - Add EnhancePort8xhDecoding in PCH_LPC_CONFIG
                - Add PsfUnlock in PCH_P2SB_CONFIG
                - Add AllowNoLtrIccPllShutdown in PCH_PCIE_CONFIG
                - Add PdtUnlock in PCH_ISH_CONFIG
                - Remove PwrmBase from policy since the base address is predefined.
                - Add DspEndpointDmic, DspEndpointBluetooth, DspEndpointI2s in PCH_HDAUDIO_CONFIG
                - Add Gen3EqPh3Method abd EqPh3LaneParam in PCH_PCIE_ROOT_PORT_CONFIG/PCH_PCIE_CONFIG
                - Remove SlotImplemented and PmeInterrupt from PCH_PCIE_ROOT_PORT_CONFIG



  Revision 3:   Updated version
                - Add PwrBtnOverridePeriod policy in PCH_PM_CONFIG
                - Add PCH_USB20_AFE in PCH_USB20_PORT_CONFIG
                - Add ClkReqSupported in PCH_LAN_CONFIG



  Revision 4:   Updated version
                - Add DeviceResetPad and DeviceResetPadActiveHigh in PCH_PCIE_ROOT_PORT_CONFIG


  Revision 5:   Updated version
                - Deprecated ScsSdioMode in PCH_SCS_CONFIG
                - Deprecated PchScsSdioMode (PCH_SCS_DEV_SD_MODE enum) for ScsSdSwitch in PCH_SCS_CONFIG
                - Add HSIO RX and TX EQ policy for PCIe and SATA
                - Add ComplianceTestMode in PCH_PCIE_CONFIG

  Revision 6:   Updated version
                - Add DisableEnergyReport in PCH_PM_CONFIG


  Revision 7:   Updated version
                - Deprecated Enabled as Acpi device option of DeviceEnable in PCH_SKYCAM_CIO2_CONFIG
                - Add PCH_SKYCAM_CIO2_FLS_CONFIG with the following elements:
                  - PortACtleEnable
                  - PortBCtleEnable
                  - PortCCtleEnable
                  - PortDCtleEnable
                  - PortACtleCapValue
                  - PortBCtleCapValue
                  - PortCCtleCapValue
                  - PortDCtleCapValue
                  - PortACtleResValue
                  - PortBCtleResValue
                  - PortCCtleResValue
                  - PortDCtleResValue
                  - PortATrimEnable
                  - PortBTrimEnable
                  - PortCTrimEnable
                  - PortDTrimEnable
                  - PortADataTrimValue
                  - PortBDataTrimValue
                  - PortCDataTrimValue
                  - PortDDataTrimValue
                  - PortAClkTrimValue
                  - PortBClkTrimValue
                  - PortCClkTrimValue
                  - PortDClkTrimValue
                - Rename and reorder the policies for better understanding.
                  - HsioTxOutDownscaleAmpAd3GbsEnable to HsioTxGen1DownscaleAmpEnable
                  - HsioTxOutDownscaleAmpAd6GbsEnable to HsioTxGen2DownscaleAmpEnable
                  - HsioTxOutDownscaleAmpAd3Gbs to HsioTxGen2DownscaleAmp
                  - HsioTxOutDownscaleAmpAd6Gbs to HsioTxGen2DownscaleAmp
                - Update SerialIo DevMode default to PCI mode.


  Revision 8:   Updated version
                - Deprecate GP27WakeFromDeepSx and add LanWakeFromDeepSx to align EDS naming
                - Add ShowSpiController policy and PCH_SPI_CONFIG.
                - Add DspUaaCompliance in PCH_HDAUDIO_CONFIG
                - Add PchPcieEqHardware support in PCH_PCIE_EQ_METHOD


    Revision 9:   Updated version
                - Add DebugUartNumber and EnableDebugUartAfterPost in PCH_SERIAL_IO_CONFIG
                - Add DetectTimeoutMs in PCH_PCIE_CONFIG
                - Add PciePllSsc in PCH_PM_CONFIG


    Revision 10:  Updated version
                - Add HsioTxDeEmph in PCH_USB30_PORT_CONFIG
                - Add HsioTxDownscaleAmp in PCH_USB30_PORT_CONFIG
                - Add HsioTxDeEmphEnable in PCH_USB30_PORT_CONFIG
                - Add HsioTxDownscaleAmpEnable in PCH_USB30_PORT_CONFIG

                - Deprecated PCH_SATA_PORT_CONFIG.HsioRxEqBoostMagAdEnable
                - Deprecated PCH_SATA_PORT_CONFIG.HsioRxEqBoostMagAd
                - Deprecated PCH_SATA_PORT_CONFIG.HsioTxGen1DownscaleAmpEnable
                - Deprecated PCH_SATA_PORT_CONFIG.HsioTxGen1DownscaleAmp
                - Deprecated PCH_SATA_PORT_CONFIG.HsioTxGen2DownscaleAmpEnable
                - Deprecated PCH_SATA_PORT_CONFIG.HsioTxGen2DownscaleAmp

                - Add PCH_HSIO_SATA_CONFIG HsioSataConfig in PCH_POLICY
                - Add HsioRxGen1EqBoostMagEnable in PCH_HSIO_SATA_PORT_LANE
                - Add HsioRxGen1EqBoostMag in PCH_HSIO_SATA_PORT_LANE
                - Add HsioRxGen2EqBoostMagEnable in PCH_HSIO_SATA_PORT_LANE
                - Add HsioRxGen2EqBoostMag in PCH_HSIO_SATA_PORT_LANE
                - Add HsioTxGen1DeEmphEnable in PCH_HSIO_SATA_PORT_LANE
                - Add HsioTxGen1DeEmph in PCH_HSIO_SATA_PORT_LANE
                - Add HsioTxGen2DeEmphEnable in PCH_HSIO_SATA_PORT_LANE
                - Add HsioTxGen2DeEmph in PCH_HSIO_SATA_PORT_LANE
                - Add HsioTxGen3DeEmphEnable in PCH_HSIO_SATA_PORT_LANE
                - Add HsioTxGen3DeEmph in PCH_HSIO_SATA_PORT_LANE
                - Add HsioTxGen3DownscaleAmpEnable in PCH_HSIO_SATA_PORT_LANE
                - Add HsioTxGen3DownscaleAmp in PCH_HSIO_SATA_PORT_LANE

                - Add PCH_HSIO_PCIE_CONFIG HsioPcieConfig in PCH_POLICY
                - Deprecated PCH_PCIE_ROOT_PORT_CONFIG.HsioRxSetCtleEnable
                - Deprecated PCH_PCIE_ROOT_PORT_CONFIG.HsioRxSetCtle
                - Add HsioRxSetCtleEnable in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioRxSetCtle in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen1DownscaleAmpEnable in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen1DownscaleAmp in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen2DownscaleAmpEnable in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen2DownscaleAmp in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen3DownscaleAmpEnable in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen3DownscaleAmp in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen1DeEmphEnable in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen1DeEmph in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen2DeEmph3p5Enable in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen2DeEmph3p5 in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen2DeEmph6p0Enable in PCH_HSIO_PCIE_LANE_CONFIG
                - Add HsioTxGen2DeEmph6p0 in PCH_HSIO_PCIE_LANE_CONFIG

                - Add DisableDsxAcPresentPulldown in PCH_PM_CONFIG
                - Add DynamicPowerGating in PCH_SMBUS_CONFIG
                - Add ZpOdd in PCH_SATA_PORT_CONFIG
                - Add Uptp and Dptp in PCH_PCIE_ROOT_PORT_CONFIG
                - Add PCH_PCIE_CONFIG2 PcieConfig2 in PCH_POLICY


    Revision 11:  Updated version
                - Add DisableComplianceMode in PCH_USB_CONFIG


    Revision 12:  Updated version
                - Add PmcReadDisable in PCH_PM_CONFIG
                - Add CapsuleResetType in PCH_PM_CONFIG

  **/
  UINT8                        Revision;

  UINT16                       AcpiBase;          ///< Power management I/O base address. Default is <b>0x1800</b>.
  UINT32                       Rsvd;
  ///
  /// PCH General configuration
  ///
  PCH_GENERAL_CONFIG           PchConfig;
  ///
  /// This member describes PCI Express controller's related configuration.
  ///
  PCH_PCIE_CONFIG              PcieConfig[ PCH_MAX_PCIE_CLUSTERS ];

  /**
    SATA controller's related configuration.
    SATA configuration that decides which Mode the SATA controller should operate in
    and whether PCH SATA TEST mode is enabled.
  **/
  PCH_SATA_CONFIG              SataConfig[PCH_MAX_SATA_CONTROLLERS];

  ///
  /// This member describes USB controller's related configuration.
  ///
  PCH_USB_CONFIG               UsbConfig;
  /**
    This member describes IOAPIC related configuration.
    Determines IO APIC ID and IO APIC Range.
  **/
  PCH_IOAPIC_CONFIG            IoApicConfig;
  ///
  /// This member describes HPET related configuration.
  ///
  PCH_HPET_CONFIG              HpetConfig;
  ///
  /// This member describes SMBus related configuration.
  ///
  PCH_SMBUS_CONFIG             SmbusConfig;
  ///
  /// This member describes LockDown related configuration.
  ///
  PCH_LOCK_DOWN_CONFIG         LockDownConfig;
  ///
  /// This member describes miscellaneous platform power management configurations.
  ///
  PCH_PM_CONFIG                PmConfig;
  ///
  /// This member describes the expected configuration of the PCH for Serial IRQ.
  ///
  PCH_LPC_SIRQ_CONFIG          SerialIrqConfig;
  ///
  /// This member describes interrupt settings for PCH.
  ///
  PCH_INTERRUPT_CONFIG         PchInterruptConfig;
  ///
  /// This member describes TraceHub settings for PCH.
  ///
  PCH_TRACE_HUB_CONFIG         PchTraceHubConfig;
  //DNV_TODO: Create dedicated structure for eMMC configuration
  UINT8                        eMMCEnabled;
#ifdef FSP_FLAG
  UINT8                        GbE0Enabled;
  UINT8                        GbE1Enabled;
#endif //FSP_FLAG
  ///
  /// This member describes the Flash Protection related configuration
  ///
  PCH_FLASH_PROTECTION_CONFIG  FlashProtectConfig;
  ///
  /// This member contains WDT enable configuration.
  ///
  PCH_WDT_CONFIG               WdtConfig;
  ///
  /// This member contains P2SB configuration.
  ///
  PCH_P2SB_CONFIG              P2sbConfig;
  ///
  /// This member contains DCI configuration.
  ///
  PCH_DCI_CONFIG               DciConfig;
  ///
  /// This member contains LPC configuration.
  ///
  PCH_LPC_CONFIG               LpcConfig;
  ///
  /// This member contains SPI configuration.
  ///
  PCH_SPI_CONFIG               SpiConfig;
  ///
  /// This member describes HSIO settings for SATA controller
  ///
  PCH_HSIO_SATA_CONFIG         HsioSataConfig;
  ///
  /// This member describes HSIO settings for PCIe controller
  ///
  PCH_HSIO_PCIE_CONFIG         HsioPcieConfig[ PCH_MAX_PCIE_CLUSTERS ];
  ///
  /// This is the extension of PCIE CONFIG
  ///
#ifdef PCH_PO_FLAG
  //
  // PwrOnSafeMode - bit definition:
  //
  union {
    struct {
      UINT32 NvmSafeRegister            :  1;
      UINT32 GpioSafeRegister           :  1;
      UINT32 DmiSafeRegister            :  1;
      UINT32 SmbusSafeRegister          :  1;
      UINT32 RtcSafeRegister            :  1;
      UINT32 ItssSafeRegister           :  1;
      UINT32 P2sbSafeRegister           :  1;
      UINT32 PsthSafeRegister           :  1;
      UINT32 HostPmSafeRegister         :  1;
      UINT32 ScsSafeRegister            :  1;
      UINT32 ThermalSafeRegister        :  1;
      UINT32 PcieSafeRegister           :  1;
      UINT32 PsfSafeRegister            :  1;
      UINT32 XhciSafeRegister           :  1;
      UINT32 XdciSafeRegister           :  1;
      UINT32 SataSafeRegister           :  1;
      UINT32 FiaSafeRegister            :  1;
      UINT32 LpcSafeRegister            :  1;
      UINT32 IshSafeRegister            :  1;
      UINT32 Obsolete1                  :  1; //previously SerialIoSafeRegister, no longer used
      UINT32 HdaSafeRegister            :  1;
      UINT32 DciSafeRegister            :  1;
      UINT32 CSI2SafeRegister           :  1;
      UINT32 SpiSafeRegister            :  1;
      UINT32 Rsvd                       :  8;
    } Bit;
    UINT32 Data;
  } PwrOnSafeMode;
#endif // PCH_PO_FLAG

  PCH_HSUART_CONFIG HsUartConfig[PCH_MAX_HSUART];

  // Denverton Aptiov override start
  // DBG2 ACPI table is installed by "Debug Port Tables" module  
  // UINT8 HsUartKDPort;
  // UINT8 HsUartMdpt;
  // Denverton Aptiov override end
  UINT8 GpioStatus;
  
  UINT8 PchHideP2sbEnable;

};

#pragma pack (pop)

#endif // _PCH_POLICY_COMMON_H_
