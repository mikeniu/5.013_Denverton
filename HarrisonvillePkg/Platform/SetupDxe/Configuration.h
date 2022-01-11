//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Configuration.h

Abstract:

  Driver configuration include file.

Revision History:

--*/

#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#define EFI_NON_DEVICE_CLASS            0x00
#define EFI_DISK_DEVICE_CLASS           0x01
#define EFI_VIDEO_DEVICE_CLASS          0x02
#define EFI_NETWORK_DEVICE_CLASS        0x04
#define EFI_INPUT_DEVICE_CLASS          0x08
#define EFI_ON_BOARD_DEVICE_CLASS       0x10
#define EFI_OTHER_DEVICE_CLASS          0x20

#define VFR_MAIN_CLASS                  0x01
#define VFR_INTELRCSETUP_CLASS          0x02  // Denverton Aptiov override - EIP#217439
#define VFR_BOOT_CLASS                  0x03
#define BOOT_OPTIONS_CLASS              0x04
#define VFR_CHIPSET_CLASS               0x05
#define VFR_SYSINFO_CLASS               0x06

#define VFR_FORMID_INTELRCSETUP         0x01  // Denverton Aptiov override - EIP#217439 
#define VFR_FORMID_PROCESSOR            0x02
#define VFR_FORMID_CHIPSET              0x03


#define VFR_PCI_FORM_ID                 0x04
#define VFR_FORMID_MEMORY               0x05
#define VFR_ACPI_FORM_ID                0x06

#define VFR_FORMID_THERMAL              0x07
#define VFR_FORMID_SUPERIO              0x08
#define VFR_FORMID_CK420                0x09


#define VFR_FORMID_MISC                 0x0A
//#define VFR_FORMID_USB                  0x0B // Denverton AptioV Override  - EIP#235760
#define VFR_TCG_FORM_ID                 0x0C
#define VFR_RESETTESTINGSV_FORM_ID      0x0D


#define VFR_FORMID_MEMORY_RAS           0x0F
#define VFR_FORMID_SIEN                 0x11
#define VFR_FORMID_NM                   0x12
#define VFR_FORMID_DCMI                 0x13
//#define VFR_FORMID_NETWORK              0x14 // Denverton AptioV Override - EIP#235760
#define VFR_FORMID_PWRMGT				0x15
#define SOCKET1_CPUPWRADVPMCFG_FORMID   0x16
#define SOCKET2_CPUPWRADVPMCFG_FORMID   0x17
#define SOCKET3_CPUPWRADVPMCFG_FORMID   0x18
#define SOCKET4_CPUPWRADVPMCFG_FORMID   0x19
#define VFR_FORMID_WHEA                 0x1A
#define VFR_FORMID_ELOG                 0x1B
#define VFR_FORMID_ELOGMEM              0x1C
#define VFR_FORMID_ELOGPCIE             0x1D
#define VFR_FORMID_UART                 0x1E

//Form ID's inside Super IO
#define AST2300_SERIAL0_FORM_ID         0x20
#define AST2300_SERIAL1_FORM_ID         0x21

//form Id's for south bridge
#define VFR_SB_FORM_ID                   0x22
#define VFR_PCIE_FORM_ID                 0x23
#define VFR_PPM_CONFIG_ID                0x24
#define VFR_ICH_PCIERP1_OPTIONS_FORM_ID  0x25
#define VFR_ICH_PCIERP2_OPTIONS_FORM_ID  0x26
#define VFR_ICH_PCIERP3_OPTIONS_FORM_ID  0x27
#define VFR_ICH_PCIERP4_OPTIONS_FORM_ID  0x28
#define VFR_P2P_SETTINGS_FORM_ID         0x29

// IOH form ID
#define VFR_IOH_FORM_ID                  0x30

// inside IOH


#define VFR_AHCI_FORM_ID                 0x34
#define VFR_GPIO_FORM_ID                 0x35
//
// MEMORY Form IDs
//
#define VFR_FORMID_MEM                      0x40
#define VFR_FORMID_BRANCH_RANK_CONFIG       0x71
#define VFR_FORMID_VALHOOKS_CONFIG          0x72
#define VFR_FORMID_THERMTHRT_CONFIG         0x73
#define VFR_FORMID_MEMTOPOLOGY_DISPLAY      0x74
#define VFR_FORMID_PAGE_POLICY_DISPLAY      0x7D
#define VFR_FORMID_MEMORY_TRAINING_DISPLAY  0x7E
#define VFR_FORMID_MEM_PWR_SAVE_ADV_ID      0x7F
#define VFR_FORMID_MEM_THERMAL_ID           0x80
#define VFR_FORMID_XMP_DISPLAY              0x81

