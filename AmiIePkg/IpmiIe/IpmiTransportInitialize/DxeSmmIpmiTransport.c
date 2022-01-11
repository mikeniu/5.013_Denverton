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

/** @file DxeSmmIpmiTransport.c
    Establishes communication to use SMM services for DXE case

**/

//----------------------------------------------------------------------

#include <Library/UefiLib.h>
#include "DxeSmmIpmiTransport.h"

//----------------------------------------------------------------------

#if IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

//
// Extern Variables
//
extern EFI_IPMI_IE_INSTANCE_DATA   *gIpmiInstance;

//
// Global variables
//
EFI_SMM_COMMUNICATION_PROTOCOL  *gSmmCommunication = NULL;
UINT8                           *gCommunicateBuffer = NULL;
UINTN                            gCommunicateBufferSize;

/**
    Calls the IPMI send command SMM service for Non SMM case

    @param This Pointer to IPMI Protocol/PPI
    @param NetFunction Net function of the command
    @param LUN Logical Unit number of the command
    @param Command Command to be sent to IE
    @param CommandData Command data to be sent along with Command
    @param CommandDataSize Command Data size
    @param ResponseData Response Data from IE
    @param ResponseDataSize Size of the Response data

    @retval EFI_SUCCESS Status returned from IE while executing the command.

**/

EFI_STATUS
SmmEfiIpmiSendCommand (
  IN EFI_IPMI_TRANSPORT    *This,
  IN UINT8                 NetFunction,
  IN UINT8                 Lun,
  IN UINT8                 Command,
  IN UINT8                 *CommandData,
  IN UINT8                 CommandDataSize,
  OUT UINT8                *ResponseData,
  OUT UINT8                *ResponseDataSize )
{
    EFI_STATUS                                    Status;
    EFI_SMM_COMMUNICATE_HEADER                   *SmmCommunicateHeader;
    SMM_IPMI_IE_SEND_COMMAND_COMMUNICATE_DATA   *SmmIpmiIeData;

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SmmEfiIpmiSendCommand entered  IpmiInstance: %x \n", gIpmiInstance));

    //
    // Copy SMM Communicate Header Here
    //
    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)gCommunicateBuffer;
    CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gEfiSmmIpmiIeTransportProtocolGuid);
    SmmCommunicateHeader->MessageLength = SMM_IPMI_IE_SEND_COMMAND_COMMUNICATE_DATA_SIZE;

    //
    // Copy SMM IPMI IE Communicate Data Here
    //
    SmmIpmiIeData = (SMM_IPMI_IE_SEND_COMMAND_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;
    SmmIpmiIeData->FunctionNumber = SMM_IPMI_IE_FUNCTION_SEND_COMMAND;
    SmmIpmiIeData->NetFunction = NetFunction;
    SmmIpmiIeData->Lun = Lun;
    SmmIpmiIeData->Command = Command;
    SmmIpmiIeData->CommandData = (UINTN) CommandData;
    SmmIpmiIeData->CommandDataSize = CommandDataSize;
    SmmIpmiIeData->ResponseData = (UINTN) ResponseData;
    SmmIpmiIeData->ResponseDataSize = (UINTN) ResponseDataSize;

    //
    // Specify some initial value to data's return status
    //
    SmmIpmiIeData->ReturnStatus = EFI_PROTOCOL_ERROR;

    //
    // Send data to SMM.
    //
    gCommunicateBufferSize = SMM_COMMUNICATE_HEADER_SIZE + SMM_IPMI_IE_SEND_COMMAND_COMMUNICATE_DATA_SIZE;
    Status = gSmmCommunication->Communicate (gSmmCommunication, gCommunicateBuffer, &gCommunicateBufferSize);
    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "gSmmCommunication->Communicate  Status: %r \n", Status));
    ASSERT_EFI_ERROR (Status);

    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *) gCommunicateBuffer;
    SmmIpmiIeData = (SMM_IPMI_IE_SEND_COMMAND_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SmmIpmiIeData->ReturnStatus : %r \n", SmmIpmiIeData->ReturnStatus));

    This->CommandCompletionCode = SmmIpmiIeData->CommandCompletionCode;

    //
    // Status returned by the SMM based routine.
    //
    return  SmmIpmiIeData->ReturnStatus;
}

/**
    Calls the IPMI IE status SMM service for Non SMM case

    @param This Pointer to IPMI Protocol/PPI
    @param IeStatus State of the IE
    @param ComAddress IE Communication address

    @retval EFI_SUCCESS Status returned

**/

