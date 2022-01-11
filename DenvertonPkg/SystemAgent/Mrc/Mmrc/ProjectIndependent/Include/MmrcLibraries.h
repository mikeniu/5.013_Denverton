/** @file
  MMRCLibraries.h
  Library specific macros and function declarations used within the MMRC.
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MMRCLIBRARIES_H_
#define _MMRCLIBRARIES_H_

#include "Mmrc.h"
#include "MmrcProjectData.h"
//#include "EvTools.h" // Denverton Aptiov override
#if JTAG
#include <stdio.h>
#endif

#ifdef __GNUC__
#define __cdecl __attribute__((__cdecl__))
#endif

#define VOC_ZERO_PHYSICAL         7
#define MIN_CMD_DELAY             0
#define MAX_CMD_DELAY             0x36F
#define CMD_HEADER_ENTRY          0
#define CMD_HEADER_INIT           1
#define CMD_HEADER_COMMON         2
#define CMD_HEADER_INDEPENDENT    3
#define CMD_HEADER_TETHER         4
#define CMD_HEADER_FINAL          5

#define RESULTS_PASS              0
#define RESULTS_FAIL              1
#define RESULTS_INIT              0xFF

#define CURRENT_SAMPLE            0
#define FIRST_SAMPLE              1
#define EDGE_SAMPLES              2
//
// Macro definitions for Early Command Training.
//
#define NUMBER_EARLY_COMMAND_PATTERNS 2
#define NUMBER_EARLY_COMMAND_RETRIES  1
#define MRW41                         41 << 4
#define MRW42                         42 << 4
#define MRW48                         48 << 4

#if FAULTY_PART_TRACKING
//
// Faulty part tracking support
//
#define LOOKUP_TABLE_1             0x1001001001001L
#define LOOKUP_TABLE_2             0x84210842108421L
#define MASK_LOWER_12BIT           0xfff
#define MODULE31                   0x1f
#define FAULTY_PART_NO_ERROR       0
#define FAULTY_PART_CORRECTABLE    1
#define FAULTY_PART_UNCORRECTABLE  2
#define BYTE_MASK                  0xFF
#define NIBBLE_MASK                0xF
#endif
#define STROBE_BITS_X8_DEVICES     8
#define STROBE_BITS_X4_DEVICES     4
#define MAX_STROBE_X8_DEVICES      9
#define MAX_LOOP_STROBE_X8_DEVICES      (MAX_STROBE_X8_DEVICES - 1)
#define MAX_STROBE_X4_DEVICES      18
//
// Flag for advanced training multiple timings
//
#define IDX_CLEAR   0xFF
#define IDX_TIMING  0x0
#define IDX_VREF    0x1
//
// Extern variables required for early command training.
//
#if defined SIM || defined JTAG
extern FILE *gLogFile;
#endif
extern UINT32 GetAddress (MMRC_DATA *, UINT8, UINT8);
extern VOID MySimStall(UINT32 count);
extern VOID OemMemoryErrorCode (UINT8);
extern UINT8
FindActiveChannel(
IN  OUT   MMRC_DATA   *MrcData
);


typedef union {
  struct {
    UINT16(*Max)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
   UINT16 Flags;
   UINT16 Index[MAX_TIMING];
   UINT16 Loop;
   UINT16 LoopTotal;
  } ReAdjust;
  struct {
    UINT8 Flags;
    CONTROLKNOBS *ControlKnobs;
    UINT16 ControlIndex;
    UINT8 HiLowFlag;
    UINT16  (*ResultsV)[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
    UINT8 Dim2Index;
    UINT8 Dim2Loop;
  } PrintHiLow;
  struct {
    UINT16  (*Max)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    UINT16  (*Center)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    UINT16  (*ResultsV)[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
    UINT16  NumberSamples;
    UINT16  Flags;
  } FinalCalculateCenter2;
  struct {
    UINT16(*Min)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    UINT16(*Max)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
  } MinMax;
  struct {
    UINT16(*Min)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    UINT16(*Max)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    UINT16(*PICenter)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    UINT8(*AccumWeights)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    UINT8   Final[MAX_TIMING];
  } MinMaxPICenter;
  struct {
    EYEMASK_T *EyeMask;
    UINT8      ControlIndex;
    UINT32     ControlValue;
    CONTROLKNOBS *ControlKnobs;
  } SetCriteria2;
  struct {
    UINT16(*PI)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    UINT16(*Lo)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    UINT8(*Done)[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    UINT8   PBType;
    UINT16  (*Results)[MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
    UINT16  Flags;
    UINT16  PBFlags;
    UINT16  Index[MAX_TIMING];
    INT16   PBIndex;
    UINT32  StrobeValue;
    UINT8  AllDone;
    UINT8  FirstEntryFlag;
    UINT16 (*StrobeVal)[MAX_RANKS][MAX_KNOBS][MAX_STROBES];
    CONTROLKNOBS *ControlKnobs;
  } ProgramSetGet;
  struct {
    UINT32  PI;
    INT8   PBFlags;
    INT16  PBIndex;
    UINT8  AllBits;
    UINT8  (*Done)[MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
  } ProgramSetGetLinear;
  struct {
    UINT8 StrobeValue;
  } GenericPrint2;
  struct {
    UINT8 StrobeValue;
    UINT16 (*PICenter)[MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    UINT8 SharedFlag;
  } GenericPrint;
  struct {
    CONTROLKNOBS *ControlKnobs;
    UINT8 ControlIndex;
    UINT8 EyeMaskIndex;
    EYEMASK_T *EyeMask;
    UINT8 AllDoneFlag;
    UINT8 ResultCount;
  } PrintCount;
  struct {
    CONTROLKNOBS *ControlKnobs;
    UINT8 ControlIndex;
    UINT8 (*PowerKnobs)[MAX_EYEMASKS-1][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS];
    EYEMASK_T *EyeMask;
  } GetSet;
  struct {
    UINT16  (*ResultsV)[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];
    UINT16  (*ResultsS)[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];// Temporary Storage locations for remaining Eyemasks.
    UINT8   (*Count)[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS]; 
    UINT8   MaxCount;        
  } CopyResults;
  struct {
    UINT16  ControlIndex[2];
    UINT8   NumberTimings;
  } StoreRestore;
}
BuildCompositeParams;


VOID
EnableMrcBreakpoint ();

VOID
DisableMrcBreakpoint ();

MMRC_STATUS
GenericMemoryTest (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         NumberElements[MAX_CHANNELS],
  IN        UINT8         NumberKnobs,
  IN  OUT   UINT8         PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3]
);


typedef struct {
  UINT16  Sample[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2];  
} SAMPLE_SWEEP;

VOID
ReadDelayElements (
  IN  OUT   MMRC_DATA    *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT16        Type,
  IN        UINT8         Cmd,
  IN        UINT32       *Value,
  IN        BOOLEAN       HeaderEn
);

MMRC_STATUS
DynamicAssignmentGenerated (
  IN  OUT   UINTX         DynamicVars[MAX_CHANNELS][DYNVAR_MAX],
  IN        UINT8         Channel,
  IN        UINT16        Index,
  IN        UINTX        *Value
)
;
/**
  Supports Late Command Training, Late Read Training, and Late Write training.
  Performs the steps outlined in the Advanced Training Algorihtm document.
  There are two main inputs to the algorithem, Control Knobs and eye Masks.  Both of
  these structures are passed in as they are dependent on the algorithm.  The main algorithm
  can be broken down into 4 fundamental steps.
    
    1.  Determing passing power/Vref given the eyemask.
    2.  Estimate Delay center given the passing eyemasks.
    3.  At the estimated Delay, determine Vref center.
    4.  At the Vref Center, sample/calculate Delay Center.
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ControlKnobs    External array of controls for the algorithm.
  @param[in]       NumberContrlKnbs Number of control knobs array.
  @param[in]       EyeMask         Array of passing EyeMasks.
  @param[in]       NumberEM        Number of eyemasks in the array.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
AdvancedTrainingAlgo (
  IN MMRC_DATA *MrcData,
  IN CONTROLKNOBS *ControlKnobsInNV,
  IN UINT8 NumberControlKnobs,
  IN EYEMASK_T  *EyeMask,
  IN UINT8 NumberEyeMask,
  IN UINT8 PerBitEnable
);

/**
  This function is called from the LoopOnComposite function.  When calculating the center position 
  using Maximum/Medium/Minimum widths method, this function is used to do the summation.  It is assumed
  that the maximum and minimum values are determined prior and setup in the Max and Min array elements.
  The function will look at the current value, compare it with the min/max values and multiply the 
  average eyewidth by the min/mid/max weights. At the end, the sum of the individual min/mid/max weights 
  that were multiplied are then divided.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Lmpb being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ApplyMaxMinEWWeight (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

extern MMRC_STATUS
RestoreDelay (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

extern MMRC_STATUS
StoreDelay (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);
/**
  Using the LoopOnComposite(), this function performs 2 steps: Applies the per-bit setting, and the
  strobelane setting.  The PBFlags and Flags accordingly, if not -1, are used to determine if the 
  appropriate step shoud be called.  It should be noted that the PBFlags and Flags are used if the
  seperate settings are indendent, but if the flags are set to bit-independent, then the flags could
  be used for per-bit setting.  

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Value         Array of values at the granulairty of Flags/PBFlags.
  @param[in]       Flags         Sharing level for the top level (strobes).
  @param[in]       PBFlags       Sharing level for the top level (per-bits).
  @param[in]       Index         GetSet Index for the top level (strobes).
  @param[in]       PBIndex       GetSet Per-bit Index for the top level (per-bits).
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ApplySetting (
  IN  MMRC_DATA *MrcData,
  IN  UINT16 Value[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS], 
  IN  UINT16 Flags,
  IN  UINT16 PBFlags,
  IN  UINT16 Index[MAX_TIMING],
  IN  UINT16 PBIndex,
  IN  CONTROLKNOBS *ControlKnobs
);

/**
  This function is the center for the Advanced Training Algos.  Currently there are 5 possible elements to loop on:
  Channel, Rank, Knob, Strobe, and Bit.  The first 5 loops on each of these elements and if they are not shared
  uses that index and goes to the next element.  If they are shared, the loop goes to the next element but will not loop
  to the next element such that only those elements that are independent will be looping on...
    For example, RANK_SHARED, BIT_SHARED   Ch[0->MAX_CHANNELS],Rn[0],Knob[0->MAX_KNOBS], Sl[0->MAX_STROBES],Bit[0]
  At the innermost loop (bit), all permutations of the non-shared [CH][RN][KN][SL][BI] will be hit.
  The system will not do another 5 loops, looping through each of these elements again but this time doing the opposite.
  Namely, hitting all the shared items... the outermost loops use IndChn, IndRnk,.. inner uses ShrChn, ShrRnk, etc...
  So at the innermost loop now, each shared elements of the unique elements can be checked.

  A couple of features can be accomplished by this:
    1.  Composite, the system can check the ranges of the passed in data and collect the worst range of all.
    2.  Composite count, the system can check the counts if not max, set to 0....
    3.  Test Function can be called with/without the compositing passing in the unique Ch/Rn/Kn/Sl/Bi.
  The test function removes the requirement to have many functions performing the same looping as required by this function.

  @param[in]  *MrcData        Host structure for all data related to MMRC.
  @param[in]  SharedFlag      Flag specifying how the current index is to be combined.
  @param[in]  NumberElements  For CMD training =1, all other, equal to number of strobes active.
  @param[in]  CompositeFlag   Upon looping on each unique item, Results/Count are composited based on the flag.
  @param[in]  NumberCounts    Number of count array elements, needed for printing, (number eyemasks)
  @param[out] Count           Count of number passing Vrefs for each eyemask.
  @param[out] Results         Single 1-D Sweep from Create1DSweep() function.
  @param[in]  Param           Local parameter being passed to test function call.
  @param[in]  TestFunction    Function to call at the end of all the compositing.
**/
void 
BuildComposite (
  IN MMRC_DATA *MrcData,
  IN UINT16 SharedFlag,
  IN UINT8 CompositeFlag, 
  IN UINT8 NumberCounts,
  IN UINT8 MaxCount,
  IN UINT8 Count[][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
  IN UINT16 Results[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN BuildCompositeParams *Param,
  IN MMRC_STATUS TestFunction (MMRC_DATA *, UINT16 *, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, BuildCompositeParams*)
);

/**
  One of two methods to calculate the center for a series of results.  This
  method assumes we do not know anything about the center of the y posisiton, it
  calculates the center based on weights only.  If the flags are perbit, it will
  calculate the per-bit center, if the flags are per-strobelane.  This method is 
  used to estimate the center position when doing advanced training since a center 
  y position is not known.  

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ResultsV        Selected series of data to calculate center.
  @param[in]       Flags           Shared flags of data.
  @param[in]       SelectedEyeMask Used to provide the number of vref samples.
  @param[out]      PICenter        Center results at the granularity of Flags.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CalculateCenter1(
   IN MMRC_DATA *MrcData, 
   IN UINT16 ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2], 
   IN UINT16 Flags,
   IN EYEMASK_T *EyeMask,
   IN UINT8 SelectedEyeMask, 
   IN UINT16 PICenter[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS]
);

/**
  Based on the dim2 center position, this function calculates the center x position
  by taking weights around the center such that as the sweep gets further away from
  the dim2 center, the values have less an impact on the calculations.  This function
  all needs to know the dim2 total range which is passed in.. This was determined by
  using the previous data and calculating the maximum eye width.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ResultsV        Selected series of data to calculate center.
  @param[in]       Flags           Shared flags of data.
  @param[in]       NumberSamples   Number of the dim2 samples.
  @param[out]      PICenter        Center results at the granularity of Flags.
  @param[in]       Max             Maximum eye height from previous sweep.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CalculateCenter2 (
   IN MMRC_DATA *MrcData,
   IN UINT16 ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
   IN UINT16 Flags,
   IN UINT8 NumberSamples,
   OUT UINT16 PiCenter[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
   IN  UINT16 Max[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS]
) ;

/**
  This function takes the data from Results, and compares the data to the passing criteria of the various
  eyemasks.  If the data point passes the eyemask, the data element is copied to the Results[S/V] array and the 
  count is incremented.  The first EyeMask goes into the ResultsV array, the remaining EyeMasks go into ResultsS
  so EM0->ResultsV, EM1->ResultsS[0], EM2->ResultsS[1],etc.  Count[EM0], Count[EM1], Etc. When the eyemask criteria
  is met, the PoweKnobs array is updated; this is needed for all but the first eyemask, since if the eyemask 0 passes,
  the test stops, but all others continue to search.  If the first never passes, the system will look for the prior 
  passing cases and restore those power knobs.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ControlKnobs    Control parameters for each dimension.
  @param[in]       Results         Single 1-D Sweep from Create1DSweep() function.
  @param[in]       ResultsV        Single Eyemask of consecutive sweeps.
  @param[in]       ResultsS        Temporary remaining Eyemask sweeps.
  @param[in]       Count           Count of number passing Vrefs for each eyemask.
  @param[in]       PowerKnobs      Passing powerknobs for each eyemask except first.
  @param[in]       EyeMask         Eyemasks used for this algorithm.
  @retval          MMRC_SUCCESS    Return value.
**/
MMRC_STATUS
CheckAndStoreEyeMasks (
  IN MMRC_DATA *MrcData,
  IN CONTROLKNOBS *ControlKnobs,
  IN UINT16 Results[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2], 
  IN UINT16 ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN UINT16 ResultsS[MAX_EYEMASKS-1][MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2], 
  IN UINT8 Count[MAX_EYEMASKS][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
  IN UINT8 PowerKnobs[MAX_POWER_KNOBS][MAX_EYEMASKS-1][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
  IN EYEMASK_T *EyeMask
);

/**
  This is the generic function that calls the Create1DSweep function.  It does three individual calls to the sweep 
  function for small/medimum/large steps.  If the LastPastInValid flag, which was passed in, was set to false only the first
  "small" step will be run.  This is because the three individual tests are used to speed up the initial sweep only.
  The high/low/dim1startpoint are all passed in to the sweep function.  The 1DSweep only does 1 channel and 1 rank, so 
  each of these sweeps are looped on every active channel/rank and are combined into the results array.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ControlKnobs    External array of controls for the algorithm.
  @param[in]       ControlIndex    Current control index being operated from ControlKnobs array.
  @param[in]       High            High range for the test to execute.
  @param[in]       Low             Low range for the test to execute.
  @param[in]       LastPassInValid Last time function was called, it was invalid, then do not do medium/large steps.
  @param[in, out]  Dim1StartPoint  The starting point and ranges for the 1dsweep.
  @param[out]      Results         Final results.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CompleteSweep (
  IN  MMRC_DATA       *MrcData,
  IN  CONTROLKNOBS    *ControlKnobs, 
  IN  UINT16           ControlIndex, 
  IN  UINT16           High[MAX_CHANNELS][MAX_STROBES], 
  IN  UINT16           Low[MAX_CHANNELS][MAX_STROBES], 
  IN  UINT8            LastPassInValid,
  IN  UINT16           Dim1StartPoint[MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2],
  OUT UINT16           Results[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2]
);

/**
  This function takes the input data and decodes the linear value.  Any number below 0xfa is a linear number
  and the value is simply returned, 0xfa-0xff is a function of the half clock.  The half clock is read on a
  per-channel basis and that is why channel is passed in.  0xff=1x, 0xfe=0x2x, 0xfd=4x, 0xfc=8x, 0xfb=16x, and 0xfa=32x.
  
  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Channel       Current channel being operated on.
  @param[out]      InputData     Input data from ControlKnobs that may be encoded.
  @retval          Data          Returns valid 16-bit value.
**/
UINT16
ConvertRange (
  IN MMRC_DATA *MrcData,
  IN UINT8 Channel,
  IN UINT16 InputData
);

/**
  This function is called from the LoopOnComposite function. At the end of the Determine Passing Vrefs,
  there are two arrays ResultsS and ResultsV.  ResultsV has one EyeMask of data, and ResultsS has the 
  remaining.  The ResultsS will be removed.  ResultsV has only EyeMask[0]'s responses, but before leaving
  those eyemasks that are passing the test needed to be copied to ResultsV.  Thereby, at the end the resultsV
  could have different EyeMasks passing different elements.  The system will check if eyemask0 passed this 
  instance, just return, otherwise find the passing one and copy it from ResultsS to ResultsV.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CopyResults (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

/**
  The recursive function for the determine passing vrefs that will be called for each control
  knob outside of timing and vref.  Each knob will loop over a range given a starting point and
  will start at the starting point going to the max, then returning to the starting point and going
  to the min.  The control knob also has a step size to reach those endpoints.  The function will 
  run across its knob and for each internal loop will either process the vref/timing or call itself 
  with the next knob until finally the vref/timing can be tested.
  The only method this function can be completed finished is if either all the knobs have hit their
  maximum value or the first eyemask has been satisfied.

  @param[in]  *MrcData        Host structure for all data related to MMRC.
  @param[in]  *EyeMask        Pointer to the various eyemasks.
  @param[in]  ControlKnobs    Control parameters for each dimension.
  @param[in]  ControlIndex    Index into the control knobs for current dimension.
  @param[in]  UnknownLastPass Flag specifying if current sweep can use lastpass results.
  @param[in]  Dim1StartPoint  Length for both strings
  @param[in]  Low             Length for both strings
  @param[in]  High            Length for both strings
  @param[out] ResultsV        Single Eyemask of consecutive sweeps.
  @param[out] ResultsS        Temporary remaining Eyemask sweeps.
  @param[out] Results         Single 1-D Sweep from Create1DSweep() function.
  @param[out] Count           Count of number passing Vrefs for each eyemask.
  @param[out] PowerKnobs      Passing powerknobs for each eyemask except first.
  @param[out] AllDoneFlag     TRUE/FALSE flag specifying if EyeMask0 has reached its completion.
**/
MMRC_STATUS 
DeterminePassingVref1 ( 
  IN  MMRC_DATA   *MrcData, 
  IN  EYEMASK_T    EyeMask[], 
  IN  CONTROLKNOBS ControlKnobs[],  
  IN  UINT8        ControlIndex, 
  IN  UINT8        UnknownLastPass,
  IN  UINT16       Dim1StartPoint[MAX_TIMING][MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2],
  IN  UINT16       Low[MAX_TIMING][MAX_CHANNELS][MAX_STROBES],
  IN  UINT16       High[MAX_TIMING][MAX_CHANNELS][MAX_STROBES],
  OUT UINT16       ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  OUT UINT16       ResultsS[MAX_EYEMASKS-1][MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  OUT UINT16       Results[MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  OUT UINT8        Count[MAX_EYEMASKS][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
  OUT UINT8        PowerKnobs[MAX_POWER_KNOBS][MAX_EYEMASKS-1][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
  OUT UINT8        *AllDoneFlag
);

/**
  This is the top function for the DeterminePassingVref step.  This function declares the variables that are needed throughout
  the lifespan of this step as well as initializes the variables needed.  The largest seconday array is the ResultsVTemp structure
  which holds the passing vrefs for the non-primary eye masks.  Rather then making this in the top function, it is put here because
  at the time this step is completed, the only results needed is the passed in ResultsV which has all the necessary data.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       ControlKnobs  Control parameters for each dimension.
  @param[in]       ControlIndex  Index of current control knob.
  @param[out]      ResultsV      Single Eyemask of consecutive sweeps.
  @param[in]       EyeMask       Eyemasks used for this algorithm.
  @param[in]       NumberEyeMask Total number of eyemasks.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
DeterminePassingVref2 ( 
  IN MMRC_DATA *MrcData, 
  IN CONTROLKNOBS ControlKnobs[],  
  IN UINT8 ControlIndex, 
  IN OUT UINT16 ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN EYEMASK_T *EyeMask,
  IN UINT8 NumberEyeMask
);

/**
  This function is called from the LoopOnComposite function.  When calculating the center based on the center position,
  the only additional data required is the maximum Eye Height.  This is passed in on the param structures in Max.
  The function will loop though each sweep giving the larger weight to the center y and less and less weight as the sweep
  sample gets further away from the center.  This goes until NumberSamples is reached.  
  The final center position is then divided by the sum weights.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
FinalCalculateCenter2 (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

/**
  This function is called from the LoopOnComposite function.  All ranges are compared with each
  other and the maximum and minimum values are determined and stored into the max and min array 
  elements.  This is to find the max/min at different dim2 components, not at the flags level.
  Prior to this funtion being called the maximum value must be set to 0, and the minimum value must
  be set to all 0xffff.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Lmpb being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
FindMaxMinEW(
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

extern MMRC_STATUS
FindCommonMaxMinEW(
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Tim,
  IN  BuildCompositeParams *Param
);

/**
  This function is called from the LoopOnComposite function. This is the most generic print function.
  It simply prints out the Channel, rank, and strobe if the strobe and bit index's are set to 0 but also
  prints out the low and high ranges seperated by a ":" for all entries.  The end result is to see :
        000 020:055 010:022 012:054 ... 

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
extern MMRC_STATUS
GenericPrint2 (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

/**
  This function is called from the LoopOnComposite function. This function upon completion of the vref
  passing criteria search, restores the power knobs to the setting that brought about the passing.
  These values are printed out at the end of the DetermineVref() function.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetSet (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

extern MMRC_STATUS
ProgramSetGetLinear (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

/**
  This function is called from the LoopOnComposite function. When doing the vref search for the eyemask 
  that can pass the tests, this is the function that prints out the response.  However, this function 
  should not be commented out as it also is used for determining if the eyemask criteria has search.
  This may be moved to another task, but then another looping function will be needed.  The format 
  of the output is CRK, Ranges for each element and followed by the EM counts.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PrintCount (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

/**
  This functiongoes through all active channel and ranks, and for each knob, strobe, and bit prints out
  the results.  This is a full dump of all the active ranks, it does not look at the flags, it simply
  prints out all responses.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PrintHiLow (
  IN  MMRC_DATA *MrcData, 
  IN BuildCompositeParams *Param
);

/**
  When preparing for the 1DSweep, this function takes the information from the
  specific control knob and sets up the initial  Low/High/Startpoint needed
  for passing to the 1DSweep function.  The Low/High are the physical range for
  the 1dsweep where the Dim1StartPoint is the initial position.  The low/high does
  not get changed, but the startpoint will get modified after each 1DSweep to provide
  the lastpass features.
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ControlKnobs    External array of controls for the algorithm.
  @param[out]      Low             Low range for 1DSweep control.
  @param[out]      High            High range for 1DSweep control.
  @param[out]      Dim1StartPoint  Starting position for 1DSweep control.
  @param[in]       Index           Specifies how the input parameters are used to fill in the output.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
ProgramInitialSettingsFor1DSweep (
  MMRC_DATA      *MrcData, 
  CONTROLKNOBS   ControlKnobs, 
  UINT16          Low[MAX_CHANNELS][MAX_STROBES],
  UINT16          High[MAX_CHANNELS][MAX_STROBES],
  UINT16          Dim1StartPoint[MAX_RANKS][MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2],
  UINT8           Index
);

/**
  This function is called from the LoopOnComposite function.  Upon looking at the flags, specified within the
  param structure, if the per-bit is set and the calculation is set to true, the system will calculate the per-bit
  value.  The per-bit value is done by getting the average of each bit, and subtracting the current value from the
  average.  if the per-bit flag is not set, the system will simply set the value and exit the function.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Lmpb being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ProgramSetGet (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

extern MMRC_STATUS
DetermineStrobeVal (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

extern MMRC_STATUS
CopyToLow (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

extern MMRC_STATUS
TestNonLinearPBForDone (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

/**
  This function is called from the LoopOnComposite function.  It will adjust the value through
  the use of the GetSet.  The offset is taken as 70% of the maximum value.  Given that the default
  value is the center, when "loop" = 0, the delta is determined by taking 70% of the max value 
  eyeWidth, dividing by 2, and subtracting from the current value.  All other loops, it simply adds
  one delta unit which is 70% max eyewidth / Number of samples.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Lmpb being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ReAdjust (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Channel, 
  IN  UINT8 Rank, 
  IN  UINT8 Knob, 
  IN  UINT8 Strobe, 
  IN  UINT8 Bit, 
  IN  UINT8 Timing,
  IN BuildCompositeParams *Param
);

/**
  This function is called from the LoopOnComposite function. This function applies the Set to the specific control.
  The function first tests if the knob needs to be set, and the only reason to not set it is if the first eyemask's
  criteria has been met.  If it has, then exit without setting the knob.  If it needs to still be set, then apply
  the value either.  The only "note" is that this is where the BIT_CONTROL fields is used such that if a register
  has bit-level controls but is not being used at that level, then BIT_CONTROL should be set to 1 and the function
  will set all the bits for the strobe to the same value.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Range         When building a composite, if composite RESULTS/COUNT is specific.
  @param[in]       Channel       Specific Channel being operated on.
  @param[in]       Rank          Specific Rank being operated on.
  @param[in]       Knob          Specific Knob being operated on.
  @param[in]       Strobe        Specific Strobe being operated on.
  @param[in]       Bit           Specific Bit being operated on.
  @param[in]       Param         Param structure that contains non-standard required inputs.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
SetCriteria2 (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Ch, 
  IN  UINT8 Rn, 
  IN  UINT8 Kn, 
  IN  UINT8 St, 
  IN  UINT8 Bi, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

/**
  This function is used twice in the advanced algo function.  When sweeping the Vref N 
  times at specific delays to determine the Vref center and when sweeping the Delays
  N times at specific vrefs to determine the Delay center. The dim1 and dim2 controls are
  the index's into the ControlKnob that are related to the sweep dimensions.  The overrides
  are to provide the overrides to the entries int he controlKnob table; for example, when 
  doing the sweep of vrefs, the delays will need to be knob shared.. but the actual knob is 
  not shared, so the override will have knob_shared.  The final results from the sweep are 
  placed in NewResults.  The original number of samples are needed to determine the maximum
  eyewidth of the current data which is about to be overwritten.  This maximum value is needed
  for the center calculation which is the max EyeHeight for the upcoming sweep.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       ResultsV        Selected series of data to calculate center.
  @param[in]       OrigNumberSamples   Number of the dim2 samples.
  @param[in]       ControlKnobs    Control parameters for each dimension.
  @param[in]       Dim1Control     Index for the X-dim controls.
  @param[in]       Dim2Control     Index for the Y-dim controls.
  @param[in]       Dim1Override    Overrides for dim1 during this sweep.
  @param[in]       Dim2Override    Overrides for dim2 during this sweep.
  @param[out]      NewResultsV     Output results for this sweep.
  @param[in]       NewNumberSamples The actual number of samples for this sweep.
  @param[out]      Max             Maximum Width from current data, prior to new sweep.
  @retval          MMRC_SUCCESS    Return value.
**/
MMRC_STATUS
SweepData1(
   IN  MMRC_DATA *MrcData,
   IN  UINT16 ResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2], 
   IN  UINT16 OrigNumberSamples,
   IN  CONTROLKNOBS *ControlKnobs,
   IN  UINT16 Dim1Control,
   IN  UINT16 Dim2Control,
   IN  UINT8 Dim1Override,
   IN  UINT8 Dim2Override,
   OUT UINT16 NewResultsV[MAX_CONSEC][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
   OUT UINT16 NewNumberSamples,
   IN  UINT16 Max[3][MAX_TIMING][MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS],
   IN  UINT8 UsePreviousMax
);


MMRC_STATUS
CheckCount (
  IN  MMRC_DATA *MrcData, 
  IN  UINT16 *Range, 
  IN  UINT8 Ch, 
  IN  UINT8 Rn, 
  IN  UINT8 Kn, 
  IN  UINT8 St, 
  IN  UINT8 Bi, 
  IN  UINT8 Timing,
  IN  BuildCompositeParams *Param
);

/**
  This routine dumps all registers in the MMRC spreadsheet. 
  On the cover sheet of the SS, the user can control the debug 
  level output: 

  DUMP_REGISTER_NAMES = 1 means all register names, ports, 
  offsets, values, and default values will be printed. 
   
  DUMP_REGISTER_FIELDS = 1 means all register field names, 
  values, default values, and access types will be printed. 
   
  DUMP_REGISTER_DESCRIPTIONS = 1 will display the description 
  for each register field. 

  If DUMP_REGISTER_NAMES is set to 0, the other enables are 
  don't-cares as this is the global enable for this routine.

  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
  DumpAllRegisters (
  );

/**
   Based on the linear delay element specified, compute the
   linear delay.  The linear delay should include the support
   for analog/digital dll.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Socket          Current Socket being examined.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Strobe          Strobe selection for the requested field. 
  @param[in]       Bit             Bit within field being examined.
  @param[in]       FrequencyIndex  Frequency Index.
  @param[in]       IoLevel         I/O Level.
  @param[in]       Type            Register to be accessed, this is pointer to 
                                   an algorithm.
  @param[in]       Cmd             Command to read/write from register/cache.
  @param[out]      Value           Value set/retrieved.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
GetLinearFromRegs (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT16        Type,
  IN        UINT8         Cmd,
      OUT   UINT32        *Value 
  );
 
/**
  Based on the linear delay element specified, it will set the appropriate 
  delay and clock crossing elements to form the linear delay.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Socket          Current Socket being examined.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Strobe          Strobe selection for the requested field. 
  @param[in]       Bit             Bit within field being examined.
  @param[in]       FrequencyIndex  Frequency Index.
  @param[in]       IoLevel         I/O Level.
  @param[in]       Type            Register to be accessed, 
                                   this is pointer to an algorithm.
  @param[in]       Cmd             Command to read/write from register/cache.
  @param[in, out]  Value           Value set/retrieved for the
                                   specified Type.
  @retval          MMRC_SUCCESS
**/ 
MMRC_STATUS 
SetRegsToLinear (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT16        Type,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value 
  );

/**
  This function will set the Minus1Select signal across all ranks and available strobes,
  and will adjust the final delay if this was previously set the Minus1 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Index           Selection of signal group to be averaged. 
  @param[in]       Minus1Index     Minus1 signal.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
HandleMinus1Select (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        Index,
  IN        UINT16        Minus1Index
  );

/**
  Set the Minus1 signal according with the DELAY across Ranks.
  If the R2R switching is disabled, the current function MUST be executed after the average function

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Index           Selection of signal group to be averaged. 
  @param[in]       Minus1Index     Minus1 signal.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
SetMinus1Select (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        Index,
  IN        UINT16        Minus1Index
  );

/**
  Single entry point to MMRC. MRC calls this function
  and then MMRC acts as a black box, performing the requested
  stage of DDRIO init and returns when complete or when
  an error is encountered.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Action          Phy init, training, post-training, etc. 
  @param[in, out]  Parameters      A structure used to pass data between MRC and MMRC.        
  @retval          Status    
**/
MMRC_STATUS
MmrcEntry (
  IN  OUT   MMRC_DATA            *MrcData,
  IN        MMRC_STAGE            Action,
  IN  OUT   HANDSHAKE_PARAMETERS *Parameters
  );

BOOLEAN
MmrcStringCompare (
  IN    VOID    *str1,
  IN    VOID    *str2,
  IN    UINT32  cnt
  )
;

/**
  Overrides the settings for CMD/CTL/CLK

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       CapsuleStartIndex 
  @param[in]       StringIndex 
  @param[in]       Channel 
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CmdOverrideSettings (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  );

/**
  Decreases the latency across ALL delay group to the minimum possible,
  in order to increase the performance

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex
  @param[in]       StringIndex
  @param[in]       Channel
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PerformanceLatency (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  );

/**
  Restores the values of the CMD training for the Dunit
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CommandClockEntry (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  );

/**
  Restores the values of the CMD training for the DDRIO
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CommandRestore (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  );

/**
  Restores the values of the CTL and CLK training for the DDRIO

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GroupSaveRestore (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  );

MMRC_STATUS
ChangeCmdMode (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Mode
);

/**
  Performs the Command Training.  Based on the DIMM configuration (1DPC/2DPC), 
  the training will be completed in 1N/2N mode. No assumption is 
  made on the beginning mode for the Command bus; however, 
  memory should be stable prior to entering. Channel is the only 
  additional parameter passed in, and the calibration will be 
  performed on the channel specified. 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CommandClockTraining (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );
MMRC_STATUS
TestCycle (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  );

MMRC_STATUS
ReInitializeFunction (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  );

MMRC_STATUS
JedecInitFunction (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  );

MMRC_STATUS
VocTraining (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  )
;
/**
  Performs the Receive Enable calibration.  All results from the calibration
  are stored within the MrcData within the TrainingData substructure.  
  Channel is the only additional parameter passed in, and the calibration will 
  be performed on the channel specified.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ReceiveEnable (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT16        CapsuleStartIndex, 
  IN        UINT16        StringIndex, 
  IN        UINT8         Channel
  );

/**
  Fine Write Leveling training algorithm
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
FineWriteLeveling (
  IN OUT MMRC_DATA         *MrcData,
  IN     UINT16            CapsuleStartIndex,
  IN     UINT16            StringIndex, 
  IN     UINT8             Channel
  );

/**
  Executes the Power training on SoC Odt 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]  CapsuleStartIndex    Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          NONE
**/
MMRC_STATUS
SocOdtTraining (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );

/**
  Executes Read Training 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]  CapsuleStartIndex    Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          NONE
**/
MMRC_STATUS
ReadTraining (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );

/**
  Executes Advanced Write Training  
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
WriteTraining (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );

/**
  Executes the Power training on Dram Rtt Nom and Rtt Wr 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]  CapsuleStartIndex    Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.    
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
DimmOdtTraining (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
)
;

/**
  Performs Early MPR Read Training. This training must be executed only before CWL in order to set
  safe read timing settings
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
EarlyMprRead (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );

/**
  Performs Coarse Write Leveling.  This is reusing many of the same routines used with the Receive Enable 
  calibration.  
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.    
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CoarseWriteLeveling (
  IN OUT MMRC_DATA         *MrcData,
  IN     UINT16            CapsuleStartIndex,
  IN     UINT16            StringIndex, 
  IN     UINT8             Channel
  );

/**
  Prints the information gotten from the training results
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
PhyViewTable (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  );

/**
  For Command Training, prints the three values associated with a command training group.
  Low/High/Default values.  The command informs which value should be printed and which values
  should be shown with xxx.

  @param[in]       Command         Command to inform which value is to be printed or masked out.
  @param[in]       LowValue        Low Value associated with the group.
  @param[in]       HighValue       High Value associated with the group.
  @param[in]       DefaultValue    Default value associated with the group.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
PrintCommandLowHigh (
  IN        UINT8         Command,
  IN        UINT32        LowValue,
  IN        UINT32        HighValue,
  IN        UINT32        DefaultValue
  ); 

/**
  It will generate a reset signal for both channels with the current settings for each Channel

  @param[in, out]  MrcData         Host structure for all data related to MMRC.

  @retval          MMRC_SUCCESS
**/
VOID
ReInitializeDramsAcrossChannels(
  IN OUT    MMRC_DATA     *MrcData
);

/**
  Resets the DRAMs by reinitializing the Drams and re-performing JEDEC on all available ranks for the provided Channel.
  CPGCEnabled is a flag passed in that informs the function if it should disable/enable CPGC prior/post to performing JEDEC.
  CPGC MUST be disable during JEDEC.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       CPGCEnabled     Flag to set/clear CPGC during JEDEC.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ReInitializeDrams (
  IN OUT    MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         CPGCEnabled
  );

/**
  Implements Diffamp, Odt and tRD Dynamic Formulas
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
PerformanceSetting (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );

/**
  Check basic margins during fast boot and S5
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
FastBootChecker (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );

/**
  Executes Rank Margin Tool Setup

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
SearchRmtWrapper (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  );

/**
  Executes Rank Margin Tool
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.

  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
SearchRmt (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel
  );

/**
  Executes CtleTunning for DDR4
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CtleTunning (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  );
MMRC_STATUS
Center2D (
  IN          MMRC_DATA       *MrcData,
  IN   OUT    SAMPLE_SWEEP    *Results,
  IN          UINT8           Channel,
  IN          UINT8           Rank,
  IN          UINT16          Dim1Index,
  IN          UINT8           Dim1Knobs,
  IN          UINT16          Dim1Min[MAX_CHANNELS][MAX_STROBES],
  IN          UINT16          Dim2Index,
  IN          UINT16          Dim2Min[MAX_CHANNELS][MAX_STROBES],
  IN          UINT16          Dim2Step,
  IN          UINT8           Dim2ConsolidateStrobes
  );

/**
  This does a copy from the register to CACHE (Trained_Value) if the


  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       TrainingDelay   Training we want to print delay.
  @param[in]       BlastDirection  0: From REG to CACHE
                                   1: From CACHE to REG
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
BlastFinalResults (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        TrainingDelay,
  IN        UINT8         BlastDirection
);

/**
  Prints the Training Results

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       TrainingDelay   Training we want to print delay.
  @param[in]       Restore         If set the function will restore training values form MRC's cache
                                   else it will display the training results.
  @retval          MMRC_SUCCESS
**/ 
MMRC_STATUS 
DisplayOrRestoreTrainingResults (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        TrainingDelay,
  IN        BOOLEAN       FaultyPart
  );

MMRC_STATUS
SampleDqs (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         NumberElements[MAX_CHANNELS],
  IN        UINT8         NumberKnobs,
  IN  OUT   UINT8         PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3]
  );

MMRC_STATUS
BytelaneMask (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        *Mask
  );

MMRC_STATUS
FillCacheArray (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *Ptr,
  IN        UINT8         Data,
  IN        UINT8         *Offset
  );

MMRC_STATUS
BuildCacheArrays (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *CL,
  IN        UINT8         *Patterns,
  IN        UINT32        NumberPatterns,
  IN        UINT32        *NumCL
  );

MMRC_STATUS
Enable16KMTRR (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        TargetAddress,
  IN        UINT32        Msr
  );

MMRC_STATUS
LoadXmm03WithPattern (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *CacheLines
  );

MMRC_STATUS
BurstOutCachelinesViaXmm (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        Address,
  IN        UINT32        CacheLineRepeatCount
  );

MMRC_STATUS
CompareCacheWithXmm47 (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        TargetAddress,
  IN        UINT32        CacheLineRepeatCount,
  IN        UINT32        *EncodedFailPtr
  );

MMRC_STATUS
ReadRamIntoCache (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        TargetAddress,
  IN        UINT32        CacheLineRepeatCount
  );

MMRC_STATUS
GeneratePattern (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        VictimPattern,
  IN        UINT32        AggressorPattern,
  IN        UINT8         ByteShift[],
  IN        UINT8         BitShift,
  IN        UINT8         VictimRepeat,
  IN        UINT8         EvenOddFlag,
  IN        VOID          *BufferPv
  );

MMRC_STATUS
FillGoldenBuffer (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        *PatternIndex,
  IN        UINT32        BufferAddress
  );

MMRC_STATUS
TransferMemory (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *DstAddr,
  IN        UINT8         *SrcAddr,
  IN        UINT32        CacheLines,
  IN        UINT8         InvertFlag,
  IN        UINT32        *AllOnes
  );

MMRC_STATUS
CompareGoldenWithDRAMPatterns (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        GoldenPatternAddress,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT16        *CompareResults
  );

MMRC_STATUS
CompareMemory (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *GoldenAddress,
  IN        UINT8         *TestAddr,
  IN        UINT32        CacheLines,
  IN        UINT32        *AllOnes,
  IN        UINT8         InvertFlag,
  IN        UINT16        *Results
  );

#if !CPGC_API

MMRC_STATUS
DisableMTRR (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        Msr
  );
#ifndef MINIBIOS
#if !defined SIM && !defined JTAG

UINT64
Shr64 (
  IN        UINT64        Value,
  IN        UINT8         Shift
  );

UINT64
Shl64 (
  IN        UINT64        Value,
  IN        UINT8         Shift
  );
#endif // SIM
#endif // MINIBIOS
#endif // CPGC_API

/**
  Based on the input type, this function will average out the 2x
  values for all ranks on the given channel.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       StartIndex      Pointer to the first Delay in the Group
  @param[in]       AllGroups       Sweep all groups
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
AverageGroups (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        StartIndex,
  IN        BOOLEAN       AllGroups
  )
;

/**
  Based on the input type, this function will average out the 2x 
  values for all ranks on the given channel. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Index           Selection of signal group to be averaged. 
  @param[in]       Minus1Index     Minus1 signal.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
Average (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        Index,
  IN        UINT16        Minus1Index
  );

/**
   Retrieves the Byteoffset, starting, and ending bit for a given channel, rank and strobe.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Element         Information on the description and flags of the desired field.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Strobe          Strobe selection for the requested field. 
  @param[in]       Bit             Bit within field being examined.
  @param[out]      Instance        Instance being specified when looping on total Instances.
  @param[out]      ByteOffset      Final ByteOffset based on Channel/Rank/Strobe.
  @param[out]      StartingBit     Final Starting bit for the field based on Channel/Rank/Strobe.
  @param[out]      EndingBit       Final Ending bit for the field based on Channel/Rank/Strobe.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetLocation (
  IN  OUT MMRC_DATA         *MrcData,
  IN      GET_SET_ELEMENTS  Element,
  IN      UINT8             Channel,
  IN      UINT8             Rank,
  IN      UINT8             Strobe,
  IN      UINT8             Bit, 
  OUT     UINT8             *Instance,
  OUT     UINT32            *ByteOffset,
  OUT     UINT8             *StartingBit,
  OUT     UINT8             *EndingBit
  );
  
/**
  Based on the Type input, returns the maximum possible value and the wait time
  required after, if necessary, chaining the value.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Socket          Current Socket being examined.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.   
  @param[in]       Level           I/O Level.
  @param[in]       Type            Register to be accessed, this is pointer to 
                                   an algorithm.
  @param[out]      MaxLimit        Maximum value allowed on this signal group.
  @param[out]      WaitTime        Amount of time in uSec to wait after programming 
                                   a new value.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetDdrioGroupLimits (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8          Socket,
  IN        UINT8          Channel,
  IN        UINT8          Dimm,
  IN        UINT8          Level,
  IN        UINT16         Type,
  OUT   UINT32        *MaxLimit,
  OUT   UINT32        *WaitTime
  );

/**
  Routine to save size which doesn't include all the parameters
  of the real GetSetDdrioGroup. Use this routine for a single level memory 
  configuration or when there is only a single level of memory per channel.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Strobe          Strobe selection for the requested field.
  @param[in]       Type            Register to be accessed, this is pointer to
                                   a register or algorithm in the Elements[] table.
  @param[in]       Cmd             Command to read/write from register/cache.
  @param[in]       Value           Value set/retrieved for the specified Type.
  @retval          MMRC_SUCCESS
  @retval          STATUS_TYPE_NOT_SUPPORTED
  @retval          STATUS_LIMIT
**/
MMRC_STATUS
GetSetDdrioGroup2 (
  IN  OUT   MMRC_DATA    *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT16        Type,
  IN        UINT8         Cmd,
  IN        UINT32       *Value
  );

/**
  Routine to save size which doesn't include all the parameters
  of the real GetSetDdrioGroup. Use this routine for a single level memory 
  configuration or when there is only a single level of memory per channel.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Socket          Current Socket being examined.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Strobe          Strobe selection for the requested field. 
  @param[in]       Bit             Bit within field being examined.
  @param[in]       FrequencyIndex  Frequency Index.
  @param[in]       IoLevel         I/O Level.
  @param[in]       Element         Register to be accessed, this is pointer to 
                                   a register or algorithm in the Elements[] table.
  @param[in]       Cmd             Command to read/write from register/cache.
  @param[in]       Value           Value set/retrieved for the specified Type.
  @retval          MMRC_SUCCESS
  @retval          STATUS_TYPE_NOT_SUPPORTED
  @retval          STATUS_LIMIT
**/
MMRC_STATUS
GetSetDdrioGroup (
  IN  OUT   MMRC_DATA    *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT16        Type,
  IN        UINT8         Cmd,
  IN        UINT32       *Value
  );

MMRC_STATUS
HandleDdrioPhyInit (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        DummyChannel
  );

MMRC_STATUS
HandleMemoryTraining (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        DummyChannel
  );

MMRC_STATUS
Create2DEye (
  IN          MMRC_DATA       *MrcData,
  IN   OUT    SAMPLE_SWEEP    *Results,
  IN          UINT8           Channel,
  IN          UINT8           Rank,
  IN          UINT16          Dim1Index,
  IN          UINT8           Dim1Knobs,
  IN          UINT16          Dim1Min[MAX_CHANNELS][MAX_STROBES],
  IN          UINT16          Dim1Step,
  IN          UINT16          Dim2Index,
  IN          UINT16          Dim2Min[MAX_CHANNELS][MAX_STROBES],
  IN          UINT16          Dim2Step
  );

MMRC_STATUS
SweepCenterEye2D (
  IN   MMRC_DATA    *MrcData,
  IN   UINT8        Channel,
  IN   UINT8        Rank,
  IN   UINT16       Dim1Index,
  IN   INT16        Dim1Start,
  IN   INT16        Dim1End,
  IN   UINT16       Dim1Step,
  IN   UINT8        Dim1Flag,
  IN   UINT16       Dim2Index,
  IN   INT16        Dim2Start,
  IN   INT16        Dim2End,
  IN   UINT16       Dim2Step,
  IN   UINT8        Dim2Flag
  );

/**
  Translates the value according to the absolute/relative flag

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Index           Selection of signal group to be averaged. 
  @param[in]       Start           Start. 
  @param[in]       End             End.   
  @param[in]       Flag            ABSOLUTE/RELATIVE Flag.
  @param[in]       Min             Returned Start.
  @param[in]       Max             Returned End.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetMinMaxRelativeAbsolute (
  IN   MMRC_DATA    *MrcData,
  IN   UINT8        Channel,
  IN   UINT8        Rank,
  IN   UINT16       Index,
  IN   INT16        Start,
  IN   INT16        End,
  IN   UINT8        Flag,
  IN   UINT16       Min[MAX_CHANNELS][MAX_STROBES],
  IN   UINT16       Max[MAX_CHANNELS][MAX_STROBES]
  );

MMRC_STATUS
Create2DSweepSim (
  IN   MMRC_DATA    *MrcData,
  IN   OUT  SAMPLE_SWEEP Results[MAX_ELEMENTS_DIM_Y],
  IN   UINT8        Channel,
  IN   UINT8        Rank,
  IN   UINT8        Dim1Index,
  IN   UINT8        Dim1Knobs,
  IN   INT16        Dim1Start,
  IN   INT16        Dim1End,
  IN   UINT16       Dim1Step,
  IN   UINT8        Dim1Flag,
  IN   UINT8        Dim2Index,
  IN   INT16        Dim2Start,
  IN   INT16        Dim2End,
  IN   UINT16       Dim2Step,
  IN   UINT8        Dim2Flag
  );

MMRC_STATUS
Create2DSweep (
  IN   MMRC_DATA    *MrcData,
  IN   OUT  SAMPLE_SWEEP Results[MAX_ELEMENTS_DIM_Y],
  IN   UINT8        Channel,
  IN   UINT8        Rank,
  IN   UINT16       Dim1Index,
  IN   UINT8        Dim1Knobs,
  IN   INT16        Dim1Start,
  IN   INT16        Dim1End,
  IN   UINT16       Dim1Step,
  IN   UINT8        Dim1Flag,
  IN   UINT16       Dim2Index,
  IN   INT16        Dim2Start,
  IN   INT16        Dim2End,
  IN   UINT16       Dim2Step,
  IN   UINT8        Dim2Flag
  );

/**
  Unified routine to do initial setup on all cpgc routines

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       SetupType       Chooses which type of CPGC setup to perform. 
                                   Typically one per algo.
  @retval          MMRC_SUCCESS    In case the setup is done properly
**/
MMRC_STATUS
CpgcSetup ( 
  IN  OUT   MMRC_DATA    *MrcData,
  IN        UINT8        Channel,
  IN        UINT8        SetupType
  );

/**
  Similar description as CpgcSetup.  This is called after CpgcSetup 
  for a particular SetupType, to reconfigure it to a new rank, 
  loopcount, or algo specific change during the algorithm. All 
  these kind of options are passed through MrcData->CpgcOptions 
 
  WARNING:  A call to CpgcSetup is required before using CpgcReconfig !!
            This is true for each algo (need to call CpgcSetup first
            inside each algo) 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CpgcReconfig (
  IN OUT  MMRC_DATA  *MrcData,
  IN      UINT8      Channel
  );

MMRC_STATUS
CpgcRunTest(
  IN OUT  MMRC_DATA  *MrcData,
  IN      UINT8      Channel,
  OUT     UINT16     *BytelaneStatus,
  OUT     UINT32     LaneStatus[],
  OUT     UINT32     *ErrorCount
  );

MMRC_STATUS
SetCpgcRank(
  IN OUT  MMRC_DATA  *MrcData,
  IN      UINT8      Channel,
  IN      UINT8      Rank
  );

/**
  DoZqCal Long across ranks within a channel

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
DoZqCal (
  IN     MMRC_DATA *MrcData,
  IN     UINT8      Channel
  );

/**
  Precharge all will detect the DDR type DDR3 vs DDR4 and apply 
  the correct command 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PrechargeAll (
  IN     MMRC_DATA *MrcData,
  IN     UINT8      Channel,
  IN     UINT8      Rank
  );

MMRC_STATUS
NopCommands (
  IN     MMRC_DATA *MrcData,
  IN     UINT8      Channel,
  IN     UINT8      Rank
  );
/**
  Wrapper function to call memory init or memory test.  Also handles error
  conditions such as whether or not to halt the system on errors 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined. 
  @param[in]       MemInitFlag     0 for memtest, 1 for meminit. 
   
  @retval          Status of success or failure, depending on 
                   errors observed. Also prints out which rank
                   failed the CPGC test if failure occurs. For
                   rank detection to work, the address map must
                   be left in its default state. If MRC changes
                   the address map, this function must be
                   modified to change it back to default at the
                   beginning, then restore it at the end.
**/
MMRC_STATUS
MemInit (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel,
  IN        UINT8        MemInitFlag
  );

/**
  This function develops the scrub memory functionality 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ScrubMemory (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  );

/**
  This function displays all the information related with Dunit 
  settings, Dimm/Dram Manufacturer, etc 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
DisplayInformation (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  );

/**
  This function does a memory testing 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
MemoryTest (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  );

/**
  Start the Cpgc test in both channels (only if both are
  enabled). DynamicVars will contains all the errors found
  during the execution of the test

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @retval          MMRC_SUCCESS    In case the setup is done properly
**/
MMRC_STATUS
CpgcExecuteParallel (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel,
  IN        UINT8        Rank
  );


/**
  This routine sweeps any signal and returns the edges where the signal transitions from pass to fail.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Rank to sweep.
  @param[in]       Dim1Index       Signal type to sweep (i.e. RxDqsDelay, TxVref, CmdGrp0, etc).
  @param[in]       Dim1Knobs       How many knobs does this signal have? Most are 1 but ones 
                                   like RxDqsDelay can have a HI and LOW (or P and N) component. 
                                   For this case, Dim1Knobs would be 2
  @param[in]       Dim1Start       Starting value (per channel/knob/strobe/edge) for both the 
                                   high and low edges.
  @param[in]       Dim1Min         The minimum value to allow the sweep to hit before stopping.
  @param[in]       Dim1Max         The maximum value to allow the sweep to hit before stopping.
  @param[in]       Dim1Step        Step size for sweep. Set to 1 for maximum detail or higher for speed improvements
                                   at the expense of accuracy.
  @param[in]       FullSweep       THIS IS NOT CURRENTLY IMPLEMENTED. If TRUE, sweep the entire range from min to max.
  @param[in] SkipInitialValueProgramming When TRUE, do not program initial low edge starting values or high edge 
                                         starting values. This is useful when you want to margin a signal starting 
                                         at its current value or when margining a group of signals that need to stay 
                                         tethered together. For example, in CMD training if CmdGrp0 and CmdGrp1 need
                                         to sweep together but be programmed to different values (i.e. tethered), 
                                         this would be set the TRUE so the two groups aren't programmed to an identical'
                                         starting value. If the user wants to skip the initial value programming,
                                         they MUST assign Dim1Start[][][][] to the current register settings upon entry
                                         to this routine.
  @param[in]       NumberElements  How many elements are we sweeping? Typically, MAX_STROBES for strobe based delays or 1 
                                   for CMD signals or others which aren't strobe-based.
  @param[in]       ExecuteTest     Pointer to the point test function which evaluates pass/fail for the current value
                                   of the signal.
  @param[in, out]  Results         Results array returning the high/low edges per channel/knob/strobe.
  @param[in]       HeaderLabel     Label to print if debugging is turned on. If "S" then the header will be a strobe 
                                   header like S00   S01   S02, etc.
  @param[in]       DitherVal       The number of successive samples that need to be either PASS or FAIL before this routine
                                   considers an edge to be found. For example, if the first sample is FAIL, the sweep will
                                   look to read X number of PASS values in a row before it considers this a FAIL to PASS
                                   edge. X = DitherVal.
  @param[in]       Stage           Determines the function's behavior, valid parameters are:
                                   RUN_PER_STROBE - Detects margins within strobe
                                   RUN_PER_BIT    - Detects margins within bits
                                   RUN_PER_STROBE_AND_BIT - Detects margins first within strobe and then executes per bit (Note: Bit0
                                   will always contain the strobe result)
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
Create1DSweep (
  IN         MMRC_DATA    *MrcData,
  IN         UINT8        Channel,
  IN         UINT8        Rank,
  IN         UINT16       Dim1Index, 
  IN         UINT8        Dim1Knobs,
  IN         UINT16       Dim1Start[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2], 
  IN         UINT16       Dim1Min[MAX_CHANNELS][MAX_STROBES],
  IN         UINT16       Dim1Max[MAX_CHANNELS][MAX_STROBES],
  IN         UINT16       Dim1Step,
  IN         BOOLEAN      FullSweep,
  IN         BOOLEAN      SkipInitialValueProgramming,
  IN         BOOLEAN      PerBitSweep,
  IN         UINT8        NumberElements[MAX_CHANNELS],
  IN         MMRC_STATUS  ExecuteTest(MMRC_DATA *, UINT8, UINT8, UINT8 NumberElements[MAX_CHANNELS], UINT8, UINT8 PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3]),
  IN   OUT   UINT16       Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN         UINT8        *HeaderLabel,
  IN         UINT8        DitherVal,
  IN         UINT8        Stage
  );

/**
  Compares the initial sample with the current sample. If they don't match, it increases a counter by 1. When this
  counter equals DitherVal it means we've found an edge. Higher dither values means we look for a stronger edge. 
  For example if DitherVal is 3 and the first sample if FAIL, it means this routine will return TRUE after sampling
  3 PASS values in a row.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       MaximumBits     Max bits per strobe
  @param[in]       PerBitSweep     Indication if we are running per bit (PerBitSweep = TRUE) or per strobe (PerBitSweep = FALSE)
  @param[in]       PassFail        Array of samples performed by the sweep function.
  @param[in]       DitherVal       The number of successive samples that need to be either PASS or FAIL before this routine
                                   considers an edge to be found. For example, if the first sample is FAIL, the sweep will
                                   look to read X number of PASS values in a row before it considers this a FAIL to PASS
                                   edge. X = DitherVal.

  @retval          TRUE/FALSE       Is the edge found?
**/
BOOLEAN
FoundEdge (
    IN        MMRC_DATA    *MrcData,
    IN        UINT8        MaximumBits,
    IN        BOOLEAN      PerBitSweep,
    IN        UINT8        (*PassFail)[3],
    IN        UINT8        DitherVal
)
;

/**
  Saves the MRC host structure to an MMX register for future use.

  @param[in, out]  MrcDataAddress  Address of MRC Host Structure.
  @retval          NONE
**/
VOID
SaveMrcHostStructureAddress (
  IN    MMRC_DATA   *MrcDataAddress
  );

/**
  Returns the MRC host structure from an MMX register.

  @retval          MrcDataAddress  Address of MRC Host Structure.
**/
MMRC_DATA *
GetMrcHostStructureAddress (
  );

VOID 
MrcDeadLoop (
  );

MMRC_STATUS
CpgcExecuteSingleTest (
  IN  OUT   MMRC_DATA     *MrcData, 
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  OUT       UINT32        *CompareFlag,
  OUT       UINT32        *LaneErrorHigh,
  OUT       UINT32        *LaneErrorLow
  );

/**
  CPGC routines to begin the CPGC Test, poll for the completion, 
  after the test is done return the the status of the test which 
  is a bytelane+ecc bit-test. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined. 
  @param[in]       NumberElements  Number of elements to test 
                                   (i.e. MAX_STROBES or MAX_BITS, etc).
  @param[in]       NumberKnobs     Number of knobs.
  @param[in, out]  PassFail        Array of passing or failing result for every element.
  @retval          MMRC_SUCCESS    If all elements passed.
  @retval         !MMRC_SUCCESS    Otherwise.
**/
MMRC_STATUS
CpgcExecuteRmtTest (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         NumberElements[MAX_CHANNELS],
  IN        UINT8         NumberKnobs,
  IN  OUT   UINT8         PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][3]
 );

BOOLEAN
IsEccStrobe (
  MMRC_DATA *MrcData,
  UINT8      Channel,
  UINT8      Strobe
  );

/**
  IsStrobeValid.   Is used to select the strobe when the platforms
  supports x4 and x8 configurations also provides the MaxBit value 
  that should be used by that strobe. On x4 configurations MaxBit would be4
  and on x8 configurations MaxBit should be 8

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Rank.
  @param[in]       Strobe          Strobe.
  @param[in]       BitLevel        Enables the fill to the return value for MaxBit.
  @param[out]      MaxBit          1 if no bit control needed, 4 or 8 depending on the device width configuration.
  @retval          MMRC_SUCCESS
**/
BOOLEAN 
IsStrobeValid (
  IN OUT MMRC_DATA *MrcData,
  IN     UINT8      Channel,
  IN     UINT8      Rank,
  IN     UINT8      Strobe,
  IN     BOOLEAN    BitLevel,
  OUT    UINT8     *MaxBit
  );

/**
  Performs Early Command Training.   If the SEARCH_FOR_EARLY_CMD_PASS 
  is set, the system will test the default value and verify that 
  this value passes the test, if it fails, it will begin 
  searching for a default value checking +/- 1/2 CLK. Otherwise, 
  if the SEARCH_FOR_EARLY_CMD_PASS is not set, the system will 
  "assume" the default value is passing and will search for the 
  best value. 
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
EarlyCommandTraining (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex, 
  UINT8             Channel
  );

/**
  Sets the Command/Address pins on the CA Bus for LPDDR3. This 
  function will transmit the LPDDR3 Rise/Fall data on the CA 
  Bus.  The 2 modes are 3N Mode or normal mode.  When in 3N 
  mode, the chip select is extended to 3 clocks (1 full clock 
  before the data and 1 full clock after the data), the data is 
  extended to 2 clocks with the data being sent out 1/2 clock 
  before the data and left 1/2 clock.  The normal mode is for 
  the CS to be enabled for just 1 clock, and the rise/fall data 
  to be applied on the rise/fall of the 1 clock. 
  

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       CARise          Data to be put on the LPDDR3 Rising clock data.
  @param[in]       CAFall          Data to be put on the LPDDR3 Rising clock data.
  @param[in]       Mode3N          Binary flag on whether the data pattern should be 
                                   transmitted in 3N mode, 
                                   0-> Normal Mode, 1-> Extended Mode.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CommandAddressPinControl (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT16        CARise,
  IN        UINT16        CAFall,
  IN        UINT8         Mode3N
  );

/**
  For Early Command training, tests the CA Bus by transmitting a known set 
  of patterns across the CA Bus a number of times.  The actual 
  number of retries is a parameter to this algorithm.  The 
  number of patterns and the patterns themselves are compile 
  time generated. 
  
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined. 
  @param[in]       Retries         Runtime parameter to support the number 
                                   of retries that should be executed on the
                                   pattern to verify errors/no errors occurred.
  @retval          TRUE            If Passes 
  @retval          FALSE           Otherwise 
**/
MMRC_STATUS
CATrainingResultsPass (
  IN OUT    MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         Retries
  );

/**
  Test function the is called from the sweep for Early Command Training. 
  It simply calls the CATrainingPass, however this function 
  returns true if pass, where as this function requires 1 for 
  failure. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined. 
  @param[in]       NumberKnobs     Number of knobs.
  @param[in]       NumberElements  Number of elements to test 
                                   (i.e. MAX_STROBES or MAX_BITS, etc).
  @param[in, out]  PassFail        Array of passing or failing result for every element.         
  @retval          MMRC_SUCCESS    If all elements passed.
  @retval         !MMRC_SUCCESS    Otherwise.
**/
MMRC_STATUS
EarlyCmdTrainingTest (
  IN  OUT   MMRC_DATA     *MrcData, 
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         NumberKnobs,
  IN        UINT8         NumberElements,
  IN  OUT   UINT8         PassFail[MAX_KNOBS][MAX_STROBES][2]
  );

/**
  Pass Gate Characterization Test
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PassGateTest (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  );

MMRC_STATUS
GetLowHighMarginPerStrobe (
  IN   MMRC_DATA    *MrcData,
  IN   UINT8        Channel,
  IN   UINT8        Rank,
  IN   UINT16       Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN   UINT32       CurrentValue[MAX_STROBES],
  IN   UINT16       DelaySignal,
  IN   UINT8        RmtLowDataOffset,
  IN   UINT8        RmtHighDataOffset
  );

#if FAULTY_PART_TRACKING
/**
  Count the number of bits set on a 32 bit segister with 6 arihmetic operations

  @param[in]       Value     32 bit value to operate on 
  @retval          The number of bits set on the 32 bit value    
**/
UINT8
CountOnes(
  UINT32  Value
  );
#endif

extern UINT16     mDdrMemoryFrequencyTable[C_MAXDDR];
extern MMRC_DATA *MyMrcData;
extern MMRC_STATUS

SendBramFlush (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT32                Channel,
  IN        UINT32                Data
  );

BOOLEAN
IsCmdSignalDelay ( 
  IN   MMRC_DATA    *MrcData,
  IN   UINT16       Delay
  );

BOOLEAN
IsCtlSignalDelay (
  IN   MMRC_DATA    *MrcData,
  IN   UINT16       Delay
  );

MMRC_STATUS
PrintFooter1DSweep (
  IN   MMRC_DATA    *MrcData,
  IN   UINT8        Channel,
  IN   UINT8        Rank,
  IN   UINT16       Dim1Index,
  IN   UINT8        Dim1Knobs,
  IN   UINT16       Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN   BOOLEAN      PerBitSweep
  );

MMRC_STATUS
PrintRmtLaneStatus (
  IN   MMRC_DATA  *MrcData,
  IN   UINT8      Channel,
  IN   UINT8      Rank,
  IN   UINT8      Strobe,
  IN   UINT8      MaximumBits,
  IN   BOOLEAN    PerBitSweep,
  IN   UINT8      LaneErrorVector[MAX_STROBES]
  );

MMRC_STATUS
PrintRmtSummaryPerBit (
  IN   OUT   MMRC_DATA    *MrcData,
  IN         UINT8        Channel,
  IN         UINT8        Rank,
  IN         UINT8        Strobe[MAX_CHANNELS],
  IN         UINT8        Dim1Knobs,
  IN         UINT16       Dim1Start[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2],
  IN         UINT8        StepSize,
  IN         UINT16       Results[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][MAX_BITS][2],
  IN         BOOLEAN      PerBitSweep
  );

VOID
GetMaxDq (
  IN         MMRC_DATA    *MrcData
  );

VOID
ForceMaxBit (
  IN         MMRC_DATA    *MrcData,
  IN         UINT8        MaxBit
  );

/**
  Returns the index into the SignalInfo array which matches the signal index
  into the Elements array.

  @param[in]       Signal         Index into the Elements array.
  @retval                         SignalInfo Index 
**/
UINT16
GetSignalInfoIndex (
  UINT16 Signal
  );

UINT16 
GetHalfClk (
  IN        MMRC_DATA         *MrcData,
  IN        UINT8             Channel
  );

UINT16 
GetQtrClk (
  IN        MMRC_DATA         *MrcData,
  IN        UINT8             Channel
  );

UINT16 
GetOneClk (
  IN        MMRC_DATA         *MrcData,
  IN        UINT8             Channel
  );

UINT16 
GetOffsetIndex (
  IN          MMRC_DATA       *MrcData,
  IN          UINT16          Type
  );

/**
  Finds the blueprint that corresponds to the current 
  configuration. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       IoLevel 
  @param[out]      Blueprint        
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ConvertPfctAndIoLevelToBlueprint (
  IN        MMRC_DATA  *MrcData, 
  IN        UINT8       Channel, 
  IN        UINT8       IoLevel,
     OUT    UINT8      *Blueprint
  );

/**
  This routine converts a PFCT value from BITx to x. 
  The Current* variables must not be in BITx format. For example 
  if CurrentPlatform is set to BIT4, this routine sets 
  CurrentPlatform to 4 instead. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          NONE 
**/
VOID
ConvertPfctFromBitXToX (
  IN        MMRC_DATA         *MrcData
  );

/**
  This function compares the current PFCT of this channel and 
  match it to the list of allowable PFCTs for this Task in the 
  call table. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       j               Task Descriptor.
  @retval          NONE
**/
VOID
CreateChannelMask (
  IN        MMRC_DATA            *MrcData,
  IN        TASK_DESCRIPTOR      *j
  );

/**
  This function tells if a Channel should be skipped/executed 
  when looping through Channels. 

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @retval          TRUE            Run in this channel 
  @retval          TRUE            Skip this channel   
**/
BOOLEAN
RunOnThisChannel (
  IN        MMRC_DATA  *MrcData,
  IN        UINT8       Channel,
  IN        UINT8       Rank
  );

UINT32
TestMrcTrainingSteps (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             SetupType,
  IN      UINT8             CmdSequence,
  IN      UINT16            PostTestFlag,
  IN      UINT8             LoopCount,
  IN      UINT8             NumBurst
);

/**
  This function is mainly used for MRC RTL testing as a check that the DDRIO init left RD/WR in 
  good shape. It can also be used as training on RTL hook for the same purpose, make sure certain
  training is not breaking RD/WR transactions. To be run pre/post training step as debug aid.

  @param[in, out]  MrcData            Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex  Starting point within the CapsuleData structure
                                      where the Init should begin.
  @param[in]       StringIndex        When provided, this is the index into the string
                                      table to show the register names.
  @retval          Channel            Run in this channel.
**/
MMRC_STATUS
CpgcBaseLineTest (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
);

UINT8
GetMaxElements(
  IN  OUT   MMRC_DATA     *MrcData
);

UINT8
GetMaxBits(
  IN  OUT   MMRC_DATA     *MrcData
);

MMRC_STATUS
MmrcWriteMsr (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT32            MsrAddress,
  IN      UINT32            MsrDataLow,
  IN      UINT32            MsrDataHigh
);

UINT64
MmrcReadMsr (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT32            MsrAddress
);

UINT16
DynamicStrobeGroupByteFlag(
  IN  MMRC_DATA       *MrcData,
  IN  UINT16          Index,
  IN  UINT8           Channel,
  IN  UINT8           Rank
);


MMRC_STATUS
Dynamicx4x8Support(
  IN  MMRC_DATA       *MrcData,
  IN  UINT16          Index,
  OUT UINT16          Max[MAX_CHANNELS][MAX_RANKS][MAX_KNOBS][MAX_STROBES][MAX_BITS]
);
#endif
