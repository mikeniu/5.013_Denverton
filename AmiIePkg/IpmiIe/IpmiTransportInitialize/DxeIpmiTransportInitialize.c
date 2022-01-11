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

/** @file DxeIpmiTransportInitialize.c
    IPMI Transport Driver.

**/

//----------------------------------------------------------------------

#include "IpmiIeCommunicate.h"
#include <Guid/EventGroup.h>

//----------------------------------------------------------------------

//
// Global Variables
//
EFI_IPMI_IE_INSTANCE_DATA       *gIpmiInstance = NULL;
IE_HECI_PROTOCOL                 *gIeProtocol = NULL;

VOID InitAmiLib(
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  );

EFI_STATUS InitializeDxeIpmiCall (VOID);

/**

    IE protocol Install Notify call back

    @param Event    Event Notify event
    @param Context  Context Pointer

    @retval VOID

**/
VOID
IeDxeInstallNotify (
  IN  EFI_EVENT     Event,
  IN  VOID          *Context )
{

    EFI_STATUS    Status;

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, " IeDxeInstallNotify entered.\n"));

    Status = gBS->LocateProtocol(
                    &gDxeIeHeciProtocolGuid,
                    NULL,
                    (VOID**)&gIeProtocol);

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "IE Protocol Locate status %r \n", Status));
    if(EFI_ERROR(Status)) {
        return;
    }

    gIeProtocol->InitializeHeci(gIeProtocol);
    InitializeDxeIpmiCall ();
    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, " IeDxeInstallNotify Exit\n"));
    gBS->CloseEvent(Event);
}

EFI_STATUS_CODE_VALUE ErrorBitToStatusCodeMap [] = {
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_FIRMWARE_CORRUPTED,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_BB_CORRUPTED,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_IE_FRU_CORRUPTED,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SDR_EMPTY,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_IPMB_NOT_RESPONDING,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_IE_FRU_NOT_ACCESSIBLE,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SDR_REPOSITORY_NOT_ACCESSABLE,
                                                     EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SEL_NOT_ACCESSIBLE
                                                   };

/**
    Notification function for Virtual address change event.
    Convert all addresses inside IpmiTransportProtocol

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @return VOID

**/

VOID
DxeIpmiIntializeVirtualAddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{
    //
    // Change all addresses inside IpmiTransportProtocol stored locally
    //
    gRT->ConvertPointer (
            0,
            (VOID **) &(gIpmiInstance->IpmiTransport.SendIpmiCommand));
    gRT->ConvertPointer (
            0,
            (VOID **) &(gIpmiInstance->IpmiTransport.GetIeStatus));
    //
    // Change the address pointed by gIpmiInstance
    //
    gRT->ConvertPointer (
                0,
                (VOID **) &gIpmiInstance);

    return;
}

/**
    Execute the Get Device ID command.

    @param ResponseData Buffer to copy the GetDeviceId response
    @param ResponseDataSize Response data size

    @return  EFI_STATUS

**/

EFI_STATUS
GetDeviceId (
 OUT  UINT8 *ResponseData,
 OUT  UINT8 *ResponseDataSize )
{
    EFI_STATUS          Status;

    Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                &gIpmiInstance->IpmiTransport,
                EFI_SM_NETFN_APP,
                IE_LUN,
                EFI_APP_GET_DEVICE_ID,
                NULL,
                0,
                ResponseData,
                ResponseDataSize );

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "Get Device ID status %r\n", Status));

    return Status;
}

/**
    Enrty point for DXE Ipmi Intialize driver

    @param ImageHandle Handle of this driver image 
    @param SystemTable Table containing standard EFI services

    @retval EFI_SUCCESS Successful driver initialization

**/
EFI_STATUS
InitializeIpmiIePhysicalLayer (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable )
{

    EFI_STATUS           Status = EFI_SUCCESS;
    EFI_EVENT            NotifyEvent;
    VOID                 *NotifyReg;
 
    InitAmiLib (ImageHandle,SystemTable);

    //
    // Locate IE Protocol for communication
    //
    Status = gBS->LocateProtocol(
                 &gDxeIeHeciProtocolGuid,
                 NULL,
                (VOID**)&gIeProtocol);
    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "IE Protocol Locate status %r \n", Status));
    if (EFI_ERROR(Status)) {
       //
       // Install IE Protocol notify
       //
       Status = gBS->CreateEvent( 
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      IeDxeInstallNotify,
                      NULL,
                      &NotifyEvent);

       SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "IE Protocol Notify event create status %r \n", Status));

       if (!EFI_ERROR(Status)) {
           Status = gBS->RegisterProtocolNotify (
                              &gDxeIeHeciProtocolGuid,
                              NotifyEvent,
                              &NotifyReg);
           SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "IE Protocol Notify register status %r \n", Status));
       }
    } else if (!EFI_ERROR(Status)) {
       gIeProtocol->InitializeHeci(gIeProtocol);
       Status = InitializeDxeIpmiCall();
    }

    return Status;
}

