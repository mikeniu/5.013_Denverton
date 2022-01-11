/** @file
  PCH Smbus Driver

@copyright
 Copyright (c) 2004 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#include "PchSmbus.h"
#include <Library/PciExpressLib.h>

extern EFI_GUID gEfiSmbusArpMapGuid;

/**
  Execute an SMBUS operation

  @param[in] This                 The protocol instance
  @param[in] SlaveAddress         The address of the SMBUS slave device
  @param[in] Command              The SMBUS command
  @param[in] Operation            Which SMBus protocol will be issued
  @param[in] PecCheck             If Packet Error Code Checking is to be used
  @param[in, out] Length          Length of data
  @param[in, out] Buffer          Data buffer

  @retval EFI_SUCCESS             The SMBUS operation is successful
  @retval Other Values            Something error occurred
**/
EFI_STATUS
EFIAPI
SmbusExecute (
  IN      CONST EFI_SMBUS_HC_PROTOCOL   *This,
  IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN      EFI_SMBUS_DEVICE_COMMAND      Command,
  IN      EFI_SMBUS_OPERATION           Operation,
  IN      BOOLEAN                       PecCheck,
  IN OUT  UINTN                         *Length,
  IN OUT  VOID                          *Buffer
  )
{
  DEBUG ((DEBUG_INFO, "SmbusExecute() Start, SmbusDeviceAddress=%x, Command=%x, Operation=%x\n", (SlaveAddress.SmbusDeviceAddress << 1), Command, Operation));
  InitializeSmbusRegisters ();

  return SmbusExec (
          SlaveAddress,
          Command,
          Operation,
          PecCheck,
          Length,
          Buffer
          );
}