//
// IIO Form IDs
//
#define VFR_FORMID_IIO_CONFIG           0x50
#define VFR_FORMID_VTD                  0x51
#define VFR_FORMID_PCIE                 0x52
#define VFR_FORMID_PCIE_IIO0P0          0x53
#define VFR_FORMID_PCIE_IIO0P1          0x54
#define VFR_FORMID_PCIE_IIO0P2          0x55
#define VFR_FORMID_PCIE_IIO0P3          0x56
#define VFR_FORMID_PCIE_IIO0P4          0x57
#define VFR_FORMID_PCIE_IIO0P5          0x58
#define VFR_FORMID_PCIE_IIO0P6          0x59
#define VFR_FORMID_PCIE_IIO0P7          0x5A
#define VFR_FORMID_PCIE_IIO0P8          0x5B
#define VFR_FORMID_PCIE_IIO0P9          0x5C
#define VFR_FORMID_PCIE_IIO0P10         0x5D
#define VFR_FORMID_PCIE_IIO1P0          0x5E
#define VFR_FORMID_PCIE_IIO1P1          0x5F
#define VFR_FORMID_PCIE_IIO1P2          0x69
#define VFR_FORMID_PCIE_IIO1P3          0x6A
#define VFR_FORMID_PCIE_IIO1P4          0x6B
#define VFR_FORMID_PCIE_IIO1P5          0x6C
#define VFR_FORMID_PCIE_IIO1P6          0x6D
#define VFR_FORMID_PCIE_IIO1P7          0x6E
#define VFR_FORMID_PCIE_IIO1P8          0x6F
#define VFR_FORMID_PCIE_IIO1P9          0x70
#define VFR_FORMID_PCIE_IIO1P10         0x75
#define VFR_FORMID_IIO0                 0x76
#define VFR_FORMID_IIO1                 0x77
#define VFR_FORMID_IOAT_CONFIG          0x78
#define VFR_FORMID_IIO0IOAT             0x79
#define VFR_FORMID_IIO1IOAT             0x7A
#define VFR_FORMID_IIO2IOAT             0x7B
#define VFR_FORMID_IIO3IOAT             0x7C
#define VFR_FORMID_LANES_PCIE           0x87

#define VFR_FORMID_SKT0SVDEVHIDE     0x82
#define VFR_FORMID_SKT1SVDEVHIDE     0x84
#define VFR_FORMID_SKT2SVDEVHIDE     0x85
#define VFR_FORMID_SKT3SVDEVHIDE     0x86
// Denverton AptioV Override Start - EIP#235760
/*
#define VFR_SECURITY_CONFIG_ID			0x90
#define VFR_SERIAL_IO_OPTIONS_CONFIG_ID	0x91
#define PCH_SERIAL_IO_I2C0_FORM_ID		0x92
#define PCH_SERIAL_IO_I2C1_FORM_ID		0x93
#define PCH_SERIAL_IO_SPI0_FORM_ID		0x94
#define PCH_SERIAL_IO_SPI1_FORM_ID		0x95
#define PCH_SERIAL_IO_UART0_FORM_ID		0x96
#define PCH_SERIAL_IO_UART1_FORM_ID		0x97
#define PCH_SERIAL_IO_GPIO_FORM_ID		0x98
#define PCH_SERIAL_IO_SDIO_FORM_ID		0x99
*/
// Denverton AptioV Override End - EIP#235760
//
// extended IIO form IDs for 4S
//
#define VFR_FORMID_PCIE_IIO2P0          0x0190
#define VFR_FORMID_PCIE_IIO2P1          0x0191
#define VFR_FORMID_PCIE_IIO2P2          0x0192
#define VFR_FORMID_PCIE_IIO2P3          0x0193
#define VFR_FORMID_PCIE_IIO2P4          0x0194
#define VFR_FORMID_PCIE_IIO2P5          0x0195
#define VFR_FORMID_PCIE_IIO2P6          0x0196
#define VFR_FORMID_PCIE_IIO2P7          0x0197
#define VFR_FORMID_PCIE_IIO2P8          0x0198
#define VFR_FORMID_PCIE_IIO2P9          0x0199
#define VFR_FORMID_PCIE_IIO2P10         0x019A
#define VFR_FORMID_PCIE_IIO3P0          0x019B
#define VFR_FORMID_PCIE_IIO3P1          0x019C
#define VFR_FORMID_PCIE_IIO3P2          0x019D
#define VFR_FORMID_PCIE_IIO3P3          0x019E
#define VFR_FORMID_PCIE_IIO3P4          0x019F
#define VFR_FORMID_PCIE_IIO3P5          0x01A0
#define VFR_FORMID_PCIE_IIO3P6          0x01A1
#define VFR_FORMID_PCIE_IIO3P7          0x01A2
#define VFR_FORMID_PCIE_IIO3P8          0x01A3
#define VFR_FORMID_PCIE_IIO3P9          0x01A4
#define VFR_FORMID_PCIE_IIO3P10         0x01A5
#define VFR_FORMID_IIO2                 0x01A6
#define VFR_FORMID_IIO3                 0x01A7
#define VFR_WOL_FORM_ID                 0x01A8

