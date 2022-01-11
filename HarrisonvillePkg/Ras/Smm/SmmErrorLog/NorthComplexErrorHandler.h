/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  NorthFabricErrorHandler.h

Abstract:


------------------------------------------------------------------*/

#ifndef _NORTH_FABRIC_ERROR_HANDLER_H
#define _NORTH_FABRIC_ERROR_HANDLER_H

#define EC_BASE                  0xE0000000

#define MAX_BOXTYPE_INSTANCES    2
#define NA                       0xFF
//
// All boxtypes supported by the MSP. Boxtypes are distinct components of silicon.
//
#define DUNIT_COMMON                                       0x00
#define DUNIT                                              0x01
#define DDRDQ                                              0x02
#define DDRDQ_PHY                                          0x03
#define DDRCC2_PHY                                         0x04
#define DDRCC1_PHY                                         0x05
#define DDRPLL                                             0x06
#define DDRCC0                                             0x07
#define DDRCC1                                             0x08
#define DDRCC2                                             0x09
#define DDRSAI                                             0x0A
#define BUNIT                                              0x0B
#define RTF                                                0x0C
#define CPGC                                               0x0D
#define BUNIT_MCHBAR                                       0x0E
#define AUNIT_MCHBAR                                       0x0F
#define BUNITMEM                                           0x10
#define PUNITSA                                            0x11
#define PMC_PCI_MMR                                        0x12
#define PMC_PCI_CFG                                        0x13
#define CLTT_CCR                                           0x14
#define SUNIT                                              0x15
#define AUNIT_MSG_MAP_1                                    0x16
#define BUNIT_MSG_MAP_0                                    0x17
#define MAX_BOXES                                          0x18
#define MAX_INSTANCE_PORTS                                 58


//
// Register access methods (sideband, PCI, etc)
//
#define eSB                                                0
#define ePCI                                               1
#define eBAR                                               2
#define eACCESS_METHOD_ERROR                               3

typedef struct {
  UINT8  TotalInstances;
  UINT8  Instances;
  UINT8  InstanceStartIndex;
  UINT8  ReadOperation;
  UINT8  WriteOperation;
  UINT8  AccessMethod;
  UINT32  AccessBase;
  UINT8  SimulationSupported;
  UINT32 StringIndex;
} INSTANCE_PORT_MAP;

// Denverton Aptiov override start - EIP#281374
#ifndef MCH_BASE_ADDRESS
#define MCH_BASE_ADDRESS  0xfed10000
#endif
// Denverton Aptiov override end - EIP#281374
#define P2SB_BASE_ADDRESS 0xFD000000
#define PMC_BASE_ADDRESS  0xFE000000


typedef struct {
  UINT16 Port;
  UINT16 Offset;
} INSTANCE_PORT_OFFSET;

typedef struct {
  UINT32  Offset;           // Register Offset relative to the base address.
  UINT64  Mask;             // Starting bit within the register.
  UINT8   ShiftBit;         // Number of bits to shift to get to the register field in question
} REGISTER_ACCESS;

typedef enum {
  NoError,
  InvalidInstance,
  InvalidBoxType,
  InvalidMode,
  BoxTypeNotInSimulation,
  InvalidRemapType,
  UnknownError
} ACCESS_ERROR_TYPE;

typedef enum {
  ModeRead,
  ModeWrite,
  ModeOther
} ACCESS_MODE;

#define  R_MCI_CTL                       0x20
#define    B_MC_RD_DATA_COR              BIT10
#define    B_MC_RD_DATA_UNC              BIT9
#define    B_BRAM_RD_PAR                 BIT8
#define    B_BRAM_WR_PAR                 BIT7
#define    B_XUCODE_ERR                  BIT6
#define    B_IA_HIT_GSM                  BIT5
#define    B_MMIO_CLFLUSH                BIT4
#define    B_MMIO_WBMTOIE                BIT3
#define    B_MMIO_HITM                   BIT2
#define    B_PII_2_MMIO                  BIT1
#define    B_UNS_IDI_OP                  BIT0
#define  R_MCI_STATUS                    0x28
#define    B_MCI_STATUS_VALID            BIT63
#define    B_ERR_OVERFLOW                BIT62
#define    B_UNCORRECTED_ERR             BIT61
#define    B_ERR_ENABLED                 BIT60
#define    B_MCI_STATUS_MISCV            BIT59
#define    B_MCI_STATUS_ADDRV            BIT58
#define    B_PROCESSOR_CONTEXT_CORRUPTED BIT57
#define    B_CORR_ERR_CNT_OVERFLOW       BIT52
#define    B_BRAM_ERR                    BIT37
#define    R_MCI_CTL2                    0x40
#define    B_CORR_ERR_INT_ENABLE         BIT30

#define	   MSR_B_CR_MCI_CTL				 0x410
#define	   MSR_B_CR_MCI_STS				 0x411
#define	   MSR_B_CR_MCI_CTL2			 0x284

VOID
ClearNorthComplexErrors(
  );

BOOLEAN
ElogNorthComplexErrorHandler 
(
  );

VOID
ProgramNorthComplexErrorSignals(
	);

UINT64
MemRegRead (
  IN        UINT8           BoxType,
  IN        UINT8           Channel,
  IN        UINT8           Instance,
  IN        REGISTER_ACCESS Offset
)
/*++

Routine Description:

  Reads registers from an specified Unit

Arguments:

  Boxtype:          Unit to select
  Instance:         Channel under test
  Offset:           Offset of register to read.

Returns:

  Value read

--*/
;

VOID
MemRegWrite (
  IN        UINT8           BoxType,
  IN        UINT8           Channel,
  IN        UINT8           Instance,
  IN        REGISTER_ACCESS Register,
  IN        UINT64          Data,
  IN        UINT8           Be 
)
/*++

Routine Description:

  Write registers from an specified Unit

Arguments:

  Boxtype:         Unit to select
  Channel:         Channel under test
  Instance:        Box instance   
  Register:        Offset of register to Write.
  Data:            Data to be written
  Be:              Byte enables

Returns:

  None

--*/
;

#endif

