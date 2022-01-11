/** @file
  MMRCData.h
  Internal and external data structures, Macros, and enumerations.
 
  Copyright (c) 2005-2017 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#ifndef _MMRCDATA_H_
#define _MMRCDATA_H_

#include "Spd.h"
#include "MmrcProjectDefinitions.h"
#include "MSysProg.h"
#if SIM || JTAG
#include <stdio.h>
#endif

//
// Define each stepping in the MMRC taking the values
// generated in the spreadsheet
//
#define STEPPING_X0   STEPPING_X0_SI
#define STEPPING_A0   STEPPING_A0_SI
#define STEPPING_A1   STEPPING_A1_SI
#define STEPPING_B0   STEPPING_B0_SI
#define STEPPING_B1   STEPPING_B1_SI

//
// Skip options
//
#define SKIP_DELAY    BIT0
#define SKIP_TEST     BIT1
#define SKIP_MEMINIT  BIT2

#if MRC_FLOW
#if !defined SIM  && !defined JTAG
#define PRINT_FUNCTION_INFO     MspDebugPrint ((MSP_DBG_MIN, "%a\n", __FUNCTION__ ));
#define PRINT_FUNCTION_INFO_MAX MspDebugPrint ((MSP_DBG_MAX, "%a\n", __FUNCTION__ ));
#else
#define PRINT_FUNCTION_INFO     MspDebugPrint ((MSP_DBG_MIN, "%s\n", __FUNCTION__ ));
#define PRINT_FUNCTION_INFO_MAX MspDebugPrint ((MSP_DBG_MAX, "%s\n", __FUNCTION__ ));
#endif
#else
#define PRINT_FUNCTION_INFO     
#define PRINT_FUNCTION_INFO_MAX 
#endif

#define MIN(a,b)          (((a) < (b)) ? (a) : (b))
#define MAX(a,b)          (((a) > (b)) ? (a) : (b))
#define GET_ABSOLUTE(a,b) (((a) > (b)) ? (a - b) : (b - a))  
#define MMRC_NA           0xFF

#define EXTERNAL      0xFE
#define INTERNAL      0xFE

#define GETSETVREF    0
#define GETSETDELAY   1
#define GETCMD        2
#define NONE          3
#define LOW           0
#define HIGH          1

#define RUN_PER_STROBE              BIT0                               //< Results per strobe
#define RUN_PER_BIT                 BIT1                               //< Results per bit
#define RUN_PER_BIT_AFTER_STROBE    (BIT2 | RUN_PER_BIT)               //< Internal variable, never attempt to use it as input parameter
#define RUN_PER_STROBE_AND_BIT      (RUN_PER_STROBE | RUN_PER_BIT)     //< Results per strobe and per bit (Bit0 of each strobe contains the strobe result)
#define RUN_LOCK_STEP_MODE           BIT3                              //< Runs all Strobes in locked-step
                                                                       //< Searches across channels until all Strobes/Bits passed or failed before going to high side
                                                                       //< Internal variable, never attempt to use it as input parameter
#define RUN_PER_BIT_LOCK            (RUN_LOCK_STEP_MODE | RUN_PER_BIT)            //< Results per bit in locked-step mode across strobes/channels
#define RUN_PER_STROBE_LOCK         (RUN_LOCK_STEP_MODE | RUN_PER_STROBE)         //< Results per strobe in locked-step mode across channels
#define RUN_PER_STROBE_BIT_LOCK     (RUN_LOCK_STEP_MODE | RUN_PER_STROBE_AND_BIT) //< Results per strobe/bit and in locked-step mode across channels
#define RUN_RESULT_BIT0_INDEPENDENT (BIT4)                             //< Do not restore Sx['b0'] after the PerBit phase is executed
#define RUN_ALL_MASK                (BIT5-1)                           //< Mask for all bits, never attempt to use it as input parameter

#define MAX_THRESHOLD_PER_BIT_VOC     1
#define MAX_THRESHOLD_PER_BIT_RWT     5

#ifndef ABSOLUTE 
#define ABSOLUTE                      1
#define RELATIVE                      2
#endif
#define CONSOLIDATE_STROBES           4
#define MULTIKNOB                     8
#define SWEEP_ALL_STROBES_TOGETHER    16
#define START_SWEEP_CENTER            32
#define FOUND_PASS_CONDITION          64
//
// Definitions for specific entries when accessing 1DSweep return data.
//
#define KNOB_0            0
#define KNOB_1            1
//
// Bit field settings for determining which rd/wr training delay controls are shared.
//
#define RANK_SHARED           1
#define STROBE_SHARED         4
#define KNOB_SHARED           2
#define BIT_SHARED            8
#define BIT_CONTROL           16
#define CHANNEL_SHARED        32
#define CALCULATE_BIT         64    // When BIT is not shared, the per bit can be calculated.
#define STROBE_GROUP_BYTE     128   // When signal is bytelane level and x4 support is POR. on x4 2 strobes would be affected by 1 shared signal
#define HW_RESTRICTION_2X     256   // When 2x is shared across Ranks.
#define TIMING_SHARED         512   // Support for more than 1 main timing signal for the controlknobs
#define NIBBLE_PBD_X4X8_DLY   1024  // When the delay must be done on Byte or Nibble Mode, but PBD must always be done per Nibble.
#define SWEEP_BIT         0   // When BIT is not shared, the per bit must be swept.
#define NO_SHARED         0

#define MAX_COMPOSITE_COUNT     5
#define MAX_EDGES               7

#define PMOP_FAST_EXIT     1
#define PMOP_SLOW_EXIT     0

//
// MEMHOT
//
#define MEMHOT_DISABLED       0
#define MEMHOT_MID            1
#define MEMHOT_HI             2
#define MEMHOT_CRIT           3

//
// Frequencies
//
#define MMRC_800           0x00
#define MMRC_1066          0x01
#define MMRC_1333          0x02
#define MMRC_1600          0x03
#define MMRC_1866          0x04
#define MMRC_2133          0x05
#define MMRC_2400          0x06
#define MMRC_2666          0x07
#define MMRC_2933          0x08
#define MMRC_3200          0x09
#define MMRC_3777          0x0A

//
// IPROCTRIM
//
#define MMRC_TYPICAL       0x02
#define MMRC_UNUSED        0x00
#define MMRC_SLOW          0x01
#define MMRC_FAST          0x03

#define C_MAXDDR          MAXDDR - MINDDR + 1

#define COREFREQ_800      0x00
#define COREFREQ_1066     0x01
#define COREFREQ_1333     0x02
#define COREFREQ_1600     0x03

#define C_MAXCOREFREQ     MAXCOREFREQ - MINCOREFREQ + 1

#define STROBE 0

#define RUN_GROUP          1
#define RUN_SIGNAL         0
#define MARGIN_PER_BIT     TRUE
#define MARGIN_PER_STROBE  FALSE

#define MMRC_ENTRY         1
#define MMRC_EXIT          0

//
// Our current tools don't know about the RDRAND instruction yet.
// Note that this encoding will not work in a 16bit code segment.
// In 64bit CPU mode, this will fill RAX but we still only
// use the low 32 bits.
//
#define RDRAND_EAX  __asm _emit 0x0f __asm _emit 0xc7 __asm _emit 0xf0

typedef enum {
  FreqA,
  FreqB,
  MaxDvfsFreqs
} DVFS_FREQUENCIES;

typedef enum {
  MrcMemInit,
  MrcMemTest
} MEM_INIT_OR_TEST;

typedef enum {
  MrcFastBootRxNDqs,
  MrcFastBootRxPDqs,
  MrcFastBootRxVref,
  MrcFastBootTxDq,
  MrcFastBootTxVref,
  MaxMrcFastBootKnobs
} MRC_FAST_BOOT_CHECKER_KNOBS;

typedef enum {
  MrcNoHaltSystemOnError,
  MrcHaltSystemOnError,
  MrcHaltCpgcEngineOnError,
  MrcNoHaltCpgcEngineOnError,
  MrcMaxHaltType
} HALT_TYPE;

typedef enum {
  TimingTras = 0,
  TimingTrp,
  TimingTrcd,
  TimingTwr,
  TimingTrfc,
  TimingTwtr,
  TimingTrrd,
  TimingTrrdl,
  TimingTrtp,
  TimingTfaw,
  TimingTccd,
  TimingTccdl,
  TimingTwtp,
  TimingTwcl,
  TimingTcmd,
  TimingTcl,
  TimingTrppb,
  TimingTrpab,
  TimingTrcpb,
  TimingTwtrl,
  TimingTrdpre,
  TimingTwrpre,
  TimingTrefi,
  TimingTxs,
  TimingTxsdll,
  TimingTxsr,
  TimingTckesre,
  TimingTxp,
  TimingTxpdll,
  TimingTcke,
  TimingTpd,
  TimingTprpden,
  TimingTrdpden,
  TimingTwrpden,
  TimingTrefpden,
  TimingTmrspden,
  TimingTcpded,
  TimingTmprr,
  TimingTmrd,
  TimingTmod,
  TimingTmrr,
  TimingTmrw,
  TimingOdtlon,
  TimingOdtloff,
  TimingOdth4,
  TimingOdth8,
  TimingTaonpdmin,
  TimingTaonpd,
  TimingTaofpdmin,
  TimingTaofpdmax,
  TimingTrpre,
  TimingTwpre,
  TimingTrrsr,
  TimingTrrsrl,
  TimingTrrdr,
  TimingTrrdd,
  TimingTwwsr,
  TimingTwwsrl,
  TimingTwwdr,
  TimingTwwdd,
  TimingTrwsr,
  TimingTrwdr,
  TimingTrwdd,
  TimingTwrsr,
  TimingTwrdr,
  TimingTwrdd,
  TimingTxpr,
  TimingTdllk,
  TimingTzqinit,
  TimingTzqoper,
  TimingTzqcs,
  TimingTzqreset,
  MaxTimingData
} TIMING_DATA_TYPE;


#define EVEN_MODE             1
#define ODD_MODE              0

#define OVERRIDE              1
#define NO_OVERRIDE           0

#define ZEROS                 0x00
#define ONES                  0xff

#define NOT_MET               0
#define MET                   1
#define MET2                  2
#define UP                    1
#define DN                    0

#define COUNT_UP_FOR_ONE      1
#define COUNT_DN_FOR_ZERO     0

#define LOC_FLAGS_U(x)        (((x[6])>>5) & 1)

#define RDWR_NOT_DONE         0
#define RDWR_DONE             1
#define RDWR_LOW              0
#define RDWR_HIGH             1

//
// RD/WR Leveling
//
#define RD_LEVELING           1
#define WR_LEVELING           2

#define CMD_THRESHOLD         10
#define CMDLOWHIGH_NONE       0
#define CMDLOWHIGH_LOW        1
#define CMDLOWHIGH_HIGH       2
#define CMDLOWHIGH_BOTH       3
#define LOOKING_FOR_FAILURE   0
#define FOUND_FAILURE         1
#define PHASE0_SEARCH_LOW     0
#define PHASE0_SEARCH_HIGH    1
#define PHASE0_FOUND_HIGH     2

#define REG_UNDEFINED           0xFF
#define DELAY_TYPE_UNDEFINED    0xFFFF
// 
// Macros for converting Little-Endian to Big-Endian.
//
#define CONV1(x)              (x&0xff)
#define CONV2(x)              ((x>>0)&0xff), ((x>>8)&0xff)
#define CONV3(x)              ((x>>0)&0xff), ((x>>8)&0xff), ((x>>16)&0xff)
#define CONV4(x)              ((x>>0)&0xff), ((x>>8)&0xff), ((x>>16)&0xff), ((x>>24)& 0xff)
#define CONV5(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff)
#define CONV6(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff), ((x>>40)& 0xff)
#define CONV7(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff), ((x>>40)& 0xff), ((x>>48)& 0xff)
#define CONV8(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff), ((x>>40)& 0xff), ((x>>48)& 0xff), ((x>>56)& 0xff)

// 
// Macros for MASK/VAL which are used in the projectlibraries.c declarations.
//
#define MASK(x)               CONV4(x)
#define VAL(x)                CONV4(x)

#define ASSIGNDONE            (0xff)
#define MSP_NA                (0xFF)
#define DETREGLIST_NULL       ((DETAILED_REGISTER_STRING *) 0xFFFFFFFF)
#define REGLIST_NULL          ((REGISTER_STRING *)    0xFFFFFFFF)

//
// RMT Definitions
//
#define RMT_SUPPORT_ENABLED           BIT0    // Enables RMT
#define RMT_SUPPORT_EV_MODE           BIT1    // Increases the Loopcount and Burst Number
#define RMT_SUPPORT_COMMAND_MODE      BIT2    // Enables commands thru SKPD registers
#define RMT_SUPPORT_FULL_EV_MODE      BIT3    // Enables Full Ev Mode
#define RMT_SUPPORT_OVERRIDE          BIT4    // Overrides the loopcount & burst number to match the ones used during training

#define NO_ACTION                       0
#define PIMOVE_JEDEC_RESET              BIT0    // Executes a Jedec reset prior to start any test
#define FAIL_RESTORE_CMD                BIT1    
#define FAIL_JEDEC_RESET                BIT2    // Executes a Jedec reset after a failure is detected
#define FAIL_RESTORE_PI                 BIT3
#define FAIL_RESTORE_PI_LAST_VALUE      BIT4
#define FAIL_RESTORE_PI_CENTER          BIT5    // Restores the center of the delay after a failure is detected
#define FAIL_SINGLE_STROBE              BIT6    // Detects any failure across any bits/strobes/knob and stops the test
                                              // (All bits, strobes and knobs are considered as failures within the same rank)
#define FAIL_SINGLE_BIT                 BIT7    // Detects any failure across any bit within an strobe and stops the test
                                              // (Error is propagated in BIT0 per failing strobe)
#define FAIL_SPREAD_BIT_RESULT          BIT8    // Detects any failure across any bit within an strobe and stops the test
                                              // (Error is propagated across ALL bits per failing strobe)
#define FAIL_RESTORE_DDR4_TXVREF        BIT9
#define FAIL_RESTORE_PI_CENTER_PERBIT   BIT10   // Restores PI Center if all bits in the strobe fails

typedef enum {
  RmtCommandNoCommand = 0,
  RmtCommandSetLoopCount,
  RmtCommandSetNumBursts,
  RmtCommandExit,
  RmtCommandRun,
  RmtCommandEnableSignal,
  RmtCommandType
} RMT_COMMANDS_TYPE;

typedef enum {
  TypeCache = 0,
  TypeCachePB,
  TypeCacheNonSR,
  TypeCacheNonSRPB,
  MaxCacheTypes
} CACHE_TYPES;
// 
// Read Value from cache
//
#define CMD_GET_CACHE         (RD_ONLY)
//
// Read from register
// 
#define CMD_GET_REG           (RD_ONLY|RD_REG)
//
// Read from register and update Value to cache
//
#define CMD_GET_REG_UC        (RD_ONLY|RD_REG|UPD_CACHE)
// 
// Read register from cache, apply offset, write back to register and update cache
//
#define CMD_SET_OFFSET_UC     (WR_OFF|FC_WR|UPD_CACHE)
// 
// Write to register with input Value, force write with NO Value condition checking and update final Value to cache
//
#define CMD_SET_VAL_FC_UC     (FC_WR|UPD_CACHE)
// 
// Write to register with input Value, force write with NO Value condition checking
//
#define CMD_SET_VAL_FC        (FC_WR)

#define STATUS_PASS                0
#define STATUS_FAIL                1
#define STATUS_TYPE_NOT_SUPPORTED  2
#define STATUS_CMD_NOT_SUPPORTED   3
#define STATUS_LIMIT               4

//
// Voltages
// Valid DDR Voltages
//
#define DDR_VOLTAGE_NOT_DETECTED  0
#define DDR_VOLTAGE_125V          1
#define DDR_VOLTAGE_135V          2
#define DDR_VOLTAGE_150V          3
//
// Invalid DDR Voltages
// Specifies the Vboot, usually a change of voltage needs to be done
//
#define DDR_VOLTAGE_100V          10
//
// VDDQ is fixed at some range so there is no need to change it.
//
#define DDR_VOLTAGE_FIXED_125V    21 
#define DDR_VOLTAGE_FIXED_135V    22 
#define DDR_VOLTAGE_FIXED_150V    23 

#ifdef MINIBIOS
#define Shr64(Value, Shift) LShiftU64 (Value, Shift)
#define Shl64(Value, Shift) LRightU64 (Value, Shift)
#endif

#define ODD_MODE_BITSHIFTS    16
#define EVEN_MODE_BITSHIFTS   1

//
// 1D/2D Eye Diagram Debug Message Parameters
//
#define MARGIN_DISTANCE(x,y)     ((x)>(y) ? (x)-(y) : (y)-(x)) 
#define MAX_LINE_SIZE            80   
#define	EYEDATA_1D_VREFPI_OFFSET 0    

typedef enum {
  MmrcNotStarted = 0,
  MmrcStarted,
  MmrcFinished
} MMRC_STATUS_FLOW;

typedef enum {
  TypeDdr3 = 0,
  TypeDdr3L,
  TypeDdr3U,
  TypeDdr3All,
  TypeLpDdr2,
  TypeLpDdr3,
  TypeLpDdr4,
  TypeDdr4,
  TypeWio2,
  TypeDdrT,
  MaxMemType
} MEM_TYPE;

#pragma pack(1)
//
// Power Knobs supported
//
typedef enum {
  SocOdt = 0,
  DramRttWrOdt,
  DramRttNomOdt,
  MaxPowerKnobs
} POWER_KNOBS;

typedef enum {
  GlobalFirstTimeExecution = 0, // First time execution
  TimingBelowTrigger,           // Trigger when the timing margin doesn't comply with the specs
  VoltageBelowTrigger,          // Trigger when the voltage margin doesn't comply with the specs
  ExecuteTrainingTrigger,       // When there was a drop of 'x' number of ticks and we need to re-train
  FailureDetectedOnFirstKnob,   // If a failure is detected during the really first Power Knob value do not continue execution
  MaxTriggerLog
} TRIGGER_LOG;

typedef enum {
  ExecuteRead = 0,
  ExecuteWrite,
  MaxTrainingRequired
} TRAINING_REQUIRED;

typedef enum {
  MarginT,
  MarginV,
  MaxMarginVerify
} MARGIN_VERIFY;

typedef enum {
  Absolute = 0,         // We should never go behind this value
  AbsoluteTrigger,      // Once the absolute trigger has been reached, so we are close to the edge
  Relative,             // After the absoluteTrigger has been reached make sure the margin doesn't drop drastically
  ExecuteTrainingDrop,  // If the Margin Drops to much Re-Execute the required training steps to re-center
  MaxPowerCriteria
} POWER_CRITERIA;
//
// Supported DRAM typed for Power Training
//
typedef enum {
  PowerDdr3l = 0,
  PowerLpddr3,
  MaxDdrTypesSupportedForPower
} DDR_TYPES_SUPPORTED_FOR_POWER;
// 
// Allocated from Stronger to weaker
//
typedef enum {
  Odt20,
  Odt30,
  Odt40,
  Odt60,
  Odt90,
  Odt120,
  Odt150,
  Odt180,
  Odt210,
  Odt240,
  OdtInfinite,     // "OFF"
  MaxOdtValues,
  InvalidOdtValue
} ODT_VALUES;

#define   SOC_ODT_KNOB_ENABLE         BIT0
#define   DIMM_RTTNOM_KNOB_ENABLE     BIT1
#define   DIMM_RTTWR_KNOB_ENABLE      BIT2
#define   DEBUG_RMT_ENABLE            BIT3

/**
  Strobe to CPGC error status bit conversion
**/
typedef struct {
  UINT16 DynamicVar;
  UINT32 ErrorMask;
  UINT8  OffsetShift;
} STROBE_TO_ERROR_MASK;

