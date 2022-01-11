//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SmmIpmiTransportInitialize.c
    SMM IPMI Transport Driver.

**/

//----------------------------------------------------------------------

#include "DxeSmmIpmiTransport.h"
#include <Library/SmmServicesTableLib.h>

//----------------------------------------------------------------------

//
// Global Variable
//
EFI_IPMI_IE_INSTANCE_DATA  *gSmmIpmiInstance = NULL;
IE_HECI_PROTOCOL            *gIeProtocol = NULL;

EFI_STATUS InitializeSmmIpmiCall (VOID);

/**

    IE protocol Install Notify call back

    @param Event    Event Notify event
    @param Context  Context Pointer

    @retval VOID

**/
EFI_STATUS
IeSmmInstallNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle)
{

    EFI_STATUS    Status;

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, " IeSmmInstallNotify entered.\n"));

    Status = gSmst->SmmLocateProtocol(
                      &gSmmIeHeciProtocolGuid,
                      NULL,
                      (VOID**)&gIeProtocol);

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "IE Protocol Locate status %r \n", Status));
    if(EFI_ERROR(Status)) {
        return Status;
    }

    gIeProtocol->InitializeHeci(gIeProtocol);
    InitializeSmmIpmiCall ();
    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, " IeSmmInstallNotify Exit\n"));
    return Status;
}

#if IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

/**
    Software SMI callback which proxies the DXE calls to SMM IPMI protocol

    @param DispatchHandle The unique handle assigned to this handler by
                           SmiHandlerRegister().
    @param RegisterContext Points to an optional handler context which was
                          specified when the handler was registered.
    @param CommBuffer A pointer to a collection of data in memory that will
                      be conveyed from a non-SMM environment into an SMM environment.
    @param CommBufferSize The size of the CommBuffer.

    @retval EFI_STATUS Return Status

**/

EFI_STATUS
EFIAPI
SmmIpmiIeHandler (
  IN     EFI_HANDLE         DispatchHandle,
  IN     CONST VOID         *RegisterContext,
  IN OUT VOID               *CommBuffer,
  IN OUT UINTN              *CommBufferSize )
{
    EFI_STATUS                                      Status = EFI_SUCCESS;
    SMM_IPMI_IE_SEND_COMMAND_COMMUNICATE_DATA      *SmmIpmiIeData;
    SMM_IPMI_IE_GET_STATUS_COMMUNICATE_DATA        *SmmIpmiGetIeStatusData;

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SmmIpmiIeHandler entered  status %r gSmmIpmiInstance: %x \n", Status, gSmmIpmiInstance));

    ASSERT (CommBuffer != NULL);

    SmmIpmiIeData = (SMM_IPMI_IE_SEND_COMMAND_COMMUNICATE_DATA *)CommBuffer;

    switch (SmmIpmiIeData->FunctionNumber) {

        case SMM_IPMI_IE_FUNCTION_SEND_COMMAND:
            SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SendIpmiCommand function is called.............. \n"));
            Status = gSmmIpmiInstance->IpmiTransport.SendIpmiCommand (
                        &gSmmIpmiInstance->IpmiTransport,
                        (UINT8)SmmIpmiIeData->NetFunction,
                        (UINT8)SmmIpmiIeData->Lun,
                        (UINT8)SmmIpmiIeData->Command,
                        (UINT8*)SmmIpmiIeData->CommandData,
                        (UINT8)SmmIpmiIeData->CommandDataSize,
                        (UINT8*)SmmIpmiIeData->ResponseData,
                        (UINT8*)SmmIpmiIeData->ResponseDataSize );
            break;

        case SMM_IPMI_IE_FUNCTION_GET_IE_STATUS:
            SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "GetIeStatus function is called.............. \n"));
            SmmIpmiGetIeStatusData = (SMM_IPMI_IE_GET_STATUS_COMMUNICATE_DATA *)CommBuffer;
            Status = gSmmIpmiInstance->IpmiTransport.GetIeStatus (
                        &gSmmIpmiInstance->IpmiTransport,
                        (EFI_IE_STATUS*)SmmIpmiGetIeStatusData->IeStatus,
                        (EFI_SM_COM_ADDRESS*)SmmIpmiGetIeStatusData->ComAddress );
            break;

        default:
            ASSERT (FALSE);
            Status = EFI_UNSUPPORTED;
    }

    SmmIpmiIeData->CommandCompletionCode = gSmmIpmiInstance->IpmiTransport.CommandCompletionCode;
    SmmIpmiIeData->ReturnStatus = Status;

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SmmIpmiIeHandler Exiting Function  Status %r\n", Status));

    return EFI_SUCCESS;
}