//
// PCH Form IDs
//
#define PCH_OPTIONS_FORM_ID             0x60
#define PCH_LAN_OPTIONS_FORM_ID         0x61
#define PCH_PCIERP0_OPTIONS_FORM_ID     0x62
#define PCH_PCIERP1_OPTIONS_FORM_ID     0x63
#define PCH_PCIERP2_OPTIONS_FORM_ID     0x64
#define PCH_PCIERP3_OPTIONS_FORM_ID     0x65
#define PCH_PCIERP4_OPTIONS_FORM_ID     0x66
#define PCH_PCIERP5_OPTIONS_FORM_ID     0x67
#define PCH_PCIERP6_OPTIONS_FORM_ID     0x102
#define PCH_PCIERP7_OPTIONS_FORM_ID     0x103
#define PCH_SATA_OPTIONS_FORM_ID        0x68
#define PCH_EVA_OPTIONS_FORM_ID         0x104
//#define VFR_FORMID_DFX_DBGMSG               0x170 // Denverton AptioV Override - EIP#235760

#define SB_FORM_ID                      0x171
#define PCH_PWRON_OPTIONS_FORM_ID       0x172
#define SATA_DEVICE_OPTIONS_FORM_ID     0x173
#define USB_OPTIONS_FORM_ID             0x174
#define SECURITY_OPTIONS_FORM_ID        0x175
#define PCH_TRACE_HUB_OPTIONS_FORM_ID   0x176
#define PCH_SATA_SFMS_OPTIONS_FORM_ID   0x177
#define SETUP_VOLATILE_DATA_ID          0x178

#define VFR_SATA0_CONFIGURATION_FORM_ID  0x0630
#define VFR_SATA1_CONFIGURATION_FORM_ID  0x0631

#define VFR_SATA0_PORT0_CONFIGURATION_FORM_ID  0x0632
#define VFR_SATA0_PORT1_CONFIGURATION_FORM_ID  0x0633
#define VFR_SATA0_PORT2_CONFIGURATION_FORM_ID  0x0634
#define VFR_SATA0_PORT3_CONFIGURATION_FORM_ID  0x0635
#define VFR_SATA0_PORT4_CONFIGURATION_FORM_ID  0x0636
#define VFR_SATA0_PORT5_CONFIGURATION_FORM_ID  0x0637
#define VFR_SATA0_PORT6_CONFIGURATION_FORM_ID  0x0638
#define VFR_SATA0_PORT7_CONFIGURATION_FORM_ID  0x0639

#define VFR_SATA1_PORT0_CONFIGURATION_FORM_ID  0x063A
#define VFR_SATA1_PORT1_CONFIGURATION_FORM_ID  0x063B
#define VFR_SATA1_PORT2_CONFIGURATION_FORM_ID  0x063C
#define VFR_SATA1_PORT3_CONFIGURATION_FORM_ID  0x063D
#define VFR_SATA1_PORT4_CONFIGURATION_FORM_ID  0x063E
#define VFR_SATA1_PORT5_CONFIGURATION_FORM_ID  0x063F
#define VFR_SATA1_PORT6_CONFIGURATION_FORM_ID  0x0640
#define VFR_SATA1_PORT7_CONFIGURATION_FORM_ID  0x0641

