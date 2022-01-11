/** @file
  MMRC.h
  The external header file that all projects must include in order to port the MMRC.
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
 
**/

#ifndef _MMRC_H_
#define _MMRC_H_

#include "MmrcData.h"
#include "Printf.h"
#include "RegAccess.h"
#include "MmrcHooks.h"

#ifndef _H2INC
#ifndef __GNUC__
#if !defined(SUSSW) || defined (CAR)
//
// C intrinsic function definitions
//
#define size_t UINT32
void * __cdecl memcpy (void * dst, void *src, size_t cnt);
#endif
#endif
#endif

extern MMRC_STATUS MmrcExecuteTask (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex,UINT8 Channel);
extern MMRC_STATUS ReceiveEnable (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex, UINT8 Channel);
extern MMRC_STATUS FineWriteLeveling (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex, UINT8 Channel);
extern MMRC_STATUS CoarseWriteLeveling (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex, UINT8 Channel);
extern MMRC_STATUS ReadTraining (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex, UINT8 Channel);
extern MMRC_STATUS PhyViewTable (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex, UINT8 Channel);
extern MMRC_STATUS WriteTraining (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex, UINT8 Channel);
extern MMRC_STATUS CommandClockTraining (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex,UINT8 Channel);
extern MMRC_STATUS ScrubMemory (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex, UINT8 Channel);
extern MMRC_STATUS MemoryTest (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex, UINT8 Channel);
extern void      *MmrcMemset (void *dst, char value, UINT32 cnt);
extern VOID      *MmrcMemCopy (void *dst, void *src,UINT32 cnt);
extern BOOLEAN    MmrcStringCompare (VOID *str1, VOID *str2, UINT32 cnt);
extern VOID       SaveMrcHostStructureAddress (MMRC_DATA *MrcDataAddress);
extern MMRC_DATA *GetMrcHostStructureAddress ();
extern VOID       MrcDeadLoop ();
extern VOID       GetMaxDq (MMRC_DATA *MrcData);

extern STROBE_TO_ERROR_MASK                   StrobeToErrorMask[2][MAX_STROBES];
extern TASK_DESCRIPTOR                        InitTasks[];
extern EXTERNAL_GETSET_FUNCTION               ExternalFunctions[];
extern UINT16                                 GsmGtToMmrcDefine[];
extern UINT16                                 GsmCsnToMmrcDefine[];
extern CONST PFCT_AND_IO_LEVEL_TO_BLUEPRINT   PfctAndIoLevelToBlueprint[MAX_IO_LEVELS + 1];

#if (!defined(MAX_GETSETS) || (defined(MAX_GETSETS) && MAX_GETSETS == MAX_BLUEPRINTS && MAX_BLUEPRINTS > 1))
  extern       UINT16                     HalfClkGenerated[MAX_BLUEPRINTS][MAX_DLL_COUNT][NUM_FREQ];
  extern       UINT8                      ADdllGenerated[MAX_BLUEPRINTS][2];
  extern CONST INDEX_TO_OFFSET_LAYER      DelayIndexToOffsetLayerGenerated[MAX_BLUEPRINTS][MAX_NUM_ALGOS];
  extern CONST INDEX_TO_OFFSET_LAYER      GroupIndexToOffsetLayerGenerated[MAX_BLUEPRINTS][MAX_NUM_GROUP];
  extern CONST INDEX_TO_OFFSET_LAYER      UniqueIndexToOffsetLayerGenerated[MAX_BLUEPRINTS][MAX_NUM_UNIQUE];

  extern CONST GET_SET_ELEMENTS           ElementsGenerated[MAX_BLUEPRINTS][MAX_NUM_ELEMENTS];
  extern CONST UINT8                      NumberAlgosGenerated[MAX_BLUEPRINTS];
  extern CONST UINT8                      NumberOfDependantElementsGenerated[MAX_BLUEPRINTS];
  extern CONST UINT8                      NumberOfDelayElementsPerAlgoGenerated[MAX_BLUEPRINTS];
  extern CONST UINT8                      NumberOfCCElementsGenerated[MAX_BLUEPRINTS];
  extern CONST UINT8                      NumberOfCCRangesGenerated[MAX_BLUEPRINTS];
  extern CONST UINT8                      NumberOfElementsPerAlgoGenerated[MAX_BLUEPRINTS];
  extern CONST UINT8                      NumberOfFrequenciesGenerated[MAX_BLUEPRINTS];
  extern       SIGNAL_INFO                SignalInfoGenerated[MAX_BLUEPRINTS][MAX_SIGNAL_INFO_ELEMENTS];
  extern       UINT16                     GranularityGenerated[MAX_BLUEPRINTS][MAX_DLL_COUNT][NUM_FREQ][MAX_NUM_DELAY_ELEMENTS];
  extern CLOCK_CROSSINGS                  ClockCrossingsGenerated[MAX_BLUEPRINTS][MAX_NUM_CC_RANGES];

  #define HalfClk                         HalfClkGenerated [MrcData->MspData.CurrentBlueprint]
  #define ADdll                           ADdllGenerated [MrcData->MspData.CurrentBlueprint]
  #define DelayIndexToOffsetLayer         DelayIndexToOffsetLayerGenerated [MrcData->MspData.CurrentBlueprint]
  #define GroupIndexToOffsetLayer         GroupIndexToOffsetLayerGenerated [MrcData->MspData.CurrentBlueprint]
  #define UniqueIndexToOffsetLayer        UniqueIndexToOffsetLayerGenerated [MrcData->MspData.CurrentBlueprint]
  #define Elements                        ElementsGenerated [MrcData->MspData.CurrentBlueprint]
  #define NumberAlgos                     NumberAlgosGenerated [MrcData->MspData.CurrentBlueprint]
  #define NumberOfDependantElements       NumberOfDependantElementsGenerated [MrcData->MspData.CurrentBlueprint]
  #define NumberOfDelayElementsPerAlgo    NumberOfDelayElementsPerAlgoGenerated [MrcData->MspData.CurrentBlueprint]
  #define NumberOfCCElements              NumberOfCCElementsGenerated [MrcData->MspData.CurrentBlueprint]
  #define NumberOfCCRanges                NumberOfCCRangesGenerated [MrcData->MspData.CurrentBlueprint]
  #define NumberOfElementsPerAlgo         NumberOfElementsPerAlgoGenerated [MrcData->MspData.CurrentBlueprint]
  #define NumberOfFrequencies             NumberOfFrequenciesGenerated [MrcData->MspData.CurrentBlueprint]
  #define SignalInfo                      SignalInfoGenerated [MrcData->MspData.CurrentBlueprint]
  #define Granularity                     GranularityGenerated [MrcData->MspData.CurrentBlueprint]
  #define ClockCrossings                  ClockCrossingsGenerated [MrcData->MspData.CurrentBlueprint]
