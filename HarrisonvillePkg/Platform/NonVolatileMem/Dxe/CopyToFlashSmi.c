/******************************************************************************
SMI based Copy To Flash Sample Code 

Copyright 2011-2016 Intel Corporation All Rights Reserved.

The source code, information and material ("Material") contained herein
is owned by Intel Corporation or its suppliers or licensors, and title
to such Material remains with Intel Corporation or its suppliers or licensors.
The Material contains proprietary information of Intel or its suppliers and 
licensors. The Material is protected by worldwide copyright laws and treaty 
provisions. No part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed or disclosed in any way
without Intel's prior express written permission. No license under any patent,
copyright or other intellectual property rights in the Material is granted to
or conferred upon you, either expressly, by implication, inducement, estoppel
or otherwise. Any license under such intellectual property rights must be 
express and approved by Intel in writing.

Unless otherwise agreed by Intel in writing, you may not remove or alter this
notice or any other notice embedded in Materials by Intel or Intel’s suppliers
or licensors in any way.

******************************************************************************/

#include "CopyToFlashSmi.h"
#include <Library/BaseLib.h>
#include "NonVolatileMemHobServices.h"
#include <Library/HobLib.h>
#include <Guid/NonVolatileMemoryHob.h>
#include <Library/GpioLib.h>
#include <Library/C2fAhciLib.h>
#include <PlatformDefinitions.h>
#include <Library/PciLib.h>
#include <GpioPinsSklH.h>
#include <PchAccess.h>

#define SMI_C2F_MODE (3)

static UINTN                 param[8];
static NON_VOLATILE_MEM_HOB  *NonVolatileMemHob;
C2F_XFER_METHOD const        method = C2F_POLLED;

VOID c2f_data_invalidated_smi(void);
VOID c2f_save_completed_smi(void);


VOID
SmmIoWrite8 (
  IN  UINT16    Address,
  IN  UINT8     Data
  )
/*++

Routine Description:

  Do a one byte IO write

Arguments:

  Address - IO address to write
  Data    - Data to write

Returns: 

  None.

--*/
{
  ASSERT (gSmst);

  gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo, 
                    SMM_IO_UINT8, 
                    Address, 
                    1, 
                    &Data
                    );
}

VOID
SmmIoWrite32 (
  IN  UINT16    Address,
  IN  UINT32     Data
  )
/*++

Routine Description:

  Do a one byte IO write

Arguments:

  Address - IO address to write
  Data    - Data to write

Returns: 

  None.

--*/
{
  ASSERT (gSmst);

  gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo, 
                    SMM_IO_UINT32, 
                    Address, 
                    1, 
                    &Data
                    );
}


UINT8
SmmIoRead8 (
  IN  UINT16    Address
  )
/*++

Routine Description:

  Do a one byte IO read

Arguments:

  Address - IO address to read
  
Returns: 
  
  Data read

--*/  
{
  UINT8   Buffer;
  
  ASSERT (gSmst);

  gSmst->SmmIo.Io.Read (
                  &gSmst->SmmIo, 
                  SMM_IO_UINT8, 
                  Address, 
                  1, 
                  &Buffer
                  );
  return Buffer;
}

UINT32
SmmIoRead32(
IN  UINT16    Address
)
/*++

Routine Description:

Do a one byte IO read

Arguments:

Address - IO address to read

Returns:

Data read

--*/
{
  UINT32   Buffer;

  ASSERT(gSmst);

  gSmst->SmmIo.Io.Read(
    &gSmst->SmmIo,
    SMM_IO_UINT32,
    Address,
    1,
    &Buffer
    );
  return Buffer;
}

void shutdown_platform()
{
	UINT16      PMBASE;  
	
	PMBASE = PciRead16 (PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
                                         PCI_DEVICE_NUMBER_PCH_PMC,
                                         PCI_FUNCTION_NUMBER_PCH_PMC, 
                                         R_PCH_PMC_ACPI_BASE));
	PMBASE = PMBASE & (~0x1);

	SmmIoWrite32(PMBASE+0x30,(SmmIoRead32(PMBASE+0x30) & (~(UINT32)(1<<4)))); // Make sure this won't generate an SMI
    SmmIoWrite32(PMBASE+0x4,((7 << 10) | (1 << 13)));
}

EFI_STATUS
c2f_aptask(void * buffer)
{
       UINT8 * pointer8;
       UINT8 number;

//      volatile int a = 0;
//      while(a==0);

       pointer8 = (UINT8*)buffer;
       number = '0';
       number += *pointer8;


        while(1)
        {
                SmmIoWrite8(0x2f8,number);
                _mm_monitor(SIPI_MONITOR_LOC,0,0);
                _mm_mwait(0x0,0x51);


        }	

        return EFI_SUCCESS;
}


