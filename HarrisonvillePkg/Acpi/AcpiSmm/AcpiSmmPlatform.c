/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c)  2009 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  AcpiSmmPlatform.c
    
Abstract:

  ACPISMM Driver implementation file. 

--*/

#include <AcpiSmmPlatform.h>
#include <Guid/AmiGlobalVariable.h> // Denverton AptioV Override - EIP#221424

#define MAX_SMRAM_RANGES 2
#define RESERVED_ACPI_S3_VARIABLE_SET_OFFSET (EFI_PAGE_SIZE - sizeof (ACPI_VARIABLE_SET))

//
// Modular variables needed by this driver
//
EFI_ACPI_SMM_DEV            mAcpiSmm;
EFI_SMM_SYSTEM_TABLE2       *mSmst2;
EFI_SMM_BASE2_PROTOCOL      *mSmmBase2;
EFI_SMM_VARIABLE_PROTOCOL   *mSmmVariable;

/**
  Allocate EfiACPIMemoryNVS below 4G memory address.

  This function allocates EfiACPIMemoryNVS below 4G memory address.

  @param Size   Size of memory to allocate.

  @return       Allocated address for output.

**/
VOID*
AllocateAcpiNvsMemoryBelow4G (
  IN UINTN  Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID*                 Buffer;

  Pages = EFI_SIZE_TO_PAGES (Size);
  Address = 0xffffffff;

  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   Pages,
                   &Address
                   );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Buffer = (VOID *) (UINTN) Address;
  ZeroMem (Buffer, Size);

  return Buffer;
}

EFI_STATUS
EFIAPI
ReserveS3Memory (
  ) 
