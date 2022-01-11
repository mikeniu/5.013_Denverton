/** @file
  CpuTraining.c
  Contains training infrastructure for using the CPU core to train 
  memory rather than CPGC
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "Mmrc.h"

#if !CPGC_API
 
#ifndef MINIBIOS
#if !defined SIM  && !defined JTAG
 
/**
  64-bit Shift Left

  @param[in]   Value      Value to be left shifted
  @param[in]   Shift      Number of Shifts to be done to the 
                          left
  @retval      Value      Shifted
**/  
UINT64
Shl64 (
  IN        UINT64        Value,
  IN        UINT8         Shift
  )
{
#ifndef _MSC_EXTENSIONS
  return (Value << Shift);
#else
  _asm {
    mov edx, dword Ptr Value[4]     // Load Eax,Edx with the 64-bit Value.
    mov eax, dword Ptr Value
    mov cl, Shift                   // Load CL with the shift amount.

    cmp cl, 32                      // If the shift is more then 32, then copy edx into eax.
    jb  less_32                     // and zero out eax, then shift edx by x-32, otherwise shift by x.

    mov edx, eax                    // This is the case shift>32, then copy edx into eax.
    xor eax, eax                    // This is the case shift>32, then zero out eax.
less_32:
    shld  edx, eax, cl              // Shift eax/edx into edx by cl which is modulo 32.
    shl   eax, cl                   // Shift eax itself.
  }
#endif
}

/**
  64-bit Shift Right

  @param[in]   Value      Value to be left shifted
  @param[in]   Shift      Number of Shifts to be done to the 
                          right
  @return      Value      Shifted      
**/
UINT64
Shr64 (
  IN        UINT64        Value,
  IN        UINT8         Shift
  )
{
#ifndef _MSC_EXTENSIONS
  return (Value >> Shift);
#else
  _asm {
    mov edx, dword Ptr Value[4]    // Load Eax,Edx with the 64-bit Value.
    mov eax, dword Ptr Value
    mov cl, Shift                  // Load CL with the shift amount.

    cmp cl, 32                     // If the shift is more then 32, then copy edx into eax.
    jb less_32                     // and zero out eax, then shift edx by x-32, otherwise shift by x.

    mov   eax, edx                 // This is the case shift>32, then copy edx into eax.
    xor   edx, edx                 // This is the case shift>32, then zero out eax.
less_32:
    shrd  eax, edx, cl             // Shift eax/edx into edx by cl which is modulo 32.
    shr   edx, cl                  // Shift eax itself.
  }
#endif
}
#endif // SIM
#endif // MINIBIOS

MMRC_STATUS
FillGoldenBuffer (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        *PatternIndex,
  IN        UINT32        BufferAddress
  )
{
  UINT16        BufferIndex             = 0;
  UINT32        VictimPatterns[]        = VICTIMPATTERNS;
  UINT32        AggressorPatterns[]     = AGGRESSORPATTERNS;
  UINT8         FlybyShift[MAX_STROBES] = FLYBY_SHIFT;

  while ((BufferIndex < 1024) & (*PatternIndex < NUMBERPATTERNS)) {
    GeneratePattern (MrcData, VictimPatterns[*PatternIndex / 17], 
                     AggressorPatterns[*PatternIndex / 17], 
                     FlybyShift, 
                     (*PatternIndex) % 17, 
                     16, 
                     ((*PatternIndex) % 17 == 17),
                     (VOID *) (BufferAddress + BufferIndex)
                     );
    *PatternIndex    += 1;
    BufferIndex += PATTERN_SIZE;
  }
  // 
  // To get to this point, either the buffer is full or PatternIndex has reached the max.
  //
  while (BufferIndex < 1024) {
    GeneratePattern (MrcData, VictimPatterns[0], AggressorPatterns[0], FlybyShift, 0, 16, 0, (VOID *) (BufferAddress + BufferIndex) );
    BufferIndex += PATTERN_SIZE;
  }

  return MMRC_SUCCESS;
}

