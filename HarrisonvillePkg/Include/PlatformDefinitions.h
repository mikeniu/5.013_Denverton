//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformDefinitions.h

Abstract:

  This header file provides platform specific definitions used by other modules
  for platform specific initialization.

  THIS FILE SHOULD ONLY CONTAIN #defines BECAUSE IT IS CONSUMED BY NON-C MODULES
  (ASL and VFR)

  This file should not contain addition or other operations that an ASL compiler or
  VFR compiler does not understand.

--*/

#ifndef _PLATFORM_DEFINITIONS_H_
#define _PLATFORM_DEFINITIONS_H_


//
// Platform Base Address definitions
//
#define PCIEX_BASE_ADDRESS          (UINTN) PcdGet64 (PcdPciExpressBaseAddress) // Pci Express Configuration Space Base Address

#define PCIEX_LENGTH                0x10000000

#define HPET_ADDRESS_0              0xFED00000
#define HPET_ADDRESS_1              0xFED01000
#define HPET_ADDRESS_2              0xFED02000
#define HPET_ADDRESS_3              0xFED03000
#define HPET_MMIO_SIZE              0x4000      // Report all range for simple

#define THERMAL_BASE_ADDRESS        0xFED08000

#ifndef MCH_BASE_ADDRESS
#define MCH_BASE_ADDRESS            0xFED10000  // MCH  Register Base Address
#endif  //MCH_BASE_ADDRESS
// From Client SNB EDS 0.7v3, 32KB window
#define MCH_MMIO_SIZE               0x8000      // MCH  Register MMIO Size

#define HECIF0_BASE_ADDRESS         0xFED1A000  // HECI Function 0 Register Base Address
// --cr HECI MMIO size window to be fixed
#define HECIF1_BASE_ADDRESS         0xFED1B000  // HECI Function 1 Register Base Address

#define DMI_BASE_ADDRESS            0xFED18000  // DMI  Register Base Address
// From Client SNB EDS 0.7v3, 4KB window
#define DMI_MMIO_SIZE               0x1000      // DMI  Register MMIO Size

#define EP_BASE_ADDRESS             0xFED19000  // EP   Register Base Address
// From Client SNB EDS 0.7v3, 4KB window
#define EP_MMIO_SIZE                0x1000      // EP  Register MMIO Size

#define PCH_PWRM_BASE_ADDRESS       0xFE000000
#define PCH_PWRM_MMIO_SIZE          0x00010000

#define PCH_SPI_BASE_ADDRESS        0xFE010000
#define PCH_SPI_MMIO_SIZE           0x00001000

#define GDXC_BASE_ADDRESS           0xFED84000  // GDXC  Register Base Address
#define GDXC_MMIO_SIZE              0x1000      // GDXC  Register MMIO Size = 4KB
#define EDRAM_BASE_ADDRESS          0xFED80000  // EDRAM  Register Base Address
#define EDRAM_MMIO_SIZE             0x4000      // EDRAM  Register MMIO Size = 16KB

#define EFI_APIC_LOCAL_ADDRESS      0xFEE00000
#define APIC_LOCAL_MMIO_SIZE        0x1000

#define EFI_APIC_IO_ADDRESS_PCH     0xFEC00000
#define APIC_IO_MMIO_SIZE           0x1000

#define GTTMM_ADDRESS               0x3EC00000  // GTTMMADR ADDRESS
#define GTTMM_SIZE                  0x400000    // GTT SIZE

#define PCH_ACPI_BASE_ADDRESS       0x1800      // ACPI Power Management I/O Register Base Address
#define PCH_ACPI_P_BLK_ADDRESS      0x1810
#define PCH_ACPI_TIMER_ADDR         0x1808
#define PCH_ACPI_TIMER_MAX_VALUE    0x1000000   // The timer is 24 bit overflow
#define PCH_TCO_BASE_ADDRESS        0x400
#define PCH_TCO1_STS_REGISTER       0x1864
#define PM_BASE_ADDRESS             PCH_ACPI_BASE_ADDRESS
// Denverton AptioV Override Start - EIP#244496
// Denverton supports Memory upto 128GB as per the Design in presentation document
// #define MAX_MEM_CAPACITY            (64 * 1024 * 1024) // in KB
#define MAX_MEM_CAPACITY            (128 * 1024 * 1024) // in KB
// Denverton AptioV Override End - EIP#244496


#define SIO_BASE_ADDRESS                          0x0680
#define SIO_BASE_ADDRESS2                         0x1640
#define SIO_DOCKING_LPC_SWITCH_REGISTER_ADDRESS   0x0690
#define PCH_SMBUS_BASE_ADDRESS                    0xEFA0
#define PCH_IDE_BASE_ADDRESS                      0xFFA0

