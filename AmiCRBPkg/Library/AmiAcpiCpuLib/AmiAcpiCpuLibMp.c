#include <Token.h>
#include <AmiHobs.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>
#include <Protocol/AmiCpuInfo.h>
// Denverton AptioV Override Start - EIP#138757
#include <Protocol\MpService.h>
#include <Library/LocalApicLib.h>
// Denverton AptioV Override End - EIP#138757

extern EFI_GUID gCpuInfoHobGuid;
extern CPUINFO_HOB *gCpuInfoHob;

#ifdef ACPI_INFO2_PROTOCOL_PUBLISHED
#include <Protocol/AmiCpuInfo2.h>
extern AMI_CPU_INFO_2_PROTOCOL* gAmiCpu2Info;
extern EFI_GUID gAmiCpuInfo2ProtocolGuid;
#endif

AMI_CPU_INFO_PROTOCOL *gAmiCpuInfo = NULL;
EFI_GUID gAmiCpuInfoProtocolGuid = AMI_CPU_INFO_PROTOCOL_GUID;

#define MP_TABLE_CPU_ENTRY_TYPE                  0
// Denverton AptioV Override Start - EIP#138757
typedef struct {
    UINT32 ProcessorNumber;
    UINT32 ApicId;
    UINT32 StatusFlag;
} PROCESSOR_APICID_INFO;

PROCESSOR_APICID_INFO *gProcApicIdInfo;
EFI_MP_SERVICES_PROTOCOL        *gMpServices;
VOID *gMpEntries;
UINT32 gNumOfMpEntries;
UINT32 gMpTableSize;
UINTN gNumberOfCPU;
UINT32 ApicVersion; 
// Denverton AptioV Override End - EIP#138757
typedef struct _MP_TABLE_CPU_ENTRY
{
    UINT8   EntryType;
    UINT8   LocalApicId;
    UINT8   LocalApicVersion;
    struct  
    {
        UINT8   CpuEnable    : 1;
        UINT8   CpuBootStrap : 1;
        UINT8   Reserved     : 6;
    } CpuFlags;
    UINT32  CpuSignature;
    UINT32  FeatureFlags;
    UINT32  Reserved[2];
} MP_TABLE_CPU_ENTRY;

#ifdef ACPI_INFO2_PROTOCOL_PUBLISHED
static EFI_STATUS CreateMpEntriesByCpuInfo2Protocol(OUT VOID **Entries, OUT UINT32 *NumEntries,  OUT UINT32 *TableSize)
{
    ACPI_PROCESSOR_INFO *AcpiProcInfo;
    EFI_STATUS  Status;
    UINT32      Index;
    UINT32      NumProtEntries;
    MP_TABLE_CPU_ENTRY *Entry;

    //Use AMI_CPU_INFO_2_PROTOCOL if published.
    if (gAmiCpu2Info == NULL) {
        Status = gBS->LocateProtocol(&gAmiCpuInfo2ProtocolGuid, NULL, &gAmiCpu2Info);
        if (EFI_ERROR(Status)) return Status;
    }

    if (gAmiCpu2Info->ProtocolVer < 3) return EFI_UNSUPPORTED;

    Status = gAmiCpu2Info->GetAcpiInfo(
        gAmiCpu2Info, (VOID**) &AcpiProcInfo, &NumProtEntries
    );
    if (EFI_ERROR(Status)) return Status;

    *NumEntries = 0; 
    
    //Calcuate Number of Entries to determine memory allocation size.
    for (Index = 0; Index < NumProtEntries; ++Index) {
      //Note: Only some ACPI_PROCESSOR_INFO fields may only valid for some minimum ACPI_PROCESSOR_INFO.Length.
      //The ACPI_PROCESSOR_INFO may have additional fields added in later version. Please see comments in
      //AmiCpuInfo2.h file.
          if (AcpiProcInfo[Index].Type == ACPI_PROCESSOR_INFO_TYPE && AcpiProcInfo[Index].Thread == 0)  ++*NumEntries;
    }

    *TableSize  = sizeof(MP_TABLE_CPU_ENTRY) * (*NumEntries);
    *Entries = AllocatePool(*TableSize);
    ASSERT(*Entries != NULL);
    
    Entry = (MP_TABLE_CPU_ENTRY*)*Entries;
    
   for (Index = 0; Index < NumProtEntries; ++Index) {
    //Note: Only some ACPI_PROCESSOR_INFO fields may only valid for some minimum ACPI_PROCESSOR_INFO.Length.
    //The ACPI_PROCESSOR_INFO may have additional fields added in later version. Please see comments in
    //AmiCpuInfo2.h file.
        if (AcpiProcInfo[Index].Type == ACPI_PROCESSOR_INFO_TYPE
            && AcpiProcInfo[Index].Thread == 0
        ) {
            Entry->EntryType = MP_TABLE_CPU_ENTRY_TYPE;
            Entry->LocalApicId = AcpiProcInfo[Index].ApicId;
            Entry->LocalApicVersion = AcpiProcInfo[Index].ApicVer;
            Entry->CpuFlags.CpuEnable = AcpiProcInfo[Index].Enable;
            Entry->CpuFlags.CpuBootStrap = AcpiProcInfo[Index].Bsp;
            Entry->CpuFlags.Reserved = 0;
            Entry->CpuSignature = AcpiProcInfo[Index].CpuSignature;
            Entry->FeatureFlags = AcpiProcInfo[Index].FeatureFlags;
            Entry->Reserved[0] = 0;
            Entry->Reserved[1] = 0;
            ++Entry;
        }
   }

   ASSERT(((UINT8*)Entry - (UINT8*)*Entries) == (sizeof(MP_TABLE_CPU_ENTRY) * (*NumEntries)));

   return EFI_SUCCESS;
}
#endif


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    GetCpuInfo
// 
// Description:  Returns number of cores and threads of given CPU
//
// Input:
// IN UINTN Cpu - Cpu number
// OUT UINT8 *Cores - number of cores in given Cpu
// OUT UINT8 *Threads - number of threads in given Cpu
//   
// Output:
// EFI_SUCCESS - function executed successfully
//
//--------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS GetCpuInfo(
    IN UINTN Cpu,
    OUT UINT8 *Cores,
    OUT UINT8 *Threads)
{
    EFI_STATUS Status;
    AMI_CPU_INFO *Info;

    Status = gAmiCpuInfo->GetCpuInfo(gAmiCpuInfo, Cpu, &Info);
    if(EFI_ERROR(Status)) return Status;

    *Cores = Info->NumCores;
    *Threads = (Info->NumHts == 0) ? 1 : Info->NumHts;
    
    return EFI_SUCCESS;
}

