//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SbPei.c
    This file contains code for Template South bridge initialization
    in the PEI stage

*/

// Module specific Includes
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <AmiPeiLib.h>
#include <Library/SbCspLib.h>
#include <Hob.h>
#include <Sb.h>
#include <Guid/Rtc.h>

// Denverton AptioV override Start - EIP#270210 
#include <PCI.h>
// Denverton AptioV override End - EIP#270210  

// Produced/used PPI interfaces
#include <Ppi/PciCfg2.h>
#include <Ppi/SbPpi.h>
#include <Ppi/SbChipsetPpi.h>
#include <Ppi/CpuIo.h>
#include <Ppi/CspLibPpi.h>
#include <Ppi/SmmControl.h>
#include <Ppi/AtaController.h>

// Produced PPIs

AMI_GPIO_INIT_TABLE_STRUCT DefaultGpioTable[] = {
    #include <SbGpio.h>
};

// GUID Definitions

// Portable Constants

// Function Prototypes

BOOLEAN IsRecovery(
    IN EFI_PEI_SERVICES     **PeiServices, 
    IN EFI_PEI_PCI_CFG2_PPI *PciCfg, 
    IN EFI_PEI_CPU_IO_PPI   *CpuIo
);

EFI_STATUS SbPeiEnableAtaChannelForRecovery(
    IN EFI_PEI_SERVICES         **PeiServices,
    IN PEI_ATA_CONTROLLER_PPI   *This,
    IN UINT8                    ChannelMask
);

EFI_STATUS CountTime(
    IN  UINTN   DelayTime, 
    IN  UINT16  BaseAddr
);

VOID InitGpio (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo
);

EFI_STATUS SbPeiInitAfterMemInstalled (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
);

EFI_STATUS ProgramSbRegBeforeEndofPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
);

// PPI interface definition
// PPI produced by this PEIM

#if SB_RESET_PPI_SUPPORT
#include <Ppi/Reset.h>

VOID SBLib_ResetSystem(
    IN EFI_RESET_TYPE ResetType
);

EFI_STATUS SbPeiResetSystem(
    IN EFI_PEI_SERVICES **PeiServices
);
// Harrisonville override start - EIP#210294

static  EFI_PEI_RESET_PPI mResetPpi = { SbPeiResetSystem };
#endif  //SB_RESET_PPI_SUPPORT
VOID SbWakeupTypeLib (
    VOID
);
// Harrisonville override end - EIP#210294
#if SB_STALL_PPI_SUPPORT
#include "Ppi/Stall.h"

EFI_STATUS SbPeiStall(
    IN EFI_PEI_SERVICES     **PeiServices, 
    IN EFI_PEI_STALL_PPI    *This, 
    IN UINTN                MicroSeconds
);


// Constants related to template Stall code
#define TIMER_RESOLUTION  1

static EFI_PEI_STALL_PPI mStallPpi = {
    TIMER_RESOLUTION,
    SbPeiStall
};
#endif  //SB_STALL_PPI_SUPPORT


// PPI Definition
static  AMI_PEI_SBINIT_POLICY_PPI mAmiPeiSbInitPolicyPpi = { TRUE };

static PEI_ATA_CONTROLLER_PPI mAtaControllerPpi = {
    SbPeiEnableAtaChannelForRecovery
};

#if SMM_SUPPORT
EFI_STATUS 
SbPeiActivateSmi (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN PEI_SMM_CONTROL_PPI  *This,
    IN OUT INT8             *ArgumentBuffer OPTIONAL,
    IN OUT UINTN            *ArgumentBufferSize OPTIONAL,
    IN BOOLEAN              Periodic OPTIONAL,
    IN UINTN                ActivationInterval OPTIONAL
);
  
EFI_STATUS SbPeiDeactivateSmi(
    IN EFI_PEI_SERVICES     **PeiServices,
    IN PEI_SMM_CONTROL_PPI  *This,
    IN BOOLEAN              Periodic OPTIONAL
);

static PEI_SMM_CONTROL_PPI mSmmControlPpi = {
    SbPeiActivateSmi,
    SbPeiDeactivateSmi
};
#endif  //SMM_SUPPORT