#else
  extern       UINT16                     HalfClkGenerated[MAX_DLL_COUNT][NUM_FREQ];
  extern       UINT8                      ADdllGenerated[2];
  extern CONST INDEX_TO_OFFSET_LAYER      DelayIndexToOffsetLayerGenerated[MAX_NUM_ALGOS];
  extern CONST INDEX_TO_OFFSET_LAYER      GroupIndexToOffsetLayerGenerated[MAX_NUM_GROUP];
  extern CONST INDEX_TO_OFFSET_LAYER      UniqueIndexToOffsetLayerGenerated[MAX_NUM_UNIQUE];

  extern CONST GET_SET_ELEMENTS           ElementsGenerated[MAX_NUM_ELEMENTS];
  extern CONST UINT8                      NumberAlgosGenerated;
  extern CONST UINT8                      NumberOfDependantElementsGenerated;
  extern CONST UINT8                      NumberOfDelayElementsPerAlgoGenerated;
  extern CONST UINT8                      NumberOfCCElementsGenerated;
  extern CONST UINT8                      NumberOfCCRangesGenerated;
  extern CONST UINT8                      NumberOfElementsPerAlgoGenerated;
  extern CONST UINT8                      NumberOfFrequenciesGenerated;
  extern       SIGNAL_INFO                SignalInfoGenerated[MAX_SIGNAL_INFO_ELEMENTS];
  extern       UINT16                     GranularityGenerated[MAX_DLL_COUNT][NUM_FREQ][MAX_NUM_DELAY_ELEMENTS];
  extern CLOCK_CROSSINGS                  ClockCrossingsGenerated[MAX_NUM_CC_RANGES];

  #define HalfClk                         HalfClkGenerated 
  #define ADdll                           ADdllGenerated 
  #define DelayIndexToOffsetLayer         DelayIndexToOffsetLayerGenerated 
  #define GroupIndexToOffsetLayer         GroupIndexToOffsetLayerGenerated 
  #define UniqueIndexToOffsetLayer        UniqueIndexToOffsetLayerGenerated 
  #define Elements                        ElementsGenerated 
  #define NumberAlgos                     NumberAlgosGenerated
  #define NumberOfDependantElements       NumberOfDependantElementsGenerated
  #define NumberOfDelayElementsPerAlgo    NumberOfDelayElementsPerAlgoGenerated
  #define NumberOfCCElements              NumberOfCCElementsGenerated
  #define NumberOfCCRanges                NumberOfCCRangesGenerated
  #define NumberOfElementsPerAlgo         NumberOfElementsPerAlgoGenerated
  #define NumberOfFrequencies             NumberOfFrequenciesGenerated
  #define SignalInfo                      SignalInfoGenerated 
  #define Granularity                     GranularityGenerated 
  #define ClockCrossings                  ClockCrossingsGenerated 
