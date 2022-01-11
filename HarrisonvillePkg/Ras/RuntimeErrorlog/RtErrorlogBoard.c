//**************************************************************************** //
//****************************************************************************//
//**                                                                                                                          //
//**        (C)Copyright 1985-2008, American Megatrends, Inc.             //
//**                                                                                                                       **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//
// $Header:
//
// $Revision: 
//
// $Date:
//
//

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		RtErrorLogBoard.c
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "RtErrorlogBoard.h"

UINT8                           mErrorSeverity;

//---------------------------------------------------------------------------
// External (eLink) handler functions
//---------------------------------------------------------------------------

typedef EFI_STATUS (MEM_ERROR_HANDLER_FUNCTION) (
    IN UINT8    Type,
    MEMORY_DEV_INFO  *MemInfo    
);

extern MEM_ERROR_HANDLER_FUNCTION MEM_ERROR_HANDLER_FUNCTION_LIST EndOfHandlerList;

MEM_ERROR_HANDLER_FUNCTION* MemErrorHandlerList[] = {
	MEM_ERROR_HANDLER_FUNCTION_LIST
	NULL
};

typedef EFI_STATUS (PCI_ERROR_HANDLER_FUNCTION) (
		IN	  UINT8       ErrorSource,
		IN	  UINT8       ErrorType,
		IN	  UINT8       Segment,
		IN	  UINT8       Bus,
		IN	  UINT8       Device,
		IN	  UINT8       Function
);

extern PCI_ERROR_HANDLER_FUNCTION PCI_ERROR_HANDLER_FUNCTION_LIST PciEndOfHandlerList;

PCI_ERROR_HANDLER_FUNCTION* PciErrorHandlerList[] = {
		PCI_ERROR_HANDLER_FUNCTION_LIST
    NULL
};

typedef EFI_STATUS (CSI_ERROR_HANDLER_FUNCTION) (
        IN  CSI_GENERIC_ERROR_LOG_HEADER *CsiErrInfo
);

extern CSI_ERROR_HANDLER_FUNCTION CSI_ERROR_HANDLER_FUNCTION_LIST CsiEndOfHandlerList;

CSI_ERROR_HANDLER_FUNCTION* CsiErrorHandlerList[] = {
        CSI_ERROR_HANDLER_FUNCTION_LIST
    NULL
};

//<AMI_PHDR_START>
//-----------------------------------------------------------------------
//
//
// Procedure:   ErrorSeverityAction
//
// Description: 
//
// Parameters:  
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
ErrorSeverityAction (
  )
{

  EFI_STATUS  Status = EFI_SUCCESS;
  if(mErrorSeverity == FATAL_ERROR_TYPE){

			#if FATAL_ERROR_ACTION == GENERATE_NMI_ON_FE
  			// Generate NMI on fatal errors
	  	  	  	  ElogGenerateNmiNow ();
			#endif
  }

	return Status;

}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogMemErrorRecoed
//
// Description: 
//
// Parameters:  
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogMemErrorRecord (
   MEM_ERROR_LOG_INFO  *MemErrorLogInfo
  )
{

    // Call each handler function.
    {
       	UINTN i;
	    for(i=0; MemErrorHandlerList[i]; i++) {
            MemErrorHandlerList[i] (
            	MemErrorLogInfo->Type,
            	MemErrorLogInfo->MemInfo
            );
           }
    }

  return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LogCsiErrorRecord
//
// Description: 
//
// Parameters:  
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
LogCsiErrorRecord (
     IN  CSI_GENERIC_ERROR_LOG_HEADER *CsiErrInfo
    )
{
    UINTN i;
            
    for(i=0; CsiErrorHandlerList[i]; i++) {
        CsiErrorHandlerList[i] (CsiErrInfo);
    }

    return EFI_SUCCESS;

}

EFI_STATUS
LogPciErrorRecord (
	PCI_PCIE_ERROR_LOCAL_STRUC   *PciErrorData
    )
{
            UINTN i;
            
            for(i=0; PciErrorHandlerList[i]; i++) {
                PciErrorHandlerList[i] (
                    PciErrorData ->ErrorSource,
                    PciErrorData ->ErrorType,
                    PciErrorData ->Segment,
                    PciErrorData ->Bus,
                    PciErrorData ->Device,
                    PciErrorData ->Function
                 );
            }

    return EFI_SUCCESS;

}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   CommonErrorHandling
//
// Description: 
//
// Parameters:  
//
// Returns:     
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
CommonErrorHandling (
  VOID  *ErrorDataStructure
  )
{

  EFI_STATUS  	Status = EFI_SUCCESS;
  UINT8  			ErrorSource;
  
  ErrorSource = *(UINT8 *)ErrorDataStructure;
  mErrorSeverity = *((UINT8 *)ErrorDataStructure + 1);
  
  
  
  switch (ErrorSource)  {
  case MemSource:
    LogMemErrorRecord ((MEM_ERROR_LOG_INFO *)ErrorDataStructure);
    break;

  case PciSource:
    LogPciErrorRecord ((PCI_PCIE_ERROR_LOCAL_STRUC *)ErrorDataStructure);
    break;

  case CsiSource:
    LogCsiErrorRecord ((CSI_GENERIC_ERROR_LOG_HEADER *)ErrorDataStructure);
    break;
    
  default:
    break;

  }
  
  ErrorSeverityAction();
	return Status;

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