/**
  <b>SMBus DXE Module Entry Point</b>\n
  - <b>Introduction</b>\n
    The SMBus module is a DXE driver that provides a standard way to execute an SMBus command.

  - @pre
    - @link _PCH_POLICY PCH_POLICY_HOB @endlink
      - This module uses the parameters in SmbusConfig for platform reserved (non ARP capable) addresses.

  - @result
    The SMBus driver produces EFI_SMBUS_HC_PROTOCOL which is documented in the SMBus Host Controller
    Protocol Specification.

  @param[in] ImageHandle          ImageHandle of this module
  @param[in] SystemTable          EFI System Table

  @retval EFI_SUCCESS             Driver initializes successfully
  @retval Other values            Some error occurred
**/
EFI_STATUS
EFIAPI
InitializePchSmbus (
  IN      EFI_HANDLE            ImageHandle,
  IN      EFI_SYSTEM_TABLE      *SystemTable
  )
{
  PCH_POLICY_HOB           *PchPolicyHob;
  EFI_STATUS               Status;
  UINTN                    DataSize;
  VOID                     *Data;
  EFI_PEI_HOB_POINTERS     HobList;
  UINT32                   SmbusSpeed;
  UINT32                   SmbusHostMemBase;
  UINTN                    SmbusHostPcieBase;
  PHYSICAL_ADDRESS         MemoryAddress;

  DEBUG ((DEBUG_INFO, "InitializePchSmbus() Start\n"));

  HobList.Guid   = GetFirstGuidHob (&gPchPolicyHobGuid);
  ASSERT (HobList.Guid != NULL);
  PchPolicyHob  = GET_GUID_HOB_DATA (HobList.Guid);


  mSmbusContext = AllocateZeroPool (sizeof (SMBUS_INSTANCE));
  if (mSmbusContext == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  mSmbusContext->Signature                  = PCH_SMBUS_PRIVATE_DATA_SIGNATURE;
  mSmbusContext->IoDone                     = IoDone;
  mSmbusContext->SmbusIoRead                = SmbusIoRead;
  mSmbusContext->SmbusIoWrite               = SmbusIoWrite;
  mSmbusContext->SmbusController.Execute    = SmbusExecute;
  mSmbusContext->SmbusController.ArpDevice  = SmbusArpDevice;
  mSmbusContext->SmbusController.GetArpMap  = SmbusGetArpMap;
  mSmbusContext->SmbusController.Notify     = SmbusNotify;
  mSmbusContext->PlatformNumRsvd            = PchPolicyHob->SmbusConfig.NumRsvdSmbusAddresses;
  mSmbusContext->PlatformRsvdAddr           = PchPolicyHob->SmbusConfig.RsvdSmbusAddressTable;
  ASSERT (mSmbusContext->PlatformNumRsvd <= PCH_MAX_SMBUS_RESERVED_ADDRESS);

  ///
  /// See if PEI already ARPed any devices, and if so, update our device map.
  ///
  /// Get Hob list
  ///
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &HobList.Raw);
  ASSERT_EFI_ERROR (Status);

  HobList.Raw = GetNextGuidHob (&gEfiSmbusArpMapGuid, HobList.Raw);
  ///
  /// If we found the right hob, store the information. Otherwise, continue.
  ///
  if (HobList.Raw != NULL) {
    Data      = (VOID *) ((UINT8 *) (&HobList.Guid->Name) + sizeof (EFI_GUID));
    DataSize  = HobList.Header->HobLength - sizeof (EFI_HOB_GUID_TYPE);
    CopyMem (mSmbusContext->DeviceMap, Data, DataSize);
    mSmbusContext->DeviceMapEntries = (UINT8) (DataSize / sizeof (EFI_SMBUS_DEVICE_MAP));
  }
  ///
  /// Initialize the NotifyFunctionList
  ///
  InitializeListHead (&mSmbusContext->NotifyFunctionList);

  ///
  /// Install the SMBUS interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSmbusContext->Handle,
                  &gEfiSmbusHcProtocolGuid,
                  &mSmbusContext->SmbusController,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);


  SmbusHostPcieBase = MmPciBase (PCI_BUS_NUMBER_PCH_SMBUS_HOST, PCI_DEVICE_NUMBER_PCH_SMBUS_HOST, PCI_FUNCTION_NUMBER_PCH_SMBUS_HOST);

  //
  //Setup Temporary base address
  //
  MmioWrite32(SmbusHostPcieBase + R_PCH_SMBUS_HOST_SMTBAR, 0x8e000000);
  MmioWrite32(SmbusHostPcieBase + PCI_COMMAND_OFFSET,
              EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);

  ///
  /// If SMBus IOSF Clock Gating setting is enabled, then set IOSFDEVCLKGCTL ICGE bit register
  ///
  if (PchPolicyHob->SmbusConfig.SMBusIOSFClockGating) {
	MmioOr16((UINTN) (SmbusHostPcieBase + R_PCH_SMBUS_IOSFDEVCLKGCTL), (UINT16) (V_PCH_SMBUS_IOSFDEVCLKGCTL_ICGE));
  } else {
	MmioAnd16((UINTN) (SmbusHostPcieBase + R_PCH_SMBUS_IOSFDEVCLKGCTL), (UINT16) (~V_PCH_SMBUS_IOSFDEVCLKGCTL_ICGE));
  }

  SmbusHostMemBase = MmioRead32(SmbusHostPcieBase + R_PCH_SMBUS_HOST_SMTBAR) & B_PCH_SMBUS_HOST_SMTBAR_MBA;

  switch(PchPolicyHob->SmbusConfig.SMBusHostSpeedConfig){
  case PchSMBusStandard80Khz:
	  SmbusSpeed = V_PCH_SMBUS_SMTBAR_SPGT_80KHZ;
	  break;
  case PchSMBusStandard100Khz:
	  SmbusSpeed = V_PCH_SMBUS_SMTBAR_SPGT_100KHZ;
	  break;
  case PchSMBusFastMode400Khz:
	  SmbusSpeed = V_PCH_SMBUS_SMTBAR_SPGT_400KHZ;
	  break;
  case PchSMBusFastModePlus1Mhz:
	  SmbusSpeed = V_PCH_SMBUS_SMTBAR_SPGT_1MHZ;
	  break;
  default:
	  SmbusSpeed = V_PCH_SMBUS_SMTBAR_SPGT_80KHZ;
  }

  MmioAndThenOr32(SmbusHostMemBase + R_PCH_SMBUS_SMTBAR_SPGT, (UINT32) ~B_PCH_SMBUS_SMTBAR_SPGT_SPD, SmbusSpeed);

  //Host SMBUS config

  //Reserve Memory for MDBA and TBBA
  MemoryAddress = 0xFFFFFFFF;
  Status = gBS->AllocatePages (
		  	  	  AllocateMaxAddress,
		  	  	  EfiReservedMemoryType,
		  	  	  EFI_SIZE_TO_PAGES (SIZE_4KB),
		  	  	  &MemoryAddress);
  if(EFI_ERROR(Status))
  {
	  DEBUG ((DEBUG_INFO, "Host SMBus Config: Cannot allocate memory for Master Descriptor MDBA\n"));
  }
  else
  {
       //Program Base MDBA
	  DEBUG ((DEBUG_INFO, "MemoryBase Address for MDBA 0x%08x\n", MemoryAddress));
	  MmioWrite64(SmbusHostMemBase + R_PCH_SMBUS_SMTBAR_MDBA, MemoryAddress);
  }
  Status = gBS->AllocatePages (
		  	  	  AllocateMaxAddress,
  		  	  	  EfiReservedMemoryType,
  		  	  	  EFI_SIZE_TO_PAGES (SIZE_64KB),
  		  	  	  &MemoryAddress);
  if(EFI_ERROR(Status))
  {
	  DEBUG ((DEBUG_INFO, "Host SMBus Config: Cannot allocate memory for Target Descriptor\n"));
  }
  else
  {
      //Program Base TBBA
	  DEBUG ((DEBUG_INFO, "MemoryBase Address for TBBA 0x%08x\n", MemoryAddress));
	  MmioWrite64(SmbusHostMemBase + R_PCH_SMBUS_SMTBAR_TBBA, MemoryAddress);
  }

  //CLTT SMBus config
  PchPcrAndThenOr32(PID_CLTT_SMBUS, R_PCH_CLTT_SMBUS_HOSTC, (UINT32) ~B_PCH_CLTT_SMBUS_HOSTC_HST_EN, B_PCH_CLTT_SMBUS_HOSTC_HST_EN);

  //PECI SMBus
  PchPcrAndThenOr32(PID_PECI_SMBUS, R_PCH_PECI_SMBUS_SPGT, (UINT32) ~B_PCH_PECI_SMBUS_SPGT_SPD, V_PCH_PECI_SMBUS_SPGT_100KHZ);

  //Denverton AptioV override Start - EIP#309093
  //New setting for SMBus due to violation of SMBus clock  
  //Program SMBUS clock
  MmioAndThenOr32(SmbusHostMemBase + R_PCH_SMBUS_SMTBAR_SPMT, (UINT32) ~B_PCH_SMBUS_SMTBAR_SPMT, (UINT32) V_PCH_SMBUS_SMTBAR_SPMT);
  //Denverton AptioV override End - EIP#309093

  //Reset PCI command register to 0
  MmioWrite32(SmbusHostPcieBase + PCI_COMMAND_OFFSET, 0);

  //
  DEBUG ((DEBUG_INFO, "InitializePchSmbus() End\n"));

  return EFI_SUCCESS;
}
