/** @file
  DetectDimms.c
  Memory Detection and spd programming.
 
  Copyright (c) 2005-2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "DetectDimms.h"
#include "OemHooks.h"
#include "Mailbox.h"
#include "MmrcLibraries.h"
#include "NvdimmHooks.h"

extern UINT32 RaplBandWidthScaleValues[MAX_RAPL_DIMM_CONFIGS][2];

UINT16 mDdr3RelevantSpdBytes[] = {
  SPD_DDR3_TYPE                , //2       // Memory type
  SPD_DDR3_MODULE              , //3       // Module type (3:0)
  SPD_DDR3_DENBANK             , //4       // Density (6:4), Banks (3:0)
  SPD_DDR3_ADDRESS             , //5       // Row (5:3), Column (2:0) address
  SPD_DDR3_VDD                 , //6       // Module Nominal Voltage
  SPD_DDR3_ORG                 , //7       // Ranks (5:3),device width (2:0)
  SPD_DDR3_WIDTH               , //8       // Bus width ext (4:3), Bus width (2:0)
  SPD_DDR3_MTBDD               , //10      // Medium Timebase (MTB) Dividend
  SPD_DDR3_MTBDS               , //11      // Medium Timebase (MTB) Divisor
  SPD_DDR3_TCLK                , //12      // Minimum cycle time (tCKmin)
  SPD_DDR3_CLL                 , //14      // CAS latency supported, low byte
  SPD_DDR3_CLH                 , //15      // CAS latency supported, high byte
  SPD_DDR3_TAA                 , //16      // Minimum CAS latency time (tAA)
  SPD_DDR3_TWR                 , //17      // Minimum write recovery time (tWR)
  SPD_DDR3_TRCD                , //18      // Minimum RAS to CAS time (tRCD)
  SPD_DDR3_TRRD                , //19      // Minimum RA to RA time (tRRD)
  SPD_DDR3_TRP                 , //20      // Minimum precharge time (tRP)
  SPD_DDR3_TRASRC              , //21      // Upper nibbles for tRAS (7:4), tRC (3:0)
  SPD_DDR3_TRAS                , //22      // Minimum active to precharge (tRAS)
  SPD_DDR3_TRC                 , //23      // Minimum active to active/refresh (tRC)
  SPD_DDR3_TRFCL               , //24      // Minimum refresh recovery (tRFC), low byte
  SPD_DDR3_TRFCH               , //25      // Minimum refresh recovery (tRFC), high byte
  SPD_DDR3_TWTR                , //26      // Minimum internal wr to rd cmd (tWTR)
  SPD_DDR3_TRTP                , //27      // Minimum internal rd to pc cmd (tRTP)
  SPD_DDR3_TFAWH               , //28      // Upper Nibble for tFAW
  SPD_DDR3_TFAWL               , //29      // Minimum Four Activate Window Delay Time (tFAWmin), Least Significant Byte
  SPD_DDR3_TCCD                , //32      // The minimum delay, in DRAM clocks, between 2 RD/WR commands
  SPD_DDR3_TWTP                , //33      // The minimum delay, in DRAM clocks, between a WR command and a PREcommand to the same bank
  SPD_DDR3_TWCL                , //34      // The delay, in DRAM clocks, between the internal write command and the availability of the first bit of DRAM input data
  SPD_DDR3_TCMD                , //35      // The time period, in DRAM clocks, that a command occupies the DRAM command bus.
  SPD_DDR3_TCK_FTB             , //34      // Fine Offset for SDRAM Minimum Cycle Time (tCKmin)
  SPD_DDR3_TAA_FTB             , //35      // Fine Offset for Minimum CAS Latency Time (tAAmin)
  SPD_DDR3_TRCD_FTB            , //36      // Fine Offset for Minimum RAS# to CAS# Delay Time (tRCDmin)
  SPD_DDR3_TRP_FTB             , //37      // Minimum Row Precharge Delay Time (tRPmin)
  SPD_DDR3_TRC_FTB             , //38      // Fine Offset for Minimum Active to Active/Refresh Delay Time (tRCmin)
  SPD_DDR3_RAW_CARD_ID         , //62      // Raw Card ID
  SPD_DDR3_ADD_MAPPING         , //63      // Address Mapping (Odd Rank Mirror)
  SPD_DDR3_MANUFACTURER_ID_LO  , //117     // Module Manufacturer ID Code LSB
  SPD_DDR3_MANUFACTURER_ID_HI  , //118     // Module Manufacturer ID Code MSB
  SPD_DDR3_MANUFACTURE_LOCATION, //119     // Module Manufacturing Location
  SPD_DDR3_MANUFACTURE_DATE_LO , //120     // Module Manufacturing Date
  SPD_DDR3_MANUFACTURE_DATE_HI , //121     // Module Manufacturing Date
  SPD_DDR3_SERIAL_NUMBER_1     , //122     // Module Serial Number 122-125
  SPD_DDR3_SERIAL_NUMBER_2     , //123     // Module Serial Number 122-125
  SPD_DDR3_SERIAL_NUMBER_3     , //124     // Module Serial Number 122-125
  SPD_DDR3_SERIAL_NUMBER_4     , //125     // Module Serial Number 122-125
  SPD_DDR3_MODULE_PN           , //128     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_1         , //129     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_2         , //130     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_3         , //131     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_4         , //132     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_5         , //133     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_6         , //134     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_7         , //135     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_8         , //136     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_9         , //137     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_10        , //138     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_11        , //139     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_12        , //140     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_13        , //141     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_14        , //142     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_15        , //143     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_16        , //144     // Module Part Number 128-345
  SPD_DDR3_MODULE_PN_17        , //145     // Module Part Number 128-345
};

UINT16 mDdr4RelevantSpdBytes[] = {
  SPD_DDR4_SPD_REVISION  ,  //1        // Revision
  SPD_DDR3_TYPE          ,  //2        // Memory type
  SPD_DDR3_MODULE        ,  //3        // Module type (3:0)
  SPD_DDR3_DENBANK       ,  // 4       // Density (6:4), Banks (3:0)
  SPD_DDR4_ADDRESS       ,  // 5       // Row (5:3), Column (2:0) address
  SPD_DDR4_TYPE          ,  // 6       // SDRAM Device Type (DDR4)
  SPD_DDR4_OPTFEAT       ,  // 7       // SDRAM Optional Features (DDR4)
  SPD_DDR4_TRFSH         ,  // 8       // SDRAM Thermal Refresh Options (DDR4)
  SPD_DDR4_OSOF          ,  // 9       // Other SDRAM optional features (DDR4)
  SPD_DDR4_VDD           ,  // 11      // Vdd DIMM supports (DDR4)
  SPD_DDR4_ORG           ,  // 12      // Number of Ranks and SDRAM device width (DDR4)
  SPD_DDR4_WIDTH         ,  // 13      // Width of SDRAM memory bus
  SPD_DDR4_TS            ,  // 14      // Module Thermal Sensor
  SPD_DDR4_TB            ,  // 17      // Timebase [3:2] MTB, [1:0] FTB
  SPD_DDR4_MIN_TCLK      ,  // 18      // Minimum cycle time (tCKmin)
  SPD_DDR4_MAX_TCLK      ,  // 19      // Maximum cycle time
  SPD_DDR4_CAS1          ,  // 20      // CAS Latencies Supported, first byte
  SPD_DDR4_CAS2          ,  // 21      // CAS Latencies Supported, second byte
  SPD_DDR4_CAS3          ,  // 22      // CAS Latencies Supported, third byte
  SPD_DDR4_CAS4          ,  // 23      // CAS Latencies Supported, fourth byte
  SPD_DDR4_TAA           ,  // 24      // Minimum CAS Latency Time (tAAmin)
  SPD_DDR4_TRCD          ,  // 25      // Minimum RAS to CAS delay
  SPD_DDR4_TRP           ,  // 26      // Minimum Row Precharge time
  SPD_DDR4_TRASRC        ,  // 27      // Upper nibbles for min tRAS and tRC
  SPD_DDR4_TRAS          ,  // 28      // Minimum Active to Precharge time
  SPD_DDR4_TRC           ,  // 29      // Minimum Active to Active/Refresh time
  SPD_DDR4_TRFCL         ,  // 30      // Minimum Refresh Recovery time least-significant byte
  SPD_DDR4_TRFCH         ,  // 31      // Minimum Refresh Recovery time most-significant byte
  SPD_DDR4_TFAWH         ,  // 36      // Upper Nibble for tFAW
  SPD_DDR4_TFAWL         ,  // 37      // Minimum Four Activate Window Delay Time (tFAWmin), Least Significant Byte
  SPD_DDR4_TRRDS         ,  // 38      // Minimum RA to RA time (tRRDs) different bank
  SPD_DDR4_TRRDL         ,  // 39      // Minimum RA to RA time (tRRDl) same bank
  SPD_DDR4_TCCD          ,  // 40      // Minimum CAS to CAS Delay Time (tCCD_Lmin), same bank group
  SPD_DDR4_TCCD_FTB      ,  //117      // Fine offset for TCCD 
  SPD_DDR4_TRC_FTB       ,  //120      // Fine offset for TRC
  SPD_DDR4_TRP_FTB       ,  //121      // Fine offset for TRP
  SPD_DDR4_TRCD_FTB      ,  //122      // Fine offset for TRCD
  SPD_DDR4_TAA_FTB       ,  //123      // Fine offset for TAA
  SPD_DDR4_MAX_TCLK_FTB  ,  //124      // Fine offset for max TCK
  SPD_DDR4_MIN_TCLK_FTB  ,  //125      // Fine offset for min TCK
  SPD_DDR4_ADD_MAPPING   ,  //131      // Address Mapping (Odd Rank Mirror)
  SPD_DDR4_RMIDL         ,  //133      // Register Manufacturer Id (LSB)
  SPD_DDR4_RMIDH         ,  //134      // Register Manufacturer Id (MSB)
  SPD_DDR4_REGREV        ,  //135      // Register Revision Number
  SPD_DDR4_ADD_MAPPINGR  ,  //136      // Address Mapping (Odd Rank Mirror) Rdimm
  SPD_DDR4_ODS_CTL       ,  //137      // Register Ouput Drive Strength for Control
  SPD_DDR4_ODS_CK        ,  //138      // Register Ouput Drive Strength for Clock
  SPD_DDR4_MMIDL         ,  //320      // Module Manufacturer ID Code, Least Significant Byte
  SPD_DDR4_MMIDH         ,  //321      // Module Manufacturer ID Code, Mostst Significant Byte
  SPD_DDR4_MMLOC         ,  //322      // Module Manufacturing Location
  SPD_DDR4_MMDATE        ,  //323      // Module Manufacturing Date 323-324
  SPD_DDR4_MODULESN      ,  //325      // Module Serial Number 325-328
  SPD_DDR4_MODULESN_1    ,  //326      // Module Serial Number 325-328
  SPD_DDR4_MODULESN_2    ,  //327      // Module Serial Number 325-328
  SPD_DDR4_MODULESN_3    ,  //328      // Module Serial Number 325-328
  SPD_DDR4_MODULEPN      ,  //329      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_1    ,  //330      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_2    ,  //331      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_3    ,  //332      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_4    ,  //333      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_5    ,  //334      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_6    ,  //335      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_7    ,  //336      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_8    ,  //337      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_9    ,  //338      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_10   ,  //339      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_11   ,  //340      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_12   ,  //341      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_13   ,  //342      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_14   ,  //343      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_15   ,  //344      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_16   ,  //345      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_17   ,  //346      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_18   ,  //347      // Module Part Number 329-348
  SPD_DDR4_MODULEPN_19   ,  //348      // Module Part Number 329-348
  SPD_DDR4_MODULERC      ,  //349      // Module Revision Code
  SPD_DDR4_MIDCL         ,  //350      // DRAM Manufacturer ID Code, Least Significant Byte
  SPD_DDR4_MIDCH         ,  //351      // DRAM Manufacturer ID Code, Most Significant Byte
  SPD_DDR4_REV           ,  //352      // DRAM Stepping
  SPD_DDR4_CRCL          ,  //382      // LSB of 16-bit CRC
  SPD_DDR4_CRCH          ,  //383      // MSB of 16-bit CRC
};

#if DETECT_DIMMS
//
// mFrequencyMultiplier is represented
// with a 0.001 ns granularity
//
UINT16 mFrequencyMultiplier[C_MAXDDR] = {
  1250,  // 1600MHz (1.250 ns)
  1072,  // 1866MHz (1.071 ns)
   938,  // 2133MHz (0.938 ns)
   833,  // 2400MHz (0.833 ns)
   750,  // 2666MHz (0.750 ns)
};

//
// mTrfcExpected is represented with a 0.001 ns granularity.
// Relationship is between density vs time (ns) DDR3 and DDR4
//
UINT32 mTrfcExpected[2][4] = {
  //
  // 2Gb    4Gb     8Gb     16Gb (DNV TODO - UNKNOWN!!)
  // 160 ns 300 ns  350 ns  440 ns
  //        260 ns
  { 160000, 300000, 350000, 440000 },
  { 160000, 260000, 350000, 440000 }
};

//
// mTaaMin is represented with
// a 0.001 ns granularity. 1st dimension is DDR3. 2nd is DDR4.
//
UINT16 mTaaMin[C_MAXDDR][2] = {
  {10000, 12500},  // 1600MHz (10.000 ns) // DDR3/DDR4
  {10700, 12850},  // 1866MHz (10.700 ns) // DDR3/DDR4
  {0, 13130},      // 2133MHz (13.130 ns) // DDR4 only
  {0, 12500},      // 2400MHz (12.500 ns) // DDR4 only
  {0, 0}           // 2666MHz (10.000 ns) // DNV TODO - DDR4 spec not complete yet.
};

UINT8 mErrorMessages[MrcMsgMaxError][50] = {
  "All Right",                                              //  MrcMsgOk = 0,
  "DDR Type Not Supported",                                 //  MrcMsgDdrTypeNotSupported,
  "Module Type Not Supported",                              //  MrcMsgModuleTypeNotSupported,
  "Device Width Not Supported",                             //  MrcMsgDeviceWidthNotSupported,
  "Dimm 0 MUST be populated on at least 1 channel",         //  MrcMsgDimm0IsNotPopulated,
  "Number of Banks Not Supported",                          //  MrcMsgBanksNotSupported,
  "Device Density Not Supported",                           //  MrcMsgDDenNotSupported,
  "SODIMMS in a channel must have same # of ranks",         //  MrcMsgDiffNoRanksBetweenSoDimms,
  "Maximum of 2 Ranks Supported",                           //  MrcMsgMoreDrNotSupported,
  "No Dimms Detected",                                      //  MrcMsgNoDimmsPresent,
  "CL * TCK > 20ns",                                        //  MrcMsgClXTckNotLess20Ns,
  "Null Pointer Error",                                     //  MrcMsgNullPtr,
  "Memory Timing Violation (tWR, tRP, etc)",                //  MrcMsgTimingOutBoundary,
  "Mixing x8 & x16 DIMMs",                                  //  MrcMsgMixingX8X16,
  "DIMM does not support selected frequency",               //  MrcMsgDimmFreqNotSupported,
  "Non-ECC DIMM detected. Unsupported",                     //  MrcMsgNonEccDimmDetected,
  "CAS Latency Not Supported",                              //  MrcMsgCasLatencyNotSupported,
  "Unknown Failure",                                        //  MrcMsgUnknown,
  "Non-ECC SODIMM detected. Unsupported",                   //  MrcMsgNonEccSoDimmNotSupported,
  "Both channels are Disabled",                             //  MrcBothChannelAreDisabled,
  "Different DDR types and the same channel",               //  MrcDifferentDdrTypeSameChannel,
  "DDR3 VDD not Supported",                                 //  MrcMsgDdr3VddNotSupported,
  "Quad Rank are not supported",                            //  MrcMsgQrankNotSupported,
  "Module Type Mixing Not Supported",                       //  MrcMsgModuleTypeMixNotSupported,
  "Device width x8 is fused off",                           //  MrcMsgDeviceWidthx8FuseOff,
  "Device width x4 is fused off",                           //  MrcMsgDeviceWidthx4FuseOff
  "DDR3 support is fused off",                              //  MrcMsgDDR3SupportFuseOff,
  "DDR4 support is fused off"                               //  MrcMsgDDR4SupportFuseOff,
};

MRC_TASK_FUNCTION mDetectDimmTasks[] = {
  &CallIdentifyDimms,
  &FindTclTacTClk,
  &FindTrasTrpTrcd,
  &CalcDimmConfig,
  (MRC_TASK_FUNCTION)NULL
};

STRUCT_TIMING_DATA mTrasTrpTrcdDdr3Tbl[] = {
  { TimingTras, MAX_SUPPORTED_TRAS, MIN_SUPPORTED_TRAS, SPD_DDR3_TRAS,  SPD_DDR3_TRASRC, 0x0F, 0xFF, "TRAS"},
  { TimingTrp,  MAX_SUPPORTED_TRP,  MIN_SUPPORTED_TRP,  SPD_DDR3_TRP,   0,               0,    SPD_DDR3_TRP_FTB, "TRP"},
  { TimingTrcd, MAX_SUPPORTED_TRCD, MIN_SUPPORTED_TRCD, SPD_DDR3_TRCD,  0,               0,    SPD_DDR3_TRCD_FTB,"TRCD"},
  { TimingTwr,  MAX_SUPPORTED_TWR,  MIN_SUPPORTED_TWR,  SPD_DDR3_TWR,   0,               0,    0xFF, "TWR"},
  { TimingTrfc, MAX_SUPPORTED_TRFC, MIN_SUPPORTED_TRFC, SPD_DDR3_TRFCL, SPD_DDR3_TRFCH,  0xFF, 0xFF, "TRFCL"},
  { TimingTwtr, MAX_SUPPORTED_TWTR, MIN_SUPPORTED_TWTR, SPD_DDR3_TWTR,  0,               0,    0xFF, "TWTR"},
  { TimingTrrd, MAX_SUPPORTED_TRRD, MIN_SUPPORTED_TRRD, SPD_DDR3_TRRD,  0,               0,    0xFF, "TRRD"},
  { TimingTrtp, MAX_SUPPORTED_TRTP, MIN_SUPPORTED_TRTP, SPD_DDR3_TRTP,  0,               0,    0xFF, "TRTP"},
  { TimingTfaw, MAX_SUPPORTED_TFAW, MIN_SUPPORTED_TFAW, SPD_DDR3_TFAWL, SPD_DDR3_TFAWH,  0x0F, 0xFF, "TFAW"},
  { TimingTccd, MAX_SUPPORTED_TCCD, MIN_SUPPORTED_TCCD, SPD_DDR3_TCCD,  0,               0,    0xFF, "TCCD"},
  { TimingTwtp, MAX_SUPPORTED_TWTP, MIN_SUPPORTED_TWTP, SPD_DDR3_TWTP,   0,               0,    0xFF, "TWTP"},
  { TimingTwcl, MAX_SUPPORTED_TWCL, MIN_SUPPORTED_TWCL, SPD_DDR3_TWCL,  0,               0,    0xFF, "TWCL"},
  { TimingTcmd, MAX_SUPPORTED_TCMD, MIN_SUPPORTED_TCMD, SPD_DDR3_TCMD,  0,               0,    0xFF, "TCMD"},
  { TimingTcl,  MAX_SUPPORTED_TCL,  MIN_SUPPORTED_TCL,  SPD_DDR3_CLL,   SPD_DDR3_CLH,    0x7F, 0xFF, "TCL"},
  { 0xFF,                   0xFFFF,              0xFF,           0xFF,            0xFF,  0xFF, 0xFF, ""}
};


STRUCT_TIMING_DATA mTrasTrpTrcdDdr4Tbl[] = {
  { TimingTras, MAX_SUPPORTED_TRAS,      MIN_SUPPORTED_TRAS, SPD_DDR4_TRAS,  SPD_DDR4_TRASRC, 0x0F, 0xFF, "TRAS"},
  { TimingTrp,  MAX_SUPPORTED_TRP,       MIN_SUPPORTED_TRP,  SPD_DDR4_TRP,   0,               0,    SPD_DDR4_TRP_FTB, "TRP"},
  { TimingTrcd, MAX_SUPPORTED_TRCD,      MIN_SUPPORTED_TRCD, SPD_DDR4_TRCD,  0,               0,    SPD_DDR4_TRCD_FTB, "TRCD"},
  { TimingTwr,  MAX_SUPPORTED_TWR,       MIN_SUPPORTED_TWR,  SPD_DDR3_TWR,   0,               0,    0xFF, "TWR"},
  { TimingTrfc, MAX_SUPPORTED_TRFC,      MIN_SUPPORTED_TRFC, SPD_DDR4_TRFCL, SPD_DDR4_TRFCH,  0xFF, 0xFF, "TRFCL"},
  { TimingTwtr, MAX_SUPPORTED_TWTR_DDR4, MIN_SUPPORTED_TWTR, SPD_DDR3_TWTR,  0,               0,    0xFF, "TWTR"},
  { TimingTrrd, MAX_SUPPORTED_TRRD,      MIN_SUPPORTED_TRRD, SPD_DDR4_TRRDS, 0,               0,    0xFF, "TRRDS"},
  { TimingTrtp, MAX_SUPPORTED_TRTP,      MIN_SUPPORTED_TRTP, SPD_DDR3_TRTP,  0,               0,    0xFF, "TRTP"},
  { TimingTfaw, MAX_SUPPORTED_TFAW,      MIN_SUPPORTED_TFAW, SPD_DDR4_TFAWL, SPD_DDR4_TFAWH,  0x0F, 0xFF, "TFAW"},
  { TimingTccd, MAX_SUPP_DDR4_TCCD,      MIN_SUPPORTED_TCCD, SPD_DDR4_TCCD,  0,               0,    SPD_DDR4_TCCD_FTB, "TCCD"},
  { TimingTwtp, MAX_SUPPORTED_TWTP,      MIN_SUPPORTED_TWTP, SPD_DDR3_TWTP,  0,               0,    0xFF, "TWTP"},
  { TimingTwcl, MAX_SUPPORTED_TWCL,      MIN_SUPPORTED_TWCL, SPD_DDR3_TWCL,  0,               0,    0xFF, "TWCL"},
  { TimingTcmd, MAX_SUPPORTED_TCMD,      MIN_SUPPORTED_TCMD, SPD_DDR3_TCMD,  0,               0,    0xFF, "TCMD"},
  { TimingTcl,  MAX_SUPPORTED_TCL,       MIN_SUPPORTED_TCL,  SPD_DDR4_CAS1,  SPD_DDR4_CAS2,   0xFF, 0xFF, "TCL"},  // ToDo CAS3 is also needed and CAS4 in the future
  { 0xFF,                   0xFFFF,              0xFF,           0xFF,            0xFF,  0xFF, 0xFF, ""}
};

UINT32 mDimmActivateTableDdr4[MAX_ACTIVATE_CONFIGS][MAX_ACTIVATE_REGISTERS] = {
  { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, //1 //E - SR Rd 0,1,2,3 Wr 0,1,2,3
  { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, //4 //SR - E
  { 0x2, 0x1, 0x0, 0x0, 0x1, 0x2, 0x0, 0x0 }, //2 //E - DR
  { 0x0, 0x0, 0x8, 0x4, 0x0, 0x0, 0x4, 0x8 }, //8 //DR - E
  { 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x4, 0x0 }, //5 //SR - SR
  { 0x2, 0x1, 0x8, 0x4, 0x1, 0x2, 0x4, 0x8 }, //A //DR - DR
  { 0x2, 0x1, 0x0, 0x0, 0x1, 0x2, 0x4, 0x0 }, //6 //SR - DR
  { 0x0, 0x0, 0x8, 0x4, 0x1, 0x0, 0x4, 0x8 }  //9 //DR - SR
};

UINT32 mDimmActivateTableDdr3[MAX_ACTIVATE_CONFIGS][MAX_ACTIVATE_REGISTERS] = {
  {0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0},  //E - SR
  {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0},  //SR - E
  {0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x0, 0x0},  //E  - DR
  {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2},  //DR - E
  {0x4, 0x0, 0x1, 0x0, 0x5, 0x0, 0x5, 0x0},  //SR - SR
  {0x4, 0x4, 0x1, 0x1, 0x5, 0x6, 0x5, 0x9},  //DR - DR
  {0x4, 0x4, 0x1, 0x0, 0x5, 0x6, 0x5, 0x0},  //SR - DR
  {0x4, 0x0, 0x1, 0x1, 0x5, 0x0, 0x5, 0x9}   //DR - SR
};


UINT16 mTones[8] = {NOTE (26163), NOTE (29366), NOTE (32963), NOTE (34923), NOTE (39200), NOTE (44000), NOTE (49388), NOTE (26163 * 2) };

VOID
PrintMemoryError (
  IN        MMRC_DATA   *MrcData,
  IN        UINT8        IndexError
)
{
  MspDebugPrint ((MSP_DBG_MIN, "Error[%d]: %s\n", IndexError, (UINT8 *) &mErrorMessages[IndexError][0]));
  MrcData->ErrorCode = MrcErrDimmsFailed;
  return;
}


VOID
BeepOn (
  IN        UINT8   note,
  IN        UINT8   octave
)
{
  UINT16 Frequency;

  Frequency = mTones[ (note % 8)];

  if (octave - 1 >= 0) {
    Frequency >>= octave - 1;
  } else {
    Frequency <<= 1 - octave;
  }

  //
  // Set up channel 1 (used for delays)
  //
  IoOut8 (0x43, 0x54);
  IoOut8 (0x41, 0x12);
  //
  // Set up channel 2 (used by speaker)
  //
  IoOut8 (0x43, 0xb6);
  IoOut8 (0x42, (UINT8) Frequency);
  IoOut8 (0x42, (UINT8) (Frequency >> 8) );
  //
  // Turn the speaker on
  //
  IoOut8 (0x61, IoIn8 (0x61) | 3);
}

MMRC_STATUS
ReadFuses (
  IN  OUT   MMRC_DATA   *MrcData
)
/*++

Routine Description:

  Read the fuses and sets the DDR and core frequency capacity

Arguments:

  MrcData:   Host structure for all data related to MMRC.

Returns:

  Success
  Failure

--*/
{
  INT8 Channel;


  for (Channel = MAX_CHANNELS - 1; Channel >= 0; Channel--) {
    //
    // Check the DDR related fuses.
    //
    MrcData->CoreFreq    = COREFREQ_1066;
    if (MrcData->MspData.CurrentPlatform[0] == P_SIMICS || MrcData->MspData.CurrentPlatform[0] == P_EMU) {
      MrcData->MspData.DynamicVars[Channel][IPROCTRIM] = MMRC_TYPICAL;
      MrcData->MspData.DynamicVars[Channel][SCRAMBLER_SUPPORTED] = 1;
      MrcData->MspData.DynamicVars[Channel][SINGLE_CHANNEL] = 0;
      MrcData->MspData.DynamicVars[Channel][TIMING_1N_SUPPORTED] = 1;
      MrcData->MspData.DynamicVars[Channel][X4_SUPPORTED] = 1;
      MrcData->MspData.DynamicVars[Channel][X8_SUPPORTED] = 1;
      MrcData->MspData.DynamicVars[Channel][DDR4_SUPPORTED] = 1;
      MrcData->MspData.DynamicVars[Channel][DDR3_SUPPORTED] = 1;
      MrcData->MspData.DynamicVars[Channel][DOUBLE_RANK_SUPPORTED] = 1;
      MrcData->MspData.DynamicVars[Channel][POPULATE_2DPC_SUPPORTED] = 1;
      MrcData->MspData.DynamicVars[Channel][ECC_SUPPORTED] = 1;
      MrcData->MspData.DynamicVars[Channel][MAX_DEN_SUPPORTED] = 3;
      MrcData->MspData.DynamicVars[Channel][MAX_MEM_SUPPORTED] = 7;
      MrcData->MspData.DynamicVars[Channel][DDR_MAX_FREQ_LIMIT] = MAXDDR - MINDDR;
      MrcData->DdrFreqCap = MAXDDR;
    } else {
      MmrcExecuteTask (MrcData, CheckDunitFuse, NO_PRINT, Channel);
      //
      // On DNV, since these bits are negative polarity (i.e. if the fuse bit says 0, it
      // really means the fuse support is enabled), invert it here to be positive polarity.
      //
      MspDebugPrint ((MSP_DBG_MIN, "Dunit Fuse Configuration \n"));
      MrcData->MspData.DynamicVars[Channel][SCRAMBLER_SUPPORTED] = ((~MrcData->MspData.DynamicVars[Channel][SCRAMBLER_SUPPORTED]) & 1);
      MspDebugPrint ((MSP_DBG_MIN, "SCRAMBLER_SUPPORTED: %d\n", MrcData->MspData.DynamicVars[Channel][SCRAMBLER_SUPPORTED]));
#if SIM
      MrcData->MspData.DynamicVars[Channel][DDR_MAX_FREQ_LIMIT] = MAXDDR - MINDDR;
      MrcData->MspData.DynamicVars[Channel][DDR_CURRENT_FREQ]   = MAXDDR - MINDDR;
      MrcData->DdrFreqCap  = MAXDDR;
#else
      //
      // The DDR fuse values on DNV are mapped:000=ddr 1600 001=ddr 1867 010=ddr 2133 011=ddr 2400 100=ddr 2667
      // the modular MRC assigns 0x3 for DDR 1600 that's why we need to adjust. MRC will mainly use modular MRC defines as possible
      //
      MrcData->DdrFreqCap = (UINT8)MrcData->MspData.DynamicVars[Channel][DDR_MAX_FREQ_LIMIT] + MINDDR;
#endif
      MspDebugPrint ((MSP_DBG_MIN, "DDR_MAX_FREQ_LIMIT: %d \n", MrcData->MspData.DynamicVars[Channel][DDR_MAX_FREQ_LIMIT]));
      MspDebugPrint ((MSP_DBG_MIN, "DDR_CURRENT_FREQ: %d \n", MrcData->MspData.DynamicVars[Channel][DDR_CURRENT_FREQ]));
      MspDebugPrint ((MSP_DBG_MIN, "SINGLE_CHANNEL: %d \n", MrcData->MspData.DynamicVars[Channel][SINGLE_CHANNEL]));
      MspDebugPrint((MSP_DBG_MIN, "IPROCTRIM: %d \n", MrcData->MspData.DynamicVars[Channel][IPROCTRIM]));
      // 
      // Force the setup option when single channel fuse is set
      //
      if (MrcData->MspData.DynamicVars[Channel][SINGLE_CHANNEL] == 1) {
        MrcData->TotalChannels = 1;
      }
      
      MrcData->MspData.DynamicVars[Channel][TIMING_1N_SUPPORTED] = (~(MrcData->MspData.DynamicVars[Channel][TIMING_1N_SUPPORTED]) & 1);
      MspDebugPrint ((MSP_DBG_MIN, "TIMING_1N_SUPPORTED: %d\n", MrcData->MspData.DynamicVars[Channel][TIMING_1N_SUPPORTED]));
      
      MrcData->MspData.DynamicVars[Channel][X4_SUPPORTED] = (~(MrcData->MspData.DynamicVars[Channel][X4_SUPPORTED]) & 1);
      MspDebugPrint ((MSP_DBG_MIN, "X4_SUPPORTED: %d\n", MrcData->MspData.DynamicVars[Channel][X4_SUPPORTED]));
      
      MrcData->MspData.DynamicVars[Channel][X8_SUPPORTED] = (~(MrcData->MspData.DynamicVars[Channel][X8_SUPPORTED]) & 1);
      MspDebugPrint ((MSP_DBG_MIN, "X8_SUPPORTED: %d\n", MrcData->MspData.DynamicVars[Channel][X8_SUPPORTED]));
      
      MrcData->MspData.DynamicVars[Channel][DDR4_SUPPORTED] = (~(MrcData->MspData.DynamicVars[Channel][DDR4_SUPPORTED]) & 1);
      MspDebugPrint ((MSP_DBG_MIN, "DDR4_SUPPORTED: %d\n", MrcData->MspData.DynamicVars[Channel][DDR4_SUPPORTED]));
      
      MrcData->MspData.DynamicVars[Channel][DDR3_SUPPORTED] = (~(MrcData->MspData.DynamicVars[Channel][DDR3_SUPPORTED]) & 1);
      MspDebugPrint ((MSP_DBG_MIN, "DDR3_SUPPORTED: %d\n", MrcData->MspData.DynamicVars[Channel][DDR3_SUPPORTED]));
      
      MrcData->MspData.DynamicVars[Channel][DOUBLE_RANK_SUPPORTED] = (~(MrcData->MspData.DynamicVars[Channel][DOUBLE_RANK_SUPPORTED]) & 1);
      MspDebugPrint ((MSP_DBG_MIN, "DOUBLE_RANK_SUPPORTED: %d\n", MrcData->MspData.DynamicVars[Channel][DOUBLE_RANK_SUPPORTED]));
      
      MrcData->MspData.DynamicVars[Channel][POPULATE_2DPC_SUPPORTED] = (~(MrcData->MspData.DynamicVars[Channel][POPULATE_2DPC_SUPPORTED]) & 1);
      MspDebugPrint ((MSP_DBG_MIN, "POPULATE_2DPC_SUPPORTED: %d\n", MrcData->MspData.DynamicVars[Channel][POPULATE_2DPC_SUPPORTED]));
      
      MrcData->MspData.DynamicVars[Channel][ECC_SUPPORTED] = (~(MrcData->MspData.DynamicVars[Channel][ECC_SUPPORTED]) & 1);
      MspDebugPrint ((MSP_DBG_MIN, "ECC_SUPPORTED: %d\n", MrcData->MspData.DynamicVars[Channel][ECC_SUPPORTED]));
      MspDebugPrint ((MSP_DBG_MIN, "MAX_DEN_SUPPORTED: %d\n", MrcData->MspData.DynamicVars[Channel][MAX_DEN_SUPPORTED]));
      MspDebugPrint ((MSP_DBG_MIN, "MAX_MEM_SUPPORTED: %d\n", MrcData->MspData.DynamicVars[Channel][MAX_MEM_SUPPORTED]));
    }
  }

  return MMRC_SUCCESS;
}
/**
  This function sets MaxDq depending on SPD data

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          NONE
**/
VOID
SetMaxDq (
  IN         MMRC_DATA    *MrcData
  )
{
  UINT8 Channel;
  UINT8 Dimm;
  UINT8 Rank;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel]) {
      continue;
    }
    for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
      if (!MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
        continue;
      }
      for (Rank = 0; Rank < MAX_RANKS_PER_DIMM; Rank++) {
        if (MrcData->MrcParameters.RankPresent[Channel][(2 * Dimm) + Rank] == 0) {
          continue;
        }
        if (MrcData->D_DataWidth[Channel][Dimm] == 0) {
          //
          // x4
          //
          MrcData->MaxDq[Channel][(2 * Dimm) + Rank]  = MAX_STROBES;
          MrcData->MaxBit[Channel][(2 * Dimm) + Rank] = STROBE_BITS_X4_DEVICES;
        } else {
          //
          // x8 or x16
          //
          MrcData->MaxDq[Channel][(2 * Dimm) + Rank]  = MAX_STROBE_X8_DEVICES;
          MrcData->MaxBit[Channel][(2 * Dimm) + Rank] = STROBE_BITS_X8_DEVICES;
        }
        MspDebugPrint((MSP_DBG_MIN, "Ch %d, Dimm %d, Rank %d, MaxDq: %d DevWidth: %d\n", Channel, Dimm, Rank, MrcData->MaxDq[Channel][(2 * Dimm) + Rank], MrcData->MaxBit[Channel][(2 * Dimm) + Rank]));
      } // Rank
    } // Dimm
  } // Channel
}