#define PCH_GPIO_BASE_ADDRESS                     0x0800
#define GPIO_BASE_ADDRESS                         PCH_GPIO_BASE_ADDRESS

//
// RTC software information
//
#define PCAT_RTC_ADDRESS_REGISTER   0x70
#define PCAT_RTC_DATA_REGISTER      0x71
#define RTC_ADDRESS_REGISTER_D      13

//
// APIC definitions
//
#define EFI_APIC_IO_ID_PCH          0x02

//
// ACPI software SMI numbers
//
#define EFI_ACPI_ENABLE_SW_SMI                    0xF0
#define EFI_ACPI_DISABLE_SW_SMI                   0xF1

#define FW_WRITE                                  0x42
#define FW_UPDATE                                 0x43

#define BIS_DATA_READ                             0x45
#define BIS_DATA_WRITE                            0x46

#define S3_RESTORE_MSR_SW_SMI                     0x48
#define FLASH_WRITE_PROTECT_POST_COMPLETED_SW_SMI 0x49
#define EFI_ACPI_LVMM_SW_SMI                      0x4A

#define PCIE_DOCK_BRIDGE_RESOURCE_PATCH_SMI       0x4D
#define INTEL_3945ABG_AUTO_DETECT_WORKAROUND_SMI  0x50

#define SET_DISPLAY_DEVICE_SMI                      00
#define GET_CURRENT_DISPLAY_DEVICE_SMI              01
#define GET_DISPLAY_DEVICE_DETECT_SMI               02
#define GET_NEXT_DISPLAY_DEVICE_SMI                 05
#define DISPLAY_SWITCH_TOGGLE_ACPI_SMI              10
#define DISPLAY_SWITCH_TOGGLE_LEGACY_SMI            11
#define DISPLAY_SWITCH_LID_OPEN_ACPI_SMI            12
#define DISPLAY_SWITCH_LID_OPEN_LEGACY_SMI          13
#define DISPLAY_SWITCH_LID_CLOSE_ACPI_SMI           14
#define DISPLAY_SWITCH_LID_CLOSE_LEGACY_SMI         15
#define POPUP_ICON_TOGGLE_SMI                       16
#define SET_HOR_VERT_COMPENSATION_SMI               17
#define SET_BACKLIGHT_CONTROL_SMI                   18
#define SET_ALI_LEVEL_SMI                           19

#define PCI_EXPRESS_HOT_PLUG_ADD_SMI                40
#define PCI_EXPRESS_HOT_PLUG_REMOVE_SMI             41
#define LID_STATE_SWITCH_SMI                        42
#define POWER_STATE_SWITCH_SMI                      43

#define HTT_LOGO_ENABLE_SMI                         50
#define HTT_THROTTLE_DISABLE_SMI                    51
#define HTT_THROTTLE_ENABLE_SMI                     52
#define SAVE_OSB_OS_TYPE_SMI                        53

#define ENABLE_C_STATE_IO_REDIRECTION_SMI           70
#define DISABLE_C_STATE_IO_REDIRECTION_SMI          71
#define ENABLE_SMI_C_STATE_COORDINATION_SMI         72
#define DISABLE_SMI_C_STATE_COORDINATION_SMI        73
#define ENABLE_P_STATE_HARDWARE_COORDINATION_SMI    74
#define DISABLE_P_STATE_HARDWARE_COORDINATION_SMI   75

#define SW_SMI_OVERCLOCKING_INTERFACE	0x72
#define SW_SMI_POST_COMPLETE	0xbb

#define SW_SMI_FROM_SMMBASE 0x55

//
// Define some fixed platform device location information
//
#define DESKTOP_LPC_SIO_CONFIG_DEFAULT_PORT 0x2E
#define DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT  0x2E
#define DESKTOP_LPC_SIO_DATA_DEFAULT_PORT   0x2F

#define LPC_SIO_CONFIG_PORT         0x4E
#define LPC_SIO_INDEX_PORT          0x4E
#define LPC_SIO_DATA_PORT           0x4F
#define LPC_SIO_CONFIG_DEFAULT_PORT 0x164E
#define LPC_SIO_INDEX_DEFAULT_PORT  0x164E
#define LPC_SIO_DATA_DEFAULT_PORT   0x164F
#define LPC_SERIAL_PORT1_BASE       0x3F8
#define LPC_SERIAL_PORT2_BASE       0x2F8


