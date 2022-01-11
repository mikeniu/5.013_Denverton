/** @file
  MmrcProjectData.c
  This file contains data structures for DNV silicon. These 
  structures are fed to MmrcLibraries.c, which use them to initialize 
  memory.
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "MmrcProjectData.h"
#include "MmrcLibraries.h"


#if RMT
RMT_SIGNAL RmtTable[MaxRmtElements] = {
// Signal Name    Select run      If running     Select granularity      Pattern type              Left Side    Right Side   STROBE will       Step     Jedec Reset
//                by signal       by group       by running per bit      to be running.            per          per          run 9 strobes     Size.    Action.
//                or group.       group name.    or per strobe.                                    Signal.      Signal.      1 runs single 
//                                                                                                                           strobe.
  {RecEnDelay,    RUN_SIGNAL,     NO_ACTION,     MARGIN_PER_STROBE,      CPGC_CONFIG_VA,           RecEnLeft,   RecEnRight,  STROBE,            5,      NO_ACTION},
  {RxDqsNDelay,   RUN_GROUP,      RxDqsDelay,    MARGIN_PER_STROBE,      CPGC_CONFIG_VA,           RxDqLeft,    RxDqRight,   STROBE,            1,      FAIL_RESTORE_PI_CENTER_PERBIT},  // FAIL_SINGLE_STROBE | FAIL_RESTORE_PI_CENTER},  // IDC: Original MARGIN_PER_BIT
  {TxDqsDelay,    RUN_SIGNAL,     NO_ACTION,     MARGIN_PER_STROBE,      CPGC_CONFIG_VA,           TxDqsLeft,   TxDqsRight,  STROBE,            1,      NO_ACTION},  // IDC: Original MARGIN_PER_BIT
  {TxDqDelay2,    RUN_GROUP,      TxDqDelayG,    MARGIN_PER_STROBE,      CPGC_CONFIG_VA,           TxDqLeft,    TxDqRight,   STROBE,            1,      FAIL_RESTORE_PI_CENTER_PERBIT}, //FAIL_SINGLE_STROBE | FAIL_RESTORE_PI_CENTER},  // IDC: Original MARGIN_PER_BIT
  {RxVref,        RUN_SIGNAL,     NO_ACTION,     MARGIN_PER_STROBE,      CPGC_CONFIG_VA,           RxVLow,      RxVHigh,     STROBE,            1,      NO_ACTION},
  {TxVref,        RUN_SIGNAL,     NO_ACTION,     MARGIN_PER_STROBE,      CPGC_CONFIG_VA,           TxVLow,      TxVHigh,     STROBE,            1,      NO_ACTION},
  {CmdGrp0,       RUN_GROUP,      CmdAll,        MARGIN_PER_STROBE,      CPGC_CONFIG_CMD_STRESS,   CmdLeft,     CmdRight,         1,            5,      FAIL_SINGLE_STROBE | FAIL_RESTORE_PI_CENTER | FAIL_JEDEC_RESET | FAIL_RESTORE_DDR4_TXVREF}
};
#endif
//
// Conversion from strobe number to error mask and dynamic variable containing the error status.
// First dimension is for any system which is trained on a bytelane level. 2nd dimension is for
// any system trained on a nibble lane level.
//
STROBE_TO_ERROR_MASK StrobeToErrorMask[2][MAX_STROBES] = {
  //
  // x8
  //
  {
    {LANE_ERR_STAT03,   0x000000FF,  0},
    {LANE_ERR_STAT03,   0x0000FF00,  8},
    {LANE_ERR_STAT03,   0x00FF0000, 16},
    {LANE_ERR_STAT03,   0xFF000000, 24},
    {LANE_ERR_STAT47,   0x000000FF,  0},
    {LANE_ERR_STAT47,   0x0000FF00,  8},
    {LANE_ERR_STAT47,   0x00FF0000, 16},
    {LANE_ERR_STAT47,   0xFF000000, 24},
    {LANE_ERR_STAT_ECC, 0x000000FF,  0},
    {0xFFFF,            MMRC_NA,     0},
    {0xFFFF,            MMRC_NA,     0},
    {0xFFFF,            MMRC_NA,     0},
    {0xFFFF,            MMRC_NA,     0},
    {0xFFFF,            MMRC_NA,     0},
    {0xFFFF,            MMRC_NA,     0},
    {0xFFFF,            MMRC_NA,     0},
    {0xFFFF,            MMRC_NA,     0},
    {0xFFFF,            MMRC_NA,     0},
  },
  //
  // x4
  //
  {
    {LANE_ERR_STAT03,   0x0000000F,  0},
    {LANE_ERR_STAT03,   0x00000F00,  8},
    {LANE_ERR_STAT03,   0x000F0000, 16},
    {LANE_ERR_STAT03,   0x0F000000, 24},
    {LANE_ERR_STAT47,   0x0000000F,  0},
    {LANE_ERR_STAT47,   0x00000F00,  8},
    {LANE_ERR_STAT47,   0x000F0000, 16},
    {LANE_ERR_STAT47,   0x0F000000, 24},
    {LANE_ERR_STAT_ECC, 0x0000000F,  0},
    {LANE_ERR_STAT03,   0x000000F0,  4},
    {LANE_ERR_STAT03,   0x0000F000, 12},
    {LANE_ERR_STAT03,   0x00F00000, 20},
    {LANE_ERR_STAT03,   0xF0000000, 28},
    {LANE_ERR_STAT47,   0x000000F0,  4},
    {LANE_ERR_STAT47,   0x0000F000, 12},
    {LANE_ERR_STAT47,   0x00F00000, 20},
    {LANE_ERR_STAT47,   0xF0000000, 28},
    {LANE_ERR_STAT_ECC, 0x000000F0,  4},
    }
};

#if defined(DDR3_SUPPORT) && DDR3_SUPPORT==1
//
// DDR3L victim group assignments
//
CONST UINT8 CadbMaGroupsDdr3[MAX_MA_GROUPS_DDR3][MAX_MA_SIGNALS_DDR3] = { 
  {MA0,  MA3,  MA6,  MA9,     MA12,  MA15,    RAS},
  {MA1,  MA4,  MA7,  MA10,    MA13,  CAS,     MMRC_NA},
  {MA2,  MA5,  MA8,  MA11,    MA14,  WE,      MMRC_NA},
};
#endif
#if defined(DDR4_SUPPORT) && DDR4_SUPPORT==1
//
// DDR4 victim group assignments
//
CONST UINT8 CadbMaGroupsDdr4[MAX_MA_GROUPS_DDR4][MAX_MA_SIGNALS_DDR4] = { 
  {MA0,  MA3,  MA6,  MA9,     MA12,  MA15,    RAS},
  {MA1,  MA4,  MA7,  MA10,    MA13,  CAS,     MMRC_NA},
  {MA2,  MA5,  MA8,  MA11,    MA14,  WE,      MMRC_NA},
};
#endif

#ifdef VALID_C0_D0
CONST UINT16 SmbusAddressToClttTable[MAX_CHANNELS][MAX_DIMMS][2] = {
  {{VALID_C0_D0, SLAVE_ID_C0_D0}, {VALID_C0_D1, SLAVE_ID_C0_D1}},
  {{VALID_C1_D0, SLAVE_ID_C1_D0}, {VALID_C1_D1, SLAVE_ID_C1_D1}},
};
#endif


UINT8 NumberEyeMaskRead       = RT_NUM_EYEMASKS;
UINT8 NumberControlKnobsRead  = 2;
UINT8 NumberControlKnobsReadPN  = 3;
UINT8 NumberControlKnobsCtle = 4;


EYEMASK_T    EyeMaskRead[] = {
  {5, RT_EYEMASK0_PI},
  {5, RT_EYEMASK1_PI}
};

CONTROLKNOBS ControlKnobsRead[] = {
{
  RxDqsDelay,                        // Index;
  (UINT16)-1,                        // SharedIndex;
  RxDqBitDelay,                      // PBIndex;
  {0, 0x3F},                         // StartValue[2];
  0x3FF,                             // PBStartValue;
  {0, 0x3F},                         // Ranges[2];
  {
    1,                               // Step[0]; for PO we should use 1 later optimze for speed
    0,                               // Step[1];
    0                                // Step[2];
  },
  MMRC_ABS,                          // RelAbsFlag;
  RT_STROBE_SHARING,                 // SharedFlags;
  RT_BIT_SHARING | TIMING_SHARED | NIBBLE_PBD_X4X8_DLY,    // PBSharedFlags;
  "PI ",                             // Label[4];
  1,                                 // NumberKnobs;
  0,                                 // Phase2VrefStart;
  0,                                 // Phase2VrefStop;
  0,                                 // Phase2VrefStep;
  -1,                                // CriteriaStep;
  FALSE,                             // PBDirectFlag;
  NO_ACTION,                         // PostJedecFlags;
  RUN_PER_STROBE,                    // SweepFlags;
  1,
  5                                  // NumberSweeps;
},
{
  RxVref,                                                       // Index;
  (UINT16)-1,                                                   // SharedIndex;
  (UINT16)-1,                                                   // PBIndex;
#ifdef SIM
  {RT_STEP1_VREFSTART + 20, RT_STEP1_VREFSTART + 25},           // StartValue[2];
#else
  { RX_VREF_MIN, RX_VREF_MAX },                                 // StartValue[2];
#endif
  0,                                                            // PBStartValue;
#ifdef SIM
  {RT_STEP1_VREFMIN+20, RT_STEP1_VREFMIN+25},                   // Ranges[2];
#else
  { RX_VREF_MIN, RX_VREF_MAX },                                 // Ranges[2];
#endif
  {
    RT_STEP2_VREFSTEP,                                          // Step[0];
    0,                                                          // Step[1];
    0                                                           // Step[2];
  },
  MMRC_ABS,                                                     // RelAbsFlag;
  RANK_SHARED | KNOB_SHARED | BIT_SHARED | STROBE_GROUP_BYTE | TIMING_SHARED,   // SharedFlags;
  (UINT16)-1,                                                   // PBSharedFlags;
  "VRF",                                                        // Label[4];
  1,                                                            // NumberKnobs;
  0,                                                            // Phase2VrefStart;
  0x3F,                                                         // Phase2VrefStop;
#ifdef SIM
  5,                                                            // Phase2VrefStep;
  RT_STEP1_VREFSTEP,                                            // CriteriaStep;
#else
  2,                                                            // Phase2VrefStep;
  1,                                                            // CriteriaStep;
#endif
  FALSE,                                                        // PBDirectFlag;
  NO_ACTION,                                                    // PostJedecFlags;
  RUN_PER_STROBE,                                               // SweepFlags;
  1,
  5                                                             // NumberSweeps;
},
};
CONTROLKNOBS ControlKnobsReadCtle[] = {
{
  RxDqsPDelay,                       // Index;
  RxDqsDelay,                        // SharedIndex;
  RxDqBitDelay,                      // PBIndex;
  {0, 0x3F},                         // StartValue[2];
  0x3FF,                             // PBStartValue;
  {0, 0x3F},                         // Ranges[2];
  {
    1,                               // Step[0]; for PO we should use 1 later optimze for speed
    0,                               // Step[1];
    0                                // Step[2];
  },
  MMRC_ABS,                          // RelAbsFlag;
  RT_STROBE_SHARING,                 // SharedFlags;
  RT_BIT_SHARING | TIMING_SHARED | NIBBLE_PBD_X4X8_DLY,    // PBSharedFlags;
  "PI ",                             // Label[4];
  1,                                 // NumberKnobs;
  0,                                 // Phase2VrefStart;
  0,                                 // Phase2VrefStop;
  0,                                 // Phase2VrefStep;
  -1,                                // CriteriaStep;
  FALSE,                             // PBDirectFlag;
  NO_ACTION,                         // PostJedecFlags;
  RUN_PER_STROBE,                    // SweepFlags;
  2,
  5                                  // NumberSweeps;
},
{
  RxDqsNDelay,                       // Index;
  RxDqsDelay,                        // SharedIndex;
  RxDqBitDelay,                      // PBIndex;
  {0, 0x3F},                         // StartValue[2];
  0x3FF,                             // PBStartValue;
  {0, 0x3F},                         // Ranges[2];
  {
    1,                               // Step[0]; for PO we should use 1 later optimze for speed
    0,                               // Step[1];
    0                                // Step[2];
  },
  MMRC_ABS,                          // RelAbsFlag;
  RT_STROBE_SHARING,                 // SharedFlags;
  RT_BIT_SHARING | TIMING_SHARED | NIBBLE_PBD_X4X8_DLY,    // PBSharedFlags;
  "PI ",                             // Label[4];
  1,                                 // NumberKnobs;
  0,                                 // Phase2VrefStart;
  0,                                 // Phase2VrefStop;
  0,                                 // Phase2VrefStep;
  -1,                                // CriteriaStep;
  FALSE,                             // PBDirectFlag;
  NO_ACTION,                         // PostJedecFlags;
  RUN_PER_STROBE,                    // SweepFlags;
  2,
  5                                  // NumberSweeps;
},
{
  RxVref,                                                       // Index;
  (UINT16)-1,                                                   // SharedIndex;
  (UINT16)-1,                                                   // PBIndex;
#ifdef SIM
  {RT_STEP1_VREFSTART + 20, RT_STEP1_VREFSTART + 25},           // StartValue[2];
#else
  { RX_VREF_MIN, RX_VREF_MAX },                                 // StartValue[2];
#endif
  0,                                                            // PBStartValue;
#ifdef SIM
  {RT_STEP1_VREFMIN+20, RT_STEP1_VREFMIN+25},                   // Ranges[2];
#else
  { RX_VREF_MIN, RX_VREF_MAX },                                 // Ranges[2];
#endif
  {
    RT_STEP2_VREFSTEP,                                          // Step[0];
    0,                                                          // Step[1];
    0                                                           // Step[2];
  },
  MMRC_ABS,                                                     // RelAbsFlag;
  RANK_SHARED | KNOB_SHARED | BIT_SHARED | STROBE_GROUP_BYTE | TIMING_SHARED,   // SharedFlags;
  (UINT16)-1,                                                   // PBSharedFlags;
  "VRF",                                                        // Label[4];
  1,                                                            // NumberKnobs;
  0,                                                            // Phase2VrefStart;
  0x3F,                                                         // Phase2VrefStop;
#ifdef SIM
  5,                                                            // Phase2VrefStep;
  RT_STEP1_VREFSTEP,                                            // CriteriaStep;
#else
  2,                                                            // Phase2VrefStep;
  1,                                                            // CriteriaStep;
#endif
  FALSE,                                                        // PBDirectFlag;
  NO_ACTION,                                                    // PostJedecFlags;
  RUN_PER_STROBE,                                               // SweepFlags;
  2,
  5                                                             // NumberSweeps;
},
{
  Ctle_Wrap,                          // Index;
  (UINT16)-1,                         // SharedIndex;
  (UINT16)-1,                         // PBIndex;
  {RT_CTLE_MIN, RT_CTLE_MAX},         // StartValue[2];
  0,                                  // PBStartValue;
  {RT_CTLE_MIN, RT_CTLE_MAX},         // Ranges[2];
  {
    1,                                // Step[0];
    0,                                // Step[1];
    0                                 // Step[2];
  },
  MMRC_ABS,                           // RelAbsFlag;
  RT_CTLE_SHARED,                     // SharedFlags;
  (UINT16)-1,                         // PBSharedFlags;
  "CTL",                              // Label[4];
  1,                                  // NumberKnobs;
  0,                                  // Phase2VrefStart;
  0,                                  // Phase2VrefStop;
  1,                                  // Phase2VrefStep;
  1,                                  // CriteriaStep;
  FALSE,                              // PBDirectFlag;
  NO_ACTION,                          // PostJedecFlags;
  RUN_PER_STROBE,                     // SweepFlags;
  2,
  5                                   // NumberSweeps;
},
};
CONTROLKNOBS ControlKnobsReadPN[] = {
{
  RxDqsPDelay,                       // Index;
  RxDqsDelay,                        // SharedIndex;
  RxDqBitDelay,                      // PBIndex;
  {0, 0x3F},                         // StartValue[2];
  0x3FF,                             // PBStartValue;
  {0, 0x3F},                         // Ranges[2];
  {
    1,                               // Step[0]; for PO we should use 1 later optimze for speed
    0,                               // Step[1];
    0                                // Step[2];
  },
  MMRC_ABS,                          // RelAbsFlag;
  RT_STROBE_SHARING,                 // SharedFlags;
  RT_BIT_SHARING | TIMING_SHARED | NIBBLE_PBD_X4X8_DLY,    // PBSharedFlags;
  "PI ",                             // Label[4];
  1,                                 // NumberKnobs;
  0,                                 // Phase2VrefStart;
  0,                                 // Phase2VrefStop;
  0,                                 // Phase2VrefStep;
  -1,                                // CriteriaStep;
  FALSE,                             // PBDirectFlag;
  NO_ACTION,                         // PostJedecFlags;
  RUN_PER_STROBE,                    // SweepFlags;
  2,
  5                                  // NumberSweeps;
},
{
  RxDqsNDelay,                       // Index;
  RxDqsDelay,                        // SharedIndex;
  RxDqBitDelay,                      // PBIndex;
  {0, 0x3F},                         // StartValue[2];
  0x3FF,                             // PBStartValue;
  {0, 0x3F},                         // Ranges[2];
  {
    1,                               // Step[0]; for PO we should use 1 later optimze for speed
    0,                               // Step[1];
    0                                // Step[2];
  },
  MMRC_ABS,                          // RelAbsFlag;
  RT_STROBE_SHARING,                 // SharedFlags;
  RT_BIT_SHARING | TIMING_SHARED | NIBBLE_PBD_X4X8_DLY,    // PBSharedFlags;
  "PI ",                             // Label[4];
  1,                                 // NumberKnobs;
  0,                                 // Phase2VrefStart;
  0,                                 // Phase2VrefStop;
  0,                                 // Phase2VrefStep;
  -1,                                // CriteriaStep;
  FALSE,                             // PBDirectFlag;
  NO_ACTION,                         // PostJedecFlags;
  RUN_PER_STROBE,                    // SweepFlags;
  2,
  5                                  // NumberSweeps;
},
{
  RxVref,                                                       // Index;
  (UINT16)-1,                                                   // SharedIndex;
  (UINT16)-1,                                                   // PBIndex;
#ifdef SIM
  {RT_STEP1_VREFSTART + 20, RT_STEP1_VREFSTART + 25},           // StartValue[2];
#else
  { RX_VREF_MIN, RX_VREF_MAX },                                 // StartValue[2];
#endif
  0,                                                            // PBStartValue;
#ifdef SIM
  {RT_STEP1_VREFMIN+20, RT_STEP1_VREFMIN+25},                   // Ranges[2];
#else
  { RX_VREF_MIN, RX_VREF_MAX },                                 // Ranges[2];
#endif
  {
    RT_STEP2_VREFSTEP,                                          // Step[0];
    0,                                                          // Step[1];
    0                                                           // Step[2];
  },
  MMRC_ABS,                                                     // RelAbsFlag;
  RANK_SHARED | KNOB_SHARED | BIT_SHARED | STROBE_GROUP_BYTE | TIMING_SHARED,   // SharedFlags;
  (UINT16)-1,                                                   // PBSharedFlags;
  "VRF",                                                        // Label[4];
  1,                                           // NumberKnobs;
  0,                                                            // Phase2VrefStart;
  0x3F,                                                         // Phase2VrefStop;
#ifdef SIM
  5,                                                            // Phase2VrefStep;
  RT_STEP1_VREFSTEP,                                            // CriteriaStep;
#else
  2,                                                            // Phase2VrefStep;
  1,                                                            // CriteriaStep;
#endif
  FALSE,                                                        // PBDirectFlag;
  NO_ACTION,                                                    // PostJedecFlags;
  RUN_PER_STROBE,                                               // SweepFlags;
  2,
  5                                                             // NumberSweeps;
},
};



UINT8 NumberEyeMaskWrite        = WT_NUM_EYEMASKS;
UINT8 NumberControlKnobsWrite   = 2;
EYEMASK_T    EyeMaskWrite[] = {
  { 5, WT_EYEMASK0_PI },
  { 5, WT_EYEMASK1_PI }
};

CONTROLKNOBS ControlKnobsWrite[] = {
{
#if defined (NIBBLE_TRAINING_SUPPORT) && NIBBLE_TRAINING_SUPPORT == 0
  TxDqDelayGExt,                // Index;
#else
  TxDqDelayG,                   // Index;
#endif
  (UINT16)-1,                   // SharedIndex;
  TxDqBitDelay,                 // PBIndex;
  { 32, 32 },                   // StartValue[2];
  0x3FF,                        // PBStartValue;
  { 64, 64 },                   // Ranges[2];
  {
#if SIM
    WT_STEP1_3_PISTEPSMALL,     // Step[0];
#else
    1,                          // Step[0];
#endif
    0,                          // Step[1];
    0                           // Step[2];
  },
  REL,                          // RelAbsFlag;
  WT_STROBE_SHARING| KNOB_SHARED,// SharedFlags;
  WT_BIT_SHARING,               // PBSharedFlags;
  "PI ",                        // Label[4];
  2,                            // NumberKnobs;
  0,                            // Phase2VrefStart;
  0,                            // Phase2VrefStop;
  0,                            // Phase2VrefStep;
  -1,                           // CriteriaStep;
  TRUE,                         // PBDirectFlag;
  NO_ACTION,                    // PostJedecFlags;
  RUN_PER_STROBE,               // SweepFlags;
  1,
  5                             // NumberSweeps;
},
{
  TxVref,                                         // Index;
  (UINT16)-1,                                     // SharedIndex;
  (UINT16)-1,                                     // PBIndex;
  {30, 72},                                       // StartValue[2];
  0,                                              // PBStartValue;
  {WT_STEP1_VREFMIN,  WT_STEP1_VREFMAX},          // Ranges[2];
  {
    1,                                            // Step[0];
    0,                                            // Step[1];
    0                                             // Step[2];
  },
  MMRC_ABS,                                       // RelAbsFlag;
  KNOB_SHARED | BIT_SHARED,                       // SharedFlags;
  (UINT16)-1,                                     // PBSharedFlags;
  "VRF",                                          // Label[4];
  2,                                              // NumberKnobs;
  WT_STEP1_VREFMIN,                               // Phase2VrefStart;
  WT_STEP1_VREFMAX,                               // Phase2VrefStop;
#ifdef SIM
  WT_STEP1_VREFSTEP,                              // Phase2VrefStep;
  WT_STEP1_VREFSTEP,                              // CriteriaStep;
#else
  2,                                              // Phase2VrefStep;
  1,                                              // CriteriaStep;
#endif
  FALSE,                                          // PBDirectFlag;
  NO_ACTION,                                      // PostJedecFlags;
  RUN_PER_STROBE_LOCK,                            // SweepFlags;
  1,
  5                                               // NumberSweeps;
},
{
  TxEqualization,                                 // Index;
  (UINT16)-1,                                     // SharedIndex;
  (UINT16)-1,                                     // PBIndex;
  { (UINT16)-1, (UINT16)-1 },                     // StartValue[2];
  0,                                              // PBStartValue;
  { (UINT16)-1, 7 },                              // Ranges[2];
  {
    1,                                            // Step[0];
    0,                                            // Step[1];
    0                                             // Step[2];
  },
  MMRC_ABS,                                       // RelAbsFlag;
  KNOB_SHARED | BIT_SHARED | RANK_SHARED | STROBE_SHARED, // SharedFlags;
  (UINT16)-1,                                     // PBSharedFlags;
  "EQ",                                           // Label[4];
  2,                                              // NumberKnobs;
  0,                                              // Phase2VrefStart;
  0,                                              // Phase2VrefStop;
  0,                                              // Phase2VrefStep;
  1,                                              // CriteriaStep;
  FALSE,                                          // PBDirectFlag;
  NO_ACTION,                                      // PostJedecFlags;
  RUN_PER_STROBE,                                 // SweepFlags;
  1,
  5                                               // NumberSweeps;
}
};

CONTROLKNOBS ControlKnobsWriteDdr3[] = {
{
#if defined (NIBBLE_TRAINING_SUPPORT) && NIBBLE_TRAINING_SUPPORT == 0
  TxDqDelayGExt,                // Index;
#else
  TxDqDelayG,                   // Index;
#endif
  (UINT16)-1,                   // SharedIndex;
  TxDqBitDelay,                 // PBIndex;
  { 32, 32 },                   // StartValue[2];
  0x3FF,                        // PBStartValue;
  { 64, 64 },                   // Ranges[2];
  {
#if SIM
    WT_STEP1_3_PISTEPSMALL,     // Step[0];
#else
    1,                          // Step[0];
#endif
    0,                          // Step[1];
    0                           // Step[2];
  },
  REL,                          // RelAbsFlag;
  WT_STROBE_SHARING| KNOB_SHARED,// SharedFlags;
  WT_BIT_SHARING,               // PBSharedFlags;
  "PI ",                        // Label[4];
  2,                            // NumberKnobs;
  0,                            // Phase2VrefStart;
  0,                            // Phase2VrefStop;
  0,                            // Phase2VrefStep;
  -1,                           // CriteriaStep;
  TRUE,                         // PBDirectFlag;
  NO_ACTION,                    // PostJedecFlags;
  RUN_PER_STROBE,               // SweepFlags;
  1,
  5                             // NumberSweeps;
},
{
  TxVref,                                         // Index;
  (UINT16)-1,                                     // SharedIndex;
  (UINT16)-1,                                     // PBIndex;
  {30, 50},                                        // StartValue[2];
  0,                                              // PBStartValue;
  {0, 63},                                        // Ranges[2];
  {
    1,                                            // Step[0];
    0,                                            // Step[1];
    0                                             // Step[2];
  },
  MMRC_ABS,                                       // RelAbsFlag;
  RANK_SHARED | STROBE_SHARED | KNOB_SHARED | BIT_SHARED,                       // SharedFlags;
  (UINT16)-1,                                     // PBSharedFlags;
  "VRF",                                          // Label[4];
  2,                                              // NumberKnobs;
  30,                                              // Phase2VrefStart;
  50,                                             // Phase2VrefStop;
  2,                                              // Phase2VrefStep;
  1,                                              // CriteriaStep;
  FALSE,                                          // PBDirectFlag;
  NO_ACTION,                                      // PostJedecFlags;
  RUN_PER_STROBE_LOCK,                            // SweepFlags;
  1,
  5                                               // NumberSweeps;
},
{
  TxEqualization,                                 // Index;
  (UINT16)-1,                                     // SharedIndex;
  (UINT16)-1,                                     // PBIndex;
  { (UINT16)-1, (UINT16)-1 },                     // StartValue[2];
  0,                                              // PBStartValue;
  { (UINT16)-1, 7 },                              // Ranges[2];
  {
    1,                                            // Step[0];
    0,                                            // Step[1];
    0                                             // Step[2];
  },
  MMRC_ABS,                                       // RelAbsFlag;
  KNOB_SHARED | BIT_SHARED | RANK_SHARED | STROBE_SHARED, // SharedFlags;
  (UINT16)-1,                                     // PBSharedFlags;
  "EQ",                                           // Label[4];
  2,                                              // NumberKnobs;
  0,                                              // Phase2VrefStart;
  0,                                              // Phase2VrefStop;
  0,                                              // Phase2VrefStep;
  1,                                              // CriteriaStep;
  FALSE,                                          // PBDirectFlag;
  NO_ACTION,                                      // PostJedecFlags;
  RUN_PER_STROBE,                                 // SweepFlags;
  1,
  5                                               // NumberSweeps;
}
};


UINT8 NumberEyeMaskCmd        = CMD_NUM_EYEMASKS;
UINT8 NumberControlKnobsCmd   = 2;
EYEMASK_T    EyeMaskCmd[] = {
#ifdef SIM
  {3, 3},
  {3, 3}
#else
  {3, 10},
  {3, 6}
#endif
};

CONTROLKNOBS ControlKnobsCmd[] = {
  {
    CmdAll,                   // Index;
    (UINT16)-1,               // SharedIndex;
    (UINT16)-1,               // PBIndex;
    {0, 0},                   // StartValue[2];
    0,                        // PBStartValue;
#ifdef SIM
    {5, 5},                   // Ranges[2];
#else
    {127, 512},               // Ranges[2];
#endif
    {
      7,                      // Step[0];
      0,                      // Step[1];
      0                       // Step[2];
    },
    REL,                      // RelAbsFlag;
    CMD_BYTE_SHARING,         // SharedFlags;
    (UINT16)-1,               // PBSharedFlags;
    "PI ",                    // Label[4];
    2,                        // NumberKnobs;
    0,                        // Phase2VrefStart;
    0,                        // Phase2VrefStop;
    0,                        // Phase2VrefStep;
    -1,                       // CriteriaStep;
    0,                        // PBDirectFlag;
    FAIL_SINGLE_STROBE | FAIL_RESTORE_PI_CENTER | FAIL_JEDEC_RESET | FAIL_RESTORE_DDR4_TXVREF,  // PostJedecFlags;
    RUN_PER_STROBE,           // SweepFlags;
    1,
    3                                               // NumberSweeps;
  },
  {
    CmdVref,                  // Index;
    (UINT16)-1,               // SharedIndex;
    (UINT16) -1,              // PBIndex;
    {25, 35},                 // StartValue[2];
    0,                        // PBStartValue;
    {0, 63} ,                 // Ranges[2];
    {
      1,                      // Step[0];
      0,                      // Step[1];
      0                       // Step[2];
    },
    MMRC_ABS,                 // RelAbsFlag;
    CMD_VREF_SHARING,         // SharedFlags;
    (UINT16)-1,               // PBSharedFlags;
    "VRF",                    // Label[4];
    2,                        // NumberKnobs;
#ifdef SIM
    25,                       // Phase2VrefStart;
    35,                       // Phase2VrefStop;
#else
    0,                        // Phase2VrefStart;
    63,                       // Phase2VrefStop;
#endif
    3,                        // Phase2VrefStep;
    1,                        // CriteriaStep;
    0,                        // PBDirectFlag;
    FAIL_SINGLE_STROBE  | FAIL_JEDEC_RESET,  // PostJedecFlags;
    RUN_PER_STROBE_LOCK,      // SweepFlags;
    1,
    3                         // NumberSweeps;
  },
};



#if PPR_SUPPORT == 1

UINT16 DynamicDmapBitLocations [MAX_RANK_BG_BA_ROW_BITS] = {
  PPR_ROW0,
  PPR_ROW1,
  PPR_ROW2,
  PPR_ROW3,
  PPR_ROW4,
  PPR_ROW5,
  PPR_ROW6,
  PPR_ROW7,
  PPR_ROW8,
  PPR_ROW9,
  PPR_ROW10,
  PPR_ROW11,
  PPR_ROW12,
  PPR_ROW13,
  PPR_ROW14,
  PPR_ROW15,
  PPR_ROW16,
  PPR_ROW17,
  PPR_BA0,
  PPR_BA1,
  PPR_BG0,
  PPR_BG1,
  PPR_RS0,
  PPR_RS1
};

UINT16 MaxGuardkeyPatterns [MAX_GUARDKEY_PATTERNS] = {
  BIT11 | BIT10               | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0,
          BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0,
  BIT11         | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0,
                  BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0
};

#endif // PPR_SUPPORT
