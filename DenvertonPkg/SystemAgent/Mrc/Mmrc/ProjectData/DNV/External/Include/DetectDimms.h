/** @file
  DetectDimms.h
  This file include all the external DetectDimm.

  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _DETECTDIMMS_H_
#define _DETECTDIMMS_H_

#include "Mrc.h"
#include "McFunc.h"
#include "MrcFunc.h"
#include "ConfigMemData.h"

extern UINT16 mDdr3RelevantSpdBytes[];
extern UINT16 mDdr4RelevantSpdBytes[];
//
// beep tones
//
#define NOTE(x) ((119318200+(x)/2)/(x))

//
// Disabling potencially uninitialized
//
#pragma warning (disable : 4701 )

#pragma pack(1)

typedef struct {
  UINT8  TimingDataIndex;
  UINT16 MaxMchVal;
  UINT8  Ddr3MinMCHVal;
  UINT8  Ddr3LowSpdByte;
  UINT8  Ddr3HighSpdByte;
  UINT8  Ddr3HighSpdByteMask;
  UINT8  Ddr3FtbSpdByte;
  char   Name[6];
} STRUCT_TIMING_DATA;

typedef enum {
  MrcMsgOk = 0,
  MrcMsgDdrTypeNotSupported,
  MrcMsgModuleTypeNotSupported,
  MrcMsgDeviceWidthNotSupported,
  MrcMsgDimm0IsNotPopulated,
  MrcMsgBanksNotSupported,
  MrcMsgDDenNotSupported,
  MrcMsgDiffNoRanksBetweenSoDimms,
  MrcMsgMoreDrNotSupported,
  MrcMsgNoDimmsPresent,
  MrcMsgClXTckNotLess20Ns,
  MrcMsgNullPtr,
  MrcMsgTimingOutBoundary,
  MrcMsgMixingX8X16,
  MrcMsgDimmFreqNotSupported,
  MrcMsgNonEccDimmDetected,
  MrcMsgCasLatencyNotSupported,
  MrcMsgUnknown,
  MrcMsgNonEccSoDimmNotSupported,
  MrcBothChannelAreDisabled,
  MrcDifferentDdrTypeSameChannel,
  MrcMsgDdr3VddNotSupported,
  MrcMsgQrankNotSupported,
  MrcMsgModuleTypeMixNotSupported,
  MrcMsgDeviceWidthx8FuseOff,
  MrcMsgDeviceWidthx4FuseOff,
  MrcMsgDDR3SupportFuseOff,
  MrcMsgDDR4SupportFuseOff,
  MrcMsgMaxError
} DETECT_DIMMS_ERROR;

//
// SPD constraints in DRAM clocks
//
#define MIN_SUPPORTED_TCL          7
#define MAX_SUPPORTED_TCL          20
#define ADJUST_TCL                 3
#define ADJUST_TRP                 3
#define ADJUST_TRCD                3
#define ADJUST_TRAS                9
#define ADJUST_TWR                 0
#define ADJUST_TRFC                15
#define ADJUST_TWTR                0
#define ADJUST_TRRD                0
#define ADJUST_TRTP                0
#define MIN_SUPPORTED_TWR          10
#define MAX_SUPPORTED_TWR          50
#define MIN_SUPPORTED_TRCD         7
#define MAX_SUPPORTED_TRCD         21
#define MIN_SUPPORTED_TRRD         6
#define MAX_SUPPORTED_TRRD         10
#define MIN_SUPPORTED_TRP          7
#define MAX_SUPPORTED_TRP          20
#define MIN_SUPPORTED_TRAS         24
#define MAX_SUPPORTED_TRAS         44
#define MIN_SUPPORTED_TRFC         128
#define MAX_SUPPORTED_TRFC         587
#define MIN_SUPPORTED_TWTR         2
#define MAX_SUPPORTED_TWTR         7
#define MIN_SUPPORTED_TRTP         6
#define MAX_SUPPORTED_TRTP         10
#define MIN_SUPPORTED_TFAW         16
#define MAX_SUPPORTED_TFAW         39
#define MIN_SUPPORTED_TCCD         4
#define MAX_SUPPORTED_TCCD         4
#define MIN_SUPPORTED_TWTP         0
#define MAX_SUPPORTED_TWTP         0xFF
#define MIN_SUPPORTED_TWCL         7
#define MAX_SUPPORTED_TWCL         18
#define MIN_SUPPORTED_TCMD         1
#define MAX_SUPPORTED_TCMD         3
#define MAX_SUPPORTED_TWTR_DDR4    10
//
// SPD constraints in DRAM clocks DDR4 specific
//
#define MAX_SUPP_DDR4_TCCD    8
#define NANOSEC_15            14994
#define NANOSEC_7p5           7497
//
// DIMM Configuration Flags, used to determine the proper voltage
// and also to enable/disable setup options
//
typedef union {
#ifndef ASM_INC
  struct {
    UINT32  Force125VDimmAt135V         : 1;  ///> [0]: Enable setup option to force 1.25V DIMM run at 1.35V
    UINT32  Force135VDimmAt150V         : 1;  ///> [1]: Enable setup option to force 1.35V DIMM run at 1.5V
    UINT32  AddMemoryBehaviour          : 1;  ///> [2]: Enable setup option for Memory Behavior
    UINT32  Detected125VDimm            : 1;  ///> [3]: 1.25V DIMM Detected
    UINT32  Detected135VDimm            : 1;  ///> [4]: 1.35V DIMM Detected
    UINT32  Detected150VDimm            : 1;  ///> [5]: 1.50V DIMM Detected
    UINT32  Detected125Or135VDimm       : 1;  ///> [6]: 1.25V/1.35V DIMM Detected
    UINT32  Detected135Or150VDimm       : 1;  ///> [7]: 1.35V/1.50V DIMM Detected
    UINT32  Detected125VDimmAt1600      : 1;  ///> [8]: 1.25V@1600 DIMM Detected
    UINT32  RunDimmVoltageAt            : 2;  ///> [10:9]: 0: Invalid; 1 = 1.25V; 2 = 1.35V; 3 = 1.5V
    UINT32  Reserved                    : 21; ///> [31:11]: Reserved
  } FLAGS;
#endif
  UINT32  Data;
} DIMM_VOLTAGE_FLAGS;

/**
  This function provides access to the relevant SPD bytes array.
  Needed by Bdat.c.

  @param[in]   MrcData           Host structure for all data related to MMRC.
  @param[in]   Channel           Channel under examination.
  @param[out]  RelevantSpdBytes  Returned pointer to the SPD byte array.
  @param[out]  NumElements       Number of elements in the SPD byte array.
**/
VOID
GetRelevantSpdBytes (
  IN        MMRC_DATA       *MrcData,
  IN        UINT8           Channel,
      OUT   UINT16          **RelevantSpdBytes,
      OUT   UINT32          *SpdElements
  );
