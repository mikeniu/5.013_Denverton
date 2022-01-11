//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PeiIpmiBmcInitialize.c
    PEI IPMI Transport Driver.

**/

//---------------------------------------------------------------------------

#include "Token.h"
#include "IpmiBmc.h"
#include "IpmiInitHooks.h"
#include <Include/ServerMgmtSetupVariable.h>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

extern INIT_LPC_BMC INIT_LPC_BMC_HOOK_LIST EndOfInitLpcBmcHookListFunctions;
INIT_LPC_BMC* gInitLpcBmcHookList[] = {INIT_LPC_BMC_HOOK_LIST NULL};

#if BMC_INIT_DELAY
extern BMC_INIT_DELAY_HOOK BMC_INIT_DELAY_HOOK_LIST EndOfBmcInitDelayHookListFunctions;
BMC_INIT_DELAY_HOOK* gBmcInitDelayHookList[] = {BMC_INIT_DELAY_HOOK_LIST NULL};
#endif

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
extern IPMI_OEM_PEI_INIT_HOOK IPMI_OEM_PEI_INIT_HOOK_LIST EndOfIpmiOemPeiInitHookListFunctions;
IPMI_OEM_PEI_INIT_HOOK* gIpmiOemPeiInitHookList[] = {IPMI_OEM_PEI_INIT_HOOK_LIST NULL};
#endif

//---------------------------------------------------------------------------

/**
    This routine calls the InitLpcBmc function or platform hooked function.

    @param PeiServices Pointer to PEI Services.

    @return EFI_STATUS Return Status

**/
EFI_STATUS
InitLpcBmcHook (
  IN CONST EFI_PEI_SERVICES          **PeiServices )
{
    EFI_STATUS Status;
    UINTN i;

    for(i = 0; gInitLpcBmcHookList[i]; i++) {
        Status = gInitLpcBmcHookList[i](PeiServices);
    }

    return Status;
}

#if BMC_INIT_DELAY
/**
    This routine calls the BmcInitDelay function or platform hooked function.

    @param PeiServices Pointer to PEI Services.

    @return BOOLEAN

**/
BOOLEAN
BmcInitDelayHook (
  IN CONST EFI_PEI_SERVICES          **PeiServices )
{
    BOOLEAN IsPowerFail;
    UINTN   i;

    for(i = 0; gBmcInitDelayHookList[i]; i++) {
        IsPowerFail = gBmcInitDelayHookList[i](PeiServices);
    }

    return IsPowerFail;
}
#endif

#if (!IPMI_DEFAULT_HOOK_SUPPORT)
/**
    Initiates the PEI hooks of OEM.

    @param PeiServices      - Pointer the set of PEI services.
    @param MmioBaseAddress  - Pointer to the MMIO base address.
    @param BaseAddressRange - Pointer to the base address range.

    @return EFI_STATUS      - Return status of the Hook.
*/

EFI_STATUS
IpmiOemPeiInitHook (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       UINTN                        *MmioBaseAddress,
  IN       UINTN                        *BaseAddressRange
)
{
    EFI_STATUS  Status = EFI_DEVICE_ERROR;
    UINTN       Index;
    for (Index = 0; gIpmiOemPeiInitHookList[Index]; Index++) {
        Status = gIpmiOemPeiInitHookList[Index] (PeiServices, MmioBaseAddress, BaseAddressRange);
    }
    return Status;
}
#endif

/**
    Execute the Get Device ID command to determine whether or not the BMC is in
    Force Update Mode. If it is, then report it to the error manager.

    @param IpmiInstance Data structure describing BMC
               variables and used for sending commands

    @return EFI_STATUS Return Status of the Sent Command
    @retval EFI_SUCCESS Command executed Successfully

**/
EFI_STATUS
GetDeviceId (
  IN  OUT  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance )
{
    EFI_STATUS                            Status;
    UINT8                                 DataSize;
    IPMI_GET_DEVICE_ID_RESPONSE           GetDeviceIdResponse;

    DataSize = sizeof (GetDeviceIdResponse);

    //
    // Get the device ID information for the BMC.
    //
    Status = IpmiInstance->IpmiTransport.SendIpmiCommand (
                &IpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_DEVICE_ID,
                NULL,
                0,
                (UINT8*)&GetDeviceIdResponse,
                &DataSize );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get Device ID Command: Status %r\n", Status));
    //
    // If there is no error then proceed to check the data returned by the BMC
    //
    if (!EFI_ERROR(Status)) {

        //
        // If the controller is in Update Mode then set the BMC Status to indicate
        // the BMC is in force update mode.
        //
        if (GetDeviceIdResponse.UpdateMode != 0) {
            IpmiInstance->BmcStatus = EFI_BMC_UPDATE_IN_PROGRESS;
        }
    }
    return Status;
}

