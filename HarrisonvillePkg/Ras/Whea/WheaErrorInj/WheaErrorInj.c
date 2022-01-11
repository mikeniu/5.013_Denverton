/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c) 2009-2017 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
    WheaErrorInj.c

Abstract:
    This is an implementation of the Core Whea error injection method.

-----------------------------------------------------------------------------*/
#pragma optimize("", off)
#include "WheaErrorInj.h"

// GLobal variables and protocols
BOOLEAN                                      mAcpi5Support = FALSE;
BOOLEAN                                      mPcieErrInjActionTable = FALSE;
BOOLEAN                                      mMemErrInjActionTable = TRUE;
BOOLEAN                                      mMemErrInjSimulated = FALSE;       // Denverton AptioV Override - EIP#321800
BOOLEAN                                      mPCIeErrInjSimulated   = TRUE;
UINT8                                        *mBiosRsvdInjAddrBase;
UINTN                                        mBiosRsvdInjAddr;
EFI_SMM_BASE2_PROTOCOL                       *mSmmBase = NULL;
EFI_SMM_SYSTEM_TABLE2                        *mSmst = NULL;
STATIC  VOID                                 *mWheaEventReg;
STATIC  EFI_WHEA_PLATFORM_SUPPORT_PROTOCOL   *mWheaPlatformSupport = NULL;
STATIC  WHEA_EINJ_PARAM_BUFFER               *mEinjParam = NULL;
STATIC  VOID                                 *mEinjEsts = NULL;
STATIC  WHEA_EINJ_TRIGGER_ACTION_TABLE       *mEinjAction = NULL;
STATIC  UINT8 EinjActionCnt = 0;