/**
  RMT Per Bit Report Printing Limits
**/
#define SECURE_OVR_FLOW    0
#define STROBE3            3
#define STROBE7            7

/**
  Initial values for each power nob  
**/
typedef struct {
  UINT8   InitialValue[MaxDdrTypesSupportedForPower][MaxPowerKnobs];
  UINT8   FinalValue[MaxDdrTypesSupportedForPower][MaxPowerKnobs];
  BOOLEAN Execute[MaxTrainingRequired]; // Only used when both Read & Write Training need to be executed
} POWER_KNOB_VALUES;

//
// Rank Margin Tool Data Index
//
typedef enum {
  RecEnLeft,
  RecEnRight,
  RxDqLeft,
  RxDqRight,
  RxVLow,
  RxVHigh,
  TxDqsLeft,
  TxDqsRight,
  TxDqLeft,
  TxDqRight,
  TxVLow,
  TxVHigh,
  CmdLeft, 
  CmdRight,
  MaxRMTData,
} RMT_DATA_TYPE;

enum {
  CPGC_CMDSEQ_WRITE, 
  CPGC_CMDSEQ_READ, 
  CPGC_CMDSEQ_WRITE_READ
};

//
// One for each type of distinct CPGC algo support.  Used by big case statement in CpgcSetup,..
//
enum {
  CPGC_CONFIG_MEMINIT = 0,
  CPGC_CONFIG_MEMTEST,
  CPGC_CONFIG_VA,
  CPGC_CONFIG_EARLYTRAINING,
  CPGC_CONFIG_EARLY_MPR_TRAINING,
  CPGC_CONFIG_TURNAROUNDS,
  CPGC_CONFIG_LFSR_ADDRESS,
  CPGC_CONFIG_CMD_STRESS
} CPGC_CONFIG_TYPES;