/**
    Execute the Get System Interface Capability command and update Ipmi Instance.
    
    @ IpmiInstance Pointer to Data structure describing BMC
                   variables and used for sending commands

    @retval  VOID

**/
VOID
GetSystemInterfaceCapability (
  IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance
)
{
    EFI_STATUS                                     Status;
    EFI_GET_SYSTEM_INTERFACE_CAPABILITY_COMMAND    GetSystemInterfaceCapabilityCmd;
    EFI_IPMI_SSIF_INTERFACE_CAPABILITY             GetSsifInterfaceCapability;
    UINT8                                          DataSize = sizeof (GetSsifInterfaceCapability);

    GetSystemInterfaceCapabilityCmd.SystemInterfaceType = 0x0; // SSIF
    GetSystemInterfaceCapabilityCmd.Reserved = 0x0;
    
    Status = IpmiInstance->IpmiTransport.SendIpmiCommand (
                &IpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_GET_SYSTEM_INTERFACE_CAPABILITIES,
                (UINT8*) &GetSystemInterfaceCapabilityCmd,
                sizeof (GetSystemInterfaceCapabilityCmd),
                (UINT8*) &GetSsifInterfaceCapability,
                &DataSize);

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get System Interface Capability status %r\n", Status));

    if (!EFI_ERROR (Status)) {
        IpmiInstance->Interface.SSIF.RwSupport = GetSsifInterfaceCapability.TransactionSupport;
        IpmiInstance->Interface.SSIF.PecSupport = GetSsifInterfaceCapability.PecSupport;
    }

}

/**
    Execute the Set Global Enable command to enable receive message queue interrupt.

    @param IpmiInstance Data structure describing BMC
                        variables and used for sending commands

    @return VOID
**/
VOID
SetGlobalEnable (
  IN  OUT  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance )
{
    EFI_STATUS                      Status;
    EFI_BMC_GLOBAL_ENABLES          BmcGlobalEnables;
    UINT32                          ResponseDataSize = sizeof (BmcGlobalEnables);

    //
    // Get Global Enable Information.
    //
    Status = IpmiInstance->IpmiTransport.SendIpmiCommand(
                &IpmiInstance->IpmiTransport,
                IPMI_NETFN_APP,
                BMC_LUN,
                IPMI_APP_SET_BMC_GLOBAL_ENABLES,
                NULL,
                0,
                (UINT8 *) (&BmcGlobalEnables),
                (UINT8 *) &ResponseDataSize
                );

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get BMC Global Enable status %r\n", Status));

    if (EFI_ERROR(Status)) {
        return;
    }

    //
    // Set SmbAlertSupport based on ReceiveMsgQueueInterrupt bit
    //

    IpmiInstance->Interface.SSIF.SmbAlertSupport = BmcGlobalEnables.ReceiveMsgQueueInterrupt;
}