//
// WHEA Serialization Table
//
STATIC
EFI_ACPI_WHEA_SERIALIZATION_ACTION mSimEinj[9] = {
  //
  // Serialization Action Table
  //
  {   //Action0
    WHEA_EINJ_BEGIN_INJECTION_OPERATION,// BEGIN_INJECTION_OPERATION
    INSTRUCTION_WRITE_REGISTER_VALUE,   // WRITE_REGISTER_VALUE
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
    EINJ_BEGIN_OPERATION,               // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  },
  {   //Action1
    WHEA_EINJ_GET_TRIGGER_ACTION_TABLE, // GET_TRIGGER_ERROR_STRUC
    INSTRUCTION_READ_REGISTER,          // READ_REGISTER
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x40, 0x00, 0x04, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffffffffffff                  // Mask is only valid for 32-bits 
  },
  {   //Action2
    WHEA_EINJ_SET_ERROR_TYPE,           // SET_ERROR_TYPE
    INSTRUCTION_WRITE_REGISTER,         // WRITE_REGISTER
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  },
  {   //Action3
    WHEA_EINJ_GET_ERROR_TYPE,           // GET_ERROR_TYPE
    INSTRUCTION_READ_REGISTER,          // READ_REGISTER
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 //{0x00, 0x20, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  },
  {   //Action4
    WHEA_EINJ_END_OPERATION,            // END_OPERATION
    INSTRUCTION_WRITE_REGISTER_VALUE,   // WRITE_REGISTER_VALUE
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 //{0x00, 0x20, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    EINJ_END_OPERATION,                 // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  },
  {   //Action5
    WHEA_EINJ_EXECUTE_OPERATION,        // EXECUTE_OPERATION
    INSTRUCTION_WRITE_REGISTER_VALUE,   // WRITE_REGISTER_VALUE
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterActionFiller,           // {0x01, 0x10, 0x00, 0x01, R_APM_CNT}, // GAS (WORD IO). Address will be filled in runtime
    EFI_WHEA_EINJ_EXECUTE_SWSMI,        // Value for InjectError()
    0xffff                              // Mask is only valid for 16-bits 
  },
  {   //Action6
    WHEA_EINJ_CHECK_BUSY_STATUS,        // CHECK_BUSY_STATUS
    INSTRUCTION_READ_REGISTER_VALUE,    // READ_REGISTER
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x30, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0x00000001,                         // Value for InjectError()
    0x00000001                          // Mask is only valid for 32-bits 
  },
  {   //Action7
    WHEA_EINJ_GET_COMMAND_STATUS,       // GET_OPERATION_STATUS
    INSTRUCTION_READ_REGISTER,          // READ_REGISTER
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x40, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0x000001fe                          // Mask is only valid for 32-bits 
  },
  {   //Action8
    WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRES,// SET_ERROR_TYPE_WITH_ADDRESS
    INSTRUCTION_WRITE_REGISTER,         // WRITE_REGISTER
    FLAG_PRESERVE_REGISTER,             // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  }
};

STATIC
WHEA_EINJ_TRIGGER_ACTION_TABLE mSimEinjAction = \
{
  {sizeof (WHEA_ERROR_TRIGGER_ACTION_HEADER), 0, sizeof (WHEA_EINJ_TRIGGER_ACTION_TABLE), 4},
  { //Action0
    WHEA_EINJ_TRIGGER_ERROR,
    INSTRUCTION_NO_OPERATION,         // Intialized as NOP. To be filled by the runtime injection code
    FLAG_NOTHING,                     // Flags
    0x00,                             // Reserved
    EinjRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
    0,                                // Value for InjectError()
    0xffffffffffffffff                // Mask is only valid for 32-bits 
  },
  { //Action1
    WHEA_EINJ_TRIGGER_ERROR,
    INSTRUCTION_NO_OPERATION,         // Intialized as NOP. To be filled by the runtime injection code
    FLAG_NOTHING,                     // Flags
    0x00,                             // Reserved
    EinjRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
    0,                                // Value for InjectError()
    0xffffffff                        // Mask is only valid for 32-bits 
  },
  { //Action2
    WHEA_EINJ_TRIGGER_ERROR,
    INSTRUCTION_NO_OPERATION,         // Intialized as NOP. To be filled by the runtime injection code
    FLAG_NOTHING,                     // Flags
    0x00,                             // Reserved
    EinjRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
    0,                                // Value for InjectError()
    0xffffffff                        // Mask is only valid for 32-bits 
  },
  { //Action3
    WHEA_EINJ_TRIGGER_ERROR,
    INSTRUCTION_NO_OPERATION,         // Write register
    FLAG_NOTHING,                     // Flags
    0x00,                             // Reserved
    EinjRegisterFiller,               // {0x00, 0x20, 0x00, 0x03, -1},    // GAS (DWORD Memory) Address will be filled during boot
    0,                                // Value for InjectError()
    0xffffffff                        // Mask is only valid for 32-bits 
  }
};

EFI_STATUS
InjectPciExError (
 IN   UINT32    PcieSBDF,
 IN	  UINT32    ErrorToInject
 )
{

  UINT8   Bus;
  UINT8   Dev;
  UINT8   Func;
  UINT16  Data;
  UINT32  Data32;
  UINT32  PciInjdevAddr = 00; 
  
  Bus = (PcieSBDF >> 16) & (0xFF);
  Dev = (PcieSBDF >> 11) & (0x1F);
  Func = (PcieSBDF >> 8) & (0x7);

  PciInjdevAddr = (UINT32) PcdGet64 (PcdPciExpressBaseAddress) | PCI_EXPRESS_LIB_ADDRESS(Bus, Dev, Func, R_PCIE_EINJCTL);

  Data = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS( Bus, Dev, Func, R_PCIE_EINJCTL));
  
  if (Data & B_EINJDIS) return EFI_UNSUPPORTED;

  //
  //clear Error Injection register before inject error
  //
  PciExpressWrite16 (PCI_EXPRESS_LIB_ADDRESS( Bus, Dev, Func, R_PCIE_EINJCTL), 0x00);

  Data = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS( Bus, Dev, Func, R_PCIE_EINJCTL));

  if (ErrorToInject == INJECT_ERROR_PCIE_UE_FATAL){

    Data |= B_INJCTOERR;

    //set uncorrectable error severity
    Data32 = PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS( Bus, Dev, Func, R_PCIE_AER_ERRUNCSEV));
    Data32 |= B_PCIE_ERRUNCSEV_CTES;
    PciExpressWrite32 (PCI_EXPRESS_LIB_ADDRESS( Bus, Dev, Func, R_PCIE_AER_ERRUNCSEV), Data32);

  }else if (ErrorToInject == INJECT_ERROR_PCIE_UE_NON_FATAL ) {

     Data |= B_INJCTOERR;

  } else if (ErrorToInject == INJECT_ERROR_PCIE_CE ){

    Data |= B_INJRCVERR;

  }
    
  if(mPcieErrInjActionTable == TRUE) {
    mEinjAction->Trigger0.Operation                            = WHEA_EINJ_TRIGGER_ERROR;
    mEinjAction->Trigger0.Instruction                           = INSTRUCTION_WRITE_REGISTER_VALUE;
    mEinjAction->Trigger0.Flags                                   = FLAG_NOTHING;
    mEinjAction->Trigger0.Reserved8                           = 00;
    mEinjAction->Trigger0.Register.AddressSpaceId     = 00;
    mEinjAction->Trigger0.Register.RegisterBitWidth     = 0x10;
    mEinjAction->Trigger0.Register.RegisterBitOffset     = 0x00;
    mEinjAction->Trigger0.Register.AccessSize            = 0x02;
    mEinjAction->Trigger0.Register.Address                 = PciInjdevAddr;
    mEinjAction->Trigger0.Value                                  = Data;
    mEinjAction->Trigger0.Mask                                   = 0xffffffff;
  } else {
    PciExpressWrite16 (PCI_EXPRESS_LIB_ADDRESS( Bus, Dev, Func, R_PCIE_EINJCTL), Data);
  }
    
  return EFI_SUCCESS;
}


EFI_STATUS
SetBiosRsvdInjAddress(
VOID
)
{

  UINTN      BiosRsvdStartAddr;
  UINTN      BiosRsvdEndAddr;

  BiosRsvdStartAddr  = (UINTN)mBiosRsvdInjAddrBase;
  BiosRsvdEndAddr    = (UINTN)mBiosRsvdInjAddrBase + WHEA_EINJ_ADDR_RANGE - 0x200;  // Safe side reduced 200h bytes from the limit.
  if(mBiosRsvdInjAddr < BiosRsvdStartAddr || mBiosRsvdInjAddr >= BiosRsvdEndAddr)  {
      return EFI_OUT_OF_RESOURCES;
  }
  mEinjParam->EinjAddr = mBiosRsvdInjAddr;
  mEinjParam->AddrMask = 0xffffffffffffffff;
  mBiosRsvdInjAddr += 0x400;
  
  return EFI_SUCCESS;
}

BOOLEAN
ValidateRootPortForInjection (
 IN  UINT32  PcieSBDF
 )
{

  UINT8   Bus;
  UINT8   Dev;
  UINT8   Func;
  UINT16  Data16;

  
  Bus = (PcieSBDF >> 16) & (0xFF);
  Dev = (PcieSBDF >> 11) & (0x1F);
  Func= (PcieSBDF >> 8) & (0x7);

  // Verfiy the  segment number
  if ( (PcieSBDF >> 24) != 00) return FALSE;

  if ((Bus == RPX16_PCIE_BUS) && (Func == RPX16_PCIE_FUNC)) {
    if ((Dev >= RPX16_PCIE_DEVICE) &&  (Dev <= (RPX16_PCIE_DEVICE + RPX16_PCIE_MAX_DEVICE))) {
      Data16 = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS(Bus, Dev, Func, R_PCIE_VID));
      if (Data16 != 0xFFFF) return TRUE;
    }
  }

  if ((Bus == RPX4_PCIE_BUS) && (Func == RPX4_PCIE_FUNC)) {
    if ((Dev >= RPX4_PCIE_DEVICE) &&  (Dev <= (RPX4_PCIE_DEVICE + RPX4_PCIE_MAX_DEVICE))) {
      Data16 = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS(Bus, Dev, Func, R_PCIE_VID));
      if (Data16 != 0xFFFF) return TRUE;
    }
  }

  return FALSE;
}

