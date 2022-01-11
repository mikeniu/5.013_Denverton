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

/** @file Ppi/CrbInfo.h
    This header file contains the CrbInfo.h definition.

**/

#ifndef _AMI_PPI_CRBINFO_H_
#define _AMI_PPI_CRBINFO_H_

/// the GUID of AMI_PEI_CRBINFO_PPI
#define AMI_PEI_CRBINFO_PPI_GUID \
	{ 0x1b1924c3, 0x6321, 0x484f, { 0x83, 0xef, 0x88, 0x94, 0xb2, 0x1d, 0xe2, 0x58 } }
	// {1B1924C3-6321-484f-83EF-8894B21DE258}

typedef struct _AMI_PEI_CRBINFO_PPI  AMI_PEI_CRBINFO_PPI;

#pragma pack(1)

/**
    Provide the chipset vendor number.

    @param This this Ppi.

    @retval 0 Unknown.
    @retval 1 Intel.
    @retval 2 AMD.

    @note  .
**/
typedef
UINT8
(EFIAPI *AMI_PEI_CRBINFO_GET_CHIPSET_VENDOR_NO)(
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

/**
    Provide the CPU ID.


    @param This this Ppi.

    @retval EAX : Version Information (Type, Family, Model, and Stepping ID)

    @note  .
**/
typedef
UINT32
(EFIAPI *AMI_PEI_CRBINFO_GET_CPU_ID)(
  IN  CONST AMI_PEI_CRBINFO_PPI  	*This
);

/**
    Provide the PCI DID/VID of the north bridge.

    @param This this Ppi.

    @retval -1 Undefined.
    @retval others PCI DID/VID.

    @note  .
**/
typedef
UINT32
(EFIAPI *AMI_PEI_CRBINFO_GET_NB_PCI_ID)(
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

/**
    Provide the PCI DID/VID of the south bridge.

    @param This this Ppi.

    @retval -1 Undefined.
    @retval others PCI DID/VID.

    @note  .
**/
typedef
UINT32
(EFIAPI *AMI_PEI_CRBINFO_GET_SB_PCI_ID)(
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

/**
    Provide the project ID.

    @param This this Ppi.

    @retval NULL Undefined / error.
    @retval others Project ID.

    @note  .
**/
typedef
CHAR8*
(EFIAPI *AMI_PEI_CRBINFO_GET_PROJECT_ID)(
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

/**
    Provide the BIOS revision.

    @param This this Ppi.

    @retval -1 Undefined / error.
    @retval others BIOS revision.

    @note  .
**/
typedef
UINT16
(EFIAPI *AMI_PEI_CRBINFO_GET_BIOS_REVISION)(
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

///
/// This ppi is used to get CRB information.
///
struct _AMI_PEI_CRBINFO_PPI {
  AMI_PEI_CRBINFO_GET_CHIPSET_VENDOR_NO GetChipsetVendorNo;
  AMI_PEI_CRBINFO_GET_CPU_ID GetCpuId;
  AMI_PEI_CRBINFO_GET_NB_PCI_ID GetNorthBridgePciId;
  AMI_PEI_CRBINFO_GET_SB_PCI_ID GetSouthBridgePciId;
  AMI_PEI_CRBINFO_GET_PROJECT_ID GetProjectId;
  AMI_PEI_CRBINFO_GET_BIOS_REVISION GetBiosRevision;
};

extern EFI_GUID   gAmiPeiCrbInfoPpiGuid;

#pragma pack()

#endif /* _AMI_PPI_CRBINFO_H_ */

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
