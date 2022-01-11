/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c)2009-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  NorthFabricErrorHandler.c

Abstract:

  SMM Error Logging.

------------------------------------------------------------------*/
#include "CommonErrorHandlerIncludes.h"

INSTANCE_PORT_MAP InstancePortMap[MAX_BOXES] = {
  //TotalInst    Inst/Channel  StartIndex     R-Op     W-Op  Access Method  Access BAR   SimSupported     StrIndex
    {     1,            0,         0,         0xFF,    0xFF,    eBAR,           MCH_BASE_ADDRESS,       TRUE,      0x00000000}, // DUNIT_COMMON
    {     2,            1,         1,         0xFF,    0xFF,    eBAR,           MCH_BASE_ADDRESS,       TRUE,      0x0000004A}, // DUNIT
    {    18,            9,         3,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,       TRUE,      0x000000B9}, // DDRDQ
    {     9,            0,        21,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,       TRUE,      0x000001B8}, // DDRDQ_PHY
    {     2,            0,        30,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,       TRUE,      0x000002B6}, // DDRCC2_PHY
    {     1,            0,        32,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,       TRUE,      0x00000300}, // DDRCC1_PHY
    {     1,            0,        33,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,       TRUE,      0x00000390}, // DDRPLL
    {     2,            1,        34,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,       TRUE,      0x0000039F}, // DDRCC0
    {     2,            1,        36,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,       TRUE,      0x000003E9}, // DDRCC1
    {     2,            1,        38,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,       TRUE,      0x00000479}, // DDRCC2
    {     1,            0,        40,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,       TRUE,      0x000004C3}, // DDRSAI
    {     1,            0,        41,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,      FALSE,      0x000004C9}, // BUNIT
    {     1,            0,        42,         0xFF,    0xFF,    ePCI,                          0,      FALSE,      0xFFFFFFFF}, // RTF
    {     2,            1,        43,         0xFF,    0xFF,    eBAR,           MCH_BASE_ADDRESS,       TRUE,      0x000004F2}, // CPGC
    {     1,            0,        45,         0xFF,    0xFF,    eBAR,           MCH_BASE_ADDRESS,      FALSE,      0x000005BF}, // BUNIT_MCHBAR
    {     1,            0,        46,         0xFF,    0xFF,    eBAR,           MCH_BASE_ADDRESS,      FALSE,      0x00000696}, // AUNIT_MCHBAR
    {     1,            0,        47,         0xFF,    0xFF,    ePCI,                          0,      FALSE,      0x000006DB}, // BUNITMEM
    {     1,            0,        48,         0xFF,    0xFF,    eBAR,           MCH_BASE_ADDRESS,      FALSE,      0x000006E1}, // PUNITSA
    {     1,            0,        49,         0xFF,    0xFF,    eBAR,           PMC_BASE_ADDRESS,      FALSE,      0x00000738}, // PMC_PCI_MMR
    {     1,            0,        50,         0xFF,    0xFF,    ePCI,                          0,      FALSE,      0x000007D4}, // PMC_PCI_CFG
    {     1,            0,        51,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,      FALSE,      0x000007E5}, // CLTT_CCR
    {     4,            1,        52,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,      FALSE,      0x00000801}, // SUNIT
    {     1,            0,        56,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,      FALSE,      0x00000812}, // AUNIT_MSG_MAP_1
    {     1,            0,        57,         0xFF,    0xFF,    eBAR,          P2SB_BASE_ADDRESS,      FALSE,      0x0000082A}, // BUNIT_MSG_MAP_0
};