UINT32
GetRootPortForInjection (
 VOID
 )
{
  UINT8       Bus = RPX16_PCIE_BUS;
  UINT8       Dev;
  UINT8       Func = RPX16_PCIE_FUNC;
  UINT32      PcieSBDF = 0;
  BOOLEAN     Found = FALSE;

  for (Dev = RPX16_PCIE_DEVICE; Dev <= (RPX16_PCIE_DEVICE + RPX16_PCIE_MAX_DEVICE); Dev++) {

      PcieSBDF  = (Bus << 16) | (Dev << 11) | (Func << 0x08);
      // Check if the root port connected to the  slot is populated
      Found = ValidateRootPortForInjection(PcieSBDF);
      if(Found == TRUE)  return PcieSBDF;
 }

  return FALSE;
}

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
  IN        UINT8           Be,
  IN        BOOLEAN         IsTriggerActionTable
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
  EFI_ACPI_WHEA_SERIALIZATION_ACTION            *TriggerPtr;

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
          MemRegWrite (BoxType, Channel, Instance, TempRegister, TempData, 0xF,FALSE);
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
      if (IsTriggerActionTable && EinjActionCnt < WHEA_EINJ_TRIGGER_ACTION_MAX) {
        TriggerPtr = &mEinjAction->Trigger0;
        TriggerPtr += EinjActionCnt;
        TriggerPtr->Operation                  = WHEA_EINJ_TRIGGER_ERROR;
        TriggerPtr->Instruction                = INSTRUCTION_WRITE_REGISTER_VALUE;
        TriggerPtr->Flags                      = FLAG_NOTHING;
        TriggerPtr->Reserved8                  = 00;
        TriggerPtr->Register.AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_MEMORY;
        TriggerPtr->Register.RegisterBitWidth  = 0x20;
        TriggerPtr->Register.RegisterBitOffset = 0x00;
        TriggerPtr->Register.AccessSize        = EFI_ACPI_3_0_DWORD;
        TriggerPtr->Register.Address           = MemOffset;
        TriggerPtr->Value                      = Data;
        TriggerPtr->Mask                       = 0xffffffff;
        EinjActionCnt++;
      } else {
        MmioWrite32(MemOffset,(UINT32)Data);  
      }      
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

VOID
SeedMemError (
  VOID   *LineAddress,
  UINT32 Value
  )
/*++

Routine Description:

  Write data to the error injection adress in order to seed the error in memory

Arguments:
  LineAdress - Error Injection Memory Adress
  Valie      - Data value to write in memory

Returns:

  None.

--*/

{
  UINTN  PhyAddr = 0;

  //
  // PhyAddr contains the error injection Address
  //
  PhyAddr = (UINT64) LineAddress;

  //
  // Write data to memory to seed error
  //
  AsmCommitLine((VOID *) (UINTN)PhyAddr, Value);
  AsmWbinvd ();

  return;
}

VOID
FlushCache (
  IN char *FakeArg
  )
/*++

Routine Description:

  Dummy function so it can be passed to AP processor trough MP Service Prootocol
  to flush the cache on a certain AP.

Arguments:

  char FakeArg - Not used

Returns:

  None.

--*/
{
  AsmWbinvd ();
  return;
}

VOID
FlushCacheOnAllAps (
  )
/*++

Routine Description:

  Flushes cache on all the AP's during Memory Error Injection to ensure
  error is triggered during memory write on OS trough the trigger action table.

Arguments:

Returns:

  None.

--*/
{
  UINTN                           Index;

  for (Index = 0; Index < mSmst->NumberOfCpus; Index++) {
    mSmst->SmmStartupThisAp (FlushCache, Index, NULL);
  }
  return;
}


EFI_STATUS
InjectMemError (
  UINT32   ErrorToInject
  )