#endif //IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

/**
    Setup and initialize the IE for the SMM phase.
    1. In order to verify the IE is functioning as expected, the IE Self-test
    is performed. Updates the IE status in Private data
    2. Installs SMM IPMI transport protocol.
    3. Registers Software SMI callback to proxy the DXE calls to SMM IPMI protocol.
    This applicable only when both DXE and SMM uses same KCS interface.
    4. Notify the DxeIpmiIe driver that SmmIpmiIeTransport protocol is ready.

        
    @param ImageHandle Handle of this driver image
    @param SystemTable Table containing standard EFI services

    @retval EFI_SUCCESS Successful driver initialization

**/

EFI_STATUS
InitializeSmmIpmiIePhysicalLayer (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{

    EFI_STATUS           Status = EFI_SUCCESS;
    VOID                 *IeReg;
    
    Status = gSmst->SmmLocateProtocol(
                          &gSmmIeHeciProtocolGuid,
                          NULL,
                          (VOID**)&gIeProtocol);

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "IE Protocol Locate status %r \n", Status));
    if(EFI_ERROR(Status)) {

       Status = gSmst->SmmRegisterProtocolNotify (
                            &gSmmIeHeciProtocolGuid,
                           IeSmmInstallNotify,
                           &IeReg);
       SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "IE Protocol Notify create status %r \n", Status));
       return Status;
    } else if (!EFI_ERROR(Status)) {
         gIeProtocol->InitializeHeci(gIeProtocol);
         Status = InitializeSmmIpmiCall();
    }
    return Status;
}