EFI_STATUS CreateMpEntriesByHobAndCpuInfoProtocol(OUT VOID **Entries, OUT UINT32 *NumEntries, OUT UINT32 *TableSize)
{
    EFI_STATUS Status;
    UINT32      CpuIndex;
    UINT32     Index;
    UINT8       NumberOfCores;
    UINT8       NumberOfThreads;
    MP_TABLE_CPU_ENTRY *Entry;

    if (gCpuInfoHob == NULL) {
        gCpuInfoHob = (CPUINFO_HOB*) GetFirstGuidHob (&gCpuInfoHobGuid);
        if (gCpuInfoHob == NULL) return EFI_NOT_FOUND;
    }
    
    if (gAmiCpuInfo == NULL) {
        Status = gBS->LocateProtocol(&gAmiCpuInfoProtocolGuid, NULL, &gAmiCpuInfo);
        if (EFI_ERROR(Status)) return Status;
    }

    *NumEntries = 0;
    CpuIndex = 0;
    while(!EFI_ERROR(GetCpuInfo(CpuIndex, &NumberOfCores, &NumberOfThreads))) {
        *NumEntries += NumberOfCores;
        CpuIndex += NumberOfCores * NumberOfThreads;
    }
                 
    *TableSize = sizeof(MP_TABLE_CPU_ENTRY) * (*NumEntries);
    *Entries = AllocatePool(*TableSize);
    ASSERT(*Entries != NULL);

    Entry = (MP_TABLE_CPU_ENTRY*)*Entries;

    CpuIndex = 0;	//CpuIndex is the iundex of the Physical CPU Core 0, Thread 0. Index is the index of the different cores and threads.
    while(!EFI_ERROR(GetCpuInfo(CpuIndex, &NumberOfCores, &NumberOfThreads))) {
        for(Index = 0;  Index < ((UINT32)NumberOfCores * (UINT32)NumberOfThreads); Index += NumberOfThreads) {
            CPUINFO *CpuInfo = &gCpuInfoHob->Cpuinfo[CpuIndex + Index];	//CpuIndex + Index = index of physical CPU, Core, and thread.

            Entry->EntryType = MP_TABLE_CPU_ENTRY_TYPE;
            Entry->LocalApicId = CpuInfo->ApicId;
            Entry->LocalApicVersion = CpuInfo->ApicVer;
            Entry->CpuFlags.CpuEnable = (CpuInfo->Disabled) ? 0 : 1;
            Entry->CpuFlags.CpuBootStrap = ((Index + CpuIndex) == gCpuInfoHob->BspNo) ? 1 : 0;
            Entry->CpuFlags.Reserved = 0;
            Entry->CpuSignature = CpuInfo->CpuSignature;
            Entry->FeatureFlags = (UINT32) CpuInfo->CpuFeatureInfo;
            Entry->Reserved[0] = 0;
            Entry->Reserved[1] = 0;
            ++Entry;
        }
        CpuIndex += NumberOfCores * NumberOfThreads;	//Get the index of the next physical CPU, core 0 thread 0.
    }

    ASSERT(((UINT8*)Entry - (UINT8*)*Entries) == (sizeof(MP_TABLE_CPU_ENTRY) * (*NumEntries)));

    return EFI_SUCCESS;
}
// Denverton AptioV Override Start - EIP#138757
VOID
AddSetupMpTableEntry ( 
  IN OUT VOID  *DataBuffer
  )
{
	UINTN  ProcNum;
	UINT32 Index;
	UINT32 CpuSignature;
	UINT32 FeatureFlags;
	volatile MP_TABLE_CPU_ENTRY *Entry;
	
	gMpServices->WhoAmI(gMpServices, &ProcNum);
	for (Index = 0; Index < (UINT32)gNumberOfCPU;++Index) {
	   if ((UINT32)ProcNum == gProcApicIdInfo[Index].ProcessorNumber) 
		  break;
	}
	ASSERT(Index < gNumberOfCPU);
	
	AsmCpuid (1, &CpuSignature, NULL, NULL, &FeatureFlags);
	    
	Entry = &((volatile MP_TABLE_CPU_ENTRY*)gMpEntries)[Index];
	Entry->EntryType = MP_TABLE_CPU_ENTRY_TYPE;
    Entry->LocalApicId = gProcApicIdInfo[Index].ApicId;
    Entry->LocalApicVersion = ApicVersion;
    Entry->CpuFlags.CpuEnable = (gProcApicIdInfo[Index].StatusFlag > 1) & 1;
    Entry->CpuFlags.CpuBootStrap = gProcApicIdInfo[Index].StatusFlag & 1;
    Entry->CpuFlags.Reserved = 0;
    Entry->CpuSignature = CpuSignature;
    Entry->FeatureFlags = FeatureFlags;
    Entry->Reserved[0] = 0;
    Entry->Reserved[1] = 0;
}