/*++

Routine Description:

  Reserved S3 memory for InstallS3Memory

Arguments:


Returns:

  EFI_OUT_OF_RESOURCES  -  Insufficient resources to complete function.
  EFI_SUCCESS           -  Function has completed successfully.

--*/
{

  VOID                                      *GuidHob;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK            *DescriptorBlock;
  VOID                                      *AcpiReservedBase; 
  UINTN                                     TsegIndex;
  UINTN                                     TsegSize;
  UINTN                                     TsegBase;
  ACPI_VARIABLE_SET                         *AcpiVariableSet;

  //
  // Get Hob list for SMRAM desc
  //
  GuidHob    = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  ASSERT (GuidHob);
  DescriptorBlock = GET_GUID_HOB_DATA (GuidHob);
  ASSERT (DescriptorBlock);
    
  //
  // Use the hob to get SMRAM capabilities
  //  
  TsegIndex = DescriptorBlock->NumberOfSmmReservedRegions - 1;
  ASSERT (TsegIndex <= (MAX_SMRAM_RANGES - 1));  
  TsegBase  = (UINTN)DescriptorBlock->Descriptor[TsegIndex].PhysicalStart;
  TsegSize  = (UINTN)DescriptorBlock->Descriptor[TsegIndex].PhysicalSize;
  
  DEBUG ((EFI_D_ERROR, "SMM  Base: %08X\n", TsegBase));
  DEBUG ((EFI_D_ERROR, "SMM  Size: %08X\n", TsegSize));

  //
  // Now find the location of the data structure that is used to store the address
  // of the S3 reserved memory.
  //
  AcpiVariableSet = (ACPI_VARIABLE_SET*) (UINTN) (TsegBase + RESERVED_ACPI_S3_VARIABLE_SET_OFFSET);

  //
  // Allocate reserved ACPI memory for S3 resume.  Pointer to this region is
  // stored in SMRAM in the first page of TSEG.
  //
  AcpiReservedBase = AllocateAcpiNvsMemoryBelow4G (0x10000);//(PcdGet32 (PcdS3AcpiReservedMemorySize));
  if (AcpiReservedBase != NULL) {
    AcpiVariableSet->AcpiReservedMemoryBase = (UINT32)(UINTN) AcpiReservedBase;
    AcpiVariableSet->AcpiReservedMemorySize = 0x10000; //PcdGet32 (PcdS3AcpiReservedMemorySize);
  }
  
  DEBUG ((EFI_D_ERROR, "S3 Memory  Base:    %08X\n", AcpiVariableSet->AcpiReservedMemoryBase));
  DEBUG ((EFI_D_ERROR, "S3 Memory  Size:    %08X\n", AcpiVariableSet->AcpiReservedMemorySize));

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
InitAcpiSmmPlatform (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:

  Initializes the SMM S3 Handler Driver.

Arguments:

  ImageHandle  -  The image handle of Wake On Lan driver.
  SystemTable  -  The starndard EFI system table.
    
Returns:

  EFI_OUT_OF_RESOURCES  -  Insufficient resources to complete function.
  EFI_SUCCESS           -  Function has completed successfully.
  Other                 -  Error occured during execution. 

--*/
{
  EFI_STATUS                      Status;
  EFI_GLOBAL_NVS_AREA_PROTOCOL    *AcpiNvsProtocol = NULL;
  // Denverton AptioV Override Start - EIP#174553
  UINTN                            VarSize;
  UINT32                           Data;
  UINT32                           Attributes;
  // Denverton AptioV Override End - EIP#174553
  Status = gBS->LocateProtocol (
    &gEfiGlobalNvsAreaProtocolGuid, 
    NULL, 
    &AcpiNvsProtocol
    );
  ASSERT_EFI_ERROR (Status);
   
  //
  // Locate SmmVariableProtocol.
  //
  Status = gSmst->SmmLocateProtocol (   // Denverton AptioV Override - EIP#221424 
    &gEfiSmmVariableProtocolGuid, 
    NULL, 
    (VOID**)&mSmmVariable
    );
  ASSERT_EFI_ERROR (Status);
   
  mAcpiSmm.BootScriptSaved  = 0;

  ReserveS3Memory();

  //
  // Locate and Register to Parent driver
  //
  Status = RegisterToDispatchDriver ();
  ASSERT_EFI_ERROR (Status);

//    Status = InitializeSmmScriptLib (SystemTable, 2, &(mAcpiSmm.RuntimeScriptTableBase));
//    if (EFI_ERROR (Status)) {
//      return Status;
//    }

//  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID**) &mSmmBase2);
//  mSmmBase2->GetSmstLocation (mSmmBase2, &mSmst2); 
  // Denverton AptioV Override Start - EIP#174553
  //
  // Delete old Patrol scrub variable if present.
  //
  VarSize = sizeof(Data);
  
  Status = gRT->GetVariable (
    PATROL_SCRUB_ADDR_VAR_NAME,
    &gAmiGlobalVariableGuid,
    &Attributes,
    &VarSize,
    &Data
    );
	
  if (!EFI_ERROR(Status)){
  Status = gRT->SetVariable (
    PATROL_SCRUB_ADDR_VAR_NAME,
    &gAmiGlobalVariableGuid, // Denverton AptioV Override - EIP#221424
    //EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
	Attributes,
    0,
    NULL
    );
  }
  // Denverton AptioV Override End - EIP#174553
  return EFI_SUCCESS;
}

EFI_STATUS
EnableAhciPort0Callback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  )
{
  UINTN                                    CpuIndex;
  EFI_SMM_CPU_STATE                        **CpuState;

  CpuIndex = mSmst2->CurrentlyExecutingCpu;
  DEBUG ((EFI_D_ERROR,"Get AHCI port 0! %x  mSmst2:%x.\n",CpuIndex,mSmst2));
  CpuState = (EFI_SMM_CPU_STATE **)mSmst2->CpuSaveState;

//  CpuState[CpuIndex]->x86._EBX = 1;

  return EFI_SUCCESS;
  
}

EFI_STATUS
EnableAhciPort1Callback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  )
{
  UINTN                                    CpuIndex;
  EFI_SMM_CPU_STATE                        **CpuState;

  CpuIndex = mSmst2->CurrentlyExecutingCpu;
  DEBUG ((EFI_D_ERROR,"Get AHCI port 1! %x  mSmst2:%x.\n",CpuIndex,mSmst2));
  CpuState = (EFI_SMM_CPU_STATE **)mSmst2->CpuSaveState;

//  CpuState[CpuIndex]->x86._EBX = 1;

  return EFI_SUCCESS;
}