// PPI that are installed
#if SB_STALL_PPI_SUPPORT
static EFI_PEI_PPI_DESCRIPTOR mBeforeBootModePpiList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gEfiPeiStallPpiGuid, &mStallPpi },
};
#endif

static EFI_PEI_PPI_DESCRIPTOR mBootModePpi[] = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMasterBootModePpiGuid, NULL};

static EFI_PEI_PPI_DESCRIPTOR mRecoveryModePpi[] = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiBootInRecoveryModePpiGuid, NULL};

static EFI_PEI_PPI_DESCRIPTOR mPpiList[] = {
    {EFI_PEI_PPI_DESCRIPTOR_PPI, &gPeiAtaControllerPpiGuid, &mAtaControllerPpi},
#if SMM_SUPPORT
     {EFI_PEI_PPI_DESCRIPTOR_PPI, &gPeiSmmControlPpiGuid, &mSmmControlPpi},
#endif
#if SB_RESET_PPI_SUPPORT
    {EFI_PEI_PPI_DESCRIPTOR_PPI, &gEfiPeiResetPpiGuid, &mResetPpi},
#endif
    {(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmiPeiSbInitPolicyGuid, &mAmiPeiSbInitPolicyPpi}

};

// PPI that are notified
static EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gEfiEndOfPeiSignalPpiGuid, ProgramSbRegBeforeEndofPei },
};

static EFI_PEI_NOTIFY_DESCRIPTOR MemInstalledNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      // Denverton AptioV Override  Start - EIP#294261  Notify GUID modified to call this function after PCIE Bifurcation happened
      //&gEfiPeiMemoryDiscoveredPpiGuid, SbPeiInitAfterMemInstalled },
      &gEfiEndOfPeiSignalPpiGuid, SbPeiInitAfterMemInstalled },
      // Denverton AptioV Override  End - EIP#294261 
};

#define SB_DEVICE_NUMBER        0
#define TempBusDevFuncNo        (PEI_PCI_CFG_ADDRESS(0, SB_DEVICE_NUMBER, 0, 0))

extern AMI_PEI_PCI_INIT_TABLE_STRUCT stSBH2P_PCIInitTable [];
extern UINT16 wSBH2P_PCIInitTableSize;

EFI_STATUS CountTime(
    IN  UINTN DelayTime, 
    IN  UINT16 BaseAddr
);


// Function Definition

