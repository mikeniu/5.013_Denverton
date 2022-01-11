/** @file
  Register names for System Agent (SA) registers
  <b>Conventions</b>:
  - Prefixes:
    - Definitions beginning with "R_" are registers
    - Definitions beginning with "B_" are bits within registers
    - Definitions beginning with "V_" are meaningful values of bits within the registers
    - Definitions beginning with "S_" are register sizes
    - Definitions beginning with "N_" are the bit position
  - In general, SA registers are denoted by "_SA_" in register names
  - Registers / bits that are different between SA generations are denoted by
    "_SA_[generation_name]_" in register/bit names. e.g., "_SA_DNV_"
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SA generation will be just named
    as "_SA_" without [generation_name] inserted.

@copyright
  Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#ifndef _SA_REGS_H_
#define _SA_REGS_H_

/*
< Extended Configuration Base Address.*/

//DNV do not open MCR/MDR interface for side-band message. use MCHBAR instead.
//
//Ranges defined below is following PND2_Cunit_HAS.
//
#define MCHBAR_RANGE_DUNIT0            0x0000
#define MCHBAR_RANGE_DUNIT1            0x0200
#define MCHBAR_RANGE_DUNIT2            0x0400
#define MCHBAR_RANGE_DUNIT3            0x0600
#define MCHBAR_RANGE_DUNIT4            0x0800
#define MCHBAR_RANGE_DUNIT5            0x0A00
#define MCHBAR_RANGE_DUNIT6            0x0C00
#define MCHBAR_RANGE_DUNIT7            0x0E00
#define MCHBAR_RANGE_DUNIT8            0x1000
#define MCHBAR_RANGE_DUNIT9            0x1200
#define MCHBAR_RANGE_DUNIT10           0x1400
#define MCHBAR_RANGE_DUNIT11           0x1600
#define MCHBAR_RANGE_CUNIT             0x6000
#define MCHBAR_RANGE_AUNIT             0x6400
#define MCHBAR_RANGE_BUNIT             0x6800
#define MCHBAR_RANGE_PUNIT             0x7000
#define MCHBAR_RANGE_TUNIT             0x7800
#define BUNIT_BMISC_MCHBAR_OFFSET      0x6800

//
// DEVICE 0 (Memory Controller Hub)
//
#define SA_MC_BUS                      0x00
#define SA_MC_DEV                      0x00
#define SA_MC_FUN                      0x00
#define R_SA_MC_VENDOR_ID              0x00
#define R_SA_MC_DEVICE_ID              0x02
#define R_SA_MC_REVISION_ID            0x08
#define V_SA_MC_VID                    0x8086
#define V_SA_MC_DID0                   0x0AF0  //Broxton
#define V_SA_MC_DID1                   0x1AF0  //Broxton Pro
#define V_SA_MC_DID2                   0x4AF0  //Broxton Pro2
#define V_SA_MC_DID3                   0x5AF0  //Broxton-P 

#define R_SA_MC_CAPID0_A               0xE4
#define R_SA_MC_CAPID0_B               0xE8  

//
// Silicon Steppings
//
#define V_SA_MC_RID_0                  0x00




/**
 <b>Description</b>:
 - This is the base address for the Host Memory Mapped Configuration space.  There is no physical memory within this 32KB window that can be addressed.  The 32KB reserved by this register does not alias to any PCI 2.3 compliant memory mapped space.  On reset, the Host MMIO Memory Mapped Configuation space is disabled and must be enabled by writing a 1 to MCHBAREN [Dev 0, offset48h, bit 0].
 - All the bits in this register are locked in LT mode.
 - The register space contains memory control, initialization, timing, and buffer strength registers; clocking registers; and power and thermal management registers.
**/
#define R_SA_MCHBAR     (0x48)
#define R_SA_MCHBAR_LO  R_SA_MCHBAR
#define R_SA_MCHBAR_HI  (R_SA_MCHBAR_LO + 4)
/**
 Description of MCHBAREN (0:0)
 - 0: MCHBAR is disabled and does not claim any memory
 - 1: MCHBAR memory mapped accesses are claimed and decoded appropriately
 - This register is locked by LT.
**/
#define N_SA_MCHBAR_MCHBAREN_OFFSET  (0x0)
#define S_SA_MCHBAR_MCHBAREN_WIDTH   (0x1)
#define B_SA_MCHBAR_MCHBAREN_MASK    (0x1)
#define V_SA_MCHBAR_MCHBAREN_DEFAULT (0x0)
/**
 Description of MCHBAR (15:38)
 - This field corresponds to bits 38 to 15 of the base address Host Memory Mapped configuration space.  BIOS will program this register resulting in a base address for a 32KB block of contiguous memory address space.  This register ensures that a naturally aligned 32KB space is allocated within the first 512GB of addressable memory space. System Software uses this base address to program the Host Memory Mapped register set. All the bits in this register are locked in LT mode.
**/
#define N_SA_MCHBAR_MCHBAR_OFFSET  (0xf)
#define S_SA_MCHBAR_MCHBAR_WIDTH   (0x18)
#define B_SA_MCHBAR_MCHBAR_MASK    (0x7fffff8000ULL)
#define V_SA_MCHBAR_MCHBAR_DEFAULT (0x0)

