//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file CrbDxe.c
    This file contains code for Chipset Reference Board Template
    initialization in the DXE stage

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

#include <PiDxe.h>
#include <Protocol/CrbInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PciLib.h>
#include <Library/S3PciLib.h>
#include <Library/BaseMemoryLib.h>

#include <token.h>
#include <AmiDxeLib.h> // Optional. should use Mde Library instead.
#include <AmiCspLib.h> // Optional. should use Mde Library instead.
#include <Setup.h> // Optional. should use Mde Library instead.


// Produced Protocols

// Consumed Protocols
#include <Protocol/S3SaveState.h>
#include <Protocol/Legacy8259.h>       // Denverton AptioV Override - EIP#380568

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)
UINT8
CrbDxeGetChipsetVendorNo (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
);

UINT32
CrbDxeGetCpuId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  	*This
);

UINT32
CrbDxeGetNorthBridgePciId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
);

UINT32
CrbDxeGetSouthBridgePciId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
);

CHAR8*
CrbDxeGetProjectId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
);

UINT16
CrbDxeGetBiosRevision (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
);

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)
CHAR8 CONST ProjectTag[5] = CONVERT_TO_STRING(CRB_PROJECT_TAG); //!< contains the string of project tag.
EFI_S3_SAVE_STATE_PROTOCOL	*gS3SaveState = NULL; //!< contains the s3 save state protocol.

// GUID Definition(s)

// Protocol Definition(s)

/// contains the AMI_EFI_CRBINFO_PROTOCOL of this CRB
AMI_EFI_CRBINFO_PROTOCOL	gAmiCrbInforProtocol = {
	CrbDxeGetChipsetVendorNo,
	CrbDxeGetCpuId,
	CrbDxeGetNorthBridgePciId,
	CrbDxeGetSouthBridgePciId,
	CrbDxeGetProjectId,
	CrbDxeGetBiosRevision,
};

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------


/**
    Provide the chipset vendor number.

    @param This this protocol.

    @retval 0 Unknown.
    @retval 1 Intel.
    @retval 2 AMD.

    @note  .
**/
UINT8
CrbDxeGetChipsetVendorNo (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
  )
{
	UINT32      Index;
	UINT32      RegisterEax;
	UINT32      RegisterEbx;
	UINT32      RegisterEcx;
	UINT32      RegisterEdx;
	// Intel
	// EBX 756E6547 "uneG"
	// ECX 6C65746E "letn"
	// EDX 49656e69 "Ieni"
	// AMD
	//EBX 68747541 "htuA"
	//ECX 444D4163 "DMAc"
	//EDX 69746E65 "itne"
	CHAR8 CONST Intel[5] = "letnI";
	CHAR8 CONST Amd[3] = "DMA";
	CHAR8 MyString[5];
	UINT8 ChipsetVendorNo = 0;

	Index = 0;
	Index = AsmCpuid(Index, &RegisterEax, &RegisterEbx, &RegisterEcx, &RegisterEdx);
	MyString[4] = ((CHAR8*)&RegisterEdx)[3];
	MyString[3] = ((CHAR8*)&RegisterEcx)[0];
	MyString[2] = ((CHAR8*)&RegisterEcx)[1];
	MyString[1] = ((CHAR8*)&RegisterEcx)[2];
	MyString[0] = ((CHAR8*)&RegisterEcx)[3];

	if (CompareMem(MyString, Intel, 5) == 0) {
		ChipsetVendorNo = 1;
	} else if (CompareMem(MyString, Amd, 3) == 0) {
		ChipsetVendorNo = 2;
	}

	return ChipsetVendorNo;
}

/**
    Provide the CPU ID.

        
    @param This this protocol.

    @retval EAX : Version Information (Type, Family, Model, and Stepping ID)

    @note  .
**/
UINT32
CrbDxeGetCpuId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  	*This
  )
{
	UINT32	Index = 1;
	UINT32	RegisterEax = -1;
	// EAX : Version Information (Type, Family, Model, and Stepping ID)
	Index = AsmCpuid(Index, &RegisterEax, NULL, NULL, NULL);
	return RegisterEax;
}

/**
    Provide the PCI DID/VID of the north bridge.

    @param This this protocol.

    @retval -1 Undefined.
    @retval others PCI DID/VID.

    @note  .
**/
UINT32
CrbDxeGetNorthBridgePciId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
  )
{
	UINT32 PciId = -1;

	if (This->GetChipsetVendorNo(This) == 1) {
		PciId = PciRead32(PCI_LIB_ADDRESS(0, 0, 0, 0));
	} else if (This->GetChipsetVendorNo(This) == 2){
		PciId = PciRead32(PCI_LIB_ADDRESS(0, 0, 0, 0));
	}

	return PciId;
}