EFI_STATUS
RegisterToDispatchDriver (
  VOID
  )
/*++

Routine Description:

  Register to dispatch driver.
  
Arguments:

  None.
      
Returns:

  EFI_SUCCESS  -  Successfully init the device.
  Other        -  Error occured whening calling Dxe lib functions.
  
--*/
{
  UINTN                         Length;
  EFI_STATUS                    Status;

  EFI_SMM_SX_DISPATCH2_PROTOCOL  *SxDispatch;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch;
  EFI_SMM_SX_REGISTER_CONTEXT   *EntryDispatchContext;
  EFI_SMM_SX_REGISTER_CONTEXT   *EntryS1DispatchContext;
  EFI_SMM_SX_REGISTER_CONTEXT   *EntryS3DispatchContext;
  EFI_SMM_SX_REGISTER_CONTEXT   *EntryS4DispatchContext;
  EFI_SMM_SX_REGISTER_CONTEXT   *EntryS5DispatchContext;
  // Denverton Aptiov Override start - EIP#221424
  // EFI_SMM_SW_REGISTER_CONTEXT   *SwContext;
  // EFI_SMM_SW_REGISTER_CONTEXT   *AcpiDisableSwContext;
  // EFI_SMM_SW_REGISTER_CONTEXT   *AcpiEnableSwContext;
  // Denverton Aptiov Override End - EIP#221424

  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmSxDispatch2ProtocolGuid,
                  NULL,
                  &SxDispatch
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmSwDispatch2ProtocolGuid,
                  NULL,
                  &SwDispatch
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Length = sizeof (EFI_SMM_SX_REGISTER_CONTEXT) * 4 + sizeof (EFI_SMM_SW_REGISTER_CONTEXT) * 2;
  Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      Length,
                      &EntryDispatchContext
                      );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetMem (EntryDispatchContext, Length, 0);

  EntryS1DispatchContext  = EntryDispatchContext++;
  EntryS3DispatchContext  = EntryDispatchContext++;
  EntryS4DispatchContext  = EntryDispatchContext++;
  EntryS5DispatchContext  = EntryDispatchContext++;

  // Denverton Aptiov Override start - EIP#221424
  // ACPI mode enable is being done using AMI module
  /*
  SwContext = (EFI_SMM_SW_REGISTER_CONTEXT *)EntryDispatchContext;
  AcpiDisableSwContext = SwContext++;
  AcpiEnableSwContext  = SwContext++;

  //
  // Register the enable handler
  //
  AcpiEnableSwContext->SwSmiInputValue = ACPI_ENABLE;
  Status = SwDispatch->Register (
                        SwDispatch,
                        EnableAcpiCallback,
                        AcpiEnableSwContext,
                        &(mAcpiSmm.DisableAcpiHandle)
                        );

  //
  // Register the disable handler
  //
  AcpiDisableSwContext->SwSmiInputValue = ACPI_DISABLE;
  Status = SwDispatch->Register (
                        SwDispatch,
                        DisableAcpiCallback,
                        AcpiDisableSwContext,
                        &(mAcpiSmm.EnableAcpiHandle)
                        );
  */
  // Denverton Aptiov Override End - EIP#221424
  //
  // Register entry phase call back function for S1
  //
  EntryS1DispatchContext->Type  = SxS1;
  EntryS1DispatchContext->Phase = SxEntry;
  Status = SxDispatch->Register (
                        SxDispatch,
                        SxSleepEntryCallBack,
                        EntryS1DispatchContext,
                        &(mAcpiSmm.S1SleepEntryHandle)
                        );

  //
  // Register entry phase call back function
  //
  EntryS3DispatchContext->Type  = SxS3;
  EntryS3DispatchContext->Phase = SxEntry;
  Status = SxDispatch->Register (
                        SxDispatch,
                        SxSleepEntryCallBack,
                        EntryS3DispatchContext,
                        &(mAcpiSmm.S3SleepEntryHandle)
                        );

  //
  // Register entry phase call back function for S4
  //
  EntryS4DispatchContext->Type  = SxS4;
  EntryS4DispatchContext->Phase = SxEntry;
  Status = SxDispatch->Register (
                        SxDispatch,
                        SxSleepEntryCallBack,
                        EntryS4DispatchContext,
                        &(mAcpiSmm.S4SleepEntryHandle)
                        );

  //
  // Register callback for S5 in order to workaround the LAN shutdown issue
  //
  EntryS5DispatchContext->Type  = SxS5;
  EntryS5DispatchContext->Phase = SxEntry;
  Status = SxDispatch->Register (
                        SxDispatch,
                        SxSleepEntryCallBack,
                        EntryS5DispatchContext,
                        &(mAcpiSmm.S5SoftOffEntryHandle)
                        );

  return Status;
}