/**
  This function detects if there's a 2R dimm across the platform

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Channel under examination
  @retval          TRUE            there's a 2R Dimm
  @retval          FALSE           there's not 2R Dimm
**/
BOOLEAN
DualRankInSystem (
  IN  OUT   MMRC_DATA   *MrcData,
  UINT8 Channel
  );
/**
  This function detects the memory configuration using the data gotten from the SPD (Serial Presence Detect)

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Channel under examination
  @retval          MMRC_SUCCESS    Memory detection was successful
  @retval         !MMRC_SUCCESS    Refer to UINT8 mErrorMessages[][] table
                                   and enum DETECT_DIMMS_ERROR to identify error
**/
MMRC_STATUS
DetectDimms (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
  This function uses SPD data to determine the timings

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examinated
  @retval          MMRC_SUCCESS    Timing comply with JEDEC spec
  @retval          !MMRC_SUCCESS   Refer to UINT8 mErrorMessages[][] table
                                   and enum DETECT_DIMMS_ERROR to identify error
**/
MMRC_STATUS
FindTrasTrpTrcd (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8                   Channel
  );

/**
  This function uses SPD data to determine the timings

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examinated
  @retval          MMRC_SUCCESS    Timing comply with JEDEC spec
  @retval          !MMRC_SUCCESS   Refer to UINT8 mErrorMessages[][] table
                                   and enum DETECT_DIMMS_ERROR to identify error
**/
MMRC_STATUS
CalcDimmConfig (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8                   Channel
  );

/** 
  This function reads SPD data and determines which slots are populated.

  @param[in]   MrcData       Host structure for all data related to MMRC.
  @param[in]   Channel       Current Channel being examined
  @retval      MMRC_SUCCESS  SPD data gotten correctly
  @retval     !MMRC_SUCCESS  Error trying to get SPD data 
**/
MMRC_STATUS
GetSpdData (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8          Channel
  );

/**
  This function uses SPD data to decide to call DDR3 or DDR4 detect DIMMs

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Current channel being 
                                   examinated
  @retval          MMRC_SUCCESS    Memory identify was successful
  @retval          !MMRC_SUCCESS   Refer to UINT8 mErrorMessages[][] table
                                   and enum DETECT_DIMMS_ERROR to identify error
**/
MMRC_STATUS
CallIdentifyDimms (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8          Channel
  );

/**
  This function uses SPD data to detect the presence of unbuffered DDR DIMMs

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Current channel being 
                                   examinated
  @retval          MMRC_SUCCESS    Memory identify was successful
  @retval          !MMRC_SUCCESS   Refer to UINT8 mErrorMessages[][] table
                                   and enum DETECT_DIMMS_ERROR to identify error
**/
MMRC_STATUS
IdentifyDdr3Dimms (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8                   Channel
  );

/**
  This function uses DDR4 SPD data required for Dunit timing programming

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Current channel being 
                                   examinated
  @retval          MMRC_SUCCESS    Memory identify was successful
  @retval          !MMRC_SUCCESS   Refer to UINT8 mErrorMessages[][] table
                                   and enum DETECT_DIMMS_ERROR to identify error
**/
MMRC_STATUS
IdentifyDdr4Dimms (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  );

/**
  This function uses SPD data to determine the timings for the 
  memory channel 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examinated
  @retval          MMRC_SUCCESS    Timing comply with JEDEC spec
  @retval          !MMRC_SUCCESS   Refer to UINT8 mErrorMessages[][] table
                                   and enum DETECT_DIMMS_ERROR to identify error
**/
MMRC_STATUS
FindTclTacTClk (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8                   Channel
  );

/**
  This function detects if any memory slot is not populated 
   
  @param[in, out]  MrcData         Host structure for all data 
                                   related to MMRC.
  @retval          MMRC_SUCCESS    If both slots are populated. 
  @retval          !MMRC_SUCCESS   If any slot is empty. 
   
**/
BOOLEAN
IsAnySlotEmpty (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  );

/**
  This function enforces the common frequency across channels

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          MMRC_SUCCESS    Timing comply with JEDEC spec.
  @retval          !MMRC_SUCCESS   Refer to UINT8 mErrorMessages[][] table
                                   and enum DETECT_DIMMS_ERROR to identify error
**/
MMRC_STATUS
EnforceCommonFreq (
  IN  OUT   MMRC_DATA     *MrcData
  );

extern VOID
VerifyResetRequests (
  IN  OUT   MMRC_DATA         *MrcData
  );

void
BusWidthVariable(
IN  OUT   MMRC_DATA   *MrcData
  );

#if DRAM_RAPL == 1
/**
CalcRaplBandWidthScale:  Calculate the average Band Width Scale factor depending on the
Memory configuration

@param[in, out]  MrcData         Host structure for all data related to MMRC.
@retval          MMRC_SUCCESS    Timing comply with JEDEC spec
@retval          !MMRC_SUCCESS   Refer to UINT8 mErrorMessages[][] table
and enum DETECT_DIMMS_ERROR to identify error
**/
MMRC_STATUS
CalcRaplBandWidthScale(
IN  OUT   MMRC_DATA     *MrcData
);
#endif // DRAM_RAPL
#pragma pack()

#endif