enum {
  CPGC_CHUNK_EVEN, 
  CPGC_CHUNK_ODD, 
  CPGC_CHUNK_ALL
};

typedef enum {
  SidebandPort,
  AddressOffset
} INSTANCE_OFFSET_TYPE;

typedef struct {
  UINT8   SpdPresent;
  UINT8   Buffer[MAX_SPD_ADDR + 1];
} SPD_DATA;

#define RCV_ENABLE_CHECKER    200
#define FWL_ENABLE_CHECKER    201

#define RCV_MIN_VALUE         70
#define RCV_MAX_VALUE         900

#define FWL_MIN_VALUE         70
#define FWL_MAX_VALUE         900
//
// Layout definitions  of the elements array.
//
#define REG_INDEX               0x3000                                  // Starting index for the delay group registers.
#define GROUP_INDEX             0x4000                                  // Starting index for the Signal Group Type Index.
#define UNIQUE_INDEX            0x5000                                  // Starting index for generic registers that aren't tied to a delay group.
#define ALGO_REG_INDEX          0x1000                                  // Starting index in the TYPE field for Signal Group Modules per blueprint.
#define SGTI_INDEX              0x2000                                  // Starting index for the Signal Group Type Index per blueprint.
#define GSM_GT_INDEX            0xE000                                  // Starting index for GSM_GT enumerated type.
#define GSM_CSN_INDEX           0xF000                                  // Starting index for GSM_CSN enumerated type.
#define INDIV_ELEMENT_INDEX     (NumberAlgos * NumberOfElementsPerAlgo) // Starting index of non-linear delay elements.

//
// The GetSet functions support a level parameter to select the appropriate DDRIO interface in the memory hierarchy
//
typedef enum {
  PreDdrLevel   = 0,        // Near memory before DDR such as wide IO. 
  DdrLevel,                 // DDR memory.
  LrbufLevel,               // Refers to data level at backside of LRDIMM or AEP buffer
  RegLevelA,                // Refers to cmd level at backside of register  side A
  RegLevelB,                // Refers to cmd level at backside of register  side B
  GsmLtMax,
  GsmLtDelim    = 0xFF
} GSM_LT;

typedef enum {
  RecEnDelay    = GSM_GT_INDEX, 
  RxDqsDelay, 
  RxDqDelay,
  RxDqsPDelay, 
  RxDqsNDelay,
  RxVref, 
  RxEq,
  RxDqBitDelay,
  RxVoc, 
  RxOdt,    
  RxOdtUp,
  RxOdtDn, 
  DramDrvStr,   
  WrLvlDelay, 
  TxDqsDelay, 
  TxDqDelay,
  TxVref,
  TxEq,
  TxDqBitDelay,
  TxRon,  
  TxRonUp, 
  TxRonDn, 
  TxSlewRate,
  TxImode,
  WrOdt,
  NomOdt,
  ParkOdt,
  TxTco,    
  CmdAll, 
  CmdGrp0, 
  CmdGrp1, 
  CmdGrp2,
  CtlAll, 
  CtlGrp0, 
  CtlGrp1, 
  CtlGrp2, 
  CtlGrp3, 
  CtlGrp4, 
  CtlGrp5,
  CmdCtlAll,
  CkAll,
  CmdVref,
  AlertVref, 
  CmdRon,
  EridDelay,
  EridVref,
  ErrorVref, 
  ReqVref,
  GsmGtMax,
  GsmGtDelim  = 0xFFFF 
} GSM_GT;

typedef enum {
  SigRasN     = GSM_CSN_INDEX, 
  SigCasN, 
  SigWeN,
  SigBa0, 
  SigBa1, 
  SigBa2,
  SigA0, 
  SigA1, 
  SigA2, 
  SigA3, 
  SigA4, 
  SigA5, 
  SigA6, 
  SigA7, 
  SigA8, 
  SigA9, 
  SigA10, 
  SigA11, 
  SigA12, 
  SigA13, 
  SigA14, 
  SigA15,
  SigA16,
  SigA17,
  SigCs0N, 
  SigCs1N, 
  SigCs2N, 
  SigCs3N, 
  SigCs4N, 
  SigCs5N, 
  SigCs6N, 
  SigCs7N, 
  SigCs8N, 
  SigCs9N,
  SigCke0, 
  SigCke1, 
  SigCke2, 
  SigCke3, 
  SigCke4, 
  SigCke5,
  SigOdt0,     // Could also be used for CA-ODT for LP4
  SigOdt1,     // Could also be used for CA-ODT for LP4
  SigOdt2, 
  SigOdt3, 
  SigOdt4, 
  SigOdt5,
  SigPar, 
  SigAlertN,
  SigBg0, 
  SigBg1, 
  SigActN, 
  SigCid0, 
  SigCid1, 
  SigCid2,
  SigCk0, 
  SigCk1, 
  SigCk2, 
  SigCk3, 
  SigCk4, 
  SigCk5,
  SigGnt0,
  SigGnt1,
  SigErid00,
  SigErid01,
  SigErid10,
  SigErid11,
  SigErr0,
  SigErr1,
  SigCa00,    // First instantiation of the CA bus for a given channel
  SigCa01,
  SigCa02, 
  SigCa03, 
  SigCa04, 
  SigCa05, 
  SigCa10,    // Second instantiation of the CA bus for a given channel
  SigCa11,
  SigCa12, 
  SigCa13, 
  SigCa14, 
  SigCa15, 
  GsmCsnMax,
  GsmCsnDelim = 0xFFFF
} GSM_CSN;

typedef enum {
  CACHE,
  NO_CACHE
} CacheType;

typedef enum {
  SmbusReadByte,
  SmbusWriteByte,
  SmbusReadBlock,
  SmbusWriteBlock,
  MaxSmbusCmd
} SMBUS_CMD_TYPES;

typedef enum {
  Rate0x  = 0,  // Disabled
  Rate1x  = 1,
  Rate2x  = 2,
  Rate4x  = 4
} REFRESH_RATE_SELECTION;

typedef enum {
  LRxVoc = 0,
  LReceiveEnable,
  LEarlyMpr,
  LFineWriteLeveling,
  LCoarseWriteLeveling,
  LReadTraining,
  LWriteTraining,
  LCommandTraining,
  LMemoryTest,
  MaxTrainingAlgos
} TRAINING_ALGOS_LIST;

//
// PATTERN MODES
//
#define PATTERN_VICAGG        0
#define LFSR_VICAGG           1
#define LFSR                  2

typedef struct {
  UINT16  location;
  UINT8   cachedIndex;
} DELAY_ELEMENT;

typedef struct {
  UINT32      RegisterOffset;
  UINT8       Lsb;
  UINT8       Msb;
  UINT8       ValueOffset;
  UINT8       Unit;
  UINT8       Name[20];
} DUNIT_PARAMETERS_STRUCT;

typedef enum {
  NoUnit,
  Clk,
  Mhz,
  TccdUnit,
  PwDDlyUnit,
  MaxUnitTypes,
} UNIT_TYPE;

typedef struct {
  UINT16  Index;
  UINT16  SharedIndex;
  UINT16  PBIndex;
  UINT16  StartValue[2];
  UINT16  PBStartValue;
  UINT16  Ranges[2];
  UINT16  Step[3];
  UINT8   RelAbsFlag;
  UINT16  SharedFlags;
  UINT16  PBSharedFlags;
  UINT8   Label[4];
  UINT8   NumberKnobs;
  UINT16  Phase2VrefStart;
  UINT16  Phase2VrefStop;
  UINT16  Phase2VrefStep;
  INT8    CriteriaStep;
  UINT8   PBDirectFlag;
  UINT16  PostJedecFlags;
  UINT8   SweepFlags;
  UINT8   NumberOfTiming;
  UINT8   NumberSweeps;
} CONTROLKNOBS;

typedef struct {
  UINT16    NumberConsecutiveVoltages;
  UINT16    MinimumDelayWidth;
} EYEMASK_T;

typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    Offset   : 12;  // PCI register offset
    UINT32    Func     : 3;   // PCI Function
    UINT32    Device   : 5;   // PCI Device
    UINT32    Bus      : 8;   // PCI Bus
    UINT32    Base     : 4;   // PCI base address
  } Bits;
#endif
} PCI_REG;

typedef struct {
  UINT8  OneX[MAX_STROBES];
  UINT8  TwoX[MAX_STROBES];
  UINT8  Pi[MAX_STROBES];
  UINT8  CC0[MAX_STROBES];
  UINT8  CC1[MAX_STROBES];
  UINT8  Min[MAX_STROBES];
  UINT8  Vref[MAX_STROBES];
  UINT16 TotalDelay[MAX_STROBES];
} ELEMENTS;

/**
  Final training values stored on a per blueprint level. Needs to be per blueprint
  in case of a system with more than 1 level of memory per channel.
**/
typedef struct {
  UINT16    Values[MAX_BLUEPRINTS][MAX_NUM_ALGOS][MAX_RANKS][MAX_STROBES];
} TRAINING_SETTING;

typedef struct {
  UINT8   *LinearToPhysicalTable;   ///> Pointer to an array that contains the physical values for external vref
  UINT8   SizeOfLtoPTable;          ///> Size of the LinearToPhysicalTable
  UINT8   *PhysicalToLinearTable;   ///> Pointer to an array that contains the linear values for external vref
  UINT8   SizeOfPtoLTable;          ///> Size of the PhysicalToLinearTable
} PHY_VREF;

typedef struct {
  UINT8 ManuIDlo[MAX_CHANNELS][MAX_DIMMS];
  UINT8 ManuIDhi[MAX_CHANNELS][MAX_DIMMS];
  UINT8 ManuLoc[MAX_CHANNELS][MAX_DIMMS];
  UINT8 ManuDateLO[MAX_CHANNELS][MAX_DIMMS];
  UINT8 ManuDateHI[MAX_CHANNELS][MAX_DIMMS];
  UINT8 SerialNumber1[MAX_CHANNELS][MAX_DIMMS];
  UINT8 SerialNumber2[MAX_CHANNELS][MAX_DIMMS];
  UINT8 SerialNumber3[MAX_CHANNELS][MAX_DIMMS];
  UINT8 SerialNumber4[MAX_CHANNELS][MAX_DIMMS];
  UINT8 SdramWidth[MAX_CHANNELS][MAX_DIMMS];
  UINT8 DimmCount[MAX_CHANNELS];
  UINT8 DdrFrequency[MAX_CHANNELS];
} FASTBOOTDATA;