EFI_STATUS
DisableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  )
/*++

Routine Description:
  SMI handler to disable ACPI mode

  Dispatched on reads from APM port with value 0xA1

  ACPI events are disabled and ACPI event status is cleared.  
  SCI mode is then disabled.
   Clear all ACPI event status and disable all ACPI events
   Disable PM sources except power button
   Clear status bits
   Disable GPE0 sources
   Clear status bits
   Disable GPE1 sources
   Clear status bits
   Disable SCI

Arguments:
  DispatchHandle  - EFI Handle
  DispatchContext - Pointer to the EFI_SMM_SW_REGISTER_CONTEXT

Returns:
  Nothing

--*/
{
  //
  // Disable SCI
  //
  IoAnd32 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_CNT, (UINT32)(~B_PCH_ACPI_PM1_CNT_SCI_EN));
  return EFI_SUCCESS;  
}

EFI_STATUS
EnableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  )

/*++

Routine Description:
  SMI handler to enable ACPI mode
  
  Dispatched on reads from APM port with value 0xA0

  Disables the SW SMI Timer.  
  ACPI events are disabled and ACPI event status is cleared.  
  SCI mode is then enabled.

   Disable SW SMI Timer

   Clear all ACPI event status and disable all ACPI events
   Disable PM sources except power button
   Clear status bits

   Disable GPE0 sources
   Clear status bits

   Disable GPE1 sources
   Clear status bits

   Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)

   Enable SCI

Arguments:
  DispatchHandle  - EFI Handle
  DispatchContext - Pointer to the EFI_SMM_SW_REGISTER_CONTEXT

Returns:
  Nothing

--*/
{
  UINT32 Data32;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  //
  // Disable SW SMI Timer
  //
  Data32 = IoRead32 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_SMI_EN);
  Data32 &= B_PCH_SMI_EN_INTEL_USB2 |
     		B_PCH_SMI_EN_LEGACY_USB2 |
    		B_PCH_SMI_EN_APMC |
    		B_PCH_SMI_EN_ON_SLP_EN |
    		B_PCH_SMI_EN_GBL_SMI;
  IoWrite32(PcdGet16 (PcdAcpiBaseAddress) + R_PCH_SMI_EN, Data32);

  //
  // Disable PM sources except power button
  //
  IoWrite16 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_EN, 0x0000);

  //
  // Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_D);
  IoWrite8 (PCAT_RTC_DATA_REGISTER, 0);

  //
  // Enable SCI
  //
  IoOr32 (PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_CNT, B_PCH_ACPI_PM1_CNT_SCI_EN);  //
  // Do platform specific stuff for ACPI enable SMI
  //

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

#define MAX_BOXTYPE_INSTANCES 2
#define NA                    0xFF
#ifndef TRUE
#define TRUE  ((BOOLEAN) 1 == 1)
#endif
#define EC_BASE   0xE0000000
#define VOLATILE  volatile
#define S3        5<<10
#define SLP_TYP   0x1c00