MMRC_STATUS
GeneratePattern (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        VictimPattern,
  IN        UINT32        AggressorPattern,
  IN        UINT8         ByteShift[],
  IN        UINT8         BitShift,
  IN        UINT8         VictimRepeat,
  IN        UINT8         EvenOddFlag,
  IN        VOID          *BufferPv
  )
{
  UINT8  Strobe;
  UINT8  BitPosition;
  UINT8  TimeSample;
  UINT32 VictimPatternStrobe[MAX_STROBES];
  UINT32 AggressorPatternStrobe[MAX_STROBES];
  UINT64 VictimMask;
  UINT64 AggressorMask;
  UINT64 VictimBits;
  UINT64 AggressorBits;
  UINT64 Bits;
  // Denverton AptioV override Start - EIP#265689
  UINT64  Temp = 0; 
  UINT8  *Temp8 = (UINT8 *) &Temp;
  UINT16 *Temp16 = (UINT16 *)&Temp;
  UINT32 *Temp32 = (UINT32 *)&Temp;
  UINT64 *Temp64 = (UINT64 *)&Temp;
  // Denverton AptioV override End - EIP#265689
  // 
  // Step 1: Using the byteShift, VictimPattern, and AggressorPattern input parameters, create the victim/aggressor
  // patterns that will be used on each BL.  Thereby creating victimPatternBL[] and aggessorPatternBL[].
  //
  for (Strobe = 0; Strobe < MAX_STROBES; Strobe++) {
    VictimPatternStrobe[Strobe] = VictimPattern << ByteShift[Strobe];
    AggressorPatternStrobe[Strobe] = AggressorPattern << ByteShift[Strobe];
  }
  // 
  // Step 2:  Create the VictimMask and AggressorMask which will provide the bit fields which should have the victim
  // and aggressor Bits accordingly, this should use the victimShift.
  //
  VictimMask = 0;
  Bits = 1;
  for (BitPosition = 0; BitPosition < MAX_STROBES * 8; BitPosition += VictimRepeat) {
    VictimMask |= (UINT64) (Shl64 (Bits, BitPosition) );
  }
  AggressorMask = ~VictimMask;
  // 
  // Step 3: Shift the aggressor and victim masks by the BitShift input Parameters.
  //
  AggressorMask = (Shl64 (AggressorMask, BitShift) ) | (Shr64 (AggressorMask, (MAX_STROBES * 8 - BitShift) ) );
  VictimMask = (Shl64 (VictimMask, BitShift) ) | (Shr64 (VictimMask, (MAX_STROBES * 8 - BitShift) ) );

  for (TimeSample = 0; TimeSample < 32; TimeSample++) {
    VictimBits = 0;
    AggressorBits = 0;
    for (Strobe = 0; Strobe < MAX_STROBES; Strobe ++) {
      if (((VictimPatternStrobe[Strobe] >> TimeSample) & 1) == 1) {
        VictimBits |= (UINT64) (0xff << (8 * Strobe) );
      }
      if (EvenOddFlag == ODD_MODE) {
        if (((AggressorPatternStrobe[Strobe] >> TimeSample) & 1) == 1) {
          AggressorBits |= 0xff << (8 * Strobe);
        }
      }
    }

    if (EvenOddFlag == ODD_MODE) {
      Bits = (VictimBits & VictimMask) | (AggressorBits & AggressorMask);
    } else {
      Bits = VictimBits;
    }

    switch (MAX_STROBES) {
    case 1:
      *Temp8 = (UINT8) Bits;
      Temp8++;
      BufferPv = (VOID *) Temp8;
      break;
    case 2:
      *Temp16 = (UINT16) Bits;
      Temp16++;
      BufferPv = (VOID *) Temp16;
      break;
    case 4:
      *Temp32 = (UINT32) Bits;
      Temp32++;
      BufferPv = (VOID *) Temp32;
      break;
    default:
      *Temp64 = (UINT64) Bits;
      Temp64++;
      BufferPv = (VOID *) Temp64;
    }
  } // End of TimeSample;

  return MMRC_SUCCESS;
}

 
MMRC_STATUS 
CompareGoldenWithDRAMPatterns (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        GoldenPatternAddress,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT16        *CompareResults
  )
{
  UINT32    TargetAddress;
  UINT8    *Ptr;
  UINT8     Strobe;
  UINT32    TempValue;

  UINT64  AllOnes[4] = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};

  Ptr = (UINT8 *) AllOnes;
  Ptr = (UINT8 *)((((UINT32) Ptr) + 15) & 0xfffffff0);
  // 
  // Get the correct Address range for the given channel and rank.
  //
  TargetAddress = GetAddress (MrcData, Channel, Rank);
  // 
  // Enable a 16K buffer at the target Address.
  //
  Enable16KMTRR (MrcData, TargetAddress, 0x20E);

  MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
  // 
  // Transfer the memory buffer from the golden area to the test area.
  //
  TransferMemory (MrcData, (UINT8 *) TargetAddress, (UINT8 *) GoldenPatternAddress, 16, 1, (UINT32 *) Ptr);

  MmrcExecuteTask (MrcData, PERFORM_FIFO_RESET, NO_PRINT, Channel);
  // 
  // Fetch the target area back into cache.
  //
  ReadRamIntoCache (MrcData, TargetAddress, 32);
  // 
  // Compare the readback Data with the golden Data for the given channel and rank.
  //
  CompareMemory (MrcData, (UINT8 *) GoldenPatternAddress, (UINT8 *) TargetAddress, 16, (UINT32 *) Ptr, 1, CompareResults);

  return MMRC_SUCCESS;
}

 
MMRC_STATUS
TransferMemory (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *DstAddr,
  IN        UINT8         *SrcAddr,
  IN        UINT32        CacheLines,
  IN        UINT8         InvertFlag,
  IN        UINT32        *AllOnes
  )
{
#ifndef _MSC_EXTENSIONS
  asm (
    "mov %0,%%eax;"
    "mov %1,%%ebx;"
    "mov %2,%%ecx;"
    "mov %3, %%dl;"
    "mov %4,%%esi;"
    "TransferMemoryLoop:"
    "movdqa 0x00(%%eax),%%xmm0;"
    "movdqa 0x10(%%eax),%%xmm1;"
    "movdqa 0x20(%%eax),%%xmm2;"
    "movdqa 0x30(%%eax),%%xmm3;"
    "movdqa 0x40(%%eax),%%xmm4;"
    "movdqa 0x50(%%eax),%%xmm5;"
    "movdqa 0x60(%%eax),%%xmm6;"
    "movdqa 0x70(%%eax),%%xmm7;"
    "movntdq %%xmm0, 0x00(%%ebx);"
    "movntdq %%xmm1, 0x10(%%ebx);"
    "movntdq %%xmm2, 0x20(%%ebx);"
    "movntdq %%xmm3, 0x30(%%ebx);"
    "movntdq %%xmm4, 0x40(%%ebx);"
    "movntdq %%xmm5, 0x50(%%ebx);"
    "movntdq %%xmm6, 0x60(%%ebx);"
    "movntdq %%xmm7, 0x70(%%ebx);"
    "add $0x80,%%ebx;"
    "add $0x80,%%eax;"
    "cmp $0x0, %%dl;"
    "je SKIP_INVERT_1;"
    "pxor 0x00(%%esi),%%xmm0;"
    "pxor 0x00(%%esi),%%xmm1;"
    "pxor 0x00(%%esi),%%xmm2;"
    "pxor 0x00(%%esi),%%xmm3;"
    "pxor 0x00(%%esi),%%xmm4;"
    "pxor 0x00(%%esi),%%xmm5;"
    "pxor 0x00(%%esi),%%xmm6;"
    "pxor 0x00(%%esi),%%xmm7;"
    "movntdq %%xmm0,0x00(%%ebx);"
    "movntdq %%xmm1,0x10(%%ebx);"
    "movntdq %%xmm2,0x20(%%ebx);"
    "movntdq %%xmm3,0x30(%%ebx);"
    "movntdq %%xmm4,0x40(%%ebx);"
    "movntdq %%xmm5,0x50(%%ebx);"
    "movntdq %%xmm6,0x60(%%ebx);"
    "movntdq %%xmm7,0x70(%%ebx);"
    "add $0x80,%%ebx;"
    "SKIP_INVERT_1:"
    "sub $0x2,%%ecx;"
    "cmp $0x0,%%ecx;"
    "jnz TransferMemoryLoop;"
    :/* no outputs */
  :"m" (SrcAddr), "m" (DstAddr), "m" (CacheLines), "m" (InvertFlag), "m" (AllOnes)
    :"%eax", "%ebx", "%esi", "%edx", "%ecx"
    );
#else
  __asm {
    mov eax, SrcAddr;
    mov ebx, DstAddr;
    mov ecx, CacheLines;
    mov dl, InvertFlag;
    mov esi, AllOnes;
TransferMemoryLoop:
    movdqa xmm0, [eax+0x00];
    movdqa xmm1, [eax+0x10];
    movdqa xmm2, [eax+0x20];
    movdqa xmm3, [eax+0x30];
    movdqa xmm4, [eax+0x40];
    movdqa xmm5, [eax+0x50];
    movdqa xmm6, [eax+0x60];
    movdqa xmm7, [eax+0x70];
    movntdq [ebx+0x00], xmm0;
    movntdq [ebx+0x10], xmm1;
    movntdq [ebx+0x20], xmm2;
    movntdq [ebx+0x30], xmm3;
    movntdq [ebx+0x40], xmm4;
    movntdq [ebx+0x50], xmm5;
    movntdq [ebx+0x60], xmm6;
    movntdq [ebx+0x70], xmm7;
    add ebx, 0x80;
    add eax, 0x80;
    cmp dl, 0;
    je SKIP_INVERT;
    pxor xmm0, [esi];
    pxor xmm1, [esi];
    pxor xmm2, [esi];
    pxor xmm3, [esi];
    pxor xmm4, [esi];
    pxor xmm5, [esi];
    pxor xmm6, [esi];
    pxor xmm7, [esi];
    movntdq [ebx+0x00], xmm0;
    movntdq [ebx+0x10], xmm1;
    movntdq [ebx+0x20], xmm2;
    movntdq [ebx+0x30], xmm3;
    movntdq [ebx+0x40], xmm4;
    movntdq [ebx+0x50], xmm5;
    movntdq [ebx+0x60], xmm6;
    movntdq [ebx+0x70], xmm7;
    add ebx, 0x80;
SKIP_INVERT:
    sub ecx, 2;
    cmp ecx, 0;
    jnz TransferMemoryLoop;
  }
#endif // VS_ENV

  return MMRC_SUCCESS;
}