VOID
BusWidthVariable (
  IN  OUT   MMRC_DATA   *MrcData
  ) 
{
  UINT8 BusWidth32;
  UINT8 ch;
  BusWidth32 = 0;
  //
  // if there is a 32 bit bus configuration on one channel set them all to 32 bits width
  //
  for (ch = 0; ch < MAX_CHANNELS; ch++) {
    if (!MrcData->MrcParameters.ChEnabled[ch]) continue;
    if ((MrcData->MspData.DynamicVars[ch][CHANNEL_WIDTH_32] == 1) || (MrcData->HalfWidthEnable == 1)) {
      BusWidth32 = 1;
      break;
    }
  }  // for channel
  if (BusWidth32 == 1) {
    for (ch = 0; ch < MAX_CHANNELS; ch++) {
      MrcData->MspData.DynamicVars[ch][CHANNEL_WIDTH_32] = 1;
    } // for channel
  } // if BusWidth32 
}
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
  )
{
  UINT8 CurrentDimm;

  if (!MrcData->MrcParameters.ChEnabled[Channel]) {
    return FALSE;
  }
  for (CurrentDimm = 0; CurrentDimm < MAX_DIMMS; CurrentDimm++) {
    if (!MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm]) {
      continue;
    }
    if (MrcData->D_Ranks[Channel][CurrentDimm] == 1) {
  	  return TRUE;
    }
  }
  return FALSE;
}

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
  )
{
  UINTN  i;
  MMRC_STATUS Status;
  UINT8  Channel;
  //
  // Read the fusing configuration (channels, ranks, dimms per channel, max and current freq, etc.)
  //
  Status = ReadFuses (MrcData);
  if (Status != MMRC_SUCCESS) {
    return Status;
  }

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel]) {
      continue;
    }

    Status = GetSpdData (MrcData, Channel);

    if (Status != MMRC_SUCCESS) {
      MspDebugPrint ((MSP_DBG_MED, "Status          = 0x%08x\n", Status));
      MrcData->ErrorCode = MrcErrGetSpdData;
      MrcDeadLoop();
    }
    if (MrcData->DimmCount[Channel] == 0) {
      MrcData->MrcParameters.ChEnabled[Channel]  = FALSE;
    }
  } // Channel loop ...

  //
  // Correlate channels discovered and channel fuse (single channel fuse) or setup option
  //
  if ((MrcData->TotalChannels == 1) &&  MrcData->MrcParameters.ChEnabled[0] && MrcData->MrcParameters.ChEnabled[1]) {
    MrcData->MrcParameters.ChEnabled[1] = FALSE;
    MrcData->TotalDimm = MrcData->DimmCount[0];
    MrcData->DimmCount[1] = 0;
    MspDebugPrint ((MSP_DBG_MAX, "Disabling a channel SINGLE_CHANNEL fuse: %d TotalChannels: %d\n", MrcData->MspData.DynamicVars[0][SINGLE_CHANNEL], MrcData->TotalChannels));
  }

  //
  // For 1DPC configurations, set Comp Vref to 0x26.
  // If one channel is 1DPC and the other is 2DPC, set Comp Vref to 0x29.
  // For 2DPC configurations, set Comp Vref to 0x2D.
  //
  if (((MrcData->DimmCount[0] == 1) && (MrcData->DimmCount[1] < 2)) || ((MrcData->DimmCount[0] < 2) && (MrcData->DimmCount[1] == 1))) {
    MrcData->MspData.DynamicVars[0][COMP_VREF_DPC] = 0x26;
    MrcData->MspData.DynamicVars[1][COMP_VREF_DPC] = 0x26;
  } else if (((MrcData->DimmCount[0] == 1) && (MrcData->DimmCount[1] == 2)) || ((MrcData->DimmCount[0] == 2) && (MrcData->DimmCount[1] == 1))) {
    MrcData->MspData.DynamicVars[0][COMP_VREF_DPC] = 0x29;
    MrcData->MspData.DynamicVars[1][COMP_VREF_DPC] = 0x29;
  } else {
    MrcData->MspData.DynamicVars[0][COMP_VREF_DPC] = 0x2D;
    MrcData->MspData.DynamicVars[1][COMP_VREF_DPC] = 0x2D;
  }

  if (MrcData->TotalDimm == 0) {
    PrintMemoryError (MrcData, (UINT8) MrcMsgNoDimmsPresent);
    MrcDeadLoop();
    return MMRC_FAILURE;
  }
  //
  // Determine the DDR Frequency and make SPD reset if needed
  //
  Status = EnforceCommonFreq (MrcData);
  if (Status != MMRC_SUCCESS) {
    PrintMemoryError (MrcData, (UINT8) Status);
    MrcDeadLoop();
  }

