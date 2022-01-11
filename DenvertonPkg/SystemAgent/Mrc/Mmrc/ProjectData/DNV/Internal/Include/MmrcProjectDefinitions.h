/** @file
  MmrcProjectDefinitions.h
  Includes all hard defines specific to a project which are NOT 
  outputted by the MMRC tool. Things like the maximum number of 
  channels, ranks, DIMMs, etc, should be included here. This file 
  needs to be included by most MMRC components, including Mmrc.h. 
 
  Copyright (c) 2005 - 2015 Intel Corporation. All rights 
  reserved This software and associated documentation (if any) 
  is furnished under a license and may only be used or copied in 
  accordance with the terms of the license. Except as permitted 
  by such license, no part of this software or documentation may 
  be reproduced, stored in a retrieval system, or transmitted in 
  any form or by any means without the express written consent 
  of Intel Corporation. 

**/

#ifndef _MMRCPROJECTDEFINITIONS_H
#define _MMRCPROJECTDEFINITIONS_H

#include "MmrcProjectDefinitionsGenerated.h"

#define MAX_TIMING          2
#define MAX_CONSEC          5
#define REL                 0
#define MMRC_ABS            1
#define COMPOSITE_NONE      0
#define COMPOSITE_COUNT     1
#define COMPOSITE_RESULTS   2
#define OVERWRITE           4
//
// Index based on control knobs.
//
#if SIM
#define NUM_CENTERV_SAMPLES 3
#else
#define NUM_CENTERV_SAMPLES 5
#endif
#define NUM_CENTERT_SAMPLES 3

#define MAX_ELEMENT_TABLE     40
#define MAX_CMDS              8
#define MAX_CTL               3

#define MAX_GROUPS            MAX(MAX_CTL,MAX_CMDS)

#define MAX_RMT_ELEMENTS      4

#define MAX_RESTORE_SIGNALS       (MAX(MAX_CMDS, 0))
#define MAX_RESTORE_SIGNALS_VREF  (1)

typedef enum {
  RmtRecEn = 0,
  RmtRxDqs,
  RmtTxDqs,
  RmtTxDq,
  RmtRxVref,
  RmtTxVref,
  RmtCmd,
  MaxRmtElements,
} RMT_ELEMENTS;

#define RMT_REC_EN_EXEC       (1 << RmtRecEn)
#define RMT_RX_DQS_EXEC       (1 << RmtRxDqs)
#define RMT_TX_DQS_EXEC       (1 << RmtTxDqs)
#define RMT_TX_DQ_EXEC        (1 << RmtTxDq)
#define RMT_RX_VREF_EXEC      (1 << RmtRxVref)
#define RMT_TX_VREF_EXEC      (1 << RmtTxVref)
#define RMT_CMD_EXEC          (1 << RmtCmd)
#define RMT_ALL_TESTS_EXEC    ((1 << MaxRmtElements) - 1)

//
// PPR definitions
//
#define MAX_BANK_BITS_DDR4            2
#define MAX_BANK_GROUP_BITS_DDR4      2
#define MAX_ROW_BITS_DDR4             18
#define MAX_RANK_SELECT_BITS_DDR4     2

#define MAX_RANK_BG_BA_ROW_BITS       (MAX_RANK_SELECT_BITS_DDR4 + MAX_BANK_GROUP_BITS_DDR4 + MAX_BANK_BITS_DDR4 + MAX_ROW_BITS_DDR4)
#define MAX_BANK_GROUPS_DDR4          4

#define MAX_GUARDKEY_PATTERNS         4

enum {
  MA0, MA1, MA2, MA3, MA4, MA5, MA6, MA7, MA8, MA9, MA10, MA11, MA12, MA13, MA14, MA15,     BA0, BA1, BA2,     RAS, CAS, WE
};

#define MINDDR                MMRC_1600
#define MAXDDR                MMRC_2666

#define MINCOREFREQ           COREFREQ_800
#define MAXCOREFREQ           COREFREQ_1333

#ifndef PCIEX_BASE_ADDRESS
//
// PCIe Base Address
//
#ifndef SIM
#define PCIEX_BASE_ADDRESS  0xE0000000
#else
#define PCIEX_BASE_ADDRESS  0xC0000000
#endif
//
// PMC Memory Space
//
#define PBASE               0xFED03000
#endif

//
// Extended Configuration Base Address.  Port to value enabled by calling code, if necessary.
// If the EC space is not enabled by code calling the MRC, then enable the
// EC space to this value
//
#ifndef EC_BASE
#define EC_BASE   PCIEX_BASE_ADDRESS
#endif