/**
    Setup and initialize the IE for the DXE phase. In order to 
    verify the IE is functioning as expected, the IE Self test is performed.
    The results are then checked and any errors are reported to the error manager.
    Errors are collected throughout this routine and reported just prior to
    installing the driver.  If there are more errors than BST_LOG_SIZE, then they
    will be ignored.

    @retval EFI_SUCCESS Successful driver initialization

**/

EFI_STATUS
InitializeDxeIpmiCall (
  IN VOID)
{

    EFI_STATUS                            Status = EFI_SUCCESS;
    EFI_STATUS                            StatusOfGetDeviceId = EFI_DEVICE_ERROR;
    EFI_HANDLE                            NewHandle = NULL;
    UINT8                                 DataSize;
    UINT8                                 Index;
    EFI_STATUS_CODE_VALUE                 StatusCodeValue[BST_LOG_SIZE];
    UINT8                                 ErrorCount = 0;
    EFI_EVENT                             EfiVirtualAddressChangeNotifyEvent = NULL;
    EFI_SM_CTRL_INFO                      ControllerInfo;
    EFI_GET_SELF_TEST_STATUS              BstStatus;
    UINT8                                 BitSelect = 0;
    UINT8                                 ErrorBitVal = 0;

    //
    // Allocate memory for IPMI Instance
    //
    gIpmiInstance = (EFI_IPMI_IE_INSTANCE_DATA*)AllocateRuntimeZeroPool (sizeof (EFI_IPMI_IE_INSTANCE_DATA));
    if (gIpmiInstance == NULL) {
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "Failed to allocate memory for gIpmiInstance! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    gIpmiInstance->IpmiTransport.SendIpmiCommand = EfiIeDcmiSendCommand;

    gIpmiInstance->Signature = SM_IPMI_IE_SIGNATURE;
    gIpmiInstance->SlaveAddress = IPMI_IE_SLAVE_ADDRESS;
    gIpmiInstance->IeStatus = EFI_IE_OK;
    gIpmiInstance->IpmiTransport.GetIeStatus = EfiIpmiIeStatus;

    //
    // Get the IE SELF TEST Results.
    //

    for ( Index = 0; Index < IPMI_SELF_TEST_COMMAND_RETRY_COUNT; Index++ ) {

        DataSize = sizeof (EFI_GET_SELF_TEST_STATUS);
        Status = gIpmiInstance->IpmiTransport.SendIpmiCommand (
                    &gIpmiInstance->IpmiTransport,
                    EFI_SM_NETFN_APP,
                    IE_LUN,
                    EFI_APP_GET_SELFTEST_RESULTS,
                    NULL,
                    0,
                    (UINT8*)&BstStatus,
                    &DataSize);
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "Get Self test results status %r\n", Status));
        if ( !EFI_ERROR(Status) ) {
            break;
        }
    }

    DEBUG ((EFI_D_LOAD, "Self test result Status: %r\n", Status));
    //
    // If Status indicates a Device error, then the IE is not responding, so send an error.
    //
    if ( EFI_ERROR (Status) ) {
        if ( ErrorCount < BST_LOG_SIZE ) {
            StatusCodeValue[ErrorCount++] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_COMM_ERROR;
        }
        gIpmiInstance->IeStatus = EFI_IE_HARDFAIL;
    } else {

        //
        // Check the self test results. Cases 55h - 58h are IPMI defined test results.
        // Additional Cases are device specific test results.
        //
        switch ( BstStatus.Status ) {
            case EFI_APP_SELFTEST_NO_ERROR: // 0x55
            case EFI_APP_SELFTEST_NOT_IMPLEMENTED: // 0x56
            case EFI_APP_SELFTEST_RESERVED: // 0xFF
                gIpmiInstance->IeStatus = EFI_IE_OK;
                break;

            case EFI_APP_SELFTEST_ERROR: // 0x57
                gIpmiInstance->IeStatus = EFI_IE_SOFTFAIL;
                //
                // Check the Error bit set and report the error.
                //
                ErrorBitVal = BstStatus.ErrorBitSet;
                while ( (BitSelect < 8) && ErrorBitVal ) { // Loop until all bits are checked. Initial value of BitSelect is Zero.
                    if (ErrorBitVal & BIT0) { // check if selected bit is set.
                        if (ErrorCount < BST_LOG_SIZE) {
                            StatusCodeValue[ErrorCount++] = ErrorBitToStatusCodeMap[BitSelect]; // Store the status code mapped for this error bit.
                        } else {
                            break;
                        }
                    }
                    BitSelect = BitSelect + 1; // Select the next bit to be checked. Initial value of BitSelect is Zero.
                    ErrorBitVal = ErrorBitVal >> 1; // Remove tested bit from the value.
                }
                break;

            default: // 0x58 and Other Device Specific Hardware Error
                gIpmiInstance->IeStatus = EFI_IE_HARDFAIL;
                break;
        } // switch case

        if ( gIpmiInstance->IeStatus == EFI_IE_HARDFAIL ) {
            if ( ErrorCount < BST_LOG_SIZE ) {
                StatusCodeValue[ErrorCount++] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_HARD_FAIL;
            }
        } else if ( gIpmiInstance->IeStatus == EFI_IE_SOFTFAIL ) {
            if ( ErrorCount < BST_LOG_SIZE ) {
                StatusCodeValue[ErrorCount++] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SOFT_FAIL;
            }
        }

        //
        // The Device ID information is only retrieved when the self test has passed or generated a SOFTFAIL.
        // The IE Status may be overwritten to show the IE is in Force Update mode.
        // But the IE Status will always be preserved if a hard fail has occurred since this code won't execute.
        //
        if (  (gIpmiInstance->IeStatus == EFI_IE_OK) || (gIpmiInstance->IeStatus == EFI_IE_SOFTFAIL) ) {

            //
            // Send Get Device ID command
            //
            DataSize = sizeof (EFI_SM_CTRL_INFO);
            StatusOfGetDeviceId = GetDeviceId ((UINT8*)&ControllerInfo, &DataSize);
            if(!EFI_ERROR (StatusOfGetDeviceId)) {

                //
                // If the controller is in Update Mode and the maximum number of errors
                // has not been exceeded, then save the error code to the StatusCode
                // array and increment the counter. Set the IE Status to indicate
                // the IE is in force update mode.
                //
                if ( ControllerInfo.UpdateMode != 0 ) {
                    gIpmiInstance->IeStatus = EFI_IE_UPDATE_IN_PROGRESS;
                    if ( ErrorCount < BST_LOG_SIZE ) {
                        StatusCodeValue[ErrorCount] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR\
                                | EFI_CU_FP_EC_FORCE_UPDATE_MODE;
                        ErrorCount++;
                    }
                }
            }
        }
    } // else case


    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "IE status %x\n", gIpmiInstance->IeStatus));

    for ( Index = 0; Index < ErrorCount; Index++ ) {
       ReportStatusCode (
          EFI_ERROR_CODE | EFI_ERROR_MINOR,
          StatusCodeValue[Index]
        );
    }
    //
    // Now install the Protocol if the IE is not in a Hard Fail State
    //
    if ( gIpmiInstance->IeStatus != EFI_IE_HARDFAIL ) {

        //
        // Install the protocol
        //
        NewHandle = NULL;
        Status = gBS->InstallProtocolInterface (
                    &NewHandle,
                    &gEfiDxeIpmiIeTransportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gIpmiInstance->IpmiTransport);
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "gEfiDxeIpmiIeTransportProtocolGuid protocol  status %r\n", Status));
        if (EFI_ERROR (Status)) {

            //
            // Error in installing the protocol. So free the allocated memory.
            //
            FreePool (gIpmiInstance);
            return Status;
        }
#if IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

        //
        // Same Interface used for both SMM and DXE phases so Setup for Proxy the DXE calls to SMM
        //
        InitializeIpmiPhysicalLayerHook ();

#endif //IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

        //
        // Create event for the Virtual Address Change Event
        //
        Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    DxeIpmiIntializeVirtualAddressChangeEvent,
                    NULL,
                    (CONST EFI_GUID *)&gEfiEventVirtualAddressChangeGuid,
                    &EfiVirtualAddressChangeNotifyEvent );
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "Create event for the Virtual Address Change Event  status %r\n", Status));

        return EFI_SUCCESS;

    } else {

        //
        // SELF test has failed, so free the memory and return EFI_UNSUPPORTED to unload driver from memory.
        //
        FreePool (gIpmiInstance);
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
