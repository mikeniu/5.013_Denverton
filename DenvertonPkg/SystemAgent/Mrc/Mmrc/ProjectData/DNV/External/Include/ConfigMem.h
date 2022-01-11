/** @file 
  ConfigMem.h
  Data definition for memory Configuration
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _CONFIGMEM_H_
#define _CONFIGMEM_H_

#include "Mrc.h"
#include "McFunc.h"
#include "MrcFunc.h"

#if SIM || JTAG
extern FILE *gLogFile;
#endif

/**
  Initializes all dynamic variables used in JEDEC init for DDR3 
  or DDR4 

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Channel       Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
InitializeJedecVariables (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  );

/**
  Initializes all dynamic variables used in JEDEC init for DDR3.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Channel       Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
InitializeDdr3JedecVariables (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  );

/**
  Initializes all dynamic variables used in JEDEC init for DDR4.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Channel       Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
InitializeDdr4JedecVariables (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  );

extern
VOID
SetOdtMatrix (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8                 Node,
  IN        UINT8        Channel,
  IN  OUT   UINT32       MrsValue[MAX_RANKS][MR_COUNT]
  );

/** 
  Notify Punit that BIOS reset is complete. Polls the
  BIOS_RESET_CPL register waiting for the  PUNIT to be ready.

  @param[in, out]   MrcData       Host structure for all MRC 
                                  global data.
  @retval           MMRC_SUCCESS  Punit Mailbox configured 
                                  correctly
**/
MMRC_STATUS 
InitPunitMailbox (
  IN OUT  MMRC_DATA *MrcData
  );

/**
  Executes the SPD-Only reset when needed

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          TRUE          If a reset is required due to a change in the frequency
  @retval          FALSE         Otherwise
**/
BOOLEAN
SpdResetNeeded (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  Routine to initialize other pieces of silicon which may be required
  for MRC to successfully initialize memory. High precision event timer,
  PUNIT mailbox, etc.

  @param   MrcData       Host structure for all data related to MMRC.
  @retval  Status        
**/
MMRC_STATUS
ExternalSiliconInit (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This routine is responsible to initialize ALL data required for
  MMRC to initialize memory

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
GetInputParameters (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This routine assumes all input parameters to the MMRC have been
  populated by GetInputParameters(). Converting all timing variables
  to the correct values for DUNIT programming is the responsibility
  of this function.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
ConvertInputParameters (
  IN  OUT   MMRC_DATA   *MrcData
  );

#endif
