//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file                                                          

  Header file for cpu only reset.                                
                                                                   
  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished 
  under a license and may only be used or copied in accordance     
  with the terms of the license. Except as permitted by such       
  license, no part of this software or documentation may be        
  reproduced, stored in a retrieval system, or transmitted in any  
  form or by any means without the express written consent of      
  Intel Corporation.                                               
                                                                   
  Module Name:  CpuOnlyReset.h                                  
                                                                   
**/
#ifndef _CPU_ONLY_RESET_H_
#define _CPU_ONLY_RESET_H_

#include "MpCommon.h"

#define  CPU_ONLY_RESET_INDICATOR   SIGNATURE_32('C','R','S','T')
#define  CPU_ONLY_RESET_TEMP_STACK_SIZE       128
#pragma pack(1)
//
// This structure describe the mininal information SEC phase need  to deal with 
// CPU only reset. 
//
typedef struct _CPU_ONLY_RESET_INFO_ {
  UINT32                     CpuOnlyResetIndicator;  // a flag to indicate the reset is CPU only reset
  UINT32                     EntryPoint;           // entry pointer SEC should jump to
  UINT32                     NewStack;             // a temporary stack for the entry point
  UINT32                     Checksum;             // the checksum to this data structure
}CPU_ONLY_RESET_INFO;
#pragma pack()

#pragma pack(1)
//
// This data structure defined the trampolean information to resume to 64-bit 
// code from CPU only reset. And this data structure will works as stack, so please
// take care the fields' sequence and alignment when modification  
// This structure is specific for 64-bit long mode
//
typedef struct _CPU_ONLY_RESET_TRAMPOLINE_INFO_ {
  CPU_ONLY_RESET_INFO        CpuOnlyResetInfo;
  //
  // ASSUMPTION: 
  // The gdt DXE drivers use, page table and CPU driver itself should 
  // be located bellow 4G memory. in feature, when this stuff allows to locate 
  // above 4G, the temporary page table, entry point and gdt
  // need be implemented below 4G.
  //
  UINT16                     TemporaryGdtLimit;
  UINT32                     TemporaryGdtBase;
  // to make sure following field aligned with 4 bytes                   
  UINT16                     Reserved0;
  
  UINT32                     TemporaryCR3;   
  //
  // prepare the parameter for calling InternalX86EnablePaging64 according to
  // C calling convention to switch to long mode. 
  //
  //  InternalX86EnablePaging64 (
  //   IN      UINT16                    Cs,
  //   IN      UINT64                    EntryPoint,
  //   IN      UINT64                    Context1,  OPTIONAL
  //   IN      UINT64                    Context2,  OPTIONAL
  //   IN      UINT64                    NewStack
  //   );
  //
  // 4 bytes reserve for return address after call into  InternalX86EnablePaging64
  // 
  UINT32                    Reserved1;  
  UINT16                    TemporaryCS;
  UINT16                    Reserved2; // for alignment reason
  UINT64                    CpuOnlyResetEntryPoint;
  UINT64                    Context1;
  UINT64                    Context2;
  UINT64                    NewStack;
  UINT8                     TemporaryStack[CPU_ONLY_RESET_TEMP_STACK_SIZE];
} CPU_ONLY_RESET_TRAMPOLINE_INFO;

#pragma pack()

#define  CPU_ONLY_RESET_TRAMPOLINE_INFO_SIZE   sizeof(CPU_ONLY_RESET_TRAMPOLINE_INFO)  

//
// When CPU only reset occurs, we need save CPU_ONLY_RESET_TRAMPOLINE_INFO
// to a well-known address which should be availabe and under 4G memory.
// here we defines it as zero
//
#define  CPU_ONLY_RESET_TRAMPOLINE_INFO_BASE_ADDRESS      0x400

//
// the structure define is generic. it will be the same one no matter 
// for 32-bit mode or 64-bit mode
//
typedef struct _CPU_ONLY_RESET_CONTEXT_BUFFER_ {
  //
  // System register 
  //
  IA32_DESCRIPTOR           GdtDesc; 
  IA32_DESCRIPTOR           IdtDesc;
  UINT16                    Ldtr;
  UINT16                    Es;
  UINT16                    Cs;
  UINT16                    Ss;
  UINT16                    Ds;
  UINT16                    Fs;
  UINT16                    Gs;
  //
  //
  UINTN                    Cr0;
  UINTN                    Cr3;
  UINTN                    Cr4;
  UINTN                    Tr;
  //
  // debug register
  //
  UINTN                    Dr0;
  UINTN                    Dr1;
  UINTN                    Dr2;
  UINTN                    Dr3;
  UINTN                    Dr6;
  UINTN                    Dr7;
  //
  // General register
  //
  BASE_LIBRARY_JUMP_BUFFER  JumpContext;     
  //
  // When CPU only reset occurs, we need save CPU_ONLY_RESET_TRAMPOLINE_INFO
  // to a well-known address which should be under 4G memory(say address 0)
  // So this field will save the original content on that address and 
  // restore it after resume
  //
  UINT8                     CpuOnlyResetTrampolineInfo[CPU_ONLY_RESET_TRAMPOLINE_INFO_SIZE];        
        
}CPU_ONLY_RESET_CONTEXT_BUFFER;

/**
  Initiate CPU only reset.
  
  This function will save CPU context, call SetJump to mark the resume pointer
  and program MCH to trigger CPU only reset.

**/
VOID
InitiateCpuOnlyReset(
  VOID
  );
/**
  This function is written in Long mode instructions and build by 64-bit 
  assembler, BUT running in 32-bit protected mode
  This function is the real entry point after CPU only reset. SEC will dire

  @param Context1   The first context save the info.
  @param Context2   The second context save the info.
**/
VOID
EFIAPI
CpuOnlyResetResumeEntryWrapper (
  VOID                 *Context1,
  VOID                 *Context2
  );
#endif // _CPU_ONLY_RESET_H_