typedef struct {
  UINT32  MrsValue[MR_COUNT];
  BOOLEAN MrsValid;
} MRS_COMMAND_SX;

#if FAULTY_PART_TRACKING
typedef struct {
  UINT32   Byte0to3;                  ///> On the fly tracking of the minimum bit failure used by all trainings 
  UINT32   Byte4to7;                  ///> On the fly tracking of the minimum failure bit
  UINT8    ByteEcc;                   ///> On the fly tracking of the minimum failure bit
  UINT16   TrainingFail;              ///> Training that failed due to faulty part
  UINT32   DataBitFailCumulativeLow;  ///> Field to cumulate bit failures found as training progresses for data bits
  UINT32   DataBitFailCumulativeHigh; ///> Field to cumulate bit failures found as training progresses for data bits
  UINT8    EccBitFailCumulative;      ///> Field to cumulate bit failures found as training progresses for ECC byte
  UINT8    ErrorType;                 ///> 0- No error, 1-Correctable, 2-Uncorrectable
} FAULTY_PART;
#endif

#if LEAKY_BUCKET == 1
typedef struct {
  UINT64    RateUnits;                                          ///>  Rate Units specified in picoseconds
  UINT64    Rate[MAX_CHANNELS][MAX_RANKS];                      ///>  Leaky Bucket Rate specified by user
  UINT16    CorrectableErrorThreshold[MAX_CHANNELS][MAX_RANKS]; ///> Correctable Error Threshold set by user
} LEAKY_BUCKET_PARAMETERS;

#define ONE_DAY_IN_PS 1440000000000000
#define ONE_US_IN_PS 1000000 
#endif 

#if DRAM_RAPL == 1
typedef struct {
  UINT64    TimeWindow;               ///> Time Window for PL1
  UINT32    PowerLimit;               ///> Power Limit (PL1) specified by user (miliWatts)
  UINT8     PowerLimitEnable;         ///> Power Limit Enable for PL1
  UINT8     PowerLimitLock;           ///> Power Limit Lock for MSR
  UINT32    BandWidthScale;           ///> BW Scale open page and close page 
} DRAM_RAPL_PARAMETERS;

#define POWER_LIMIT_RESOLUTION   125
#endif

#if INTERLEAVING_SUPPORT
typedef enum {
  NoInterleave = 0,
  Mode0,
  Mode1,
  Mode2,
  HVM
} INTERLEAVE;
#endif

typedef struct {
  INT32  CmdOffsetValue;
  INT32  ClkOffsetValue;
  INT32  CsOffsetValue;
  INT32  CkeOffsetValue;
  INT32  OdtOffsetValue;
} OVERRIDE_CLOCKS;

typedef struct {
  UINT8   Margin[MaxMrcFastBootKnobs];
} MRC_MARGIN_LEVEL;

typedef enum {
  CriteriaNormal = 0,
  CriteriaWarning,
  CriteriaCritical
} CHECKER_CRITERIA;

typedef struct {
  UINT8             EnableFastBootTester;
  MRC_MARGIN_LEVEL  Critical;
  MRC_MARGIN_LEVEL  Warning;
  UINT8             CriteriaTestResult; // 0: Normal, 1: Warning, 2: Critical
} MRC_FAST_BOOT_CHECKER;

typedef struct {
  INT8  LowMargin[MaxMrcFastBootKnobs][MAX_CHANNELS][MAX_RANKS][MAX_STROBES];
  INT8  HighMargin[MaxMrcFastBootKnobs][MAX_CHANNELS][MAX_RANKS][MAX_STROBES];
} MRC_MARGIN_CHECKER;

typedef struct {
  FASTBOOTDATA        FastBootData;
  UINT8               DimmVrefWrite[MAX_CHANNELS][MAX_RANKS][MAX_STROBES];
  UINT8               CmdVrefWrite[MAX_CHANNELS][MAX_RANKS][MAX_STROBES];
  TRAINING_SETTING    Trained_Value[MAX_CHANNELS];
  UINT64              Touud;
  UINT64              Tolud;
  UINT64              RsvdMem;
  //
  // Cached Values + Save Restore
  //
  UINT32              CachedValues[MAX_CHANNELS][MAX_NUM_CACHE_ELEMENTS][MAX_RANKS][MAX_STROBES];
  //
  // Cached Values + Save Restore per Bit
  //
  UINT32              CachedValuesPB[MAX_CHANNELS][MAX_NUM_CACHE_ELEMENTS_PB][MAX_RANKS][MAX_STROBES][MAX_BITS];
  MRS_COMMAND_SX      MrsCommandSx[MAX_CHANNELS][MAX_RANKS];
  UINT16              ScramblerSeed[MAX_CHANNELS];
  UINT8               MrcParamsValidFlag;
  UINT8               InputFineDdrVoltage;
  UINT16              SreDly;
  UINT32              Txeq[MAX_CHANNELS];
  UINT32              CtleBias[MAX_CHANNELS][MAX_STROBES/2];
  UINT32              CtleCap[MAX_CHANNELS][MAX_STROBES/2];
#if defined(PPR_SUPPORT) && (PPR_SUPPORT == 1)
  BOOLEAN             AlreadyRepaired[MAX_CHANNELS][MAX_RANKS][MAX_BANK_GROUPS_DDR4];
#endif // defined(PPR_SUPPORT) && (PPR_SUPPORT == 1)
  MRC_MARGIN_CHECKER  MrcMarginChecker;
} MRC_SAVE_RESTORE;

#define MAX_MRC_SCRUB_SEGMENTS    5
//
// The memory segment is scrubbed from Low --> High - 1
// the -1 represents one cacheline subtracted
//
typedef struct {
  UINT32    Low;    // Determines the low range for a memory segment (in MB)
  UINT32    High;   // Determines the high range for a memory segment (in MB)
} MRC_SCRUB_SEGMENT;

typedef struct {
  BOOLEAN   MemorySegmentationEnabled;
  UINT8     NumberOfSegments;
  MRC_SCRUB_SEGMENT   ScrubSegment[MAX_MRC_SCRUB_SEGMENTS];
} MRC_MEMORY_SCRUB_SEGMENTS;

typedef struct {
  UINT16                    MmioSize;         ///> MMIO 1 MB Aligned
  UINT16                    TsegSize;         ///> TSEG 1 MB Aligned
  PHY_VREF                  TxVrefTable;
  PHY_VREF                  CmdVrefTable;
  UINT8                     SpdAddressTable[MAX_CHANNELS][MAX_DIMMS];
  BOOLEAN                   EnableMemoryDown[MAX_CHANNELS][MAX_DIMMS];
  UINT8                     VrefSmbAddress[MAX_CHANNELS];
  UINT8                     CmdVrefSmbAddress[MAX_CHANNELS];
  MRC_MEMORY_SCRUB_SEGMENTS MemoryScrubSegments;
} MRC_OEM_FRAME;

typedef struct {
  UINT8     RcvEn;
  UINT8     RxDqs;
  UINT8     TxDqs;
  UINT8     TxDq;
} R2R_SWITCHING_PARAMETERS;


typedef struct {
  //
  // User Input Parameters
  // 
  BOOLEAN   EnableTest;                 ///> Enables/Disables the Pass Gate Test
  BOOLEAN   EnablePeriodicSelfRefresh;  ///> Enables/Disables periodic Self Refresh during the test
  UINT8     Direction;                  ///> '0': Lowest -> Highest  '1': Highest -> Lowest
  UINT32    ActivationsCount;           ///> The number of times a Row will be tested
  UINT32    IterationOnRow;             ///> How many times the test over the same row will be repeated
  UINT8     SwizzleMode;                ///> 0: Automatic 1: Force Samsung
  UINT8     Pattern;                    ///> Pattern used for the Aggressor
  UINT8     TargetPattern;              ///> Pattern used for the Victim
  BOOLEAN   EnablePartialTest;          ///> Enable the execution of the Pass Gate in a memory segment
  UINT32    PartialRowBankMin;          
  UINT32    PartialRowBankMax;          
  BOOLEAN   EnableMontecarloSearch;     ///> MonteCarlo Search Algorithm
  UINT8     MontecarloMaxFailures;      ///> Max number of error prior to go to next Repetition
  UINT32    MontecarloStartRepetition;  ///> Max repetition number for Montecarlo Algorithm
  UINT32    MontecarloDecrement;        ///> Min repetition number for Montecarlo Algorithm
  UINT8     SpeedSelect;                ///> 0: None, 1x, 2x, 4x and 8x
  BOOLEAN   RankTest[MAX_CHANNELS][MAX_RANKS];  ///> Specifies which Ranks will be tested
  //
  // Internal variables, these have to be populated with proper value prior to test starts
  //
  UINT8     DeviceDataWidth;            ///> Device Data Width (0: x4, 1: x8, 2: x16)
  UINT8     DeviceDensity;              ///> Device Density (0: 1Gb 1: 2Gb, 2: 4Gb, 3: 8Gb, 4: 16Gb, 5: 32Gb)
  UINT8     DimmSwizzle[MAX_DIMMS];     ///> Determine if the Dimm is Swizzle or Sequential. 0: Sequential. 1: Swizzle
  UINT8     DimmMirrored[MAX_DIMMS];    ///> Determine if the Odd Rank is mirrored (1,3...). 0: Not mirrored, 1: Mirrored, 2: Reserved

  UINT32    RowMask;                    ///> Row Mask that indicates row bits
  UINT32    RowBankTrackerMask;         ///> Provides a mask for the merge of Bank groups, Banks and Rows

  UINT8     BitsPerColumns;             ///> Number of bits used to specify a column
  UINT8     BitsPerBank;                ///> Number of bits used to specify a bank
  UINT8     BitsPerBankGroup;           ///> Number of bits used to specify a bank group (only DDR4)
  UINT8     BitsPerRow;                 ///> Number of bits used to specify a row

  UINT8     BankOffset;                 ///> Provide the address offset to the first Bank bit
  UINT8     BankGroupOffset;            ///> Provide the address offset to the first Bank Group bit
  UINT8     RowOffset;                  ///> Provide the address offset to the first Row bit
  UINT8     RankOffset;                 ///> Provide the address offset to the first Rank bit

  UINT16    Burst;                      ///> Number of bursts used for writing (Ag, Vc0 and Vc1) and reading (Only Vc0 and Vc1)
                                        
  UINT16    ActivateAdjust;             ///> Consider a number of activates before the test stops

  BOOLEAN   SamsungDram[MAX_CHANNELS][MAX_DIMMS];   ///> Defines if the DIMM is using Samsung DRAM's
} PASS_GATE_PARAMETERS;

