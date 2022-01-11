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

/** @file PeiIpmiTransportInitialize.c
    PEI IPMI Transport Driver.

**/

//----------------------------------------------------------------------

#include "IpmiIeCommunicate.h"

EFI_STATUS
IeInstallNotify (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN  VOID                         *Ppi );

EFI_STATUS     InitializeCall (  IN  EFI_PEI_SERVICES             **PeiServices);
static EFI_PEI_NOTIFY_DESCRIPTOR  mIeNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gPeiIeHeciPpiGuid,
    IeInstallNotify
  }
};

/**

    IE PPI Install Notify call back

    @param PeiServices         Pointer to PEI Services.
    @param NotifyDescriptor    Pointer to notify descriptor
    @param Ppi                 Pointer to Ppi

    @return EFI_STATUS Return Status

**/
EFI_STATUS
IeInstallNotify (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN  VOID                         *Ppi )
{

    EFI_STATUS                      Status;
    IE_HECI_PPI_EXT                 *IePpi;


    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, " IeInstallNotify entered \n"));
    //
    // Locate the IE Ppi for communication.
    // Return if Ppi is not available.
    //
    Status = (*PeiServices)->LocatePpi (
                    (CONST EFI_PEI_SERVICES **)PeiServices,
                    &gPeiIeHeciPpiGuid,
                    0,
                    NULL,
                    (VOID **)&IePpi);
    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "IE PPI Locate status %r \n", Status));
    if(EFI_ERROR(Status)) {
        return Status;
    }

    IePpi->Ppi.InitializeHeci (&IePpi->Ppi);

    InitializeCall (PeiServices);
    return Status;
}

/**
    Execute the Get Device ID command to determine whether or not the IE is in
    Force Update Mode. If it is, then report it to the error manager.

    @param IpmiInstance Data structure describing IE
               variables and used for sending commands

    @return EFI_STATUS Return Status of the Sent Command
    @retval EFI_SUCCESS Command executed Successfully

**/
EFI_STATUS
GetDeviceId (
  IN  OUT  EFI_IPMI_IE_INSTANCE_DATA  *IpmiInstance )
{
    EFI_STATUS                            Status;
    UINT8                                 DataSize;
    EFI_SM_CTRL_INFO                      ControllerInfo;

    DataSize = sizeof (EFI_SM_CTRL_INFO);

    //
    // Get the device ID information for the IE.
    //
    Status = IpmiInstance->IpmiTransport.SendIpmiCommand (
                &IpmiInstance->IpmiTransport,
                EFI_SM_NETFN_APP,
                IE_LUN,
                EFI_APP_GET_DEVICE_ID,
                NULL,
                0,
                (UINT8*)&ControllerInfo,
                &DataSize );

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "Get Device ID Command: Status %r\n", Status));
    //
    // If there is no error then proceed to check the data returned by the IE
    //
    if (!EFI_ERROR(Status)) {

        //
        // If the controller is in Update Mode then set the IE Status to indicate
        // the IE is in force update mode.
        //
        if (ControllerInfo.UpdateMode != 0) {
            IpmiInstance->IeStatus = EFI_IE_UPDATE_IN_PROGRESS;
        }
    }
    return Status;
}