#ifndef MRC_DATA_REQUIRED_FROM_OUTSIDE
//
// SysCtl SMBus address and block size
//
#define VF_SC_SMBUS_ADDRESS         0x60
#define VF_SC_BYTE_LEN              1
#define VF_SC_BLOCK_LEN             4
#define VF_SC_SMBUS_WRCMD           1
#define VF_SC_SMBUS_RDCMD           0
//
// SMBus I/O Registers
//
#define R_PCH_SMBUS_HSTS                0x00  // Host Status Register R/W
#define B_PCH_SMBUS_HBSY                0x01
#define R_PCH_SMBUS_HCTL                0x02  // Host Control Register R/W
#define B_PCH_SMBUS_START               0x40  // Start
#define B_PCH_SMBUS_DERR                0x04
#define B_PCH_SMBUS_BERR                0x08
#define B_PCH_SMBUS_IUS                 0x40
#define B_PCH_SMBUS_BYTE_DONE_STS       0x80
#define B_PCH_SMBUS_HSTS_ALL            0xFF
#define V_PCH_SMBUS_SMB_CMD_BYTE_DATA   0x08  // Byte Data
#define V_PCH_SMBUS_SMB_CMD_BLOCK       0x14  // Block


#define R_PCH_SMBUS_HCMD                0x03  // Host Command Register R/W

#define R_PCH_SMBUS_TSA                 0x04  // Transmit Slave Address Register R/W
#define B_PCH_SMBUS_RW_SEL_READ         0x01  // Read

#define R_PCH_SMBUS_HD0                 0x05  // Data 0 Register R/W
#define R_PCH_SMBUS_HD1                 0x06  // Data 1 Register R/W
#define R_PCH_SMBUS_HBD                 0x07  // Host Block Data Register R/W

#define R_PCH_SMBUS_AUXS                0x0C  // Auxiliary Status Register R/WC
#define B_PCH_SMBUS_CRCE                0x01  // CRC Error

#define R_PCH_SMBUS_AUXC                0x0D  // Auxiliary Control Register R/W
#define B_PCH_SMBUS_E32B                0x02  // Enable 32-byte Buffer
#define B_PCH_SMBUS_AAC                 0x01  // Automatically Append CRC

#define BUS_TRIES                       3       // How many times to retry on Bus Errors

#endif // MRC_DATA_REQUIRED_FROM_OUTSIDE

#define PHYENTERWRLVL               0x1
#define PHYEXITWRLVL                0x0

#define DQ_BURSTLEN_8   0x00
#define DQ_BURSTLEN_DYN 0x01;
#define DQ_BURSTLEN_4   0x02

//
// Various portions of the MMRC that can be run by calling MmrcEntry().
//
typedef enum {
  EntireMmrc,
  DdrioPhyInit,
  MemoryTraining,
  PostTraining,
  HandlePeriodicComp,
  ExecuteSpecificFunction,
  CapsuleInit,
  MmrcMaxStage
} MMRC_STAGE;
//
// Message Bus Commands
//
#define CMD_READ_REG          0x10    // Read Message Bus Register Command 
#define CMD_WRITE_REG         0x11    // Write Message Bus Register Command 
#define CMD_DRAM_INIT         0x68    // JEDEC Command 
#define CMD_WAKE              0xCA    // Wake Command 
#define CMD_SUSPEND           0xCC    // Suspend Command 

#define PASS_MARGIN_AXIS_X(x)     ((7*x)/10)
#define PASS_MARGIN_AXIS_Y(y)     ((1*y)/10)

#define RD_REG                0x01
#define RD_ONLY               0x02
#define WR_OFF                0x04
#define FC_WR                 0x08
#define UPD_CACHE             0x10
#define FC_WR_PRINT           0x20
#define FC_PRINT              0x40

#define VLV_CPU 0x670
#define AVN_CPU 0x6D0

#define CPGC_LFSR_VICTIM_SEED       0xAA5AC355 //0xF294BA21 // Random seed for victim.
#define CPGC_LFSR_AGGRESSOR_SEED    0x5FA53CAA //0xEBA7492D // Random seed for aggressor.

#define AUTO_OPTION           15
#define DDR_CHANNELS_AUTO     0        //Both channels are enabled
#define DDR_CHANNEL0_DISABLED 1
#define DDR_CHANNEL1_DISABLED 2
//
// Favor
//
#define DDR_FAVOR_DISABLED     0
#define DDR_FAVOR_PERFORMANCE  1
#define DDR_FAVOR_POWER_SAVING 2
#define DDR_FAVOR_CUSTOM       3

#endif // _MMRCPROJECTDEFINITIONS_H
