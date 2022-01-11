//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  1999 - 2007 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Platform.h

Abstract:

  Romley platform specific information

--*/

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "PlatformDefinitions.h"
#include "BackCompatible.h"
#include "Uefi.h"
#include "Gpio.h"


//#define EFI_FIRMWARE_VENDOR L"Intel Corporation"

//
// MMIO space allocation
// 0x00000000 -- 0xC0000000 (TOLM)
// 0xC0000000 -- 0xD0000000 (PCIe map  space)
// 0xD0000000 -- 0xFE000000 (PCI space)
//
//
// Onboard / Integrated Graphics Device
//

#define VIDEO_DEV_NUM                   0x00
#define VIDEO_FUNC_NUM                  0x00
#define VIDEO_ROOTPORT_DEV_NUM          PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS
#define VIDEO_ROOTPORT_FUNC_NUM         PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5

#define GPE0_BASE_ADDRESS               0x09C0  //PcdBriarwoodGpeIoPortBaseAddress
#define IDE_BASE_ADDRESS                0x0580
#define RCBA_ADDRESS                    0xFED1C000  // Root complex base address
#define SMBUS_BASE_ADDRESS              0xEFA0
#define FV_CMC_BASE                     0xfffb0000
#define ACPI_P_BLOCK_BASE_ADDRESS	    0x910
#define HPET_BASE_ADDRESS	             0xFED00000


#define SIO_BASE_ADDRESS                0x0680
#define SIO_BASE_MASK                   0xFFF0
#define SIO_GPIO_BASE_ADDRESS           0x0800
#define PILOTII_MAILBOX_BASE_ADDRESS    0x0600
#define PILOTII_MAILBOX_MASK            0xFFE0

#define IOH_PACKET_HUB_MEM_BASE	0xb0000000
#define IOH_IIC_MEM_BASE	0xb0001000
#define IOH_GPIO_MEM_BASE_ADDR	0xb0002000
#define IOH_SATA_MEM_BASE	0xb0003000



//
// Local and I/O APIC addresses.
//
#define EFI_APIC_LOCAL_ADDRESS          0xFEE00000
#define LOCAL_APIC_ADDRESS              0xFEE00000

#define UNCORE_IOAPIC_ID              2

//
// CMOS usage
//
#define CMOS_CPU_BSP_SELECT             0x10
#define CMOS_CPU_UP_MODE                0x11
#define CMOS_CPU_RATIO_OFFSET           0x12

#define CMOS_CPU_CORE_HT_OFFSET         0x13
#define CMOS_CPU1_CORE_HT_OFFSET        0x14
#define CMOS_CPU2_CORE_HT_OFFSET        0x15
#define CMOS_CPU3_CORE_HT_OFFSET        0x16

#define CMOS_CPU0_CORE_DISABLE       	  0x1B            //Processor0Core Offset
#define CMOS_CPU1_CORE_DISABLE           0x1C            //Processor1Core Offset
#define CMOS_CPU2_CORE_DISABLE           0x1D            //Processor2Core Offset
#define CMOS_CPU3_CORE_DISABLE           0x1E            //Processor3Core Offset
#define CMOS_CPUx_CORE8_9_DISABLE        0x1F            //Processor[i]Core[9:8] Offset, Proc[i].Core[9:8] at this CMOS bits[x+1:x], where x = i * 2

#define CMOS_CPU_HT_OFFSET         	    0x14             //Processor Hyperthreading Offset
#define CMOS_EFI_DEBUG                  0x17
#define CMOS_CPU_BIST_OFFSET            0x18
#define CMOS_CPU_VMX_OFFSET             0x19
#define CMOS_PCH_PORT80_OFFSET          0x1A
#define CMOS_PLATFORM_DESIGNATOR        0x13                          // Second bank CMOS location of Platform ID


#define CMOS_DRAM_SHARED_MAILBOX_ADDR_REG    0xF0    // 2 bytes CMOS Space for the DRAM Share Mailbox address [31:16]

//
// Define valid platform types
//
typedef enum {
  TypeUnknown = 0,      // !!! SHOULD BE THE FIRST ENTRY !!!
  TypeDoubleCove,
  TypeIndyPeak,
  TypeCarsonPeak,
  TypePlatformMax       // !!! SHOULD BE THE LAST ENTRY !!!
} EFI_PLATFORM_TYPE;

//
// Memory information display for setup
//
struct memInfoSetup {
  UINT16  MemFreq;  // in MHz
  UINT16  MemMode;
  UINT32  TotalMem; // in MB
  UINT8   RasModes;
};

//
// Define platform type register
// feature space location for GPI register in FWH
//
#define FWH_GPI_BASE_ADDRESS  0xFFBC0100
#define PLATFORM_TYPE_REG     (*(UINT8 volatile *) FWH_GPI_BASE_ADDRESS)

//
// COM (Serial) Port Base address
//
#define  COM1_BASE                      0x3F8
#define  COM2_BASE                      0x2F8

//
// LPT (Parallel) Port Base address
//
#define  LPT1_BASE                      0x378


