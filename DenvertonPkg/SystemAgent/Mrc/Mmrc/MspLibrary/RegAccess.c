/** @file
  RegAccess.h
  This file handles register accesses.
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "RegAccess.h"
#include "MSysProg.h"
#ifdef MSP_DXE_EXECUTION
  #include "MspPrintf.h"
#else
  #include "DataTypes.h"
  #include "Printf.h"
#endif


#if !defined SIM && !defined JTAG
//
// Memory Mapped IO
//
UINT32
Mmio32Read (
  IN        UINTN      RegisterAddress
)
{
  VOLATILE UINT32 *addr;
  addr = (UINT32 *) (RegisterAddress);

  return *addr;
}

UINT16
Mmio16Read (
  IN        UINTN      RegisterAddress
)
{
  VOLATILE UINT16 *addr = (UINT16 *) RegisterAddress;
  return *addr;
}

UINT8
Mmio8Read (
  IN        UINTN      RegisterAddress
)
{
  VOLATILE UINT8 *addr = (UINT8 *) RegisterAddress;
  return *addr;
}

VOID
Mmio32Write (
  IN        UINTN      RegisterAddress,
  IN        UINT32      Value
)
{
  VOLATILE UINT32 *addr;
  addr = (UINT32 *) (RegisterAddress);
  *addr = Value;
}

VOID
Mmio16Write (
  IN        UINTN      RegisterAddress,
  IN        UINT16      Value
)
{
  VOLATILE UINT16 *addr = (UINT16 *) RegisterAddress;
  *addr = Value;
}

VOID
Mmio8Write (
  IN        UINTN      RegisterAddress,
  IN        UINT8       Value
)
{
  VOLATILE UINT8 *addr = (UINT8 *) RegisterAddress;
  *addr = Value;
}
#elif defined JTAG
UINT32
  MySimIoRead (UINT8 Size, UINT32 Port) {
  return 1;
}

VOID
MySimIoWrite(UINT8 Size, UINT32 Port, UINT32 Data) {
}
 
VOID
Mmio32Write (
  IN       UINT32       Address,
  IN       UINT32       Value,
  IN       UINT32       Command
  ) 
{
	//return;
  isb32Write(0, Address, Command, Value, 0x0f); // IDC
}
UINT32
Mmio32Read (
  IN        UINT32      RegisterAddress,
  IN        UINT32      Command
)
{
  //VOLATILE UINT32 *addr = (UINT32 *) RegisterAddress;
  //return *addr;
  //return -1;
  //kirk

  return isb32Read(0, RegisterAddress, Command, 0xFE); // IDC: Added to support translation from PCI to iosfsb transactions in headless.

}
UINT8
Mmio8Read (
  IN        UINT32      RegisterAddress
)
{
  VOLATILE UINT8 *addr = (UINT8 *) RegisterAddress;
  return *addr;
}
VOID
Mmio8Write (
  IN        UINT32      RegisterAddress,
  IN        UINT8       Value
)
{
  VOLATILE UINT8 *addr = (UINT8 *) RegisterAddress;
  *addr = Value;
}
#endif

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
)
{
  MSP_DATA    *MspData;
  UINT32      Error;

  MspData = GetMspHostStructureAddress ();
  Error = NoError;

#if SIM
  if (!MspInstancePortMap[BoxType].SimulationSupported) {
    return BoxTypeNotInSimulation;
  }
#endif
  //
  // Provided into this function is the instance number within the channel, getRegAccess requires the total instance index.
  // This translates the channel to the instance number in the INSTANCE_PORT_OFFSET structure.
  //
  if (MspChannelToInstanceMap[BoxType][Channel] == 0xFF) {
    return InvalidInstance;
  } else {
    Instance += MspChannelToInstanceMap[BoxType][Channel];
  }
  //
  // If the Type is a PCI access, get the B/D/F.
  //
  if (MspInstancePortMap[BoxType].AccessMethod == ePCI) {
    *Bus    = (MspInstancePortOffset[MspInstancePortMap[BoxType].InstanceStartIndex + Instance].Port >> 8) & 0xff;
    *Device = (MspInstancePortOffset[MspInstancePortMap[BoxType].InstanceStartIndex + Instance].Port >> 3) & 0x1f;
    *Func   = MspInstancePortOffset[MspInstancePortMap[BoxType].InstanceStartIndex + Instance].Port & 0x07;
    *Offset = 0;
  } else if (MspInstancePortMap[BoxType].AccessMethod == eSB) {
    //
    // Look at the type, which is either a sideband port or an address offset for this box type.
    // If it's a port, assign it here. If it's an address offset, do the appropriate math to
    // target the Instance passed in.
    //
    *PortId = (UINT8) MspInstancePortOffset[MspInstancePortMap[BoxType].InstanceStartIndex + Instance].Port;
    *Offset = MspInstancePortOffset[MspInstancePortMap[BoxType].InstanceStartIndex + Instance].Offset;
  } else if (MspInstancePortMap[BoxType].AccessMethod == eBAR) {
    //
    // For BAR accesses, compile a full 32 bit offset from Port (upper 16 bits) and Offset (lower 16 bits).
    // We reuse the definition of Port for the upper 16 bits to save on code space in MspInstancePortOffset.
    //
    *Offset = (MspInstancePortOffset[MspInstancePortMap[BoxType].InstanceStartIndex + Instance].Port << 16) + MspInstancePortOffset[MspInstancePortMap[BoxType].InstanceStartIndex + Instance].Offset;
  }
  //
  // Get the sideband command to read/write this particular box type.
  // If the command is not read or write, do not attempt to change it.
  //
  if (Mode == ModeRead) {
  *Command = MspInstancePortMap[BoxType].ReadOperation  << 24;
  } else if (Mode == ModeWrite) {
  *Command = MspInstancePortMap[BoxType].WriteOperation << 24;
  } else {
    *Command = *Command << 24;
  }
  //
  // Handle all errors.
  //
  if (Error != NoError) {
    if (Mode == ModeRead) {
      MspDebugPrint ((MSP_DBG_MIN, "RegRead ERROR: "));
    } else if (Mode == ModeWrite) {
      MspDebugPrint ((MSP_DBG_MIN, "RegWrite ERROR: "));
    }
  }
  switch (Error) {
  case NoError:
    break;
  case InvalidInstance:
    MspDebugPrint ((MSP_DBG_MIN, "Invalid Instance 0x%x for Box 0x%d (offset = 0x%x)\n", Instance, BoxType, *Offset));
    break;
  default:
    break;
  }
  return Error;
}

UINTX
MemRegRead (
  IN        UINT8           BoxType,
  IN        UINT8           Channel,
  IN        UINT8           Instance,
  IN        REGISTER_ACCESS Register
)
/*++

Routine Description:

  Reads registers from an specified Unit

Arguments:

  Boxtype:          Unit to select
  Instance:         Channel under test
  Offset:           Offset of register to read.
  PciReg:           If this is 

Returns:

  Value read

--*/
{
  UINT32          Command;
  UINT8           PortId;
  UINT32          Data;
  UINT32          BaseOffset;
  UINT8           Bus;
  UINT8           Device;
  UINT8           Func;
  UINT32         *MemOffset;
  MSP_DATA       *MspData;
  UINT8           OffsetShift;
  UINT8           ShiftLeftAmount;
  UINT8           ShiftRightAmount;
  UINTX           TotalValue;
  REGISTER_ACCESS TempRegister;

  OffsetShift       = 0;
  ShiftLeftAmount   = 0;
  ShiftRightAmount  = 0;
  TotalValue        = 0;
  MspData           = GetMspHostStructureAddress();

  if (GetRegisterAccessInfo (BoxType, Channel, Instance, &Command, &PortId, &BaseOffset, &Bus, &Device, &Func, ModeRead) == NoError) {
    //
    // If we are dealing with a PCI or MEM type register, the offset could
    // not be DWORD aligned. In this case, we may need to do multiple reads to
    // piece together the entire register since reads to PCI or MEM should be 
    // DWORD aligned. 
    //
    if (MspInstancePortMap[BoxType].AccessMethod == ePCI || MspInstancePortMap[BoxType].AccessMethod == eBAR) {
      OffsetShift       = (Register.Offset % 4);
      ShiftLeftAmount   = OffsetShift * 8;
      ShiftRightAmount  = 32 - ShiftLeftAmount;
      //
      // Align the offset to the next DWORD down (i.e. offset 0x33 will be offset 0x30 after this).
      //
      Register.Offset  -= OffsetShift;
      //
      // If the offset needed to be shifted (ie. it's not DWORD aligned) or the
      // mask is more than 32 bits, we need to recurse to read another DWORD.
      //
      if (OffsetShift != 0 || Register.Mask > 0xFFFFFFFF) {
        //
        // Create a temporary register mask in case we need to recurse with a new mask.
        //
        TempRegister.Mask     = Register.Mask >> ShiftRightAmount;
        TempRegister.Offset   = Register.Offset + 4;
        if (TempRegister.Mask != 0) {
          TotalValue = MemRegRead (BoxType, Channel, Instance, TempRegister);
        }
      }
      //
      // We've completed recursion, building up all upper DWORDs of the register. Now we
      // need to read the final DWORD. Since we've subtracted OffsetShift from the original
      // offset, we need to align the mask with the new DWORD aligned offset.
      //
      Register.Mask <<= ShiftLeftAmount;
    }
    // 
    // If the action is to be taking place and the transaction is a PCI, then just do the PCI Access. 
    // 
    if (MspInstancePortMap[BoxType].AccessMethod == ePCI) {
      //
      // PCI access
      //
#if defined SIM || defined SIM_BUILD || defined KTI_SW_SIMULATION
      Data = 0;
//
// EP/EX segment simulation hooks.
//
/*      Data = CpuSimRegAccess (MrcData, MrcData->var.mem.currentSocket, 
        (UINT32)(MrcData->var.common.mmCfgBase + 
        (UINT32)(Bus << 20) + 
        (UINT32)(Device << 15) + 
        (UINT32)(Func << 12) + 
                 Register.Offset + BaseOffset), 4, 0, READ_ACCESS);*/
#else
#if JTAG
      Data = PCI_CFG_32B_READ(MspData->EcBase, Bus, Device, Func, Register.Offset + BaseOffset, Command);
#else
      Data = PCI_CFG_32B_READ (MspData->EcBase, Bus, Device, Func, Register.Offset + BaseOffset);
#endif
#endif
        MspDebugPrint ((MSP_DBG_REG_ACCESS, "PCI read %02X/%02X/%01X, Offset=0x%X, Mask=0x%08X, Value=0x%08X\n",
           Bus, Device, Func, Register.Offset + BaseOffset, (UINT32) Register.Mask, (UINT32) Data));
    } else if (MspInstancePortMap[BoxType].AccessMethod == eBAR) {
      //
      // Memory BAR access
      //
      MemOffset = (UINT32 *)(UINTN)(MspData->DynamicVars[Channel][MspInstancePortMap[BoxType].AccessBase] + Register.Offset + BaseOffset);
#if SIM
      Data = MySimMmio32Read (0xF, (INT32) MemOffset);
#elif JTAG
      Data = JtagMem32Read(0xF, MemOffset, Command);
#else
      Data = (UINT32)*MemOffset;
#endif
      MspDebugPrint ((MSP_DBG_REG_ACCESS, "MEM read to offset=0x%08X; data=0x%08X\n", MemOffset, (UINT32) Data));
    } else {
      //
      // Sideband access.
      //
      //
      // If the read and write opcodes are the same, it means reads are not supported.
      //
      if (MspInstancePortMap[BoxType].ReadOperation == MspInstancePortMap[BoxType].WriteOperation) {
        return 0;
      }

#if SIM
      Data = MySimMmio32ReadFast (PortId, Register.Offset + BaseOffset, 0xF);
#elif JTAG
      Data = isb32Read(PortId, Register.Offset + BaseOffset, Command);
#else
      Mmio32Write (MspData->EcBase + 0xD8, (Register.Offset + BaseOffset) & 0xFFFFFF00);
      Mmio32Write (MspData->EcBase + 0xD0, (Command | (PortId << 16) | ((Register.Offset & 0x000000FF) << 8) + 0xF0) );
      Data = Mmio32Read (MspData->EcBase + 0xD4);
#endif
      MspDebugPrint ((MSP_DBG_REG_ACCESS, "SB read to port=0x%02X; opcode=0x%02X; offset=0x%08X; data=0x%08X\n", 
        PortId, MspInstancePortMap[BoxType].ReadOperation, Register.Offset + BaseOffset, (UINT32) Data));
    }
    TotalValue = (TotalValue << ShiftRightAmount) | ((Data & (UINT32) Register.Mask) >> ShiftLeftAmount);
    return TotalValue;
  }
#if USE_64_BIT_VARIABLES
  return 0xFFFFFFFFFFFFFFFF;
#else
  return 0xFFFFFFFF;
#endif
}

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

  Reads registers from an specified Unit