INSTANCE_PORT_OFFSET InstancePortOffset[MAX_INSTANCE_PORTS] = {
  {0x0000, 0x1400}, // DUNIT_COMMON0
  {0x0000, 0x1000}, // DUNIT0
  {0x0000, 0x1200}, // DUNIT1
  {0x0015, 0x0000}, // DDRDQ0
  {0x0015, 0x0800}, // DDRDQ1
  {0x0015, 0x1000}, // DDRDQ2
  {0x0015, 0x1800}, // DDRDQ3
  {0x0015, 0x2000}, // DDRDQ4
  {0x0015, 0x2800}, // DDRDQ5
  {0x0015, 0x3000}, // DDRDQ6
  {0x0015, 0x3800}, // DDRDQ7
  {0x0015, 0x4000}, // DDRDQ8
  {0x0015, 0x0230}, // DDRDQ9
  {0x0015, 0x0A30}, // DDRDQ10
  {0x0015, 0x1230}, // DDRDQ11
  {0x0015, 0x1A30}, // DDRDQ12
  {0x0015, 0x2230}, // DDRDQ13
  {0x0015, 0x2A30}, // DDRDQ14
  {0x0015, 0x3230}, // DDRDQ15
  {0x0015, 0x3A30}, // DDRDQ16
  {0x0015, 0x4230}, // DDRDQ17
  {0x0015, 0x0000}, // DDRDQ_PHY0
  {0x0015, 0x0800}, // DDRDQ_PHY1
  {0x0015, 0x1000}, // DDRDQ_PHY2
  {0x0015, 0x1800}, // DDRDQ_PHY3
  {0x0015, 0x2000}, // DDRDQ_PHY4
  {0x0015, 0x2800}, // DDRDQ_PHY5
  {0x0015, 0x3000}, // DDRDQ_PHY6
  {0x0015, 0x3800}, // DDRDQ_PHY7
  {0x0015, 0x4000}, // DDRDQ_PHY8
  {0x0015, 0x4800}, // DDRCC2_PHY0
  {0x0015, 0x5800}, // DDRCC2_PHY1
  {0x0015, 0x5000}, // DDRCC1_PHY0
  {0x0015, 0x6000}, // DDRPLL0
  {0x0015, 0x4800}, // DDRCC00
  {0x0015, 0x4A30}, // DDRCC01
  {0x0015, 0x5000}, // DDRCC10
  {0x0015, 0x5230}, // DDRCC11
  {0x0015, 0x5800}, // DDRCC20
  {0x0015, 0x5A30}, // DDRCC21
  {0x0015, 0xF800}, // DDRSAI0
  {0x004C, 0x0000}, // BUNIT0
  {0x0070, 0x0000}, // RTF0
  {0x0000, 0x3C00}, // CPGC0
  {0x0000, 0x4000}, // CPGC1
  {0x0000, 0x0000}, // BUNIT_MCHBAR0
  {0x0000, 0x0000}, // AUNIT_MCHBAR0
  {0x0000, 0x0000}, // BUNITMEM0
  {0x0000, 0x0000}, // PUNITSA0
  {0x0000, 0x0000}, // PMC_PCI_MMR0
  {0x00FA, 0x0000}, // PMC_PCI_CFG0
  {0x00CC, 0x0000}, // CLTT_CCR0
  {0x0054, 0x0000}, // SUNIT0
  {0x0055, 0x0000}, // SUNIT1
  {0x0056, 0x0000}, // SUNIT2
  {0x0057, 0x0000}, // SUNIT3
  {0x004F, 0x0000}, // AUNIT_MSG_MAP_10
  {0x004B, 0x0000}, // BUNIT_MSG_MAP_00
};


UINT8 ChannelToInstanceMap[MAX_BOXES][MAX_CHANNELS] = {
//CH00  CH01  
  {0x00, 0x00}, // DUNIT_COMMON
  {0x00, 0x01}, // DUNIT
  {0x00, 0x09}, // DDRDQ
  {0x00, 0x00}, // DDRDQ_PHY
  {0x00, 0x00}, // DDRCC2_PHY
  {0x00, 0x00}, // DDRCC1_PHY
  {0x00, 0xFF}, // DDRPLL
  {0x00, 0x01}, // DDRCC0
  {0x00, 0x01}, // DDRCC1
  {0x00, 0x01}, // DDRCC2
  {0x00, 0xFF}, // DDRSAI
  {0x00, 0x00}, // BUNIT
  {0x00, 0x00}, // RTF
  {0x00, 0x01}, // CPGC
  {0x00, 0x00}, // BUNIT_MCHBAR
  {0x00, 0x00}, // AUNIT_MCHBAR
  {0x00, 0xFF}, // BUNITMEM
  {0x00, 0x00}, // PUNITSA
  {0x00, 0x00}, // PMC_PCI_MMR
  {0x00, 0x00}, // PMC_PCI_CFG
  {0x00, 0xFF}, // CLTT_CCR
  {0x00, 0xFF}, // SUNIT
  {0x00, 0xFF}, // AUNIT_MSG_MAP_1
  {0x00, 0xFF}, // BUNIT_MSG_MAP_0
};