//
// BAR for Sata 0 (D31F2) and Sata 1 (D31F5) in Recovery Mode
//
#define PCH_SATA_LEGACY_BAR           0x1130
#define PCH_SATA_SIDPBA               0x1120
#define PCH_SATA2_LEGACY_BAR          0x1110
#define PCH_SATA2_SIDPBA              0x1100

#define PCH_SATA_PRIMARY_CMD_BAR      0x1F0
#define PCH_SATA_PRIMARY_CTL_BAR      0x3F6
#define PCH_SATA_SECONDARY_CMD_BAR    0x170
#define PCH_SATA_SECONDARY_CTL_BAR    0x376

#define PCH_SATA2_PRIMARY_CMD_BAR     0x4100
#define PCH_SATA2_PRIMARY_CTL_BAR     0x4110
#define PCH_SATA2_SECONDARY_CMD_BAR   0x4120
#define PCH_SATA2_SECONDARY_CTL_BAR   0x4130

//
// GPIO[22] is used to detect recovery mode.
//
#define RECOVERY_DETECT_JUMPER      BIT22

#include "PlatformGpio.h"

//
// Define the SMC EXTSMI# GPIO offset
//
#define SMC_EXTSMI_BIT_POSITION 1

//
// GPI Route Value( LPC + B8h) for SCI/EXTSMI/RUNTIME_SCI Gpio0~15.
// SMI is two-bit-Pair(bit1:bit0) = 01
// SCI is two-bit-Pair(bit1:bit0) = 10
//
#define PCH_GPI_ROUTE_SMI(Gpio)          ((( 0 << ((Gpio * 2) + 1)) | (1 << (Gpio * 2))))
#define PCH_GPI_ROUTE_SCI(Gpio)          ((( 1 << ((Gpio * 2) + 1)) | (0 << (Gpio * 2))))

//#define PCH_GPI_ROUTE               (PCH_GPI_ROUTE_SMI(1) | PCH_GPI_ROUTE_SCI(7) | PCH_GPI_ROUTE_SCI(8))
#define PCH_GPI_ROUTE               (PCH_GPI_ROUTE_SCI(7) | PCH_GPI_ROUTE_SCI(8))

//
// SmBus address to read DIMM SPD
//
#define PCH_SMBUS_BUS_DEV_FUNC              0x1F0400
#define PLATFORM_NUM_SMBUS_RSVD_ADDRESSES   4
#define SMBUS_ADDR_CH_A_1                   0xA0
#define SMBUS_ADDR_CH_A_2                   0xA2
#define SMBUS_ADDR_CH_B_1                   0xA4
#define SMBUS_ADDR_CH_B_2                   0xA6

#include "PlatformClock.h"

//
// SMI I/O Trap Type
//
#define TRAP_TYPE_PPM     0x01
#define TRAP_TYPE_DTS     0x02
#define TRAP_TYPE_IGD     0x03

//
// This could change for different flash parts
//
#define EFI_RUNTIME_UPDATABLE_FV_HEADER_LENGTH  0x48

//
// Define the offset in flash region of start of bios image
// This definition needs to be updated based on the flash
// map used and is only used for Non-descriptor mode. In descriptor
// mode the base address of the BIOS is read from the flash
// region 1 register.
//
#define BIOS_START_OFFSET   0x100000

//
// Define the offset between the KSC critical thermal trip point
// and the critical temperature reported to the OS via ACPI
//
#define KSC_CRITICAL_THERMAL_OFFSET  0x0A

//
//
//
#define Smsc1007_PME_BASE_ADDRESS  0x680
#define Thermal_Base_Address	   0xfed08000
#define SMBUS_BASE_ADDRESS	       0xEFA0

#define PLATFORM_DEBUG_PRINT_ERROR_LEVEL_VALUE  0x80000046  // DEBUG_ERROR|DEBUG_INFO|DEBUG_LOAD|DEBUG_WARN
#define PLATFORM_SERIAL_BAUD_RATE_VALUE         115200
#define CMOS_FAST_BOOT_REG_DEFAULT_VALUE        0x01

//
// Define TSEG size for platform
//
#define PLATFORM_TSEG_SIZE                      0x1000000

#define  CSME_HECI1PciRegBase         PCIEX_BASE_ADDRESS + (UINT32) (24 << 15) + (UINT32) (0 << 12)
#define  CSME_HECI2PciRegBase         PCIEX_BASE_ADDRESS + (UINT32) (24 << 15) + (UINT32) (1 << 12)



#endif //_PLATFORM_DEFINITIONS_H_