/**
    Setup and initialize the BMC for the PEI phase. In order to
    verify the BMC is functioning as expected, the BMC Self-test is performed.
    The results are then checked and any errors are reported to the error manager.
    Errors are collected throughout this routine and reported just prior to 
    installing the driver.  If there are more errors than BST_LOG_SIZE, then they
    will be ignored.

    @param FileHandle    Pointer to image file handle.
    @param PeiServices   Pointer to the PEI Core data Structure

    @return EFI_STATUS  Status of Driver execution
    @retval EFI_SUCCESS Successful driver initialization

**/
EFI_STATUS
InitializeIpmiPhysicalLayer (
  IN        EFI_PEI_FILE_HANDLE     FileHandle,
  IN  CONST EFI_PEI_SERVICES        **PeiServices )
{

    EFI_STATUS                              Status = EFI_SUCCESS;
    EFI_IPMI_BMC_INSTANCE_DATA              *IpmiInstance = NULL;
    UINT8                                   DataSize;
    UINT8                                   Index;
    EFI_PEI_PPI_DESCRIPTOR                  *PeiIpmiPpiDesc = NULL;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI         *PeiVariable;
    SERVER_MGMT_CONFIGURATION_DATA          ServerMgmtConfiguration;
    IPMI_SELF_TEST_RESULT_RESPONSE          BstStatus;
    UINTN                                   Size;
    EFI_PEI_I2C_MASTER_PPI                  *MasterTransmit = NULL;
    EFI_PEI_SMBUS2_PPI                      *EfiPeiSmbus2Ppi = NULL;

    //
    // Locate the Setup configuration value.
    //
    Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0,
                NULL,
                (VOID **)&PeiVariable);
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "gEfiPeiReadOnlyVariable2PpiGuid Status %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    if (!EFI_ERROR(Status)) {
        Size = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
        Status = PeiVariable->GetVariable (
                    PeiVariable,
                    L"ServerSetup",
                    &gEfiServerMgmtSetupVariableGuid,
                    NULL,
                    &Size,
                    &ServerMgmtConfiguration);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "PeiVariable->GetVariable Status %r ServerMgmtConfiguration.BmcSupport: %x \n", Status, ServerMgmtConfiguration.BmcSupport));

        if (!EFI_ERROR(Status) && !ServerMgmtConfiguration.BmcSupport) {
            return EFI_UNSUPPORTED;
        }
    }

#if BMC_INIT_DELAY
    if (EFI_ERROR(Status)) {
         ServerMgmtConfiguration.WaitForBmc = 0; //Load default value.
    }   