/**
 <b>Description</b>:
 - GMCH Graphics Control Register
**/
#define R_SA_GGC (0x50)
/**
 Description of GGCLCK (0:0)
 - When set to 1b, this bit will lock all bits in this register.
**/
#define N_SA_GGC_GGCLCK_OFFSET   (0x0)
#define S_SA_GGC_GGCLCK_WIDTH    (0x1)
#define B_SA_GGC_GGCLCK_MASK     (0x1)
#define V_SA_GGC_GGCLCK_DEFAULT  (0x0)
/**
 Description of IVD (1:1)
 - 0: Enable.  Device 2 (IGD) claims VGA memory and IO cycles, the Sub-Class Code within Device 2 Class Code register is 00.
 - 1: Disable.  Device 2 (IGD) does not claim VGA cycles (Mem and IO), and the Sub- Class Code field within Device 2 function 0 Class Code register is 80.
 - BIOS Requirement:  BIOS must not set this bit to 0 if the GMS field (bits 7:3 of this register) pre-allocates no memory.
 - This bit MUST be set to 1 if Device 2 is disabled disabled DEVEN_0_0_0_PCI.D2F0EN 0.
**/
#define N_SA_GGC_IVD_OFFSET      (0x1)
#define S_SA_GGC_IVD_WIDTH       (0x1)
#define B_SA_GGC_IVD_MASK        (0x2)
#define V_SA_GGC_IVD_DEFAULT     (0x0)
/**
 Description of VAMEM (2:2) Enables the use of the iGFX enbines for Versatile Acceleration.  
 - 1  iGFX engines are in Versatile Acceleration Mode. Device 2 Class Code is 048000h.  
 - 0  iGFX engines are in iGFX Mode. Device 2 Class Code is 030000h. 
**/
#define N_SA_GGC_VAMEM_OFFSET    (0x2)
#define S_SA_GGC_VAMEM_WIDTH     (0x1)
#define B_SA_GGC_VAMEM_MASK      (0x4)
#define V_SA_GGC_VAMEM_DEFAULT   (0x0)
/**
 Description of RSVD0 (5:3)
**/
#define N_SA_GCC_RSVD0_OFFSET    (0x3)
#define S_SA_GCC_RSVD0_WIDTH     (0x3)
#define B_SA_GCC_RSVD0_MASK      (0x38)
#define V_SA_GCC_RSVD0_DEFAULT   (0x0)
/**
  Description of GGMS (7:6)
 - This field is used to select the amount of Main Memory that is pre-allocated to support the Internal Graphics Translation Table.  The BIOS ensures that memory is pre-allocated only when Internal graphics is enabled.
 - GSM is assumed to be a contiguous physical DRAM space with DSM, and BIOS needs to allocate a contiguous memory chunk.  Hardware will derive the base of GSM from DSM only using the GSM size programmed in the register.
 - Valid options:
 - 0h: 0 MB of memory pre-allocated for GTT.
 - 1h: 2 MB of memory pre-allocated for GTT.
 - 2h: 4 MB of memory pre-allocated for GTT.
 - 3h: 8 MB of memory pre-allocated for GTT.
**/
#define N_SA_GGC_GGMS_OFFSET  (0x6)
#define S_SA_GGC_GGMS_WIDTH   (0x2)
#define B_SA_GGC_GGMS_MASK    (0xc0)
#define V_SA_GGC_GGMS_DEFAULT (0x0)
#define V_SA_GGC_GGMS_DIS     0
#define V_SA_GGC_GGMS_2MB     1
#define V_SA_GGC_GGMS_4MB     2
#define V_SA_GGC_GGMS_8MB     3
/**
 Description of GMS (15:8)
 - This field is used to select the amount of Main Memory that is pre-allocated to support the Internal Graphics device in VGA (non-linear) and Native (linear) modes.  The BIOS ensures that memory is pre-allocated only when Internal graphics is enabled.
 - This register is also LT lockable.
 - Valid options are 0 (0x0) to 2016MB (0x3F) in multiples of 32 MB
 - Default is 32MB
 - All other values are reserved
 - Hardware does not clear or set any of these bits automatically based on IGD being disabled/enabled.
 - BIOS Requirement: BIOS must not set this field to 0h if IVD (bit 1 of this register) is 0.
**/
#define N_SA_GGC_GMS_OFFSET  (0x8)
#define S_SA_GGC_GMS_WIDTH   (0x8)
#define B_SA_GGC_GMS_MASK    (0xff00)
#define V_SA_GGC_GMS_DEFAULT (0x01)
#define V_SA_GGC_GMS_DIS     0
#define V_SA_GGC_GMS_32MB    1
#define V_SA_GGC_GMS_64MB    2
#define V_SA_GGC_GMS_96MB    3
#define V_SA_GGC_GMS_128MB   4
#define V_SA_GGC_GMS_160MB   5
#define V_SA_GGC_GMS_192MB   6
#define V_SA_GGC_GMS_224MB   7
#define V_SA_GGC_GMS_256MB   8
#define V_SA_GGC_GMS_288MB   9
#define V_SA_GGC_GMS_320MB   0x0a
#define V_SA_GGC_GMS_352MB   0x0b
#define V_SA_GGC_GMS_384MB   0x0c
#define V_SA_GGC_GMS_416MB   0x0d
#define V_SA_GGC_GMS_448MB   0x0e
#define V_SA_GGC_GMS_480MB   0x0f
#define V_SA_GGC_GMS_512MB   0x10
#define V_SA_GGC_GMS_1024MB  0x20
#define V_SA_GGC_GMS_2016MB  0x3F
/**
 Description of RSVD1 (31:16)
**/
#define N_SA_GCC_RSVD1_OFFSET    (0x10)
#define S_SA_GCC_RSVD1_WIDTH     (0xff)
#define B_SA_GCC_RSVD1_MASK      (0xffff0000)
#define V_SA_GCC_RSVD1_DEFAULT   (0x0)


