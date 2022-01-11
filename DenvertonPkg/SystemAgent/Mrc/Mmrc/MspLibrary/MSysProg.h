/** @file
  MSysProg.h
  Library specific macros and function declarations used within 
  the MSP. 
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MSYSPROG_H_
#define _MSYSPROG_H_


#ifdef MSP_DXE_EXECUTION
  #include "MspPrintf.h"
#else
  #include "DataTypes.h"
#endif
#include "MSysProgStructures.h"


typedef VOID (*Callback) (MSP_DATA *MspData, VOID *InputStructure);

#ifdef MSP_DXE_EXECUTION
  extern MSP_DATA *MyMspData;
  #define TASK_FUNCTION_DESC_DONE 0, 0, ((MSP_STATUS(*)(MSP_DATA *, UINT8, UINT16, UINT16, UINT8)) 0), 0, 0, 0, 0, 0, 0, 0
#endif

MSP_STATUS 
MspDelay (
  IN        UINT8   Type,
  IN        UINT32  Delay
);
BOOLEAN
MmrcStringCompare (
  IN    VOID    *str1,
  IN    VOID    *str2,
  IN    UINT32  cnt
  )
;
MSP_STATUS
MspInit (
  IN      MSP_DATA         *MspData,
  IN      VOID             *InputStructure,
  IN      Callback          FunctionPtr
  );

MSP_STATUS
MspExecuteTask (
  IN      MSP_DATA         *MspData,
  IN      UINT8             Socket,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );

VOID
FillUpMspDataStructure (
  IN  OUT     MSP_DATA      *MspData,
  IN          UINT8         Channel,
  IN          BOOLEAN       Enabled,
  IN          UINT16        CurrentPlatform,
  IN          UINT16        CurrentFrequency,
  IN          UINT16        CurrentTechnology,
  IN          UINT8         DebugMsgLevel,
  IN          BOOLEAN       ExecuteThisRoutineInParallel,
  IN          UINT8         CurrentBlueprint,
  IN          UINT32        EcBase
);

VOID
LockMspDataStructure ();

MSP_DATA *
GetMspHostStructureAddress ();

#ifndef SIM
UINT32
Mmio32Read (
  IN        UINTN      RegisterAddress
);

VOID
Mmio32Write (
  IN        UINTN      RegisterAddress,
  IN        UINT32      Value
);
#endif

UINTX
ByteEnableToUintX (
  IN       UINT8    Be,
  IN       UINTX    Value,
  IN       UINT8    *ValueLength
  );

MSP_STATUS
DynamicAssignmentGenerated (
  IN  OUT   UINTX         DynamicVars[MAX_CHANNELS][DYNVAR_MAX],
  IN        UINT8         Channel,
  IN        UINT16        Index,
  IN        UINTX        *Value
  );

VOID
SetMspData (  
  IN      MSP_DATA         *MspData,
  IN      VOID             *InputStructure
  );

/**
  Enables the High Precision Event Timer

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Channel under examination
  @retval          Status
**/
MSP_STATUS
McEnableHpet (
  IN  OUT   MSP_DATA   *MspData
  );


extern  CONST UINT8                   InitData[];
extern  char                          *CapsuleStrings[];
extern  CONST SUBTASK_INDEX_OFFSET    SubtaskOffsetIndex[MAX_SUBTASKS + 1];
extern  char                          *OutputStrings[];

#if MAX_BLUEPRINTS > 1
  extern INSTANCE_PORT_MAP            InstancePortMapGenerated[MAX_BLUEPRINTS][MAX_BOXES];
  extern INSTANCE_PORT_OFFSET         InstancePortOffsetGenerated[MAX_BLUEPRINTS][MAX_INSTANCE_PORTS];
  extern UINT8                        ChannelToInstanceMapGenerated[MAX_BLUEPRINTS][MAX_BOXES][MAX_CHANNELS];
#ifdef MSP_DXE_EXECUTION
#else
  extern CONST FLOORPLAN              FloorPlanGenerated[MAX_BLUEPRINTS][MAX_CHANNELS][MAX_STROBES];
  extern CONST INT16                  StrobeToRegisterGenerated[MAX_BLUEPRINTS][MAX_STROBES][2];
  extern CONST INT16                  RankToRegisterGenerated[MAX_BLUEPRINTS][MAX_RANKS][2];
#endif // defined (MSP_DXE_EXECUTION) && (MSP_DXE_EXECUTION==1)
  extern CONST UINT8                  UnitStringsGenerated[MAX_BLUEPRINTS][MAX_BOXES][MAX_BOXNAME_LENGTH];

  #define MspInstancePortMap          InstancePortMapGenerated [MspData->CurrentBlueprint]
  #define MspInstancePortOffset       InstancePortOffsetGenerated [MspData->CurrentBlueprint]
  #define MspChannelToInstanceMap     ChannelToInstanceMapGenerated [MspData->CurrentBlueprint]
  #define MspFloorPlan                FloorPlanGenerated [MspData->CurrentBlueprint]
  #define MspStrobeToRegister         StrobeToRegisterGenerated [MspData->CurrentBlueprint]
  #define MspRankToRegister           RankToRegisterGenerated [MspData->CurrentBlueprint]
  #define MspUnitStrings              UnitStringsGenerated [MspData->CurrentBlueprint]
#else
  extern INSTANCE_PORT_MAP            InstancePortMapGenerated[MAX_BOXES];
  extern INSTANCE_PORT_OFFSET         InstancePortOffsetGenerated[MAX_INSTANCE_PORTS];
  extern UINT8                        ChannelToInstanceMapGenerated[MAX_BOXES][MAX_CHANNELS];
#ifdef MSP_DXE_EXECUTION
#else
  extern CONST FLOORPLAN              FloorPlanGenerated[MAX_CHANNELS][MAX_STROBES];
  extern CONST INT16                  StrobeToRegisterGenerated[MAX_STROBES][2];
  extern CONST INT16                  RankToRegisterGenerated[MAX_RANKS][2];
#endif // MSP_DXE_EXECUTION
  extern CONST UINT8                  UnitStringsGenerated[MAX_BOXES][MAX_BOXNAME_LENGTH];

  #define MspInstancePortMap          InstancePortMapGenerated 
  #define MspInstancePortOffset       InstancePortOffsetGenerated
  #define MspChannelToInstanceMap     ChannelToInstanceMapGenerated
  #define MspFloorPlan                FloorPlanGenerated
  #define MspStrobeToRegister         StrobeToRegisterGenerated
  #define MspRankToRegister           RankToRegisterGenerated
  #define MspUnitStrings              UnitStringsGenerated
#endif // MAX_BLUEPRINTS > 1

#endif // _MSYSPROG_H_
