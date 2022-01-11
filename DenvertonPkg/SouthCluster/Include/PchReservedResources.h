/** @file
  PCH preserved MMIO resource definitions.

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
#ifndef _PCH_PRESERVED_RESOURCES_H_
#define _PCH_PRESERVED_RESOURCES_H_

 /** PCH preserved MMIO range, 24 MB, from 0xFA000000 to 0xFE7FFFFF

  Detailed recommended static allocation
  +-------------------------------------------------------------------------+
  | Size    | Start       | End         | Usage                             |
  | 16 MB   | 0xFD000000  | 0xFDFFFFFF  | SBREG                             |
  | 64 KB   | 0xFE000000  | 0xFE00FFFF  | PMC MBAR                          |
  | 4 KB    | 0xFE010000  | 0xFE010FFF  | SPI BAR0                          |
  | 88 KB   | 0xFE020000  | 0xFE035FFF  | SerialIo BAR in ACPI mode         | 
  | 24 KB   | 0xFE036000  | 0xFE03BFFF  | Unused                            |
  | 4 KB    | 0xFE03C000  | 0xFE03CFFF  | Thermal Device in ACPI mode       |
  | 524 KB  | 0xFE03D000  | 0xFE0BFFFF  | Unused                            |
  | 1 MB    | 0xFA000000  | 0xFA0FFFFF  | TraceHub MTB BAR                  |
  | 2 KB    | 0xFA100000  | 0xFA1007FF  | TraceHub RTIT BAR                 |
  | 8 MB    | 0xFB000000  | 0xFB7FFFFF  | TraceHub SW BAR                   |
  | 8 MB    | 0xFB800000  | 0xFBFFFFFF  | TraceHub FW BAR                   |
  | 64 KB   | 0xFE400000  | 0xFE40FFFF  | CIO2 MMIO BAR in ACPI mode        |
  | 2 MB - 64KB | 0xFE410000  | 0xFE5FFFFF  | Unused                        |
  | 2 MB    | 0xFE600000  | 0xFE7FFFFF  | Temp address                      |
  +-------------------------------------------------------------------------+
  **/
#define PCH_PRESERVED_BASE_ADDRESS      0xFD000000     ///< Pch preserved MMIO base address
#define PCH_PRESERVED_MMIO_SIZE         0x01800000     ///< 24MB
#define PCH_PCR_BASE_ADDRESS            0xFD000000     ///< SBREG MMIO base address
#define PCH_PCR_MMIO_SIZE               0x01000000     ///< 16MB
#define PCH_PWRM_BASE_ADDRESS           0xFE000000     ///< PMC MBAR MMIO base address
#define PCH_PWRM_MMIO_SIZE              0x00010000     ///< 64KB
#define PCH_SPI_BASE_ADDRESS            0xFE010000     ///< SPI BAR0 MMIO base address
#define PCH_SPI_MMIO_SIZE               0x00001000     ///< 4KB
#define PCH_SERIAL_IO_BASE_ADDRESS      0xFE020000     ///< SerialIo MMIO base address
#define PCH_SERIAL_IO_MMIO_SIZE         0x00016000     ///< 88KB
#define PCH_THERMAL_BASE_ADDRESS        0xFE03C000     ///< Thermal Device in ACPI mode
#define PCH_THERMAL_MMIO_SIZE           0x00001000     ///< 4KB
#define PCH_TRACE_HUB_MTB_BASE_ADDRESS  0xFA000000     ///< TraceHub MTB MMIO base address
#define PCH_TRACE_HUB_MTB_MMIO_SIZE     0x00100000     ///< 1MB
#define PCH_TRACE_HUB_RTIT_BASE_ADDRESS 0xFA100000     ///< TraceHub RTIT MMIO base address
#define PCH_TRACE_HUB_RTIT_MMIO_SIZE    0x00000800     ///< 2KB
#define PCH_TRACE_HUB_SW_BASE_ADDRESS   0xFB000000     ///< TraceHub SW MMIO base address
#define PCH_TRACE_HUB_SW_MMIO_SIZE      0x00800000     ///< 8MB
#define PCH_TRACE_HUB_FW_BASE_ADDRESS   0xFB800000     ///< TraceHub FW MMIO base address
#define PCH_TRACE_HUB_FW_MMIO_SIZE      0x00800000     ///< 8MB
#define PCH_TEMP_BASE_ADDRESS           0xFE600000     ///< preserved temp address for misc usage
#define PCH_TEMP_MMIO_SIZE              0x00200000     ///< 2MB

#endif // _PCH_PRESERVED_RESOURCES_H_