/**
 Description:
 - Allows for enabling/disabling of PCI devices and functions that are within the CPU package. The table below the bit definitions describes the behavior of all combinations of transactions to devices controlled by this register.
  All the bits in this register are LT Lockable.
**/
#define R_SA_DEVEN (0x54)
/**
 Description of D0F0EN (0:0)
 - Bus 0 Device 0 Function 0 may not be disabled and is therefore hardwired to 1.
**/
#define N_SA_DEVEN_D0F0EN_OFFSET   (0x0)
#define S_SA_DEVEN_D0F0EN_WIDTH    (0x1)
#define B_SA_DEVEN_D0F0EN_MASK     (0x1)
#define V_SA_DEVEN_D0F0EN_DEFAULT  (0x1)
/**
 Description of D0F1EN (1:1)
 - 0: Bus 0 Device 1 Function 2 is disabled and hidden.
 - 1: Bus 0 Device 1 Function 2 is enabled and visible.
 - This bit will remain 0 if PEG12 capability is disabled.
**/
#define N_SA_DEVEN_D0F1EN_OFFSET   (0x1)
#define S_SA_DEVEN_D0F1EN_WIDTH    (0x1)
#define B_SA_DEVEN_D0F1EN_MASK     (0x2)
#define V_SA_DEVEN_D0F1EN_DEFAULT  (0x2)
/**
 Description of RSVD0 (2:2)
**/
#define N_SA_DEVEN_RSVD0_OFFSET    (0x2)
#define S_SA_DEVEN_RSVD0_WIDTH     (0x1)
#define B_SA_DEVEN_RSVD0_MASK      (0x4)
#define V_SA_DEVEN_RSVD0_DEFAULT   (0x0)
/**
 Description of RSVD (3:3)
**/
#define N_SA_DEVEN_RSVD_OFFSET     (0x3)
#define S_SA_DEVEN_RSVD_WIDTH      (0x1)
#define B_SA_DEVEN_RSVD_MASK       (0x8)
#define V_SA_DEVEN_RSVD_DEFAULT    (0x0)
/**
 Description of D2F0EN (4:4)
 - 0:  Bus 0 Device 2 is disabled and hidden
 - 1:  Bus 0 Device 2 is enabled and visible
 - This bit will remain 0 if Device 2 capability is disabled.
**/
#define N_SA_DEVEN_D2F0EN_OFFSET   (0x4)
#define S_SA_DEVEN_D2F0EN_WIDTH    (0x1)
#define B_SA_DEVEN_D2F0EN_MASK     (0x10)
#define V_SA_DEVEN_D2F0EN_DEFAULT  (0x10)
/**
 Description of D3F0EN (5:5)
 - 0:  Bus 0 Device 3 is disabled and hidden
 - 1:  Bus 0 Device 3 is enabled and visible
 - This bit will remain 0 if Device 3 capability is disabled.
**/
#define N_SA_DEVEN_D3F0EN_OFFSET   (0x5)
#define S_SA_DEVEN_D3F0EN_WIDTH    (0x1)
#define B_SA_DEVEN_D3F0EN_MASK     (0x20)
#define V_SA_DEVEN_D3F0EN_DEFAULT  (0x20)
/**
 Description of RSVD1 (31:6)
**/
#define N_SA_DEVEN_RSVD1_OFFSET    (0x6)
#define S_SA_DEVEN_RSVD1_WIDTH     (0x1A)
#define B_SA_DEVEN_RSVD1_MASK      (0xFFFFFFC0)
#define V_SA_DEVEN_RSVD1_DEFAULT   (0x0)

/**
 Description
 - Protected Audio Video Path Control
 - All the bits in this register are locked by LT.  When locked the R/W bits are RO.
**/
#define R_SA_PAVPC (0x58)
/**
 Description of PCME (0:0)
 - This field enables Protected Content Memory within Graphics Stolen Memory.
 - This register is locked (becomes read-only) when PAVPLCK = 1b.
 - This register is read-only (stays at 0b) when PAVP fuse is set to "disabled"
 - 0: Protected Content Memory is disabled
 - 1: Protected Content Memory is enabled
**/
#define N_SA_PAVPC_PCME_OFFSET   (0x0)
#define S_SA_PAVPC_PCME_WIDTH    (0x1)
#define B_SA_PAVPC_PCME_MASK     (0x1)
#define V_SA_PAVPC_PCME_MASK     (0x0)
/**
 Description of PAVPE (1:1)
 - 0: PAVP path is disabled
 - 1: PAVP path is enabled
 - This register is locked (becomes read-only) when PAVPLCK = 1b
 - This register is read-only (stays at 0b) when PAVP capability is set to "disabled" as defined by CAPID0_B[PAVPE].
**/
#define N_SA_PAVPC_PAVPE_OFFSET  (0x1)
#define S_SA_PAVPC_PAVPE_WIDTH   (0x1)
#define B_SA_PAVPC_PAVPE_MASK    (0x2)
#define V_SA_PAVPC_PAVPE_DEFAULT (0x0)
/**
 Description of PAVPLCK (2:2)
 - This bit will lock all writeable contents in this register when set (including itself).
 - This bit will be locked if PAVP is fused off.
**/
#define N_SA_PAVPC_PAVPLCK_OFFSET  (0x2)
#define S_SA_PAVPC_PAVPLCK_WIDTH   (0x1)
#define B_SA_PAVPC_PAVPLCK_MASK    (0x4)
#define V_SA_PAVPC_PAVPLCK_DEFAULT (0x0)
/**
 Description of HVYMODSEL (3:3)
 - 1b   Serpent Mode
 - 0b   Big PCM Mode
**/
#define N_SA_PAVPC_HVYMODSEL_OFFSET  (0x3)
#define S_SA_PAVPC_HVYMODSEL_WIDTH   (0x1)
#define B_SA_PAVPC_HVYMODSEL_MASK    (0x8)
#define V_SA_PAVPC_HVYMODSEL_DEFAULT (0x0)
/**
 Description of OVTATTACK (4:4)
 - 0b - Disable Override. Attack Terminate allowed
 - 1b - Enable Override. Attack Terminate disallowed.
**/
#define N_SA_PAVPC_OVTATTACK_OFFSET  (0x4)
#define S_SA_PAVPC_OVTATTACK_WIDTH   (0x1)
#define B_SA_PAVPC_OVTATTACK_MASK    (0x10)
#define V_SA_PAVPC_OVTATTACK_DEFAULT (0x0)
/**
 Description of ASMFEN  (6:6)
 - 0b - ASMF method disabled 
 - 1b - ASMF method enabled
**/
#define N_SA_PAVPC_ASMFEN_OFFSET  (0x4)
#define S_SA_PAVPC_ASMFEN_WIDTH   (0x1)
#define B_SA_PAVPC_ASMFEN_MASK    (0x10)
#define V_SA_PAVPC_ASMFEN_DEFAULT (0x0)
/**
 Description of PCMBASE (20:31)
 - This field is used to set the base of Protected Content Memory.
 - This corresponds to bits 31:20 of the system memory address range, giving a 1MB granularity. This value MUST be at least 1MB above the base and below the top of stolen memory.
 - This register is locked (becomes read-only) when PAVPE = 1b.
**/
#define N_SA_PAVPC_PCMBASE_OFFSET  (0x14)
#define S_SA_PAVPC_PCMBASE_WIDTH   (0xc)
#define B_SA_PAVPC_PCMBASE_MASK    (0xfff00000)
#define V_SA_PAVPC_PCMBASE_DEFAULT (0x0)

