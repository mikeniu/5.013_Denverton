;------------------------------------------------------------------------------
;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;
; Copyright (c)2009-2017 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
;
; Module Name:
;
;   Smm.asm
;
; Abstract:
;
;   Assembly code of EINJ
;
;------------------------------------------------------------------------------

    .486p
    .xmm
    .model flat,C
    .code

;------------------------------------------------------------------------------
; VOID
; IA32API
; AsmWbinvd (
;   VOID
;   );
;------------------------------------------------------------------------------
AsmWbinvd   PROC
    wbinvd
    ret
AsmWbinvd   ENDP

;------------------------------------------------------------------------------
; VOID
; IA32API
; AsmNop (
;   VOID
;   );
;------------------------------------------------------------------------------
AsmNop   PROC
  nop
  nop
  ret
AsmNop   ENDP

;;;;;; Added for WheaEinj driver ;;;;;
;------------------------------------------------------------------------------
; VOID *
; EFIAPI
; AsmFlushCacheLine (
;   IN      VOID                      *LinearAddress
;   );
;------------------------------------------------------------------------------
AsmFlushCacheLine   PROC
    mfence
    clflush [eax]
    mfence
    mov     eax, ecx
    ret
AsmFlushCacheLine   ENDP

;;;;;; Added for WheaEinj driver ;;;;;
;------------------------------------------------------------------------------
; VOID *
; EFIAPI
; AsmCommitLine (
;   IN      VOID                      *LinearAddress,
;   IN      UINT32                    Value
;   );
;------------------------------------------------------------------------------
AsmCommitLine   PROC
    mfence
    mov dword ptr[ecx],edx
    clflush [ecx]
    mfence
    mov     eax, ecx
    ret
AsmCommitLine   ENDP

    END