VOID
ClearBUnitErrors(
  )
/*++

Routine Description:

  Clear Bunit errors before enabling them.

Arguments:
  
  None 

Returns:

  None

--*/
{  

  UINT64 BMCISts;
  BMCISts = AsmReadMsr64(MSR_B_CR_MCI_STS);

  //clear MCI Status valid field.
  // Denverton Aptiov override Start - EIP#247187
  // System hangs while trying to clear B_MCI_STATUS_VALID bit alone, as we have MC error logged in status register
  //BMCISts &= ~B_MCI_STATUS_VALID; // ???? need to check the properties of this bit. whether write 1 to Clear?
  BMCISts = 0;// writing zero's to clear the register
  // Denverton Aptiov override End - EIP#247187
  AsmWriteMsr64(MSR_B_CR_MCI_STS, BMCISts);
}

VOID
ClearNorthComplexErrors(
  )
/*++

Routine Description:

  Clear all North Complex errors before enabling them.

Arguments:
  
  ClearSystemErrors : 

Returns:

  none

--*/
{  

//  ClearDUnitErrors ();
//  ClearBUnitErrors ();

}

VOID
EnableElogBUnit(
	)
/*++

Routine Description:

  This function Enables or disables the BUnit errors.

Returns:

  None

--*/
{
  UINT64                          MciCtl 	= 0;
  UINT64                          MciCtl2 	= 0;

  MciCtl = AsmReadMsr64(MSR_B_CR_MCI_CTL);

  if ((mErrLogSetupData.SystemErrorEn != ENABLED) || (!mErrLogSetupData.BUnitErrorLog)) {

    MciCtl &= ~(B_MC_RD_DATA_COR | B_MC_RD_DATA_UNC | B_BRAM_RD_PAR | B_BRAM_WR_PAR | B_XUCODE_ERR |\
	          B_IA_HIT_GSM | B_MMIO_CLFLUSH | B_MMIO_WBMTOIE | B_MMIO_HITM | B_PII_2_MMIO | B_UNS_IDI_OP);
  } else {
    MciCtl |= (B_MC_RD_DATA_COR | B_MC_RD_DATA_UNC | B_BRAM_RD_PAR | B_BRAM_WR_PAR | B_XUCODE_ERR |\
	          B_IA_HIT_GSM | B_MMIO_CLFLUSH | B_MMIO_WBMTOIE | B_MMIO_HITM | B_PII_2_MMIO | B_UNS_IDI_OP);

    AsmWriteMsr64(MSR_B_CR_MCI_CTL, MciCtl);

    MciCtl2 = AsmReadMsr64(MSR_B_CR_MCI_CTL2);
    MciCtl2 |=  B_CORR_ERR_INT_ENABLE;

	AsmWriteMsr64(MSR_B_CR_MCI_CTL2, MciCtl2);
  }
}

VOID
ProgramNorthComplexErrorSignals(
	)
/*++

Routine Description:

  This function Enables or disables the Memory and Processor errors.

Returns:

  none

--*/
{

  //
  //Enable DUNIT memory error reporting.
  //
  EnableElogDUnit();

  EnableElogBUnit();
	
}

BOOLEAN
ElogBUnitErrorHandler 
(
  )