typedef struct {
  UINT8 Type;
  UINT8 MaxInstances;
  UINT8 Instance[MAX_BOXTYPE_INSTANCES];
  UINT8 PortId;
  UINT8 ReadOperation;
  UINT8 WriteOperation;
  UINT8 SimulationSupported;
} INSTANCE_PORT_MAP;

typedef enum {
  SidebandPort,
  AddressOffset
} INSTANCE_OFFSET_TYPE;

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
  ModeWrite
} ACCESS_MODE;

//
// Add more units here if they need to be read/written by MRC.
//
typedef enum {
  DUNIT,
  CPGC,
  DDRIO,
  DDRIO_DQ,
  DDRIO_CMD,
  DDRIO_CLKCTL,
  DDRIO_COMP,
  DDRIO_PLL,
  BUNIT,
  TUNIT,
  PUNIT,
  AUNIT,
  CUNIT,
  RTF,
  DRNG,
  CCK
} CPU_BOX_TYPE;

#define BDPSCRUB_REG 0x0002017A

/* Struct format extracted from XML file ..\..\..\data\xml\ConfigDB-04132012_08_49\ssa.xml.
 * Bunit DRAM Patrol Scrub Engine Register (BDPSCRUB):
 * This register provides the ability to control patrol scrub operations.
 */
typedef union {
  struct {
    /* pscrben - Bits[0:0], RW, default = 0h
       Patrol scrub engine enable field:

     */
    UINT32 pscrben : 1;

    /* state - Bits[3:1], RO, default = 0h
       Current patrol scrub engine state:

     */
    UINT32 state : 3;

    /* scrubper - Bits[15:4], RW, default = 284h
       Scrub Period:

     */
    UINT32 scrubper : 12;

    /* scrben - Bits[16:16], RW, default = 0h
       Scrub enable field:

     */
    UINT32 scrben : 1;

    /* reserved_0 - Bits[31:17], RO, default = 0h
       Reserved
     */
    UINT32 reserved_0 : 15;

  } Bits;
  UINT32 Data;
} BDPSCRUB_STRUCT;


#define BDPSADDR_REG 0x0002017B

/* Struct format extracted from XML file ..\..\..\data\xml\ConfigDB-04132012_08_49\ssa.xml.
 * Bunit DRAM Patrol Scrub Engine Address Register (BDPSADDR):
 *
 */
typedef union {
  struct {
    /* reserved_1 - Bits[1:0], RO, default = 0h
       Reserved
     */
    UINT32 reserved_1 : 2;

    /* scrbaddr - Bits[31:2], RW, default = 0h
       Patrol scrub address:

     */
    UINT32 scrbaddr : 30;

  } Bits;
  UINT32 Data;
} BDPSADDR_STRUCT;

#define SSKPD3_DUNIT_REG 0x0002004B

