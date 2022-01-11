/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
--*/
/**@file
  PCIE Initialization PPI used in EFI PEI interface
  
Copyright (c)  2010 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/
#include "PchRegs.h"
#ifndef __INTEL_PROC_PCIE_INIT_H__
#define __INTEL_PROC_PCIE_INIT_H__

#define PROC_PCIE_PLATFORM_POLICY_1          1

#ifndef MAX_PCI_EXPRESS_ROOT_PORTS
#define MAX_PCI_EXPRESS_ROOT_PORTS      4
#endif

//                                                        
// Processor PCIE policy
//                                                        
typedef struct {
  UINT32  Revision;
  
  UINT16  Boardid;
  UINT16  GpioBase;
  UINT32  PcieBaseAddr;
  /* Root Complex 0 */
  UINT8   MaxPayloadRTF;
  
  UINT32  Rerserved[18];
} PROC_PCIE_PLATFORM_POLICY_STRUCTURE;

//                                                        
// PPI Function Declarations                              
//                                                        
typedef                                                   
EFI_STATUS                                                
(EFIAPI *PEI_PROC_PCIE_INIT_BEFOREMEM) (                                 
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PROC_PCIE_PLATFORM_POLICY_STRUCTURE                           *PciePlatformPolicy
  );                                                     

typedef                                                   
EFI_STATUS                                                
(EFIAPI *PEI_PROC_PCIE_INIT_AFTERMEM) (                                 
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PROC_PCIE_PLATFORM_POLICY_STRUCTURE                           *PciePlatformPolicy
  );                                                     
                                                         
typedef struct _PEI_PROC_PCIE_INIT_PPI {           
  PEI_PROC_PCIE_INIT_BEFOREMEM                               PcieInitBeforeMem;
  PEI_PROC_PCIE_INIT_AFTERMEM                                PcieInitAfterMem;
}PEI_PROC_PCIE_INIT_PPI;

extern EFI_GUID gProcPcieInitPpiGuid;

#endif