//
// CPU Equates
//
#define MAX_THREAD                      2
#define MAX_CORE                        4
#define MAX_DIE                         1
#define MAX_CPU_NUM                     (MAX_THREAD * MAX_CORE * MAX_DIE * MAX_CPU_SOCKET)
#define MAX_SOCKET                      1
#define MAX_BR                          2
#ifndef MAX_CH
#define MAX_CH                          4
#endif
#define MAX_MEM_NODE                    MAX_CPU_SOCKET*MAX_BR    // Two memory controllers per socket
#define MAX_CRS_ENTRIES_PER_NODE        4                        // Max number of ranges allowed on a memory node
#define MEM_ADDR_SHIFT_VAL              30                       // Shift for creating 64 bit address from 1GB granularity

#define MAX_DDR_PER_BRANCH              (MAX_CH*2)                  // due to lock step
#define MAX_DDR_PER_SOCKET              (MAX_DDR_PER_BRANCH*MAX_BR) // due to lock step

#define MEM64_LEN                       0x00100000000
#define RES_MEM64_36_BASE               0x01000000000 - MEM64_LEN   // 2^36
#define RES_MEM64_36_LIMIT              0x01000000000 - 1           // 2^36
#define RES_MEM64_39_BASE               0x08000000000 - MEM64_LEN   // 2^39
#define RES_MEM64_39_LIMIT              0x08000000000 - 1           // 2^39
#define RES_MEM64_40_BASE               0x10000000000 - MEM64_LEN   // 2^40
#define RES_MEM64_40_LIMIT              0x10000000000 - 1           // 2^40

#define MEM_4GB							0x100000000ULL

//
// Need minimum of 48MB during PEI phase for IAG.
//
#define  PEI_MIN_MEMORY_SIZE               (6 * 0x800000)     // 48MB
#define  PEI_RECOVERY_MIN_MEMORY_SIZE      (6 * 0x800000)     // 48MB

#define HPET_BLOCK_ADDRESS                            0x0FED00000
#define PCI_BUS_NUMBER_PCH_HPET                       0xF0
#define PCI_DEVICE_NUMBER_PCH_HPET                    0x0F
#define PCI_FUNCTION_NUMBER_PCH_HPET0                 0x00
#define PCI_FUNCTION_NUMBER_PCH_HPET1                 0x01
#define PCI_FUNCTION_NUMBER_PCH_HPET2                 0x02
#define PCI_FUNCTION_NUMBER_PCH_HPET3                 0x03
#define PCI_FUNCTION_NUMBER_PCH_HPET4                 0x04
#define PCI_FUNCTION_NUMBER_PCH_HPET5                 0x05
#define PCI_FUNCTION_NUMBER_PCH_HPET6                 0x06
#define PCI_FUNCTION_NUMBER_PCH_HPET7                 0x07
#define R_PCH_LPC_H0BDF                               0x70
#define R_PCH_LPC_H1BDF                               0x72
#define R_PCH_LPC_H2BDF                               0x74
#define R_PCH_LPC_H3BDF                               0x76
#define R_PCH_LPC_H4BDF                               0x78
#define R_PCH_LPC_H5BDF                               0x7A
#define R_PCH_LPC_H6BDF                               0x7C
#define R_PCH_LPC_H7BDF                               0x7E


//
// I/O APIC IDs, the code uses math to generate the numbers
// instead of using these defines.
//
#define ICH_IOAPIC                     (1 << 0)
#define ICH_IOAPIC_ID                   0x08

//
// SMBUS Data
//
#define PLATFORM_NUM_SMBUS_RSVD_ADDRESSES   4
#define SMBUS_ADDR_CH_A_1                   0xA0
#define SMBUS_ADDR_CH_A_2                   0xA2
#define SMBUS_ADDR_CH_B_1                   0xA4
#define SMBUS_ADDR_CH_B_2                   0xA6
#define SMBUS_IO_EXPANDER_ADDRESS           0x38

#define PXE_BASE_OPTION_ROM_FILE_GUID \
  { \
    0x6f2bc426, 0x8d83, 0x4f17, 0xbe, 0x2c, 0x6b, 0x1f, 0xcb, 0xd1, 0x4c, 0x80 \
  }

#define SATA_AHCI_OPTION_ROM_FILE_GUID \
{ 0x7f07f91c, 0xb74e, 0x4017, 0xa1, 0xdd, 0x62, 0xe6, 0x47, 0xcb, 0x9f, 0xf3 }

//
// NIC DIDs
// 
#define V_SOC_LAN_DEVICE_ID_VF_HYPER_V        0x15B4
#define V_SOC_LAN_DEVICE_ID_10G_BLACKPLANE    0x15C2
#define V_SOC_LAN_DEVICE_ID_1G_BLACKPLANE     0x15C3
#define V_SOC_LAN_DEVICE_ID_10G_SFP_P         0x15C4
#define V_SOC_LAN_DEVICE_ID_VIRTUAL_FUNCTION  0x15C5
#define V_SOC_LAN_DEVICE_ID_1G_SGMII          0x15C6
#define V_SOC_LAN_DEVICE_ID_1G_SGMII_2        0x15C7
#define V_SOC_LAN_DEVICE_ID_10G_BASE_T        0x15C8
#define V_SOC_LAN_DEVICE_ID_10G_SFP_SFP_P     0x15CE

//EDK2_TODO Need to check with CCB to see if this can be moved to PiBootmode.h
#define BOOT_WITH_MFG_MODE_SETTINGS                   0x07

// added for ACPIbase address
#define R_ACPI_LV2            0x14
#endif