MMRC_STATUS
CompareMemory (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *GoldenAddress,
  IN        UINT8         *TestAddr,
  IN        UINT32        CacheLines,
  IN        UINT32        *AllOnes,
  IN        UINT8         InvertFlag,
  IN        UINT16        *Results
  )
{
  UINT16  CompareResults;
#ifndef _MSC_EXTENSIONS
  asm (
    "mov %1,%%eax;"
    "mov %2,%%ebx;"
    "mov %3,%%esi;"
    "mov %4,%%edi;"
    "mov $0x0000FFFF,%%edx;"
    "CompareMemLoop:"
    "movdqa 0x00(%%eax),%%xmm0;"        // Copy golden cacheline into xmm0:7
    "movdqa 0x10(%%eax),%%xmm1;"
    "movdqa 0x20(%%eax),%%xmm2;"
    "movdqa 0x30(%%eax),%%xmm3;"
    "movdqa 0x40(%%eax),%%xmm4;"
    "movdqa 0x50(%%eax),%%xmm5;"
    "movdqa 0x60(%%eax),%%xmm6;"
    "movdqa 0x70(%%eax),%%xmm7;"
    "pcmpeqb 0x00(%%ebx),%%xmm0;"
    "pmovmskb %%xmm0,%%ecx;"
    "and %%ecx,%%edx;"            // Compare each octword to the target Address, accumulating results into EDX.
    "pcmpeqb 0x10(%%ebx),%%xmm1;"
    "pmovmskb %%xmm1,%%ecx;"
    "and %%ecx,%%edx;"
    "pcmpeqb 0x20(%%ebx),%%xmm2;"
    "pmovmskb %%xmm2,%%ecx;"
    "and %%ecx,%%edx;"
    "pcmpeqb 0x30(%%ebx),%%xmm3;"
    "pmovmskb %%xmm3,%%ecx;"
    "and %%ecx,%%edx;"
    "pcmpeqb 0x40(%%ebx),%%xmm4;"
    "pmovmskb %%xmm4,%%ecx;"
    "and %%ecx,%%edx;"            // Compare each octword to the target Address, accumulating results into EDX.
    "pcmpeqb 0x50(%%ebx),%%xmm5;"
    "pmovmskb %%xmm5,%%ecx;"
    "and %%ecx,%%edx;"            // Compare each octword to the target Address, accumulating results into EDX.
    "pcmpeqb 0x60(%%ebx),%%xmm6;"
    "pmovmskb %%xmm6,%%ecx;"
    "and %%ecx,%%edx;"            // Compare each octword to the target Address, accumulating results into EDX.
    "pcmpeqb 0x70(%%ebx),%%xmm7;"
    "pmovmskb %%xmm7,%%ecx;"
    "and %%ecx,%%edx;"            // Compare each octword to the target Address, accumulating results into EDX.
    "mov %5,%%cl;"
    "cmp $1,%%cl;"
    "jne SKIP_INVERT;"
    "add $0x80,%%ebx;"
    "movdqa 0x00(%%eax),%%xmm0;"        // Copy golden cacheline into xmm0:7
    "movdqa 0x10(%%eax),%%xmm1;"
    "movdqa 0x20(%%eax),%%xmm2;"
    "movdqa 0x30(%%eax),%%xmm3;"
    "movdqa 0x40(%%eax),%%xmm4;"
    "movdqa 0x50(%%eax),%%xmm5;"
    "movdqa 0x60(%%eax),%%xmm6;"
    "movdqa 0x70(%%eax),%%xmm7;"
    "pxor   (%%edi),%%xmm0;"
    "pxor   (%%edi),%%xmm1;"
    "pxor   (%%edi),%%xmm2;"
    "pxor   (%%edi),%%xmm3;"
    "pxor   (%%edi),%%xmm4;"
    "pxor   (%%edi),%%xmm5;"
    "pxor   (%%edi),%%xmm6;"
    "pxor   (%%edi),%%xmm7;"
    "pcmpeqb 0x00(%%ebx),%%xmm0;"
    "pmovmskb %%xmm0,%%ecx;"
    "and %%ecx,%%edx;"            // Compare each octword to the target Address, accumulating results into EDX.
    "pcmpeqb 0x10(%%ebx),%%xmm1;"
    "pmovmskb %%xmm1,%%ecx;"
    "and %%ecx,%%edx;"
    "pcmpeqb 0x20(%%ebx),%%xmm2;"
    "pmovmskb %%xmm2,%%ecx;"
    "and %%ecx,%%edx;"
    "pcmpeqb 0x30(%%ebx),%%xmm3;"
    "pmovmskb %%xmm3,%%ecx;"
    "and %%ecx,%%edx;"
    "pcmpeqb 0x40(%%ebx),%%xmm4;"
    "pmovmskb %%xmm4,%%ecx;"
    "and %%ecx,%%edx;"          // Compare each octword to the target Address, accumulating results into EDX.
    "pcmpeqb 0x50(%%ebx),%%xmm5;"
    "pmovmskb %%xmm5,%%ecx;"
    "and %%ecx,%%edx;"          // Compare each octword to the target Address, accumulating results into EDX.
    "pcmpeqb 0x60(%%ebx),%%xmm6;"
    "pmovmskb %%xmm6,%%ecx;"
    "and %%ecx,%%edx;"          // Compare each octword to the target Address, accumulating results into EDX.
    "pcmpeqb 0x70(%%ebx),%%xmm7;"
    "pmovmskb %%xmm7,%%ecx;"
    "and %%ecx,%%edx;"          // Compare each octword to the target Address, accumulating results into EDX.
    "SKIP_INVERT:"
    "cmp $0x00000000,%%edx;"      // If all bytelanes are failing, immediately exit the loop as no more test is required.
    "je CompareMemoryDone;"
    "add $0x80,%%ebx;"
    "add $0x80,%%eax;"
    "sub $0x02,%%esi;"
    "cmp $0x00,%%esi;"
    "jne CompareMemLoop;"
    "CompareMemoryDone:"
    "mov %%dx,%0;"
    :"=m" (CompareResults)
    :"m" (GoldenAddress), "m" (TestAddr), "m" (CacheLines), "m" (AllOnes), "m" (InvertFlag)
    :"%eax", "%ebx", "%esi", "%edi", "%ecx", "%edx"
    );
#else
  __asm {
    mov eax, GoldenAddress;
    mov ebx, TestAddr;
    mov esi, CacheLines;
    mov edi, AllOnes;
    mov edx, 0x0000FFFF;          // Initialize EDX=results criteria, default=0xffff
CompareMemoryLoop:
    movdqa xmm0, [eax+0x00];        // Copy golden cacheline into xmm0:7
    movdqa xmm1, [eax+0x10];
    movdqa xmm2, [eax+0x20];
    movdqa xmm3, [eax+0x30];
    movdqa xmm4, [eax+0x40];
    movdqa xmm5, [eax+0x50];
    movdqa xmm6, [eax+0x60];
    movdqa xmm7, [eax+0x70];
    pcmpeqb xmm0, [ebx+0x00];
    pmovmskb ecx, xmm0;
    and edx, ecx;           // Compare each octword to the target Address, accumulating results into EDX.
    pcmpeqb xmm1, [ebx+0x10];
    pmovmskb ecx, xmm1;
    and edx, ecx;
    pcmpeqb xmm2, [ebx+0x20];
    pmovmskb ecx, xmm2;
    and edx, ecx;
    pcmpeqb xmm3, [ebx+0x30];
    pmovmskb ecx, xmm3;
    and edx, ecx;
    pcmpeqb xmm4, [ebx+0x40];
    pmovmskb ecx, xmm4;
    and edx, ecx;           // Compare each octword to the target Address, accumulating results into EDX.
    pcmpeqb xmm5, [ebx+0x50];
    pmovmskb ecx, xmm5;
    and edx, ecx;           // Compare each octword to the target Address, accumulating results into EDX.
    pcmpeqb xmm6, [ebx+0x60];
    pmovmskb ecx, xmm6;
    and edx, ecx;           // Compare each octword to the target Address, accumulating results into EDX.
    pcmpeqb xmm7, [ebx+0x70];
    pmovmskb ecx, xmm7;
    and edx, ecx;           // Compare each octword to the target Address, accumulating results into EDX.
    mov cl, InvertFlag;
    cmp cl, 1
      jne SKIP_INVERT
      add ebx, 0x80;
    movdqa xmm0, [eax+0x00];        // Copy golden cacheline into xmm0:7
    movdqa xmm1, [eax+0x10];
    movdqa xmm2, [eax+0x20];
    movdqa xmm3, [eax+0x30];
    movdqa xmm4, [eax+0x40];
    movdqa xmm5, [eax+0x50];
    movdqa xmm6, [eax+0x60];
    movdqa xmm7, [eax+0x70];
    pxor   xmm0, [edi];
    pxor   xmm1, [edi];
    pxor   xmm2, [edi];
    pxor   xmm3, [edi];
    pxor   xmm4, [edi];
    pxor   xmm5, [edi];
    pxor   xmm6, [edi];
    pxor   xmm7, [edi];
    pcmpeqb xmm0, [ebx+0x00];
    pmovmskb ecx, xmm0;
    and edx, ecx;           // Compare each octword to the target Address, accumulating results into EDX.
    pcmpeqb xmm1, [ebx+0x10];
    pmovmskb ecx, xmm1;
    and edx, ecx;
    pcmpeqb xmm2, [ebx+0x20];
    pmovmskb ecx, xmm2;
    and edx, ecx;
    pcmpeqb xmm3, [ebx+0x30];
    pmovmskb ecx, xmm3;
    and edx, ecx;
    pcmpeqb xmm4, [ebx+0x40];
    pmovmskb ecx, xmm4;
    and edx, ecx;           // Compare each octword to the target Address, accumulating results into EDX.
    pcmpeqb xmm5, [ebx+0x50];
    pmovmskb ecx, xmm5;
    and edx, ecx;           // Compare each octword to the target Address, accumulating results into EDX.
    pcmpeqb xmm6, [ebx+0x60];
    pmovmskb ecx, xmm6;
    and edx, ecx;           // Compare each octword to the target Address, accumulating results into EDX.
    pcmpeqb xmm7, [ebx+0x70];
    pmovmskb ecx, xmm7;
    and edx, ecx;           // Compare each octword to the target Address, accumulating results into EDX.

SKIP_INVERT:
    cmp edx, 0x00000000;          // If all bytelanes are failing, immediately exit the loop as no more test is required.
    je CompareMemoryDone;
    add ebx, 0x80;
    add eax, 0x80;
    sub esi, 0x02;
    cmp esi, 0x00;
    jne CompareMemoryLoop;
CompareMemoryDone:
    mov CompareResults, dx;
  };
#endif // _MSC_EXTENSIONS
  *Results = CompareResults;

  return MMRC_SUCCESS;
}