#if defined(PPR_SUPPORT) && (PPR_SUPPORT == 1)
typedef struct {
  BOOLEAN     Repair[MAX_CHANNELS][MAX_RANKS][MAX_BANK_GROUPS_DDR4];        ///> Must be populated prior to start PPR fix
  UINT32      RowAddress[MAX_CHANNELS][MAX_RANKS][MAX_BANK_GROUPS_DDR4];    ///> Must be populated prior to start PPR fix
  UINT8       BankAddress[MAX_CHANNELS][MAX_RANKS][MAX_BANK_GROUPS_DDR4];   ///> Must be populated prior to start PPR fix (zero-based)
  UINT8       DramDevice[MAX_CHANNELS][MAX_RANKS][MAX_BANK_GROUPS_DDR4];    ///> Must be populated prior to start PPR fix (zero-based)
  BOOLEAN     PprSupport[MAX_CHANNELS][MAX_DIMMS];                          ///> Automatically populated with SPD Information
  BOOLEAN     PprSoft[MAX_CHANNELS][MAX_DIMMS];                             ///> Automatically populated with SPD Information
  BOOLEAN     GuardKey[MAX_CHANNELS][MAX_DIMMS];                            ///> Enables Guard key MRS sequence when soft/hard PPR is executed
  //
  // Internal variables for PPR. These should be populated automatically
  //
  UINT8       RsBgBaRowBitPosition[MAX_RANK_BG_BA_ROW_BITS];                ///> Bit positions
  UINT8       DataWidth[MAX_CHANNELS][MAX_DIMMS];                           ///> 0: x4, 1: x8 and 2: x16
  UINT8       DeviceDensity[MAX_CHANNELS][MAX_DIMMS];                       ///> 0: 2Gb, 1: 4Gb, 2: 8Gb, 3: 16Gb 4: 32Gb
} PPR_PARAMETERS;
#endif // defined(PPR_SUPPORT) && (PPR_SUPPORT == 1)

typedef struct {
  UINT8     SetupType;
  UINT8     CmdSequence;
  UINT32    Pattern;
  UINT32    VicPattern;
  UINT8     BusWidth;
  UINT8     MinRankBit;
  UINT8     MinBankBit;
  UINT8     NumBursts;
  UINT8     LoopCount;
  UINT8     ChunkErrorMask;
  UINT8     Rank;
  UINT16    RmtRestoreJedecResetType;
  UINT16    RmtRestoreSignalGroupBegin;
  UINT8     RmtRestoreSignalCount;
  UINT32    RmtRestoreSignalValue[MAX_RESTORE_SIGNALS][MAX_CHANNELS][MAX_RANKS][MAX_STROBES];

  UINT8     NumberOfElementsVref[MAX_CHANNELS];

  UINT16    RmtRestoreSignalGroupBeginVref;
  UINT8     RmtRestoreSignalCountVref;
  UINT32    RmtRestoreSignalValueVref[MAX_RESTORE_SIGNALS_VREF][MAX_CHANNELS][MAX_RANKS][MAX_STROBES];

  UINT8     RmtFakeErrorCount;
} CPGC_OPTIONS;

typedef struct {
  //
  // MRC_SAVE_RESTORE must be always the first line
  //
  MRC_SAVE_RESTORE          SaveRestore;
  BOOLEAN                   ChEnabled[MAX_CHANNELS];
  UINT8                     DimmPresent[MAX_CHANNELS][MAX_DIMMS];
  UINT32                    SlotMem[MAX_CHANNELS][MAX_DIMMS];
  SPD_DATA                  SpdData[MAX_CHANNELS][MAX_DIMMS];
  BOOLEAN                   EccEnabled[MAX_CHANNELS];
  UINT8                     RankPresent[MAX_CHANNELS][MAX_RANKS];
  UINT16                    TimingData[MAX_CHANNELS][MaxTimingData][MaxDvfsFreqs];
  UINT8                     DramType[MAX_CHANNELS];
  UINT8                     BootMode;
  UINT8                     DdrFrequency;
  UINT32                    SystemMemorySize;
  UINT32                    TotalMem[MAX_CHANNELS];
} MRC_PARAMETERS;

typedef struct {
  //
  // MRC_PARAMETERS must be always the first line
  //
  MRC_PARAMETERS            MrcParameters;
  //
  // Cached Values
  //
  UINT32                    CachedValuesNonSR[MAX_CHANNELS][MAX_NUM_CACHE_ELEMENTS_NON_SR][MAX_RANKS][MAX_STROBES];
  //
  // Cached Values Per Bit
  //
  UINT32                    CachedValuesNonSRPB[MAX_CHANNELS][MAX_NUM_CACHE_ELEMENTS_NON_SR_PB][MAX_RANKS][MAX_STROBES][MAX_BITS];
#if defined(PPR_SUPPORT) && (PPR_SUPPORT == 1)
  PPR_PARAMETERS            PprParameters;
#endif // defined(PPR_SUPPORT) && (PPR_SUPPORT == 1)
  OVERRIDE_CLOCKS           OverrideClocks;
  FASTBOOTDATA              FastBootDataCurrent;
  UINT8                     CoreFreq;
  UINT8                     DimmCount[MAX_CHANNELS];
  UINT8                     TotalDimm;
  UINT32                    DimmVoltageFlags;
  UINT8                     DVFSFreqIndex;
  UINT8                     NumberOfTiming;
  UINT8                     D_Ranks[MAX_CHANNELS][MAX_DIMMS];
  UINT8                     D_DataWidth[MAX_CHANNELS][MAX_DIMMS];
  UINT8                     D_Banks[MAX_CHANNELS][MAX_DIMMS];
  UINT8                     D_BusWidth[MAX_CHANNELS][MAX_DIMMS];
  UINT8                     D_Size[MAX_CHANNELS][MAX_DIMMS];
  UINT8                     D_Type[MAX_CHANNELS][MAX_DIMMS];
  UINT8                     Dimm_1Gb[MAX_CHANNELS];
  UINT8                     D_NvDimm[MAX_CHANNELS];
  UINT8                     Ddr3S[MAX_CHANNELS][MAX_DIMMS];
  UINT8                     Ddr3L[MAX_CHANNELS][MAX_DIMMS];
  UINT8                     Ddr3U[MAX_CHANNELS][MAX_DIMMS];
  UINT8                     DimmFrequency[MAX_CHANNELS][MAX_DIMMS];
  BOOLEAN                   RankEnabled[MAX_CHANNELS][MAX_RANKS];
  UINT32                    EccStrobes[MAX_CHANNELS];
  UINT8                     MaxDq[MAX_CHANNELS][MAX_RANKS];
  UINT8                     MaxBit[MAX_CHANNELS][MAX_RANKS];
  UINT8                     MaxDqCache[MAX_CHANNELS][MAX_RANKS];
  UINT8                     MaxBitCache[MAX_CHANNELS][MAX_RANKS];
  MSP_DATA                  MspData;
  UINT16                    TimingDataOverrides[MAX_CHANNELS][MaxTimingData][MaxDvfsFreqs];
  INT16                     RmtData[MAX_CHANNELS][MAX_RANKS][MaxRMTData];
  INT16                     RmtMargins[MAX_CHANNELS][MAX_RANKS][MAX_STROBES];
  UINT8                     TwoClkRdPreamble[MAX_CHANNELS];
  UINT8                     TwoClkWrPreamble[MAX_CHANNELS];
  UINT8                     WriteDataEarlyEnable[MAX_CHANNELS];
#if SIM && DDR4_SUPPORT
  UINT16                    TbMr5[MAX_CHANNELS];
#endif
#if FAULTY_PART_TRACKING
  FAULTY_PART               FaultyPart[MAX_CHANNELS][MAX_RANKS];
#endif
  UINT32                    NvDimmType[MAX_CHANNELS][MAX_DIMMS];   // Index into the nv manufacture array for NVDIMM vender operations table 0=not NVDIMM
  UINT32                    NvDimmStatus[MAX_CHANNELS][MAX_DIMMS];

  UINT8                     PostCode;
  MMRC_STATUS               ErrorCode;
  BOOLEAN                   RestorePath;
  UINT8                     DigitalDll;
  UINT8                     MrcRmtSupport;
  UINT32                    MrcRmtEnableSignal;
  UINT8                     MrcRmtCpgcExpLoopCnt;
  UINT8                     MrcRmtCpgcNumBursts;
  UINT8                     MrcTrainingCpgcExpLoopCnt;
  UINT8                     MrcCpgcNumBursts;
#if MITIGATION_FEATURE == 1
  UINT16                    MitigationProbability;
#endif
#if FAULTY_PART_TRACKING
  BOOLEAN                   FaultyPartTracking;           
  BOOLEAN                   FaultyPartContinueOnCorrectable;
#endif
  UINT16                    SmbusBar;
  UINT32                    SbRegBase;
#if SIM || JTAG
  FILE                      *LogFile;
#endif
  MRC_OEM_FRAME             OemMrcData;
  UINT8                     SocStepping;
  UINT16                    CpuType;
  UINT32                    MemoryCeiling;
  UINT32                    EcBase;
  UINT32                    MchBar;
  UINT16                    GpioBar;
  UINT32                    PmcBar;
  UINT32                    HpetHptcSave;
  UINT8                     DdrFreqCap;
  UINT8                     DrbSecondTimePath;
  UINT8                     PowerOn;
  UINT8                     PpoPlatform;
  UINT8                     FastBootEnable;
  UINT8                     DynSR;
  UINT8                     DynPmop;
  UINT8                     PatrolScrubEnable;
  UINT8                     PatrolScrubPeriod;
  UINT32                    PatrolScrubAddr;
  UINT8                     DemandScrub;
  UINT16                    MaxScrubDebitCount;
  UINT8                     OpenPolicyTimer;
  UINT8                     EnableScrambler;
  UINT8                     EnableSlowPowerDownExit;
  BOOLEAN                   MemoryPreservation;
  BOOLEAN                   MemoryOverwriteRequestControl;
  //
  // Persistent Memory
  //
  UINT8                     NonVolMemMode;
  UINT8                     NonVolMemInterleaving;
  UINT32                    NonVolMemSize;
  UINT8                     AdrRecoveryDetected;
  UINT8                     eraseArmNVDIMMS;      // BIOS will perform NVDIMM erase & ARM operations or a OS driver
  UINT8                     restoreNVDIMMS;       // BIOS will perform NVDIMM Restore operation
  UINTN                     CkeLatchGpioCtrl;     // GPIO controlling SelfRefresh(CKE) latch
  //
  // Persistent Memory end
  //
  UINT8                     MemoryTestLoop;
  UINT32                    LoopCount;
  UINT8                     HaltSystemOnMemTestError;
  UINT8                     ResetLoop;
  UINT8                     ResetLoopCycles;
  UINT8                     OutOfOrderAgingThreshold;
  UINT8                     OutOfOrderDisable;
  UINT8                     NewRequestBypassDisable;
  UINT8                     VrefOverrideEnable;
  UINT8                     VrefOverrideValue;

#if ME_SUPPORT_FLAG
  BOOLEAN                   MeSeg;
  UINT32                    MeUmaSize;
#endif
  UINT8                     OdlaParalPrtEn;
  UINT32                    MemoryDepth;
  UINT8                     BramParityEnable;
  UINT8                     CkePowerDownDisable;
  UINT8                     OverrideCheckpoints;
  UINT8                     SelectRefreshRate;
#if INTERLEAVING_SUPPORT
  UINT8                     InterleaveMode;
#endif
  BOOLEAN                   ZqcEnable;
  UINT8                     TotalChannels;
  BOOLEAN                   EnableCkeByRank;
  UINT16                    DimmConfig[MAX_CHANNELS];
  UINT32                    BCtrl;
  UINT32                    BDebug1;
  UINT32                    BwFlush;
  UINT8                     AbSegToDram;
  UINT8                     ESegToDram;
  UINT8                     FSegToDram;
  UINT32                    SupportedCasLat;
  UINT16                    SignalType;
  UINT8                     HalfWidthEnable;
  UINT8                     TclIdle;
  BOOLEAN                   EnableParallelTraining;
  UINT8                     PowerTrainingEnable;
  POWER_KNOB_VALUES         SettingsPowerKnobs;
  UINT8                     BankXorMapEnable;
  CPGC_OPTIONS              CpgcOptions;
  UINT32                    RestoreStrobeMaskEnable;
  BOOLEAN                   InvertChannelSpdAddresses;
  BOOLEAN                   PerformanceSettings;
  UINT8                     MciIehN;
#if DDR4_SUPPORT == 1
  UINT8                     CommandAddressParity;
#endif
#if PASS_GATE_TEST == 1
  PASS_GATE_PARAMETERS      PassGateParameters;
#endif
  UINT8                     DVFS_FreqA;
  UINT8                     DVFS_FreqB;
  UINT8                     DVFS_Enable;
  UINT8                     TimingParamsConfig;
#if LEAKY_BUCKET == 1
  LEAKY_BUCKET_PARAMETERS   LeakyBucketParameters;
#endif
#if DRAM_RAPL == 1
  DRAM_RAPL_PARAMETERS      DramRaplParameters;
#endif
  BOOLEAN                   EnableParityCheck;
  UINT8                     MemoryThermalEnable;
  UINT8                     ClttMode;
  UINT8                     ThermalThrottlingType;
  UINT8                     TempHi;
  UINT8                     TempMid;
  UINT8                     TempLo;
  UINT8                     BwLevelOnThrtCrit;
  UINT8                     BwLevelOnThrtHi;
  UINT8                     BwLevelOnThrtMid;
  UINT8                     OlttThrtBw;
  UINT8                     Rk2Rk[MAX_CHANNELS];
  UINT8                     MemHotThrotLvl;
  UINT8                     EnMemTrip;
  UINT8                     TempMemTrip;
  BOOLEAN                   ReadPerBitEnable;
  BOOLEAN                   WritePerBitEnable;
  UINT8                     CommandMode;              ///> 0: Auto, 1: Force 1N, 2: Force 2N
  UINT8                     SkipOptions;              ///> Skips options for tests and/or delays
  UINT8                     RxSkewCtl;
  UINT8                     TxSkewCtl;
  UINT8                     PerformanceProfile;
  UINT8                     VocSweep;
  BOOLEAN                   SkipTrainingAlgos[MaxTrainingAlgos];
  BOOLEAN                   PdaEnable;
  UINT8                     CacheTxVref[MAX_CHANNELS][MAX_RANKS][MAX_STROBES];
  UINT8                     CacheRxP[MAX_CHANNELS][MAX_RANKS][MAX_STROBES];
  UINT8                     CacheRxN[MAX_CHANNELS][MAX_RANKS][MAX_STROBES];
  UINT8                     EarlyWriteTrainingEnable;
  UINT8                     CurrentMode[MAX_CHANNELS];
  UINT8                     MaxThresholdPerBit;
  UINT16                    CorrectErrThreshold;
  BOOLEAN                   ExecuteTxVrefSetup;
  BOOLEAN                   SecondTimeExecution;
  MRC_FAST_BOOT_CHECKER     MrcFastBootChecker;
  INT8                      RxDqsTetherVal[MAX_STROBES];
  BOOLEAN                   SpdWriteDisable;
} MMRC_DATA;