#endif

    //
    // Allocate memory for IPMI Instance
    //
    IpmiInstance = (EFI_IPMI_BMC_INSTANCE_DATA*)AllocateZeroPool (sizeof (EFI_IPMI_BMC_INSTANCE_DATA));
    if (IpmiInstance == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for IpmiInstance! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Allocate memory for PPI Descriptor
    //
    PeiIpmiPpiDesc = (EFI_PEI_PPI_DESCRIPTOR*)AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
    if (PeiIpmiPpiDesc == NULL) {
        SERVER_IPMI_DEBUG ((EFI_D_ERROR, "Failed to allocate memory for PeiIpmiPpiDesc! \n"));
        //
        // AllocateZeroPool has failed, so free the allocated memory and return EFI_OUT_OF_RESOURCES.
        //
        FreePool (IpmiInstance);
        return EFI_OUT_OF_RESOURCES;
    }

    //
    //Platform Hook to initialize LPC for BMC and Host interface.
    //
    Status = InitLpcBmcHook (PeiServices);
    if (EFI_ERROR(Status)) {
        //
        // InitLpcBmcHook has failed, so free the allocated memory and return Status.
        //
        FreePool (IpmiInstance);
        FreePool (PeiIpmiPpiDesc);
        return Status;
    }

    if (IPMI_DEFAULT_HOOK_SUPPORT) {
        //
        // Initialize IPMI Instance Data
        //
        if (IPMI_SYSTEM_INTERFACE == KCS_INTERFACE) { // KCS Interface
            IpmiInstance->Interface.KCS.DataPort        = IPMI_KCS_DATA_PORT;       // KCS Data Port
            IpmiInstance->Interface.KCS.CommandPort     = IPMI_KCS_COMMAND_PORT;    // KCS Command Port
        } else if (IPMI_SYSTEM_INTERFACE == BT_INTERFACE) { // BT_Interface
            IpmiInstance->Interface.BT.CtrlPort    = IPMI_BT_CTRL_PORT;     // BT Control Port
            IpmiInstance->Interface.BT.ComBuffer   = IPMI_BT_BUFFER_PORT; // BT Buffer Port
            IpmiInstance->Interface.BT.IntMaskPort = IPMI_BT_INTERRUPT_MASK_PORT; // BT IntMask Port
        }
        IpmiInstance->MmioBaseAddress = 0;
        IpmiInstance->AccessType = IPMI_IO_ACCESS;
        IpmiInstance->BaseAddressRange = 0;
    } else {
#if (!IPMI_DEFAULT_HOOK_SUPPORT)
        IpmiInstance->AccessType = IPMI_MMIO_ACCESS;
        Status = IpmiOemPeiInitHook (PeiServices, &IpmiInstance->MmioBaseAddress, &IpmiInstance->BaseAddressRange);
        if (EFI_ERROR (Status)) {
            //
            // IpmiOemPeiInitHook has failed, so free the allocated memory and return Status.
            //
            FreePool (IpmiInstance);
            FreePool (PeiIpmiPpiDesc);
            SERVER_IPMI_DEBUG ((EFI_D_ERROR, "IpmiOemPeiInitHook is failed and Status is %r.\n", Status));
            return Status;
        }
#endif
    }

    if (IPMI_SYSTEM_INTERFACE == KCS_INTERFACE) { // KCS Interface
        IpmiInstance->Interface.KCS.KcsRetryCounter = IPMI_KCS_COMMAND_PORT_READ_RETRY_COUNTER;
        IpmiInstance->IpmiTransport.SendIpmiCommand = EfiIpmiSendCommand;       // KCS SendCommand
        IpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiIpmiSendCommandEx; // KCS SendCommand Extension
    } else if (IPMI_SYSTEM_INTERFACE == BT_INTERFACE) { // BT_Interface
        IpmiInstance->Interface.BT.BtDelay     = BT_DELAY; // BT Delay
        IpmiInstance->IpmiTransport.SendIpmiCommand = EfiBtIpmiSendCommand; // BT SendCommand
        IpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiBtIpmiSendCommandEx; // BT SendCommand Extension
    } else if (IPMI_SYSTEM_INTERFACE == SSIF_INTERFACE) { // SSIF Interface
        //
        // Locate the SMBUS PPI for Communication.
        //
        Status = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gEfiPeiSmbus2PpiGuid,
                                   0,
                                   NULL,
                                   (VOID **)&EfiPeiSmbus2Ppi);

        if (EFI_ERROR(Status)) {
            //
            // LocatePpi has failed, so freeing the allocated memory and return Status.
            //
            FreePool (IpmiInstance);
            FreePool (PeiIpmiPpiDesc);
            return Status;
        }

        IpmiInstance->IpmiTransport.SendIpmiCommand = EfiSsifIpmiSendCommand;   // SSIF SendCommand
        IpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiSsifIpmiSendCommandEx; // SSIF SendCommand Extension
        IpmiInstance->Interface.SSIF.SsifRetryCounter = IPMI_SSIF_COMMAND_REQUEST_RETRY_COUNTER; // SSIF retry counter
        IpmiInstance->Interface.SSIF.PecSupport = FALSE; // SSIF PEC support
        IpmiInstance->Interface.SSIF.RwSupport = 0x0; // SSIF multi-part reads/writes support
        IpmiInstance->Interface.SSIF.SmbAlertSupport = FALSE; // Smb alert pin support
    } else {
        //
        // Locate the I2C Ppi for communication.
        // Return if Ppi is not available.
        //
        Status = (*PeiServices)->LocatePpi (
                        PeiServices,
                        &gEfiPeiI2cMasterPpiGuid,
                        0,
                        NULL,
                        (VOID **)&MasterTransmit);
        if(EFI_ERROR(Status)) {
            //
            // LocatePpi has failed, so freeing the allocated memory and return Status.
            //
            FreePool (IpmiInstance);
            FreePool (PeiIpmiPpiDesc);
            return Status;
        }
        IpmiInstance->IpmiTransport.SendIpmiCommand = EfiIpmbIpmiSendCommand; // IPMB SendCommand
        IpmiInstance->IpmiTransport.SendIpmiCommandEx = EfiIpmbIpmiSendCommandEx; // IPMB SendCommand Extension
    }

    IpmiInstance->Signature     = SM_IPMI_BMC_SIGNATURE;
    IpmiInstance->SlaveAddress  = IPMI_BMC_SLAVE_ADDRESS;
    IpmiInstance->BmcStatus     = EFI_BMC_OK;
    IpmiInstance->IpmiTransport.GetBmcStatus = EfiIpmiBmcStatus;