/**
    This function is the entry point for this PEI. This function
    initializes the chipset SB

    @param FfsHeader Pointer to the FFS file header
    @param PeiServices Pointer to the PEI services table

    @retval Return Status based on errors that occurred while waiting for
            time to expire.

    @note  This function should initialize South Bridge for memory detection.
              Install AMI_PEI_SBINIT_POLICY_PPI to indicate that SB Init PEIM
              is installed
              Following things can be done at this point:
                  - Enabling top of 4GB decode for full flash ROM
                  - Programming South Bridge ports to enable access to South
                    Bridge and other I/O bridge access

**/
EFI_STATUS SbPeiInit(
  IN EFI_FFS_FILE_HEADER *FfsHeader,
  IN EFI_PEI_SERVICES    **PeiServices
)
{
    EFI_STATUS                  Status;
    EFI_PEI_PCI_CFG2_PPI        *PciCfg;
    EFI_PEI_CPU_IO_PPI          *CpuIo;
    AMI_PEI_PCI_TABLE_INIT_PPI  *AmiPciTableInit;
#if defined CREATE_CMOS_BAD_HOB && CREATE_CMOS_BAD_HOB != 0
    UINT16                      HobSize = sizeof(CMOS_BAD_HOB);
    CMOS_BAD_HOB                *CmosBadHob;
#endif


    //Report Progress code
    PEI_PROGRESS_CODE (PeiServices, PEI_CAR_SB_INIT);

    // Get pointer to the PCI config & CPU I/O PPI
    PciCfg = (*PeiServices)->PciCfg;
    CpuIo = (*PeiServices)->CpuIo;

    SbWakeupTypeLib();

    // Locate AMI PCI Table Init PPI
    Status = (*PeiServices)->LocatePpi(
                            PeiServices,
                            &gAmiPeiPciTableInitPpiGuid,
                            0,
                            NULL,
                            &AmiPciTableInit
                            );
    // Assert if not found - the AMI PCI Table Init PPI should exist
    ASSERT_PEI_ERROR( PeiServices, Status );

    AmiPciTableInit->AMIPEIProgramPCITable(
                        PeiServices, AmiPciTableInit, PciCfg,
                        TempBusDevFuncNo,
                        stSBH2P_PCIInitTable,
                        wSBH2P_PCIInitTableSize);

    // Program GPIOs.
    InitGpio( PeiServices, CpuIo );

#if SB_STALL_PPI_SUPPORT
    // Install the SB Stall PPI before update Boot Mode
    Status = (*PeiServices)->InstallPpi( PeiServices, \
                                         &mBeforeBootModePpiList[0] );
    ASSERT_PEI_ERROR( PeiServices, Status );
#endif
  
#if defined DISABLE_DAYLIGHT_SAVINGS && DISABLE_DAYLIGHT_SAVINGS != 0
/** PORTING REQUIRED **
    // When BUC.SDO bit is set, Daylight Savings feature is ALWAYS DISABLED
    // regardless of the setting in RTC_REGB.DSE.
    SET_MEM16_RCRB(R_PCH_RCRB_BUC, B_PCH_RCRB_BUC_SDO);
**/
#endif  

#if defined CREATE_CMOS_BAD_HOB && CREATE_CMOS_BAD_HOB != 0
      Status = (*PeiServices)->CreateHob( PeiServices,
                                          EFI_HOB_TYPE_GUID_EXTENSION,
                                          HobSize,
                                          &CmosBadHob);
      if(!EFI_ERROR(Status)) {
        CmosBadHob->Header.Name = gAmiCmosBadHobGuid;
        CmosBadHob->Health = SbIsRtcPwrValid(); 
      }   
#endif

    // Denverton AptioV Override Start - EIP#288083  
    // Commenting out the below code as BootMode updation is done prior to this by PlatformStage1InitBootMode() function 
    // UpdateBootMode(PeiServices, CpuIo, PciCfg);      
    // Denverton AptioV Override End - EIP#288083 
      

    // Install the SB Init Policy PPI
    Status = (*PeiServices)->InstallPpi(PeiServices, &mPpiList[0]);
    ASSERT_PEI_ERROR (PeiServices, Status);

    // Setup a SBPEI entry after PEI permanent memory be installed
    Status = (*PeiServices)->NotifyPpi( PeiServices, MemInstalledNotifyList );
    ASSERT_PEI_ERROR ( PeiServices, Status );

    return EFI_SUCCESS;
}


#if SB_RESET_PPI_SUPPORT

/**
    This function is the reset call interface function published
    by the reset PPI

    @param PeiServices Pointer to the PEI services table

    @retval EFI_STATUS

**/
EFI_STATUS SbPeiResetSystem(
    IN EFI_PEI_SERVICES **PeiServices
)
{

    SBLib_ResetSystem(COLD_RESET);

    // We should never get this far
    return EFI_SUCCESS;
}

#endif


#if SB_STALL_PPI_SUPPORT

/**
    This function is used to stall the boot process (provides delay)
    for specified number of microseconds

    @param PeiServices Pointer to the PEI services table
    @param This Pointer to the Stall PPI
    @param MicroSeconds Time to wait for (in Micro seconds)

    @retval EFI_STATUS

**/
EFI_STATUS SbPeiStall(
    IN EFI_PEI_SERVICES     **PeiServices, 
    IN EFI_PEI_STALL_PPI    *This, 
    IN UINTN                MicroSeconds
)
{
    EFI_STATUS              Status;
    EFI_PEI_PCI_CFG2_PPI    *PciCfg;
//  UINT32                  Buffer32;

    // Get pointer to the PCI config PPI
    PciCfg = (*PeiServices)->PciCfg;

/*** NEEDS PORTING ****
    PciCfg->Read(
                PeiServices,
                PciCfg,
                EfiPeiPciCfgWidthUint32,
                LPC_BUS_DEV_FUNC | REG_PMBASE,
                &Buffer32
                );
 ***/

    // At this time no manipulation needed.  The value passed in is in
    //  MicroSeconds(us) and that is what the library function uses

    // Call Library function that is shared with Metronome
    //  Architecture Protocol
    Status = CountTime(MicroSeconds, PM_BASE_ADDRESS);

    return Status;
}