#define VFR_USBSS0_CONFIGURATION_FORM_ID         0x0642
#define VFR_USBHS0_CONFIGURATION_FORM_ID         0x0643

#define VFR_USBHS0_PORT0_CONFIGURATION_FORM_ID   0x0644
#define VFR_USBHS0_PORT1_CONFIGURATION_FORM_ID   0x0645
#define VFR_USBHS0_PORT2_CONFIGURATION_FORM_ID   0x0646
#define VFR_USBHS0_PORT3_CONFIGURATION_FORM_ID   0x0647

#define VFR_USBSS0_PORT0_CONFIGURATION_FORM_ID   0x0648
#define VFR_USBSS0_PORT1_CONFIGURATION_FORM_ID   0x0649
#define VFR_USBSS0_PORT2_CONFIGURATION_FORM_ID   0x064A
#define VFR_USBSS0_PORT3_CONFIGURATION_FORM_ID   0x064B




#define PCH_USB_OPTIONS_FORM_ID         0x107
#define PCH_EVA_CLOCK_GATING_FORM_ID    0x10A
#define PCH_THERMAL_FORM_ID             0x10B
#define PCH_EVA_SCU_CABLELENGTH_FORM_ID 0x10C
//
// Processor labels
//
#define PROCESSOR_HT_MODE               0x0100
#define PROCESSOR_FSB_MULTIPLIER        0x0101

#define BOOT_TIMEOUT_KEY_ID             0x1
#define BOOT_MANAGER_KEY_ID             0x2
#define BOOT_ORDER_KEY_ID               0x3
#define LEGACY_DEV_KEY_ID               0x4
#define LEGACY_ORDER_KEY_ID             0x5
#define AMI_CALLBACK_KEY_ID             0x6
#define EFI_SHELL_KEY_ID                0x7

#define VFR_FORMID_CPUCORE       0x83
//
// Memory labels
//
#define MEMORY_SLOT1_SPEED              0x0200
#define MEMORY_SLOT2_SPEED              0x0201
#define MEMORY_SLOT3_SPEED              0x0202
#define MEMORY_SLOT4_SPEED              0x0203
#define END_MEMORY_SLOT_SPEED           0x020F

#define VAR_EQ_ADMIN_DECIMAL_NAME       L"65"
#define VAR_EQ_USER_DECIMAL_NAME        L"66"
#define VAR_EQ_CONFIG_MODE_DECIMAL_NAME L"67"
#define VAR_EQ_HT_MODE_NAME             0x0048  // H
#define VAR_EQ_HT_MODE_DECIMAL_NAME     L"72"
#define VAR_EQ_RAID_MODE_NAME           0x0052  // R
#define VAR_EQ_RAID_MODE_DECIMAL_NAME   L"82"

//
// New variables used for VFR
//
#define VAR_EQ_PORT_WIDTH_NAME          0x000A
#define VAR_EQ_PORT_WIDTH_DECIMAL_NAME  L"10"

//
#define VFR_FORMID_CHIPSET_NB           0x0305
#define TRACE_HUB_FORM_ID               0x0306
#define REDRIVER_FORM_ID                0x0307
#define CAPABILITYLOCK_FORM_ID          0x0308
#define BWR_CBDMA0_FORM_ID              0x0309
#define BWD_NB_PCIE_FORM_ID             0x030A
#define ROOT_COMPLEX_0_FORM             0x030B
#define ROOT_COMPLEX_1_FORM             0x030C
#define ROOT_COMPLEX_2_FORM             0x030D
#define NTB_FORM                        0x030E
#define SELECT_CONTROL_ASPM_RP_1_FORM   0x030F
#define SELECT_LANE_REVERSAL_FORM       0x0310
#define GEN_SPEED_FORM                  0x0311
#define CLOCK_SOURCE_FORM               0x0312
#define BAR_PLACEMENT_FORM              0x0313
#define CONTROL_LINK_DISABLE_FORM       0x0314
#define CONTROL_CLOCKGATING_DISABLE_FORM 0x0315
#define ROOT_PORT_FORM                  0x0316
#define CBDMA_FORM_ID                   0x0317
#define BWR_CBDMA1_FORM_ID              0x0318
#define BWR_CBDMA2_FORM_ID              0x0319
#define BWR_CBDMA3_FORM_ID              0x031A
#define TIMING_MRC_FORM_ID              0x031B
#define VFR_SATA_CONFIGURATION_FORM_ID  0x031C
#define VFR_IQAT_CONFIGURATION_FORM_ID  0x031D