#if BMC_INIT_DELAY

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "ServerMgmtConfiguration.WaitForBmc: %x \n", ServerMgmtConfiguration.WaitForBmc));

    //
    // Add 30 Sec delay before sending Self Test command during First AC Power Cycle On.
    // BMC starts simultaneously with BIOS so BMC takes 30 Sec to respond.
    // Give a delay of 30 Sec if the Power Failure (PWR_FLR) bit is set and WaitForBmc Setup
    // option is enabled.
    //
    if (ServerMgmtConfiguration.WaitForBmc && BmcInitDelayHook(PeiServices)) {

        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Giving DELAY_OF_THIRTY_SECOND Sec delay.. start \n"));
        MicroSecondDelay (DELAY_OF_THIRTY_SECOND);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Giving DELAY_OF_THIRTY_SECOND Sec delay.. end \n"));

    }
#endif

    //
    // Check SSIF interface capability
    //
    if (IPMI_SYSTEM_INTERFACE == SSIF_INTERFACE) {
        GetSystemInterfaceCapability(IpmiInstance);
        SetGlobalEnable(IpmiInstance);
    }

    //
    // Get the BMC SELF TEST Results.
    //

    for (Index = 0; Index < IPMI_SELF_TEST_COMMAND_RETRY_COUNT; Index++) {

        DataSize = sizeof (BstStatus);
        Status = IpmiInstance->IpmiTransport.SendIpmiCommand (
                    &IpmiInstance->IpmiTransport,
                    IPMI_NETFN_APP,
                    BMC_LUN,
                    IPMI_APP_GET_SELFTEST_RESULTS,
                    NULL,
                    0,
                   (UINT8*) &BstStatus,
                    &DataSize );
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "Get Self test results Index: %x Status %r\n", Index, Status));
        if (!EFI_ERROR(Status)) {
            break;
        }
    }

    DEBUG ((EFI_D_INFO, "Self test result Status: %r\n", Status));

    //
    // If Status indicates a Device error, then the BMC is not responding, so send an error.
    //
    if (EFI_ERROR (Status)) {
        IpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
    } else {

        //
        // Check the self test results.  Cases 55h - 58h are IPMI defined test results.
        // Additional Cases are device specific test results.
        //
        switch (BstStatus.Result) {
            case IPMI_APP_SELFTEST_NO_ERROR: // 0x55
            case IPMI_APP_SELFTEST_NOT_IMPLEMENTED: // 0x56
            case IPMI_APP_SELFTEST_RESERVED: // 0xFF
                IpmiInstance->BmcStatus = EFI_BMC_OK;
                break;

            case IPMI_APP_SELFTEST_ERROR: // 0x57
                IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
                break;

            default: // 0x58 and Other Device Specific Hardware Error
                IpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
                break;
        }

        //
        // The Device ID information is only retrieved when the self test has passed or generated a SOFTFAIL.
        // The BMC Status may be overwritten to show the BMC is in Force Update mode.
        // But the BMC Status will always be preserved if a hard fail has occurred since this code won't execute.
        //
        if ((IpmiInstance->BmcStatus == EFI_BMC_OK) || (IpmiInstance->BmcStatus == EFI_BMC_SOFTFAIL))
        {
            //
            // Send Get Device ID command
            //
            GetDeviceId (IpmiInstance);
        }
    }

    DEBUG ((EFI_D_INFO, "BMC Status %x\n", IpmiInstance->BmcStatus));

    //
    // Now install the PPI if the BMC is not in a Hard Fail State
    //
    if (IpmiInstance->BmcStatus != EFI_BMC_HARDFAIL) {
        PeiIpmiPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
        PeiIpmiPpiDesc->Guid = &gEfiPeiIpmiTransportPpiGuid;
        PeiIpmiPpiDesc->Ppi = &IpmiInstance->IpmiTransport;

        Status = (*PeiServices)->InstallPpi (
                    PeiServices,
                    PeiIpmiPpiDesc);
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "gEfiPeiIpmiTransportPpiGuid PPI  Status %r\n", Status));
        if (EFI_ERROR(Status)) {
            //
            // Error in installing the PPI. So free the allocated memory.
            //
            FreePool (IpmiInstance);
            FreePool (PeiIpmiPpiDesc);
        }
        return Status;

    } else {
        //
        // SELF test has failed, so free the memory and return EFI_UNSUPPORTED to unload driver from memory.
        //
        FreePool (IpmiInstance);
        FreePool (PeiIpmiPpiDesc);
        return EFI_UNSUPPORTED;
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