//
// The ordering of these MUST match the ordering in the CPU_BOX_TYPE enumerated type.
//
INSTANCE_PORT_MAP InstancePortMap[] = {
  //SidebandPort/Off  # of Instances Instance0 Instance1     Port     R-Op     W-Op  SimSupported
  {SidebandPort,            2,         0x10,     0x13,         NA,    0x10,    0x11,   TRUE},   // DUNIT
  {SidebandPort,            2,         0x11,     0x14,         NA,    0x10,    0x11,   TRUE},   // CPGC
  {SidebandPort,            2,         0x12,     0x15,         NA,    0x06,    0x07,   TRUE},   // DDRIO
  {SidebandPort,            2,         0x12,     0x15,         NA,    0x06,    0x07,   TRUE},   // DDRIO_DQ
  {SidebandPort,            2,         0x12,     0x15,         NA,    0x06,    0x07,   TRUE},   // DDRIO_CMD
  {SidebandPort,            2,         0x12,     0x15,         NA,    0x06,    0x07,   TRUE},   // DDRIO_CLKCTL
  {SidebandPort,            2,         0x12,     0x15,         NA,    0x06,    0x07,   TRUE},   // DDRIO_COMP
  {SidebandPort,            2,         0x12,     0x15,         NA,    0x06,    0x07,   TRUE},   // DDRIO_PLL
  {SidebandPort,            1,         0x03,      NA,          NA,    0x10,    0x11,   FALSE},  // BUNIT
  {SidebandPort,            1,         0x02,      NA,          NA,    0x10,    0x11,   FALSE},  // TUNIT
  {SidebandPort,            1,         0x04,      NA,          NA,    0x06,    0x07,   FALSE},  // PUNIT
  {SidebandPort,            1,         0x00,      NA,          NA,    0x10,    0x11,   FALSE},  // AUNIT
  {SidebandPort,            1,         0x08,      NA,          NA,    0x10,    0x11,   FALSE},  // CUNIT
  {SidebandPort,            1,         0x06,      NA,          NA,    0x10,    0x11,   FALSE},  // RTF
  {SidebandPort,            1,         0x0F,      NA,          NA,    0x10,    0x11,   FALSE},  // DRNG
  {SidebandPort,            1,         0x16,      NA,          NA,    0x06,    0x07,    FALSE},  // CCK
};



UINT32
GetRegisterAccessInfo (
  IN        UINT8       MrcDebugMsgLevel,
  IN        UINT8       BoxType,
  IN        UINT8       Instance,
  IN  OUT   UINT32      *Command,
  IN  OUT   UINT8       *PortId,
  IN  OUT   UINT32      *Offset,
  IN        UINT32      Mode
)
{
  UINT32 Error;
  Error = NoError;

  //
  // Get only the low word of the offset since the registers currently have meta-information
  // stored in the upper 16 bits which we are not using at present.
  //
  *Offset &= 0x0000FFFF;
  //
  // Look at the type, which is either a sideband port or an address offset for this box type.
  // If it's a port, assign it here. If it's an address offset, do the appropriate math to
  // target the Instance passed in.
    //
  if (InstancePortMap[BoxType].Type == SidebandPort) {
    *PortId  = InstancePortMap[BoxType].Instance[Instance];
  } else if (InstancePortMap[BoxType].Type == AddressOffset) {
    *PortId  = InstancePortMap[BoxType].PortId;
    *Offset += InstancePortMap[BoxType].Instance[Instance];
    } else {
    Error = InvalidRemapType;
    }
    //
  // Get the sideband command to read this particular box type.
    //
    if (Mode == ModeRead) {
    *Command = InstancePortMap[BoxType].ReadOperation  << 24;
    } else if (Mode == ModeWrite) {
    *Command = InstancePortMap[BoxType].WriteOperation << 24;
    } else {
      Error = InvalidMode;
    }
    //
  // If we've requested a register access to an instance that is
  // not valid (i.e. CH3 DDRIO register on a 2CH platform), flag
  // an error.
    //
  if (Instance >= InstancePortMap[BoxType].MaxInstances) {
      Error = InvalidInstance;
    }
  //
  // Handle all errors.
  //
  if (Error != NoError) {
    if (Mode == ModeRead) {
      //rcPrintf (MrcDebugMsgLevel, SDBG_MIN, "MemRegRead ERROR: ");
    } else if (Mode == ModeWrite) {
      //rcPrintf (MrcDebugMsgLevel, SDBG_MIN, "MemRegWrite ERROR: ");
    } else {
      //rcPrintf (MrcDebugMsgLevel, SDBG_MIN, "Unknown Mode (0x%x) in register access functions.\n", Mode);
    }
  }
  switch (Error) {
  case NoError:
    break;
  case InvalidInstance:
    //rcPrintf (MrcDebugMsgLevel, SDBG_MIN, "Invalid Instance 0x%x for BoxType 0x%d (offset = 0x%x)\n", Instance, BoxType, *Offset);
    break;
  case InvalidBoxType:
    //rcPrintf (MrcDebugMsgLevel, SDBG_MIN, "Unknown box type (0x%x) (offset = 0x%x)\n", BoxType, *Offset);
    break;
  case BoxTypeNotInSimulation:
    //rcPrintf (MrcDebugMsgLevel, SDBG_MIN, "Box type (0x%x) is not in simulation environment (offset = 0x%x)\n", BoxType, *Offset);
    break;
  case InvalidRemapType:
    //rcPrintf (MrcDebugMsgLevel, SDBG_MIN, "Invalid remap type %d\n", InstancePortMap[BoxType].Type);
    break;
  default:
    break;
  }
  return Error;
}


