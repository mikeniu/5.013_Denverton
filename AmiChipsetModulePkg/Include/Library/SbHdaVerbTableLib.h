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

/** @file SbHdaVerbTableLib.h
    This header file define South bridge HDA Verb Table Override  
    function and the necessary structures

*/
#ifndef  _AMI_SB_HDA_VERB_TABLE_LIB_H   //To Avoid this header get compiled twice
#define  _AMI_SB_HDA_VERB_TABLE_LIB_H

//
// The content of AMI_AZALIA_VERB_TABLE_HEADER can be
// changed per your chipset SW design.
//
typedef struct {
    UINT32  VendorDeviceId;
    UINT16  SubSystemId;
    UINT8   RevisionId;                       /// 0xFF applies to all steppings
    UINT8   FrontPanelSupport;
    UINT16  NumberOfRearJacks;
    UINT16  NumberOfFrontJacks;
} AMI_AZALIA_VERB_TABLE_HEADER;

typedef struct {
    AMI_AZALIA_VERB_TABLE_HEADER  VerbTableHeader;
    UINT32                        *VerbTableData;
} SB_HDA_VERB_TABLE;

EFI_STATUS
SbHdaVerbTableOverride (
    IN OUT SB_HDA_VERB_TABLE    **VerbTable,
    IN OUT UINT8                *VerbTableNum,
    IN BOOLEAN                  Pei
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