EFI_STATUS
SmmEfiIpmiIeStatus (
  IN EFI_IPMI_TRANSPORT     *This,
  OUT EFI_IE_STATUS         *IeStatus,
  OUT EFI_SM_COM_ADDRESS    *ComAddress )
{

    EFI_STATUS                                   Status;
    EFI_SMM_COMMUNICATE_HEADER                   *SmmCommunicateHeader;
    SMM_IPMI_IE_GET_STATUS_COMMUNICATE_DATA      *SmmIpmiGetIeStatusData;

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SmmEfiIpmiSendCommand entered  IpmiInstance: %x \n", gIpmiInstance));

    //
    // Copy SMM Communicate Header Here
    //
    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)gCommunicateBuffer;
    CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gEfiSmmIpmiIeTransportProtocolGuid);
    SmmCommunicateHeader->MessageLength = SMM_IPMI_IE_GET_STATUS_COMMUNICATE_DATA_SIZE;

    //
    // Copy SMM IPMI Get IE Communicate Data Here
    //
    SmmIpmiGetIeStatusData = (SMM_IPMI_IE_GET_STATUS_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;
    SmmIpmiGetIeStatusData->FunctionNumber = SMM_IPMI_IE_FUNCTION_GET_IE_STATUS;
    SmmIpmiGetIeStatusData->IeStatus = (UINTN) IeStatus;
    SmmIpmiGetIeStatusData->ComAddress = (UINTN) ComAddress;

    //
    // Specify some initial value to data's return status
    //
    SmmIpmiGetIeStatusData->ReturnStatus = EFI_PROTOCOL_ERROR;

    //
    // Send data to SMM.
    //
    gCommunicateBufferSize = SMM_COMMUNICATE_HEADER_SIZE + SMM_IPMI_IE_GET_STATUS_COMMUNICATE_DATA_SIZE;
    Status = gSmmCommunication->Communicate (gSmmCommunication, gCommunicateBuffer, &gCommunicateBufferSize);
    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "gSmmCommunication->Communicate  Status: %r \n", Status));
    ASSERT_EFI_ERROR (Status);

    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *) gCommunicateBuffer;
    SmmIpmiGetIeStatusData = (SMM_IPMI_IE_GET_STATUS_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SmmIpmiGetIeStatusData->ReturnStatus : %r \n", SmmIpmiGetIeStatusData->ReturnStatus));

    //
    // Status returned by the SMM based routine.
    //
    return  SmmIpmiGetIeStatusData->ReturnStatus;
}

/**
    Notification function for SmmIpmi Transport protocol.
    Performs necessary steps to proxy the DXE calls to SMM

    @param Event Event which caused this handler
    @param Context Context passed during Event Handler registration

    @retval VOID

**/

VOID
EFIAPI
SmmIpmiIeTransportProtocolReady (
  IN  EFI_EVENT         Event,
  IN  VOID              *Context )
{
    EFI_STATUS                    Status;
    EFI_IPMI_TRANSPORT            *DummySmmmIpmiTransport = NULL;

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SmmIpmiIeTransportProtocolReady entered IpmiInstance: %x \n", gIpmiInstance));
    Status = gBS->LocateProtocol (&gEfiSmmIpmiIeTransportProtocolGuid, NULL, (VOID **)&DummySmmmIpmiTransport);
    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "gBS->LocateProtocol gEfiSmmIpmiIeTransportProtocolGuid protocol  status %r\n", Status));
    if (EFI_ERROR (Status)) {
        return;
    }

    Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &gSmmCommunication);
    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "gBS->LocateProtocol gEfiSmmCommunicationProtocolGuid protocol  status %r\n", Status));
    if (EFI_ERROR (Status)) {
        return;
    }

    //
    // Allocate memory for variable store.
    //
    gCommunicateBufferSize  = SMM_COMMUNICATE_HEADER_SIZE + SMM_IPMI_IE_SEND_COMMAND_COMMUNICATE_DATA_SIZE;
    gCommunicateBuffer      = AllocateRuntimeZeroPool (gCommunicateBufferSize);
    ASSERT (gCommunicateBuffer != NULL);

    //
    // Override SendIpmiCommand and GetIeStatus entries in IpmiTransport protocol
    //
    gIpmiInstance->IpmiTransport.SendIpmiCommand = SmmEfiIpmiSendCommand;
    gIpmiInstance->IpmiTransport.GetIeStatus = SmmEfiIpmiIeStatus;

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "SmmIpmiIeTransportProtocolReady Exiting....  IpmiInstance: %x \n", gIpmiInstance));

    return;
}

/**
    Hook to register Notification function for SmmIpmi Transport protocol.

    @param VOID

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/

EFI_STATUS
InitializeIpmiPhysicalLayerHook (
  VOID )
{
    VOID        *SmmIpmiIeRegistration;

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "InitializeIpmiPhysicalLayerHook entered  gIpmiInstance: %x \n", gIpmiInstance));

    //
    // Create Notification event for SmmDxeIpmiIe GUID
    //
    EfiCreateProtocolNotifyEvent (
        &gEfiSmmIpmiIeTransportProtocolGuid,
        TPL_CALLBACK,
        SmmIpmiIeTransportProtocolReady,
        NULL,
        &SmmIpmiIeRegistration
        );

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, "InitializeIpmiPhysicalLayerHook exiting.......\n"));
    return EFI_SUCCESS;

}

#endif //IPMI_SEPARATE_DXE_SMM_INTERFACES == 0

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
