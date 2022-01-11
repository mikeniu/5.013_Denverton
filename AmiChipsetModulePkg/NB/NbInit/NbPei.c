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

/** @file NbPei.c
    This file contains code for Template North bridge initialization
    in the PEI stage

*/

// Module specific Includes
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <StatusCodes.h>
#include <PCI.h>
#include <Nb.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <PPI/CspLibPPI.h>
#include <Library/NbCspLib.h>
#include <AmiChipsetIoLib.h>
#include <Ppi/PciCfg.h>
#include <Ppi/CpuIo.h>
#include <Ppi/NbPpi.h>

extern  AMI_PEI_PCI_INIT_TABLE_STRUCT   stNBH2P_PCIInitTable [];
extern  UINT16                          wNBH2P_PCIInitTableSize;
// Produced PPIs

// GUID Definitions

// Portable Constants


// Function Prototypes

// PPI interface definition

static AMI_PEI_NBINIT_POLICY_PPI mAmiPeiNbInitPolicyPpi =
    {
        0
    };

// PPI that are installed

/** PORTING **
    Include any additional PPI needed for memory detection in this
    list and define the functions in this file
    **/
static EFI_PEI_PPI_DESCRIPTOR mPpiList[] =
  { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmiPeiNbInitPolicyGuid,
    &mAmiPeiNbInitPolicyPpi };

#define NB_DEVICE_NUMBER        0
#define TempBusDevFuncNo        (PEI_PCI_CFG_ADDRESS(0, NB_DEVICE_NUMBER, 0, 0))

// Function Definition

EFI_STATUS NbPeiInitAfterMemInstalled (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
);

EFI_STATUS ProgramNbRegBeforeEndofPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
);

// PPI that are notified

static EFI_PEI_NOTIFY_DESCRIPTOR MemInstalledNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gEfiPeiMemoryDiscoveredPpiGuid, NbPeiInitAfterMemInstalled },
};

static EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gEfiEndOfPeiSignalPpiGuid, ProgramNbRegBeforeEndofPei },
};

