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

/** @file NbSetup.c
    This C file contains code related to NB setup routines

*/

#include <Setup.h>
#include <AmiDxeLib.h>
#include <NB.h>
// Denverton AptioV Override Start - EIP#217439
#define MRC_DATA_REQUIRED_FROM_OUTSIDE
#include <Library/HobLib.h>
#include <Guid/MemoryConfigData.h>
#include "MmrcData.h"
// Denverton AptioV Override End - EIP#217439

/**
    Initializes North Bridge Setup String

    @param HiiHandle - Handle to HII database
    @param Class - Indicates the setup class

    @retval VOID

    @note  PORTING REQUIRED
**/
VOID 
InitNbStrings (
    IN EFI_HII_HANDLE      HiiHandle, 
    IN UINT16              Class
)
{
//  UINT8                     DraValue_A;
//  UINT8                     DraValue_B;
//  UINT8                     DrbValue_A;
//  UINT8                     DrbValue_B;
  UINT64                    MemorySize;
  // Denverton AptioV Override Start - EIP#217439
  MRC_PARAMETERS            *MrcParameters = NULL;
  EFI_HOB_GUID_TYPE         *GuidHob = NULL;
  // Denverton AptioV Override End - EIP#217439

  if ((Class == MAIN_FORM_SET_CLASS) || (Class == CHIPSET_FORM_SET_CLASS)) {

// SAMPLE CODE BASED ON INTEL 945 Chipset
//    DraValue_A = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_A_DRA_0_1);
//    DraValue_B = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_B_DRA_0_1);
//    DrbValue_A = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_A_DRB_3);
//    DrbValue_B = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_B_DRB_3);
//    if ((DraValue_A == DraValue_B) && (DrbValue_A == DrbValue_B)){
//        DrbValue_B = DrbValue_B * 2;
//    }

    // *** PORTING NEEDED **
    // Read the appropriate chipset registers and update MemorySize with 
    // total memory present in the system in MB

//    MemorySize = (UINT64) (DrbValue_B * 32);
    // Denverton AptioV Override Start - EIP#217439
    //
    // Retrieve GUID HOB data that contains the Memory Information.
    //
	GuidHob = GetFirstGuidHob (&gEfiMemoryConfigDataGuid);
	ASSERT (GuidHob != NULL);
	if(GuidHob != NULL)
	{
	  MrcParameters = (MRC_PARAMETERS *)GET_GUID_HOB_DATA (GuidHob);
	  if(MrcParameters != NULL)
	  {
	    MemorySize = MrcParameters->SystemMemorySize;     
	    InitString(
              HiiHandle,
              STRING_TOKEN(STR_MEMORY_SIZE_VALUE),
              L"%4d MB (DDR4)",
              MemorySize
              );
	  }
	}
	// Denverton AptioV Override End - EIP#217439

  }

  if (Class == CHIPSET_FORM_SET_CLASS) {

// SAMPLE CODE BASED ON INTEL 945 Chipset
//    DraValue_A = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_A_DRA_0_1);
//    DraValue_B = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_B_DRA_0_1);
//    DrbValue_A = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_A_DRB_1);
//    DrbValue_B = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_B_DRB_1);


    //
    // Check for Interleaved Memory
    //
//    if ((DraValue_A != DraValue_B) || (DrbValue_A != DrbValue_B)){
      // Non-Interleaved
//      DrbValue_B -= *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_A_DRB_3);
//    }

//    if ((DraValue_A && V_RANK_UNPOPULATED_MASK) != 0){
      //
      // Memory is Populated in Slot 0
      //
    // *** PORTING NEEDED **
    // Read the appropriate chipset registers and update MemorySize with 
    // total memory present in the system in MB

//      MemorySize = (UINT64) (DrbValue_A * 32);
    MemorySize = 512;
      InitString(
                HiiHandle,
                STRING_TOKEN(STR_MEMORY_SIZE_SLOT0_VALUE),
                L"%4d MB (DDR2)",
                MemorySize
                );
      
//    }
//    if ((DraValue_B && V_RANK_UNPOPULATED_MASK) != 0){
      //
      // Memory is Populated in Slot 2
      //
    // *** PORTING NEEDED **
    // Read the appropriate chipset registers and update MemorySize with 
    // total memory present in the system in MB

//      MemorySize = (UINT64) (DrbValue_B * 32);
    MemorySize = 512;
      InitString(
                HiiHandle,
                STRING_TOKEN(STR_MEMORY_SIZE_SLOT2_VALUE),
                L"%4d MB (DDR2)",
                MemorySize
                );
      
//    }

// SAMPLE CODE BASED ON INTEL 945 Chipset
//    DraValue_A = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_A_DRA_2_3);
//    DraValue_B = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_B_DRA_2_3);
//    DrbValue_A = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_A_DRB_3);
//    DrbValue_B = *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_B_DRB_3);

//    DrbValue_A -= *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_A_DRB_1);
//    DrbValue_B -= *(UINT32*)(MCH_BASE_ADDRESS + R_CHANNEL_B_DRB_1);

//    if ((DraValue_A && V_RANK_UNPOPULATED_MASK) != 0){
      //
      // Memory is Populated in Slot 1
      //
    // *** PORTING NEEDED **
    // Read the appropriate chipset registers and update MemorySize with 
    // total memory present in the system in MB

//      MemorySize = (UINT64) (DrbValue_A * 32);
        MemorySize = 0;
      InitString(
                HiiHandle,
                STRING_TOKEN(STR_MEMORY_SIZE_SLOT1_VALUE),
                L"%4d MB (DDR2)",
                MemorySize
                );
      
//    }
//    if ((DraValue_B && V_RANK_UNPOPULATED_MASK) != 0){
      //
      // Memory is Populated in Slot 3
      //
    // *** PORTING NEEDED **
    // Read the appropriate chipset registers and update MemorySize with 
    // total memory present in the system in MB

//      MemorySize = (UINT64) (DrbValue_B * 32);
        MemorySize = 0;
      InitString(
                HiiHandle,
                STRING_TOKEN(STR_MEMORY_SIZE_SLOT3_VALUE),
                L"%4d MB (DDR2)",
                MemorySize
                );
      
//    }
  }
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
