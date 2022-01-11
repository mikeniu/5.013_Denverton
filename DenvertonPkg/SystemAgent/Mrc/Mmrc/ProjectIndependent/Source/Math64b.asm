;**************************************************************************
;*                                                                        *
;*      Intel Restricted Secret                                           *
;*                                                                        *
;*      Avoton Memory Reference Code                                   *
;*                                                                        *
;*      Copyright (c) 2013 Intel Corp.                                    *
;*                                                                        *
;*      This program has been developed by Intel Corporation.             *
;*      Licensee has Intel's permission to incorporate this source code   *
;*      into their product, royalty free.  This source code may NOT be    *
;*      redistributed to anyone without Intel's written permission.       *
;*                                                                        *
;*      Intel specifically disclaims all warranties, express or           *
;*      implied, and all liability, including consequential and other     *
;*      indirect damages, for the use of this code, including liability   *
;*      for infringement of any proprietary rights, and including the     *
;*      warranties of merchantability and fitness for a particular        *
;*      purpose.  Intel does not assume any responsibility for any        *
;*      errors which may appear in this code nor any responsibility to    *
;*      update it.                                                        *
;*                                                                        *
;**************************************************************************
;**************************************************************************
;*                                                                        *
;*  PURPOSE:                                                              *
;*                                                                        *
;*      This file contains sample code to initialize processor cache      *
;*      and pass control to the Memory Reference Code                     *
;*                                                                        * 
;**************************************************************************

.386
.model  flat,C
.code
;  Performs a mutiplication of 2 64bit number.
;  Applies for both signed and unsigned numbers 
;   
;  The multiplication will be calculated with the next equation:
; 
;    Multiplier  = A {AH, AL}
;    Mutiplicand = B {BH, BL}
; 
;    Multiplication[63:32] = BL*AH + AL*BH
;    Multiplication[31:0]  = AL*BL
;           
;  @param[in]   [bsp + 0x08]       Multiplier.
;  @param[in]   [bsp + 0x10]       Mutiplicand.        
;  @param[out]  EDX:EAX            Product Mutiplier * Multiplicand (64b).
_allmul  PROC  NEAR
  ; Save stack
  push  ebp
  mov   ebp, esp
  push  ebx

  ; Get AL (Multplier Low)
  mov   edx, dword ptr [ebp + 08h]
  ; Get BL (Multiplicand Low)
  mov   ebx, dword ptr [ebp + 10h]

  mov   ecx, ebx
  mov   eax, edx
  ; Multiply BL (EBX) * AH (ebp + 0x0c) -> EBX
  imul  ebx, dword ptr [ebp + 0ch]
  ; Multiply AL (EDX) * BH (ebp + 0x14) -> EDX
  imul  edx, dword ptr [ebp + 14h]
  ; BL*AH * AL*BH -> EBX
  add   ebx, edx
  ; Multiply AL * BL -> EDX:EAX
  mul   ecx
  ; EBX + EDX {AL * BL}
  add   edx, ebx

  ; Restore stack
  pop   ebx
  pop   ebp

  ret   16
_allmul  ENDP


; Performs a division of one 64bit number (dividend) and one 32 
;  bit number (divisor). 
;   
;  The division will be calculated with the next equation:
; 
;    Dividend  = A {AH, AL}
;    Divisor   = B {BH, BL}
; 
;    Division[63:32] = AH / BL -> EDX
;    Division[31:0]  = AL / BL -> EAX
;           
;  @param[in]   [bsp + 0x08]     Dividend.
;  @param[in]   [bsp + 0x10]     Divisor.
;  @param[out]  EDX:EAX          Division Dividend (64b) / Divisor (32b).
_aulldiv  PROC  NEAR
  ; Save stack
  push  ebp
  mov   ebp, esp

  ; Get AH (Dividend High)
  mov   eax, dword ptr [ebp + 0ch]
  ; Get BL (Divisor Low)
  mov   ecx, dword ptr [ebp + 10h]
  ; Clear EDX
  xor   edx, edx
  ; Division AH / BL
  div   ecx
  ; Save value stored in EAX Quotient
  push  eax
  ; Get AL (Dividend Low)
  mov   eax, dword ptr [ebp + 08h]
  ; Division AL / BL
  div   ecx
  ; Restore the High Quotient
  pop   edx

  ; Restore stack
  pop   ebp

  ret   16
_aulldiv  ENDP

END
