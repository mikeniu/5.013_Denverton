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

/** @file IpmiIeCommunicate.c
    IPMI Transport Driver.

**/

#include "IpmiIeCommunicate.h"

/**
    Updates the SoftErrorCount variable based on the IE Error input

    @param IeError - IE Error
    @param IpmiInstance - Data structure describing IE
                variables and used for sending commands

    @return EFI_STATUS 
    @retval EFI_SUCCESS is returned

**/
EFI_STATUS
UpdateSoftErrorCount (
  IN        UINT8                       IeError,
  IN OUT    EFI_IPMI_IE_INSTANCE_DATA  *IpmiInstance )
{
    UINT8   Errors[] = {0xC0, 0xC3, 0xC4, 0xC9, 0xCE, 0xCF, 0xFF, 0x00};
    UINT8   i;

    i = 0;
    while (Errors[i] != 0) {
        if (Errors[i] == IeError) {
            IpmiInstance->SoftErrorCount++;
            break;
        }
        i++;
    }
    return EFI_SUCCESS;
}

/**
    IPMI IE status Implementation

    @param  This - Pointer to IPMI Protocol/PPI
    @param  IeStatus - State of the IE
    @param  ComAddress - IE Communication address
    @param  Context - NULL here

    @retval EFI_STATUS EFI_SUCCESS Status returned

**/
EFI_STATUS
IpmiIeStatus (
  IN EFI_IPMI_TRANSPORT               *This,
  OUT EFI_IE_STATUS                   *IeStatus,
  OUT EFI_SM_COM_ADDRESS              *ComAddress,
  IN  VOID                            *Context )
{
    EFI_STATUS                            Status;
    UINT8                                 DataSize;
    EFI_SM_CTRL_INFO                      ControllerInfo;
    EFI_IPMI_IE_INSTANCE_DATA            *IpmiInstance;

    IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_IE_THIS (This);

    DataSize = sizeof (EFI_SM_CTRL_INFO);

    //
    // Get the device ID information for the IE.
    //
    Status = This->SendIpmiCommand (
                &IpmiInstance->IpmiTransport,
                EFI_SM_NETFN_APP,
                IE_LUN,
                EFI_APP_GET_DEVICE_ID,
                NULL,
                0,
                (UINT8*)&ControllerInfo,
                &DataSize );

    SERVER_IPMI_IE_DEBUG ((EFI_D_LOAD, " Get Device ID Command: Status %r\n", Status));
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
        } else {
            IpmiInstance->IeStatus = EFI_IE_OK;
        }
    } else {
        if (IpmiInstance->SoftErrorCount >= MAX_SOFT_COUNT) {
            IpmiInstance->IeStatus = EFI_IE_HARDFAIL;
        } else {
            IpmiInstance->IeStatus = EFI_IE_SOFTFAIL;
        }
    }

    *IeStatus = IpmiInstance->IeStatus;
    ComAddress->ChannelType = EfiSmIe;
    ComAddress->Address.IeAddress.SlaveAddress = IpmiInstance->SlaveAddress;
    ComAddress->Address.IeAddress.ChannelAddress = 0x0;

    return EFI_SUCCESS;
}

/**
    IPMI IE status API

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  IeStatus - State of the IE
    @param  ComAddress - IE Communication address

    @return EFI_STATUS
    @retval EFI_SUCCESS Status returned

**/
EFI_STATUS
EfiIpmiIeStatus (
  IN EFI_IPMI_TRANSPORT               *This,
  OUT EFI_IE_STATUS                   *IeStatus,
  OUT EFI_SM_COM_ADDRESS              *ComAddress )
{
    return IpmiIeStatus (This, IeStatus, ComAddress, NULL);
}

