/** @file
  Build time limits of PCH resources.

@copyright
 Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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
#ifndef _PCH_LIMITS_H_
#define _PCH_LIMITS_H_

//
// PCIe limits
//
#define PCH_MAX_PCIE_CLUSTERS               2
#define PCIE_MAX_VIRTUAL_ROOT_PORTS          3
#define MAX_NUMBER_OF_ROOT_PORTS_PER_CONTROLLER 4
#define PCH_MAX_PCIE_ROOT_PORTS             PCH_H_PCIE_MAX_ROOT_PORTS
#define PCH_H_PCIE_MAX_ROOT_PORTS           8

#define PCH_LP_PCIE_MAX_ROOT_PORTS          12

#define PCH_MAX_PCIE_CONTROLLERS            PCH_H_PCIE_MAX_CONTROLLERS
#define PCH_PCIE_CONTROLLER_PORTS           4
#define PCH_H_PCIE_MAX_CONTROLLERS          (PCH_H_PCIE_MAX_ROOT_PORTS / PCH_PCIE_CONTROLLER_PORTS)
#define PCH_LP_PCIE_MAX_CONTROLLERS         (PCH_LP_PCIE_MAX_ROOT_PORTS / PCH_PCIE_CONTROLLER_PORTS)

//
// PCIe clocks limits
//
#define PCH_MAX_PCIE_CLOCKS                 PCH_H_PCIE_MAX_ROOT_PORTS
#define PCH_LP_PCIE_MAX_CLK_REQ             6
#define PCH_H_PCIE_MAX_CLK_REQ              16

//
// RST PCIe Storage Cycle Router limits
//
#define PCH_MAX_RST_PCIE_STORAGE_CR         3

//
// SATA limits
//
#define PCH_MAX_SATA_PORTS                  PCH_H_AHCI_MAX_PORTS
#define PCH_H_AHCI_MAX_PORTS                8       ///< Max number of sata ports in SKL PCH H
#define PCH_LP_AHCI_MAX_PORTS               3       ///< Max number of sata ports in SKL PCH LP
#define PCH_SATA_MAX_DEVICES_PER_PORT       1       ///< Max support device numner per port, Port Multiplier is not support.

//
// USB limits
//
#define PCH_MAX_USB2_PORTS                  PCH_H_XHCI_MAX_USB2_PORTS

#define PCH_H_XHCI_MAX_USB2_PHYSICAL_PORTS  4      ///< Max Physical Connector XHCI, not counting virtual ports like USB-R.
#define PCH_LP_XHCI_MAX_USB2_PHYSICAL_PORTS 10      ///< Max Physical Connector XHCI, not counting virtual ports like USB-R.

#define PCH_H_XHCI_MAX_USB2_PORTS           4      ///< 14 High Speed lanes + Including two ports reserved for USBr
#define PCH_LP_XHCI_MAX_USB2_PORTS          12      ///< 10 High Speed lanes + Including two ports reserved for USBr

#define PCH_MAX_USB3_PORTS                  PCH_H_XHCI_MAX_USB3_PORTS

#define PCH_H_XHCI_MAX_USB3_PORTS           4      ///< 10 Super Speed lanes
#define PCH_LP_XHCI_MAX_USB3_PORTS          6       ///< 6 Super Speed lanes

#define PCH_XHCI_MAX_SSIC_PORT_COUNT        2       ///< 2 SSIC ports in SKL PCH-LP and SKL PCH-H

//
// SerialIo limits
//
#define PCH_SERIALIO_MAX_CONTROLLERS         11  ///< Number of SerialIo controllers, this includes I2C, SPI and UART
#define PCH_SERIALIO_MAX_I2C_CONTROLLERS      6  ///< Number of SerialIo I2C controllers
#define PCH_LP_SERIALIO_MAX_I2C_CONTROLLERS   6  ///< Number of SerialIo I2C controllers for PCH-LP
#define PCH_H_SERIALIO_MAX_I2C_CONTROLLERS    4  ///< Number of SerialIo I2C controllers for PCH-H
#define PCH_SERIALIO_MAX_SPI_CONTROLLERS      2  ///< Number of SerialIo SPI controllers
#define PCH_SERIALIO_MAX_UART_CONTROLLERS     4  ///< Number of SerialIo UART controllers
#define PCH_MAX_HSUART                        3

//
// ISH limits
//
#define PCH_ISH_MAX_GP_PINS                   8
#define PCH_ISH_MAX_UART_CONTROLLERS          2
#define PCH_ISH_MAX_I2C_CONTROLLERS           3
#define PCH_ISH_MAX_SPI_CONTROLLERS           1

//
// SCS limits
//
#define PCH_SCS_MAX_CONTROLLERS            3  ///< Number of Storage and Communication Subsystem controllers, this includes eMMC, SDIO, SDCARD

//
// Flash Protection Range Register
//
#define PCH_FLASH_PROTECTED_RANGES         5

//
// Number of eSPI slaves
//
#define PCH_ESPI_MAX_SLAVE_ID              2
#endif // _PCH_LIMITS_H_