#pragma warning( push )
#pragma warning( disable : 4306 )

VOID
Mmio32Write (
  IN        UINT32      RegisterAddress,
  IN        UINT32      Value
)
{
  
  VOLATILE UINT32 *addr = (UINT32 *)RegisterAddress;
  *addr = Value;
}

UINT32
Mmio32Read (
  IN        UINT32      RegisterAddress
)
{
  VOLATILE UINT32 *addr = (UINT32 *)RegisterAddress;
  return *addr;
}
#pragma warning( pop )

UINT32
MemRegRead (
  IN        UINT8       MrcDebugMsgLevel,
  IN        UINT8       BoxType,
  IN        UINT8       Instance,
  IN        UINT32      Offset
)
/*++

Routine Description:

  Reads registers from an specified Unit

Arguments:

  MrcDebugMsgLevel: Message level at which the current MRC run is operating
  Boxtype:          Unit to select
  Instance:         Channel under test
  Offset:           Offset of register to read.

Returns:

  Value read

--*/
{
  UINT32 Command;
  UINT8  PortId;
  UINT32 Data;

  if (GetRegisterAccessInfo (MrcDebugMsgLevel, BoxType, Instance, &Command, &PortId, &Offset, ModeRead) == NoError) {
    //
    // We have the ability to force register reads in simulation by bypassing the sideband and going
    // straight to the RTL signals. To enable, SIM_REGISTER_ACCESS_BYPASS_SIDEBAND must be enabled.
    // In addition, on the simulation run command line, the following argument must be specified:
    // -simv_args +DDR_MRC_FAST
    //
    Mmio32Write (EC_BASE + 0xD8, Offset & 0xFFFFFF00);
    Mmio32Write (EC_BASE + 0xD0, (Command | (PortId << 16) | ( (Offset & 0x000000FF) << 8) + 0xF0) );
    Data = Mmio32Read (EC_BASE + 0xD4);

    //rcPrintf (MrcDebugMsgLevel, SDBG_TRACE, "MsgBusRead  to port=0x%02x; Opcode=0x%02x; offset=0x%08x; data=0x%08x\n", PortId, InstancePortMap[BoxType].ReadOperation, Offset, Data);
    return Data;
  }
  return 0xFFFFFFFF;
}


VOID
MemRegWrite (
  IN        UINT8       MrcDebugMsgLevel,
  IN        UINT8       BoxType,
  IN        UINT8       Instance,
  IN        UINT32      Offset,
  IN        UINT32      Data
)
/*++

Routine Description:

  Reads registers from an specified Unit

Arguments:

  MrcDebugMsgLevel: Message level at which the current MRC run is operating
  Boxtype:          Unit to select
  Instance:         Channel under test
  Offset:           Offset of register to write.
  Data:             Data to be written

Returns:

  None

--*/
{
  UINT32 Command;
  UINT8  PortId;

  if (GetRegisterAccessInfo (MrcDebugMsgLevel, BoxType, Instance, &Command, &PortId, &Offset, ModeWrite) == NoError) {
    //
    // We have the ability to force register writes in simulation by bypassing the sideband and going
    // straight to the RTL signals. To enable, SIM_REGISTER_ACCESS_BYPASS_SIDEBAND must be enabled.
    // In addition, on the simulation run command line, the following argument must be specified:
    // -simv_args +DDR_MRC_FAST
    //
    Mmio32Write (EC_BASE + 0xD8, Offset & 0xFFFFFF00);
    Mmio32Write (EC_BASE + 0xD4, Data);
    Mmio32Write (EC_BASE + 0xD0, (Command | (PortId << 16) | ( (Offset & 0x000000FF) << 8) + 0xF0) );

    //rcPrintf (MrcDebugMsgLevel, SDBG_TRACE, "MsgBusWrite to port=0x%02x; Opcode=0x%02x; offset=0x%08x; data=0x%08x\n", PortId, InstancePortMap[BoxType].WriteOperation, Offset, Data);
  }
}

