/** @file
  This is the Uefi driver that initializes the Intel PCH.

@copyright
  Copyright (c) 2015 - 2017 Intel Corporation. All rights reserved
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
#include "PchInit.h"


#include <SocRegs.h>
#include <Library/DebugPrintErrorLevelLib.h> // Denverton Aptiov override - EIP#281374


GLOBAL_REMOVE_IF_UNREFERENCED EFI_HANDLE               mImageHandle;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                   mPcieIoTrapAddress;
extern PCH_USB_HC_PORT_PRECONDITION                    *mPrivatePreConditionList;

HSUART_BDF HsUartBdfs[] =
{
    {PCI_BUS_NUMBER_HSUART0,PCI_DEVICE_NUMBER_HSUART0,PCI_FUNCTION_NUMBER_HSUART0},
    {PCI_BUS_NUMBER_HSUART1,PCI_DEVICE_NUMBER_HSUART1,PCI_FUNCTION_NUMBER_HSUART1},
    {PCI_BUS_NUMBER_HSUART2,PCI_DEVICE_NUMBER_HSUART2,PCI_FUNCTION_NUMBER_HSUART2}
};


/**
  PCH initialization before ExitBootServices / LegacyBoot events
  Useful for operations which must happen later than at EndOfPost event

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
PchOnBootToOsUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

/**
  PCH initialization on ExitBootService. This event is used if only ExitBootService is used
  and not in legacy boot

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

  @retval None
**/
VOID
EFIAPI
PchOnExitBootServicesUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