//
// DRAM Initialization Structures used in JEDEC Message Bus Commands
//
typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    bankAddress     : 3;            // Bank Address (BA[2:0]) 
    UINT32    BL              : 2;            // Burst Length, CDV:1
    UINT32    CL              : 1;            // CL Reserved CDV:0 
    UINT32    RBT             : 1;            // Read Burst Type 
    UINT32    casLatency      : 3;            // cas Latency 
    UINT32    TM              : 1;            // Test mode 
    UINT32    dllReset        : 1;            // DLL Reset 
    UINT32    writeRecovery   : 3;            // Write Recovery for Auto Pre-Charge: 001=2,010=3,011=4,100=5,101=6 
    UINT32    PPD             : 1;            // DLL Control for Precharge Power-Down CDV:1 
    UINT32    reserved1       : 3;
    UINT32    rankSelect      : 4;            // Rank Select 
    UINT32    reserved2       : 6;
  } Bits;
#endif
} DramInitDDR3MRS0;                           // DDR3 Mode Register Set (MRS) Command 

typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    bankAddress     : 3;            // Bank Address (BA[2:0]) 
    UINT32    dllEnabled      : 1;            // CDV=0 
    UINT32    DIC0            : 1;            // Output Driver Impedance Control 
    UINT32    rttNom0         : 1;            // RTT_nom[0] 
    UINT32    MRC_AL          : 2;            // Additive Latency = 0 
    UINT32    DIC1            : 1;            // Reserved 
    UINT32    rttNom1         : 1;            // RTT_nom[1] 
    UINT32    wlEnabled       : 1;            // Write Leveling Enable 
    UINT32    reserved1       : 1;
    UINT32    rttNom2         : 1;            // RTT_nom[2] 
    UINT32    reserved2       : 1;
    UINT32    TDQS            : 1;            // TDQS Enable 
    UINT32    Qoff            : 1;            // Output Buffers Disabled 
    UINT32    reserved3       : 3;
    UINT32    rankSelect      : 4;            // Rank Select 
    UINT32    reserved4       : 6;
  } Bits;
#endif
} DramInitDDR3EMR1;                           // DDR3 Extended Mode Register 1 Set (EMRS1) Command 

typedef union {
  UINT32      Data;
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    bankAddress     : 3;            // Bank Address (BA[2:0]) 
    UINT32    PASR            : 3;            // Partial Array Self-Refresh 
    UINT32    CWL             : 3;            // CAS Write Latency 
    UINT32    ASR             : 1;            // Auto Self-Refresh 
    UINT32    SRT             : 1;            // SR Temperature Range = 0
    UINT32    reserved1       : 1;
    UINT32    rtt_WR          : 2;            // Rtt_WR 
    UINT32    reserved2       : 5;
    UINT32    rankSelect      : 4;            // Rank Select 
    UINT32    reserved3       : 6;
  } Bits;
#endif
} DramInitDDR3EMR2;                           // DDR3 Extended Mode Register 2 Set (EMRS2) Command 

typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    bankAddress     : 3;            // Bank Address (BA[2:0]) 
    UINT32    MPR_Location    : 2;            // MPR Location 
    UINT32    MPR             : 1;            // MPR: Multi Purpose Register 
    UINT32    reserved1       : 13;
    UINT32    rankSelect      : 4;            // Rank Select 
    UINT32    reserved2       : 6;
  } Bits;
#endif
} DramInitDDR3EMR3;                           // DDR3 Extended Mode Register 2 Set (EMRS2) Command 

typedef union {
  UINT32        Data;
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110  ZQ Calibration,111-NOP 
    UINT32    bankAddress     : 3;            // Bank Address (BA[2:0]) 
    UINT32    multAddress     : 16;           // Multiplexed Address (MA[15:0]) 
    UINT32    rankSelect      : 2;            // Rank Select 
    UINT32    reserved3       : 8;
  } Bits;
#endif
} DramInitMisc;                               // Miscellaneous DDRx Initialization Command 

#if DDR4_SUPPORT == 1
typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    ACT_N           : 1;            // ACT_N
    UINT32    bankAddress     : 2;            // Bank Address (BA[1:0]) 
    UINT32    bankGroup       : 2;            // Bank Address (BG[1:0]) 
    UINT32    BurstLength     : 2;            // Burst Length [A1:A0] 00 = 8 (Fixed) 01 = BC4 or 8 (on the fly)10 = BC4 (Fixed)11 = Reserved
    UINT32    CL              : 1;            // cas Latency A2 lsb
    UINT32    readBurstType   : 1;            // Read Burst Type [A3]  0 = Sequential 1 = Interleave
    UINT32    casLatency      : 3;            // cas Latency [A6:A4] msb
    UINT32    TM              : 1;            // Test mode [A7]
    UINT32    dllReset        : 1;            // DLL Reset [A8]
    UINT32    writeRecovery   : 3;            // Write Recovery [A11:A9] for Auto Pre-Charge: Jedec DDR4 MR0 table 2 page 21
    UINT32    reserved        : 6;            // reserved
    UINT32    rankSelect      : 2;            // Rank Select [27:26]
    UINT32    reserved1       : 4;            // reserved
  } Bits;
#endif
} DramInitDDR4MRS0;                           // DDR4 Extended Mode Register 0 Set (EMRS0) Command


typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    ACT_N           : 1;            // ACT_N
    UINT32    bankAddress     : 2;            // Bank Address (BA[1:0]) 
    UINT32    bankGroup       : 2;            // Bank Address (BG[1:0]) 
    UINT32    dllEnabled      : 1;            // DLL Enable [A0] 0 = Disable2 1 = Enable 
    UINT32    DIC0            : 2;            // Output Driver Impedance Control [A2:A1]: 00 RZQ/7 01 RZQ/5
    UINT32    MRC_AL          : 2;            // Additive Latency [A4:A3]: 00 = 0(AL disabled) 01 = CL-1 10 = CL-2
    UINT32    reserved0       : 2;            // [A6:A5]
    UINT32    wlEnabled       : 1;            // Write Leveling Enable [A7]
    UINT32    rttNom          : 3;            // RTT_nom [A10:A9:A8] Jedec page 22  RZQ/#
    UINT32    TDQS            : 1;            // TDQS Enable  [A11]
    UINT32    Qoff            : 1;            // Output Buffers Disabled [A12]
    UINT32    reserved1       : 5;            // reserved
    UINT32    rankSelect      : 2;            // Rank Select [27:26]
    UINT32    reserved2       : 4;            // reserved
  } Bits;
#endif
} DramInitDDR4EMR1;                           // DDR4 Extended Mode Register 1 Set (EMRS1) Command


typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    ACT_N           : 1;            // ACT_N
    UINT32    bankAddress     : 2;            // Bank Address (BA[1:0]) 
    UINT32    bankGroup       : 2;            // Bank Address (BG[1:0]) 
    UINT32    TRRAddress      : 2;            // TRR Mode [A1:A0] 
    UINT32    TRRBankG0       : 1;            // TRR Mode [A2]  
    UINT32    CWL             : 3;            // CAS Write Latency [A5:A3]
    UINT32    LPASR           : 2;            // Low Power Array Self-Refresh [A7:A6]
    UINT32    TRRBankG1       : 1;            // TRR Mode [A8]
    UINT32    RTT_WR          : 3;            // RTT_WR[A11:A10:A9]
    UINT32    writeCRC        : 1;            // Write CRC [A12] 0 = Disable 1 = Enable
    UINT32    TRR             : 1;            // TRR [A13]
    UINT32    reserved        : 4;            // reserved
    UINT32    rankSelect      : 2;            // Rank Select [27:26]
    UINT32    reserved1       : 4;            // reserved
  } Bits;
#endif
} DramInitDDR4EMR2;                           // DDR4 Extended Mode Register 2 Set (EMRS2) Command

typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    ACT_N           : 1;            // ACT_N
    UINT32    bankAddress     : 2;            // Bank Address (BA[1:0]) 
    UINT32    bankGroup       : 2;            // Bank Address (BG[1:0])
    UINT32    MPR_page        : 2;            // MPR page [A1:A0]
    UINT32    MPR             : 1;            // MPR Operation [A2]
    UINT32    Geardown        : 1;            // Geardown [A3]
    UINT32    PDA             : 1;            // Per DRAM Addressability [A4] 0 = Disable 1 = Enable
    UINT32    Temp_sensor     : 1;            // Temperature sensor readout [A5] 0 : disabled 1: enabled
    UINT32    FineGranRefresh : 3;            // Fine Granularity Refresh Mode [A8:A6]
    UINT32    WL_crcdm        : 2;            // Write CMD Latency when CRC and DM are enabled [A10:A9]
    UINT32    MPR_readformat  : 2;            // MPR Read Format [A12:A11] 00 = Serial 10 = Staggered 01 = Parallel 11 = Reserved
    UINT32    reserved        : 5;            // reserved
    UINT32    rankSelect      : 2;            // Rank Select [27:26]
    UINT32    reserved1       : 4;            // reserved
  } Bits;
#endif
} DramInitDDR4EMR3;                           // DDR4 Extended Mode Register 3 Set (EMRS3) Command


typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    ACT_N           : 1;            // ACT_N
    UINT32    bankAddress     : 2;            // Bank Address (BA[1:0]) 
    UINT32    bankGroup       : 2;            // Bank Address (BG[1:0]) 
    UINT32    reserved0       : 1;            // 0 = must be programmed to 0 during MRS
    UINT32    Max_PD          : 1;            // Maximum Power Down [A1]  
    UINT32    Temp_ctledrr    : 1;            // Temperature ControlledRefresh Range [A2] 0 = Normal 1 = Extended
    UINT32    Temp_ctledrm    : 1;            // Temperature Controlled Refresh Mode [A3] 0 = Disable 1 = Enable
    UINT32    Int_VrefMon     : 1;            // Internal Vref Monitor [A4] 0 : disabled 1: enabled
    UINT32    SPPR            : 1;            // Soft PPR
    UINT32    CS2cmdaddrlat   : 3;            // CS to CMD/ADDR Latency Mode (cycles) [A8:A6]
    UINT32    SRabort         : 1;            // Self Refresh Abort[A9]
    UINT32    ReadPreamMode   : 1;            // Read Preamble Training Mode [A10] 0 = Disable 1 = Enable
    UINT32    ReadPream       : 1;            // Read Preamble Training Mode [A11] 0 = 1 nCK 1 = 2 nCK
    UINT32    WritePream      : 1;            // Write Preamble Training Mode [A12] 0 = 1 nCK 1 = 2 nCK
    UINT32    PPR             : 1;            // PPR [A13] 0 = Disable 1 = Enable
    UINT32    reserved2       : 4;            // reserved
    UINT32    rankSelect      : 2;            // Rank Select [27:26]
    UINT32    reserved3       : 4;            // reserved
  } Bits;
#endif
} DramInitDDR4EMR4;                           // DDR4 Extended Mode Register 4 Set (EMRS4) Command

typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    ACT_N           : 1;            // ACT_N
    UINT32    bankAddress     : 2;            // Bank Address (BA[1:0]) 
    UINT32    bankGroup       : 2;            // Bank Address (BG[1:0]) 
    UINT32    CA_paritylat    : 3;            // C/A Parity Latency Mode [A2:A0]
    UINT32    Crc_errclear    : 1;            // CRC Error Clear [A3] 0 = Clear 1 = Error  
    UINT32    CA_parerrsts    : 1;            // C/A Parity Error Status [A4] 0 = Clear 1 = Error
    UINT32    Odt_inbuff      : 1;            // ODT Input Buffer during Power Down mode [A5]
    UINT32    RTT_PARK        : 3;            // Internal Vref Monitor [A8:A6]
    UINT32    CA_parperserr   : 1;            // CA parity Persistent Error [A9] 0 = Disable1 = Enable
    UINT32    DM              : 1;            // Data Mask [A10] 0 = Disable 1 = Enable
    UINT32    WriteDBI        : 1;            // Write DBI [A11]0 = Disable 1 = Enable
    UINT32    ReadDBI         : 1;            // Read DBI [A12] 0 = Disable 1 = Enable
    UINT32    reserved        : 5;            // reserved
    UINT32    rankSelect      : 2;            // Rank Select [27:26]
    UINT32    reserved1       : 4;            // reserved
  } Bits;
#endif
} DramInitDDR4EMR5;                           // DDR4 Extended Mode Register 5 Set (EMRS5) Command

typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    ACT_N           : 1;            // ACT_N
    UINT32    bankAddress     : 2;            // Bank Address (BA[1:0]) 
    UINT32    bankGroup       : 2;            // Bank Address (BG[1:0]) 
    UINT32    Vrefdq_Value    : 6;            // VrefDQ Training Value [A5:A0]
    UINT32    Vrefdq_Range    : 1;            // VrefDQ Training Range [A6]
    UINT32    Vrefdq_Enable   : 1;            // VrefDQ Training Enable [A7] 0 = Disable(Normal operation Mode) 1 = Enable(Training Mode)
    UINT32    reserved0       : 2;            // Must be 0 during MRS [A9:A8]
    UINT32    tCCD_L          : 3;            // tCCD_L [A12:A10]
    UINT32    reserved1       : 5;            // reserved
    UINT32    rankSelect      : 2;            // Rank Select [27:26]
    UINT32    reserved2       : 4;            // reserved
  } Bits;
#endif
} DramInitDDR4EMR6;                           // DDR4 Extended Mode Register 6 Set (EMRS6) Command

typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110-ZQ,111-NOP 
    UINT32    ACT_N           : 1;            // ACT_N
    UINT32    bankAddress     : 2;            // Bank Address (BA[1:0]) 
    UINT32    bankGroup       : 2;            // Bank Address (BG[1:0]) 
    UINT32    MemoryAddress   : 18;           // Memory Address: Sets the value of MA[17:0].
    UINT32    rankSelect      : 2;            // Rank Select [27:26]
    UINT32    reserved1       : 4;            // reserved
  } Bits;
#endif
} DramInitDDR4Misc;                           // DDR4 Extended Mode Misc Register Command

#endif // DDR4_SUPPORT

#ifndef ASM_INC
typedef union {
  struct {
    //
    // Command (RAS#,CAS#,WE#): Sets the value to be driven on the RAS#, CAS# and WE#
    // pins, respectively. 
    // 
    // Bits[2:0], WO, default = 0x0
    //
    UINT32 command : 3;
    //
    // ACT_N
    // 
    // Bits[3:3], WO, default = 0x0
    //
    UINT32 act_n : 1;
    //
    // Bank Address: Sets the value of BA[1:0].
    // 
    // Bits[5:4], WO, default = 0x0
    //
    UINT32 bankaddress : 2;
    //
    // Bank Group: Sets the value of BG[1:0]
    // 
    // Bits[7:6], WO, default = 0x0
    //
    UINT32 bankgroup : 2;
    //
    // Memory Address: Sets the value of MA[17:0]
    // 
    // Bits[25:8], WO, default = 0x0
    //
    UINT32 memoryaddress : 18;
    //
    // Rank Select: 00 = Rank0, 01 = Rank1, 10 = Rank2, 11 = Rank3
    // 
    // Bits[27:26], WO, default = 0x0
    //
    UINT32 rankselect : 2;
    //
    // BA[1:0] to be used by read command when the MPR mode sequence is selected (bit
    // 30 = 1) 
    // 
    // Bits[29:28], WO, default = 0x0
    //
    UINT32 ba : 2;
    //
    // MPR Mode Sequence
    // 
    // Bits[30:30], WO, default = 0x0
    //
    UINT32 mpr : 1;
    //
    // PREA issued prior to the command; intended to be used with an MRS command. (Functionality
    // not implemented in Dunit) 
    // 
    // Bits[31:31], WO, default = 0x0
    //
    UINT32 prea : 1;
  } Bits;
  UINT32 Data;
} DRAM_CMD_DDR4;
#endif // ASM_INC