/*

Routine Description:

  Inject Multi Bit or Single bit ECC error.

Arguments:

  ErrorToInject - Type of Memory error to Inject (Correctable/UnCorrectable)

Returns:

  None

*/
{
  EFI_STATUS            Status;
  ERRINJCTL             ErrInjCtl;
  ECCCTRL               EccCtrl;
  REGISTER_ACCESS       Register;
  UINT8                 ch;
  UINT8                 Instance;
  UINT32                Data32;
  UINT32                EinjValue;
  UINT64                AddrMask;

  ErrInjCtl.Data = 0;
  Status         = EFI_SUCCESS;

  //
  // Init EinjAddr with Dunit0 addr as done on C-scripts
  //
  ch = 0;
  Instance = 0;
  mEinjParam->EinjAddr = DUNIT0_ADDR;
  mEinjParam->AddrMask = 0xffffffffffffffff;

  //
  // Init ERRINJCTL register's masks
  //
  if (mEinjParam->EinjAddr != 0) {
    Register.Offset = R_ERRINJADDR;
    Register.Mask   = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    Data32 =   (UINT32) RShiftU64(mEinjParam->EinjAddr, 5);  // Address Bit [35:5]
    MemRegWrite (DUNIT, ch, Instance, Register, Data32, 0xF, FALSE);

    AddrMask = ~mEinjParam->AddrMask;
    Register.Offset = R_ERRINJMSK;
    Register.Mask = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    Data32 =   (UINT32) RShiftU64 (AddrMask, 5);  // Address Bit [35:5]
    MemRegWrite (DUNIT, ch, Instance, Register, Data32, 0xF, FALSE);

    ErrInjCtl.Bits.injaddr3 = (RShiftU64 (mEinjParam->EinjAddr, 3) & BIT0);
    ErrInjCtl.Bits.injaddr4 = (RShiftU64 (mEinjParam->EinjAddr, 4) & BIT0);
    ErrInjCtl.Bits.injamsk3 = (RShiftU64 (AddrMask, 3) & BIT0);
    ErrInjCtl.Bits.injamsk4 = (RShiftU64 (AddrMask, 4) & BIT0);
    ErrInjCtl.Bits.injadren = 1;
  } else {
    Register.Offset = R_ERRINJADDR;
    Register.Mask = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    Data32 =   0;  // Address Bit [35:5]
    MemRegWrite (DUNIT, ch, Instance, Register, Data32, 0xF, FALSE);

    AddrMask = ~mEinjParam->AddrMask;
    Register.Offset = R_ERRINJMSK;
    Register.Mask = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    Data32 =   0;  // Address Bit [35:5]
    MemRegWrite (DUNIT, ch, Instance, Register, Data32, 0xF, FALSE);

    ErrInjCtl.Bits.injaddr3 = 0;
    ErrInjCtl.Bits.injaddr4 = 0;
    ErrInjCtl.Bits.injamsk3 = 0;
    ErrInjCtl.Bits.injamsk4 = 0;
    ErrInjCtl.Bits.injadren = 0;
  }

  DEBUG ((EFI_D_INFO, "mEinjParam->EinjAddr:%lx \n", mEinjParam->EinjAddr));
  DEBUG ((EFI_D_INFO, "mEinjParam->AddrMask:%lx \n", mEinjParam->AddrMask));
  DEBUG ((EFI_D_INFO, "AddrMask:%lx \n", AddrMask));

  Register.Offset = R_ERRINJDATA1;
  Register.Mask = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  MemRegWrite (DUNIT, ch, Instance, Register, 0, 0xF, FALSE);

  //
  // Set single / multi Bit ECC / parity error
  //
  if (ErrorToInject == INJECT_ERROR_MEMORY_CE) {
    Register.Offset = R_ERRINJDATA0;
    Register.Mask = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    MemRegWrite (DUNIT, ch, Instance, Register, 1, 0xF, FALSE);

    Register.Offset = R_ERRINJDATA1;
    Register.Mask = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    MemRegWrite (DUNIT, ch, Instance, Register, 0, 0xF, FALSE);
  } else if (ErrorToInject == INJECT_ERROR_MEMORY_UE_NON_FATAL) {
    Register.Offset = R_ERRINJDATA0;
    Register.Mask = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    MemRegWrite (DUNIT, ch, Instance, Register, 7, 0xF, FALSE);

    Register.Offset = R_ERRINJDATA1;
    Register.Mask = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    MemRegWrite (DUNIT, ch, Instance, Register, 0, 0xF, FALSE);
  }
  DEBUG ((EFI_D_INFO, "ErrInjCtl.Bits.injeccpar ch:%lx injeccpar:%lx \n", ch, ErrInjCtl.Bits.injeccpar));

  //
  // Init DECCCTRL register
  //
  Register.Offset = R_ECCCTRL;
  EccCtrl.Data = (UINT32)MemRegRead (DUNIT, ch, 0, Register);
  DEBUG ((EFI_D_INFO, " Original DECCCTRL ch:%lx Data32:%lx \n", ch, EccCtrl.Data));

  EccCtrl.Bits.encbgen = 1;
  EccCtrl.Bits.dparen  = 0;
  EccCtrl.Bits.derren  = 1;
  EccCtrl.Bits.serren  = 1;
  EccCtrl.Bits.eccen   = 1;

  Register.Mask = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  MemRegWrite (DUNIT, ch, Instance, Register, EccCtrl.Data, 0xF, TRUE);
  DEBUG ((EFI_D_INFO, "DECCCTRL ch:%lx Data32:%lx \n", ch, EccCtrl.Data));

  //
  // Init ERRINJCTL register behaviour
  //
// Denverton AptioV Override Start - EIP#375357 

  //Single/Multi Bit Ecc/parity error
  if (ErrorToInject == INJECT_ERROR_MEMORY_CE) {
    ErrInjCtl.Bits.injeccpar = 0; //Single Bit ECC
    ErrInjCtl.Bits.injcap = 0x0;
  } else if (ErrorToInject == INJECT_ERROR_MEMORY_UE_NON_FATAL) { 
    ErrInjCtl.Bits.injeccpar = 1; //Multi Bit ECC
    ErrInjCtl.Bits.injcap = 0x0;
  }


  // Injection in the ECC path
  ErrInjCtl.Bits.injecc    = 1;
  ErrInjCtl.Bits.injadren = 0;
  
  // Error in the Read/Write Data path
  ErrInjCtl.Bits.injread = 1;
  ErrInjCtl.Bits.injwrite = 1;

// Denverton AptioV Override End - EIP#375357

  // Disable error src
  ErrInjCtl.Bits.injsrcen = 0;

  // Inject error once
  ErrInjCtl.Bits.injonce = 1;

  // Finally set Error injection enable bit to inject error once
  ErrInjCtl.Bits.injen = 1;

  Register.Offset = R_ERRINJCTL;
  Register.Mask = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  MemRegWrite (DUNIT, ch, Instance, Register, ErrInjCtl.Data, 0xF, TRUE);

  DEBUG ((EFI_D_INFO, "R_ERRINJCTL ch:%lx Data32:%lx \n", ch, ErrInjCtl.Data));

  //
  // Seed error - second write will trigger
  //
  EinjValue = 0x13579BDF;
  SeedMemError ((VOID *) (UINTN)mEinjParam->EinjAddr, EinjValue);

  //
  // Flush cache on all cores
  //
  FlushCacheOnAllAps ();

  return Status;
}