/**
 Description:
  This is the base address for the PCI Express configuration space.  This window of addresses contains the 4KB of configuration space for each PCI Express device that can potentially be part of the PCI Express Hierarchy associated with the Uncore.  There is no actual physical memory within this window of up to 256MB that can be addressed.  The actual size of this range is determined by a field in this register.
  Each PCI Express Hierarchy requires a PCI Express BASE register.  The Uncore supports one PCI Express Hierarchy.  The region reserved by this register does not alias to any PCI2.3 compliant memory mapped space.  For example, the range reserved for MCHBAR is outside of PCIEXBAR space.
  On reset, this register is disabled and must be enabled by writing a 1 to the enable field in this register.  This base address shall be assigned on a boundary consistent with the number of buses (defined by the length field in this register), above TOLUD and still within 39-bit addressable memory space.
  The PCI Express Base Address cannot be less than the maximum address written to the Top of physical memory register (TOLUD).  Software must guarantee that these ranges do not overlap with known ranges located above TOLUD.
  Software must ensure that the sum of the length of the enhanced configuration region + TOLUD + any other known ranges reserved above TOLUD is not greater than the 39-bit addessable limit of 512GB.  In general, system implementation and the number of PCI/PCI Express/PCI-X buses supported in the hierarchy will dictate the length of the region.
  All the bits in this register are locked in LT mode.
**/
#define R_SA_PCIEXBAR  (0x60)
/**
 Description of PCIEXBAREN (0:0)
 - 0: The PCIEXBAR register is disabled.  Memory read and write transactions proceed s if there were no PCIEXBAR register.  PCIEXBAR bits 38:26 are R/W with no functionality behind them.
 - 1: The PCIEXBAR register is enabled.  Memory read and write transactions whose address bits 38:26 match PCIEXBAR will be translated to configuration reads and writes within the Uncore.  These Translated cycles are routed as shown in the above table.
 - This register is locked by LT.
**/
#define N_SA_PCIEXBAR_PCIEXBAREN_OFFSET  (0x0)
#define S_SA_PCIEXBAR_PCIEXBAREN_WIDTH   (0x1)
#define B_SA_PCIEXBAR_PCIEXBAREN_MASK    (0x1)
#define V_SA_PCIEXBAR_PCIEXBAREN_DEFAULT (0x0)
/**
 Description of LENGTH (1:2)
 - This field describes the length of this region.
 - 00: 256MB (buses 0-255).  Bits 38:28 are decoded in the PCI Express Base Address Field.
 - 01: 128MB (buses 0-127).  Bits 38:27 are decoded in the PCI Express Base Address Field.
 - 10: 64MB (buses 0-63).  Bits 38:26 are decoded in the PCI Express Base Address Field.
 - 11: Reserved.
 - This register is locked by LT.
**/
#define N_SA_PCIEXBAR_LENGTH_OFFSET  (0x1)
#define S_SA_PCIEXBAR_LENGTH_WIDTH   (0x2)
#define B_SA_PCIEXBAR_LENGTH_MASK    (0x6)
#define V_SA_PCIEXBAR_LENGTH_DEFAULT (0x0)
/**
 Description of ADMSK64 (26:26)
  This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register.
**/
#define N_SA_PCIEXBAR_ADMSK64_OFFSET   (0x1a)
#define S_SA_PCIEXBAR_ADMSK64_WIDTH    (0x1)
#define B_SA_PCIEXBAR_ADMSK64_MASK     (0x4000000)
#define V_SA_PCIEXBAR_ADMSK64_DEFAULT  (0x0)
/**
 Description of ADMSK128 (27:27)
  This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register.
**/
#define N_SA_PCIEXBAR_ADMSK128_OFFSET  (0x1b)
#define S_SA_PCIEXBAR_ADMSK128_WIDTH   (0x1)
#define B_SA_PCIEXBAR_ADMSK128_MASK    (0x8000000)
#define V_SA_PCIEXBAR_ADMSK128_DEFAULT (0x0)
/**
 Description of PCIEXBAR (28:38)
 - This field corresponds to bits 38 to 28 of the base address for PCI Express enhanced configuration space.  BIOS will program this register resulting in a base address for a contiguous memory address space.  The size of the range is defined by bits [2:1] of this register.
 - This Base address shall be assigned on a boundary consistent with the number of buses (defined by the Length field in this register) above TOLUD and still within the 39-bit addressable memory space.  The address bits decoded depend on the length of the region defined by this register.
 - This register is locked by LT.
 - The address used to access the PCI Express configuration space for a specific device can be determined as follows:
 - PCI Express Base Address + Bus Number * 1MB + Device Number * 32KB + Function Number * 4KB
 - This address is the beginning of the 4KB space that contains both the PCI compatible configuration space and the PCI Express extended configuration space.
**/
#define N_SA_PCIEXBAR_PCIEXBAR_OFFSET  (0x1c)
#define S_SA_PCIEXBAR_PCIEXBAR_WIDTH   (0xb)
#define B_SA_PCIEXBAR_PCIEXBAR_MASK    (0x7ff0000000)
#define V_SA_PCIEXBAR_PCIEXBAR_DEFAULT (0x0)