/**
    Setup and initialize the IE for the DXE phase. In order to
    verify the IE is functioning as expected, the IE Self-test is performed.
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
InitializeIpmiIePhysicalLayer (
  IN        EFI_PEI_FILE_HANDLE     FileHandle,
  IN  CONST EFI_PEI_SERVICES        **PeiServices )
{

    EFI_STATUS                              Status = EFI_SUCCESS;
    IE_HECI_PPI_EXT                         *IePpi;
    EFI_STATUS                              NotifyStatus = EFI_SUCCESS;
    //
    // Locate the IE Ppi for communication.
    // Return if Ppi is not available.
    //
    Status = (*PeiServices)->LocatePpi (
                    PeiServices,
                    &gPeiIeHeciPpiGuid,
                    0,
                    NULL,
                    (VOID **)&IePpi);
    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "IE PPI Locate status %r \n", Status));
    if(EFI_ERROR(Status)) {

        //
        // Install Notify for IE Ppi Installation.
        //
        NotifyStatus = (*PeiServices)->NotifyPpi (
                    PeiServices,
                    mIeNotifyList);
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "IE PPI Notify register status %r \n", NotifyStatus));

    }

    if (!EFI_ERROR(Status)) {
        IePpi->Ppi.InitializeHeci(&IePpi->Ppi);
        Status = InitializeCall ((EFI_PEI_SERVICES**)PeiServices);
    }
    return Status;
}
EFI_STATUS
InitializeCall (
  IN  EFI_PEI_SERVICES             **PeiServices
 )
{
    EFI_STATUS                              Status = EFI_SUCCESS;
    EFI_IPMI_IE_INSTANCE_DATA              *IpmiInstance = NULL;
    UINT8                                   DataSize;
    UINT8                                   Index;
    EFI_PEI_PPI_DESCRIPTOR                  *PeiIpmiPpiDesc = NULL;
    EFI_GET_SELF_TEST_STATUS                BstStatus;

    //
    // Allocate memory for IPMI Instance
    //
    IpmiInstance = (EFI_IPMI_IE_INSTANCE_DATA*)AllocateZeroPool (sizeof (EFI_IPMI_IE_INSTANCE_DATA));
    if (IpmiInstance == NULL) {
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "Failed to allocate memory for IpmiInstance! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Allocate memory for PPI Descriptor
    //
    PeiIpmiPpiDesc = (EFI_PEI_PPI_DESCRIPTOR*)AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
    if (PeiIpmiPpiDesc == NULL) {
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "Failed to allocate memory for PeiIpmiPpiDesc! \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    IpmiInstance->IpmiTransport.SendIpmiCommand = EfiIeDcmiSendCommand; // IE DCMI Communication

    IpmiInstance->Signature = SM_IPMI_IE_SIGNATURE;
    IpmiInstance->SlaveAddress = IPMI_IE_SLAVE_ADDRESS;
    IpmiInstance->IeStatus = EFI_IE_OK;
    IpmiInstance->IpmiTransport.GetIeStatus = EfiIpmiIeStatus;

    //
    // Get the IE SELF TEST Results.
    //

    for ( Index = 0; Index < IPMI_SELF_TEST_COMMAND_RETRY_COUNT; Index++ ) {

        DataSize = sizeof (EFI_GET_SELF_TEST_STATUS);
        Status = IpmiInstance->IpmiTransport.SendIpmiCommand (
                    &IpmiInstance->IpmiTransport,
                    EFI_SM_NETFN_APP,
                    IE_LUN,
                    EFI_APP_GET_SELFTEST_RESULTS,
                    NULL,
                    0,
                   (UINT8*) &BstStatus,
                    &DataSize );
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "Get Self test results Index: %x Status %r\n", Index, Status));
        if ( !EFI_ERROR(Status) ) {
            break;
        }
    }

    DEBUG ((EFI_D_LOAD, "Self test result Status: %r\n", Status));

    //
    // If Status indicates a Device error, then the IE is not responding, so send an error.
    //
    if ( EFI_ERROR (Status) ) {
        IpmiInstance->IeStatus = EFI_IE_HARDFAIL;
    } else {

        //
        // Check the self test results.  Cases 55h - 58h are IPMI defined test results.
        // Additional Cases are device specific test results.
        //
        switch ( BstStatus.Status ) {
            case EFI_APP_SELFTEST_NO_ERROR: // 0x55
            case EFI_APP_SELFTEST_NOT_IMPLEMENTED: // 0x56
            case EFI_APP_SELFTEST_RESERVED: // 0xFF
                IpmiInstance->IeStatus = EFI_IE_OK;
                break;

            case EFI_APP_SELFTEST_ERROR: // 0x57
                IpmiInstance->IeStatus = EFI_IE_SOFTFAIL;
                break;

            default: // 0x58 and Other Device Specific Hardware Error
                IpmiInstance->IeStatus = EFI_IE_HARDFAIL;
                break;
        }

        //
        // The Device ID information is only retrieved when the self test has passed or generated a SOFTFAIL.
        // The IE Status may be overwritten to show the IE is in Force Update mode.
        // But the IE Status will always be preserved if a hard fail has occurred since this code won't execute.
        //
        if ( (IpmiInstance->IeStatus == EFI_IE_OK) || (IpmiInstance->IeStatus == EFI_IE_SOFTFAIL) )
        {
            //
            // Send Get Device ID command
            //
            GetDeviceId (IpmiInstance);
        }
    }

    DEBUG ((EFI_D_LOAD, "IE Status %x\n", IpmiInstance->IeStatus));

    //
    // Now install the PPI if the IE is not in a Hard Fail State
    //
    if ( IpmiInstance->IeStatus != EFI_IE_HARDFAIL ) {
        PeiIpmiPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
        PeiIpmiPpiDesc->Guid = &gEfiPeiIpmiIeTransportPpiGuid;
        PeiIpmiPpiDesc->Ppi = &IpmiInstance->IpmiTransport;

        Status = (*PeiServices)->InstallPpi (
                    PeiServices,
                    PeiIpmiPpiDesc);
        SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "gEfiPeiIpmiIeTransportPpiGuid PPI  Status %r\n", Status));
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
