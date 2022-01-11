/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
--*/
/**@file
  Memory Initialization PPI used in EFI PEI interface
  
Copyright (c)  2010 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef __INTEL_RC_MEM_INIT_H__
#define __INTEL_RC_MEM_INIT_H__

#include "CommonDefinitions.h"
#include "MchRegs.h"
#include "DetectDimms.h"


#define PEI_RC_MEMORY_INIT_PPI_GUID \
  {0xa55d6970, 0x1306, 0x440c, {0x8c, 0x72, 0x8f, 0x51, 0xfa, 0xfb, 0x29, 0x26}}

//                                                        
// MRC Data Structure                                     
//                                                        
typedef struct {
  MRC_PARAMETER_FRAME        MrcPara;
} MRC_DATA_STRUCTURE;             /**Memory Parameters for MRC */


//                                                        
// PPI Function Declarations                              
//                                                        
typedef                                                   
EFI_STATUS                                                
(EFIAPI *PEI_RC_MEMORY_INIT) (                                 
  IN OUT    MRC_DATA_STRUCTURE                           *MRCDATA
  );                                                     
                                                         
typedef struct _PEI_RC_MEMORY_INIT_PPI {           
  PEI_RC_MEMORY_INIT                               MrcStart;
}PEI_RC_MEMORY_INIT_PPI;

extern EFI_GUID gRCMemoryInitPpiGuid;

#endif
