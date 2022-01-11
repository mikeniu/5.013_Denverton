/** @file
  Header file for IqatIp registers.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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

#ifndef _IQAT_IP_REGS_H_
#define _IQAT_IP_REGS_H_

#define IQAT_SECURE_RAM_BASE_ADDRESS_HIGH_MASK   (0xFFFE << 16)   // IQAT Secure RAM Valid Address Range - Bits[48:0]

/* MCH/IMR Macros */
#define IMR_LOW         0x6870 // IMR0 offsets
#define IMR_HIGH        0x6874
#define IMR_CP          0x6878
#define IMR_RAC         0x6880
#define IMR_WAC         0x6888
#define IMR_BASE_SHIFT  10
#define IMR_IA_SMM_ACCESS      0x04
#define IMR_BOOT_BIOS_ACCESS   0x10
#define IMR_IQAT_ACCESS        BIT26

/* VRP2 Macros */
#define VRP2_DEVICE_NUM  0x6
#define VRP2_FUNC_NUM    0
#define VRP2_BUS_OFFSET  0x18
#define PCICMD_OFFSET    0x04
#define PCICMD_MSE       0x02
#define PCICMD_WR_EN     0x06
#define PCI_BAR_MEM_MASK 0x0000000F
#define GET_SEC_BUS(B) (((B) >> 8) & 0xFF)

/* QAT Macros */
#define IQAT_RESERVE_MEM_SIZE  0x80000
#define IQAT_RESERVE_MEM_ALIGN 19
#define IQAT_DEVICE_NUM  0
#define IQAT_FUNC_NUM    0
#define PMISCBAR_OFFSET  0x18
#define RAM_BASE_SHIFT   32
#define RAM_BASE_ADDR_LO 0x308D8
#define RAM_BASE_ADDR_HI 0x308D4
#define BIT_IS_SET(REG,BIT) (((REG) & (BIT)) == (BIT))
#define FUSECTL_REG_OFFSET  0x40
#define FUSECTL_VALUE_SKU_1 0x0       // Default, all on
#define FUSECTL_VALUE_SKU_2 0x800000  // Compression SKU, Bit 23: PGCOM
#define FUSECTL_VALUE_SKU_3 0x1000000 // PKE SKU, Bit 24: PGPKE
#define FUSECTL_VALUE_SKU_4 0x6003C   // Low Perf SKU, Bits disable_MEs[2:5] and disable_SSMs[1:2]
#define PPDCNTL_REG_OFFSET  0x7C      // PCI Express Device Control Register
#define PPDCNTL_MRR_64B     (BIT12 | BIT13 | BIT14) // MRR: Bits[14:12]
#define PPDCNTL_MPL_64B     (BIT5 | BIT6 | BIT7) // MPL: Bits[7:5]

#define R_IQAT_PPAERUCS             0x104
#define B_IQAT_PPAERUCS_IEUNC       BIT22
#define R_IQAT_PPAERCS              0x110
#define B_IQAT_PPAERCS_ANFES        BIT13
#define R_IQAT_PFIEERRUNCSTSR       0x280
#define B_IQAT_PFIEERRUNCSTSR_EPPPE BIT11
#define B_IQAT_PFIEERRUNCSTSR_OCDE  BIT10

#endif // _IQAT_IP_REGS_H_