/**
    IE DCMI interface IPMI send command Implementation

    @param This -  Pointer to IPMI Protocol/PPI
    @param NetFunction - Net function of the command
    @param LUN - Logical Unit number of the command
    @param Command - Command to be sent to IE
    @param CommandData - Command data to be sent along with Command
    @param CommandDataSize - Command Data size
    @param ResponseData - Response Data from IE
    @param ResponseDataSize - Size of the Response data
    @param Context - NULL here

    @return EFI_STATUS Status returned from IE while executing the command.

**/
EFI_STATUS
IpmiIeDcmiSendCommand (
  IN EFI_IPMI_TRANSPORT               *This,
  IN UINT8                            NetFunction,
  IN UINT8                            Lun,
  IN UINT8                            Command,
  IN UINT8                            *CommandData,
  IN UINT8                            CommandDataSize,
  OUT UINT8                           *ResponseData,
  OUT UINT8                           *ResponseDataSize,
  IN VOID                             *Context )
{
    EFI_IPMI_IE_INSTANCE_DATA          *IpmiInstance;
    UINT8                               DataSize;
    EFI_STATUS                          Status;
    UINT8                               Seq = 0;
    UINT8                               Commit = 1;
    UINT8                               TempDataSize = 0;
    UINT8                               TempDataAccess;

    if (!ResponseDataSize || (!ResponseData && *ResponseDataSize)) {
        return EFI_INVALID_PARAMETER;
    }

    IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_IE_THIS (This);

    //
    // Prepare payload data.
    //
    // Pay load format
    //   Byte 1       |  Byte 2    | Byte 3 | Byte 4 | Byte 5: N-1| Byte N
    // ---------------------------------------------------------------------
    // RsSlaveAddress |  NetFn/LUN |  Seq   | Cmd    | Data       | Commit
    // ---------------------------------------------------------------------
    IpmiInstance->TempData[0] = BIOS_IE_HECI_SLAVE_ADDRESS;
    IpmiInstance->TempData[1] = (UINT8) ((NetFunction << 2) | (Lun & 0xfc));
    IpmiInstance->TempData[2] = Seq;
    IpmiInstance->TempData[3] = Command;

    if (CommandDataSize > 0) {
        if (CommandData == NULL) {
            return EFI_INVALID_PARAMETER;
        }
        if (CommandDataSize <= (MAX_TEMP_DATA - 5) ) {
            CopyMem (&IpmiInstance->TempData[4], CommandData, CommandDataSize);
            IpmiInstance->TempData[4 + CommandDataSize] = Commit;
        } else {
            return EFI_BAD_BUFFER_SIZE;
        }
    } else {
        IpmiInstance->TempData[4 + CommandDataSize] = Commit;
    }
    if ((Status = SendDataToIeDcmi (IpmiInstance, Context, IpmiInstance->TempData,
                                   (CommandDataSize + 5))) != EFI_SUCCESS ) {
        IpmiInstance->IeStatus = EFI_IE_SOFTFAIL;
        IpmiInstance->SoftErrorCount++;
        return Status;
    }
    // Response data format
    //   Byte 1       |  Byte 2    | Byte 3 | Byte 4 | Byte 5          | Byte 6: N-1| Byte N
    // -------------------------------------------------------------------------------------
    // RsSlaveAddress |  NetFn/LUN |  Seq   | Cmd    | Completion Code | Data       | Commit
    // -------------------------------------------------------------------------------------
    DataSize = *ResponseDataSize + 6;   // Added 6 for bytes accessed along with Response data 
    if ((Status = ReceiveDataFromIeDcmi (IpmiInstance, Context,
                IpmiInstance->TempData, &DataSize)) != EFI_SUCCESS) {
        IpmiInstance->IeStatus = EFI_IE_SOFTFAIL;
        IpmiInstance->SoftErrorCount++;
        return Status;
    }
    This->CommandCompletionCode = IpmiInstance->TempData[4];
    if (ERROR_COMPLETION_CODE(This->CommandCompletionCode)) {
        UpdateSoftErrorCount (This->CommandCompletionCode, IpmiInstance);
        // Write completion code into return buffer if an IPMI command returns an error
        if (*ResponseDataSize) {
            *ResponseData = This->CommandCompletionCode;
            *ResponseDataSize = 1;
        }
        return EFI_DEVICE_ERROR;
    }

    //
    // Finding Last byte of data to remove padded bytes.
    // Bytes are padded to make UINT32 size of packet.
    //
    TempDataAccess = DataSize - 1;
    while (IpmiInstance->TempData[TempDataAccess - TempDataSize] != Commit) {
        TempDataSize++;
    }

    //
    // Removing padded Bytes size to get only the response data.
    //
    if (TempDataSize > 0){
        DataSize = DataSize - TempDataSize ;
    }

    if (DataSize < 6) {
        return EFI_DEVICE_ERROR;
    }

    if ((DataSize - 6) > *ResponseDataSize) {
        *ResponseDataSize = (UINT8) (DataSize - 6);
        return EFI_BUFFER_TOO_SMALL;
    }

    //
    // Copying the response data into ResponseData buffer
    //
    CopyMem (ResponseData, &IpmiInstance->TempData[5], (DataSize - 6));
    *ResponseDataSize = (UINT8) (DataSize - 6);

    return EFI_SUCCESS;
}

/**
    IPMI send command API for BT interface

    @param  This -  Pointer to IPMI Protocol/PPI
    @param  NetFunction - Net function of the command
    @param  LUN - Logical Unit number of the command
    @param  Command - Command to be sent to IE
    @param  CommandData - Command data to be sent along with Command
    @param  CommandDataSize - Command Data size
    @param  ResponseData - Response Data from IE
    @param  ResponseDataSize - Size of the Response data

    @return EFI_STATUS Status returned from IE while executing the command.

**/
EFI_STATUS
EfiIeDcmiSendCommand (
  IN EFI_IPMI_TRANSPORT *This,
  IN UINT8              NetFunction,
  IN UINT8              Lun,
  IN UINT8              Command,
  IN UINT8              *CommandData,
  IN UINT8              CommandDataSize,
  OUT UINT8             *ResponseData,
  OUT UINT8             *ResponseDataSize )
{
    return IpmiIeDcmiSendCommand (
            This,
            NetFunction,
            Lun,
            Command,
            CommandData,
            CommandDataSize,
            ResponseData,
            ResponseDataSize,
            NULL);
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