MMRC_STATUS
DisableMTRR (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        Msr
  )
{
#ifndef _MSC_EXTENSIONS
  asm ("mov $0xFFFFC000,%%eax;"
    "mov $0x00000000,%%edx;"
    "mov $0x0000020E,%%ecx;"
    "wrmsr;"
    :/* no outputs */
  :/* no inputs */
  :"%eax", "%ecx", "%edx"
    );
#else
  _asm mov eax, 0xFFFFC000;
  _asm mov edx, 0x00000000;
  _asm mov ecx, Msr;
  _asm wrmsr;
#endif // VS_ENV

  return MMRC_SUCCESS;
}
 
MMRC_STATUS
FillCacheArray (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *Ptr,
  IN        UINT8         Data,
  IN        UINT8         *Offset
  )
{
  UINT8 DataPosition;
  UINT8 Pattern;
  UINT8 BusWidth;

  *Offset = 0;
  for (DataPosition = 0; DataPosition < 8; DataPosition++) {
    if (((Data >> DataPosition) & 1 ) == 0 ) {
      Pattern = ZEROS;
    } else {
      Pattern = ONES;
    }
    for (BusWidth = 0; BusWidth < BUSWIDTH; BusWidth++) {
      Ptr[ (*Offset) ++] = Pattern;
    }
  }
  return MMRC_SUCCESS;
}

 
MMRC_STATUS
BuildCacheArrays (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *CL,
  IN        UINT8         *Patterns,
  IN        UINT32        NumberPatterns,
  IN        UINT32        *NumCL
  )
{
  UINT32 ClIndex;
  UINT32 PatternIndex;
  UINT32 ClSize;
  UINT8  TempValue;

  ClIndex = 0;
  ClSize = 0;

  for (PatternIndex = 0; PatternIndex < NumberPatterns; PatternIndex++ ) {
    FillCacheArray (MrcData, CL + (ClIndex * 64) + ClSize, Patterns[PatternIndex], &TempValue);
    ClSize += TempValue;
    if (ClSize >= 64) {
      ClIndex += 1;
      ClSize = 0;
    }
  }

  *NumCL = ClIndex + 1;
  return MMRC_SUCCESS;
}

 
MMRC_STATUS
Enable16KMTRR (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        TargetAddress,
  IN        UINT32        Msr
  )
{
  volatile UINT32 MtrrAddr;

  MtrrAddr = TargetAddress & 0xFFFFC000; // 16K align
  MtrrAddr |= 0x01; // set Type to WC
#ifndef _MSC_EXTENSIONS
  asm (// set Variable MTRR at ADDRESS to WB
    "mov %0,%%eax;"
    "mov $0x00000000,%%edx;"
    "mov $0x0000020E,%%ecx;"
    "wrmsr;"
    :/* no outputs */
  :"c" (MtrrAddr)
    :"%eax", "%edx"
    );
  asm (// set size to 16K and enable
    "mov $0xFFFFC800,%%eax;"
    "mov $0x00000000,%%edx;"
    "mov $0x0000020F,%%ecx;"
    "wrmsr;"
    :/* no outputs */
  :/* no inputs */
  :"%eax", "%ecx", "%edx"
    );
#else
  _asm mov eax, MtrrAddr;
  _asm mov edx, 0x00000000;
  _asm mov ecx, Msr;
  _asm wrmsr; // set Variable MTRR at ADDRESS to WB
  _asm mov eax, 0xFFFFC800;
  _asm mov edx, 0x00000000;
  _asm mov ecx, Msr;
  _asm add ecx, 1;
  _asm wrmsr; // set size to 16K and enable
#endif // VS_ENV

  return MMRC_SUCCESS;
}

 
MMRC_STATUS 
LoadXmm03WithPattern (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *CacheLines
  )
{
#ifndef _MSC_EXTENSIONS
  asm ("mov %0,%%eax;"
    "movdqa 0x00(%%eax),%%xmm0;"
    "movdqa 0x10(%%eax),%%xmm1;"
    "movdqa 0x20(%%eax),%%xmm2;"
    "movdqa 0x30(%%eax),%%xmm3;"
    :/* no outputs */
  :"r" (CacheLines)
    :"%eax"
    );
#else
  _asm mov eax, CacheLines;
  _asm movdqa xmm0, [eax+0x00];
  _asm movdqa xmm1, [eax+0x10];
  _asm movdqa xmm2, [eax+0x20];
  _asm movdqa xmm3, [eax+0x30];
#endif // _MSC_EXTENSIONS

  return MMRC_SUCCESS;
}

 
MMRC_STATUS
BurstOutCachelinesViaXmm (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        Address,
  IN        UINT32        CacheLineRepeatCount
  )
{
#ifndef _MSC_EXTENSIONS
  asm (// write to RAM from CACHE
    "mov %1,%%eax;" // CacheLineRepeatCount
    "mov %0,%%ebx;" // Address
    "mrc_wr_loop:"
    "movntdq %%xmm0,0x00(%%ebx);"
    "movntdq %%xmm1,0x10(%%ebx);"
    "movntdq %%xmm2,0x20(%%ebx);"
    "movntdq %%xmm3,0x30(%%ebx);"
    "add $0x40,%%ebx;" // next cacheline
    "dec %%eax;"
    "jnz mrc_wr_loop;"
    "sfence;"
    :
  :"m" (Address), "m" (CacheLineRepeatCount)
    :"%eax", "%ebx"
    );
#else
  __asm {
    // write to RAM from CACHE
    mov eax, CacheLineRepeatCount;
    mov ebx, Address;
mrc_wr_loop:
    movntdq [ebx+0x00], xmm0;
    movntdq [ebx+0x10], xmm1;
    movntdq [ebx+0x20], xmm2;
    movntdq [ebx+0x30], xmm3;
    add ebx, 0x40;
    dec eax;
    jnz mrc_wr_loop;
    sfence;
  };
#endif // _MSC_EXTENSIONS
  return MMRC_SUCCESS;
}

 
MMRC_STATUS
ReadRamIntoCache (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        TargetAddress,
  IN        UINT32        CacheLineRepeatCount
  )
{
#ifndef _MSC_EXTENSIONS
  asm ("mov %1,%%eax;" // burst_cnt
    "mov %0,%%ebx;" // Address
    "mrc_rd_loop:"
    "prefetcht0 0x00(%%ebx);" // read cacheline at (EBX)
    "add $0x40,%%ebx;" // next cacheline
    "dec %%eax;"
    "jnz mrc_rd_loop;"
    "lfence;"
    :
  :"m" (TargetAddress), "m" (CacheLineRepeatCount)
    :"%eax", "%ebx", "%ecx", "%edx"
    );
#else
  __asm {
    // write to RAM from CACHE
    mov eax, CacheLineRepeatCount;
    mov ebx, TargetAddress;
ReadRAMIntoCacheLoop:
    prefetcht0 [ebx];
    add ebx, 0x40;
    dec eax;
    jnz ReadRAMIntoCacheLoop;
    lfence;
    mov eax, [ebx-0x40];
  };
#endif // _MSC_EXTENSIONS

  return MMRC_SUCCESS;
}

 
MMRC_STATUS
CompareCacheWithXmm47 (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT32        TargetAddress,
  IN        UINT32        CacheLineRepeatCount,
  IN        UINT32        *EncodedFailPtr
  )
{
  VOLATILE UINT32 EncodedFail;
  VOLATILE UINT32 FailingAddress;

#ifndef _MSC_EXTENSIONS
  asm ("mov %3,%%eax;" // burst_cnt
    "mov %2,%%ebx;" // Address
    "mrc_verify_loop:"
    "movdqa %%xmm0,%%xmm4;"     // Place the original cachline values into XMM4-7 for inplace comparison.
    "movdqa %%xmm1,%%xmm5;"
    "movdqa %%xmm2,%%xmm6;"
    "movdqa %%xmm3,%%xmm7;"
    "mov $0x0000FFFF,%%edx;"    // assume passing condition
    "pcmpeqb 0x00(%%ebx),%%xmm4;"   // XMM0 has the failing bytes for 1st OWORD in cacheline (0x00 is a failure)
    "pmovmskb %%xmm4,%%ecx;"        // ECX = packed WORD result for 1st OWORD in cacheline
    "and %%ecx,%%edx;"              // EDX = packed WORD result for cacheline (cumulative)
    "pcmpeqb 0x10(%%ebx),%%xmm5;"   // XMM1 has the failing bytes for 2nd OWORD in cacheline (0x00 is a failure)
    "pmovmskb %%xmm5,%%ecx;"        // ECX = packed WORD result for 2nd OWORD in cacheline
    "and %%ecx,%%edx;"              // EDX = packed WORD result for cacheline (cumulative)
    "pcmpeqb 0x20(%%ebx),%%xmm6;"   // XMM2 has the failing bytes for 3rd OWORD in cacheline (0x00 is a failure)
    "pmovmskb %%xmm6,%%ecx;"        // ECX = packed WORD result for 3rd OWORD in cacheline
    "and %%ecx,%%edx;"              // EDX = packed WORD result for cacheline (cumulative)
    "pcmpeqb 0x30(%%ebx),%%xmm7;"   // XMM3 has the failing bytes for 4th OWORD in cacheline (0x00 is a failure)
    "pmovmskb %%xmm7,%%ecx;"        // ECX = packed WORD result for 4th OWORD in cacheline
    "and %%ecx,%%edx;"              // EDX = packed WORD result for cacheline (cumulative)
    "cmp $0x0000FFFF,%%edx;"    // check for failure
    "jne mrc_return_fail;"      // failure detected
    "add $0x40,%%ebx;"
    "dec %%eax;"
    "jnz mrc_verify_loop;" // finish the dataset (BURST_CNT)
    "jmp mrc_return_pass;" // everything passed
    "mrc_return_fail:"
    "xor $0xFFFFFFFF,%%edx;" // invert to make 0xFF a failure
    "and $0x0000FFFF,%%edx;" // only 16 Bits are valid
    "jmp mrc_verify_end;"
    "mrc_return_pass:"
    "mov $0x00,%%edx;"
    "mrc_verify_end:"
    "mov %%edx,%0;" // encoded WORD in edx goes to encoded_fail
    "mov %%ebx,%1;" // Address of failure goes to failing_addr
    :"=m" (EncodedFail), "=m" (FailingAddress)
    :"m" (TargetAddress), "m" (CacheLineRepeatCount)
    :"%eax", "%ebx", "%ecx", "%edx"
    );
#else
  __asm {
    mov eax, CacheLineRepeatCount;
    mov ebx, TargetAddress;
mrc_verify_loop:
    movdqa xmm4, xmm0;
    movdqa xmm5, xmm1;
    movdqa xmm6, xmm2;
    movdqa xmm7, xmm3;
    mov edx, 0x0000FFFF;
    pcmpeqb xmm4, [ebx+0x00];
    pmovmskb ecx, xmm4;
    and edx, ecx;
    pcmpeqb xmm5, [ebx+0x10];
    pmovmskb ecx, xmm5;
    and edx, ecx;
    pcmpeqb xmm6, [ebx+0x20];
    pmovmskb ecx, xmm6;
    and edx, ecx;
    pcmpeqb xmm7, [ebx+0x30];
    pmovmskb ecx, xmm7;
    and edx, ecx;
    cmp edx, 0x0000FFFF;
    jne mrc_return_fail;
    add ebx, 0x40;
    dec eax;
    jnz mrc_verify_loop;
    jmp mrc_return_pass;
mrc_return_fail:
    xor edx, 0xFFFFFFFF;
    and edx, 0x0000FFFF;
    jmp mrc_verify_end;
mrc_return_pass:
    mov edx, 0x00;
mrc_verify_end:
    mov EncodedFail, edx;
    mov FailingAddress, ebx;
  };
#endif // _MSC_EXTENSIONS

  *EncodedFailPtr = EncodedFail;
  return MMRC_SUCCESS;
}

#endif // CPGC_API