#endif

/**
    This function initializes SB GPIOs

    @param PeiServices - Pointer to the PEI services table
    @param CpuIo       - Pointer to the CPU I/O PPI

    @retval VOID
**/
VOID InitGpio (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo )
{

    EFI_STATUS                  Status = EFI_SUCCESS;
    AMI_PEI_SB_CUSTOM_PPI       *SbPeiOemPpi;
    AMI_GPIO_INIT_TABLE_STRUCT  *GpioInitTable = DefaultGpioTable;

    Status = (*PeiServices)->LocatePpi( PeiServices, \
                                        &gAmiPeiSbCustomPpiGuid, \
                                        0, \
                                        NULL, \
                                        &SbPeiOemPpi );
/**** PORTING REQUIRED ****

    if (Status == EFI_SUCCESS) {
        if (SbPeiOemPpi->GpioInit != NULL) {
            if (SbPeiOemPpi->GpioInit->GpioBaseAddr == GPIO_BASE_ADDRESS) {
                if (SbPeiOemPpi->GpioInit->InitDefaultGpioSetting)
                // Program SB default GPIO setting.
                    ProgramGpio( PeiServices, \
                                 CpuIo, \
                                 SbPeiOemPpi->GpioInit->GpioBaseAddr, \
                                 GpioInitTable );

                GpioInitTable = SbPeiOemPpi->GpioInit->GpioTable;
                ProgramGpio( PeiServices, \
                             CpuIo, \
                             SbPeiOemPpi->GpioInit->GpioBaseAddr, \
                             GpioInitTable );
            }
        } else {
            ProgramGpio(PeiServices, CpuIo, GPIO_BASE_ADDRESS, GpioInitTable);
        }
    } else {
        ProgramGpio(PeiServices, CpuIo, GPIO_BASE_ADDRESS, GpioInitTable);
    }

    // Clear GPI Status (0x2A)
    CpuIo->IoWrite16( PeiServices, CpuIo, \
                      PM_BASE_ADDRESS + ACPI_IOREG_GPE0_STS, 0xffff );
****/
}