/*++

Routine Description:
    BUNIT SMI handler

Arguments:
    None

Returns:
    None

--*/ 
{

  BOOLEAN                        ErrorDetected = FALSE;
  NON_STANDARD_DEV_ERROR_INFO    NonStdDevErrInfo;
  UINT64                         BMCISts;
  REGISTER_ACCESS                Register;

  Register.Offset = R_MCI_STATUS;
  Register.Mask = 0xFFFFFFFF;
  Register.ShiftBit = 0;

  BMCISts = MemRegRead (BUNIT, 0, 0, Register);

  if ((BMCISts & B_MCI_STATUS_VALID) && (BMCISts & B_ERR_ENABLED)) {

	NonStdDevErrInfo.PortType = BUNIT;

	if (BMCISts & B_UNCORRECTED_ERR) 
	  NonStdDevErrInfo.ErrorType = PLATFORM_NON_STANDARD_DEV_ERROR_BUNIT_UNC;
	if (BMCISts & B_PROCESSOR_CONTEXT_CORRUPTED)
	  NonStdDevErrInfo.ErrorType = PLATFORM_NON_STANDARD_DEV_ERROR_BUNIT_PCC;
	if (BMCISts & B_BRAM_ERR)
	  NonStdDevErrInfo.ErrorType = PLATFORM_NON_STANDARD_DEV_ERROR_BUNIT_BRAM_PARITY;

    mUCNonStdErrDetected = TRUE;

	mWheaPlatformSupport->WheaPlatformElogNonStandardDev (
                                        mWheaPlatformSupport,
                                        GEN_ERR_SEV_NON_STANDARD_DEV_FATAL,
                                        &NonStdDevErrInfo
                                        );
  }
  return ErrorDetected;
}

BOOLEAN
ElogNorthComplexErrorHandler 
(
  )
/*++

Routine Description:
    North Complex Elog SMI handler to handle CPU Memory and Uncore errors

Arguments:
    None

Returns:
    None

--*/ 
{

  UINT32           Global_CO_Status;
  UINT32           Global_NF_Status;
  UINT32           Global_FA_Status;
  BOOLEAN          ErrorDetected = FALSE;

  
  //
  //Global corrected errors status
  //
  Global_CO_Status = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GCOERRSTS));

  //
  //Global Non Fatal errors status
  //
  Global_NF_Status = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GNFERRSTS));

  //
  //Global Fatal errors status
  //
  Global_FA_Status = PciExpressRead32 (PCI_PCIE_ADDR(GLREG_BUS, GLREG_DEVICE, GLREG_FUNC, R_GLREG_GFAERRSTS));

  if ((Global_CO_Status & (B_DUNIT0_CO_STS | B_DUNIT1_CO_STS)) ||
      (Global_NF_Status & (B_DUNIT0_NF_STS | B_DUNIT1_NF_STS)) ||
      (Global_FA_Status & (B_DUNIT0_FA_STS | B_DUNIT1_FA_STS))) {

    ErrorDetected |= ElogMemoryErrorHandler ();
  }

  if ((Global_CO_Status & (B_BUNIT_CO_STS)) ||
      (Global_NF_Status & (B_BUNIT_NF_STS)) ||
      (Global_FA_Status & (B_BUNIT_FA_STS))) {

    ErrorDetected |= ElogBUnitErrorHandler ();
  }

  return ErrorDetected;
}

