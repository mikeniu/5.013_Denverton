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
// Name:		RtErrorLogBmcNullLib.c
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "RtErrorLogBmc.h"


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
    return	EFI_SUCCESS;    
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
    return	EFI_SUCCESS;    
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   LogMcErrorToBmc
//
// Description:
//  Logs the machine check error to Bmc
//
// Input:
//  Machine check structure
//
// Output: 
//  None
// Notes:
//  None
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
EFIAPI
LogMcErrorToBmc (
  IN  MC_ERROR_LOG_INFO *McErrLogInfo
  )
{

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
