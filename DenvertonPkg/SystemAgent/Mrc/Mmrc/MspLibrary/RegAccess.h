/** @file
  RegAccess.h
  This file include register access.
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _REGACCESS_H_
#define _REGACCESS_H_

#ifdef MSP_DXE_EXECUTION
#else
  #include "DataTypes.h"
#endif
#include "MSysProgStructures.h"

#if SIM
  #include "vpi_user.h"
  #include "mrc_wrapper.h"
#endif
#if SUSSW
  #include "Susapi.h"
#endif

#if SIM
extern VOID   MySimMmio32WriteFast (INT32 PortId, INT32 Command, INT32 Offset, INT32 Data, INT32 be);
extern INT32  MySimMmio32ReadFast  (INT32 PortId, INT32 Offset, INT32 be);
extern VOID   MySimMmio32Write     (INT32 Offset, INT32 Data);
extern INT32  MySimMmio32Read      (INT32 be, INT32 Offset);
#endif

#if JTAG
extern INT32  JtagMem32Read(INT32 be, INT32 Offset, UINT32 Command);
extern VOID   JtagMem32Write(unsigned long opcode, unsigned long Offset, unsigned long Data, unsigned long be);
//extern VOID   JtagMem32Write       (INT32 Offset, INT32 Data);
#endif


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

#if SIM
#define MmioCacheFlush()                

#define Mmio32Read(Register)            0
#define Mmio32Write(Register, Value)    

#define Mmio16Read(Register)            0
#define Mmio16Write(Register, Value)    

#define Mmio8Read(Register)             0
#define Mmio8Write(Register, Value)     
#else
//
// Memory Mapped IO
//
UINT32
Mmio32Read (
  IN        UINTN      RegisterAddress
)
;

VOID
Mmio32Write (
  IN        UINTN      RegisterAddress,
  IN        UINT32      Value
)
;

UINT16
Mmio16Read (
  IN        UINTN      RegisterAddress
)
;

VOID
Mmio16Write (
  IN        UINTN      RegisterAddress,
  IN        UINT16      Value
)
;

UINT8
Mmio8Read (
  IN        UINTN      RegisterAddress
)
;

VOID
Mmio8Write (
  IN        UINTN      RegisterAddress,
  IN        UINT8       Value
)
;

#endif


#ifndef Mmio32Or
#define Mmio32Or(Register, OrData)    Mmio32Write (Register, Mmio32Read (Register) | OrData)
#define Mmio16Or(Register, OrData)    Mmio16Write (Register, Mmio16Read (Register) | OrData)
#define Mmio8Or(Register, OrData)     Mmio8Write (Register, Mmio8Read (Register) | OrData)

#define Mmio32And(Register, AndData)  Mmio32Write (Register, Mmio32Read (Register) & (AndData))
#define Mmio16And(Register, AndData)  Mmio16Write (Register, Mmio16Read (Register) & (AndData))
#define Mmio8And(Register, AndData)   Mmio8Write (Register, Mmio8Read (Register) & (AndData))

#define Mmio32AndThenOr(Register, AndData, OrData)  Mmio32Write (Register, (((Mmio32Read (Register)& (AndData))) | OrData))
#define Mmio16AndThenOr(Register, AndData, OrData)  Mmio16Write (Register, (((Mmio16Read (Register)& (AndData))) | OrData))
#define Mmio8AndThenOr(Register, AndData, OrData)   Mmio8Write (Register, (((Mmio8Read (Register)& (AndData))) | OrData))
#endif

//
// Memory mapped PCI IO
//
#define PCI_CFG_PTR(Bus, Device, Function, Register )\
    ((UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register))

#define PCI_CFG_32B_READ_CF8CFC(B,D,F,R)\
  (UINT32)(IoOut32 (0xCF8,(0x80000000|(B<<16)|(D<<11)|(F<<8)|(R))),IoIn32 (0xCFC))

#define PCI_CFG_32B_WRITE_CF8CFC(B,D,F,R,Data) \
  (IoOut32 (0xCF8,(0x80000000|(B<<16)|(D<<11)|(F<<8)|(R))),IoOut32 (0xCFC,Data))

#define PCI_CFG_32B_AND_THEN_OR_CF8CFC(B,D,F,R,A,O) \
  PCI_CFG_32B_WRITE_CF8CFC (B,D,F,R, \
    (PCI_CFG_32B_READ_CF8CFC (B,D,F,R) & (A)) | (O))

#define PCI_CFG_16B_READ_CF8CFC(B,D,F,R) \
  (UINT16)(IoOut32 (0xCF8,(0x80000000|(B<<16)|(D<<11)|(F<<8)|(R))),IoIn16 (0xCFC))

#define PCI_CFG_16B_WRITE_CF8CFC(B,D,F,R,Data) \
  (IoOut32 (0xCF8,(0x80000000|(B<<16)|(D<<11)|(F<<8)|(R))),IoOut16 (0xCFC,Data))

#define PCI_CFG_16B_AND_THEN_OR_CF8CFC(B,D,F,R,A,O) \
  PCI_CFG_16B_WRITE_CF8CFC (B,D,F,R, \
    (PCI_CFG_16B_READ_CF8CFC (B,D,F,R) & (A)) | (O))

#define PCI_CFG_8B_READ_CF8CFC(B,D,F,R) \
  (UINT8)(IoOut32 (0xCF8,(0x80000000|(B<<16)|(D<<11)|(F<<8)|(R))),IoIn8 (0xCFC))

#define PCI_CFG_8B_WRITE_CF8CFC(B,D,F,R,Data) \
  (IoOut32 (0xCF8,(0x80000000|(B<<16)|(D<<11)|(F<<8)|(R))),IoOut8 (0xCFC,Data))

#define PCI_CFG_8B_AND_THEN_OR_CF8CFC(B,D,F,R,A,O) \
  PCI_CFG_8B_WRITE_CF8CFC (B,D,F,R, \
    (PCI_CFG_8B_READ_CF8CFC (B,D,F,R) & (A)) | (O))

#if JTAG
#define PCI_CFG_32B_READ(PciExpressBase, Bus, Device, Function, Register, Command) \
  Mmio32Read (PciExpressBase + \
  (UINT32)(Bus << 20) + \
  (UINT32)(Device << 15) + \
  (UINT32)(Function << 12) + \
  (UINT32)(Register), \
  Command)
#else
#define PCI_CFG_32B_READ(PciExpressBase, Bus, Device, Function, Register) \
  Mmio32Read (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register))
#endif

#if JTAG
#define PCI_CFG_32B_WRITE(PciExpressBase, Bus, Device, Function, Register, Value, Command) \
  Mmio32Write (PciExpressBase + \
  (UINT32)(Bus << 20) + \
  (UINT32)(Device << 15) + \
  (UINT32)(Function << 12) + \
  (UINT32)(Register), \
  Value, \
  Command)
#else
#define PCI_CFG_32B_WRITE(PciExpressBase, Bus, Device, Function, Register, Value) \
  Mmio32Write (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register), \
    Value)
#endif

#define PCI_CFG_16B_READ(PciExpressBase, Bus, Device, Function, Register) \
  Mmio16Read (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register))

#define PCI_CFG_16B_WRITE(PciExpressBase, Bus, Device, Function, Register, Value) \
  Mmio16Write (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register), \
    Value)

#define PCI_CFG_8B_READ(PciExpressBase, Bus, Device, Function, Register) \
  Mmio8Read (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register))

#define PCI_CFG_8B_WRITE(PciExpressBase, Bus, Device, Function, Register, Value) \
  Mmio8Write (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register), \
    Value)

#define PCI_CFG_32B_OR(PciExpressBase, Bus, Device, Function, Register, OrValue) \
  Mmio32Write (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register), \
    (PCI_CFG_32B_READ (PciExpressBase, Bus, Device, Function, Register)|OrValue))

#define PCI_CFG_32B_AND(PciExpressBase, Bus, Device, Function, Register, AndValue) \
  Mmio32Write (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register), \
    (PCI_CFG_32B_READ (PciExpressBase, Bus, Device, Function, Register)& (AndValue)))

#define PCI_CFG_16B_OR(PciExpressBase, Bus, Device, Function, Register, OrValue) \
  Mmio16Write (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register), \
    (PCI_CFG_16B_READ(PciExpressBase, Bus, Device, Function, Register)|OrValue))

#define PCI_CFG_16B_AND(PciExpressBase, Bus, Device, Function, Register, AndValue) \
  Mmio16Write (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register), \
    (PCI_CFG_16B_READ (PciExpressBase, Bus, Device, Function, Register)& (AndValue)))

#define PCI_CFG_8B_OR(PciExpressBase, Bus, Device, Function, Register, OrValue) \
  Mmio8Write (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register), \
    (PCI_CFG_8B_READ (PciExpressBase, Bus, Device, Function, Register)|OrValue))

#define PCI_CFG_8B_AND(PciExpressBase, Bus, Device, Function, Register, AndValue) \
  Mmio8Write (PciExpressBase + \
    (UINT32)(Bus << 20) + \
    (UINT32)(Device << 15) + \
    (UINT32)(Function << 12) + \
    (UINT32)(Register), \
    (PCI_CFG_8B_READ (PciExpressBase, Bus, Device, Function, Register)& (AndValue)))


//
// Read Message Register
//
#define MSG_BUS_32B_READ(portid,offset,data)\
{\
  Mmio32Write (EC_BASE + 0xD8, 0); \
  Mmio32Write (EC_BASE + 0xD0, ((0x06000000) | (portid <<16)| ((offset) << 8) + 0xF0))   ; \
  (data) = Mmio32Read (EC_BASE + 0xD4);\
}

//
// Write Message Register
//
#define MSG_BUS_32B_WRITE(portid,offset,data)\
{\
  Mmio32Write (EC_BASE + 0xD8, 0); \
  Mmio32Write (EC_BASE + 0xD4, data)   ; \
  Mmio32Write (EC_BASE + 0xD0, ((0x07000000) | (portid <<16)| ((offset) << 8) + 0xF0)); \
}

UINT32
GetRegisterAccessInfo (
  IN        UINT8       BoxType,
  IN        UINT8       Channel,
  IN        UINT8       Instance,
  IN  OUT   UINT32      *Command,
  IN  OUT   UINT8       *PortId,
  IN  OUT   UINT32      *Offset,
  IN  OUT   UINT8       *Bus,
  IN  OUT   UINT8       *Device,
  IN  OUT   UINT8       *Func,
  IN        UINT32      Mode
);

UINTX
MemRegRead (
  IN        UINT8           BoxType,
  IN        UINT8           Channel,
  IN        UINT8           Instance,
  IN        REGISTER_ACCESS Register
)
/*++

Routine Description:

  Reads registers from a specified Unit

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
  IN        UINTX           Data,
  IN        UINT8           Be
)
/*++

Routine Description:

  Writes registers to a specified Unit

Arguments:

  Boxtype:          Unit to select
  Instance:         Channel under test
  Offset:           Offset of register to write.
  Data:             Data to be written

Returns:

  None

--*/
;
UINTX
MemFieldRead (
              IN        UINT8           BoxType,
              IN        UINT8           Channel,
              IN        UINT8           Instance,
              IN        REGISTER_ACCESS Register
)
;