Arguments:

  Boxtype:          Unit to select
  Instance:         Channel under test
  Offset:           Offset of register to write.
  Data:             Data to be written
  Be:               Byte enables

Returns:

  None

--*/
{
  UINT32          Command;
  UINT8           PortId;
  UINT32          BaseOffset;
  UINT8           Bus;
  UINT8           Device;
  UINT8           Func;
  UINT32         *MemOffset;
  MSP_DATA       *MspData;
  UINT8           OffsetShift;
  UINT8           ShiftLeftAmount;
  UINT8           ShiftRightAmount;
  UINTX           TotalValue;
  REGISTER_ACCESS TempRegister;
  UINTX           TempData;
  UINT8           Temp;

  OffsetShift       = 0;
  ShiftLeftAmount   = 0;
  ShiftRightAmount  = 0;
  TotalValue        = 0;
  MspData           = GetMspHostStructureAddress();

  if (GetRegisterAccessInfo (BoxType, Channel, Instance, &Command, &PortId, &BaseOffset, &Bus, &Device, &Func, ModeWrite) == NoError) {
    //
    // If we are dealing with a PCI or MEM type register, the offset could
    // not be DWORD aligned. In this case, we may need to do multiple writes to
    // piece together the entire register since reads to PCI or MEM should be 
    // DWORD aligned.
    //
    if (MspInstancePortMap[BoxType].AccessMethod == ePCI || MspInstancePortMap[BoxType].AccessMethod == eBAR) {
      OffsetShift       = (Register.Offset % 4);
      ShiftLeftAmount   = OffsetShift * 8;
      ShiftRightAmount  = 32 - ShiftLeftAmount;
      //
      // Align the offset to the next DWORD down (i.e. offset 0x33 will be offset 0x30 after this).
      //
      Register.Offset  -= OffsetShift;
      //
      // If the offset needed to be shifted (ie. it's not DWORD aligned) or the
      // mask is more than 32 bits, we need to recurse to read another DWORD.
      //
      if (OffsetShift != 0 || Register.Mask > 0xFFFFFFFF) {
        //
        // Write the next 32 bits (0xFFFFFFFF), but leave any bits above that
        // left over so if we need to recurse again, we can.
        //
        TempRegister.Mask     = Register.Mask >> ShiftRightAmount;
        TempRegister.Offset   = Register.Offset + 4;
        TempRegister.ShiftBit = Register.ShiftBit;
        TempData              = Data >> ShiftRightAmount;
        if (TempRegister.Mask != 0) {
          MemRegWrite (BoxType, Channel, Instance, TempRegister, TempData, 0xF);
        }
      }
      //
      // We've completed recursion, writing all upper DWORDs of the register. Now we
      // need to write the final DWORD. Since we've subtracted OffsetShift from the original
      // offset, we need to align the mask with the new DWORD aligned offset.
      //
      Register.Mask <<= ShiftLeftAmount;
      //
      // If the register is DWORD aligned and we're writing all 32 bits, skip the read.
      // Else, need to read/modify/write register.
      //
      if (OffsetShift != 0 || (UINT32) Register.Mask != 0xFFFFFFFF) {
        //
        // Since we're doing a 32 bit read/modify/write, only use the lower 32 bits of
        // the mask.
        //
        TempRegister.Mask = (UINT32) ~Register.Mask;
        TempRegister.Offset = Register.Offset;
        TotalValue = MemRegRead (BoxType, Channel, Instance, TempRegister);
      }
      //
      // Now that we have the fields read from the register and masked, OR in the new data.
      //
      TotalValue |= (Data << Register.ShiftBit);
      Data        = TotalValue << ShiftLeftAmount;
    }
    // 
    // If the action is to be taking place and the transaction is a PCI, then just do the PCI Access. 
    // 
    if (MspInstancePortMap[BoxType].AccessMethod == ePCI) {
      MspDebugPrint ((MSP_DBG_REG_ACCESS, "PCI write to %02X/%02X/%01X, Offset=0x%X, Mask=0x%08X, Data=0x%08X\n",
        Bus, Device, Func, Register.Offset + BaseOffset, (UINT32) Register.Mask, (UINT32) Data));
      //
      // PCI access
      //
#if defined SIM || defined SIM_BUILD || defined KTI_SW_SIMULATION
//
// EP/EX segment simulation hooks.
//
/*      CpuSimRegAccess (MrcData, MrcData->var.mem.currentSocket, 
        (UINT32)(MrcData->var.common.mmCfgBase +
        (UINT32)(Bus << 20) + 
        (UINT32)(Device << 15) + 
        (UINT32)(Func << 12) + 
                 Register.Offset + BaseOffset), 4, (UINT32)Data, WRITE_ACCESS);*/
#else
#if JTAG
      PCI_CFG_32B_WRITE (MspData->EcBase, Bus, Device, Func, Register.Offset + BaseOffset, (UINT32) Data, Command);
#else
      PCI_CFG_32B_WRITE(MspData->EcBase, Bus, Device, Func, Register.Offset + BaseOffset, (UINT32)Data);
#endif
#endif
    } else if (MspInstancePortMap[BoxType].AccessMethod == eBAR) {
      //
      // Memory BAR access
      //
      MemOffset = (UINT32 *)(UINTN)(MspData->DynamicVars[Channel][MspInstancePortMap[BoxType].AccessBase] + Register.Offset + BaseOffset);
      MspDebugPrint ((MSP_DBG_REG_ACCESS, "MEM write to offset=0x%08X; data=0x%08X\n", MemOffset, (UINT32) Data));

      if (MspData->EmulationOverrides.ConvertAccessToAssembly) {

        MspDebugPrint ((MSP_DBG_REG_ACCESS, "BoxType: 0x%02x  BaseOffset=0x%08X\n", BoxType, BaseOffset));

        if (MspData->EmulationOverrides.ConvertMchBarToP2SBAR == FALSE) {

          MspDebugPrint ((MSP_DBG_REG_ACCESS, "mov DWORD PTR ds:[0%08Xh], %08Xh\n", MemOffset, (UINT32) Data));

        } else {
#if EMU_MOV_SUPPORT == 1
          if (BaseOffset == MspInstancePortOffset[MspInstancePortMap[DUNIT_COMMON].InstanceStartIndex + 0].Offset) {

            MspDebugPrint ((MSP_DBG_REG_ACCESS, "mov DWORD PTR ds:[0FD16%04Xh], %08Xh\n", Register.Offset, (UINT32) Data));

          } else if (BaseOffset == MspInstancePortOffset[MspInstancePortMap[DUNIT].InstanceStartIndex + 0].Offset) {

            MspDebugPrint ((MSP_DBG_REG_ACCESS, "mov DWORD PTR ds:[0FD10%04Xh], %08Xh\n", Register.Offset, (UINT32) Data));

          } else if (BaseOffset == MspInstancePortOffset[MspInstancePortMap[DUNIT].InstanceStartIndex + 1].Offset) {

            MspDebugPrint ((MSP_DBG_REG_ACCESS, "mov DWORD PTR ds:[0FD12%04Xh], %08Xh\n", Register.Offset, (UINT32) Data));

          } else if (((BaseOffset >> 16) & 0xFF) == MspInstancePortOffset[MspInstancePortMap[DDRDQ_PHY].InstanceStartIndex].Port) {

            MspDebugPrint ((MSP_DBG_REG_ACCESS, "mov DWORD PTR ds:[0FD15%04Xh], %08Xh\n", Register.Offset, (UINT32) Data));

          }
#endif // EMU_MOV_SUPPORT == 1
        }
      }


#if SIM
      MySimMmio32Write ((INT32) MemOffset, (UINT32) Data);
#elif defined JTAG
      JtagMem32Write(Command, (UINT32)MemOffset, (UINT32)Data, Be);
      //JtagMem32Write (MemOffset, (UINT32) Data);
#else
      *MemOffset = (UINT32)Data;
#endif
    } else {
      //
      // Sideband access.
      //
      //
      // If we're writing all bits for each byte enable, skip the read.
      // Else, need to read/modify/write register.
      //
#if USE_64_BIT_VARIABLES
      if (ByteEnableToUintX (Be, 0xFFFFFFFFFFFFFFFF, &Temp) != Register.Mask) {
#else
      if (ByteEnableToUintX (Be, 0xFFFFFFFF, &Temp) != Register.Mask) {
#endif
        TempRegister.Mask   = (UINT32) ~Register.Mask;
        TempRegister.Offset = Register.Offset;
        TotalValue          = MemRegRead (BoxType, Channel, Instance, TempRegister);
        Data                = TotalValue | (Data << Register.ShiftBit);
      }
      MspDebugPrint ((MSP_DBG_REG_ACCESS, "SB write to port=0x%02X; opcode=0x%02X; offset=0x%08X; data=0x%08X; be=0x%01X\n", 
        PortId, MspInstancePortMap[BoxType].WriteOperation, Register.Offset + BaseOffset, (UINT32) Data, Be));
#if SIM
      MySimMmio32WriteFast (PortId, Command, Register.Offset + BaseOffset, (UINT32)Data, Be);
#elif defined JTAG
      isb32Write (PortId, Register.Offset + BaseOffset, Command, (UINT32) Data, Be);
#else
      Mmio32Write (MspData->EcBase + 0xD8, (Register.Offset + BaseOffset) & 0xFFFFFF00);
      Mmio32Write (MspData->EcBase + 0xD4, (UINT32) Data);
      Mmio32Write (MspData->EcBase + 0xD0, (Command | (PortId << 16) | ((Register.Offset & 0x000000FF) << 8) + (Be << 4)) );
#endif
    }
  }
}


UINTX
MemFieldRead (
              IN        UINT8           BoxType,
              IN        UINT8           Channel,
              IN        UINT8           Instance,
              IN        REGISTER_ACCESS Register
              )
{
  UINTX Value;
  //
  // Read the entire register specified.
  //
  Value = MemRegRead (BoxType, Channel, Instance, Register);
  //
  // Shift it over so the field starting at ShiftBit is returned.
  //
  Value >>= Register.ShiftBit;
  //
  // Return the Value.
  //
  return Value;
}

VOID
MemFieldWrite (
               IN        UINT8           BoxType,
               IN        UINT8           Channel,
               IN        UINT8           Instance,
               IN        REGISTER_ACCESS Register,
               IN        UINTX           Value,
               IN        UINT8           Be
               )
{
  UINTX TempValue;    // Value passed in is the Value that goes to the Bits, this contains the entire register Value.

  if (Register.Offset == 0xffff) {
    return;
  }
  // 
  // Read the 32-bit register.
  //
  TempValue = MemRegRead (BoxType, Channel, Instance, Register);
  // 
  // Shift the inputted Value to the correct start location
  //
  TempValue |= (TempValue << Register.ShiftBit);
  // 
  // Write the register back.
  //
  MemRegWrite (BoxType, Channel, Instance, Register, TempValue, Be);

  return;
}