VOID c2f_data_invalidated_smi(void)
{
    SmmIoWrite8(CMOS_INDEX, C2F_CMOS_LOC);
    SmmIoWrite8(CMOS_DATA, C2F_DATA_EMPTY); // Indicate that a transfer completed
}

VOID c2f_save_completed_smi(void)
{
    SmmIoWrite8(CMOS_INDEX, C2F_CMOS_LOC);
    SmmIoWrite8(CMOS_DATA, C2F_DATA_VALID); // Indicate that a transfer completed
}


/**
  SMI Handler Copy To Flash Handler
  
  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]   SwRegisterContext Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer       A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.
  
  @retval EFI_UNSUPPORTED        The input SMI is not the expected SMI value.
  @retval EFI_INVALID_PARAMETER  The input capsule image is not the valid capsule. 
  @retval EFI_OUT_OF_RESOURCES   No enough memory to store the capsule image.
  @retval EFI_SUCCESS            Update capsule or Get update status successfully.
**/
EFI_STATUS
EFIAPI
CopyToFlashSMIHandler (
  IN EFI_HANDLE                        DispatchHandle,
  IN CONST EFI_SMM_GPI_REGISTER_CONTEXT *GpiRegisterContext,
  IN OUT VOID            				*CommBuffer,
  IN OUT UINTN                         *CommBufferSize
  )
{
  EFI_STATUS         Status;
//  UINT8              i;
  C2F_MEMORY_REGION  MemoryRange;
  C2F_STORAGE_DEVICE StorageDevice;
  UINTN const        AhciMmio = PCIEX_BASE_ADDRESS | PCI_LIB_ADDRESS( 0, 20, 0, 0 );
  UINT8 const        SataPort = 4;
  
  DEBUG((EFI_D_ERROR,"In CopyToFlashSMIHandler\n"));
  DEBUG((EFI_D_ERROR,"%d cores present\n", gSmst->NumberOfCpus));
  
/*  
  for (i = 1; i < gSmst->NumberOfCpus; i++) {
	  param[i]=i;
	  gSmst->SmmStartupThisAp (c2f_aptask, i, &param[i]);
  }
*/  
  DEBUG((EFI_D_ERROR,"Kicked off APs, spinning\n"));
  MemoryRange.LowStart = NonVolatileMemHob->C2fLowStart;
  MemoryRange.LowSize = NonVolatileMemHob->C2fLowLength;
  MemoryRange.HighStart = NonVolatileMemHob->C2fHighStart;
  MemoryRange.HighSize = NonVolatileMemHob->C2fHighLength;
  

  StorageDevice.AhciMmio = (UINT32)AhciMmio;
  StorageDevice.SataPort = SataPort;
  StorageDevice.LbaLoc = NonVolatileMemHob->C2fLbaLocMethod;
  StorageDevice.LBA = NonVolatileMemHob->C2fLba;
  StorageDevice.Method = method;
  StorageDevice.Vector = VECTOR_NUM;
  Status = c2f_entry(&MemoryRange, WRITE, &StorageDevice);
  if(Status != EFI_SUCCESS) {
	  DEBUG ((EFI_D_ERROR, "C2F failed \n"));
	  c2f_data_invalidated_smi();
  } else {
    c2f_save_completed_smi();
	  DEBUG ((DEBUG_INFO, "C2F save completed\n"));
  }

  shutdown_platform();
   
  return EFI_SUCCESS;
}


/**
  SMI Copy To Flash point. It initializes memory and services,
  and registers two SMI handler for capsule update.

  @param  ImageHandle    The firmware allocated handle for the EFI image.
  @param  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    The entry point is executed successfully.
  @retval Other          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
CopyToFlashEntry (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_HANDLE                     GpiHandle;
  EFI_SMM_GPI_DISPATCH2_PROTOCOL *GpiDispatch;
  EFI_SMM_GPI_REGISTER_CONTEXT   GpiContext;
  UINT32 const                   AdrGpio = 96; //GPIO_96

  GetNonVolatileMemHob(&NonVolatileMemHob);
  if (SMI_C2F_MODE != NonVolatileMemHob->NonVolMemMode) {
      return EFI_SUCCESS;
  }

  Status = gSmst->SmmLocateProtocol (&gEfiSmmGpiDispatch2ProtocolGuid, NULL, &GpiDispatch);
  ASSERT_EFI_ERROR (Status);
  
  DEBUG((EFI_D_ERROR, "About to Hook SMI\n"));
  
  //
  // Register SMI handler for Update and GetInfo.
  //

  GpiContext.GpiNum = AdrGpio;
  Status = GpiDispatch->Register (
                                  GpiDispatch,
                                  CopyToFlashSMIHandler,
                                  &GpiContext,
                                  &GpiHandle
                                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG((EFI_D_ERROR, "Hooked SMI\n"));
    
  
  return EFI_SUCCESS;
}