VOID
MemFieldWrite (
               IN        UINT8           BoxType,
               IN        UINT8           Channel,
               IN        UINT8           Instance,
               IN        REGISTER_ACCESS Register,
               IN        UINTX           Value,
               IN        UINT8           Be
)
;
//
// IO
//
#if defined SIM || defined JTAG
#define IoIn8(Port)         MySimIoRead (0x1, Port)
#define IoIn16(Port)        MySimIoRead (0x3, Port)
#define IoIn32(Port)        MySimIoRead (0xf, Port)
#define IoOut8(Port, Data)  MySimIoWrite (0x1, Port, Data)
#define IoOut16(Port, Data) MySimIoWrite (0x3, Port, Data)
#define IoOut32(Port, Data) MySimIoWrite (0xf, Port, Data)
#else
#pragma intrinsic(_inp, _outp, _inpw, _inpd, _outpd)
#ifndef IoIn8
#define IoIn8(Port)         _inp (Port)
#define IoIn16(Port)        _inpw (Port)

#define IoIn32(Port)        _inpd (Port)

#define IoOut8(Port, Data)  _outp (Port, Data)

#define IoOut16(Port, Data) _outpw (Port, Data)

#define IoOut32(Port, Data) _outpd (Port, Data)
#endif // IoIn8
#endif // SIM

#endif // _REGACCESS_H