/**
 Description:
  This register Defines the top of the upper usable DRAM range and start of the upper MMIO address range.
  Formerly defined in BMBOUND_HI. This register contains the lower 32 bits of TOUUD
**/
#define R_SA_TOUUD_LO  (0xA8)
/**
 Description of Top of Upper Usable DRAM (31:20)
 Upper usable DRAM address range ends at the preceding byte.
 Upper MMIO Range starts at this address and extends up to the maximum system addressable memory range.
 Bits 38:20 are compared with bits 38:20 of the incoming request address to determine whether the request targets the upper usable DRAM range or upper MMIO range. If Request Address[38:20]>=TOUUD[38:20] and any bit in Request Address[38:32] is set, request is determined to target the upper MMIO range.
**/
#define B_SA_TOUUD_LO_TOUUD_MASK    (0xfff00000)

/**
 Description:
  This register Defines the top of the upper usable DRAM range and start of the upper MMIO address range.
  Formerly defined in BMBOUND_HI. This register contains the lower 32 bits of TOUUD
**/
#define R_SA_TOUUD_HI  (0xAC)
/**
 Description of Top of Upper Usable DRAM (6:0)
 Upper usable DRAM address range ends at the preceding byte.
 Upper MMIO Range starts at this address and extends up to the maximum system addressable memory range.
 Bits 38:20 are compared with bits 38:20 of the incoming request address to determine whether the request targets the upper usable DRAM range or upper MMIO range. If Request Address[38:20]>=TOUUD[38:20] and any bit in Request Address[38:32] is set, request is determined to target the upper MMIO range.
**/
#define B_SA_TOUUD_HI_TOUUD_MASK    (0x0000007f)

/*
  Description:
  This register contains base of Base of Data Stolen Memory
*/
#define R_SA_BDSM (0xb0)
/*
  Description of LOCK (0:0)
  This bit will lock all writeable settings in this register, including itself.
*/
#define N_SA_BDSM_LOCK_OFFSET   (0x0)
#define S_SA_BDSM_LOCK_WIDTH    (0x1)
#define B_SA_BDSM_LOCK_MASK     (0x1)
#define V_SA_BDSM_LOCK_DEFAULT  (0x0)
/*
  Description of BDSM (20:31)
  This register contains the base address of the Data Stolen Memory. The limit for the Data Stolen Memory is TOLUD-1. This range is not decoded by the system agent, 
  but is a sub-region of BGSM decoded by the Integrated Graphics Device. Incoming Request Address[31:20] is compared against BDSM[31:20] and TOLUD[31:20] to determine if the address falls in the range. 
*/

#define N_SA_BDSM_BDSM_OFFSET    (0x14)
#define S_SA_BDSM_BDSM_WIDTH     (0xc)
#define B_SA_BDSM_BDSM_MASK      (0xfff00000)
#define V_SA_BDSM_BDSM_DEFAULT   (0x0)


/*
  Description:
  This register contains base of GTT Stolen memory
*/
#define R_SA_BGSM (0xb4)
/*
  Description of LOCK (0:0)
  This bit will lock all writeable settings in this register, including itself.
*/
#define N_SA_BGSM_LOCK_OFFSET   (0x0)
#define S_SA_BGSM_LOCK_WIDTH    (0x1)
#define B_SA_BGSM_LOCK_MASK     (0x1)
#define V_SA_BGSM_LOCK_DEFAULT  (0x1) // Denverton AptioV Override #EIP319702
/*
  Description of BGSM (20:31)
  This register contains the base address of stolen DRAM memory for the GTT. BIOS determines the base of GTT stolen memory by subtracting the GTT graphics stolen memory size PCI Device 0 offset 52 bits 11:8 from 
  the Graphics Base of Data Stolen Memory PCI Device 0 offset B0 bits 31:20.
*/
#define N_SA_BGSM_BGSM_OFFSET    (0x14)
#define S_SA_BGSM_BGSM_WIDTH     (0xc)
#define B_SA_BGSM_BGSM_MASK      (0xfff00000)
#define V_SA_BGSM_BGSM_DEFAULT   (0x0)