#endif


#if MAX_BLUEPRINTS > 1
  extern INSTANCE_PORT_MAP                InstancePortMapGenerated[MAX_BLUEPRINTS][MAX_BOXES];
  extern INSTANCE_PORT_OFFSET             InstancePortOffsetGenerated[MAX_BLUEPRINTS][MAX_INSTANCE_PORTS];
  extern UINT8                            ChannelToInstanceMapGenerated[MAX_BLUEPRINTS][MAX_BOXES][MAX_CHANNELS];
  extern CONST FLOORPLAN                  FloorPlanGenerated[MAX_BLUEPRINTS][MAX_CHANNELS][MAX_STROBES];
  extern CONST INT16                      StrobeToRegisterGenerated[MAX_BLUEPRINTS][MAX_STROBES][2];
  extern CONST INT16                      RankToRegisterGenerated[MAX_BLUEPRINTS][MAX_RANKS][2];
  extern CONST UINT8                      UnitStringsGenerated[MAX_BLUEPRINTS][MAX_BOXES][MAX_BOXNAME_LENGTH];

  #define InstancePortMap                 InstancePortMapGenerated [MspData->CurrentBlueprint]
  #define InstancePortOffset              InstancePortOffsetGenerated [MspData->CurrentBlueprint]
  #define ChannelToInstanceMap            ChannelToInstanceMapGenerated [MspData->CurrentBlueprint]
  #define FloorPlan                       FloorPlanGenerated [MrcData->MspData.CurrentBlueprint]
  #define StrobeToRegister                StrobeToRegisterGenerated [MrcData->MspData.CurrentBlueprint]
  #define RankToRegister                  RankToRegisterGenerated [MrcData->MspData.CurrentBlueprint]
  #define UnitStrings                     UnitStringsGenerated [MrcData->MspData.CurrentBlueprint]
#else
  extern INSTANCE_PORT_MAP                InstancePortMapGenerated[MAX_BOXES];
  extern INSTANCE_PORT_OFFSET             InstancePortOffsetGenerated[MAX_INSTANCE_PORTS];
  extern UINT8                            ChannelToInstanceMapGenerated[MAX_BOXES][MAX_CHANNELS];
  extern CONST FLOORPLAN                  FloorPlanGenerated[MAX_CHANNELS][MAX_STROBES];
  extern CONST INT16                      StrobeToRegisterGenerated[MAX_STROBES][2];
  extern CONST INT16                      RankToRegisterGenerated[MAX_RANKS][2];
  extern CONST UINT8                      UnitStringsGenerated[MAX_BOXES][MAX_BOXNAME_LENGTH];

  #define InstancePortMap                 InstancePortMapGenerated 
  #define InstancePortOffset              InstancePortOffsetGenerated 
  #define ChannelToInstanceMap            ChannelToInstanceMapGenerated
  #define FloorPlan                       FloorPlanGenerated
  #define StrobeToRegister                StrobeToRegisterGenerated
  #define RankToRegister                  RankToRegisterGenerated
  #define UnitStrings                     UnitStringsGenerated
#endif

extern       Register                 Registers[];
extern       UINT16                   EarlyCommandPatternsRise[];
extern       UINT16                   EarlyCommandPatternsFall[];
extern       UINT32                   EarlyCommandResults41[];
extern       UINT32                   EarlyCommandResults48[];
extern       UINT16                   MrsCommandIndex[MAX_RANKS][MR_COUNT];
extern       char                    *OutputStrings[];
extern       UINT8                    NumberEyeMaskRead;
extern       EYEMASK_T                EyeMaskRead[];
extern       UINT8                    NumberControlKnobsRead;
extern       UINT8                    NumberControlKnobsCtle;
extern       UINT8                    NumberControlKnobsReadPN;
extern       CONTROLKNOBS             ControlKnobsRead[];
extern       CONTROLKNOBS             ControlKnobsReadPN[];
extern       CONTROLKNOBS             ControlKnobsReadCtle[];
extern       UINT8                    NumberEyeMaskWrite;
extern       EYEMASK_T                EyeMaskWrite[];
extern       UINT8                    NumberControlKnobsWrite;
extern       CONTROLKNOBS             ControlKnobsWrite[];
extern       CONTROLKNOBS             ControlKnobsWriteDdr3[];
extern       EYEMASK_T                EyeMaskCmd[];
extern       UINT8                    NumberEyeMaskCmd;
extern       UINT8                    NumberControlKnobsCmd;
extern       CONTROLKNOBS             ControlKnobsCmd[];

#endif
