/** @file
  McFunc.h
  This file inlcude external Memory DIMM initialization.
   
  Copyright (c) 2005-2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MCFUNC_H_
#define _MCFUNC_H_

#include "Mrc.h"
#include "MmrcHooks.h"

extern UINT16     mFrequencyMultiplier[C_MAXDDR];
extern UINT8      mDdr3Freq2WLTable[C_MAXDDR];
extern UINT8      mDdr4Freq2WLTable[C_MAXDDR];
extern UINT8      mFawTableDdr3[C_MAXDDR][2];
extern UINT8      mFawTableDdr4[C_MAXDDR][3];

#define DNV_A0_STEP 0x0
#define DNV_A1_STEP 0x01
#define DNV_B0_STEP 0x10

typedef enum {
  MrcRank0 = 0x01,
  MrcRank1 = 0x02,
  MrcRank2 = 0x08,
  MrcRank3 = 0x04,
  MrcRank4 = 0x10,
  MrcRank5 = 0x20,
  MrcRank6 = 0x40,
  MrcRank7 = 0x80,
  MrcAllRanks = 0xFF
} RANK_NUM;

/**
  It delays the execution in the MRC for 'DelayUs' microseconds
  The delay must never be between 0-9 microseconds 
  in order to do not create an overlap in the HPET and max value
  should be ('desired' / 0xF) < (2^32 - 1)

  @param[in]       DelayUs         Delay specified in microseconds.
  @retval          NONE
**/
VOID
McStall (
  IN      UINT32    DelayUs
  );

/**
  It delays the execution in the MRC for 'DelayNs' nanoseconds
  There is a restriction this delay should be a multiple 50

  @param[in]       DelayNs         Delay specified in nanoseconds. 
                                   It should be a mutiple of 50
  @retval          NONE
**/
VOID
McStallNanoSecond (
  IN      UINT32    DelayNs
  );

/**
  Initialize the MRC parameters with default values and/or from setup options

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          Status
**/
MMRC_STATUS
MrcInitializeParameters (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  Calculate timing parameters dependent on other parameters 
  gotten from the SPD Data Bytes 

  @param[in, out]  MrcData       Host structure for all data related to MMRC. 
  @retval          NONE 
**/ 
MMRC_STATUS
TimingParams (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  Overrides timing parameter detected from Dimms

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
OverrideTimingParams (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This function gets the platform setup

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          Status
**/
MMRC_STATUS
GetPlatformSettings (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This function configures the SMBus SPD Write Disable function
  DNV has two SMBus controllers and both of them have their own SPD Write Disable bit.
  SMBus Legacy (B0:D31:F4): DIMMS are connected to this controller.
  SMBus Host (B0:D18:F0): It is not intended to participate in the DIMM SPD.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
ConfigureSMBusSpdWriteDisable (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This function sets the SPD Write Disable bit in the SMBus Legacy controller.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
SetSMBusLegacySpdWriteDisable (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This function sets the SPD Write Disable bit in the SMBus Host controller.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
SetSMBusHostSpdWriteDisable (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This function determines which kind of boot path needs to be followed in MRC flow

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          Status
**/
MMRC_STATUS
DetermineBootMode (
  IN  OUT   MMRC_DATA   *MrcData
  );

UINT8
GetSocStepping ();

UINT16
GetCpuType ();

UINT32
get_initial_seed ();

/**
  This function configures the slow or fast exit power down for any type of memory (DDR3/4)

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Channel under examination
**/
MMRC_STATUS
SetSlowFastExit (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  );

/**
  This function configures the scrambler

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Channel under examination
**/
MMRC_STATUS
SetScrambler (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  );

/**
  This function determines if coldboot is required because DIMMs have been changed
  in between boots.

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          TRUE            Cold Boot Required
  @retval          FALSE           Cold Boot Not Required
**/
BOOLEAN
CheckColdBootRequired (
  IN  OUT   MMRC_DATA   *MrcData
  );

MMRC_STATUS
SimSetDunitConfigValid (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  );

/**
  Initialize the Modular MRC parameters with values detected
  by the MRC. This is necessary so MMRC has its own copies of
  variables it needs to operate correctly (like which ranks
  are enabled, what the timing parameters are, etc).

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          Status
**/
MMRC_STATUS
FillModMrcDataStructure (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  Calculates the largest integer that is both
  a power of two and less than Inpu

  @param[in]       Operand         Value to calculate power of two
  @retval                          The largest integer that is both a power of
                                   two and less than Input
**/
UINT32
GetPowerOfTwo (
  IN        UINT32        Operand
  );

/**
  Finds an active channel that have read the fuses and current DDR freq

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          0xFF            No Active Channel Found
  @retval          Channel         Channel Active Found
**/
UINT8
FindActiveChannel (
  IN  OUT   MMRC_DATA   *MrcData
  );

#endif

