/** @file
  Library functions for RC Config Block policy management.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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
#include "Include/Library/ConfigBlockLib.h"

/**
  Add required IP config block into main structure

  @param[in,out] ConfigBlockAddress           - Main config block structure where new child config blocks can be inserted
  @param[out]    RequestedConfigBlock         - A pointer that will store new child config block address
  @param[in]     RequestedConfigBlockId       - New child config block ID
  @param[in]     RequestedConfigBlockRevision - New child config block Revision
  @param[in]     RequestedConfigBlockSize     - New child config block Size

  @retval EFI_UNSUPPORTED - too many child config blocks to add into main structure.
  @retval EFI_SUCCESS     - execution successfully
**/
EFI_STATUS
EFIAPI
AddNewConfigBlock (
  IN  OUT CONFIG_BLOCK_STRUCT *ConfigBlockAddress,
  OUT     VOID                **RequestedConfigBlock,
  IN      UINT16              RequestedConfigBlockId,
  IN      UINT16              RequestedConfigBlockRevision,
  IN      UINT16              RequestedConfigBlockSize
  )
{
  UINT16 BlockCount;
  IP_BLOCK_STRUCT *TempConfigBlk;

  BlockCount = ConfigBlockAddress->Header.BlockCount;

  if (ConfigBlockAddress->Header.BlockCount == 0) {
    ConfigBlockAddress->OffsetTable.BlockOffset[0] = (UINT32) (sizeof (CONFIG_BLOCK_STRUCT));
  } else if (ConfigBlockAddress->Header.BlockCount >= MAX_CONFIG_BLOCKS){
    return EFI_UNSUPPORTED;
  } else {
    TempConfigBlk = (IP_BLOCK_STRUCT *) ((UINTN)(ConfigBlockAddress->OffsetTable.BlockOffset[BlockCount-1]) + (UINTN)ConfigBlockAddress);
    ConfigBlockAddress->OffsetTable.BlockOffset[BlockCount] = ConfigBlockAddress->OffsetTable.BlockOffset[BlockCount-1] + TempConfigBlk->BlockSize;
  }

  TempConfigBlk = (IP_BLOCK_STRUCT *) ((UINTN)(ConfigBlockAddress->OffsetTable.BlockOffset[BlockCount]) + (UINTN)ConfigBlockAddress);

  TempConfigBlk->BlockSize = RequestedConfigBlockSize;
  TempConfigBlk->BlockRevision = (UINT8) RequestedConfigBlockRevision;
  TempConfigBlk->BlockId = RequestedConfigBlockId;
  ConfigBlockAddress->Header.BlockCount++;
  ConfigBlockAddress->Header.Size += RequestedConfigBlockSize;

  *RequestedConfigBlock = (VOID *) TempConfigBlk;
  return EFI_SUCCESS;
}

/**
  Retrieve one child config block data by BlockID

  @param[in]     ConfigBlockAddress           - Main config block structure which contains many child config blocks
  @param[in]     RequestedConfigBlockId       - Child config block ID
  @param[out]    RequestedConfigBlock         - A pointer that will store child config block address requested by BlockID

  @retval EFI_INVALID_PARAMETER - BlockID out of supported range
  @retval EFI_NOT_FOUND         - Requested config block not found
  #retval EFI_SUCCESS           - Config block found and return
**/
EFI_STATUS
EFIAPI
GetConfigBlock (
 IN  CONFIG_BLOCK_STRUCT         *ConfigBlockAddress,
 IN UINT16                       RequestedConfigBlockId,
 IN OUT VOID                     **RequestedConfigBlock 
 ) 
{
  UINT8 i;
  IP_BLOCK_STRUCT *TempConfigBlk;

  if (RequestedConfigBlockId >= EnumConfigBlockIdMax) {
    return EFI_INVALID_PARAMETER;
  }
  for(i = 0; i < ConfigBlockAddress->Header.BlockCount; i++) {
    TempConfigBlk = (IP_BLOCK_STRUCT *) ((UINTN)(ConfigBlockAddress->OffsetTable.BlockOffset[i]) + (UINTN)ConfigBlockAddress);

    if (TempConfigBlk->BlockId == RequestedConfigBlockId) {
	  *RequestedConfigBlock = (VOID *) TempConfigBlk;
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