EFI_STATUS CreateCpuMpTableEntries(OUT VOID **Entries, OUT UINT32 *NumEntries, OUT UINT32 *TableSize)
{
    EFI_STATUS                    Status;
	EFI_PROCESSOR_INFORMATION     ProcessorInfo;
	UINTN                         NumberOfEnabledCPUs;
	UINT8                         Index;
	
    //Get number of CPUs.
	Status = gBS->LocateProtocol(
        &gEfiMpServiceProtocolGuid,
        NULL,
        &gMpServices
        );
	ASSERT_EFI_ERROR(Status);
    if (!EFI_ERROR(Status)) {
        Status = gMpServices->GetNumberOfProcessors( 
        		                gMpServices, 
        		                &gNumberOfCPU, 
        		                &NumberOfEnabledCPUs );
        ASSERT_EFI_ERROR(Status);
    }
    
    gNumOfMpEntries = (UINT32)gNumberOfCPU;
	gMpTableSize = gNumOfMpEntries * sizeof(MP_TABLE_CPU_ENTRY);
	gMpEntries = AllocatePool(gMpTableSize);
	ASSERT(gMpEntries != NULL);
	
	gProcApicIdInfo = AllocatePool(gNumberOfCPU * sizeof(PROCESSOR_APICID_INFO));
	ASSERT(gProcApicIdInfo != NULL);
	
	for (Index = 0; Index < (UINT32)gNumberOfCPU;++Index) {
		gMpServices->GetProcessorInfo(gMpServices, Index, &ProcessorInfo);
		gProcApicIdInfo[Index].ProcessorNumber = Index;
		gProcApicIdInfo[Index].ApicId = (UINT32)ProcessorInfo.ProcessorId;
		gProcApicIdInfo[Index].StatusFlag = ProcessorInfo.StatusFlag;
	}
	
	//
	// Read bits 0..7 of Local APIC Version Register for Local APIC version.
	//
    ApicVersion = GetApicVersion () & 0xff;
	
    // Denverton AptioV Override Start - EIP#138757
    AddSetupMpTableEntry(NULL); 
    gMpServices->StartupAllAPs(gMpServices, AddSetupMpTableEntry, TRUE, NULL, 0, NULL, NULL);
    // Denverton AptioV Override End - EIP#138757
    
	*NumEntries = gNumOfMpEntries;
	*TableSize = gMpTableSize;
	*Entries = AllocatePool(gMpTableSize);
	ASSERT(*Entries != NULL);
	gBS->CopyMem(*Entries, gMpEntries, gMpTableSize);
	
	return EFI_SUCCESS;
/*
#ifdef ACPI_INFO2_PROTOCOL_PUBLISHED
    Status = CreateMpEntriesByCpuInfo2Protocol(Entries, NumEntries, TableSize);
    if (!EFI_ERROR(Status)) return EFI_SUCCESS;
#endif
    Status =  CreateMpEntriesByHobAndCpuInfoProtocol(Entries, NumEntries, TableSize);
    
    return Status;*/
}
// Denverton AptioV Override End - EIP#138757