/**
    This function programs SB PCI devices sub-vendor ID and
    sub-system ID.

    @param PeiServices - Pointer to the PEI services table
    @param PciCfg      - Pointer to the PCI Configuration PPI

    @retval VOID

    @note  1. This routine only programs the PCI device in SB, hence, we
                 have to check the bus/device/function numbers whether they
                 are a SB PCI device or not.
              2. This routine is invoked by PEI phase.(After PEI permanent
                 memory be installed)
**/
VOID ProgramSbSubId (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg )
{
    EFI_STATUS                    Status = EFI_SUCCESS;
    AMI_PEI_SB_CUSTOM_PPI         *SbPeiOemPpi;
    UINTN                         i = 0;
    UINT32                        PciSid = 0xffffffff;
    AMI_SB_PCI_SSID_TABLE_STRUCT  DefaultSidTbl[] = {SB_PCI_DEVICES_SSID_TABLE};
    AMI_SB_PCI_SSID_TABLE_STRUCT  *SsidTblPtr = DefaultSidTbl;

    Status = (*PeiServices)->LocatePpi( PeiServices, \
                                        &gAmiPeiSbCustomPpiGuid, \
                                        0, \
                                        NULL, \
                                        &SbPeiOemPpi );

    if (Status == EFI_SUCCESS) {
        if (SbPeiOemPpi->SsidTable != NULL) 
            SsidTblPtr = SbPeiOemPpi->SsidTable;
    }
    // Denverton AptioV override Start - EIP#270210 - SSVID and SSID programming for Denverton 
    while (SsidTblPtr[i].PciAddr != 0xffffffffffffffff) {
        if (SsidTblPtr[i].Sid == 0xffffffff) {
            Status = PciCfg->Read( PeiServices,
                                    PciCfg,
                                    EfiPeiPciCfgWidthUint32,
                                    SsidTblPtr[i].PciAddr,
                                    &PciSid);
        } else {
             PciSid = SsidTblPtr[i].Sid;
        }
        // Denverton AptioV Override  Start - EIP#294261  Included RP devices Check
        // For Root Ports, Virtual root ports dedicated to QAT, LAN Controller 0 and LAN Controller 1, write to 0x8c
        if (SsidTblPtr[i].PciAddr ==  RP0_BUS_DEV_FUN || SsidTblPtr[i].PciAddr ==  RP1_BUS_DEV_FUN || SsidTblPtr[i].PciAddr ==  RP2_BUS_DEV_FUN || \
            SsidTblPtr[i].PciAddr ==  RP3_BUS_DEV_FUN || SsidTblPtr[i].PciAddr ==  RP4_BUS_DEV_FUN || SsidTblPtr[i].PciAddr ==  RP5_BUS_DEV_FUN || \
            SsidTblPtr[i].PciAddr ==  RP6_BUS_DEV_FUN || SsidTblPtr[i].PciAddr ==  RP7_BUS_DEV_FUN || SsidTblPtr[i].PciAddr ==  QAT_BUS_DEV_FUN|| \
            SsidTblPtr[i].PciAddr ==  LAN0_BUS_DEV_FUN || SsidTblPtr[i].PciAddr ==  LAN1_BUS_DEV_FUN ){        
        // Denverton AptioV Override  End - EIP#294261 
            Status = PciCfg->Write( PeiServices,
                                   PciCfg,
                                   EfiPeiPciCfgWidthUint32,
                                   SsidTblPtr[i].PciAddr | PCI_vRP_SVID,
                                   &PciSid);
            
            // Denverton AptioV Override Start - EIP#305851
            PciSid = B_PCH_PCIE_PLKCTL_CL;        // Capability Lock (CL) key bit for all RW-L bits
            
            //
            // Lock Key bit for all RW-L bits (capabilities, next capability pointer, SSID/SVID, slot register, etc)
            //
            Status = PciCfg->Write( PeiServices,
                                   PciCfg,
                                   EfiPeiPciCfgWidthUint32,
                                   SsidTblPtr[i].PciAddr | R_PCH_PCIE_PLKCTL,
                                   &PciSid);
            // Denverton AptioV Override End - EIP#305851
            
        }
        // For other PCI, write to 0x2c
        else {
           Status = PciCfg->Write( PeiServices,
                                   PciCfg,
                                   EfiPeiPciCfgWidthUint32,
                                   SsidTblPtr[i].PciAddr | PCI_SVID,
                                   &PciSid);
        }
        i++;
    }
    // Denverton AptioV override End - EIP#270210 - SSVID and SSID programming for Denverton
}

/**
    This function can be used to program any SB register after
    PEI permanent memory be installed.

    @param PeiServices      - Pointer to the PEI services table
    @param NotifyDescriptor - Pointer to the descriptor for the
                              notification event.
    @param InvokePpi        - Pointer to the PPI that was installed

    @retval EFI_STATUS
**/
EFI_STATUS SbPeiInitAfterMemInstalled (
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

    PEI_PROGRESS_CODE (PeiServices, PEI_MEM_SB_INIT);

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);

    if (BootMode == BOOT_ON_S3_RESUME) {

    } else {

    }

    // Program SB Devices' Subsystem Vendor ID & Subsystem ID
    ProgramSbSubId( PeiServices, PciCfg );

    // Set up necessary PPI notifications after PEI permanent memory
    // be installed
    Status = (*PeiServices)->NotifyPpi( PeiServices, &mNotifyList[0] );
    ASSERT_PEI_ERROR ( PeiServices, Status );

    return  EFI_SUCCESS;
}