#define VFR_DE_EMPHASIS_FORM_ID         0x031E
#define VFR_FORMID_RATIO_LIMITS         0x031F
#define SELECT_RTTO_FORM                0x0320
#define SELECT_COMPLETION_TIMEOUT_FORM  0x0321
#define SSA_CONFIG_FORM_ID              0x0323
#define PASS_GATE_FORM_ID               0x0324
#define LEAKY_BUCKET_FORM_ID            0x032A
#define NONVOL_MEM_FORM_ID              0x0325
#define LINK_STATUS_INFO_FORM           0x0322
#define SELECT_RP_MAX_SPEED_FORM        0x0326
#define PCIE_RP_CLOCK_GATING_FORM       0x0327
#define PCIE_VRP_CLOCK_GATING_FORM      0x0328
#define PORT_DISABLE_FORM               0x0329
#define NONVOL_MEM_INTER_ADR_ID         0x0330

#define NB_PPR0_FORM_ID                 0x0331
#define NB_PPR1_FORM_ID                 0x0332
#define NB_PPR2_FORM_ID                 0x0333
#define NB_PPR3_FORM_ID                 0x0334
#define NB_PPR4_FORM_ID                 0x0335
#define NB_PPR5_FORM_ID                 0x0336
#define NB_PPR6_FORM_ID                 0x0337
#define NB_PPR7_FORM_ID                 0x0338

#define VFR_USB_CONFIGURATION_FORM_ID   0x0339
//
// ME Form ID's
//
#define VFR_FORMID_ME                   0x400
#define VFR_FORMID_FWDEBUG              0x401
#define VFR_FORMID_ICC_ENABLES          0x402
#define VFR_FORMID_ICC_CLOCK            0x403
#define ICC_ENABLES_FORM_QUESTIONID     0x404

#define VFR_FORMID_IE         			0x410
#define VFR_FORMID_FIAMUX_ID            0x0411
// New Definitions added for sdl tokens in Aptio
//
#define VFR_FORMID_DFX_MEMORY           0x0412
#define VFR_PCI_DFX_FORM_ID             0x0413
#define VFR_FORMID_DFX_PROCESSOR        0x0414
#define VFR_FORMID_DFX_SB               0x0415
#define VFR_FORMID_DFX_THERMAL          0x0416
#define DFX_SSA_CONFIG_FORM_ID          0x0417
#define ATTP				0x0 //Switch to Enable/Disable Active Thermal Trip Point
#define RSP_DISPLAY_THERMAL_ITEMS       0x0 //Switch to Hide/Display Thermal items in Setup
#define EMUL6064_SUPPORT                0x0 //Switch to Enable/Disable 6064 USB Emulation
#define USB_HOTPLUG_FDD                 0x0 //Switch to Enable/Disable FDD Hotplug
#define USB_HOTPLUG_HDD                 0x0 //Switch to Enable/Disable HDD Hotplug
#define USB_HOTPLUG_CDROM               0x0 //Switch to Enable/Disable CDROM Hotplug
#define USB_ONEOF_POWERGOODDEVICEDELAY  0x0
#define USB_NUMERIC_POWERGOODDEVICENUMDELAY 0x0
#define TPM_PASSWORD_AUTHENTICATION     0x1
#define TXT_SUPPORT                     0x0
#define CSM_SUPPORT                     0x1
#define DEFAULT_SATA_BOOT_OPTION        0x1
#define DEFAULT_SCU_BOOT_OPTION         0x1
#define DEFAULT_PXE_BOOT_OPTION         0x1
#define DEFAULT_SCSI_BOOT_OPTION        0x1
#define DEFAULT_EFI_OR_LEGACY_BOOT_OPTION 0x1
#define HPET_SUPPORT                    0x1
#define PciExpress_SUPPORT              0x1
#define WAKE_FROM_Sx_SUPPORT            0x1

#define LAKEMORE_SUPPORT                0x1

#ifndef AUTO_ID
#define AUTO_ID(x) x
#endif

#endif // #ifndef _CONFIGURATION_H