/*
  Description:
  This register contains base of TSEG
*/
#define R_SA_TSEG (0xb8)
///
/// Description of LOCK (0:0)
///  This bit will lock all writeable settings in this register, including itself.
///
#define N_SA_TSEGMB_LOCK_OFFSET  (0x0)
#define S_SA_TSEGMB_LOCK_WIDTH   (0x1)
#define B_SA_TSEGMB_LOCK_MASK    (0x1)
#define V_SA_TSEGMB_LOCK_DEFAULT (0x1) // Denverton AptioV Override #EIP319702
///
/// Description of TSEGMB (20:31)
///  This register contains the base address of TSEG DRAM memory. BIOS determines the base of TSEG memory which must be at or below Graphics Base of GTT Stolen Memory (PCI Device 0 Offset B4 bits 31:20).
///
#define N_SA_TSEGMB_TSEGMB_OFFSET  (0x14)
#define S_SA_TSEGMB_TSEGMB_WIDTH   (0xc)
#define B_SA_TSEGMB_TSEGMB_MASK    (0xfff00000)
#define V_SA_TSEGMB_TSEGMB_DEFAULT (0x0)

/*
 Description:
  This register contains the Top of low memory address.
*/
#define R_SA_TOLUD (0xbc)
/*
  Description of LOCK (0:0)
  This bit will lock all writeable settings in this register, including itself.
*/
#define N_SA_TOLUD_LOCK_OFFSET   (0x0)
#define S_SA_TOLUD_LOCK_WIDTH    (0x1)
#define B_SA_TOLUD_LOCK_MASK     (0x1)
#define V_SA_TOLUD_LOCK_DEFAULT  (0x0)
/*
  Description of TOLUD (20:31)
  This register contains bits 31 to 20 of an address one byte above the maximum DRAM memory below 4G that is usable by the operating system. Address bits 31 down to 20 programmed to 01h implies a minimum memory size of 1MB. Configuration software must set this value to the smaller of the following 2 choices: maximum amount memory in the system minus ME stolen memory plus one byte or the minimum address allocated for PCI memory. Address bits 19:0 are assumed to be 0_0000h for the purposes of address comparison. The Host interface positively decodes an address towards DRAM if the incoming address is less than the value programmed in this register.
  The Top of Low Usable DRAM is the lowest address above both Graphics Stolen memory and Tseg. BIOS determines the base of Graphics Stolen Memory by subtracting the Graphics Stolen Memory Size from TOLUD and further decrements by Tseg size to determine base of Tseg. All the Bits in this register are locked in LT mode.
  This register must be 1MB aligned when reclaim is enabled.
*/
#define N_SA_TOLUD_TOLUD_OFFSET    (0x14)
#define S_SA_TOLUD_TOLUD_WIDTH     (0xc)
#define B_SA_TOLUD_TOLUD_MASK      (0xfff00000)
#define V_SA_TOLUD_TOLUD_DEFAULT   (0x100000)

//
// Device 3 Equates
//
#define SA_IMGU_BUS_NUM    0x00
#define SA_IMGU_DEV_NUM    0x03
#define SA_IMGU_FUN_NUM    0x00

/* SideBand Units Port ID*/

#define DUNIT0		0x10    	/*D Unit0*/
#define CPGC0           0x11            /*CPGC0*/
#define DUNIT1		0x12    	/*D Unit1*/
#define CPGC1           0x13            /*CPGC1*/
#define DDRPHY		0x15    	/*DDR IO Unit*/
#define DUNIT_Shared	0x16    	/*DUnit Shared*/
#define CUNIT		0x41	 	/*C Unit*/
#define	FUSECONT	0x42    	/*Fuse Controller*/
#define sVID		0x44		/*VID Controller*/
#define PUNIT0		0x46		/*P Unit*/
#define PUNIT1		0x47		/*P Unit*/
#define PUNIT2		0x48		/*P Unit*/
#define PUNIT3		0x49		/*P Unit*/
#define PUNIT4		0x4A		/*P Unit*/
#define BUNIT1		0x4B		/*SSA-B Unit*/
#define BUNIT0		0x4C		/*SSA-B Unit*/
#define AUNIT0		0x4D		/*SSA-A Unit0 (IO Arbiter)*/
#define DRNG		0x4E    	/*Random Number Generator*/
#define AUNIT1		0x4F		/*SSA-A Unit1 (IO Arbiter)*/
#define TUNIT0		0x50    	/*SSA-T Unit*/
#define TUNIT1		0x51    	/*SSA-T Unit*/
#define TUNIT2		0x52    	/*SSA-T Unit*/
#define GPIOC0		0xC2		/*GPIO (North) Controller*/
#define GPIOC1		0xC5		/*GPIO (South) Controller*/
#define ICLK		0x99		/*Clock Controller*/

//
//DNV Units Registers Definition
//Register Symbol		Register Start		//Register Name
//
// SSA-AUnit (IO Aribter)  Register Offset
// 

//
// SSA-BUnit (System Memory Arbiter)  Register Offset
// 

#define R_BUNIT_BISOC				0x44
#define B_BUNIT_BISOC_SCHEDULER_LATENCY			(BIT23 | BIT22 | BIT21 |BIT20)
#define N_BUNIT_BISOC_SHEDULER_LATENCY_B			20

#define R_BUNIT_BSCHCTRL0			0x60
#define B_BUNIT_BSCHCTRL0_BEST_EFFORT_MAX_LAT     (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24)
#define N_BUNIT_BSCHCTRL0_BEST_EFFORT_MAX_LAT_B    24

#define R_BUNIT_BCOSCAT				0x4C
#define BUNIT_BCOSCAT_WIDTH			0x2

