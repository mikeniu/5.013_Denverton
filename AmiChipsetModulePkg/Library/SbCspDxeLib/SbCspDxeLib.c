//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SbCspDxeLib.c
    This file contains code for Sorth Bridge platform
    initialization in the Library stage

*/

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------

#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/ExitPmAuth.h>


//
// GUID Definition(s)
//


/**
    The eLink will signal gExitPmAuthProtocolGuid Event.

    @param VOID

    @retval VOID

**/
VOID SignalExitPmAuthProtocolEvent (
  VOID
)
{
  EFI_HANDLE  Handle = NULL;

  //
  // Signaling gExitPmAuthProtocolGuid Event
	//
  pBS->InstallProtocolInterface (
        &Handle,
        &gExitPmAuthProtocolGuid,
        EFI_NATIVE_INTERFACE,
        NULL
        );

  pBS->UninstallProtocolInterface (
        Handle,
        &gExitPmAuthProtocolGuid,
        NULL
        );
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
