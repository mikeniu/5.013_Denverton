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

/** @file SbPolicy.h
    South Bridge setup data header file, define all the South
    Bridge setup items and a structures in this file. 

    @note  The context of the SB_SETUP_DATA may be able to copy from
           SB.SD directly 

*/


#ifndef  _AMI_SB_POLICY_H   //To Avoid this header get compiled twice
#define  _AMI_SB_POLICY_H

typedef struct  _SB_SETUP_DATA  SB_SETUP_DATA;

#pragma pack(push, 1)

typedef struct _SB_SETUP_DATA
{
    // SB Setup header
    UINT32   SbPolicyVersion;
    //.......
    UINT8    IdeMode;
    UINT8    Sata;
    UINT8    Sata0;
    UINT8    Sata1;
    UINT8    Sata2;
    UINT8    Sata3;
    // Other items
    // .....
    // ..
    // .
}SB_SETUP_DATA;

#pragma pack(pop)

typedef VOID (SB_OEM_SETUP_CALLBACK) (
    IN VOID                 *Services,
    IN OUT SB_SETUP_DATA    *SbSetupData,
    IN SETUP_DATA           *SetupData,
    IN BOOLEAN              Pei
);


VOID GetSbSetupData(
    IN VOID                 *Service,
    IN OUT SB_SETUP_DATA    *SbSetupData,
    IN BOOLEAN              Pei
);

#endif

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
