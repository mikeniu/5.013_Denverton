/** @file
  Macros that simplify accessing PCH devices's PCI registers.

@copyright
 Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
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
#ifndef _PCH_ACCESS_H_
#define _PCH_ACCESS_H_

#include "PchLimits.h"
#include "PchReservedResources.h"

#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 1
#endif
#ifndef STALL_ONE_SECOND
#define STALL_ONE_SECOND 1000000
#endif


///
/// The default PCH PCI bus number
///
#define DEFAULT_PCI_BUS_NUMBER_PCH  0

//
// Default Vendor ID and Subsystem ID
//
#define V_PCH_INTEL_VENDOR_ID   0x8086      ///< Default Intel PCH Vendor ID
#define V_PCH_DEFAULT_SID       0x7270      ///< Default Intel PCH Subsystem ID
#define V_PCH_DEFAULT_SVID_SID  (V_INTEL_VENDOR_ID + (V_PCH_DEFAULT_SID << 16))   ///< Default INTEL PCH Vendor ID and Subsystem ID

//
// Include device register definitions
//
#include "PcieRegs.h"
#include "Register/PchRegsPcr.h"
#include "Register/PchRegsP2sb.h"
#include "Register/PchRegsHsio.h"
#include "Register/PchRegsLpc.h"
#include "Register/PchRegsPmc.h"
#include "Register/PchRegsSata.h"
#include "Register/PchRegsSmbus.h"
#include "Register/PchRegsSpi.h"
#include "Register/PchRegsUsb.h"
#include "Register/PchRegsGpio.h"
#include "Register/PchRegsTraceHub.h"
#include "Register/PchRegsScc.h"
#include "Register/PchRegsItss.h"
#include "Register/PchRegsPsf.h"
#include "Register/PchRegsFia.h"
#include "Register/PchRegsDci.h"
#include "Register/PchRegsScc.h"
//
//  LPC Device ID macros
//
//
// Device IDs that are PCH-H Desktop specific
//
#define IS_PCH_H_LPC_DEVICE_ID_DESKTOP(DeviceId) \
    (  \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_DT_0) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_DT_1) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_DT_2) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_DT_3) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_DT_4) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_DT_5) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_DT_6) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_UNFUSE) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_DT_SUPER_SKU) \
    )

#define IS_PCH_LPC_DEVICE_ID_DESKTOP(DeviceId) \
    ( \
      IS_PCH_H_LPC_DEVICE_ID_DESKTOP(DeviceId) \
    )

//
// Device IDs that are PCH-H Mobile specific
//

#define IS_PCH_H_LPC_DEVICE_ID_MOBILE(DeviceId) \
    ( \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_MB_0) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_MB_1) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_MB_2) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_MB_SUPER_SKU) \
    )


//
// Device IDs that are PCH-LP Mobile specific
//
#define IS_PCH_LP_LPC_DEVICE_ID_MOBILE(DeviceId) \
    ( \
      (DeviceId == V_PCH_LP_LPC_DEVICE_ID_UNFUSE) || \
      (DeviceId == V_PCH_LP_LPC_DEVICE_ID_MB_SUPER_SKU) || \
      (DeviceId == V_PCH_LP_LPC_DEVICE_ID_MB_0) || \
      (DeviceId == V_PCH_LP_LPC_DEVICE_ID_MB_1) || \
      (DeviceId == V_PCH_LP_LPC_DEVICE_ID_MB_2) || \
      (DeviceId == V_PCH_LP_LPC_DEVICE_ID_MB_3) \
    )

#define IS_PCH_LPC_DEVICE_ID_MOBILE(DeviceId) \
    ( \
      IS_PCH_H_LPC_DEVICE_ID_MOBILE(DeviceId) || \
      IS_PCH_LP_LPC_DEVICE_ID_MOBILE(DeviceId) \
    )

//
// Device IDS that are PCH Server\Workstation specific
//


#define IS_PCH_H_LPC_DEVICE_ID_USERVER(DeviceId) \
    ( \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_USVR_SUPER_SKU) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_USVR_0) \
    )

#define IS_PCH_LPC_DEVICE_ID_USERVER(DeviceId) \
    ( \
      IS_PCH_H_LPC_DEVICE_ID_USERVER(DeviceId) \
    )

#define IS_PCH_H_LPC_DEVICE_ID_SERVER(DeviceId) \
    ( \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_SVR_0) || \
      (DeviceId == V_PCH_H_LPC_DEVICE_ID_SVR_1) || \
      IS_PCH_H_LPC_DEVICE_ID_USERVER(DeviceId) \
    )


#define IS_PCH_LPC_DEVICE_ID_SERVER(DeviceId) \
    ( \
      IS_PCH_H_LPC_DEVICE_ID_SERVER(DeviceId) \
    )
    
#define IS_PCH_H_LPC_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_LPC_DEVICE_ID_DESKTOP (DeviceId) || \
      IS_PCH_H_LPC_DEVICE_ID_MOBILE (DeviceId) || \
      IS_PCH_H_LPC_DEVICE_ID_SERVER (DeviceId) \
    )

#define IS_PCH_LP_LPC_DEVICE_ID(DeviceId) \
    ( \
     IS_PCH_LP_LPC_DEVICE_ID_MOBILE (DeviceId) \
    )

#define IS_PCH_LPC_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_LPC_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_LPC_DEVICE_ID(DeviceId) \
    )

//
//  SATA AHCI Device ID macros
//
#define IS_PCH_H_SATA_AHCI_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_H_SATA_DEVICE_ID_D_AHCI) || \
      (DeviceId == V_PCH_H_SATA_DEVICE_ID_US_AHCI) || \
      (DeviceId == V_PCH_H_SATA2_DEVICE_ID_US_AHCI) \
    )

#define IS_PCH_LP_SATA_AHCI_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_LP_SATA_DEVICE_ID_M_AHCI) \
    )

#define IS_PCH_SATA_AHCI_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_SATA_AHCI_DEVICE_ID (DeviceId) || \
      IS_PCH_LP_SATA_AHCI_DEVICE_ID (DeviceId) \
    )


//
//  SATA RAID Device ID macros
//
#define IS_PCH_H_SATA_RAID_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_H_SATA_DEVICE_ID_D_RAID) || \
      (DeviceId == V_PCH_H_SATA_DEVICE_ID_D_RAID_PREM) || \
      (DeviceId == V_PCH_H_SATA_DEVICE_ID_D_RAID_ALTDIS) || \
      (DeviceId == V_PCH_H_SATA_DEVICE_ID_D_RAID_RSTE) || \
      (DeviceId == V_PCH_H_SATA_DEVICE_ID_D_RAID_RRT) \
    )


#define IS_PCH_LP_SATA_RAID_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_LP_SATA_DEVICE_ID_M_RAID) || \
      (DeviceId == V_PCH_LP_SATA_DEVICE_ID_M_RAID_ALTDIS) || \
      (DeviceId == V_PCH_LP_SATA_DEVICE_ID_M_RAID_PREM) || \
      (DeviceId == V_PCH_LP_SATA_DEVICE_ID_M_RAID_RRT) \
    )

#define IS_PCH_SATA_RAID_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_SATA_RAID_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_SATA_RAID_DEVICE_ID(DeviceId) \
    )

//
//  Combined SATA IDE/AHCI/RAID Device ID macros
//
#define IS_PCH_H_SATA_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_SATA_AHCI_DEVICE_ID (DeviceId) || \
      IS_PCH_H_SATA_RAID_DEVICE_ID (DeviceId) \
    )

#define IS_PCH_LP_SATA_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_LP_SATA_AHCI_DEVICE_ID (DeviceId) || \
      IS_PCH_LP_SATA_RAID_DEVICE_ID (DeviceId) \
    )
#define IS_PCH_SATA_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_SATA_DEVICE_ID (DeviceId) || \
      IS_PCH_LP_SATA_DEVICE_ID (DeviceId) \
    )

#define IS_PCH_H_RAID_AVAILABLE(DeviceId) (TRUE)
#define IS_PCH_LP_RAID_AVAILABLE(DeviceId) (TRUE)

#define IS_PCH_RAID_AVAILABLE(DeviceId) \
    ( \
      IS_PCH_H_RAID_AVAILABLE(DeviceId) || \
      IS_PCH_LP_RAID_AVAILABLE(DeviceId) \
    )

//
//  SPI Device ID macros
//
#define IS_PCH_H_SPI_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_H_SPI_DEVICE_ID) || \
      FALSE \
    )

#define IS_PCH_LP_SPI_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_LP_SPI_DEVICE_ID) || \
      FALSE \
    )

#define IS_PCH_SPI_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_SPI_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_SPI_DEVICE_ID(DeviceId) \
    )

//
//  USB Device ID macros
//
#define IS_PCH_H_USB_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_H_USB_DEVICE_ID_XHCI_1) \
    )

#define IS_PCH_LP_USB_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_LP_USB_DEVICE_ID_XHCI_1) \
    )

#define IS_PCH_USB_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_USB_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_USB_DEVICE_ID(DeviceId) \
    )

//
//  PCIE Device ID macros
//
#define IS_PCH_H_PCIE_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT1) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT2) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT3) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT4) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT5) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT6) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT7) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT8) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT9) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT10) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT11) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT12) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT13) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT14) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT15) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT16) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT17) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT18) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT19) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_PORT20) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_MB_SUBD) || \
      (DeviceId == V_PCH_H_PCIE_DEVICE_ID_DT_SUBD) \
    )

#define IS_PCH_LP_PCIE_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT1) || \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT2) || \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT3) || \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT4) || \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT5) || \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT6) || \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT7) || \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT8) || \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT9) || \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT10) || \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT11) || \
      (DeviceId == V_PCH_LP_PCIE_DEVICE_ID_PORT12) \
    )

#define IS_PCH_PCIE_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_PCIE_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_PCIE_DEVICE_ID(DeviceId) \
    )

//
//  HD Audio Device ID macros
//
#define IS_PCH_H_HDA_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_H_HDA_DEVICE_ID_0) || \
      (DeviceId == V_PCH_H_HDA_DEVICE_ID_1) || \
      (DeviceId == V_PCH_H_HDA_DEVICE_ID_2) || \
      (DeviceId == V_PCH_H_HDA_DEVICE_ID_3) || \
      (DeviceId == V_PCH_H_HDA_DEVICE_ID_4) || \
      (DeviceId == V_PCH_H_HDA_DEVICE_ID_5) || \
      (DeviceId == V_PCH_H_HDA_DEVICE_ID_6) || \
      (DeviceId == V_PCH_H_HDA_DEVICE_ID_7) \
    )

#define IS_PCH_LP_HDA_DEVICE_ID(DeviceId) \
    ( \
      (DeviceId == V_PCH_LP_HDA_DEVICE_ID_0) || \
      (DeviceId == V_PCH_LP_HDA_DEVICE_ID_1) || \
      (DeviceId == V_PCH_LP_HDA_DEVICE_ID_2) || \
      (DeviceId == V_PCH_LP_HDA_DEVICE_ID_3) || \
      (DeviceId == V_PCH_LP_HDA_DEVICE_ID_4) || \
      (DeviceId == V_PCH_LP_HDA_DEVICE_ID_5) || \
      (DeviceId == V_PCH_LP_HDA_DEVICE_ID_6) || \
      (DeviceId == V_PCH_LP_HDA_DEVICE_ID_7) \
    )

#define IS_PCH_HDA_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_HDA_DEVICE_ID(DeviceId) || \
      IS_PCH_LP_HDA_DEVICE_ID(DeviceId) \
    )

///
/// Any device ID that is PCH-H
///
#define IS_PCH_H_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_H_LPC_DEVICE_ID (DeviceId) || \
      IS_PCH_H_SATA_DEVICE_ID (DeviceId) || \
      IS_PCH_H_USB_DEVICE_ID (DeviceId) || \
      IS_PCH_H_PCIE_DEVICE_ID (DeviceId) || \
      IS_PCH_H_SPI_DEVICE_ID (DeviceId) || \
      IS_PCH_H_HDA_DEVICE_ID (DeviceId) || \
      (DeviceId) == V_PCH_H_THERMAL_DEVICE_ID || \
      (DeviceId) == V_PCH_H_SMBUS_DEVICE_ID || \
      (DeviceId) == V_PCH_H_LAN_DEVICE_ID \
    )

///
/// Any device ID that is PCH-Lp
///
#define IS_PCH_LP_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_LP_LPC_DEVICE_ID (DeviceId) || \
      IS_PCH_LP_SATA_DEVICE_ID (DeviceId) || \
      IS_PCH_LP_USB_DEVICE_ID (DeviceId) || \
      IS_PCH_LP_PCIE_DEVICE_ID (DeviceId) || \
      IS_PCH_LP_HDA_DEVICE_ID (DeviceId) || \
      (DeviceId == V_PCH_LP_THERMAL_DEVICE_ID) || \
      (DeviceId == V_PCH_LP_SMBUS_DEVICE_ID) || \
      (DeviceId == V_PCH_LP_SPI_DEVICE_ID) || \
      (DeviceId == V_PCH_LP_LAN_DEVICE_ID) || \
      (DeviceId == V_PCH_LP_SERIAL_IO_DMA_DEVICE_ID) || \
      (DeviceId == V_PCH_LP_SERIAL_IO_I2C0_DEVICE_ID) || \
      (DeviceId == V_PCH_LP_SERIAL_IO_I2C1_DEVICE_ID) || \
      (DeviceId == V_PCH_LP_SERIAL_IO_SPI0_DEVICE_ID) || \
      (DeviceId == V_PCH_LP_SERIAL_IO_SPI1_DEVICE_ID) || \
      (DeviceId == V_PCH_LP_SERIAL_IO_UART0_DEVICE_ID) || \
      (DeviceId == V_PCH_LP_SERIAL_IO_UART1_DEVICE_ID ) || \
      (DeviceId == V_PCH_LP_SERIAL_IO_SDIO_DEVICE_ID) \
    )

///
/// Combined any device ID that is PCH-H or PCH-LP
///
#define IS_PCH_DEVICE_ID(DeviceId) \
    (\
     IS_PCH_H_DEVICE_ID(DeviceId) || \
     IS_PCH_LP_DEVICE_ID(DeviceId) \
    )

/** 
  PCH PCR boot script accessing macro
  Those macros are only available for DXE phase.
**/ 
#define PCH_PCR_BOOT_SCRIPT_WRITE(Width, Pid, Offset, Count, Buffer) \
          S3BootScriptSaveMemWrite (Width, PCH_PCR_ADDRESS (Pid, Offset), Count, Buffer); \
          S3BootScriptSaveMemPoll (Width, PCH_PCR_ADDRESS (Pid, Offset), Buffer, Buffer, 1, 1);

#define PCH_PCR_BOOT_SCRIPT_READ_WRITE(Width, Pid, Offset, DataOr, DataAnd) \
          S3BootScriptSaveMemReadWrite (Width, PCH_PCR_ADDRESS (Pid, Offset), DataOr, DataAnd); \
          S3BootScriptSaveMemPoll (Width, PCH_PCR_ADDRESS (Pid, Offset), DataOr, DataOr, 1, 1);

#endif

