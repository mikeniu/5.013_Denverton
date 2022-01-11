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
// Name:		RtErrorLogGpnv.c
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "RtErrorLogGpnv.h"


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: LogMemErrorToGpnv
//
// Description: This function logs Memory error to Gpnv
//
// Input:
//  UINT8    Socket
//  UINT8    Channel
//  UINT8    Dimm
//  UINT8    Type
//
// Output: EFI_STATUS - EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
LogMemErrorToGpnv (
    IN UINT8    Type,
    MEMORY_DEV_INFO  *MemInfo
  )
{

  EFI_STATUS                        Status = EFI_SUCCESS; 
  UINT64                            RecordId;
  EFI_GPNV_RUNTIME_ERROR            ToWrite;  
  EFI_SM_ELOG_PROTOCOL              *GenericElogProtocol = NULL;
  
  ToWrite.EventLogType = 0;

  ToWrite.RecordType = EFI_EVENT_LOG_TYPE_OEM_GPNV_RUNTIME;
  ToWrite.EventLogType = (Type == COR_ERROR_TYPE ?
                          EFI_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR :
                          EFI_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR);

  if (ToWrite.EventLogType) {						  
  
    Status = gSmst->SmmLocateProtocol (&gEfiGenericElogProtocolGuid, NULL, &GenericElogProtocol);
  
    if (EFI_ERROR(Status)) {
      return Status;
    }
   
    Status = GenericElogProtocol->SetEventLogData(GenericElogProtocol,
                                                  (UINT8*)&ToWrite,
                                                  EfiElogSmSMBIOS,
                                                  FALSE,
                                                  sizeof (EFI_GPNV_RUNTIME_ERROR),
                                                  &RecordId			                                 
                                                  );
  }
  
  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: LogPciErrorToGpnv
//
// Description: This function logs pci error to Gpnv
//
// Input:
//  UINT8    ErrorSource
//  UINT8    ErrorType
//  UINT8    Segment
//  UINT8    Bus
//  UINT8    Device
//  UINT8    Function
//
// Output: EFI_STATUS - EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
LogPciErrorToGpnv (
   IN	  UINT8       ErrorSource,
   IN	  UINT8       ErrorType,
   IN	  UINT8       Segment,
   IN	  UINT8       Bus,
   IN	  UINT8       Device,
   IN	  UINT8       Function
 )
{
    EFI_STATUS                      Status = EFI_SUCCESS; 
    UINT16                          Data16;
    UINT64                          RecordId;
    EFI_GPNV_RUNTIME_ERROR          ToWrite;
    EFI_SM_ELOG_PROTOCOL            *GenericElogProtocol = NULL;

    ToWrite.EventLogType = 0;
	
    ToWrite.RecordType = EFI_EVENT_LOG_TYPE_OEM_GPNV_RUNTIME;

    // Determine what sort of error occurred.
    Data16 = *(UINT16*)PCIE_CFG_ADDR( Bus, Device, Function, PCI_STS );
    Data16 &= (PCI_STS_MSTR_DATA_PERR | PCI_STS_SIG_SERR | PCI_STS_DET_PERR);

    if (Data16) {
        // Determine whether PERR or SERR.
        if (Data16 & (PCI_STS_MSTR_DATA_PERR | PCI_STS_DET_PERR)) {
            ToWrite.EventLogType = EFI_EVENT_LOG_TYPE_PCI_PARITY_ERROR;
        } else {
            ToWrite.EventLogType = EFI_EVENT_LOG_TYPE_PCI_SYSTEM_ERROR;
        }
    }    
	
    if (ToWrite.EventLogType) {
    
      Status = gSmst->SmmLocateProtocol (&gEfiGenericElogProtocolGuid, NULL, &GenericElogProtocol);
	
      if (EFI_ERROR(Status)) {
        return Status;
      }
    
      // Logging Error to GPNV Area
      Status = GenericElogProtocol->SetEventLogData(GenericElogProtocol,
                                                    (UINT8*)&ToWrite,
                                                    EfiElogSmSMBIOS,
  		                                            FALSE,
  		                                            sizeof (EFI_GPNV_RUNTIME_ERROR),
  		                                            &RecordId			                                 
  		                                            );

    }
    
    return Status;
}
    