#pragma pack()

#define NO_PRINT                0xFFFF

#define ANALOGDLL               0
#define DIGITALDLL              1

#define LPDDR3_WL_ENABLE        BIT19
#define DDR3_WL_ENABLE          BIT13

#define JEDEC_PRECHARGEALL      0x01
#define JEDEC_MRS               0x02
#define JEDEC_REFRESH           0x03

#define FORCEODT_LOW            0x00
#define FORCEODT_HIGH           0x01
#define FORCEODT_RELEASE        0x02

#define TASK_FUNCTION_DESC_DONE 0, 0, ((MMRC_STATUS(*)(MMRC_DATA *, UINT16, UINT16, UINT8)) 0), 0, 0, 0, 0, 0, 0, 0
#define DETREGLIST_NULL         ((DETAILED_REGISTER_STRING *) 0xFFFFFFFF)
#define REGLIST_NULL            ((REGISTER_STRING *)    0xFFFFFFFF)
#define PHYINIT_LIST_DONE       ((UINT8 *) 0xFFFFFFFF), REGLIST_NULL, DETREGLIST_NULL

typedef struct {
  UINT8   PostCode;
  UINT8   BootMode;
  MMRC_STATUS  (*Function) (MMRC_DATA *, UINT16, UINT16, UINT8);
  UINT16  CapsuleStartIndex;
  UINT8   StringIndex;
  UINT8   Channel;
  UINT16  CondExec;
  UINT16  PList;
  UINT16  FList;
  UINT16  CList;
  UINT16  TList;
} TASK_DESCRIPTOR;

typedef struct {
  MMRC_STATUS  (*Function) (MMRC_DATA *MrcData, UINT8 Socket, UINT8 Channel, UINT8 Dimm, UINT8 Rank, UINT8 Strobe, UINT8 Bit, UINT8 FrequencyIndex, UINT8 IoLevel, UINT8 Cmd, UINT32 *Value);
} EXTERNAL_GETSET_FUNCTION;

/**
  When specifying a unique location, the elements required are 
  offset/starting/ending bits.
**/
typedef struct {
  UINT16 Offset;              ///> Byte offset for the specific rank. 
  UINT8 StartingBit;          ///> Starting bit position for the specific rank.
  UINT8 EndingBit;            ///> Ending bit position for the specific rank.
} REG_LOC;

typedef union {
  UINT8      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT8    GranularityValue   : 4;  // Granularity of the element. Used for clock crossing addition/subtraction when a CC directly modifies a delay register.
    UINT8    BitBased           : 1;  // This element has bit level granularity (not strobe level)
    UINT8    GranularityValid   : 1;  // When 1, the Granularity field is valid.
    UINT8    StrobeLookupTable  : 1;  // When 1, the Strobe Lookup Table is used to calculate ByteOffset and BoxType Offset.
    UINT8    RankLookupTable    : 1;  // When 1, the Rank Lookup Table is used to calculate ByteOffset and BoxType Offset.
  } Bits;
#endif
} ELEMENT_ATTRIBUTES;

typedef union {
  UINT8      Data;
  //
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT8    GetSetExternal     : 1;  // When in an external function uses the GetSet function
    UINT8    Reserved           : 7;  // This element has bit level granularity (not strobe level)
  } Bits;
#endif
} ACCESS_ATTRIBUTES;

/** 
  Elements is the main structure for identifying the location for an individual register
  assignment with the Get/Set API.
**/
typedef struct {
  UINT8               Box_Type;            ///> Box type.
  REG_LOC             RegisterLocation[MAX_RANKS];
  UINT8               CacheIndex;          ///> Index into the cache array where this element is stored.
  ELEMENT_ATTRIBUTES  ElementAttributes;   ///> Various attributes about this element.
  ACCESS_ATTRIBUTES   AccessAttributes;    ///> Various attributes about this element's access.
  INT16               SPMByteOffset;       ///> Strobe-to-Strobe Per Module Byte offset.
  INT8                SPMBitOffset;        ///> Strobe-to-Strobe Per Module Bit offset.
  INT16               NbSPMByteOffset;     ///> Nibble Strobe-to-Strobe Per Module Byte offset.
  INT8                NbSPMBitOffset;      ///> Nibble Strobe-to-Strobe Per Module Bit offset.
  INT16               BitSPMByteOffset;    ///> Bit Strobe-to-Strobe Per Module Byte offset.
  INT8                BitSPMBitOffset;     ///> Bit Strobe-to-Strobe Per Module Bit offset.
  UINT32              MaxLimit;            ///> Maximum absolute value allowed
  UINT32              WaitTime;            ///> Time in nanoseconds required for new setting to take effect
} GET_SET_ELEMENTS;

typedef struct {
  UINT16     DelayOffset;
} INDEX_TO_OFFSET_LAYER;

/** 
  Information about the signals. Things like number of signal groups, name of signal, etc.
**/
typedef struct {
  UINT8               NumSignalGroups;     ///> Number of signal groups for this signal type.
  UINT8               FrequencyOffset;     ///> Offset to the same element for the next frequency in a system which support multiple runtime freqs.
  UINT8               FrequencyIndex;      ///> The frequency index of this signal type.
  UINT16              VrefIndex;           ///> The index of the vref associated with this delay.
  UINT16              Minus1Index;         ///> The index of the minus 1 select associated with this delay
  char               *Name;                ///> Name of the signal
} SIGNAL_INFO;

typedef struct {
  UINT16        Signal;
  UINT8         RunByGroup;
  UINT16        GroupId;
  BOOLEAN       MarginPerBit;
  UINT8         CpgcConfig;
  UINT8         LowSide;
  UINT8         HighSide;
  UINT8         NumElements;
  UINT8         StepSize;
  UINT16        JedecResetType;
} RMT_SIGNAL;

/**
  ClockCrossing structure provides when the specific clock crossing will change
  based on the linear PI values.  The range is specified in (%) of the PI range
  since the PI range is dynamic based on analog/digitial dll.
**/
typedef struct {
  UINT8       CcNumber;         ///> Clock crossing index.
  UINT16      MinPercent;       ///> Minimum PI value for InValue to be programmed.
  UINT16      MaxPercent;       ///> Maximum PI value for InValue to be programmed.
  INT16       InValue;          ///> In Range Value.
  BOOLEAN     IsAbsolute;       ///> If TRUE, program InValue directly to register. If FALSE, 
                                ///> add InValue to current value of register.
} CLOCK_CROSSINGS;

typedef struct {
  INT8    bitoffset;
  INT16   byteoffset;
} LP_TABLE;

/** 
  Layout for the Register Display Strings
**/
typedef struct {
  UINT8 string[16]; ///> Maximum of 16 characters for the name of the register.
  UINT8 start;      ///> Index into the detailed register (fields Names) where the first field is listed.
  UINT8 number;     ///> Number of fields in the detailed register array.
} REGISTER_STRING;

/** 
  Layout for the Detailed Register Display Strings (Fields).
**/
typedef struct {
  UINT8 string[13]; ///> Maximum of 13 character for the register field name.
  UINT8 endBit;     ///> End bit for the field.
  UINT8 startBit;   ///> Starting bit for the field.
} DETAILED_REGISTER_STRING;

typedef struct {
  CONST UINT8                       *regAssign;
  CONST REGISTER_STRING             *regString;
  CONST DETAILED_REGISTER_STRING    *detRegString;
} PHYINIT_LIST;

typedef struct {
  UINT16   halfClk;
  UINT16   minVal1;
  UINT16   maxVal1;
  UINT16   minVal2;
  UINT16   maxVal2;
} DIGITAL_DLL_LIST;

/**
  Used to pass data between MRC and MMRC. Not used at the moment
  so setting the variable to Reserved.
**/
typedef struct {
  BOOLEAN Enable;       ///> Set to TRUE if you want the feature enabled. FALSE if you want it disabled.
  UINT8   Channel;      ///> Optional channel on which to operate. If not populated, all channels will be handled.
  BOOLEAN ChannelValid; ///> Set to TRUE if Channel contains a particular channel or channels on which to operate.
  MMRC_STATUS  (*Function) (MMRC_DATA *, UINT8);
} HANDSHAKE_PARAMETERS;

#ifndef MRC_DATA_REQUIRED_FROM_OUTSIDE
typedef MMRC_STATUS (*MRC_TASK_FUNCTION) (MMRC_DATA  *MrcData, UINT8 Channel);
#endif
//
// Prototypes for each Power Nob function
//
typedef MMRC_STATUS (*HOOK_SET_VALUE) (MMRC_DATA *MrcData, UINT8 Channel, UINT16 Index);
typedef MMRC_STATUS (*TRAINING_STEP)  (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex, UINT8 Channel);

typedef enum {
  MrcErrOk = 0,
  MrcErrDebug,
  MrcErrDimmsFailed,
  MrcErrInstallPeiMemory,
  MrcErrInvalidChannelNumber,
  MrcErrInvalidMemorySize,
  MrcErrIllegalVoltageConfig,
  MrcErrMailbox,
  MrcErrMemTest,
  MrcErrWarmResetError,
  MrcErrGetSpdData, 
  MrcErrNoDimmsPresent, 
  MrcErrInvalidRankNumber,
  MrcErrInvalidDimmNumber, 
  MrcErrConfigureMemory, 
  MrcErrNullPointer,
  MrcSetLimitReached,
  MrcErrFaultyPartCorrectable,
  MrcErrFaultyPartUncorrectable,
  MrcErrInvalidFrequency,
  MrcErrPlatformSettings,
  MrcErrInvalidLinearValue,
  MrcErrInvalidPhysicalValue,
  MrcErrRankStrobeClockDelta,
  MrcErrCenter2DAccMarginXZero,
  MrcErrCenter2DAccMarginYZero,
  MrcErrSearchBeyondHalfClk,
  MrcErrInvalidPowerKnob,
  MrcErrChannelsDisabled,
  MrcErrSetupErrorPassGate,
  MrcRankNotSupported,
  MrcIoLevelNotSupported,
  MrcMarginGroupNotSupported,
  MrcSignalNotSupported,
  MrcFrequencyIndexNotSupported,
  MrcErrInvalidDdrType,
  MrcErrSpdBufferOverrun,
  MrcErrPprInputParameter,
  MrcErrX4NotSupported,
  MrcErrPerBitNotSupported,
  MrcErrIllegalAdvConstrains,
  MrcErrInvalidMaxBits,
  MrcErrInvalidCmdForCache,
  MrcErrInvalidCacheIndexMash,
  MrcErrParameterOutOfBound,
  MrcErrMsgMaxError
} OEM_MRC_ERROR;

#endif