#if PUNIT_MAILBOX == 1
  //
  // VSafe flow will set the voltage depending on the DDR type 1.2V DDR4 and 1.35V DDR3
  // Some platforms will not support Vsafe flow (fixed pot) for those cases the Punit command will return
  // "mailbox_cc_illegal_vr_id" error so once we are out of PO we should ignore only this error
  //
  if (MrcData->MspData.CurrentPlatform[0] != P_SIMICS) {
    Status = VSafe (MrcData);
    if (Status != MMRC_SUCCESS && (Status != mailbox_cc_illegal_vr_id) && (MrcData->MspData.CurrentPlatform[0] != P_MEM_DOWN)) {
      MrcData->ErrorCode = MrcErrMailbox;
      MrcDeadLoop();
    }
  }
#endif

  for (i = 0; mDetectDimmTasks[i] != NULL; i++) {
    for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
      if (!MrcData->MrcParameters.ChEnabled[Channel]) {
        continue;
      }
      MspDebugPrint ((MSP_DBG_MAX, "DetectDimms[%d], Channel %d\n", i, Channel));
      Status = mDetectDimmTasks[i] (MrcData, Channel);
      if (Status != MMRC_SUCCESS) {
        PrintMemoryError (MrcData, (UINT8) Status);
        MrcDeadLoop();
        return MMRC_FAILURE;
      }
    } // Channel loop ...
    //
    // Unify the BusWidth dinamyc variable since the control is global to the system not channel independent
    //
    BusWidthVariable(MrcData);
  } // mDetectDimmTasks loop ...
  //
  //Modify Vref2 and Vref3 to 0x40 only for 1DPC 1R config
  //
  MrcData->MspData.DynamicVars[0][COMP_VREF2_DPC] = 0x23;
  MrcData->MspData.DynamicVars[1][COMP_VREF2_DPC] = 0x23;
  MrcData->MspData.DynamicVars[0][COMP_VREF3_DPC] = 0x23;
  MrcData->MspData.DynamicVars[1][COMP_VREF3_DPC] = 0x23;

  if ((!DualRankInSystem(MrcData, 0)) && (!DualRankInSystem(MrcData, 1))) {
    MspDebugPrint ((MSP_DBG_MAX, "Modifying CompVref2 and CompVref3 for 1DPC configuration \n"));
	MrcData->MspData.DynamicVars[0][COMP_VREF2_DPC] = 0x26;
    MrcData->MspData.DynamicVars[1][COMP_VREF2_DPC] = 0x26;
    MrcData->MspData.DynamicVars[0][COMP_VREF3_DPC] = 0x26;
    MrcData->MspData.DynamicVars[1][COMP_VREF3_DPC] = 0x26;
  }
  if ((MrcData->OemMrcData.EnableMemoryDown[0][0]) || (MrcData->OemMrcData.EnableMemoryDown[0][1]) || (MrcData->OemMrcData.EnableMemoryDown[1][0]) || (MrcData->OemMrcData.EnableMemoryDown[1][1])) {
	MrcData->MspData.DynamicVars[0][COMP_VREF2_DPC] = 0x30;
    MrcData->MspData.DynamicVars[1][COMP_VREF2_DPC] = 0x30;
    MrcData->MspData.DynamicVars[0][COMP_VREF3_DPC] = 0x30;
    MrcData->MspData.DynamicVars[1][COMP_VREF3_DPC] = 0x30;
  }
  //
  // Based on the results from DetectDimms, we can now choose the appropriate
  // boot path.
  // Fastboot will be executed only if we are in ColdBoot
  // If it is Fastboot, make sure the DIMMs haven't changed.
  // If they have changed, force the cold boot path with full training.
  //

  //
  // Fill current fast boot data structure
  //
  FillFastBootDataStructure (MrcData);
  if (MrcData->MrcParameters.BootMode == S5) {
    if ((MrcData->FastBootEnable) && (MrcData->MrcParameters.SaveRestore.MrcParamsValidFlag)) {
      if (CheckColdBootRequired (MrcData)) {
        MspDebugPrint ((MSP_DBG_MIN, "Cold Boot Required\n"));
        MrcData->MrcParameters.BootMode = S5;
      } else {
        MspDebugPrint ((MSP_DBG_MIN, "Fast Boot\n"));
        MrcData->MrcParameters.BootMode = FB;
      }
    } else {
      //
      // Data not valid S5 path should be executed
      //
      MrcData->MrcParameters.BootMode = S5;
    }
  }

  //
  // If Fast Boot enabled and Boot Mode is FB --> System completed MRC training and a Cold Reset is performed.
  // If Fast Boot enabled and Boot Mode is S0 --> System completed MRC training and a Warm Reset is performed.
  //
  if (((MrcData->FastBootEnable) && (MrcData->MrcParameters.BootMode == FB)) || ((MrcData->FastBootEnable) && (MrcData->MrcParameters.BootMode == S0))) {
    MspDebugPrint ((MSP_DBG_MIN, "Disabling MRC Messages\n"));
    MrcData->MspData.DebugMsgLevel = 0;
  }

  return Status;
}

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
  )
{
  MMRC_STATUS Status;

  Status = MMRC_SUCCESS;

  switch(MrcData->MrcParameters.DramType[Channel]){
    case TypeDdr3:
    case TypeDdr3L:
    case TypeDdr3U:
    case TypeDdr3All:
      Status = IdentifyDdr3Dimms (MrcData, Channel); 
      break;
    case TypeDdr4:
      Status = IdentifyDdr4Dimms (MrcData, Channel); 
      break;
    default:
      //
      // We already validated no different from DDR3/DDR4 allowed we may need DDRT at some point
      //
      break;
  }

  return Status;
}

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
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  )
{
  UINT8     CurrentDimm;
  SPD_DATA  *CurrentSpdData;
  UINT8     Ranks;
  UINT8     DimmPopulation;
  
  Ranks           = 0;
  DimmPopulation  = 0;
  
  for (CurrentDimm = 0; CurrentDimm < MAX_DIMMS; CurrentDimm++) {
    CurrentSpdData = & (MrcData->MrcParameters.SpdData[Channel][CurrentDimm]);
    MrcData->MrcParameters.RankPresent[Channel][CurrentDimm * 2]      = FALSE;
    MrcData->MrcParameters.RankPresent[Channel][CurrentDimm * 2 + 1]  = FALSE;
    if (CurrentSpdData->SpdPresent == FALSE) {
      //
      // No DIMM present...no need to proceed with SPD decode
      //
      continue;
    }
    DimmPopulation++;
    MrcData->MrcParameters.RankPresent[Channel][CurrentDimm * 2] = TRUE;
    //
    //;-----------------------------------------------------------------------;
    //  STEP 5: Verify that all DIMMs present are either UDIMM, SO-DIMM, or
    //   SO-DIMM with ECC or RDIMM.
    //    See Section 5.2.1 for information on the SDRAM Type field of the SPD.
    //;-----------------------------------------------------------------------;
    // Allow Non-ECC SODIMMs because some DIMMs have their SPDs programmed as non-ECC
    // even when they are ECC SODIMMs.  Since we don't want non-ECC SODIMMS
    // installed, we'll catch these on the data width checks (64 vs. 72 bits).
    //
    MrcData->D_Type[Channel][CurrentDimm] = CurrentSpdData->Buffer[SPD_DDR3_MODULE] & SPD_DDR3_MTYPE_MASK;
    if (((MrcData->D_Type[Channel][CurrentDimm]) ==  SPD_DDR3_UNBUFFERED) ||
        ((MrcData->D_Type[Channel][CurrentDimm]) ==  SPD_DDR3_SODIMM_ECC_U) ||
        ((MrcData->D_Type[Channel][CurrentDimm]) ==  SPD_DDR3_SODIMM)) {
      MrcData->D_Type[Channel][CurrentDimm] = CurrentSpdData->Buffer[SPD_DDR3_MODULE] & SPD_DDR3_MTYPE_MASK;
    } else {
      return MrcMsgModuleTypeNotSupported;
    }

    MrcData->D_DataWidth[Channel][CurrentDimm] = (CurrentSpdData->Buffer[SPD_DDR3_ORG] & 0x7);
    MrcData->D_Banks[Channel][CurrentDimm] = (CurrentSpdData->Buffer[SPD_DDR3_DENBANK] & 0x70) >> 4;
    if ((CurrentSpdData->Buffer[SPD_DDR3_DENBANK] & 0xF) == 2) {
      MrcData->D_Size[Channel][CurrentDimm] = (CurrentSpdData->Buffer[SPD_DDR3_DENBANK] & 0xF);
      MrcData->Dimm_1Gb[Channel] = 1;
    } else {
      MrcData->D_Size[Channel][CurrentDimm] = (CurrentSpdData->Buffer[SPD_DDR3_DENBANK] & 0xF) - 3;
    }
    Ranks = (CurrentSpdData->Buffer[SPD_DDR3_ORG] >> 3) & 0x07;

    //
    // Check for non-ECC DIMM. All DIMMs must be ECC or non-ECC in VLV platform, so 1 non-ECC DIMM
    // is enough to disable ECC for the entire platform. AVN supports both ECC and non-ECC DIMMs.
    //
    if ((CurrentSpdData->Buffer[SPD_DDR3_WIDTH] & SPD_DDR3_BWE_MASK) == 0) {
      MrcData->MrcParameters.EccEnabled[Channel] = FALSE;
    }
    MrcData->D_BusWidth[Channel][CurrentDimm] = (CurrentSpdData->Buffer[SPD_DDR3_WIDTH] & 0x7);
    //
    // if (SPD_DDR4_WIDTH & 7) == 2 we have 32 bits bus config value of 3 is 64 bits
    //
    MrcData->MspData.DynamicVars[Channel][CHANNEL_WIDTH_32] = (MrcData->D_BusWidth[Channel][CurrentDimm] == 2) ? 1 : 0;
    if (MrcData->HalfWidthEnable == 1) {
      MrcData->MspData.DynamicVars[Channel][CHANNEL_WIDTH_32] = 1;
    }
    if (MrcData->CpuType == AVN_CPU) {
      //
      // Non-ECC SODIMMS are not supported in AVN
      //
#if !defined JTAG
      if ((!MrcData->MrcParameters.EccEnabled[Channel]) && ((MrcData->D_Type[Channel][CurrentDimm]) ==  SPD_DDR3_SODIMM)) {
        return MrcMsgNonEccSoDimmNotSupported;
      }
#endif
    }
    //
    // Support x8 and x16 for DDR3.
    //
    if ((MrcData->D_DataWidth[Channel][CurrentDimm] != 1) && (MrcData->D_DataWidth[Channel][CurrentDimm] != 2)) {
      return MrcMsgDeviceWidthNotSupported;
    }
    //
    // Verify the device width x8 fuse is right
    //
    if ((!MrcData->MspData.DynamicVars[Channel][X8_SUPPORTED]) && (MrcData->D_DataWidth[Channel][CurrentDimm] == 1)) {
      return MrcMsgDeviceWidthx8FuseOff;
    }
    //
    // DDR3 only supports 8 banks
    //
    if ((MrcData->D_Banks[Channel][CurrentDimm] != 0)) {
      return MrcMsgBanksNotSupported;
    }
    //
    // Not 2Gb/ 4Gb/ 8Gb for DDR3
    //
    if (MrcData->D_Size[Channel][CurrentDimm] > 2) {
      return MrcMsgDDenNotSupported;
    }

    if (MrcData->D_Size[Channel][CurrentDimm] > MrcData->MspData.DynamicVars[Channel][MAX_DEN_SUPPORTED]) {
      MspDebugPrint ((MSP_DBG_MIN, "Warning: Device Density found:%d and fuse:%d don't match downgrading it\n", MrcData->D_Size[Channel][CurrentDimm], MrcData->MspData.DynamicVars[Channel][MAX_DEN_SUPPORTED]));
      MrcData->D_Size[Channel][CurrentDimm] = (UINT8) MrcData->MspData.DynamicVars[Channel][MAX_DEN_SUPPORTED];
    }

    MrcData->D_Ranks[Channel][CurrentDimm] = Ranks;
    if (Ranks > 0 && MrcData->MspData.DynamicVars[Channel][DOUBLE_RANK_SUPPORTED]) {
      MrcData->MrcParameters.RankPresent[Channel][ (CurrentDimm * 2) + 1] = TRUE;
    }

    if (Ranks > 0 && !MrcData->MspData.DynamicVars[Channel][DOUBLE_RANK_SUPPORTED]) {
      MspDebugPrint ((MSP_DBG_MIN, "Warning: Second rank is disabled by fuse\n"));
    }
    //
    //;-----------------------------------------------------------------------;
    //   STEP 7: Verify that all DIMMs are single-sided or double-sided.
    //    See Section 5.2.1 for information on the SDRAM Type field of the SPD.
    //;-----------------------------------------------------------------------;
    //
    if ((MrcData->DimmCount[Channel] > 0)) {
      //
      // Restriction only applies to SODIMMs
      //
      if ((MrcData->D_Type[Channel][0] ==  SPD_DDR3_SODIMM) && (MrcData->D_Type[Channel][1] ==  SPD_DDR3_SODIMM)) {
        if (MrcData->D_Ranks[Channel][0] != MrcData->D_Ranks[Channel][1]) {
          return MrcMsgDiffNoRanksBetweenSoDimms;
        }
      }
    }

    if (Ranks > 1) {
      return MrcMsgMoreDrNotSupported;
    }

    //
    //;-----------------------------------------------------------------------;
    //   STEP 8: Verify the voltage supported by the DIMM.
    //;-----------------------------------------------------------------------;
    //
    if (CurrentSpdData->Buffer[SPD_DDR3_VDD] & SPD_DDR3_L) {
      MrcData->Ddr3L[Channel][CurrentDimm] = TRUE;
      MrcData->MrcParameters.DramType[Channel] = TypeDdr3L;
    } else {
      MrcData->Ddr3L[Channel][CurrentDimm] = FALSE;
      return MrcMsgDdr3VddNotSupported;
    }
#if PASS_GATE_TEST
    MspDebugPrint ((MSP_DBG_MED, "SPD_DDR3_DRAM_MANUFACTURER_ID_HIGH: 0x%04x\n", CurrentSpdData->Buffer[SPD_DDR3_DRAM_MANUFACTURER_ID_HIGH]));
    if (CurrentSpdData->Buffer[SPD_DDR3_DRAM_MANUFACTURER_ID_HIGH] == SAMSUNG_ID) {
      MrcData->PassGateParameters.SamsungDram[Channel][CurrentDimm] = TRUE;
    } else {
      MrcData->PassGateParameters.SamsungDram[Channel][CurrentDimm] = FALSE;
    }
#endif
    MrcData->NvDimmType[Channel][CurrentDimm] = PlatformDetectNvDimm (MrcData, 0, Channel, CurrentDimm);

  } // CurrentDimm loop ...
  return MMRC_SUCCESS;
}

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
  )
{
  UINT8            CurrentDimm;
  SPD_DATA        *CurrentSpdData;
  UINT8            Ranks;
  UINT8            DimmPopulation;
  UINT8            PreviewD_Type;
  UINT16           DramManufacturerId;

  Ranks = 0;
  DimmPopulation = 0;
  PreviewD_Type = 0;
  DramManufacturerId  = 0;
  MrcData->D_NvDimm[Channel] = 0;

  for (CurrentDimm = 0; CurrentDimm < MAX_DIMMS; CurrentDimm++) {
    CurrentSpdData = & (MrcData->MrcParameters.SpdData[Channel][CurrentDimm]);
    MrcData->MrcParameters.RankPresent[Channel][CurrentDimm * 2]      = FALSE;
    MrcData->MrcParameters.RankPresent[Channel][CurrentDimm * 2 + 1]  = FALSE;
    if (CurrentSpdData->SpdPresent == FALSE) {
      //
      // No DIMM present...no need to proceed with SPD decode
      //
      continue;
    }
    DimmPopulation++;
    MrcData->MrcParameters.RankPresent[Channel][CurrentDimm * 2] = TRUE;
    //
    //;-----------------------------------------------------------------------;
    //  STEP 5: Verify that all DIMMs present are either UDIMM, SO-DIMM, or
    //   SO-DIMM with ECC or RDIMM.
    //    See Section 5.2.1 for information on the SDRAM Type field of the SPD.
    //;-----------------------------------------------------------------------;
    // Allow Non-ECC SODIMMs because some DIMMs have their SPDs programmed as non-ECC
    // even when they are ECC SODIMMs.  Since we don't want non-ECC SODIMMS
    // installed, we'll catch these on the data width checks (64 vs. 72 bits).
    //
    if (MrcData->D_NvDimm[Channel] == 0) {
      //
      // At least one NvDimm detected will be enough to use special Dmap
      //
      MrcData->D_NvDimm[Channel] = (CurrentSpdData->Buffer[SPD_DDR3_MODULE] >> 4) & 1;
    }
    MrcData->D_Type[Channel][CurrentDimm] = CurrentSpdData->Buffer[SPD_DDR3_MODULE] & SPD_DDR3_MTYPE_MASK;
    if (((MrcData->D_Type[Channel][CurrentDimm]) ==  SPD_DDR3_UNBUFFERED) || 
        ((MrcData->D_Type[Channel][CurrentDimm]) ==  SPD_DDR_RDIMM) ||
        ((MrcData->D_Type[Channel][CurrentDimm]) ==  SPD_DDR4_SODIMM_ECC_U) ||
		((MrcData->D_Type[Channel][CurrentDimm]) ==  SPD_DDR4_MINI_RDIMM) ||
		((MrcData->D_Type[Channel][CurrentDimm]) ==  SPD_DDR4_MINI_UDIMM) ||
        ((MrcData->D_Type[Channel][CurrentDimm]) ==  SPD_DDR3_SODIMM)) {
      MrcData->D_Type[Channel][CurrentDimm] = CurrentSpdData->Buffer[SPD_DDR3_MODULE] & SPD_DDR3_MTYPE_MASK;
      if (CurrentDimm == 0) {
        PreviewD_Type = MrcData->D_Type[Channel][CurrentDimm];
      } else {
        //
        // Different DIMM type is not supported RDIMM or UDIMM
        //
        if ((PreviewD_Type != MrcData->D_Type[Channel][CurrentDimm]) && (DimmPopulation > 1)) {
//          return MrcMsgModuleTypeMixNotSupported;
        }
      }
    } else {
      return MrcMsgModuleTypeNotSupported;
    }

    MrcData->D_DataWidth[Channel][CurrentDimm] = (CurrentSpdData->Buffer[SPD_DDR4_ORG] & 0x7);
    MrcData->D_Banks[Channel][CurrentDimm] = (CurrentSpdData->Buffer[SPD_DDR3_DENBANK] & 0x70) >> 4;
    MrcData->D_Size[Channel][CurrentDimm] = (CurrentSpdData->Buffer[SPD_DDR3_DENBANK] & 0xF) - 3;
    Ranks = (CurrentSpdData->Buffer[SPD_DDR4_ORG] >> 3) & 0x07;

    //
    // Check for non-ECC DIMM. All DIMMs must be ECC or non-ECC, so 1 non-ECC DIMM
    // is enough to disable ECC for the entire platform.
    //
    if ((CurrentSpdData->Buffer[SPD_DDR4_WIDTH] & SPD_DDR3_BWE_MASK) == 0) {
      MrcData->MrcParameters.EccEnabled[Channel] = FALSE;
    }
    MrcData->D_BusWidth[Channel][CurrentDimm] = (CurrentSpdData->Buffer[SPD_DDR4_WIDTH] & 0x7);
    //
    // if (SPD_DDR4_WIDTH & 7) == 2 we have 32 bits bus config, a value of 3 is 64 bits
    //
    MrcData->MspData.DynamicVars[Channel][CHANNEL_WIDTH_32] = (MrcData->D_BusWidth[Channel][CurrentDimm] == 2) ? 1 : 0;
    if (MrcData->HalfWidthEnable == 1) {
      MrcData->MspData.DynamicVars[Channel][CHANNEL_WIDTH_32] = 1;
    }
    //
    // Support only x4 on RDIMM type
    //
    if ((MrcData->D_DataWidth[Channel][CurrentDimm] == 0)  && (MrcData->D_Type[Channel][CurrentDimm] !=  SPD_DDR_RDIMM)) {
      return MrcMsgDeviceWidthNotSupported;
    }

    //
    // Verify fuse support
    //
    if ((!MrcData->MspData.DynamicVars[Channel][X4_SUPPORTED]) && (MrcData->D_DataWidth[Channel][CurrentDimm] == 0)) {
      return MrcMsgDeviceWidthx4FuseOff;
    }
    if ((!MrcData->MspData.DynamicVars[Channel][X8_SUPPORTED]) && (MrcData->D_DataWidth[Channel][CurrentDimm] == 1)) {
      return MrcMsgDeviceWidthx8FuseOff;
    }

    //
    // Not 2Gb/ 4Gb/ 8Gb /16Gb for DDR4
    //
    if (MrcData->D_Size[Channel][CurrentDimm] > 3) {
      return MrcMsgDDenNotSupported;
    }
    if (MrcData->D_Size[Channel][CurrentDimm] > MrcData->MspData.DynamicVars[Channel][MAX_DEN_SUPPORTED]) {
      MspDebugPrint ((MSP_DBG_MIN, "Warning: Device Density found:%d and fuse:%d don't match downgrading it\n", MrcData->D_Size[Channel][CurrentDimm], MrcData->MspData.DynamicVars[Channel][MAX_DEN_SUPPORTED]));
      MrcData->D_Size[Channel][CurrentDimm] = (UINT8) MrcData->MspData.DynamicVars[Channel][MAX_DEN_SUPPORTED];
    }


    MrcData->D_Ranks[Channel][CurrentDimm] = Ranks;
    if (Ranks > 0 && MrcData->MspData.DynamicVars[Channel][DOUBLE_RANK_SUPPORTED]) {
      MrcData->MrcParameters.RankPresent[Channel][ (CurrentDimm * 2) + 1] = TRUE;
    }

    if (Ranks > 0 && !MrcData->MspData.DynamicVars[Channel][DOUBLE_RANK_SUPPORTED]) {
      MspDebugPrint ((MSP_DBG_MIN, "Warning: Second rank is disabled by fuse\n"));
    }

    if (Ranks == 3) {
      return MrcMsgQrankNotSupported;
    }


#if PASS_GATE_TEST
    MspDebugPrint ((MSP_DBG_MIN, "SPD_DDR4_MMIDH: 0x%04x\n", CurrentSpdData->Buffer[SPD_DDR4_MMIDH]));
    if (CurrentSpdData->Buffer[SPD_DDR4_MMIDH] == SAMSUNG_ID) {
      MrcData->PassGateParameters.SamsungDram[Channel][CurrentDimm] = TRUE;
    } else {
      MrcData->PassGateParameters.SamsungDram[Channel][CurrentDimm] = FALSE;
    }
#endif
#if PPR_SUPPORT
    //
    // Verify if there is PPR support or not, and the type of PPR - Hard or Soft
    //
    if ((CurrentSpdData->Buffer[SPD_DDR4_OSOF] & BIT5) != 0) {
      MrcData->PprParameters.PprSoft[Channel][CurrentDimm]    = TRUE;
    } else {
      MrcData->PprParameters.PprSoft[Channel][CurrentDimm]    = FALSE;
    }
    if ((CurrentSpdData->Buffer[SPD_DDR4_OSOF] & (BIT6 | BIT7)) != 0) {
      MrcData->PprParameters.PprSupport[Channel][CurrentDimm] = TRUE;
    } else {
      MrcData->PprParameters.PprSupport[Channel][CurrentDimm] = FALSE;
      MrcData->PprParameters.PprSoft[Channel][CurrentDimm]    = FALSE;
    }
    MspDebugPrint ((MSP_DBG_MIN, "PPR: %d S/H: %d\n",  MrcData->PprParameters.PprSupport[Channel][CurrentDimm], MrcData->PprParameters.PprSoft[Channel][CurrentDimm]));
    MrcData->PprParameters.DataWidth[Channel][CurrentDimm]      = MrcData->D_DataWidth[Channel][CurrentDimm];
    MrcData->PprParameters.DeviceDensity[Channel][CurrentDimm]  = MrcData->D_Size[Channel][CurrentDimm];
#endif // PPR_SUPPORT

    MrcData->NvDimmType[Channel][CurrentDimm] = PlatformDetectNvDimm (MrcData, 0, Channel, CurrentDimm);
  } // CurrentDimm loop ...
  return MMRC_SUCCESS;
}

