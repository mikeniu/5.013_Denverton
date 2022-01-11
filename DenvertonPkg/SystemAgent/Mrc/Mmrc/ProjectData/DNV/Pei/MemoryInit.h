/** @file
  MemoryInit.h
  Framework PEIM to initialize memory on a Memory Controller.

  Copyright (c) 2005-2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MEMORY_INIT_H_
#define _MEMORY_INIT_H_

#include <Ppi/SaPolicy.h>
#include <Library/ConfigBlockLib.h>
#include "Mrc.h"
#include "McFunc.h"
#include "OemHooks.h"
#include "Bdat.h"
#if ME_SUPPORT_FLAG
#include <Ppi/MeUma.h>
#include <HeciRegs.h>
#endif

#ifdef IE_SUPPORT
#include <Ppi/IeHeci.h>
#endif //IE_SUPPORT

//
// MRC Variable Attributes
//
#define MEM_DET_COMMON_MEM_ATTR \
          (EFI_RESOURCE_ATTRIBUTE_PRESENT                 | \
           EFI_RESOURCE_ATTRIBUTE_INITIALIZED             | \
           EFI_RESOURCE_ATTRIBUTE_TESTED                  | \
           EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE             | \
           EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE       | \
           EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE | \
           EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE)

#define MEM_DET_COMMON_MEM_ATTR1 \
          (EFI_RESOURCE_ATTRIBUTE_PRESENT                 | \
           EFI_RESOURCE_ATTRIBUTE_INITIALIZED             | \
           EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE             | \
           EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE       | \
           EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE | \
           EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE)

extern EFI_GUID gEfiPeiBeforeMemoryDiscoveredPpiGuid;

/**
  This function installs the EFI Memory

  @param[in]       PeiServices     PEI Services Table.
  @param[in]       BootMode        Indicates the boot mode selected.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          EFI_SUCCESS     When no errors when installing memory
**/
EFI_STATUS
InstallEfiMemory (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN        EFI_BOOT_MODE         BootMode,
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This function installs the EFI Memory when S3 path is executed

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          EFI_SUCCESS     When no errors when installing memory
**/
EFI_STATUS
InstallS3Memory (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This function restores the MRC parameters previously saved and
  configured in the Setup Options

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          EFI_SUCCESS     MRC parameters restored correctly.
**/
EFI_STATUS
MrcParamsRestore (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This function saves the BDAT structure to Hob for later use.

  @param[in]       PeiServices         PEI Services Table.
  @param[in, out]  MrcData             Host structure for all data related to MMRC.
  @param[in] BdatMemoryData2Structure  Pointer to the BDAT data structure.
  @retval          EFI_SUCCESS         Hob is successfully built
**/
EFI_STATUS
MrcBdatSaveHob (
  IN        EFI_PEI_SERVICES                **PeiServices,
  IN        MMRC_DATA                       *MrcData,
  IN        BDAT_MEMORY_DATA_2_STRUCTURE    *BdatMemoryData2Structure
  );

/**
  This function saves the MRC Version to Hob for later use.

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcVersion      MRC Version.
  @retval          EFI_SUCCESS     Hob is successfully built.
**/
EFI_STATUS
MrcVersionSaveHob (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN        UINT32                MrcVersion
  );

/**
  This function saves the MRC Parameters to Hob for later use.

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          EFI_SUCCESS     Hob is successfully built.
**/
EFI_STATUS
MrcParamsSave (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This overrides the MRC Parameters set in DetectDimms with
  setup values.

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          EFI_SUCCESS     Setup correctly done.
**/
EFI_STATUS
GetTimingOverrides (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  Configure the memory thru several training steps

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
ConfigureMemory (
  IN  OUT   MMRC_DATA           *MrcData
  );

/**
  Write to CMOS.

  @param[in]   Index      Location to write to CMOS
  @param[in]   Value      Value to be written to the CMOS
  @retval      NONE
**/
VOID
WriteCmosMrc (
  IN      UINT8   Index,
  IN      UINT8   Value
  );

/**
  Read From CMOS.

  @param[in]   Index       Location to read from CMOS
  @retval      Value       Contains the value read from the CMOS
**/
UINT8
ReadCmosMrc (
  IN      UINT8   Index
  );

#endif
#if ME_SUPPORT_FLAG

/**
  This function read and returns the amount of ME UMA requested
  by ME ROM from the HECI Device. Skips using ME UMA PPI if not
  located.

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          NONE
**/
VOID
OemGetSetMeUma (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   MMRC_DATA              *MrcData
  );

/**
  MeConfigDidReg     - Send indication to ME FW to start using allocated ME UMA Space.
  HandleMeBiosAction - Will enforce the BIOS Action that was requested by ME FW
                       as part of the DRAM Init Done message
  Skips using ME UMA PPI if not located.

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcStatus       Pointer to MRC Status.
  @retval          NONE
**/
VOID
OemSendDramInitDoneCommand (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   EFI_STATUS            *MrcStatus
  );

#endif

#ifdef IE_SUPPORT
/**

	//TODO

  @param[in]       PeiServices     PEI Services Table.
  @param[in, out]  MrcStatus       Pointer to MRC Status.
  @retval          NONE
**/
VOID
OemSendIEDramInitDoneCommand (
  IN        EFI_PEI_SERVICES      **PeiServices,
  IN  OUT   EFI_STATUS            *MrcStatus
  );

#endif //IE_SUPPORT

#ifdef FSP_FLAG
/**
  Build FSP SMBIOS memory info HOB

  @param[in] MrcData          Pointer to Mrc Parameters
**/
VOID
BuildFspSmbiosMemoryInfoHob (
  IN MMRC_DATA                *MrcData
  );
#endif //FSP_FLAG