#define R_SA_MCHBAR_BMISC		   0x6800
#define	V_SA_MCHBAR_BMISC_RESDRAM  0x01   	//Bit 0 - When this bit is set, reads targeting E-segment are routed to DRAM.
#define	V_SA_MCHBAR_BMISC_RFSDRAM  0x02   	//Bit 1 - When this bit is set, reads targeting F-segment are routed to DRAM.
#define BUNIT_BSMRCP			0x2B
#define	BSMRCP_SMM_CTRL_REG_LOCK        0x00	//Dont allow any access to the register until the system is reset
#define BUNIT_BSMRRAC			0x2C
#define	BSMRRAC_SMM_WRITE_OPEN_FOR_ALL_CORE	0xFF	//Allow access only to all CPU HOST
#define	BSMRRAC_SMM_WRITE_CLOSED_FOR_IA_SMM	0x04	//Allow access only to CPU HOST IA SMM
#define BUNIT_BSMRWAC	                0x2D
#define	BSMRWAC_SMM_WRITE_OPEN_FOR_ALL_CORE	0xFF	//Allow access only to all CPU HOST
#define	BSMRWAC_SMM_WRITE_CLOSED_FOR_IA_SMM	0x04	//Allow access only to CPU HOST IA SMM
#define BUNIT_BISOCWT		0x56C
#define V_BISOCWT_ENABLE_ISOC_WEIGHTS                    (0x80000000)

//
// SSA-CUnit (Message Bus Controller)  Register Offset
// 

//
// SSA-CUnit (Message Bus Controller)  Register Offset
// Accessing by PCI Config B0D0F0
// 

//
// SSA-TUnit (CPU Bus Interface Controller)  Register Offset
// 
#define TUNIT_INTR_REDIR_CTL	0x00
#define V_MASK_INTR_REDIR_CTL_MCHBAR_FIXED 0xF8
#define V_MASK_INTR_REDIR_CTL_MCHBAR_RR    0x01
#define TUNIT_CLKGATE_CTL	0x08
#define TUNIT_VC_READ_ORDERING  0x1C
#define TUNIT_VC_WRITE_ORDERING  0x20
#define V_UPSTREAM_VC0A_IN_ORDER     0x1
#define V_UPSTREAM_VC0MMU_IN_ORDER   0x100

//
// SSA-DUnit (System Memory Controller)  Register Offset
// 

///
/// GTTMMADR for HSW is 4MB alignment  (Base address = [38:22])
/// For BDW is 16MB alignment (Base address = [38:24])
///
#define R_SA_IGD_GTTMMADR          0x10
#define R_SA_IGD_GMADR             0x18
#define R_SA_IGD_IOBAR             0x20

#define R_SA_IGD_BSM_OFFSET        0x005C  ///< Base of Stolen Memory
#define R_SA_IGD_MSAC_OFFSET       0x0062  ///< Multisize Aperture Control
#define R_SA_IGD_MSICAPID_OFFSET   0x0090  ///< MSI Capabilities ID
#define R_SA_IGD_MC_OFFSET         0x0092  ///< Message Control
#define R_SA_IGD_MA_OFFSET         0x0094  ///< Message Address
#define R_SA_IGD_MD_OFFSET         0x0098  ///< Message Data
#define R_SA_IGD_SWSCI_OFFSET      0x00E8
#define R_SA_IGD_ASLS_OFFSET       0x00FC  ///< ASL Storage

//
// MCHBAR IO Register Offset Equates
//
#define R_SA_MCHBAR_PGCB_CLKGATE_DIS      (0x4D4C)       ///< PGCB CLKGATE DIS base address
#define V_SA_MCHBAR_PGCB_CLKGATE_DIS_EN   (0x0)          ///< PGCB CLKGATE DIS Enable value
#define V_SA_MCHBAR_PGCB_CLKGATE_DIS_MASK (0xFFFFFFF0)   ///< PGCB CLKGATE DIS Bitwise Mask

