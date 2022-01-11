//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		RtErrorLogBmc.c
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "RtErrorLogBmc.h"

EFI_SEL_RECORD_DATA           mSelRecord;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   UpdateGenericSelInfo
//
// Description: 
// This function will update generic IPMI info fields 
//
// Input:
//
//    SelRecord           - Pointer to SEL Record
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
UpdateGenericSelInfo (
  IN OUT  EFI_SEL_RECORD_DATA     *SelRecord
  )
{

  SelRecord->TimeStamp.Year = 0;
  SelRecord->RecordType = SEL_SYSTEM_RECORD;
  SelRecord->GeneratorId = (UINT16)EFI_GENERATOR_ID(EFI_FORMAT_REV1);
  SelRecord->EvMRevision = EFI_EVM_REVISION;
  SelRecord->EventDirType = EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
  return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   UpdateSelInfoForMe
//
// Description: 
// Update the SEL Record information for Memory Errors
//
// Input:
//
//    MemorySensorOffset  - Memory offset to enum EFI_SEL_SENSOR_MEMORY_OFFSET
//    Channel          		- Channel, base 0
//    DimmNumber          - Dimm Number, base 0
//    SelRecord           - Pointer to SEL Record
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
UpdateSelInfoForMe (
  IN      EFI_SEL_SENSOR_MEMORY_OFFSET        MemorySensorOffset,
	IN      MEM_ERROR_LOCATION									*MemErrorLoc,
  IN OUT  EFI_SEL_RECORD_DATA                 *SelRecord
  )
{
  // Update Generic SEL related information

  UpdateGenericSelInfo (SelRecord);

  SelRecord->SensorType = EFI_MEMORY_SENSOR_TYPE;
  SelRecord->SensorNumber = (UINT8)0x8; // for UCE/CE

  if (MemorySensorOffset == EFI_MEMORY_SENSOR_OFFSET_ECC_UNCORRECTABLE) {
  // UCE error
    SelRecord->OEMEvData1 = 0x20 | MEMORY_SENSOR_UCE_OFFSET; // Add error Id to identify the error
    SelRecord->OEMEvData2 = 0xFF; 
    if (MemErrorLoc->Dimm == 0xff) {
      // No info about the Dimm number

      SelRecord->OEMEvData3 = (UINT8)((MemErrorLoc->Socket << OEMEvData3_SOCKET_NUM_BITS) |(MemErrorLoc->DdrChannel << OEMEvData3_CHANNEL_NUM_BITS) | DIMM_NUM_BITS);

    } else {
      SelRecord->OEMEvData3 = (UINT8)((MemErrorLoc->Socket << OEMEvData3_SOCKET_NUM_BITS) |(MemErrorLoc->DdrChannel << OEMEvData3_CHANNEL_NUM_BITS) | MemErrorLoc->Dimm);
    }

  } else if (MemorySensorOffset == EFI_MEMORY_SENSOR_OFFSET_ECC_CORRECTABLE){

    // Single bit error

    SelRecord->OEMEvData1 = 0x20 | MEMORY_SENSOR_CE_OFFSET; // Add error Id to identify the error
    SelRecord->OEMEvData2 = 0xFF; 
    if (MemErrorLoc->Dimm == 0xff) {
      // No info about the error is available
      SelRecord->OEMEvData3 = (UINT8)((MemErrorLoc->Socket << OEMEvData3_SOCKET_NUM_BITS) |(MemErrorLoc->DdrChannel << OEMEvData3_CHANNEL_NUM_BITS) | DIMM_NUM_BITS);
    }

    else {
      SelRecord->OEMEvData3 = (UINT8)((MemErrorLoc->Socket << OEMEvData3_SOCKET_NUM_BITS) |(MemErrorLoc->DdrChannel << OEMEvData3_CHANNEL_NUM_BITS) | MemErrorLoc->Dimm);
    }
  } else if (MemorySensorOffset==EFI_MEMORY_SENSOR_OFFSET_ECC_CORRECTABLE_MAX){
    SelRecord->SensorNumber = (UINT8)0x9; // CE logging disable
      SelRecord->OEMEvData1 = 0x20 | MEMORY_SENSOR_CEMAX_OFFSET; // Add error Id to identify the error
      SelRecord->OEMEvData2 = 0xFF;  
    if (MemErrorLoc->Dimm == 0xff) {
      // No info about the error is available
      SelRecord->OEMEvData3 = (UINT8)((MemErrorLoc->Socket << OEMEvData3_SOCKET_NUM_BITS) |(MemErrorLoc->DdrChannel << OEMEvData3_CHANNEL_NUM_BITS) | DIMM_NUM_BITS);
    } else {
      SelRecord->OEMEvData3 = (UINT8)((MemErrorLoc->Socket << OEMEvData3_SOCKET_NUM_BITS) |(MemErrorLoc->DdrChannel << OEMEvData3_CHANNEL_NUM_BITS) | MemErrorLoc->Dimm);
    }
  }

  return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   AllocSelRecord
//
// Description: 
//     Allocate memory for SelRecord
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
AllocSelRecord(
  IN OUT EFI_SEL_RECORD_DATA **SelRecord
  )
{
  *SelRecord = &mSelRecord;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogInfoToSel
//
// Description: 
//
//	This function will log the data into SEL using Generic Log API

// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogInfoToSel (
  IN  EFI_SEL_RECORD_DATA     *SelRecord
  )
{
  EFI_STATUS      				Status;
  EFI_IPMI_TRANSPORT			*IpmiTransport;
  UINT8      		  		    ResponseData[10];
  UINT8        	  				ResponseDataSize;
  
#if IPMI_COMPLETION_CODE_SUPPORT
  UINT8                         CompletionCode;
#endif
 ResponseDataSize = 10;

 Status = gSmst->SmmLocateProtocol (&gEfiSmmIpmiTransportProtocolGuid, NULL, &IpmiTransport);
  if (Status == EFI_SUCCESS) {
    Status = IpmiTransport-> SendIpmiCommand (IpmiTransport,
						EFI_SM_STORAGE,
						0,
						ADD_SEL_ENTRY,
						(UINT8 *) SelRecord,
						sizeof (EFI_SEL_RECORD_DATA),
						(UINT8 *) &ResponseData,
#if IPMI_COMPLETION_CODE_SUPPORT
						(UINT8 *) &ResponseDataSize,
						(UINT8 *) &CompletionCode
#else
						(UINT8 *) &ResponseDataSize
#endif
						); 
  }
  

  Status = EFI_SUCCESS;
  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogMeToSel
//
// Description: 
//	Log the memory errors to BMC SEL
//
// Input:
//
// Output:
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogMeToSel (
    IN UINT8    Type,
    MEMORY_DEV_INFO  *MemInfo
 )
{	
    EFI_STATUS                      Status = EFI_SUCCESS;
    EFI_SEL_RECORD_DATA             *SelRecord = NULL;
    EFI_SEL_SENSOR_MEMORY_OFFSET    MemorySensorOffset = EFI_MEMORY_SENSOR_OFFSET_ECC_CORRECTABLE;
    MEM_ERROR_LOCATION              MemErrorLoc;

    // Allocate memory for SelRecord
    AllocSelRecord( &SelRecord );

    // Set location
    MemErrorLoc.Socket = (UINT8)MemInfo->Node;
    MemErrorLoc.DdrChannel = (UINT8)MemInfo->Channel;
    MemErrorLoc.Dimm = (UINT8)MemInfo->Dimm;

    // Set type
    MemorySensorOffset = (Type == COR_ERROR_TYPE ? 
        EFI_MEMORY_SENSOR_OFFSET_ECC_CORRECTABLE :
        EFI_MEMORY_SENSOR_OFFSET_ECC_UNCORRECTABLE);

    // Log sel entry to report
    Status = UpdateSelInfoForMe (MemorySensorOffset, &MemErrorLoc, SelRecord);
    Status = LogInfoToSel( SelRecord );
    return	Status;
    
}


//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   LogPciErrorToBmc
//
// Description:
//  Takes in an error type and the address of a PCI device. Correctly fills
//  in a SEL record and logs to the SEL.
//
// Input:
//  IN  UINT8     ErrorType - Type of PCI error (SERR, PERR, etc.)
//  IN  UINT8     Bus
//  IN  UINT8     Dev
//  IN  UINT8     Func
//
// Output: 
//  EFI_STATUS
//      EFI_SUCCESS - Error successfully logged.
//      Others - A problem occurred while trying to log error.
//
// Modified:
//  Nothing
//
// Referrals:
//  UpdateGenericSelInfo()
//  LogInfoToSel()
//
// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogPciErrorToBmc (
    IN UINT8    ErrorSource,
    IN UINT8    ErrType,
    IN UINT8    Segment,
    IN UINT8    Bus,
    IN UINT8    Dev,
    IN UINT8    Func
)
{
    EFI_SEL_RECORD_DATA SelRecord;
    EFI_STATUS          Status;

    // Initialize generic parts of SEL record.
    Status = UpdateGenericSelInfo( &SelRecord );

    SelRecord.SensorType = SEL_SENS_TYPE_CRIT_ERR;
    SelRecord.SensorNumber = 0x00;
    SelRecord.OEMEvData1 = 0xA0 | ErrType;    // OEM Data for byte 2, 3.
    SelRecord.OEMEvData2 = Bus;                 // Fill with Bus.
    SelRecord.OEMEvData3 = (Dev << 3) | Func;   // Fill with Dev and Func.

    // Report the error.
    Status = LogInfoToSel( &SelRecord ); 

    return Status;
}


//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   LogMcErrorToBmc
//
// Description:
//  Logs the machine check error to Bmc
//
// Input:
//  Machine check Error log Info Structure
//
// Output: 
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
EFIAPI
LogMcErrorToBmc (
  IN  MC_ERROR_LOG_INFO *McErrLogInfo
  )
{
    EFI_SEL_RECORD_DATA SelRecord;

    // Initialize generic parts of SEL record.
    UpdateGenericSelInfo( &SelRecord );

    SelRecord.SensorType = EFI_PROCESSOR_SENSOR_TYPE;
    SelRecord.SensorNumber = 0x03;            // 0x03 - Processor type
    SelRecord.OEMEvData1 = 0x8B;              // 0x8B - 80 - OEM data in OEMEvData2 | 0B - Machine Check
    SelRecord.OEMEvData2 = McErrLogInfo->McDevInfo->CpuIndex;
    
    // Report the error.
    LogInfoToSel( &SelRecord ); 
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   LogIerrToBmc
//
// Description:
//  Logs the intenal cpu error to Bmc
//
// Input:
//  Internal Error log Info Structure
//
// Output: 
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
EFIAPI
LogIerrToBmc (
  IN  IERR_ERROR_LOG_INFO *IerrLogInfo
  )
{
    EFI_SEL_RECORD_DATA SelRecord;

    // Initialize generic parts of SEL record.
    UpdateGenericSelInfo( &SelRecord );

    SelRecord.SensorType = EFI_PROCESSOR_SENSOR_TYPE;
    SelRecord.SensorNumber = 0x03;            // 0x03 - Processor type
    SelRecord.OEMEvData1 = 0x80;              // 0x8B - 80 - OEM data in OEMEvData2 | 00 - IERR
    SelRecord.OEMEvData2 = IerrLogInfo->IerrStatusInfo->Data;
    
    // Report the error.
    LogInfoToSel( &SelRecord ); 
}


//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   LogCsiErrorToBmc
//
// Description:
//  Logs the machine check error and IERR to BMC
//
// Input:
//  Csi Generic header
//
// Output: 
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
EFIAPI
LogCsiErrorToBmc (
  IN  CSI_GENERIC_ERROR_LOG_HEADER *CsiErrLogHdr
  )
{

	switch(CsiErrLogHdr->Type)
	{
	case MC_ERROR_TYPE:
		LogMcErrorToBmc((MC_ERROR_LOG_INFO *) CsiErrLogHdr);
		break;
		
	case IERR_ERROR_TYPE:
		LogIerrToBmc((IERR_ERROR_LOG_INFO *) CsiErrLogHdr);
		break;
	default:
		return;
	}
}


//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
