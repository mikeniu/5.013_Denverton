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
//<AMI_FHDR_START>
//
// Name:	CryptoPkgTest.C
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Token.h>
#include <Library/BaseCryptLib.h>
#include <openssl/crypto.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <CryptoPkgTest.h>


AMI_CRYPTOPKG_TEST_PROTOCOL  AmiCryptoPkgTestProtocol = {
    AMI_CRYPTOPKG_TEST_PROTOCOL_REVISION_01,
    GetOpenSslVersionString,
    GetOpenSslVersionNumber
};

EFI_STATUS GetOpenSslVersionString(const char **Buffer)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  
  *Buffer = SSLeay_version(SSLEAY_VERSION);
  
  return Status;
}

EFI_STATUS GetOpenSslVersionNumber(unsigned long *Buffer)
{
  EFI_STATUS    Status = EFI_SUCCESS;	
  
  *Buffer = SSLeay();
  
  return Status;
}	

EFI_STATUS
EFIAPI
CryptoPkgTestEntryPoint (
  IN EFI_HANDLE              ImageHandle,
  IN EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_HANDLE	Handle;

  InitAmiLib(ImageHandle, SystemTable);

  Handle = NULL;
  Status = pBS->InstallProtocolInterface (
                 &Handle,
                 &gAmiCryptoPkgTestProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &AmiCryptoPkgTestProtocol
                 );
    
  return Status;
}

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