/**

  This routine fakes a PCIe error injection by:
    1. Create Elog of the specified SBDF
    2. Notify OS using
      -SCI for CE Errors
      -NMI for UC Errors
  @param PcieSBDF  - Decimal representation of SBDF
  @param ErrorType - Error type to for Elog

  Returns:
    -EFI_SUCCESS              - on success
    -EFI_INVALID_PARAMETER    - on fail
**/
EFI_STATUS
SimulatePcieErrorInj (
  IN UINT32 PcieSBDF,
  IN UINT32 ErrorType
  )
{
  //
  // NV ELog support
  //
  PCI_PCIE_ERROR_LOCAL_STRUCT  PciErrStruct;
  //
  // Whea support
  //
  PCIE_PCI_DEV_INFO           PciDev;
  UINT16                      WheaErrType   = 0;
  UINT8                       Bus           = 0;
  UINT8                       Device        = 0;
  UINT8                       Function      = 0;
  UINT8                       ElogErrorType = 0;


  //
  // Decode Error Type for Elog
  //
  if (ErrorType == INJECT_ERROR_PCIE_CE) {
    ElogErrorType = COR_ERROR_TYPE;
  } else if (ErrorType == INJECT_ERROR_PCIE_UE_NON_FATAL) {
    ElogErrorType = NONFATAL_ERROR_TYPE;
  } else if (ErrorType == INJECT_ERROR_PCIE_UE_FATAL) {
    ElogErrorType = FATAL_ERROR_TYPE;
  }

  //
  // Decode SBDF
  //
  Bus      = (PcieSBDF >> 16) & (0xFF);
  Device   = (PcieSBDF >> 11) & (0x1F);
  Function = (PcieSBDF >>  8) & (0x7);


  //
  // Fill Elog info structs
  //
  PciDev.Segment      = 0;
  PciDev.Bus          = Bus;
  PciDev.Device       = Device;
  PciDev.Function     = Function;

  PciErrStruct.ErrorSource  = 1;
  PciErrStruct.ErrorType    = ElogErrorType;
  PciErrStruct.Segment      = 0;
  PciErrStruct.Bus          = Bus;
  PciErrStruct.Device       = Device;
  PciErrStruct.Function     = Function;

  switch (ElogErrorType) {

    case SERROR_TYPE:
      WheaErrType             = GEN_ERR_SEV_PCI_DEV_FATAL;
      break;

    case PERROR_TYPE:
      WheaErrType             = GEN_ERR_SEV_PCI_DEV_FATAL;
      break;

    case COR_ERROR_TYPE:
      WheaErrType             = GEN_ERR_SEV_PCIE_CORRECTED;
      break;

    case NONFATAL_ERROR_TYPE:
      WheaErrType             = GEN_ERR_SEV_PCIE_RECOVERABLE;
      break;

    case FATAL_ERROR_TYPE:
      WheaErrType             = GEN_ERR_SEV_PCIE_FATAL;
      break;

    default:
      return EFI_INVALID_PARAMETER;
  }

  //
  // WHEA Logging support
  //
  if (WheaErrType == GEN_ERR_SEV_PCI_DEV_RECOVERABLE || WheaErrType == GEN_ERR_SEV_PCI_DEV_CORRECTED || \
      WheaErrType == GEN_ERR_SEV_PCI_DEV_FATAL || WheaErrType == GEN_ERR_SEV_PCI_DEV_INFORMATION) {
     mWheaPlatformSupport->WheaPlatformElogPciDev (mWheaPlatformSupport, WheaErrType, &PciDev);
  } else {
     mWheaPlatformSupport->WheaPlatformElogPcieRootDevBridge (mWheaPlatformSupport, WheaErrType, &PciDev);
  }

  //
  // Notify OS
  //
  if (ErrorType == INJECT_ERROR_PCIE_CE) {
    TriggerWheaSci (mSmmBase, mSmst);
  } else if (ErrorType == INJECT_ERROR_PCIE_UE_NON_FATAL || ErrorType == INJECT_ERROR_PCIE_UE_FATAL) {
    TriggerWheaNmi (mSmmBase, mSmst);
  }

  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
WheaEinjHandler (
  IN  EFI_HANDLE                          DispatchHandle,
  IN  CONST EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext, OPTIONAL
  IN  OUT VOID                            *CommBuffer,      OPTIONAL
  IN  OUT UINTN                           *CommBufferSize   OPTIONAL
  )
/*

WheaEinjHandler:

SW SMI Handler for Error Injection.
This handler supports Memory / PCIe Un correctable and correctable error injections.

*/
{
  UINT32                 PcieSBDF = 00;
  MEMORY_DEV_INFO        MemInfo;
  UINT32                 ErrorToInject;
  BOOLEAN                PortValidatedForInjection = FALSE;
  EFI_STATUS             Status;


  mEinjParam->OpStatus  = 0;
  mEinjParam->CmdStatus = 0;
  EinjActionCnt         = 0;

  if (mWheaPlatformSupport == NULL) {
    mEinjParam->CmdStatus = WHEA_EINJ_CMD_UNKNOWN_FAILURE;
    return EFI_INTERRUPT_PENDING;
  }

  //
  // Pre-fill Memory Info in case Memory Error Injection will be simulated
  //
  MemInfo.Node = 9;
  MemInfo.Branch = 8;
  MemInfo.Channel = 1;
  MemInfo.Dimm = 7;
  MemInfo.Rank = 2;
  MemInfo.Row = 0x18;

  mEinjParam->EinjAddr = 00;  // Set address
  mEinjParam->AddrMask = 00;  // Address Mask

  //
  // Check what type of error to be injected and create log accordingly.
  //
  CopyMem (mEinjAction, &mSimEinjAction, sizeof(WHEA_EINJ_TRIGGER_ACTION_TABLE));

  //
  // From SetError type field.
  //
  ErrorToInject = (UINT32)mEinjParam->ErrorToInject[0] & 0x7fffffff;

  //
  // For ACPI 5.1 get the error arguments from Address Table
  //
  if (mAcpi5Support == TRUE)  {

    if (mEinjParam->SetAddressTable.ErrType & (INJECT_ERROR_PCIE_CE |
                                               INJECT_ERROR_PCIE_UE_NON_FATAL |
                                               INJECT_ERROR_PCIE_UE_FATAL)) {
      ErrorToInject = mEinjParam->SetAddressTable.ErrType;
	    if (mEinjParam->SetAddressTable.PcieSBDF &&
          (mEinjParam->SetAddressTable.Flags & WHEA_SET_ERROR_TYPE_WITH_SBDF_VALID)) {
        PcieSBDF  = mEinjParam->SetAddressTable.PcieSBDF;
	    }
    }

    if (mEinjParam->SetAddressTable.ErrType & (INJECT_ERROR_MEMORY_CE |
                                               INJECT_ERROR_MEMORY_UE_NON_FATAL |
                                               INJECT_ERROR_MEMORY_UE_FATAL)) {
  	  ErrorToInject = mEinjParam->SetAddressTable.ErrType;
	    if (mEinjParam->SetAddressTable.Flags & WHEA_SET_ERROR_TYPE_WITH_MEMORY_ADDRESS_VALID) {
        mEinjParam->EinjAddr = mEinjParam->SetAddressTable.MemAddress;       // Set address
        mEinjParam->AddrMask = mEinjParam->SetAddressTable.MemAddressRange;  // Address Mask
	    }
	  }

    //
    // We need to clear the arguments after consumption
    //
    mEinjParam->SetAddressTable.ErrType         = 0;
  	mEinjParam->SetAddressTable.Flags           = 0;
    mEinjParam->SetAddressTable.ApicId          = 0;
    mEinjParam->SetAddressTable.MemAddress      = 0;
    mEinjParam->SetAddressTable.MemAddressRange = 0;
    mEinjParam->SetAddressTable.PcieSBDF        = 0;
  }

  //
  // Re-start Elog
  //
  mWheaPlatformSupport->WheaPlatformRestartAllElog (mWheaPlatformSupport);


  //
  // Inject Error
  //
  switch (ErrorToInject) {

  case INJECT_ERROR_MEMORY_CE:

    //
    // Simulate CE Memory Error Injection (Create Elog and notify OS using SCI)
    //
    if (mMemErrInjSimulated) {
      DEBUG ((EFI_D_INFO, "einj INJECT_ERROR_MEMORY_CE\n"));
      mWheaPlatformSupport->WheaPlatformElogMemory (
                              mWheaPlatformSupport,
                              GEN_ERR_SEV_PLATFORM_MEMORY_CORRECTED,
                              &MemInfo);
      TriggerWheaSci (mSmmBase, mSmst);

    //
    // Inject Error using DFX's
    //
    } else {
      DEBUG ((EFI_D_INFO, "einj INJECT_ERROR_MEMORY_CE\n"));

      //
      //Inject Single Bit ECC error
      //
      Status = InjectMemError (INJECT_ERROR_MEMORY_CE);
      if (EFI_ERROR(Status)) {
        mEinjParam->CmdStatus  = WHEA_EINJ_CMD_INVALID_ACCESS;
      }
    }

    break;

  case INJECT_ERROR_MEMORY_UE_NON_FATAL:
  case INJECT_ERROR_MEMORY_UE_FATAL:
    //
    // Simulate UC Memory Error Injection (Create Elog and notify OS using NMI)
    //
    if (mMemErrInjSimulated) {
      Status = mWheaPlatformSupport->WheaPlatformElogMemory (
                                       mWheaPlatformSupport,
                                       GEN_ERR_SEV_PLATFORM_MEMORY_FATAL,
                                       &MemInfo);
      TriggerWheaNmi (mSmmBase, mSmst);
    } else {
      DEBUG ((EFI_D_INFO, "einj INJECT_ERROR_MEMORY_UE_NON_FATAL\n"));
      //
      //Inject Multi bit ECC/Parity error
      //
      Status = InjectMemError (INJECT_ERROR_MEMORY_UE_NON_FATAL);
      if (EFI_ERROR(Status)) {
        mEinjParam->CmdStatus  = WHEA_EINJ_CMD_INVALID_ACCESS;
      }
    }

    break;

  case INJECT_ERROR_PCIE_CE:
  case INJECT_ERROR_PCIE_UE_NON_FATAL:
  case INJECT_ERROR_PCIE_UE_FATAL:

    DEBUG ((EFI_D_INFO, "PcieSBDF:%lx \n", PcieSBDF));

    //
    // Check BDF from EINJ table is valid or get a valid one otherwise
    //
    if (PcieSBDF) {
      PortValidatedForInjection	= ValidateRootPortForInjection (PcieSBDF);
    } else {
      PcieSBDF	= GetRootPortForInjection ();
      PortValidatedForInjection = (PcieSBDF > 0) ? TRUE : FALSE;
    }

    //
    // Try to inject PCIe Error if we could get a valid BDF
    //
    if (PortValidatedForInjection)	{

      //
      // Simulate PCIe Error Injection (Create Elog and notify OS using SCI)
      //
      if (mPCIeErrInjSimulated) {
        Status = SimulatePcieErrorInj (PcieSBDF, ErrorToInject);

      //
      // Inject PCIe Error using DFX's
      //
      } else {

        Status = InjectPciExError (PcieSBDF, ErrorToInject);
        DEBUG ((EFI_D_ERROR, "InjectPciExError Status:%lx \n", Status));
      }

      if (EFI_ERROR (Status))	{
        mEinjParam->CmdStatus  = WHEA_EINJ_CMD_INVALID_ACCESS;
      }

    } else {
      mEinjParam->CmdStatus  = WHEA_EINJ_CMD_INVALID_ACCESS;
      DEBUG ((EFI_D_ERROR, "InjectPciExError PortValidatedForInjection:%lx \n", PortValidatedForInjection));
    }

    break;

  default:
    break;

  }

  return EFI_SUCCESS;
}

VOID
EFIAPI
WheaPlatformCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
/*++

Routine Description:

  Updates the gCpuIo variable upon the availability of CPU IO Protocol.

Arguments:

  Event    - Event type
  *Context - Context for the event

Returns: 

  None

--*/
{
  EFI_STATUS                        Status;
  Status = mSmst->SmmLocateProtocol(&gEfiWheaPlatformSupportProtocolGuid, NULL, &mWheaPlatformSupport);
}

EFI_STATUS
UpdateEinjTable (
  VOID
  )
/*++

Routine Description: 

  Update Error Injection table

--*/
{

  UINT8                             *Ests;
  UINT32                            Offset;
  EFI_PEI_HOB_POINTERS              HobPtr;
  struct RAS_GLOBAL_POLICY          *RasPolicy;  
  EFI_WHEA_SUPPORT_PROTOCOL         *WheaDrv;
  EFI_STATUS                        Status;
  SYSTEM_CONFIGURATION              SetupData;
  UINTN                             VariableSize;
  EFI_GUID                          mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

  Status = gBS->LocateProtocol (&gEfiWheaSupportProtocolGuid, NULL, &WheaDrv);
  if (EFI_ERROR(Status))return Status;

  //
  // Get System configuration variables
  //
  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                            L"IntelSetup",	// Denverton AptioV Override
                            &mSystemConfigurationGuid,
                            NULL,
                            &VariableSize,
                            &SetupData
                            );
  ASSERT_EFI_ERROR (Status);

  //
  // Get PCH Policy HOB.
  //
  HobPtr.Guid   = GetFirstGuidHob (&gRasPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  RasPolicy = GET_GUID_HOB_DATA (HobPtr.Guid);

  //
  // Allocate Error status block for Error Injection - we should change this to use
  // Error status block assigned for the error source
  //
  Status = gBS->AllocatePool (EfiReservedMemoryType, 0x900, (VOID **)&Ests);
  ASSERT_EFI_ERROR (Status);

  ZeroMem (Ests, 0x900);

  //
  // Init buffer pointers and data for Einj parameter and Error log address range.
  //
  mEinjParam    = (WHEA_EINJ_PARAM_BUFFER *)Ests;
  mEinjAction   = (WHEA_EINJ_TRIGGER_ACTION_TABLE *)(Ests + ((sizeof(WHEA_EINJ_PARAM_BUFFER)+0x3F)/0x40)*0x40);
  mEinjEsts     = ((UINT8 *)mEinjAction) + ((sizeof(WHEA_EINJ_TRIGGER_ACTION_TABLE)+0x3F)/0x40)*0x40;

  //
  // Init EINJ action table in the allocated reserved memory
  // Update Error injection Action table - Static initialization is sufficient
  //
  gBS->CopyMem(mEinjAction, &mSimEinjAction, sizeof(WHEA_EINJ_TRIGGER_ACTION_TABLE));

  //
  // Allocate memory for 6000h bytes for Error injection structure.
  //
  Status = gBS->AllocatePool (EfiReservedMemoryType, WHEA_EINJ_ADDR_RANGE, &mBiosRsvdInjAddrBase);
  ZeroMem(mBiosRsvdInjAddrBase,WHEA_EINJ_ADDR_RANGE);

  //
  // Make it cache line boundary - This is the incremental injection address Bios uses for error injection
  //
  DEBUG ((EFI_D_ERROR, "einj mBiosRsvdInjAddrBase 1 :%lx \n", (UINTN)mBiosRsvdInjAddrBase));

  mBiosRsvdInjAddr  = (UINTN)ALIGN_VALUE((UINTN)mBiosRsvdInjAddrBase + CACHE_LINE_SIZE,CACHE_LINE_SIZE);
  DEBUG ((EFI_D_INFO, "einj mBiosRsvdInjAddr  :%lx \n", mBiosRsvdInjAddr));
  mBiosRsvdInjAddrBase = (UINT8 *)mBiosRsvdInjAddr;
  DEBUG ((EFI_D_INFO, "einj mBiosRsvdInjAddrBase 2  :%lx \n", (UINTN)mBiosRsvdInjAddrBase));

  // Check Acpi 5.0 support enabled.
  if (RasPolicy->WheaEinj_05Extn)
    mAcpi5Support  = TRUE;

  //
  // Init EINJ parameters
  //
  mEinjParam->TriggerActionTable = (UINT64) (UINTN)mEinjAction;

  if (SetupData.RelaxSecConf) {
    mEinjParam->ErrorInjectCap = INJECT_ERROR_MEMORY_UE_NON_FATAL | INJECT_ERROR_MEMORY_CE | INJECT_ERROR_MEMORY_UE_FATAL;
    mEinjParam->ErrorInjectCap |= INJECT_ERROR_PCIE_CE  | INJECT_ERROR_PCIE_UE_NON_FATAL | INJECT_ERROR_PCIE_UE_FATAL;
  } else {
    return EFI_UNSUPPORTED;
  }

  mEinjParam->ErrorToInject[3]      = 0;
  mEinjParam->ErrorToInject[4]      = 0;
  mEinjParam->OpState               = 0;
  mEinjParam->OpStatus              = 0;
  mEinjParam->CmdStatus             = 0;
  mEinjParam->ErrorToInject[0]      = 0;
  mEinjParam->EinjAddr              = 0;
  mEinjParam->AddrMask              = 0;


  //
  // Update Error injection serialization informations
  //
  mSimEinj[0].Register.Address = (UINT64) (UINTN)&mEinjParam->OpState;              // Update address for BEGIN_OPERATION
  mSimEinj[1].Register.Address = (UINT64) (UINTN)&mEinjParam->TriggerActionTable;   // Update address for GET_TRIGGER_TABLE
  mSimEinj[2].Register.Address = (UINT64) (UINTN)&mEinjParam->ErrorToInject[0];     // Update address for SET_ERROR
  mSimEinj[3].Register.Address = (UINT64) (UINTN)&mEinjParam->ErrorInjectCap;       // Update address for GET_ERROR_CAP
  mSimEinj[4].Register.Address = (UINT64) (UINTN)&mEinjParam->OpState;              // Update address for END_OPERATION
  mSimEinj[6].Register.Address = (UINT64) (UINTN)&mEinjParam->OpStatus;             // Update address for CHECK_BUSY_STATUS
  mSimEinj[7].Register.Address = (UINT64) (UINTN)&mEinjParam->CmdStatus;            // Update address for GET_CMD_STATUS

  if(mAcpi5Support == TRUE)  {
    mSimEinj[8].Register.Address = (UINT64) (UINTN)&mEinjParam->SetAddressTable;    // Update address for SET_ERROR_TYPE_WITH_ADDRESS
  
    Offset = (UINT32)((UINTN)&mEinjParam->VendorExt - (UINTN)&mEinjParam->SetAddressTable);
    mEinjParam->SetAddressTable.VendorErrTypeExtOffset = Offset;
    mEinjParam->SetAddressTable.PcieSBDF  = 00;
  
    mEinjParam->VendorExt.Length		= sizeof (WHEA_EINJ_VENDOR_EXT);
    mEinjParam->VendorExt.SBDF			= 00;
  }

  //
  // Register core Whea Error injection method
  //
  Status = WheaDrv->InstallErrorInjectionMethod (
             WheaDrv,
             INJECT_ERROR_PROCESSOR_UE_NON_FATAL | INJECT_ERROR_MEMORY_UE_NON_FATAL | INJECT_ERROR_MEMORY_CE,
             00,         // WheaSupport.c file updates the entries.
             &mSimEinj[0]
             );

  ASSERT_EFI_ERROR (Status);  

  return Status;
}

EFI_STATUS
InitWheaErrorInj (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description: 

  Entry point of the Whea Error Injection driver. 

Arguments:  

  ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table        

Returns:  

  EFI_SUCCESS:              Driver initialized successfully
  EFI_LOAD_ERROR:           Failed to Initialize or has been loaded 
  EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

--*/
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        SwHandle;
  EFI_SMM_SW_REGISTER_CONTEXT       SwContext;
  EFI_SMM_SW_DISPATCH2_PROTOCOL     *SwDispatch  = 0;
  BOOLEAN                           InSmm;

  //
  // Retrieve SMM Base Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  &mSmmBase
                  );

  if (mSmmBase == NULL) {
    InSmm = FALSE;
  } else {
    mSmmBase->InSmm (mSmmBase, &InSmm);
    mSmmBase->GetSmstLocation (mSmmBase, &mSmst);
  }

  if (InSmm) {

    //
    // Update EINJ table entries.
    //
    Status = UpdateEinjTable();
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // Register our SMI handlers
    //
    Status = mSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
    ASSERT_EFI_ERROR (Status);

    //
    // Install SW SMI handler to build error data for simulated error injection
    //
    SwContext.SwSmiInputValue = EFI_WHEA_EINJ_EXECUTE_SWSMI;
    Status = SwDispatch->Register (SwDispatch, WheaEinjHandler, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);

    //
    // Find Whea Platform protocol instance
    //
    mWheaPlatformSupport = NULL;

    Status = mSmst->SmmLocateProtocol(&gEfiWheaPlatformSupportProtocolGuid, NULL, &mWheaPlatformSupport);

    if (Status != EFI_SUCCESS) {

    
    //
    // Register for protocol notifications on this event
    //
    Status = mSmst->SmmRegisterProtocolNotify(
                    &gEfiWheaPlatformSupportProtocolGuid,
                    (EFI_SMM_NOTIFY_FN)WheaPlatformCallback,
                    &mWheaEventReg
                    );
    ASSERT_EFI_ERROR (Status);

    }
  }

  return EFI_SUCCESS;
}