/**
  This function detects if any memory slot is not populated 
   
  @param[in, out]  MrcData   Host structure for all data related
                             to MMRC.
  @retval          FALSE     If both slots are populated. 
  @retval          TRUE      If any slot is empty. 
   
**/
BOOLEAN
IsAnySlotEmpty (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  )
{
  UINT8 CurrentDimm;

  for (CurrentDimm = 0;  CurrentDimm < MAX_DIMMS; CurrentDimm++) {
    if (MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm] == 0) {
      return TRUE;
    }
  }
  return FALSE;
}


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
  )
{
  UINT8             CurrentDimm;
  UINT8             DimmFreq;
  UINT8             SetupFreq;
  UINT8             Channel;
  SPD_DATA          *CurrentSpdData;
  UINT16            tckminall;
  UINT16            tck;
  UINT8             i;
  UINT16            MtbInPs;
  UINT8             CasLat3;
  UINT8             CasLat2;
  UINT8             CasLat1;
  UINT8             TclkSpd;
  INT8             TclkFtbSpd;
  //
  // CL7 through CL18 supported by the DUNIT.
  //
  MrcData->SupportedCasLat = 0x7FF8;

  //;-----------------------------------------------------------------------;
  //   STEP 9: Verify a common CAS latency is supported by all DIMMs and the DUNIT.
  //    See Section 5.2.1 for information on the SDRAM Type field of the SPD.
  //;-----------------------------------------------------------------------;
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel] || MrcData->DimmCount[Channel] == 0) {
      continue;
    }
    for (CurrentDimm = 0;  CurrentDimm <  MAX_DIMMS; CurrentDimm++) {
      CurrentSpdData = & (MrcData->MrcParameters.SpdData[Channel][CurrentDimm]);
      if (MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm]) {
        switch (MrcData->MrcParameters.DramType[Channel]) {
        case TypeDdr3:
        case TypeDdr3L:
        case TypeDdr3U:
        case TypeDdr3All:
          CasLat1 = CurrentSpdData->Buffer[SPD_DDR3_CLL];
          CasLat2 = CurrentSpdData->Buffer[SPD_DDR3_CLH];
          CasLat3 = 0;
          break;
        case TypeDdr4:
          CasLat1 = CurrentSpdData->Buffer[SPD_DDR4_CAS1];
          CasLat2 = CurrentSpdData->Buffer[SPD_DDR4_CAS2];
          CasLat3 = CurrentSpdData->Buffer[SPD_DDR4_CAS3];
          break;
        default:
          CasLat1 = CasLat2 = CasLat3 = 0;
          break;
        }
        MrcData->SupportedCasLat &= ((CasLat3 << 16) | (CasLat2 << 8) | CasLat1);
      }
    }
  } // Channel loop
  if (MrcData->SupportedCasLat == 0) {
    return MrcMsgCasLatencyNotSupported;
  }
  //;-----------------------------------------------------------------------;
  //   Step 10: Determine a common frequency and CAS latency that can be supported.
  //;-----------------------------------------------------------------------;
  tckminall = 0;
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (!MrcData->MrcParameters.ChEnabled[Channel] || MrcData->DimmCount[Channel] == 0) {
      continue;
    }

    for (CurrentDimm = 0; CurrentDimm < MAX_DIMMS; CurrentDimm++) {
      CurrentSpdData = & (MrcData->MrcParameters.SpdData[Channel][CurrentDimm]);
      if (MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm]) {
        switch (MrcData->MrcParameters.DramType[Channel]) {
        case TypeDdr3:
        case TypeDdr3L:
        case TypeDdr3U:
        case TypeDdr3All:
          MtbInPs = 1000 * CurrentSpdData->Buffer[SPD_DDR3_MTBDD] / CurrentSpdData->Buffer[SPD_DDR3_MTBDS];
          TclkSpd = CurrentSpdData->Buffer[SPD_DDR3_TCLK];
          TclkFtbSpd = CurrentSpdData->Buffer[SPD_DDR3_TCK_FTB];
          break;
        case TypeDdr4:
          //
          // According with the DDR4 SPD spec the only valid Timebase is 125 ps
          //
          MtbInPs = SPD_DDR4_MTB;
          TclkSpd = CurrentSpdData->Buffer[SPD_DDR4_MIN_TCLK];
          TclkFtbSpd = CurrentSpdData->Buffer[SPD_DDR4_MIN_TCLK_FTB];
          break;
        default:
          TclkSpd = TclkFtbSpd = 0xFF;
          MtbInPs = (UINT8) 0xFF;
          break;
        }
        tck = (TclkSpd * MtbInPs) + (INT8) TclkFtbSpd;
        for (i = MAXDDR; (char) i >= MINDDR; i--) {
          if (tck <= mFrequencyMultiplier[i - MINDDR]) {
            break;
          }
        }
        MrcData->DimmFrequency[Channel][CurrentDimm] = i;
        if (tck > tckminall) {
          tckminall = tck;
        }
      }
    } // CurrentDimm loop ...
  } // Channel loop
  //
  // Align to JEDEC standard tCK value
  //
  for (i = MAXDDR; (char) i >= MINDDR; i--) {
    if (tckminall <= mFrequencyMultiplier[i - MINDDR]) {
      break;
    }
  }
  DimmFreq = i;
  MspDebugPrint ((MSP_DBG_MIN, "DDR Common Frequency - DIMM capability: %d\n", DimmFreq));
  //
  // AUTO is greater than all other frequencies.  DdrFreqCap is the max POR
  // freq limited by SKU.
  //
  SetupFreq = MIN (MrcData->MrcParameters.DdrFrequency, MrcData->DdrFreqCap);
  MspDebugPrint ((MSP_DBG_MIN, "Setup DDR Frequency - minimum of setup and cap: %d\n", SetupFreq));
  //
  // Calculated SPD Dimm Freq
  //
  MrcData->MrcParameters.DdrFrequency = DimmFreq;
  //
  // Update CurrentFrequency based the MIN of (Fuse, Setup Option or SPD calculated Speed)
  //
  if (SetupFreq < MrcData->MrcParameters.DdrFrequency) {
    MrcData->MrcParameters.DdrFrequency = SetupFreq;
  }
  //
  // Enforce MINDDR/MAXDDR limit
  //
  if (MrcData->MrcParameters.DdrFrequency > MAXDDR) {
    MrcData->MrcParameters.DdrFrequency = MAXDDR;
  }
  if (MrcData->MrcParameters.DdrFrequency < MINDDR) {
    MrcData->MrcParameters.DdrFrequency = MINDDR;
  }
  //
  // In a configuration of 1 Dimm Per Channel installed
  // in a platform with support of 2 Slots Per Channel 
  // running @ 2666 MHz, DDR Speed should downgrade to 
  // 2400 MHz.
  //
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if ((!MrcData->MrcParameters.ChEnabled[Channel]) || (MrcData->DimmCount[Channel] == 0)) {
      continue;
    }
    if ((MrcData->MrcParameters.DdrFrequency == MMRC_2666) && (IsAnySlotEmpty(MrcData, Channel))) {
#ifndef SIM
      MrcData->MrcParameters.DdrFrequency = MMRC_2400;
#endif
    }
  }
  //
  // At this point read the PMC DDR freq and decide if SPD reset is required
  //
  VerifyResetRequests (MrcData);
  //
  // Report results
  //
  switch (MrcData->MrcParameters.DdrFrequency) {
  case MMRC_1600:
    MspDebugPrint ((MSP_DBG_MIN, "DDR Frequency : 1600\n"));
    break;
  case MMRC_1866:
    MspDebugPrint ((MSP_DBG_MIN, "DDR Frequency : 1866\n"));
    break;
  case MMRC_2133:
    MspDebugPrint ((MSP_DBG_MIN, "DDR Frequency : 2133\n"));
    break;
  case MMRC_2400:
    MspDebugPrint ((MSP_DBG_MIN, "DDR Frequency : 2400\n"));
    break;
  case MMRC_2666:
    MspDebugPrint ((MSP_DBG_MIN, "DDR Frequency : 2666\n"));
    break;
  default:
    break;
  }
  return MMRC_SUCCESS;
}
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
  )
{
  UINT8             CurrentDimm;
  UINT32            MyCasLat;
  INT8              LowCasLat;
  SPD_DATA          *CurrentSpdData;
  UINT8             CLdesired;
  UINT16            tckminall;
  UINT16            taaminall;
  UINT16            taa;
  UINT16            MtbInPs;
  UINT8             TaaSpd;
  UINT8             TaasFtbSpd;
  UINT8             CasLatLowBound;

  //
  // Now determine Common CL value
  //
  taaminall = 0;
  CasLatLowBound = 0;
  for (CurrentDimm = 0; CurrentDimm < MAX_DIMMS; CurrentDimm++) {
    CurrentSpdData = &(MrcData->MrcParameters.SpdData[Channel][CurrentDimm]);
    if (MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm]) {
      switch (MrcData->MrcParameters.DramType[Channel]) {
      case TypeDdr3:
      case TypeDdr3L:
      case TypeDdr3U:
      case TypeDdr3All:
        MtbInPs = 1000 * CurrentSpdData->Buffer[SPD_DDR3_MTBDD] / CurrentSpdData->Buffer[SPD_DDR3_MTBDS];
        TaaSpd  = CurrentSpdData->Buffer[SPD_DDR3_TAA];
        TaasFtbSpd = CurrentSpdData->Buffer[SPD_DDR3_TAA_FTB];
        CasLatLowBound = 4;
        break;
      case TypeDdr4:
        //
        // According with the DDR4 SPD spec the only valid Timebase is 125 ps
        //
        MtbInPs = SPD_DDR4_MTB;
        TaaSpd  = CurrentSpdData->Buffer[SPD_DDR4_TAA];
        TaasFtbSpd = CurrentSpdData->Buffer[SPD_DDR4_TAA_FTB];
        CasLatLowBound = 7;
        break;
      default:
        TaaSpd = TaasFtbSpd = 0xFF;
        MtbInPs = 0xFF;
        break;
      }

      taa = (TaaSpd * MtbInPs) + (INT8) TaasFtbSpd;
      if (taa > taaminall) {
        taaminall = taa;
      }
    }
  }
  //
  // Desired CL is taa/tck rounded up to nearest integer.
  //
  tckminall = mFrequencyMultiplier[MrcData->MrcParameters.DdrFrequency - MINDDR];
  CLdesired = (UINT8) (taaminall / tckminall);
  if (taaminall % tckminall) {
    CLdesired++;
  }
  MspDebugPrint ((MSP_DBG_MIN, "taaminall %d, tckminall %d, CLdesired %d\n", taaminall, tckminall, CLdesired));

  //
  // Round desired CL up to next supported value.
  //
  LowCasLat = 0;
  MyCasLat = MrcData->SupportedCasLat;
  while (MyCasLat != 0) {
    LowCasLat = BitScanReverse32 (MyCasLat) + CasLatLowBound;
    if (LowCasLat < CasLatLowBound) { 
      MspDebugPrint ((MSP_DBG_MIN, "BitScanReverse32 failed.  Probably CAS passed in was 0.\n"));
      return MrcMsgCasLatencyNotSupported;
    }
    if ((UINT8) LowCasLat == CLdesired) {
      break;
    }
    //
    // Don't need to error check this instance of BitScanReverse32() because it is identical to the one above.
    //
    MyCasLat &= ~ (UINT32) (1 << BitScanReverse32 (MyCasLat) );
  }
  //
  // Save Common CL value
  //
  MrcData->MrcParameters.TimingData[Channel][TimingTcl][FreqA] = (UINT8) LowCasLat;
  if (MrcData->TimingDataOverrides[Channel][TimingTcl][FreqA] != 0xFF) {
    MrcData->MrcParameters.TimingData[Channel][TimingTcl][FreqA] = MrcData->TimingDataOverrides[Channel][TimingTcl][FreqA];
  }
  MspDebugPrint ((MSP_DBG_MIN, "CH%d TCL %d\n", Channel, MrcData->MrcParameters.TimingData[Channel][TimingTcl][FreqA]));

  return MMRC_SUCCESS;
}
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
  IN        UINT8         Channel
  )
{
  UINT8                      CurrentDimm;
  SPD_DATA                  *CurrentSpdData;
  UINT8                      FoundGoodValue;
  UINT8                      j;
  UINT8                      Ddridx;
  UINT16                     i;
  UINT32                     Temp;
  UINT32                     MinSPDVal[MaxTimingData];
  STRUCT_TIMING_DATA         *TimingCalcPtr;
  UINT32                     ProposedTime;
  UINT8                      DdrFreqInx;
  UINT16                     MtbInPs;
  UINT8                      DdrType;
  STRUCT_TIMING_DATA         *TimingTbl = mTrasTrpTrcdDdr4Tbl;

  TimingCalcPtr = (STRUCT_TIMING_DATA *)NULL;
  DdrFreqInx = MrcData->MrcParameters.DdrFrequency - MINDDR;
  MtbInPs = 0;
  i = 0;
  DdrType = 0;
  MmrcMemset (MinSPDVal, 0, sizeof (MinSPDVal) );

  //
  // No DIMMs on this DUNIT, no need to calculate timing params.
  //
  if (MrcData->DimmCount[Channel] == 0) {
    return MMRC_SUCCESS;
  }

  for (CurrentDimm = 0; CurrentDimm < MAX_DIMMS; CurrentDimm++) {
    if (!MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm]) {
      //
      // Skip the non-present DIMMs
      //
      continue;
    }
    CurrentSpdData = & (MrcData->MrcParameters.SpdData[Channel][CurrentDimm]);

    switch (MrcData->MrcParameters.DramType[Channel]) {
    case TypeDdr3:
    case TypeDdr3L:
    case TypeDdr3U:
    case TypeDdr3All:
      MtbInPs = 1000 * CurrentSpdData->Buffer[SPD_DDR3_MTBDD] / CurrentSpdData->Buffer[SPD_DDR3_MTBDS];
      TimingTbl = mTrasTrpTrcdDdr3Tbl;
      Ddridx = 0;
      break;
    case TypeDdr4:
      if (((CurrentSpdData->Buffer[SPD_DDR4_TB] >> 2) & 0x3) == 0) {
        MtbInPs = SPD_DDR4_MTB;
      }
      TimingTbl = mTrasTrpTrcdDdr4Tbl;
      Ddridx = 1;
      break;
    default:
      MtbInPs =  0xFF;
      //
      // unsupported memory tech, get out of here
      //
      return MMRC_FAILURE;
    }
    DdrType = MrcData->MrcParameters.DramType[Channel];
    j = 0;
    while (TimingTbl[j].TimingDataIndex != 0xFF)  {
      Temp  = MtbInPs * ((((CurrentSpdData->Buffer[TimingTbl[j].Ddr3HighSpdByte] & TimingTbl[j].Ddr3HighSpdByteMask) << 8)
                            + CurrentSpdData->Buffer[TimingTbl[j].Ddr3LowSpdByte]));
      //
      // DDR4 does not have Twr/Trtp SPD byte DDR4. Jedec defined fixed values for these timings.
      //
      if (DdrType == TypeDdr4) {
        switch (TimingTbl[j].TimingDataIndex) {
        case TimingTwr:
          Temp = NANOSEC_15;    // 15ns  
          break;                                                     
        case TimingTwtr:
        case TimingTrtp:
          Temp = NANOSEC_7p5;   // 7.5ns 
          break;
        default :
          break;
        }
      }
      //
      // Add/subtract the 2's complement FTB (fine adjustment) to this timing param if applicable.
      //
      if (TimingTbl[j].Ddr3FtbSpdByte != 0xFF) {
        Temp += (INT8) CurrentSpdData->Buffer[TimingTbl[j].Ddr3FtbSpdByte];
      }
      MinSPDVal[TimingTbl[j].TimingDataIndex] = MAX (MinSPDVal[TimingTbl[j].TimingDataIndex], Temp);

      if (TimingTbl[j].TimingDataIndex == TimingTrfc) {
        if (MinSPDVal[TimingTbl[j].TimingDataIndex] != mTrfcExpected[Ddridx][MrcData->D_Size[Channel][CurrentDimm]]) {
          MspDebugPrint ((MSP_DBG_MIN, "Warning tRFC requirements are NOT met for C%dD%d\n", Channel, CurrentDimm));
          MspDebugPrint ((
            MSP_DBG_MIN,
            "tRFC: %d tRFC Expected: %d\n",
            MinSPDVal[TimingTbl[j].TimingDataIndex],
            mTrfcExpected[Ddridx][MrcData->D_Size[Channel][CurrentDimm]]
          ));
        }
      } // TimingTrfc
      j++;
    } // MaxTimingData
  } // CurrentDimm loop ...

  //
  //;-----------------------------------------------------------------------;
  //  STEP 11: Determine the smallest common tRAS for all DIMMs. See Section 5.3.3 for details.
  //  STEP 12: Determine the smallest common tRP for all DIMMs. See Section 5.3.4 for details.
  //  STEP 13: Determine the smallest common tRCD for all DIMMs. See Section 5.3.5 for details.
  //  STEP 15: Verify all DIMMs support burst length of 8. See Section 5.2.1 for details.
  //  STEP 16: Determine the smallest common tWR for all DIMMs. See Section 5.3.6 for details.
  //  STEP 17: Determine the smallest common tRFC for all DIMMs. See Section 5.3.7 for details.
  //  STEP 18: Determine the smallest common tWTR for all DIMMs. See Section 5.3.8 for details.
  //  STEP 19: Determine the smallest common tRRD for all DIMMs. See Section 5.3.9 for details.
  //  STEP 20: Determine the smallest common tRTP for all DIMMs. See Section 5.3.10 for details.
  //;-----------------------------------------------------------------------;
  //
  // Outer loop j loops though all SPD data timings in TimingDataTbl and
  // gets them calculated and stored in MrcData->MrcParameters.TimingData[Channel] array.
  //
  TimingCalcPtr = &TimingTbl[0];
  if (TimingCalcPtr == NULL) {
    return MrcMsgNullPtr;
  }

  while (TimingCalcPtr->TimingDataIndex != 0xFF) {
    j = TimingCalcPtr->TimingDataIndex;
    if ((j == TimingTccd && DdrType <= TypeDdr3All)  || j == TimingTwtp || j == TimingTwcl || j == TimingTcmd || j == TimingTcl) {
      //
      // Skip if TCCD, TWTP, TWCL or TCMD
      // These registers don't exist in SPD bytes or are calculated else where.
      //
      TimingCalcPtr++;
      continue;
    }
    //
    // Inner loop i loops through the min to max supported timing data looking for
    // best possible timing data.
    //
    FoundGoodValue = FALSE;
    if (j < MaxTimingData) {
    if (MinSPDVal[j] != 0) {
      for (i = TimingCalcPtr->Ddr3MinMCHVal; i <= TimingCalcPtr->MaxMchVal; i++) {
        ProposedTime = i * mFrequencyMultiplier[DdrFreqInx];
        if (ProposedTime >= MinSPDVal[j]) {
          FoundGoodValue = TRUE;
          MspDebugPrint ((MSP_DBG_MIN, "%-5s = %d\n", TimingCalcPtr->Name, i));
          break;
        }
      }
    }
    if ((FoundGoodValue == TRUE)) {
      if (j == TimingTrfc) {
        //
        // Special case for tRFC round up to next even number of clocks
        //
        i = (i + 1) & 0xFFFE;
      }
      MrcData->MrcParameters.TimingData[Channel][j][FreqA] = i;
    } else {
      MspDebugPrint ((MSP_DBG_MIN, "%-5s = %d - not a valid value.\n", TimingCalcPtr->Name, i));
      return MrcMsgTimingOutBoundary;
    }
    TimingCalcPtr++;
    } else {
    	MspDebugPrint ((MSP_DBG_MIN, "ASSERT: Trying to access an invalid array position \n"));
      MrcDeadLoop();
    }
  } // MaxTimingData loop ...

  return MMRC_SUCCESS;
}
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
  IN        UINT8         Channel
  )
{
  DIMM_CONFIG DimmConfig;
  UINT16 i;
  UINT32 (*ActivationTable)[MAX_ACTIVATE_CONFIGS][MAX_ACTIVATE_REGISTERS];
  //
  // No DIMMs on this DUNIT, no need to enforce population rules.
  //
  if (MrcData->DimmCount[Channel] == 0) {
    return MMRC_SUCCESS;
  }
#ifndef SIM
  //
  // DIMM0 must be populated on a given DUNIT for DDR3.
  //
  if (!MrcData->MrcParameters.DimmPresent[Channel][0] && MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
      return MrcMsgDimm0IsNotPopulated;
  }
#endif

  //
  // Both DUNITs must have identical memory configuration so populate
  // the DimmConfig structure based on the memory config in DUNIT[0]. Then
  // set DimmConfig.Bits.NumChannelsPopulated based on memory present
  // in DUNIT[1].
  //
  DimmConfig.Data = 0;
  switch (MrcData->MrcParameters.DramType[Channel]) {
  case TypeDdr3:
  case TypeDdr3L:
  case TypeDdr3U:
  case TypeDdr3All:
    DimmConfig.Bits.DdrType = 1;
    DimmConfig.Bits.PerformanceProfile = 0;
    ActivationTable = &mDimmActivateTableDdr3;
    break;
  case TypeDdr4:
    DimmConfig.Bits.DdrType = 2;
    DimmConfig.Bits.PerformanceProfile = MrcData->PerformanceProfile;
    DimmConfig.Bits.NvDimm = MrcData->D_NvDimm[Channel];
    ActivationTable = &mDimmActivateTableDdr4;
    MmrcMemset (MrcData->Dimm_1Gb, 0, (sizeof (MrcData->Dimm_1Gb[0]) * MAX_CHANNELS) );
    break;
  default :
    DimmConfig.Bits.DdrType = 0;
    return MrcMsgNullPtr;
  }

  if (MrcData->MrcParameters.DimmPresent[Channel][0]) {
    DimmConfig.Bits.Dimm0NumRanks = MrcData->D_Ranks[Channel][0] + 1;
    if (MrcData->Dimm_1Gb[Channel] == 1) {
      DimmConfig.Bits.Dimm0Density = 0;
      DimmConfig.Bits.Dimm1Gb = 1;
    } else {
      DimmConfig.Bits.Dimm0Density = 1 << MrcData->D_Size[Channel][0];
    }
    DimmConfig.Bits.Dimm0Width = MrcData->D_DataWidth[Channel][0] + 1;
  }

  if (MrcData->MrcParameters.DimmPresent[Channel][1]) {
    DimmConfig.Bits.Dimm1NumRanks = MrcData->D_Ranks[Channel][1] + 1;
    if (MrcData->Dimm_1Gb[Channel] == 1) {
      DimmConfig.Bits.Dimm1Density = 0;
    } else {
      DimmConfig.Bits.Dimm1Density = 1 << MrcData->D_Size[Channel][1];
    }
    DimmConfig.Bits.Dimm1Width = MrcData->D_DataWidth[Channel][1] + 1;
  }
  DimmConfig.Bits.BusWidth32 = (UINT32) MrcData->MspData.DynamicVars[Channel][CHANNEL_WIDTH_32];
  //
  // If the configuration is single dimm NvDimm the NvDimm indication has to be turned off due to DMAP
  //
  if ((DimmConfig.Bits.NvDimm == 1) && ((DimmConfig.Bits.Dimm0NumRanks == 0) || (DimmConfig.Bits.Dimm1NumRanks == 0))) {
    DimmConfig.Bits.NvDimm = 0;
  }
  MspDebugPrint ((MSP_DBG_MIN, "DimmConfig = 0x%08x\n", DimmConfig.Data));
  //
  // Find the index of the DIMM config from the 1056 available DIMM configs on DNV.
  //
  for (i = 0; i < MAX_DIMM_CONFIGS; i++) {
    if (mDimmConfigs[i] == DimmConfig.Data) {
      MrcData->DimmConfig[Channel] = i;
      break;
    }
  }
  MspDebugPrint ((MSP_DBG_MIN, "DimmConfigs = %d\n", MrcData->DimmConfig[Channel]));
  if (i == MAX_DIMM_CONFIGS) {
    MspDebugPrint ((MSP_DBG_MIN, "FATAL: Could not find valid DIMM configuration\n"));
    return MrcMsgUnknown;
  }
  SetMaxDq (MrcData);
  //
  // Setup the odt activation table based only on the rank population
  //
  for (i = 0; i < MAX_ACTIVATE_CONFIGS; i++) {
    if (mDimmActivate[i] == ((DimmConfig.Bits.Dimm0NumRanks & 3) | ((DimmConfig.Bits.Dimm1NumRanks & 3) << 2))) {
      //
      // ODT Activation table setup
      //
      MrcData->MspData.DynamicVars[Channel][WRODT_RANK3] = (*ActivationTable)[i][7];
      MrcData->MspData.DynamicVars[Channel][WRODT_RANK2] = (*ActivationTable)[i][6];
      MrcData->MspData.DynamicVars[Channel][WRODT_RANK1] = (*ActivationTable)[i][5];
      MrcData->MspData.DynamicVars[Channel][WRODT_RANK0] = (*ActivationTable)[i][4];
      MrcData->MspData.DynamicVars[Channel][RDODT_RANK3] = (*ActivationTable)[i][3];
      MrcData->MspData.DynamicVars[Channel][RDODT_RANK2] = (*ActivationTable)[i][2];
      MrcData->MspData.DynamicVars[Channel][RDODT_RANK1] = (*ActivationTable)[i][1];
      MrcData->MspData.DynamicVars[Channel][RDODT_RANK0] = (*ActivationTable)[i][0];
      break;
    }
  }

  if (i == MAX_ACTIVATE_CONFIGS) {
    MspDebugPrint ((MSP_DBG_MIN, "FATAL: Could not find valid DIMM activation configuration\n"));
    return MrcMsgUnknown;
  }

  return MMRC_SUCCESS;
}

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
CalcRaplBandWidthScale (
IN  OUT   MMRC_DATA     *MrcData
)
{
  RAPL_DIMM_CONFIG RaplDimmConfig;
  SPD_DATA        *CurrentSpdData;
  UINT8  ch;
  UINT8  dm;
  UINT8  i;
  UINT32 AcumulatedBandWidthScale;
  UINT32 DimmBandWidthScale;
  UINT32 AcumulatedCapacity;

  AcumulatedCapacity = 0;
  AcumulatedBandWidthScale = 0;
  DimmBandWidthScale = 0;
  for (ch = 0; ch < MAX_CHANNELS; ch++) {
    if (!MrcData->MrcParameters.ChEnabled[ch]) continue;
    for (dm = 0; dm < MAX_DIMMS; dm++) {
      if (!MrcData->MrcParameters.DimmPresent[ch][dm]) continue;
      CurrentSpdData = & (MrcData->MrcParameters.SpdData[ch][dm]);
      //
      // Get the Dimm config encoding for the look up table
      //
      RaplDimmConfig.Data = 0;
      //
      //Use 2 Gb mapping value if 1 Gb dimm is found
      //
      if (MrcData->Dimm_1Gb[ch] == 1) {
        RaplDimmConfig.Bits.DimmDensity  = 1;
      } else {
        RaplDimmConfig.Bits.DimmDensity  = 1 << MrcData->D_Size[ch][dm];
      }
      RaplDimmConfig.Bits.DimmWidth = MrcData->D_DataWidth[ch][dm] + 1;
      RaplDimmConfig.Bits.DimmNumRanks = MrcData->D_Ranks[ch][dm] + 1;
      RaplDimmConfig.Bits.DimmInChannel = ch + 1;
      RaplDimmConfig.Bits.Frequency = MrcData->MrcParameters.DdrFrequency - MINDDR;
      switch (MrcData->D_Type[ch][dm]){
      case SPD_DDR_RDIMM:
        RaplDimmConfig.Bits.DimmType = 2;
        break;
      case SPD_DDR3_UNBUFFERED:
        RaplDimmConfig.Bits.DimmType = 1;
        if ((CurrentSpdData->Buffer[SPD_DDR4_WIDTH] & SPD_DDR3_BWE_MASK) == 1) {
          RaplDimmConfig.Bits.DimmType = 3;
        }
        break;
      default:
        RaplDimmConfig.Bits.DimmType = 1;
        break;
      }
      //
      // Use the Lookup table to find Band Width scale factors for close and open page
      //
      for (i = 0; i < MAX_RAPL_DIMM_CONFIGS; i++) {
        if (RaplDimmConfigs[i] == RaplDimmConfig.Data) {
          //
          // RaplBandWidthScaleValues Index 0: Close page value / Index 1: Open page. 50% weight on each
          //
          DimmBandWidthScale = (RaplBandWidthScaleValues[i][0]/2) + (RaplBandWidthScaleValues[i][1]/2);
          DimmBandWidthScale *= MrcData->MrcParameters.SlotMem[ch][dm];
          AcumulatedBandWidthScale += DimmBandWidthScale;
          AcumulatedCapacity += MrcData->MrcParameters.SlotMem[ch][dm];
          break;
        }
      }
    }
  }
  if (AcumulatedCapacity == 0) {
    MrcData->DramRaplParameters.BandWidthScale = 0;
  } else {
    MrcData->DramRaplParameters.BandWidthScale = AcumulatedBandWidthScale / AcumulatedCapacity;
  }
  return MMRC_SUCCESS;
}
#endif // DRAM_RAPL
/** 
  This function reads SPD data and determines which slots are populated.

  @param[in]   MrcData       Host structure for all data related to MMRC.
  @param[in]   Channel       Current Channel being examined
  @retval      MMRC_SUCCESS  SPD data gotten correctly
  @retval     !MMRC_SUCCESS  Error trying to get SPD data 
**/ 
MMRC_STATUS
GetSpdData (
  IN        MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  )
{
  MMRC_STATUS    Status;
  UINT8     CurrentDimm;
  UINT8     i;
  SPD_DATA  *SpdData;
  UINT8     Length;
  UINT16    *SpdTable;
  UINT8     TableLen;
  UINT8     Dimmtype;
  UINT8     Dimmtypebackup;


  SpdData = MrcData->MrcParameters.SpdData[Channel];
  MrcData->DimmCount[Channel] = 0;
  Length = VF_SC_BYTE_LEN;
  Dimmtype  = 0;
  Dimmtypebackup = 0;

  //
  // Now examine each DIMM.
  //
  for (CurrentDimm = 0; CurrentDimm < MAX_DIMMS; CurrentDimm++) {
    MspDebugPrint ((MSP_DBG_MAX, "%s(ch:%x) Dimm:%x MemDn=%d\n", __FUNCTION__, Channel, CurrentDimm, MrcData->OemMrcData.EnableMemoryDown[Channel][CurrentDimm]));
    Status = MMRC_SUCCESS;
    //
    // OEM Hook for configuring I2C Mux(es) between Dimm SPD Reads
    //
    Status = OemI2cMux (Channel, CurrentDimm);
    if (!MrcData->OemMrcData.EnableMemoryDown[Channel][CurrentDimm]) {
      Status = MrcSmbusExec (
                 MrcData->SmbusBar,
                 MrcData->OemMrcData.SpdAddressTable[Channel][CurrentDimm],
                 SmbusReadByte,
                 SPD_DDR3_TYPE,
                 &Length,
                 &SpdData[CurrentDimm].Buffer[SPD_DDR3_TYPE]
               );
    }

    if (Status != MMRC_SUCCESS) {
      SpdData[CurrentDimm].SpdPresent = FALSE;
      MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm] = FALSE;
      continue;
    }

    Dimmtype = SpdData[CurrentDimm].Buffer[SPD_DDR3_TYPE];
    //
    // Verify the fuse match the DDR type detected. Assign the right SPD layout
    //
    switch (Dimmtype) {
    case SPD_DDR3:
      if (!MrcData->MspData.DynamicVars[Channel][DDR3_SUPPORTED]) {
        return MrcMsgDDR3SupportFuseOff;
      }
      SpdTable = &mDdr3RelevantSpdBytes[0],
      TableLen = ((sizeof (mDdr3RelevantSpdBytes)) / (sizeof (mDdr3RelevantSpdBytes[0])));
      MspDebugPrint ((MSP_DBG_MED, "DDR3 dimm detected\n"));
      MrcData->MrcParameters.DramType[Channel] = TypeDdr3;
      break;
    case SPD_DDR4:
      if (!MrcData->MspData.DynamicVars[Channel][DDR4_SUPPORTED]) {
        return MrcMsgDDR4SupportFuseOff;
      }
      SpdTable = &mDdr4RelevantSpdBytes[0],
      TableLen = ((sizeof (mDdr4RelevantSpdBytes)) / (sizeof (mDdr4RelevantSpdBytes[0])));
      MspDebugPrint ((MSP_DBG_MED, "DDR4 dimm detected\n"));
      MrcData->MrcParameters.DramType[Channel] = TypeDdr4;
      break;
    default:
#if JTAG
      SpdData[CurrentDimm].SpdPresent = FALSE;
      MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm] = FALSE;
      continue;
#else
      MspDebugPrint ((MSP_DBG_MED, "Error! Unsuported DDR type: %d \n", Dimmtype));
      return MrcMsgDdrTypeNotSupported;
#endif
    };
    //
    // Verify more than 1 DIMM configuration are of the same type
    //
    if (MrcData->DimmCount[Channel] > 1) {
      if (Dimmtypebackup != Dimmtype) {
        MspDebugPrint ((MSP_DBG_MED, "Error! Different DIMM types same channel: %d vs %d\n", Dimmtype, Dimmtypebackup));
        return MrcDifferentDdrTypeSameChannel;
      }
    }
    Dimmtypebackup = Dimmtype;
    //
    // Read SPD after selecting layout
    //
    for (i = 0; i < TableLen; i++) {
      Status = MMRC_SUCCESS;
      if (!MrcData->OemMrcData.EnableMemoryDown[Channel][CurrentDimm]) {
        Status = MrcSmbusExec (
                   MrcData->SmbusBar,
                   MrcData->OemMrcData.SpdAddressTable[Channel][CurrentDimm],
                   SmbusReadByte,
                   * (SpdTable + i),
                   &Length,
                   &SpdData[CurrentDimm].Buffer[* (SpdTable + i)]
                 );
      }
      MspDebugPrint ((MSP_DBG_MED, "C%d.D%d: SPD byte %d = 0x%X\n", Channel, CurrentDimm, * (SpdTable + i), SpdData[CurrentDimm].Buffer[* (SpdTable + i)]));
    }
    MspDebugPrint ((MSP_DBG_MED, "\n"));

    if (Status != MMRC_SUCCESS || (Status == MMRC_SUCCESS && CurrentDimm == 1 && !MrcData->MspData.DynamicVars[Channel][POPULATE_2DPC_SUPPORTED])) {

      if (Status == MMRC_SUCCESS && CurrentDimm == 1 && !MrcData->MspData.DynamicVars[Channel][POPULATE_2DPC_SUPPORTED]) {
        MspDebugPrint ((MSP_DBG_MIN, "Warning: 2DPC is disabled by fuse\n"));
      }
      SpdData[CurrentDimm].SpdPresent = FALSE;
      MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm] = FALSE;
    } else {
      SpdData[CurrentDimm].SpdPresent = TRUE;
      MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm] = TRUE;
      MrcData->DimmCount[Channel]++;
      MrcData->TotalDimm++;
    }
  }

  return MMRC_SUCCESS;
}


#endif // DETECT_DIMMS

#if BDAT
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
  )
{
  if (RelevantSpdBytes == NULL || SpdElements == NULL) {
    MspDebugPrint ((MSP_DBG_MIN, "Null pointer(s) passed.\n"));
    return;
  }
  if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
    *RelevantSpdBytes = mDdr3RelevantSpdBytes;
    *SpdElements      = ((sizeof (mDdr3RelevantSpdBytes)) / (sizeof (mDdr3RelevantSpdBytes[0])));
  } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
    *RelevantSpdBytes = mDdr4RelevantSpdBytes;
    *SpdElements      = ((sizeof (mDdr4RelevantSpdBytes)) / (sizeof (mDdr4RelevantSpdBytes[0])));
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "Unkown DramType.\n"));
    *RelevantSpdBytes = NULL;
    *SpdElements      = 0;
  }
  return;
}
#endif // BDAT