/**
  PCH initialization before boot to OS

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
PchOnReadyToBootUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );


/**
  PCH initialization before Boot Sript Table is closed

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
PchOnEndOfDxeUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );



/**
  Reveal TraceHub device which is hidden when PEI memory init done to skip from Pci enumertion.

**/
VOID
PchRevealTraceHub (
  VOID
  )
{
  UINTN           TraceHubBaseAddress;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  ///
  /// After PCI Enumeration, TraceHub Device has to be unhidden if it is not power gated
  ///
  // DNV_TODO: no CFG_DIS register for D31D7?
  PchPcrAndThenOr32 (
          PID_PSF1,
          R_PCH_PCR_PSF1_T0_SHDW_TRACE_HUB_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_CFG_DIS,
          (UINT32)~BIT0,
          (UINT32)0x00
          );

  TraceHubBaseAddress = MmPciBase (
                            DEFAULT_PCI_BUS_NUMBER_PCH,
                            PCI_DEVICE_NUMBER_PCH_TRACE_HUB,
                            PCI_FUNCTION_NUMBER_PCH_TRACE_HUB
                            );
  DEBUG ((DEBUG_INFO, "\tSW_LBAR (before)  = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR)));
  MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_UBAR, (UINT32)0x00);
  MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR, PCH_TRACE_HUB_SW_BASE_ADDRESS);
  DEBUG ((DEBUG_INFO, "\tMTB_LBAR = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_LBAR)));
  DEBUG ((DEBUG_INFO, "\tRTIT_LBAR = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_RTIT_LBAR)));
  DEBUG ((DEBUG_INFO, "\tSW_LBAR  = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR)));
  DEBUG ((DEBUG_INFO, "\tFW_LBAR  = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_FW_LBAR)));

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}

/**
  This is the callback function for PCI ENUMERATION COMPLETE.
**/
VOID
EFIAPI
PchOnPciEnumComplete (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS          Status;
  VOID                *ProtocolPointer;
  UINT32              PchPwrmBase;

  ///
  /// Check if this is first time called by EfiCreateProtocolNotifyEvent() or not,
  /// if it is, we will skip it until real event is triggered
  ///
  Status = gBS->LocateProtocol (&gEfiPciEnumerationCompleteProtocolGuid, NULL, (VOID **) &ProtocolPointer);
  if (EFI_SUCCESS != Status) {
     return;
  }
  gBS->CloseEvent (Event);


  PchPwrmBaseGet (&PchPwrmBase);
  if ((MmioRead32 (PchPwrmBase + R_PCH_PWRM_HSWPGCR1) & B_PCH_PWRM_DFX_SW_PG_CTRL) == 0) {
  //
  // Reveal TraceHub controller.
  //
  PchRevealTraceHub ();
  }

}

VOID
PchRegisterNotifications (
  VOID
  )
{
  EFI_STATUS  Status;


  EFI_EVENT   EndOfDxeEvent;
  EFI_EVENT   ReadyToBoot;
  EFI_EVENT   LegacyBootEvent;
  EFI_EVENT   ExitBootServicesEvent;
  VOID        *Registration;

  ///
  /// Create PCI Enumeration Completed callback for PCH
  ///
  EfiCreateProtocolNotifyEvent (
    &gEfiPciEnumerationCompleteProtocolGuid,
    TPL_CALLBACK,
    PchOnPciEnumComplete,
    NULL,
    &Registration
    );

  //
  // Register an end of DXE event for PCH to do tasks before invoking any UEFI drivers,
  // applications, or connecting consoles,...
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  PchOnEndOfDxeUefi,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register a Ready to boot event to config PCIE power management setting after OPROM executed
  //
  Status = EfiCreateEventReadyToBootEx (
            TPL_CALLBACK,
            PchOnReadyToBootUefi,
            NULL,
            &ReadyToBoot
            );
  ASSERT_EFI_ERROR (Status);

  //
  // Create events for PCH to do the task before ExitBootServices/LegacyBoot.
  // It is guaranteed that only one of two events below will be signalled
  //
  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_CALLBACK,
                  PchOnExitBootServicesUefi,
                  NULL,
                  &ExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);

  Status = EfiCreateEventLegacyBootEx (
            TPL_CALLBACK,
            PchOnBootToOsUefi,
            NULL,
            &LegacyBootEvent
            );
  ASSERT_EFI_ERROR (Status);
}




VOID HsUartSetPcieMode(PCH_HSUART_CONFIG *HsUartConfig, UINTN HsUartBase)
{

  MmioAnd32(HsUartBase + R_HSUART_RDCFG, ~(UINT32)B_HSUART_RDCFG_HIDE); // Clear hide bit
}

VOID HsUartSetHideMode(PCH_HSUART_CONFIG *HsUartConfig, UINTN HsUartBase)
{

  MmioAndThenOr32(HsUartBase + R_HSUART_RDCFG, ~(UINT32)B_HSUART_RDCFG_HIDE, (UINT32)B_HSUART_RDCFG_HIDE); // Set hide bit
  MmioAnd32(HsUartBase + R_HSUART_PCICMD, ~(UINT32)B_HSUART_PCICMD_IOSE);                          // Clear IO EN in CMD
}

VOID HsUartSetLegacyMode(PCH_HSUART_CONFIG *HsUartConfig, UINTN HsUartBase)
{

  MmioAndThenOr32(HsUartBase + R_HSUART_PCICMD, ~(UINT32)B_HSUART_PCICMD_IOSE, (UINT32)B_HSUART_PCICMD_IOSE);     // Set IO en in CMD
  MmioAndThenOr32(HsUartBase + R_HSUART_IOBA, ~(UINT32)B_HSUART_IOBA_BASE_ADDR, HsUartConfig->HsUartCOM); // Set BAR address
  MmioAndThenOr32(HsUartBase + R_HSUART_RDCFG, ~(UINT32)B_HSUART_RDCFG_HIDE, (UINT32)B_HSUART_RDCFG_HIDE);        // Set hide bit

}

VOID HsUartConfigureDefault(PCH_HSUART_CONFIG *HsUartConfig)
{
  UINT8 Index;
  UINTN HsUartBase;

  DEBUG ((DEBUG_INFO, "Configure default HSUART begin\n"));

  for(Index = 0; Index < PCH_MAX_HSUART; Index++)
  {
    DEBUG ((DEBUG_INFO, "Index: %x\n",Index));


    HsUartBase = MmPciBase (HsUartBdfs[Index].bus,
                            HsUartBdfs[Index].device,
                            HsUartBdfs[Index].function);



    HsUartSetPcieMode(&HsUartConfig[Index],HsUartBase);
  }
  DEBUG ((DEBUG_INFO, "Configure default HSUART end\n"));

}

VOID HsUartConfigure(PCH_HSUART_CONFIG *HsUartConfig)
{
  UINT8 Index;
  UINTN HsUartBase;

  DEBUG ((DEBUG_INFO, "Configure HSUART begin\n"));

  for(Index = 0; Index < PCH_MAX_HSUART; Index++)
  {
    DEBUG ((DEBUG_INFO, "Index: %x\n",Index));


    HsUartBase = MmPciBase (HsUartBdfs[Index].bus,
                            HsUartBdfs[Index].device,
                            HsUartBdfs[Index].function);



    switch (HsUartConfig[Index].HsUartMode) {
      case HsUartModePcie:
        HsUartSetPcieMode(&HsUartConfig[Index],HsUartBase);
        break;
      case HsUartModeLegacy:
        HsUartSetLegacyMode(&HsUartConfig[Index],HsUartBase);
        break;
      case HsUartModeHide:
        HsUartSetHideMode(&HsUartConfig[Index],HsUartBase);
        break;
      default:
        DEBUG ((DEBUG_INFO, "\tWrong configuration of HSUART %x: Mode %x\n", Index, HsUartConfig[Index].HsUartMode));
        ASSERT(FALSE);
        break;
    }
  }
  DEBUG ((DEBUG_INFO, "Configure HSUART end\n"));

}


/**
  Initialize the PCH device according to the PCH Policy HOB
  and install PCH info instance.

**/
VOID
InitializePchDevice (
  VOID
  )
{
  EFI_HANDLE              Handle;
  PCH_INFO_PROTOCOL       *PchInfo;
  EFI_STATUS              Status;
  UINT32                  PchPwrmBase;
  UINT32                  FuncDisableReg;
  Handle = NULL;

  DEBUG ((DEBUG_INFO, "InitializePchDevice() Start\n"));

  PchPwrmBaseGet (&PchPwrmBase);

  //
  // Create the PCH Info protocol instance
  //
  PchInfo = AllocateZeroPool (sizeof (PCH_INFO_PROTOCOL));
  if (PchInfo == NULL) {
    ASSERT (FALSE);
    return;
  }

  PchInfo->Revision = PCH_INFO_PROTOCOL_REVISION_1;



  //
  // Perform USB precondition on XHCI
  //
  if (mPchPolicyHob->UsbConfig.UsbPrecondition == 1) {
    XhciPrecondition ();
    PchInfo->Preconditioned   = mPrivatePreConditionList;
  }



  //
  // Install PCH info protocol on new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gPchInfoProtocolGuid,
                  PchInfo,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Configure SCC Devices
  ///
  Status = ConfigureScc (mPchPolicyHob, &FuncDisableReg);
  ASSERT_EFI_ERROR (Status);

// Denverton AptioV override Start - EIP#342261
// AMI sets HSUART mode based on SDL token and default is Legacy mode, to support legacy serial redirection by default.
// As this code sets HSUART to Pcie by default, modifying it to set the mode based on SDL token.
//  HsUartConfigureDefault(mPchPolicyHob->HsUartConfig);
  HsUartConfigure(mPchPolicyHob->HsUartConfig);
// Denverton AptioV override End - EIP#342261   
  DEBUG ((DEBUG_INFO, "InitializePchDevice() End\n"));
}
/**
  <b>PchInit DXE Module Entry Point</b>\n
  - <b>Introduction</b>\n
      The PchInit module is a DXE driver that initializes the Intel Platform Controller Hub
      following the PCH BIOS specification and EDS requirements and recommendations. It consumes
      the PCH_POLICY_HOB SI_POLICY_HOB for expected configurations per policy.
      This is the standard EFI driver point that detects whether there is an supported PCH in
      the system and if so, initializes the chipset.

  - <b>Details</b>\n
    This module is required for initializing the Intel Platform Controller Hub to
    follow the PCH BIOS specification and EDS.
    This includes some initialization sequences, enabling and disabling PCH devices,
    configuring clock gating, RST PCIe Storage Remapping, SATA controller, ASPM of PCIE devices. Right before end of DXE,
    it's responsible to lock down registers for security requirement.

  - @pre
    - PCH PCR base address configured
    - EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
      - This is to ensure that PCI MMIO and IO resource has been prepared and available for this driver to allocate.

  - @result
    - Publishes the @link _PCH_INFO_PROTOCOL PCH_INFO_PROTOCOL @endlink
    - Publishes the @link _PCH_EMMC_TUNING_PROTOCOL PCH_EMMC_TUNING_PROTOCOL @endlink

  - <b>References</b>\n
    - @link _PCH_POLICY PCH_POLICY_HOB @endlink.
    - @link _SI_POLICY_STRUCT SI_POLICY_HOB @endlink.

  - <b>Integration Checklists</b>\n
    - Verify prerequisites are met. Porting Recommendations.
    - No modification of this module should be necessary
    - Any modification of this module should follow the PCH BIOS Specification and EDS

  @param[in] ImageHandle          Handle for the image of this driver
  @param[in] SystemTable          Pointer to the EFI System Table

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
PchInitEntryPointUefi (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  EFI_PHYSICAL_ADDRESS  MemRegBuff = 0;

  DEBUG ((DEBUG_INFO, "Uefi PchInitEntryPoint() Start\n"));

  mImageHandle = ImageHandle;
  //
  // Set PCH LPC/eSPI memory range decoding.
  //
  MemRegBuff = 0xFFFFFFFF;
  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateMaxAddressSearchBottomUp,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  16,
                  128 * 1024,
                  &MemRegBuff,
                  mImageHandle,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "\tMemory allocation failed!\n"));
    ASSERT (Status);
  } else {
    PchLpcMemRangeSet((UINT32)MemRegBuff);
  }
  PchInitEntryPoint();

  //
  // Initialize PCH devices and install PCH info protocol
  //
  InitializePchDevice ();


  Status = PchAcpiInit (ImageHandle);


  PchRegisterNotifications ();

  DEBUG ((DEBUG_INFO, "Uefi PchInitEntryPoint() End\n"));

  return Status;
}

/**
  PCH initialization before ExitBootServices / LegacyBoot events
  Useful for operations which must happen later than at EndOfPost event

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
PchOnBootToOsUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  ///
  /// Closed the event to avoid call twice
  ///
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  ConfigureSccAtBoot (mPchPolicyHob);

  return;
}

/**
  PCH initialization on ExitBootService. This event is used if only ExitBootService is used
  and not in legacy boot

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

  @retval None
**/
VOID
EFIAPI
PchOnExitBootServicesUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
// Denverton AptioV override Start - EIP#342261
// AMI sets HSUART mode based on SDL token and default is Legacy mode, to support legacy serial redirection by default.
// This call is not necessary
//  HsUartConfigure(mPchPolicyHob->HsUartConfig);
// Denverton AptioV override End - EIP#342261

  PchOnBootToOsUefi (NULL, NULL);

  PchUpdateNvsOnExitBootServices ();

  return;
}

/**
  PCH initialization before boot to OS

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
PchOnReadyToBootUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_PEI_HOB_POINTERS  HobPtr;
  UINTN                 TraceHubBaseAddress;
  UINT32                TraceHubCmd;

  DEBUG ((DEBUG_INFO, "Uefi PchOnReadyToBoot() Start\n"));

  if (Event != NULL) {
    gBS->CloseEvent(Event);
  }

  HobPtr.Guid = GetFirstGuidHob( &gPchPolicyHobGuid );
  if ( HobPtr.Guid == NULL ) {
	DEBUG ((DEBUG_ERROR, "GUID for Tracehub is not found it\n"));
	ASSERT ( FALSE ) ;
  } else {
    mPchPolicyHob = GET_GUID_HOB_DATA( HobPtr.Guid );
    if ( mPchPolicyHob->PchTraceHubConfig.TraceHubMemoryEnabled == 0 ) {
      DEBUG ((DEBUG_ERROR, "Tracehub memory access will be disabled\n"));
      TraceHubBaseAddress = MmPciBase( DEFAULT_PCI_BUS_NUMBER_PCH , PCI_DEVICE_NUMBER_PCH_TRACE_HUB, PCI_FUNCTION_NUMBER_PCH_TRACE_HUB );
      TraceHubCmd = MmioRead32( TraceHubBaseAddress + 0x04 );
      TraceHubCmd &= ~(UINT32) (BIT1);
      MmioWrite32(TraceHubBaseAddress + 0x04, TraceHubCmd);
    }
  }
  DEBUG ((DEBUG_INFO, "Uefi PchOnReadyToBoot() End\n"));
}


/**
  PCH initialization before Boot Sript Table is closed

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
PchOnEndOfDxeUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{

  DEBUG ((DEBUG_INFO, "Uefi PchOnEndOfDxe() Start\n"));

  ///
  /// Closed the event to avoid call twice when launch shell
  ///
  gBS->CloseEvent (Event);

  PchOnEndOfDxe();

  DEBUG ((DEBUG_INFO, "Uefi PchOnEndOfDxe() End\n"));

  return;
}
