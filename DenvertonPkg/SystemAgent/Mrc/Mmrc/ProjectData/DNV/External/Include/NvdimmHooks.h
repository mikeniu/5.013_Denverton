/**  @file
  NvdimmHooks.h
  This file include all the OEM hooks for MRC.

  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#ifndef NVDIMMHOOKS_H_
#define NVDIMMHOOKS_H_
#include "Mrc.h"

#define NOT_NVDIMM                 0
#define NVDIMM_VENDOR_NOT_FOUND    1

#define NVDIMM_SUCCESS 0
#define NVDIMM_ERROR 1
#define NVDIMM_NODATA 2
#define NVDIMM_RESTORE 3
#define ERROR_SAVE     0x10

#define ADR_MODE         1
#define C2F_MODE         2
#define NVDIMM_MODE      4

#define STATUS_RESTORE_NEEDED      (1 << 0)
#define STATUS_RESTORE_INPROGRESS  (1 << 1)
#define STATUS_RESTORE_SUCCESSFUL  (1 << 2)
#define STATUS_ARMED               (1 << 3)

#define ERROR_DETECT               (1 << 8)
#define ERROR_RESTORE              (1 << 9)
#define ERROR_ARM                  (1 << 10)

#define NVDIMM_NO_CHANGE           (0)
#define NVDIMM_MODE_WANTED         (1)
#define NVDIMM_ADR_RECOVERY        (2)

/**
  This function checks if the DIMM is a supported NVDIMM.

  @param[in, out]  MrcData  Host structure for all data related to MMRC.
  @retval          VenId    The internal identification of the NVDIMM 0=NOT NVDIMM
**/
UINT32
PlatformDetectNvDimm (
  IN  OUT  MMRC_DATA   *MrcData,
  IN       UINT8 node,
  IN       UINT8 ch,
  IN       UINT8 dimm
  );

/**
  This function return if a NonVolatile memory boot flow is needed.

  @param[in, out]  MrcData     Host structure for all data related to MMRC.
  @retval          ModeWanted  The NonVolatile boot mode wanted 0=NO CHANGE
**/
UINT32
PlatformUpdateBootMode (
    IN  OUT  MMRC_DATA   *MrcData
  );

/**
  This function waits for all NVDIMMs to be ready and updates a DIMMS status

  @param[in, out]  MrcData  Host structure for all data related to MMRC.
  @retval          N/A
**/
UINT32 PlatformNvDimmStatus(
  IN  OUT   MMRC_DATA   *MrcData);

/**
  This function walks all NVDIMMs thru a data recovery

  @param[in, out]  MrcData  Host structure for all data related to MMRC.
  @retval          N/A
**/
UINT32 PlatformRestoreNvDimms(
  IN  OUT   MMRC_DATA   *MrcData);

/**
  This function walks all NVDIMMs to a point where then can store data

  @param[in, out]  MrcData  Host structure for all data related to MMRC.
  @retval          N/A
**/
UINT32 PlatformArmNvDimms(
  IN  OUT   MMRC_DATA   *MrcData);

/**
  This function turns off Self-Refresh and prepairs NVDIMMs for normal use

  @param[in, out]  MrcData  Host structure for all data related to MMRC.
  @retval          N/A
**/
VOID PlatformMrsRestoreNvDimms(
  IN  OUT  MMRC_DATA   *MrcData);

/**
  This function does the time consuming work so that PlatformMrsRestoreNvDimms()
  runs as fast as possible

  @param[in, out]  MrcData  Host structure for all data related to MMRC.
  @retval          N/A
**/
VOID PlatformCacheMrsRestoreNvDimms(
  IN  OUT   MMRC_DATA   *MrcData);

/**
  This function reports if an address should have its ECC initialized
  and returns the length the memory region.

  @param[in, out]  MrcData  Host structure for all data related to MMRC.
  @param[in,    ]  Chan     Channel the DIMM is located {0 or 1}
  @param[in,    ]  Dimm     Dimm location in channel { 0 or 1}
  @param[  , out]  Skip     Set to 1 if no ECC init is needed otherwise 0
  @retval          N/A
**/
VOID EccInitializeNeeded(
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8       Chan,
  IN        UINT8       Dimm,
	  OUT   UINT8       *Skip);


#endif /* NVDIMMHOOKS_H_ */