EFI_STATUS
SxSleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  )

/*++

Routine Description:

  Callback function entry for Sx sleep state.

Arguments:

  DispatchHandle   -  The handle of this callback, obtained when registering.
  DispatchContext  -  The predefined context which contained sleep type and phase.
    
Returns:

  EFI_SUCCESS            -  Operation successfully performed.
  EFI_INVALID_PARAMETER  -  Invalid parameter passed in.
  
--*/
{
/*
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeS3SuspendStart));

  //
  // Handling S1 - setting appropriate wake bits in GPE0_EN
  //
  if ((DispatchHandle == mAcpiSmm.S1SleepEntryHandle) && (((EFI_SMM_SX_REGISTER_CONTEXT *)DispatchContext)->Type == SxS1)) {
  }

  //
  // Handling S4, S5 and WOL - setting appropriate wake bits in GPE0_EN
  //
  if (((DispatchHandle == mAcpiSmm.S4SleepEntryHandle) && (((EFI_SMM_SX_REGISTER_CONTEXT *)DispatchContext)->Type == SxS4)) ||
      ((DispatchHandle == mAcpiSmm.S5SoftOffEntryHandle) && (((EFI_SMM_SX_REGISTER_CONTEXT *)DispatchContext)->Type == SxS5))
     ) {
  }

  if ((DispatchHandle != mAcpiSmm.S3SleepEntryHandle) || (((EFI_SMM_SX_REGISTER_CONTEXT *)DispatchContext)->Type != SxS3)) {
    return EFI_INVALID_PARAMETER;
  }

  if (mAcpiSmm.BootScriptSaved == 0) {
  }
*/
  UINT16 Pm1_Cnt = 0;
  BDPSCRUB_STRUCT       PatrolScrubReg;
  BDPSADDR_STRUCT       PatrolScrubAddr;
  UINT32                Attribute;
  UINT16                PmBase = 0;  // Denverton AptioV Override  - EIP#302008

  Pm1_Cnt = IoRead16(PcdGet16 (PcdAcpiBaseAddress) + R_PCH_ACPI_PM1_CNT);
  
  if ( (Pm1_Cnt&SLP_TYP) == S3 ) {

    PatrolScrubReg.Data = MemRegRead (1, BUNIT, 0, BDPSCRUB_REG);
    if (PatrolScrubReg.Bits.pscrben) {
      PatrolScrubAddr.Data = MemRegRead (1, BUNIT, 0, BDPSADDR_REG);

      //
      // Save Patrol Scrub Address in a NV Variable for restoring on S3 resume.
      //
      Attribute = (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS);
      mSmmVariable->SmmSetVariable (
        PATROL_SCRUB_ADDR_VAR_NAME, 
        &gAmiGlobalVariableGuid,  // Denverton AptioV Override - EIP#221424
        Attribute, 
        sizeof(PatrolScrubAddr.Data), 
        &PatrolScrubAddr.Data
        );
    }
  }

  // Denverton AptioV Override Start - EIP#302008
  // Add setting of RTC_EN bit.
  PmBase = PcdGet16 (PcdAcpiBaseAddress);
  IoOr16 (PmBase + R_PCH_ACPI_PM1_EN, B_PCH_ACPI_PM1_EN_RTC);
  // Denverton AptioV Override End - EIP#302008

  return EFI_SUCCESS;
}