/**
    This function can be used to program any SB register before
    end of PEI phase.

    @param PeiServices      - Pointer to the PEI services table
    @param NotifyDescriptor - Pointer to the descriptor for the
                              notification event.
    @param InvokePpi        - Pointer to the PPI that was installed

    @retval EFI_STATUS
**/
EFI_STATUS ProgramSbRegBeforeEndofPei (
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

/**
    This function is used to initialize the IDE ATA channel
    for BIOS recovery from ATA devices

    @param PeiServices Pointer to the PEI services table
    @param This Pointer to the PEI ATA Controller PPI
    @param ChannelMask Bit flag indicating which channel has to be
           enabled. The following is the bit definition:
            Bit0    IDE Primary
            Bit1    IDE Secondary
            Bit2    No SATA 
            Bit3    SATA as Primary
            Bit4    SATA as Secondary

    @retval EFI_STATUS

**/
EFI_STATUS SbPeiEnableAtaChannelForRecovery(
    IN EFI_PEI_SERVICES         **PeiServices,
    IN PEI_ATA_CONTROLLER_PPI   *This,
    IN UINT8                    ChannelMask
)
{
  /*
    UINT8                           Func,
    UINT8                           SetData; 
    UINT8                           ResetData; 
    UINT8                           PciData8=0;
    UINT8                           *SbRcba = (UINT8*)SB_RCBA;
    UINTN                           VariableSize;
    EFI_GUID                        gSetupGuid = SETUP_GUID;
    EFI_STATUS                      Status;
    SB_SETUP_DATA                   SetupData;
    EFI_PEI_PCI_CFG2_PPI            *PciCfg;
    EFI_HOB_HANDOFF_INFO_TABLE      *pHob;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadOnlyVariable;
    UINT16   Zero16 = 0;
    UINT16   Buffer16 = 0;


    PciCfg = (*PeiServices)->PciCfg;

    // This will execute only if it is in recovery.  
    Status = (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid,
                    0, NULL, &ReadOnlyVariable);
    ASSERT_PEI_ERROR(PeiServices, Status);

    // Get memory for the Setup data
//  (*PeiServices)->GetHobList(PeiServices,&pHob);
//  SetupData = (SETUP_DATA*)pHob->EfiFreeMemoryBottom;
//  VariableSize = (UINTN)(pHob->EfiFreeMemoryTop - pHob->EfiFreeMemoryBottom);

    // Get the setup data

    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup",
                    &gSetupGuid, NULL, &VariableSize, SetupData);

    GetSbSetupData(*PeiServices, &SetupData, TRUE);

    if (EFI_ERROR(Status)) 
    {
        // Setup data not available. Program default programming
        // Porting Required //
        Func = PCI_FUNCTION_NUMBER_ICH_IDE;
    } 
    else 
    {
        // Setup data available. Program based on setup option
        // Porting Required //
        if (SetupData.IdeMode == 0)
        {
            //*(UINT32 *)(SbRcba + 0x3418) |= 0x00f6;
        }
        else
        {
            if(SetupData.IdeMode == 2) 
            {
                //Func = PCI_FUNCTION_NUMBER_ICH_IDE;
            } 
            else 
            {
                Func = PCI_FUNCTION_NUMBER_ICH_SATA;
                Buffer16 = 0x7f;
                *(UINT32 *)(SbRcba + 0x3418) |= 0x00f3;
                PciCfg->Modify (PeiServices, PciCfg, EfiPeiPciCfgWidthUint16,
                        ((PCI_DEVICE_NUMBER_ICH_IDE << 16) | (PCI_FUNCTION_NUMBER_ICH_SATA << 8) | Programing_Interface),
                        &Zero16, &Buffer16);
                ;
            }
        }
    }

    // Porting Required //
    PciCfg->Modify (PeiServices, PciCfg, EfiPeiPciCfgWidthUint32,
            ((PCI_DEVICE_NUMBER_ICH_IDE << 16) | (Func << 8) | R_ICH_IDE_BASE_ADDRESS),  // Register
            ICH_IDE_BASE_ADDRESS, B_ICH_IDE_BASE_ADDRESS_BAR);

    // Enable IDE Controller
    PciCfg->Modify (PeiServices, PciCfg, EfiPeiPciCfgWidthUint16,
            ((PCI_DEVICE_NUMBER_ICH_IDE << 16) | (Func << 8) | R_ICH_IDE_COMMAND),
            (B_ICH_IDE_COMMAND_IOSE | B_ICH_IDE_COMMAND_BME), 0);  

    PciCfg->Modify (PeiServices, PciCfg, EfiPeiPciCfgWidthUint32,
            ((PCI_DEVICE_NUMBER_ICH_IDE << 16) | (Func << 8) | R_ICH_IDE_CONFIG), // Register
            0, // SetBits
            0x30000 // ClearBits
            );


    if ((ChannelMask & PEI_ICH_IDE_PRIMARY) == PEI_ICH_IDE_PRIMARY) {
        PciCfg->Modify (PeiServices, PciCfg, EfiPeiPciCfgWidthUint16,
                ((PCI_DEVICE_NUMBER_ICH_IDE << 16) | (Func << 8) | R_ICH_IDE_TIMP),
                B_ICH_IDE_DEC_EN, 0);
    }

    if ((ChannelMask & PEI_ICH_IDE_SECONDARY) == PEI_ICH_IDE_SECONDARY) {
        // Enable Secondary IDE Channel
       PciCfg->Modify (PeiServices, PciCfg, EfiPeiPciCfgWidthUint16,
            ((PCI_DEVICE_NUMBER_ICH_IDE << 16) | (Func << 8) | R_ICH_IDE_TIMS),
            B_ICH_IDE_DEC_EN, 0);
    }

    SetData = 0; ResetData = 0;

    if (SetupData.IdeMode == 0 || SetupData.IdeMode == 1) 
    {
        if (SetupData.Sata0 == 1) 
        {
            //SetData = B_SATA_PORT_0_ENABLED;
        } 
        else 
        {
            //ResetData = B_SATA_PORT_0_ENABLED;
        }
        if (SetupData.Sata2 == 1)
        {
            //SetData |= B_SATA_PORT_2_ENABLED;
        } 
        else 
        {
            //ResetData |= B_SATA_PORT_2_ENABLED;
        }           
        //Enable SATA ports
        PciCfg->Modify (PeiServices, PciCfg, EfiPeiPciCfgWidthUint8,
                ((PCI_DEVICE_NUMBER_ICH_IDE << 16) | (PCI_FUNCTION_NUMBER_ICH_SATA << 8) | R_ICH_SIDE_PCS ),
                &SetData, &ResetData);
    }

    switch (SetupData.IdeMode) {
        case 0:
//          PciData8 |= V_SATA_MAP_NON_COMBINED;
            break;

        case 1:
//          PciData8 |= V_SATA_MAP_COMBINED_SATA;
            break;

        case 2:
//          PciData8 |= V_SATA_MAP_NON_COMBINED;
            break;
    }

    //Program AMR register based on setup
    PciCfg->Modify (PeiServices, PciCfg, EfiPeiPciCfgWidthUint16,
            ((PCI_DEVICE_NUMBER_ICH_IDE << 16) | (PCI_FUNCTION_NUMBER_ICH_SATA << 8) | R_ICH_SATA_AMR ), 
            PciData8, 0x03);

    if (SetupData.IdeMode == 2) {
        *(UINT32 *)(SbRcba + 0x3418) |= 0x00f5;
    }
*/

    return EFI_SUCCESS;
}

#if     SMM_SUPPORT

/**
    This function is used to generate S/W SMI in the system. This
    call is mainly used during S3 resume to restore SMMBase

    @param PeiServices Pointer to the PEI services table
    @param This Pointer to the SMM Control PPI
    @param ArgumentBuffer Argument to be used to generate S/W SMI
    @param ArgumentBufferSize Size of the argument buffer
    @param Periodic Indicates the type of SMI invocation
    @param ActivationInterval If it is periodic invocation, this field
           indicates the period at which the SMI is generated

    @retval EFI_STATUS

**/
EFI_STATUS 
SbPeiActivateSmi (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN PEI_SMM_CONTROL_PPI  *This,
    IN OUT INT8             *ArgumentBuffer OPTIONAL,
    IN OUT UINTN            *ArgumentBufferSize OPTIONAL,
    IN BOOLEAN              Periodic OPTIONAL,
    IN UINTN                ActivationInterval OPTIONAL
)
{
    UINT8   Data;
//  UINT8   Value;

    if (Periodic) return EFI_INVALID_PARAMETER;

    if (ArgumentBuffer == NULL) {
        Data = 0xFF;                        //If no data given, use 0xFF to trigger SMI.
    } else {
        if (ArgumentBufferSize == NULL || *ArgumentBufferSize != 1) 
            return EFI_INVALID_PARAMETER;   // Only able to send 1 byte.
        Data = *ArgumentBuffer;
    }

/** Porting required. Include code to generate S/W SMI
    // Enable Software SMIs
    Value =  IoRead8(PM_BASE_ADDRESS + ICH_SMI_EN) | (1 << 5) | 1;
    IoWrite8(PM_BASE_ADDRESS + ICH_SMI_EN, Value);

    IoWrite8(ICH_APM_CNT,Data);     //This triggers SMI

 Porting End **/

    return EFI_SUCCESS;
}
  
/**
    This function is used to clear the SMI and issue End-of-SMI

    @param PeiServices Pointer to the PEI services table
    @param This Pointer to the SMM Control PPI
    @param Periodic Indicates the type of SMI invocation to stop

    @retval EFI_STATUS

**/
EFI_STATUS SbPeiDeactivateSmi(
    IN EFI_PEI_SERVICES     **PeiServices,
    IN PEI_SMM_CONTROL_PPI  *This,
    IN BOOLEAN              Periodic OPTIONAL
)
{
//  UINT8       Value;

    if (Periodic) return EFI_INVALID_PARAMETER;

    /** Porting Required.  Include code to clear all SMI status
    // Clear the Power Button Override Status Bit
    IoWrite16(PM_BASE_ADDRESS, 1 << 8);

     // Clear the APM SMI Status Bit
    IoWrite8(PM_BASE_ADDRESS+ICH_SMI_STS, 1 << 5);

    //Set EOS
    Value = IoRead8(PM_BASE_ADDRESS+ICH_SMI_EN) | 2;
    IoWrite8(PM_BASE_ADDRESS+ICH_SMI_EN,Value);
**/ // Porting end

    return EFI_SUCCESS;
}
#endif

/**
    Get the wake type and save it in PCD.

    @param

    @retval VOID

    @note
**/
VOID SbWakeupTypeLib (
    VOID
)
{
    SB_WAKEUP_TYPE  SbWakeType = DEFAULT_SYSTEM_WAKEUP_TYPE;
    /**** PORTING REQUIRED ****
    UINT16          Pm1Status = IoRead16(PM_BASE_ADDRESS + SB_ACPI_PM1_STS);
    UINT16          Pm1En = IoRead16(PM_BASE_ADDRESS + SB_ACPI_PM1_EN);
    UINT32          Gpe0Status = IoRead32(PM_BASE_ADDRESS + SB_ACPI_GPE0_STS);
    UINT32          Gpe0En = IoRead32(PM_BASE_ADDRESS + SB_ACPI_GPE0_EN);

    if (SbCheckAfterG3()) {
      SbWakeType = SbWakeupTypeAcPowerRestored;
    } else if (Pm1Status & BIT15) {
      // Power button
      if (Pm1Status & Pm1En & BIT8) {
        SbWakeType = SbWakeupTypePowerSwitch;
      } else if (Pm1Status & Pm1En & BIT10) {
        SbWakeType = SbWakeupTypeApmTimer;
      } else if (Gpe0Status & Gpe0En & BIT13) {
        SbWakeType = SbWakeupTypePciPme;
      } else {
        SbWakeType = SbWakeupTypeOther; // don't know what the type is. Let's assign "Other".
      }
    } else {
      SbWakeType = SbWakeupTypeUnknown; // not sure what's is.
    }
    **** PORTING REQUIRED ****/
    PcdSet8(PcdSbWakeType, SbWakeType);
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