///
/// IMR registers
///
#define R_SA_MCHBAR_BSMRCP                               (0x6838)
#define R_SA_MCHBAR_BSMRRAC                              (0x6840)
#define R_SA_MCHBAR_BSMRWAC                              (0x6848)
#define V_SA_MCHBAR_BSMRCP_SMM_CTRL_REG_LOCK             (0x00)
#define V_SA_MCHBAR_BSMR_SMM_OPEN_FOR_IA                 (0x14)
#define	V_SA_MCHBAR_BSMRRAC_SMM_WRITE_OPEN_FOR_ALL_CORE	 (0x04)
#define V_SA_MCHBAR_BSMRRAC_SMM_WRITE_CLOSED_FOR_IA_SMM  (0xFF)
#define	V_SA_MCHBAR_BSMRWAC_SMM_WRITE_OPEN_FOR_ALL_CORE	 (0x04)
#define V_SA_MCHBAR_BSMRWAC_SMM_WRITE_CLOSED_FOR_IA_SMM  (0xFF)
#define R_SA_MCHBAR_BIMR0BASE                            0x6870
#define R_SA_MCHBAR_BIMR0MASK                            0x6874
#define R_SA_MCHBAR_BIMR0CP                              0x6878
#define R_SA_MCHBAR_BIMR0RAC                             0x6880
#define R_SA_MCHBAR_BIMR0WAC                             0x6888
#define V_SA_MCHBAR_BIMRRAC_DISABLE                      0x0
#define V_SA_MCHBAR_BIMRWAC_DISABLE                      0x0
#define V_SA_MCHBAR_BIMRCP_ENABLE_IA_UNSTRUSTED          0x1
#define N_SA_MCHBAR_BIMR_LOCK_VALUE                      0x0         ///Policy value to lock IMR from all agents
#define B_SA_MCHBAR_BIMR_ADDRESS_MASK                    0x1FFFFFFF  ///Bit[28:0]
#define BUNIT_BIMRH_HIGHER_BOUND_MASK                    0x3FFFFFF
#define BUNIT_BIMRL_LOWER_BOUND_MASK                     0x3FFFFFF
#define B_SA_MCHBAR_BIMR_DEFAULT_BASE                    0x0         ///Default value of BIMR#BASE & ADDRESS_MASK
#define B_SA_MCHBAR_BIMR_DEFAULT_MASK                    0x0         ///Default value of BIMR#MASK & ADDRESS_ASK
#define N_SA_MCHBAR_BIMR_ADDRESS_LSHIFT                  10          ///Left shift value of BIMR#BASE & ADDRESS_MASK to align with real address
#define N_SA_MCHBAR_BIMR_ADDRESS                         10          ///First bit of address after shifting by ADDRESS_LSHIFT
#define N_SA_MCHBAR_BIMR_OFFSET                          0x20        ///Offset to next IMR reg
#define N_SA_MCHBAR_BIMR_NUM_OF_IMRS                     20          ///Number of IMR regs
#define BUNIT_BIMRH_ENABLE_ALL                           0xC0000000
#define	BUNIT_BIMR_RAC_READ_OPEN_FOR_ALL_CORE            0x14
#define	BUNIT_BIMR_RAC_READ_DISABLED                     0
#define	BUNIT_BIMR_WAC_WRITE_OPEN_FOR_ALL_CORE           0x14
#define	BUNIT_BIMR_WAC_WRITE_DISABLED                    0
#define BUNIT_BIMRCP_DISABLE_ALL                         0
#define R_SA_MCHBAR_BIMR1CP								 0x6898
//
// PID: PID_AUNIT
//
#define R_AUNIT_PGCB_CLKGATE_DIS                           (0x4D4C)       ///< PGCB CLKGATE DIS base address
#define V_AUNIT_PGCB_CLKGATE_DIS_EN                        (0x0)          ///< PGCB CLKGATE DIS Enable value
#define V_AUNIT_PGCB_CLKGATE_DIS_MASK                      (0xFFFFFFF0)   ///< PGCB CLKGATE DIS Bitwise Mask

//
// PID: PID_BUNIT
//
#define R_BUNIT_BCTRL                                      (0x4CD4)       ///< BCTRL base address
#define V_BUNIT_BCTRL_EN                                   (0x7)          ///< BCTRL Enable Value
#define V_BUNIT_BCTRL_MASK                                 (0xFFFFFFF8)   ///< BCTRL Bitwise Mask

//
// PID: PID_CUNIT
//
#define R_CUNIT_LOCAL_CONTROL_MODE                         (0x4104)       ///< LOCAL CONTROL MODE base address
#define V_CUNIT_LOCAL_CONTROL_MODE_EN                      (0x0)          ///< LOCAL CONTROL MODE Enable Value
#define V_CUNIT_LOCAL_CONTROL_MODE_MASK                    (0xFFFFFFFE)   ///< LOCAL CONTROL MODE Bitwise Mask

//
// PID: PID_TUNIT
//
#define R_TUNIT_CR_CLKGATE_CTL                             (0x5234)       ///< CR CLKGATE CTL base address
#define V_TUNIT_CR_CLKGATE_CTL_EN                          (0xFF000000)   ///< CR CLKGATE CTL Enable Value
#define V_TUNIT_CR_CLKGATE_CTL_MASK                        (0x00FFFFFF)   ///< OR CLKGATE CTL Bitwise Mask
#define TUNIT_MSI_ALGORITH_FIXED                           (0x00)
#define TUNIT_MSI_ALGORITH_ROUNDROBIN		               (0x01)

//
// PID: PID_DUNIT
//
#define R_DUNIT_DFUSESTAT                                  (0x0000001C)   ///< DFUSESTAT base address
#define B_DUNIT_DFUSESTAT_SINGLE_CHANNEL                   (0x00004000)   ///< DFUSESTAT Single Channel bit
#define R_DUNIT_DSCH                                       (0x00000000)   ///< DSCH base address
#define B_DUNIT_DSCH_DDR4EN                                (0x00000004)   ///< DSCH DDR4EN bit

//
// PID: PID_TUNIT
//

///
/// MOT Region registers
///
#define R_SA_MCHBAR_MOT_OUT_BASE           0x6AF0
#define R_SA_MCHBAR_MOT_OUT_MASK           0x6AF4
#define B_SA_MCHBAR_MOT_OUT_BASE_EN        BIT31       ///Enable flag for MOT region
#define B_SA_MCHBAR_MOT_OUT_ADDR_MASK      0x1FFFC000  ///Bit[28:14]
#define N_SA_MCHBAR_MOT_OUT_ADDR_LSHIFT    10          ///Left shift value to align with real address
#define N_SA_MCHBAR_MOT_OUT_ADDR           24          ///Start bit of address & mask

///
/// Vt-d Engine base address.
/// For Denverton
#define R_SA_MCHBAR_DEFVTDBAR_OFFSET       0x6C80
#define B_SA_MCHBAR_DEFVTDBAR_DEFVTDBAREN  BIT0
#define R_SA_MCHBAR_GFXVTDBAR_OFFSET       0x6C88
//#define R_SA_MCHBAR_VTD1_OFFSET  0x5400  ///< HW UNIT2 for IGD
//#define R_SA_MCHBAR_VTD2_OFFSET  0x5410  ///< HW UNIT3 for all other - PEG, USB, SATA etc
#endif