UINT64
ByteEnableToUint64 (
  IN     UINT8 Be, 
  IN     UINT64 Value, 
  IN     UINT8 *ValueLength
) 
{
  UINT8 CurrentByte;
  UINT64 FinalValue;
  UINT8  TestByte;
  
  *ValueLength = 0;
  CurrentByte = 0;
  FinalValue = 0;
  for (TestByte = 0; TestByte < 64; TestByte += 8) {
    if ((Be & 1) == 1) {
      (*ValueLength)++;
      FinalValue += LShiftU64((Value & 0xff), TestByte);
      Value = RShiftU64(Value, 8);
    }
    Be >>= 1;
  }
  //
  // In case of no byte enables (i.e. wake command or something), set to 1 byte of data.
  //
  if (*ValueLength == 0)
  {
    *ValueLength = 1;
  }
  return FinalValue;
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
)
{
  UINT32 Error;
  Error = NoError;

  //
  // Provided into this function is the instance number within the channel, getRegAccess requires the total instance index.
  // This translates the channel to the instance number in the INSTANCE_PORT_OFFSET structure.
  //
  if (ChannelToInstanceMap[BoxType][Channel] == 0xFF) {
    return InvalidInstance;
  } else {
    Instance += ChannelToInstanceMap[BoxType][Channel];
  }
  //
  // If the Type is a PCI access, get the B/D/F.
  //
  if (InstancePortMap[BoxType].AccessMethod == ePCI) {
    *Bus    = (InstancePortOffset[InstancePortMap[BoxType].InstanceStartIndex + Instance].Port >> 8) & 0xff;
    *Device = (InstancePortOffset[InstancePortMap[BoxType].InstanceStartIndex + Instance].Port >> 3) & 0x1f;
    *Func   = InstancePortOffset[InstancePortMap[BoxType].InstanceStartIndex + Instance].Port & 0x07;
    *Offset = 0;
  } else if (InstancePortMap[BoxType].AccessMethod == eSB) {
    //
    // Look at the type, which is either a sideband port or an address offset for this box type.
    // If it's a port, assign it here. If it's an address offset, do the appropriate math to
    // target the Instance passed in.
    //
    *PortId = (UINT8) InstancePortOffset[InstancePortMap[BoxType].InstanceStartIndex + Instance].Port;
    *Offset = InstancePortOffset[InstancePortMap[BoxType].InstanceStartIndex + Instance].Offset;
  } else if (InstancePortMap[BoxType].AccessMethod == eBAR) {
    //
    // For BAR accesses, compile a full 32 bit offset from Port (upper 16 bits) and Offset (lower 16 bits).
    // We reuse the definition of Port for the upper 16 bits to save on code space in MspInstancePortOffset.
    //
    *Offset = (InstancePortOffset[InstancePortMap[BoxType].InstanceStartIndex + Instance].Port << 16) + InstancePortOffset[InstancePortMap[BoxType].InstanceStartIndex + Instance].Offset;
  }
  //
  // Get the sideband command to read/write this particular box type.
  // If the command is not read or write, do not attempt to change it.
  //
  if (Mode == ModeRead) {
  *Command = InstancePortMap[BoxType].ReadOperation  << 24;
  } else if (Mode == ModeWrite) {
  *Command = InstancePortMap[BoxType].WriteOperation << 24;
  } else {
    *Command = *Command << 24;
  }
  //
  // If we've requested a register access to an instance that is
  // not valid (i.e. CH3 DDRIO register on a 2CH platform), flag
  // an error.
  //
  if (Instance >= InstancePortMap[BoxType].TotalInstances) {
    Error = InvalidInstance;
  }

  return Error;
}