/**
    This function is the entry point for this PEI. This function
    initializes the chipset NB

    @param FileHandle - Handle of the file being invoked
    @param PeiServices - Pointer to the PEI services table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

    @note  This function should initialize North Bridge for memory detection.
              Install AMI_PEI_NBINIT_POLICY_PPI to indicate that NB Init PEIM
              is installed
              Following things can be done at this point:
                  - Enabling top of 4GB decode for full flash ROM
                  - Programming North Bridge ports to enable access to South
                    Bridge and other I/O bridge access

**/
EFI_STATUS
EFIAPI
NbPeiInit (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{

    EFI_STATUS                  Status;
    EFI_HOB_CPU                 *pCpu = NULL;
    EFI_PEI_PCI_CFG2_PPI        *PciCfg;
    AMI_PEI_PCI_TABLE_INIT_PPI  *AmiPciTableInit;

    // Report Progress code
    PEI_PROGRESS_CODE (PeiServices, PEI_CAR_NB_INIT);

    // Install the NB Init Policy PPI
    Status = (*PeiServices)->InstallPpi(PeiServices, &mPpiList[0]);
    ASSERT_PEI_ERROR (PeiServices, Status);

    // Get pointer to the PCI config PPI
    PciCfg = (*PeiServices)->PciCfg;

    // Locate AMI PCI Table Init PPI
    Status = (*PeiServices)->LocatePpi(PeiServices, &gAmiPeiPciTableInitPpiGuid,
                        0, NULL, &AmiPciTableInit);
    // Assert if not found - the AMI PCI Table Init PPI should exist
    if (Status) {
        return Status;
    }

    // Create CPU HOB with appropriate memory space size and IO space size
    Status = (*PeiServices)->CreateHob(PeiServices, EFI_HOB_TYPE_CPU, sizeof(EFI_HOB_CPU), &pCpu);
    if (Status == EFI_SUCCESS) {
        pCpu->SizeOfMemorySpace = 40;       // Maximum address space supported by the CPU/Chipset
        pCpu->SizeOfIoSpace     = 16;
        pCpu->Reserved[0] = pCpu->Reserved[1] = pCpu->Reserved[2] = pCpu->Reserved[3] =
                            pCpu->Reserved[4] = pCpu->Reserved[5] = 0;
    }


    AmiPciTableInit->AMIPEIProgramPCITable(
                        PeiServices, AmiPciTableInit, PciCfg,
                        TempBusDevFuncNo,
                        stNBH2P_PCIInitTable,
                        wNBH2P_PCIInitTableSize);


    // Setup a NBPEI entry after PEI permanent memory be installed
    Status = (*PeiServices)->NotifyPpi( PeiServices, MemInstalledNotifyList );
    ASSERT_PEI_ERROR ( PeiServices, Status );

    return EFI_SUCCESS;

}

/**
    This function programs NB PCI devices sub-vendor ID and
    sub-system ID.

    @param PeiServices - Pointer to the PEI services table
    @param PciCfg - Pointer to the PCI Configuration PPI

    @retval VOID

    @note  1. This routine only programs the PCI device in NB, hence, we
                 have to check the bus/device/function numbers whether they
                 are a NB PCI device or not.
              2. This routine is invoked by PEI phase.(After PEI permanent
                 memory be installed)
**/
VOID ProgramNbSubId(
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg )
{
    EFI_STATUS                    Status = EFI_SUCCESS;
    AMI_PEI_NB_CUSTOM_PPI         *NbPeiOemPpi;
    UINTN                         i = 0;
    UINT32                        PciSid = 0xffffffff;
    AMI_NB_PCI_SSID_TABLE_STRUCT  DefaultSidTbl[] = {NB_PCI_DEVICES_SSID_TABLE};
    AMI_NB_PCI_SSID_TABLE_STRUCT  *SsidTblPtr = DefaultSidTbl;

    Status = (*PeiServices)->LocatePpi( PeiServices, \
                                        &gAmiPeiNbCustomPpiGuid, \
                                        0, \
                                        NULL, \
                                        &NbPeiOemPpi );

    if (Status == EFI_SUCCESS) {
        if (NbPeiOemPpi->SsidTable != NULL) 
            SsidTblPtr = NbPeiOemPpi->SsidTable;
    }

    // Porting Start
    while (SsidTblPtr[i].PciAddr != 0xffffffffffffffff) {
        // Denverton AptioV override Start - EIP#270210 
        //Checking if the PCI Address matches that of the System Agent
        if ( SsidTblPtr[i].PciAddr == (NB_BUS_DEV_FUN) ) { 
        // Denverton AptioV override End - EIP#270210
            if (SsidTblPtr[i].Sid == 0xffffffff) {
                Status = PciCfg->Read( PeiServices,
                                       PciCfg,
                                       EfiPeiPciCfgWidthUint32,
                                       SsidTblPtr[i].PciAddr,
                                       &PciSid);
            } else {
                PciSid = SsidTblPtr[i].Sid;
            }

            Status = PciCfg->Write( PeiServices,
                                    PciCfg,
                                    EfiPeiPciCfgWidthUint32,
                                    SsidTblPtr[i].PciAddr | PCI_SVID,
                                    &PciSid);

        }

        i++;
    }
    // Porting End

}

/**
    This function can be used to program any NB register after
    PEI permanent memory be installed.

    @param PeiServices      - Pointer to the PEI services table
    @param NotifyDescriptor - Pointer to the descriptor for the
                              notification event.
    @param InvokePpi        - Pointer to the PPI that was installed

    @retval EFI_STATUS
**/
EFI_STATUS NbPeiInitAfterMemInstalled (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_PEI_CPU_IO_PPI          *CpuIo;
    EFI_PEI_PCI_CFG2_PPI        *PciCfg;
    EFI_BOOT_MODE               BootMode;

    CpuIo = (*PeiServices)->CpuIo;
    PciCfg = (*PeiServices)->PciCfg;

    PEI_PROGRESS_CODE (PeiServices, PEI_MEM_NB_INIT);

    // Program NB Devices' Subsystem Vendor ID & Subsystem ID
    ProgramNbSubId( PeiServices, PciCfg );

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);

    if (BootMode != BOOT_ON_S3_RESUME) {
        // Porting if needed.
    } else {
        // Porting if needed.
    }

    // Set up necessary PPI notifications after PEI permantent memory
    // be installed
    Status = (*PeiServices)->NotifyPpi( PeiServices, &mNotifyList[0] );
    ASSERT_PEI_ERROR ( PeiServices, Status );

    return  EFI_SUCCESS;
}

/**
    This function can be used to program any NB register before
    end of PEI phase.

    @param PeiServices      - Pointer to the PEI services table
    @param NotifyDescriptor - Pointer to the descriptor for the
                              notification event.
    @param InvokePpi        - Pointer to the PPI that was installed

    @retval EFI_STATUS
**/
EFI_STATUS ProgramNbRegBeforeEndofPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_PEI_CPU_IO_PPI          *CpuIo;
    EFI_PEI_PCI_CFG2_PPI        *PciCfg;
    EFI_BOOT_MODE               BootMode;

    CpuIo = (*PeiServices)->CpuIo;
    PciCfg = (*PeiServices)->PciCfg;

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);

    if (BootMode == BOOT_ON_S3_RESUME) {
        // Porting if needed.
    } else {
        // Porting if needed.
    }

    return  EFI_SUCCESS;
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
