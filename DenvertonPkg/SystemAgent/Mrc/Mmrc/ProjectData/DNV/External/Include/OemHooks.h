/**  @file
  OemHooks.h
  This file the include all the external OEM Hooks.

  Copyright (c) 1999 - 2017 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _OEMHOOKS_H_
#define _OEMHOOKS_H_

#include "Mrc.h"
#include "Regs.h"

#define PLATFORM_VARIATIONS  3

typedef enum {
  DimmInstalled = 0,    // Check Spd Data thru the Smbus
  SolderDownMemory,     // Check fixed Spd table
  OemDimmTypeMax
} OEM_MEMORY_DIMM_TYPE;

typedef struct {
  UINT16  ByteOffset;
  UINT8   OverrideValue;
} OVERRIDE_SPD_DELTA;

//
// Added specific CAR_LAKE board variation from the P_MEM_DOWN
//
#define P_CAR_LAKE    (P_MEM_DOWN | BIT15)
#define P_CAR_LAKE_2  (P_MEM_DOWN | BIT14)
#define P_OEM_BRD0    (P_MEM_DOWN | BIT13)
#define P_OEM_BRD1    (P_MEM_DOWN | BIT12)
#define P_OEM_BRD2    (P_MEM_DOWN | BIT11)



#define MTRR_PHYS_BASE_0  0200h
#define MTRR_PHYS_MASK_0  0201h
#define MTRR_PHYS_BASE_1  0202h
#define MTRR_PHYS_MASK_1  0203h
#define MTRR_PHYS_BASE_2  0204h
#define MTRR_PHYS_MASK_2  0205h
#define MTRR_PHYS_BASE_3  0206h
#define MTRR_PHYS_MASK_3  0207h
#define MTRR_PHYS_BASE_4  0208h
#define MTRR_PHYS_MASK_4  0209h
#define MTRR_PHYS_BASE_5  020Ah
#define MTRR_PHYS_MASK_5  020Bh
#define MTRR_PHYS_BASE_6  020Ch
#define MTRR_PHYS_MASK_6  020Dh
#define MTRR_PHYS_BASE_7  020Eh
#define MTRR_PHYS_MASK_7  020Fh
#define MTRR_PHYS_BASE_8  0210h
#define MTRR_PHYS_MASK_8  0211h
#define MTRR_MEMORY_TYPE_WP 05h
#define MTRR_MEMORY_TYPE_WB 06h
#define MTRR_MEMORY_TYPE_WC 01h
#define TEMP_CACHE_SIZE_ASM 01000h
#define TEMP_CACHE_SIZE_CPP 0x1000

#define SUS_IO_START_ADDRESS      0x80
#define SCORE_IO_START_ADDRESS    0x00

#define MAX_BOARD_PRODUCT_NAME    50
#define FRU_DEVICE_ADDRESS        0xAC

typedef struct {
  UINT8   BoardProductName[MAX_BOARD_PRODUCT_NAME];
  UINT16  PlatformId;
} BOARD_PRODUCT_NAME;

//
// Define region of Reserved memory just below TSEG for nCPM usage
// Comment out this define to disable this region
//
#define IQAT_RESERVED_MEMORY_REGION_SIZE  0x100000

/**
  This function detects the platform under test.

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          MMRC_SUCCESS    The platform was detected.
**/
MMRC_STATUS
OemGetPlatformType (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This function sets the OEM specific information

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
OemSetMrcData (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This function configures the OEM board mux(es) appropriately.

  @param[in]  Channel         Channel being examined. 
  @param[in]  Dimm            Dimm being examined. 
  @retval     MMRC_SUCCESS    OEM muxes are set correctly. 
**/
MMRC_STATUS
OemI2cMux (
  IN        UINT8                 Channel,
  IN        UINT8                 Dimm
  );

/**
  This function releases the board's activation of self refresh to the DIMMs

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex
  @param[in]       StringIndex
  @param[in]       Channel
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
OemClearSelfRefreshClamp (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  );
#endif