/**
    Provide the PCI DID/VID of the south bridge.

    @param This this protocol.

	@retval -1 Undefined.
    @retval others PCI DID/VID.

    @note  .
**/
UINT32
CrbDxeGetSouthBridgePciId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
  )
{
	UINT32 PciId = -1;

	if (This->GetChipsetVendorNo(This) == 1) {
		PciId = PciRead32(PCI_LIB_ADDRESS(0, 31, 0, 0));
	} else if (This->GetChipsetVendorNo(This) == 2){
		PciId = PciRead32(PCI_LIB_ADDRESS(0, 20, 0, 0));
	}

	return PciId;
}

/**
    Provide the project ID.

    @param This this protocol.

    @retval NULL Undefined / error.
    @retval others Project ID.

    @note  .
**/
CHAR8*
CrbDxeGetProjectId (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
  )
{
	return ProjectTag;
}

/**
    Provide the BIOS revision.

    @param This this protocol.

    @retval  -1 Undefined / error.
    @retval others BIOS revision.

    @note  .
**/
UINT16
CrbDxeGetBiosRevision (
  IN  CONST AMI_EFI_CRBINFO_PROTOCOL  *This
  )
{
	return (CRB_PROJECT_MAJOR_VERSION << 8) | CRB_PROJECT_MINOR_VERSION;
}


// Denverton AptioV Override Start- EIP#380568

/**
 * This Callback function gets executed when the gEfiEventLegacyBootGuid is installed.
 * Callback code will disable the IRQ 7,  to prevent spurious IRQ 7 issue in DOS
 *
 * @param Event Pointer to the EFI_EVENT
 * @param Context Pointer to the context for this event
**/
VOID
DisableIrq7 (
    IN EFI_EVENT    Event,
    IN VOID         *Context
)
{
	UINT16      IRQMask; 
	EFI_STATUS  Status;
	EFI_LEGACY_8259_PROTOCOL *mLegacy8259;
	
    gBS->CloseEvent (Event);

    // Use Legacy8259 protocol to Modify the IRQ Mask Register
    Status = gBS->LocateProtocol (&gEfiLegacy8259ProtocolGuid, NULL, (VOID **) &mLegacy8259);
    if (EFI_ERROR (Status)) {
        return;
    }

    // Get the current IRQ mask
    Status = mLegacy8259->GetMask (mLegacy8259, &IRQMask, NULL, NULL, NULL);
    if (EFI_ERROR (Status)) {
        return;
    }

    IRQMask = IRQMask | 0x80;      // Mask Bit 7 to disable IRQ 7

    // Modified the IRQ mask register to disable IRQ 7
    Status = mLegacy8259->SetMask(mLegacy8259, &IRQMask, NULL, NULL, NULL);
    if (EFI_ERROR (Status)) {
        return;
    }

    DEBUG ((DEBUG_INFO, "Disabled IRQ7\n"));
}
// Denverton AptioV Override End- EIP#380568

/**
    This function is the entry point for CRB DXE driver.
    This function initializes the CRB in DXE phase.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval EFI_SUCCESS CRB DXE driver be loaded successfully.

    @note  This routine is called very early, prior to SBDXE and NBDXE.
**/

EFI_STATUS
EFIAPI
CrbDxeInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    EFI_EVENT               LegacyBootEvt = NULL;      // Denverton AptioV Override - EIP#380568

    Status = gBS->InstallMultipleProtocolInterfaces(
                    &ImageHandle,
                    &gAmiEfiCrbInfoProtocolGuid, &gAmiCrbInforProtocol,
                    NULL
                    );

    // Denverton AptioV Override Start- EIP#380568
    // Register LegacyBootEvent callback to Disable IRQ7 assignments to prevent spurious IRQ 7 issue in DOS

    Status = gBS->CreateEventEx (EVT_NOTIFY_SIGNAL, \
                        TPL_CALLBACK, \
                        DisableIrq7, \
                        NULL, \
                        &gEfiEventLegacyBootGuid, \
                        &LegacyBootEvt );

    DEBUG ((DEBUG_INFO, "Register LegacyBootEvent to Disable IRQ7: %r\n", Status));

    // Denverton AptioV Override End- EIP#380568

    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