EFI_STATUS 
InitializeSmmIpmiCall(
  IN VOID  ) 
{
    EFI_STATUS                            Status = EFI_SUCCESS;
    EFI_HANDLE                            NewHandle = NULL;
    UINT8                                 DataSize;
    UINT8                                 Index;
    EFI_GET_SELF_TEST_STATUS              BstStatus;

    //
    // Allocate memory for IPMI Instance
    //
    gSmmIpmiInstance = AllocateRuntimeZeroPool (sizeof (EFI_IPMI_IE_INSTANCE_DATA));
    if (gSmmIpmiInstance == NULL) {
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "Failed to allocate memory for gSmmIpmiInstance! \n"));
        return EFI_OUT_OF_RESOURCES;
    }
    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "gSmmIpmiInstance: %x \n", gSmmIpmiInstance));

    gSmmIpmiInstance->IpmiTransport.SendIpmiCommand = EfiIeDcmiSendCommand;

    gSmmIpmiInstance->Signature     = SM_IPMI_IE_SIGNATURE;
    gSmmIpmiInstance->SlaveAddress  = IPMI_IE_SLAVE_ADDRESS;
    gSmmIpmiInstance->IeStatus     = EFI_IE_OK;
    gSmmIpmiInstance->IpmiTransport.GetIeStatus = EfiIpmiIeStatus;

    //
    // Get the IE SELF TEST Results.
    //

    for ( Index = 0; Index < IPMI_SELF_TEST_COMMAND_RETRY_COUNT; Index++ )  {
        DataSize = sizeof (EFI_GET_SELF_TEST_STATUS);
        Status = gSmmIpmiInstance->IpmiTransport.SendIpmiCommand (
                    &gSmmIpmiInstance->IpmiTransport,
                    EFI_SM_NETFN_APP,
                    IE_LUN,
                    EFI_APP_GET_SELFTEST_RESULTS,
                    NULL,
                    0,
                    (UINT8*)&BstStatus ,
                    &DataSize );
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SMM: Get Self test results status %r\n", Status));
        if ( Status != EFI_NOT_READY ) {
            break;
        }
    }

    //
    // If Status indicates a Device error, then the IE is not responding, so send an error.
    //
    if ( EFI_ERROR (Status) ) {
        gSmmIpmiInstance->IeStatus = EFI_IE_HARDFAIL;
    } else {

        //
        // Check the self test results.  Cases 55h - 58h are IPMI defined test results.
        // Additional Cases are device specific test results.
        //
        switch ( BstStatus.Status ) {
            case EFI_APP_SELFTEST_NO_ERROR: // 0x55
            case EFI_APP_SELFTEST_NOT_IMPLEMENTED: // 0x56
            case EFI_APP_SELFTEST_RESERVED: // 0xFF
                gSmmIpmiInstance->IeStatus = EFI_IE_OK;
                break;

            case EFI_APP_SELFTEST_ERROR: // 0x57
                gSmmIpmiInstance->IeStatus = EFI_IE_SOFTFAIL;
                break;

            default: // 0x58 and Other Device Specific Hardware Error
                gSmmIpmiInstance->IeStatus = EFI_IE_HARDFAIL;
                break;
        }
    }
    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SMM IE status %x\n", gSmmIpmiInstance->IeStatus));

    //
    // Now install the Protocol if the IE is not in a Hard Fail State
    //
    if ( gSmmIpmiInstance->IeStatus != EFI_IE_HARDFAIL ) {
        NewHandle = NULL;
        Status = gSmst->SmmInstallProtocolInterface (
                    &NewHandle,
                    &gEfiSmmIpmiIeTransportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gSmmIpmiInstance->IpmiTransport );
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SMM gEfiSmmIpmiIeTransportProtocolGuid protocol status %r\n", Status));
        if (EFI_ERROR (Status)) {

            //
            // Error in installing the protocol. So free the allocated memory.
            //
            FreePool (gSmmIpmiInstance);
            return Status;
        }

#if IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

        //
        // Same Interface used for both SMM and DXE phases so proxy the DXE calls to SMM
        //
        if ( !EFI_ERROR(Status) ) {

            //
            // Register SMM IPMI IE SMI handler
            //
            NewHandle = NULL;
            Status = gSmst->SmiHandlerRegister (
                        SmmIpmiIeHandler,
                        &gEfiSmmIpmiIeTransportProtocolGuid,
                        &NewHandle);
            SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "gSmst->SmiHandlerRegister  status %r\n", Status));
            ASSERT_EFI_ERROR (Status);

            if ( !EFI_ERROR(Status) ) {

                //
                // Notify the DxeIpmiIe driver that SmmIpmiIeTransport protocol is ready
                //
                NewHandle = NULL;
                Status = gBS->InstallProtocolInterface (
                            &NewHandle,
                            &gEfiSmmIpmiIeTransportProtocolGuid,
                            EFI_NATIVE_INTERFACE,
                            NULL );
                SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "gBS->Install: gEfiSmmIpmiIeTransportProtocolGuid protocol  status %r\n", Status));
                ASSERT_EFI_ERROR (Status);
            }
        }
#endif //IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

        return EFI_SUCCESS;

    } else {

        //
        // SELF test has failed, so free the memory and return EFI_UNSUPPORTED to unload driver from memory.
        //
        gSmst->SmmFreePool (gSmmIpmiInstance);
        return EFI_UNSUPPORTED;
    }

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