UINT64
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

  Boxtype:         Unit to select
  Channel:         Channel under test
  Instance:        Box instance   
  Register:        Offset of register to read.

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
  UINTN           MemOffset;
  UINT8           OffsetShift;
  UINT8           ShiftLeftAmount;
  UINT8           ShiftRightAmount;
  UINT64          TotalValue;
  REGISTER_ACCESS TempRegister;

  OffsetShift       = 0;
  ShiftLeftAmount   = 0;
  ShiftRightAmount  = 0;
  TotalValue        = 0;

  if (GetRegisterAccessInfo (BoxType, Channel, Instance, &Command, &PortId, &BaseOffset, &Bus, &Device, &Func, ModeRead) == NoError) {
    //
    // If we are dealing with a PCI or MEM type register, the offset could
    // not be DWORD aligned. In this case, we may need to do multiple reads to
    // piece together the entire register since reads to PCI or MEM should be 
    // DWORD aligned. 
    //
    if (InstancePortMap[BoxType].AccessMethod == ePCI || InstancePortMap[BoxType].AccessMethod == eBAR) {
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
        TempRegister.Mask     = RShiftU64(Register.Mask, ShiftRightAmount);
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
      Register.Mask = LShiftU64(Register.Mask, ShiftLeftAmount);
    }
    // 
    // If the action is to be taking place and the transaction is a PCI, then just do the PCI Access. 
    // 
    if (InstancePortMap[BoxType].AccessMethod == ePCI) {
      //
      // PCI access
      //
      Data = PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS(Bus, Device, Func, Register.Offset + BaseOffset));
    } else if (InstancePortMap[BoxType].AccessMethod == eBAR) {
      //
      // MCHBAR access
      //
      MemOffset = (UINTN)(InstancePortMap[BoxType].AccessBase + Register.Offset + BaseOffset);
      Data = MmioRead32 (MemOffset);
    } else {
      //
      // Sideband access.
      //
      //
      // If the read and write opcodes are the same, it means reads are not supported.
      //
      if (InstancePortMap[BoxType].ReadOperation == InstancePortMap[BoxType].WriteOperation) {
        return 0;
      }

      MmioWrite32 (EC_BASE + 0xD8, (Register.Offset + BaseOffset) & 0xFFFFFF00);
      MmioWrite32 (EC_BASE + 0xD0, (Command | (PortId << 16) | ((Register.Offset & 0x000000FF) << 8) + 0xF0) );
      Data = MmioRead32 (EC_BASE + 0xD4);
    }

    TotalValue = LShiftU64(TotalValue, ShiftRightAmount) | RShiftU64((Data & (UINT32) Register.Mask),  ShiftLeftAmount);
    return TotalValue;
  }
  return 0xFFFFFFFFFFFFFFFF;
}

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
{
  UINT32          Command;
  UINT8           PortId;
  UINT32          BaseOffset;
  UINT8           Bus;
  UINT8           Device;
  UINT8           Func;
  UINTN           MemOffset;
  UINT8           OffsetShift;
  UINT8           ShiftLeftAmount;
  UINT8           ShiftRightAmount;
  UINT64          TotalValue;
  REGISTER_ACCESS TempRegister;
  UINT64          TempData;
  UINT8           Temp;

  OffsetShift       = 0;
  ShiftLeftAmount   = 0;
  ShiftRightAmount  = 0;
  TotalValue        = 0;

  if (GetRegisterAccessInfo (BoxType, Channel, Instance, &Command, &PortId, &BaseOffset, &Bus, &Device, &Func, ModeWrite) == NoError) {
    //
    // If we are dealing with a PCI or MEM type register, the offset could
    // not be DWORD aligned. In this case, we may need to do multiple writes to
    // piece together the entire register since reads to PCI or MEM should be 
    // DWORD aligned.
    //
    if (InstancePortMap[BoxType].AccessMethod == ePCI || InstancePortMap[BoxType].AccessMethod == eBAR) {
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
        TempRegister.Mask     = RShiftU64(Register.Mask, ShiftRightAmount);
        TempRegister.Offset   = Register.Offset + 4;
        TempRegister.ShiftBit = Register.ShiftBit;
        TempData              = RShiftU64(Data, ShiftRightAmount);
        if (TempRegister.Mask != 0) {
          MemRegWrite (BoxType, Channel, Instance, TempRegister, TempData, 0xF);
        }
      }
      //
      // We've completed recursion, writing all upper DWORDs of the register. Now we
      // need to write the final DWORD. Since we've subtracted OffsetShift from the original
      // offset, we need to align the mask with the new DWORD aligned offset.
      //
      Register.Mask = LShiftU64(Register.Mask, ShiftLeftAmount);
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
      TotalValue |= LShiftU64(Data, Register.ShiftBit);
      Data        = LShiftU64(TotalValue, ShiftLeftAmount);
    }
    // 
    // If the action is to be taking place and the transaction is a PCI, then just do the PCI Access. 
    // 
    if (InstancePortMap[BoxType].AccessMethod == ePCI) {
      //
      // PCI access
      //
      PciExpressWrite32 (PCI_EXPRESS_LIB_ADDRESS(Bus, Device, Func, Register.Offset + BaseOffset), (UINT32) Data);
    } else if (InstancePortMap[BoxType].AccessMethod == eBAR) {
      //
      // MCHBAR access
      //
      MemOffset = (UINTN)(InstancePortMap[BoxType].AccessBase + Register.Offset + BaseOffset);
      MmioWrite32(MemOffset,(UINT32)Data);
    } else {
      //
      // Sideband access.
      //
      //
      // If we're writing all bits for each byte enable, skip the read.
      // Else, need to read/modify/write register.
      //
      if (ByteEnableToUint64 (Be, 0xFFFFFFFFFFFFFFFF, &Temp) != Register.Mask) {
        TempRegister.Mask   = (UINT32) ~Register.Mask;
        TempRegister.Offset = Register.Offset;
        TotalValue          = MemRegRead (BoxType, Channel, Instance, TempRegister);
        Data                = TotalValue | LShiftU64(Data, Register.ShiftBit);
      }
      MmioWrite32 (EC_BASE + 0xD8, (Register.Offset + BaseOffset) & 0xFFFFFF00);
      MmioWrite32 (EC_BASE + 0xD4, (UINT32) Data);
      MmioWrite32 (EC_BASE + 0xD0, (Command | (PortId << 16) | ((Register.Offset & 0x000000FF) << 8) + (Be << 4)) );
    }
  }
}

