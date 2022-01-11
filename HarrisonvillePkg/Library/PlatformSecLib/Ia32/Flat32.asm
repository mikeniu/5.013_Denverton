;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
; Module Name:
;
;  Flat32.asm
;
; Abstract:
;
;  This is the code that goes from real-mode to protected mode.
;  It consumes the reset vector.
;
;------------------------------------------------------------------------------
  INCLUDE Platform.inc
  INCLUDE Ia32.inc
  INCLUDE Chipset.inc
  INCLUDE SecCore.inc
  
  include token.equ	; Denverton AptioV Override - EIP#303338

.686p
.xmm
.model small, c

;PUBLIC  TopOfCar
EXTRN   SecStartup:NEAR
;EXTRN   MicrocodeStart:DWORD
;EXTRN   MicrocodeEnd:DWORD

EXTRN   PcdGet32 (PcdNemCodeCacheBase):DWORD
EXTRN   PcdGet32 (PcdNemCodeCacheSize):DWORD
EXTRN   PcdGet32 (PcdTemporaryRamBase):DWORD
EXTRN   PcdGet32 (PcdTemporaryRamSize):DWORD
EXTRN   PcdGet64 (PcdPciExpressBaseAddress):QWORD
EXTRN   PcdGet64 (PcdCpuMicrocodePatchAddress):QWORD
EXTRN   PcdGet64 (PcdCpuMicrocodePatchRegionSize):QWORD

; Denverton AptioV Override Start - EIP#303338
FV_LENGTH		EQU (16 + 16)
FV_SIGNATURE    EQU (FV_LENGTH + 8)
FV_HDR_LENGTH	EQU (FV_SIGNATURE + 4 + 4)
FV_EXT_HDR_OFFSET EQU (FV_HDR_LENGTH + 2 + 2)

FV_EXT_HDR_SIZE EQU 16

FFS_TYPE        EQU (16 + 2)
FFS_LENGTH      EQU (FFS_TYPE + 1 + 1)
FFS_HDR_LENGTH	EQU (FFS_LENGTH + 3 + 1)

uCODE_CPU_SIGNATURE	EQU 12
uCODE_CPU_FLAGS		EQU	24
uCODE_DATA_SIZE		EQU	28
uCODE_TOTAL_SIZE	EQU	32
; Denverton AptioV Override End - EIP#303338

_TEXT_REALMODE      SEGMENT PARA PUBLIC USE16 'CODE'
                    ASSUME  CS:_TEXT_REALMODE, DS:_TEXT_REALMODE

;------------------------------------------------------------------------------
;
;  SEC "Security" Code module.
;
;  Transition to non-paged flat-model protected mode from a
;  hard-coded GDT that provides exactly two descriptors.
;  This is a bare bones transition to protected mode only
;  used for while in PEI and possibly DXE.
;
;  IA32 specific cache as RAM modules
;
;  After enabling protected mode, a far jump is executed to
;  TransferToPEI using the newly loaded GDT.
;  This code also enables the Cache-as-RAM
;
;  RETURNS:    none
;
;  MMX Usage:
;              MM0 = BIST State
;              MM1 = Current Package Physical Info
;                    [7:0]   = Cluster ID
;                    [15:8]  = Total Prossor pacakge detected in system
;                    [16] = BAD CMOS Flag
;                    [17] = AuburnDale or ClarksField
;                           [0] = AuburnDale
;                           [1] = ClarksField
;                    [18] = Contain SEC reset flag
;                           CPU Only Reset Flag
;                    [19] = Contain SEC reset flag
;                           Power Good Reset Flag
;                    [23:20] = Reserved
;                    [31:24] = Reserved
;              MM2 = store common MAX & MIN ratio
;              MM3 = Patch Revision
;              MM4 = Patch Pointer
;              MM5 = Reserved
;              MM6 = Reserved
;              MM7 = Used in CALL_MMX & RET_ESI micaro
;
;------------------------------------------------------------------------------

; Nehalem Reset Boot Flow Start

align 4
Flat32Start PROC NEAR C PUBLIC
  ;
  ; Save BIST state in MM0
  ;
  ;jmp $
  nop
  nop
  nop
  
  fninit                                ; clear any pending Floating point exceptions
  movd    mm0, eax
  cli
  

  ;
  ; Enter Protected mode.
  ; 
  STATUS_CODE (01h)                     ; BSP_PROTECTED_MODE_START, IN BXT - CHECKPOINT: 01 (0 milliseconds)
  ;jmp $
  nop
  nop
  nop
  
  mov     esi,  OFFSET GdtDesc
  DB      66h
  lgdt    fword ptr cs:[si]
  mov     eax, cr0                      ; Get control register 0
  or      eax, 00000003h                ; Set PE bit (bit #0) & MP bit (bit #1)
  mov     cr0, eax                      ; Activate protected mode
  mov     eax, cr4                      ; Get control register 4
  or      eax, 00000600h                ; Set OSFXSR bit (bit #9) & OSXMMEXCPT bit (bit #10)
  mov     cr4, eax

  ;
  ; Now we're in Protected16
  ; Set up the selectors for protected mode entry
  ;
  mov     ax, SYS_DATA_SEL
  mov     ds, ax
  mov     es, ax
  mov     fs, ax
  mov     gs, ax
  mov     ss, ax

  ;
  ; Go to Protected32
  ;
  mov     esi, offset NemInitLinearAddress
  jmp     fword ptr cs:[si]

TightLoop:
  cli
  hlt
  jmp     TightLoop

Flat32Start ENDP
_TEXT_REALMODE      ENDS

_TEXT_PROTECTED_MODE      SEGMENT PARA PUBLIC USE32 'CODE'
                          ASSUME  CS:_TEXT_PROTECTED_MODE, DS:_TEXT_PROTECTED_MODE

; Denverton AptioV Override Start - EIP#303338
MICOCODE_FFS_GUID label dword
	dd	17088572h
	dw	377Fh
	dw	44efh
	db	8Fh,4Eh,0B0h,9Fh,0FFh,46h,0A0h,70h
; Denverton AptioV Override End - EIP#303338

CALL_MMX macro   RoutineLabel

  local   ReturnAddress
  mov     esi, offset ReturnAddress
  movd    mm7, esi                      ; save ReturnAddress into MM7
  jmp     RoutineLabel
ReturnAddress:

endm

RET_ESI  macro

  movd    esi, mm7                      ; restore ESP from MM7
  jmp     esi

endm

CALL_EBP macro   RoutineLabel

  local   ReturnAddress
  mov     ebp, offset ReturnAddress
  jmp     RoutineLabel
ReturnAddress:

endm

RET_EBP  macro

  jmp     ebp                           ; restore ESP from EBP

endm

align 4
ProtectedModeSECStart PROC NEAR PUBLIC

  STATUS_CODE (02h)
  STATUS_CODE (03h)

  CALL_MMX  PlatformInitialization
  STATUS_CODE (04h)

  STATUS_CODE (09h)
  CALL_MMX  InitializeNEM

  STATUS_CODE (0Ah)
  CALL_MMX  EstablishStack         ; For CPU SV

  STATUS_CODE (0Bh)
  
  jmp  CallPeiCoreEntryPoint

ProtectedModeSECStart ENDP

ProtectedModeEntryPoint PROC NEAR PUBLIC

  RET_ESI

ProtectedModeEntryPoint  ENDP

; Denverton AptioV Override Start - EIP#303338
FindMicrocode proc
IF  MKF_MICROCODE_SPLIT_BB_UPDATE
    mov     ebx, MKF_FV_MICROCODE_UPDATE_BASE
FindMicrocodeFv:
ELSE
    mov     ebx, MKF_FV_MICROCODE_BASE
ENDIF
	mov		edx, ebx
    cmp     dword ptr [edx + FV_SIGNATURE], 'HVF_'
    jne     Microcode_Not_Found             ;Corrupt FV?
	mov		eax, [edx + FV_HDR_LENGTH]
	and		eax, 0ffffh		;Get 16 bit FV header length
	add		ebx,  eax		;ebx = start of FFS
    jc      Microcode_Not_Found             ;Corrupt FV?

	movzx   eax, word ptr [edx + FV_EXT_HDR_OFFSET]	;Optional Header
    cmp ax, 0								;Optional exteneded header ?
	je  @f
	lea eax, [edx + eax]					;eax = extended header

	mov ecx, [eax + FV_EXT_HDR_SIZE]		;ecx = extended header size
	mov ebx, eax
	add ebx, ecx
	jc  Microcode_Not_Found                 ;Corrupt FV?
@@:

	add		ebx, 7							;Align to 8-bytes for FFs
	and		ebx, not 7

	add		edx, [edx + FV_LENGTH]			;edx = end of FV.
    jz      @f                              ;zf if Boot Block
    jc      Microcode_Not_Found             ;Corrupt FV?
@@:
    dec     edx                             ;edx = End of FFS

;---Find File ---
;ebx = Start of FFS
;edx = End of FFS
Get_Next_File:
    cmp     byte ptr [ebx + FFS_TYPE], 0f0h ;iqnore guid for pad type
    je  @f
	cmp		dword ptr [ebx], -1		;Is in end of files, but not firmware volume.
	je		File_not_found
@@:
	mov		ecx, 4
	mov		esi, ebx
	mov		edi, offset MICOCODE_FFS_GUID
	;compare file guid
	repe 	cmpsd
	je		Found_File
;---get next file--
	;get file length and add to ebx
	mov		eax, [ebx + FFS_LENGTH]
	and		eax, 0ffffffh			;eax = File length (24 bits)
	add		ebx, eax				;next file
	;align file
	add		ebx, 7
    jc      Microcode_Not_Found     ;Check for wrap-around if bad data.
	and		ebx, not 7

    mov     eax, ebx
    add     eax, FFS_HDR_LENGTH     ;eax is microcode data
    cmp     eax, edx                ;Is end of firmware volume?
	jb		Get_Next_File
File_not_found:
    jmp     Microcode_Not_Found
;---Found File---
Found_File:
;ebx = Start of Microcode FFS
;edx = End of FFS + 1

;---Search micocode for match.---
	mov		edi, ebx

	mov		eax, [ebx + FFS_LENGTH]
	and		eax, 0ffffffh			;eax = File length (24 bits)
	add		edi, eax				;edi = end of file
    jc      Microcode_Not_Found     ;Check for wrap-around if bad data.

IF MKF_MPDTable_CREATED
    movzx   eax, word ptr [edi-2]           ;eax = MPDT length
    sub     edi, eax                        ;edi = end of microcodes.
ENDIF

	add		ebx, FFS_HDR_LENGTH
    jc      Microcode_Not_Found     ;Check for wrap-around if bad data.

	mov		esi, ebx

    mov     eax, 1
    cpuid
    mov		ebx, eax	;ebx = CPU signature

	mov		ecx, 17h
	rdmsr				;read CPUS flags

	shr		edx, 18
	and		dl, 7		;dl = CPU FLAG
	mov		cl, dl
	mov		dl, 1
	shl		dl, cl		;dl = bit set for CPU flag

    xchg    ebx, esi    ;ebx = ptr, esi = signature
Next_Microcode:
	cmp		ebx, edi                ;Check to see if microcode at end of FFS File.
	jae		Microcode_Not_Found

	cmp		dword ptr [ebx], 1 ;Check if correct header revision
	jne		Microcode_Not_Found

	mov		ecx, 2048	;total size if data size in field = 0.
	cmp		dword ptr [ebx + uCODE_DATA_SIZE], 0
	jz		@f
	mov		ecx, [ebx + uCODE_TOTAL_SIZE]	;Get total size.
@@:

	cmp		esi, [ebx + uCODE_CPU_SIGNATURE]
	jne		Find_Next_Microcode  ;Extended signature check not required

        mov     eax, ebx                        ;Store uC addr in eax, so if jnz succeeds.
	test	dl, [ebx + uCODE_CPU_FLAGS]     ;uC flags match?
	jnz		Exit_Find_Microcode
Find_Next_Microcode:
    ;align to next block size
IF MKF_PACK_MICROCODE
    add     ecx, 15
    and     ecx, 0fffffff0h
ELSE
    add     ecx, MKF_MICROCODE_BLOCK_SIZE - 1
    and     ecx, NOT (MKF_MICROCODE_BLOCK_SIZE - 1)
ENDIF
	add		ebx, ecx
    jc      Microcode_Not_Found             ;Overflow - bad data.
	jmp		Next_Microcode

Microcode_Not_Found:
IF  MKF_MICROCODE_SPLIT_BB_UPDATE
    cmp     ebx, MKF_FV_MICROCODE_BASE
    jae     @f
    mov     ebx, MKF_FV_MICROCODE_BASE
    jmp     FindMicrocodeFv
@@:
ENDIF
	xor		eax, eax
Exit_Find_Microcode:
jmp FindMicrocodeEnd
FindMicrocode endp
; Denverton AptioV Override End - EIP#303338


UpdateMicrocode    PROC    NEAR    PRIVATE

; Find and Load the microcode update

; Denverton AptioV Override Start - EIP#249361

; Use "Intel Microcode" of AMI, to place microcode inside FV_MAIN/FV_BB and load from there, dynamically.

  ;
  ; Get the PLATFORM_ID[52:50] (i.e., MSR 17, bits 52:50) 
  ;
;  mov     ecx, IA32_PLATFORM_ID         ; Load the platform ID
;  rdmsr
;  shr     edx, (50-32)                  ; We are only looking for bits 52:50
;  and     edx, 07h                      ; Mask the platform ID bits
;  mov     esi, 0
;  bts     esi, edx                      ; Processor Flag bit (i.e., 1 << PLATFORM_ID) to test against
 
;  mov     eax, 1
;  cpuid
;  and     eax, 0FFF3FFFh                ; Save Processor Signature (i.e., CPUID) in eax

;  assume  edi:near ptr UpdateHeaderStruc
  
;  mov     edi, DWORD PTR PcdGet64 (PcdCpuMicrocodePatchAddress)  ; edi = Start address of ucode region

;FindUcodeMatch:

  ;
  ; Check the Microcode Update Header Version, should be 0x00000001
  ;
;  cmp     DWORD PTR [edi].dHeaderVersion, 1
;  jnz     InvalidHeaderVersion
  
  ;
  ; Check if the Processor Signature (i.e., CPUID) matches
  ;
;  cmp     eax, DWORD PTR [edi].dProcessorSignature 
;  jnz     NoUcodeMatch

  ;
  ; Check if the platform ID bit matches
  ;
;  and     esi, DWORD PTR [edi].dProcessorFlags
;  jnz     LoadUcode

;NoUcodeMatch:
;  mov     ebx, DWORD PTR [edi].dTotalSize
;  cmp     ebx, 0                        ; Use dTotalSize if it was specified
;  jne     NextMicrocodeUpdate
;  mov     ebx, 2048                     ; Use the legacy size of 2048 when dTotalSize is not specified
;  jmp     NextMicrocodeUpdate  

;InvalidHeaderVersion:
;  mov     ebx, 1024                     ; No valid Header Version, scan on the next 1K offset for a microcode update.
  
;NextMicrocodeUpdate:
;  add     edi, ebx
;  mov     ebx, DWORD PTR PcdGet64 (PcdCpuMicrocodePatchAddress)
;  add     ebx, DWORD PTR PcdGet64 (PcdCpuMicrocodePatchRegionSize)
;  cmp     edi, ebx
;  jae     EndUcodeLoad                  ;Jif address is outside of ucode region

;  jmp     FindUcodeMatch
  
;LoadUcode:  
;  mov     esi, DWORD PTR [edi].dUpdateRevision     

  ;
  ; Find and Load the microcode update
  ;

  jmp     FindMicrocode             ; Denverton AptioV Override - EIP#336766

FindMicrocodeEnd::
  ;
  ; Check if microcode is found
  ;
    or     eax, eax
    jz     EndUcodeLoad             ;No microcode found

FoundMicrocode: 
; Denverton AptioV Override End - EIP#249361

; Denverton AptioV Override Start - EIP#293996

  ; Store UpdateRevision of the microcode found, to check after update
  mov    esi, [eax + uCODE_UPDATE_REVISION] 
  
; Denverton AptioV Override End - EIP#293996
      
  ;
  ; Update the microcode
  ;
  mov     ecx, IA32_BIOS_UPDT_TRIG
;  mov     eax, edi		; Denverton AptioV Override - EIP#249361
  add     eax, sizeof UpdateHeaderStruc 
  xor     edx, edx
  wrmsr                                 ; Execute the uCode update load
  
  ;
  ; Check whether the update is successful
  ;        
  mov     ecx, IA32_BIOS_SIGN_ID
  xor     eax, eax
  xor     edx, edx
  wrmsr                                 ; Clear the uCode signature
  
  mov     eax, 1
  cpuid                                 ; Executing CPUID.(EAX=1) forces the uCode signature to be reloaded
  
  mov     ecx, IA32_BIOS_SIGN_ID
  rdmsr
  cmp     edx, esi                      ; if the equal, the microcode successfully loaded.
  jz      EndUcodeLoad
  
  ;
  ; Update failed
  ; 
  mov     al,  0EEh
  out     080h, al
;  jmp     $                            ; Indicate failure, but NEVER halt the system because Microcode fails to load!

EndUcodeLoad:
  assume  edi:nothing

  RET_EBP

UpdateMicrocode  ENDP

PlatformInitialization    PROC    NEAR    PRIVATE

  ;
  ; Program PCIEXBAR and enable it in 0/0/0 
  ; Lo - Offset 0x60
  ; Hi - Offset 0x64
  ;
  
  mov      eax, 080000060h
  mov      dx,  0CF8h
  out      dx,  eax
  mov      eax, DWORD PTR PcdGet64(PcdPciExpressBaseAddress) 
  add      eax, CPU_HEC_EN     
  mov      dx,  0CFCh
  out      dx,  eax

  ;
  ; Program and enable MCH base address.
  ;
  mov     edi, R_MCH_BASE
  mov     Dword Ptr [edi], MCH_BASE_ADDRESS + B_MCH_BASE_ADDRESS_EN

  ;
  ;   WA for Ax steppings.
  ;   Unset bit B_CR_BMCMODE_LOW.MC_SIGNAL_MODE (MCHBAR + 0x6E48[0]).
  ;   This will allow for the NEM setup loop to complete without hanging.
  ;

  ;
  ;   Test for Ax stepping first
  ;
  mov    eax, R_CUNIT_MANID
  mov    ebx, DWORD PTR [eax]
  and    ebx, CUNIT_MANID_MSID_MASK
  cmp    ebx, V_CUNIT_MANID_MSID_B_STEPPING
  jge    @F

  ;
  ;   Unset BMCMODE.MC_SIGNAL_MODE bit
  ;
  mov    edi, MCH_BASE_ADDRESS + R_MCH_BMCMODE
  mov    eax, DWORD PTR [edi]
  and    eax, NOT (B_MCH_BMCMODE_MC_SIGNAL)
  mov    DWORD PTR [edi], eax

@@:
  ;
  ; Update the microcode on the BSP.
  ;

  CALL_EBP UpdateMicrocode

  ;
  ; Program and enable all known base addresses
  ;

  ;
  ; Program and enable SPI base address.
  ;
  mov     edi, R_PCH_LPC_SPI_BASE
  mov     Dword Ptr [edi], SPI_BASE_ADDRESS

  mov     edi, SPI_CFG_BASE + R_PCI_CMD
  mov     Byte Ptr [edi], B_PCI_CMD_MSE
  ;
  ; Program and enable PMC base address.
  ;
  mov     edi, R_PCH_LPC_PMC_BASE
  mov     Dword Ptr [edi], PMC_BASE_ADDRESS

  mov     edi, R_PCH_LPC_PMC_ACTL
  or      Dword Ptr [edi], B_PCH_PMC_ACTL_BASE_EN

  ;
  ; Program and enable SBREG base address.
  ;
  mov     edi, R_P2SB_SBREG_BASE
  mov     Dword Ptr [edi], SBREG_BASE_ADDRESS

  mov     edi, P2SB_CFG_BASE + R_PCI_CMD
  mov     Byte Ptr [edi], B_PCI_CMD_MSE
  ;
  ; Program and enable ACPI base address.
  ;
  mov     edi, R_PCH_LPC_ACPI_BASE
  mov     Dword Ptr [edi], ACPI_BASE_ADDRESS

  mov     edi, R_PCH_LPC_PMC_ACTL
  or      Dword Ptr [edi], B_PCH_PMC_ACTL_ACPI_EN
  
  ;
  ; Program SMBus IO base address.
  ;
  mov     edi, R_PCH_SMBUS_BASE
  mov     Word Ptr [edi], SMBUS_BASE_ADDRESS

  ;
  ; Enable SMBus IO space.
  ;
  mov     edi, SMBUS_CFG_BASE + R_PCI_CMD
  mov     Byte Ptr [edi], B_PCI_CMD_IOSE

  ;
  ; End program and enable all known base addresses
  ;

  ;
  ; HPET memory address enable
  ;

  mov     edi, R_PCH_P2SB_HPTC
  mov     Byte Ptr [edi], HPTC_AE
	
  ;
  ; Check SUS Well Power Failure and General Reset Status to take right boot path
  ;
  mov     edi, PMC_CFG_BASE + R_PCH_PMC_GEN_PMCON_B        ; PMC_BASE_ADDRESS + R_PCH_PMC_GEN_PMCON_B
  mov     ax,  WORD PTR [edi]
  and     ax,  NOT (B_PCH_PMC_GEN_PMCON_B_PWR_FLR)
  mov     WORD PTR [edi], ax                               ; clear B_PCH_PMC_GEN_PMCON_SUS_PWR_FLR and B_PCH_PMC_GEN_PMCON_HOST_RST_STS
  mov     cx, ax                                           ; Save
  test    ax,  4000h                                       ; B_PCH_PMC_GEN_PMCON_SUS_PWR_FLR
  jnz     force_cold_boot_path		
  test    ax,  0200h                                       ; B_PCH_PMC_GEN_PMCON_GEN_RST_STS							
  jz      check_RTC_PWR_STS
  
  mov     edi, PMC_CFG_BASE + R_PCH_PMC_GEN_PMCON_A        ; PMC_BASE_ADDRESS + R_PCH_PMC_GEN_PMCON_A
  mov     eax,  DWORD PTR [edi]               
  test    eax,  00800000h                                  ; B_PCH_PMC_GEN_PMCON_DRAM_INIT
  jnz     check_RTC_PWR_STS                                ; DRAM initialization in progress

force_cold_boot_path:
  mov     dx, ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT       ; ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT
  in      ax, dx
  and     ax, NOT (01C00h)                                 ; V_PCH_ACPI_PM1_CNT_S5 ; Clear sleep type field SLP_TYP [12:10]
  out     dx, ax

check_RTC_PWR_STS:
  mov     ax, cx                                           ; restore
  test    ax, 004h                                         ; B_PCH_PMC_GEN_PMCON_RTC_PWR_STS
  jz      no_RTC_pwr_failure
    
  ;
  ; According to BIOS Specification, the following sequence must be programmed
  ; in order to ensure RTC state has been initialized.
  ;
  ; The System BIOS should execute the sequence below if the RTC_PWR_STS bit is set before memory initialization.
  ; This will ensure that the RTC state machine has been initialized.
  ;  1. If the RTC_PWR_STS bit is set, steps 2 through 5 should be executed.
  ;  2. Set RTC Register 0Ah[6:4] to '110' or '111'.
  ;  3. Set RTC Register 0Bh[7].
  ;  4. Set RTC Register 0Ah[6:4] to '010'.
  ;  5. Clear RTC Register 0Bh[7].
    
init_RTC_state_machine:
    
  ;
  ; Set RTC Register 0Ah[6:4] to '110' or '111'.
  ;
  mov     al, 0Ah
  out     070h, al
  nop                  ; Delay
  nop                  ; Delay
  mov     al, 066h
  out     071h, al
  nop                  ; Delay
  nop                  ; Delay

  ;
  ; Set RTC Register 0Bh[7].
  ;
  mov     al, 0Bh
  out     070h, al
  nop                  ; Delay
  nop                  ; Delay
  in      al, 071h
  nop                  ; Delay
  nop                  ; Delay
  or      al, 080h
  out     071h, al
  nop                  ; Delay
  nop                  ; Delay

  ;
  ; Set RTC Register 0Ah[6:4] to '010'.
  ;
  mov     al, 0Ah
  out     070h, al
  nop                  ; Delay
  nop                  ; Delay
  mov     al, 026h
  out     071h, al
  nop                  ; Delay
  nop                  ; Delay

  ;
  ; Clear RTC Register 0Bh[7].
  ;
  mov     al, 0Bh
  out     070h, al
  nop                  ; Delay
  nop                  ; Delay
  in      al, 071h
  nop                  ; Delay
  nop                  ; Delay
  and     al, NOT 080h
  out     071h, al
  nop                  ; Delay
  nop                  ; Delay

no_RTC_pwr_failure:
  ;
  ; Enable SPI Prefetch
  ;
  mov   	edi, SPI_CFG_BASE + R_PCH_SPI_BCR           ; SPI_BASE_ADDRESS BIOS Control Register
  or    	Dword Ptr [edi], 08h		            ; Bits [3:2] = '10' - enable prefetching and caching.

  ;
  ; Program 8259 Interrupt Controller to disable all interrupts
  ;
  mov     al, 0FFh
  out     21h, al     ; Mask off all interrupts in master 8259
  out     0a1h, al    ; Mask off all interrupts in slave 8259

  ;;; DNV_TODO: TCO registers are not in the spec.
  ;
  ; Halt TCO Timer
  ;
  ;mov     edi, ACPI_BASE_ADDRESS + R_PCH_TCO_CNT
  ;mov     ax,  WORD PTR [edi]
  ;or      ax, B_PCH_TCO_CNT_TMR_HLT
  ;mov     WORD PTR [edi], ax

  ;
  ; Clear the Second Timeout Status bit by writing 1
  ;
  ;mov     edi, ACPI_BASE_ADDRESS + R_PCH_TCO_STS
  ;mov     eax,  DWORD PTR [edi]
  ;or      eax, B_PCH_TCO_STS_SECOND_TO
  ;mov     DWORD PTR [edi], eax

  ;
  ; Enable SMBUS Host Controller and SPD write disable
  ;
  mov     edi, SMBUS_CFG_BASE + R_PCH_SMBUS_HOSTC
  mov     al,  011h                                        ; B_PCH_SMBUS_HOSTC_HST_EN | R_PCH_SMBUS_HOSTC[4]
  mov     Byte Ptr [edi], al

  mov     dx,  SMBUS_BASE_ADDRESS + R_PCH_SMBUS_AUXC
  xor     al,  al
  out     dx,  al

  ;
  ; Check to see if 0xCF9 Global Reset bit is set. if set clear it.
  ;
  mov     edi, PMC_CFG_BASE + R_PCH_PMC_ETR3
  mov     eax, DWORD PTR [edi]
  test    eax, B_PCH_PMC_ETR3_CF9GR                        ; B_PCH_PMC_PMIR_CF9GR  ; Check whether 0xCF9 Global Reset bit is set
  jz      GlobalresetClear                                 ; If no, continue
  and     eax, NOT (B_PCH_PMC_ETR3_CF9GR)                  ; B_PCH_PMC_PMIR_CF9GR  ; Clear 0xCF9 Global Reset bit
  mov     DWORD PTR [edi], eax

GlobalresetClear:
  ;
  ; Clear HPET Timer 0 Lower and Upper Comparator Value.
  ;

  xor     eax, eax
  mov     esi, HPET_COMP_1
  mov     Dword Ptr [esi], eax
  mov     esi, HPET_COMP_2
  mov     Dword ptr [esi], eax

  ;
  ; Enable Devices
  ;
  mov	dx,0cf8h
  mov	eax,80000054h
  out	dx,eax
  add	dx,4
  mov	eax,0ffffffffh
  out	dx,eax

  RET_ESI

PlatformInitialization    ENDP

;  STATUS_CODE (07h)
VeryEarlyMicrocodeUpdate    PROC    NEAR    PRIVATE

    RET_EBP
    
VeryEarlyMicrocodeUpdate    ENDP


;  STATUS_CODE (09h)
;************************************************************
; Description:
;
;   This function initializes the Cache for Data, Stack, and Code
;   as specified in the  BIOS Writer's Guide.
;************************************************************
InitializeNEM    PROC    NEAR    PRIVATE

  ;
  ;  Enable cache for use as stack and for caching code
  ;  The algorithm is specified in the processor BIOS writer's guide
  ;

  ;
  ;  Ensure that the system is in flat 32 bit protected mode. 
  ;
  ;  Platform Specific - configured earlier
  ;
  ;  Ensure that only one logical processor in the system is the BSP.
  ;  (Required step for clustered systems).
  ;
  ;  Platform Specific - configured earlier
  
  ;  Ensure all APs are in the Wait for SIPI state.
  ;  This includes all other logical processors in the same physical processor
  ;  as the BSP and all logical processors in other physical processors.
  ;  If any APs are awake, the BIOS must put them back into the Wait for
  ;  SIPI state by issuing a broadcast INIT IPI to all excluding self.
  ;
  mov     edi, APIC_ICR_LO               ; 0FEE00300h - Send INIT IPI to all excluding self 
  mov     eax, ORAllButSelf + ORSelfINIT ; 0000C4500h
  mov     [edi], eax

@@:
  mov     eax, [edi]
  bt      eax, 12                       ; Check if send is in progress
  jc      @B                            ; Loop until idle

  ;
  ;   Load microcode update into BSP.
  ;
  ;   Ensure that all variable-range MTRR valid flags are clear and 
  ;   IA32_MTRR_DEF_TYPE MSR E flag is clear.  Note: This is the default state
  ;   after hardware reset.
  ;
  ;   Platform Specific - MTRR are usually in default state.
  ;

  ;
  ;   Initialize all fixed-range and variable-range MTRR register fields to 0.
  ;
   mov   ecx, IA32_MTRR_CAP         ; get variable MTRR support
   rdmsr
   movzx ebx, al                    ; EBX = number of variable MTRR pairs
   shl   ebx, 2                     ; *4 for Base/Mask pair and WORD size
   add   ebx, MtrrCountFixed * 2    ; EBX = size of  Fixed and Variable MTRRs

   xor   eax, eax                       ; Clear the low dword to write
   xor   edx, edx                       ; Clear the high dword to write
   ;mov    ebx, MtrrCount * 2            ; ebx <- sizeof MtrrInitTable
InitMtrrLoop:
   add   ebx, -2
   movzx ecx, WORD PTR cs:MtrrInitTable[ebx]  ; ecx <- address of mtrr to zero
   wrmsr
   jnz   InitMtrrLoop                   ; loop through the whole table
  
  ;
  ;   Configure the default memory type to un-cacheable (UC) in the 
  ;   IA32_MTRR_DEF_TYPE MSR.
  ;
  mov     ecx, MTRR_DEF_TYPE            ; Load the MTRR default type index
  rdmsr
  and     eax, NOT (00000CFFh)          ; Clear the enable bits and def type UC.
  wrmsr
  
  ;
  ; Configure MTRR_PHYS_MASK_HIGH for proper addressing above 4GB
  ; based on the physical address size supported for this processor
  ; This is based on read from CPUID EAX = 080000008h, EAX bits [7:0]
  ; 
  ; Examples: 
  ;  MTRR_PHYS_MASK_HIGH = 00000000Fh  For 36 bit addressing
  ;  MTRR_PHYS_MASK_HIGH = 0000000FFh  For 40 bit addressing
  ;
  mov   eax, 80000008h                  ; Address sizes leaf
  cpuid  
  sub   al, 32
  movzx eax, al
  xor   esi, esi
  bts   esi, eax
  dec   esi                             ; esi <- MTRR_PHYS_MASK_HIGH

  ;   
  ;   Configure the DataStack region as write-back (WB) cacheable memory type
  ;   using the variable range MTRRs.
  ;

  ;
  ; Set the base address of the DataStack cache range
  ;
  mov     eax, PcdGet32 (PcdTemporaryRamBase)
  or      eax, MTRR_MEMORY_TYPE_WB
                                        ; Load the write-back cache value
  xor     edx, edx                      ; clear upper dword
  mov     ecx, MTRR_PHYS_BASE_0         ; Load the MTRR index
  wrmsr                                 ; the value in MTRR_PHYS_BASE_0
  
  ;
  ; Set the mask for the DataStack cache range
  ; Compute MTRR mask value:  Mask = NOT (Size - 1)
  ;
  mov  eax, PcdGet32 (PcdTemporaryRamSize)
  dec  eax
  not  eax
  or   eax, MTRR_PHYS_MASK_VALID
                                        ; turn on the Valid flag
  mov  edx, esi                         ; edx <- MTRR_PHYS_MASK_HIGH
  mov  ecx, MTRR_PHYS_MASK_0            ; For proper addressing above 4GB
  wrmsr                                 ; the value in MTRR_PHYS_BASE_0

  ;
  ;   Configure the BIOS code region as write-protected (WP) cacheable 
  ;   memory type using a single variable range MTRR.
  ;
  ;   Platform Specific - ensure region to cache meets MTRR requirements for 
  ;   size and alignment.
  ;

  ;
  ; Set the base address of the CodeRegion cache range
  ;
  mov     eax, PcdGet32 (PcdNemCodeCacheSize)
  mov     edi, PcdGet32 (PcdNemCodeCacheBase)

  ;
  ; Round up to page size
  ;
  mov     ecx, eax                      ; Save
  and     ecx, 0FFFF0000h               ; Number of pages in 64K
  and     eax, 0FFFFh                   ; Number of "less-than-page" bytes
  jz      Rounded
  mov     eax, 10000h                   ; Add the whole page size

Rounded:
  add     eax, ecx                      ; eax - rounded up code cache size

  ;
  ; Define "local" vars for this routine
  ; Note that mm0 is used to store BIST result for BSP,
  ; mm1 is used to store the number of processor and BSP APIC ID,
  ; mm2 is used to store common MAX & MIN ratio
  ;
  CODE_SIZE_TO_CACHE    TEXTEQU  <mm3>
  CODE_BASE_TO_CACHE    TEXTEQU  <mm4>
  NEXT_MTRR_INDEX       TEXTEQU  <mm5>
  NEXT_MTRR_SIZE        TEXTEQU  <mm6>
  ;
  ; Initialize "locals"
  ;
  sub     ecx, ecx
  movd    NEXT_MTRR_INDEX, ecx          ; Count from 0 but start from MTRR_PHYS_BASE_1

  ;
  ; Save remaining size to cache
  ;
  movd    CODE_SIZE_TO_CACHE, eax       ; Size of code cache region that must be cached
  movd    CODE_BASE_TO_CACHE, edi       ; Base code cache address

NextMtrr:
  ;
  ; Get remaining size to cache
  ;
  movd    eax, CODE_SIZE_TO_CACHE
  and     eax, eax
  jz      CodeRegionMtrrdone            ; If no left size - we are done
  ;
  ; Determine next size to cache.
  ; We start from bottom up. Use the following algorythm:
  ; 1. Get our own alignment. Max size we can cache equals to our alignment
  ; 2. Determine what is bigger - alignment or remaining size to cache.
  ;    If aligment is bigger - cache it.
  ;      Adjust remaing size to cache and base address
  ;      Loop to 1.
  ;    If remaining size to cache is bigger
  ;      Determine the biggest 2^N part of it and cache it.
  ;      Adjust remaing size to cache and base address
  ;      Loop to 1.
  ; 3. End when there is no left size to cache or no left MTRRs
  ;
  movd    edi, CODE_BASE_TO_CACHE
  bsf     ecx, edi                      ; Get index of lowest bit set in base address
  ;
  ; Convert index into size to be cached by next MTRR
  ;
  mov     edx, 1h
  shl     edx, cl                       ; Alignment is in edx
  cmp     edx, eax                      ; What is bigger, alignment or remaining size?
  jbe     gotSize                       ; JIf aligment is less
  ;
  ; Remaining size is bigger. Get the biggest part of it, 2^N in size
  ;
  bsr     ecx, eax                      ; Get index of highest set bit
  ;
  ; Convert index into size to be cached by next MTRR
  ;
  mov     edx, 1
  shl     edx, cl                       ; Size to cache

GotSize:
  mov     eax, edx
  movd    NEXT_MTRR_SIZE, eax           ; Save

  ;
  ; Compute MTRR mask value:  Mask = NOT (Size - 1)
  ;
  dec     eax                           ; eax - size to cache less one byte
  not     eax                           ; eax contains low 32 bits of mask
  or      eax, MTRR_PHYS_MASK_VALID     ; Set valid bit

  ;
  ; Program mask register
  ;
  mov     ecx, MTRR_PHYS_MASK_1         ; setup variable mtrr
  movd    ebx, NEXT_MTRR_INDEX
  add     ecx, ebx

  mov     edx, esi                      ; edx <- MTRR_PHYS_MASK_HIGH
  wrmsr
  ;
  ; Program base register
  ;
  sub     edx, edx
  mov     ecx, MTRR_PHYS_BASE_1         ; setup variable mtrr
  add     ecx, ebx                      ; ebx is still NEXT_MTRR_INDEX

  movd    eax, CODE_BASE_TO_CACHE
  or      eax, MTRR_MEMORY_TYPE_WP      ; set type to write protect
  wrmsr
  ;
  ; Advance and loop
  ; Reduce remaining size to cache
  ;
  movd    ebx, CODE_SIZE_TO_CACHE
  movd    eax, NEXT_MTRR_SIZE
  sub     ebx, eax
  movd    CODE_SIZE_TO_CACHE, ebx

  ;
  ; Increment MTRR index
  ;
  movd    ebx, NEXT_MTRR_INDEX
  add     ebx, 2
  movd    NEXT_MTRR_INDEX, ebx
  ;
  ; Increment base address to cache
  ;
  movd    ebx, CODE_BASE_TO_CACHE 
  movd    eax, NEXT_MTRR_SIZE
  add     ebx, eax
  movd    CODE_BASE_TO_CACHE, ebx 

  jmp     NextMtrr

CodeRegionMtrrdone:
;  ; Program the variable MTRR's MASK register for WDB
;  ; (Write Data Buffer, used in MRC, must be WC type)
;  ;
;  mov     ecx, MTRR_PHYS_MASK_1
;  movd    ebx, NEXT_MTRR_INDEX
;  add     ecx, ebx
;  mov     edx, esi                                          ; edx <- MTRR_PHYS_MASK_HIGH
;  mov     eax, WDB_REGION_SIZE_MASK OR MTRR_PHYS_MASK_VALID ; turn on the Valid flag
;  wrmsr

;  ;
;  ; Program the variable MTRR's BASE register for WDB
;  ;
;  dec     ecx
;  xor     edx, edx
;  mov     eax, WDB_REGION_BASE_ADDRESS OR MTRR_MEMORY_TYPE_WC
;  wrmsr

  ;
  ; Enable the MTRRs by setting the IA32_MTRR_DEF_TYPE MSR E flag.
  ;
  mov     ecx, MTRR_DEF_TYPE            ; Load the MTRR default type index
  rdmsr
  or      eax, MTRR_DEF_TYPE_E          ; Enable variable range MTRRs
  wrmsr

  ;
  ;   Enable the logical processor's (BSP) cache: execute INVD and set 
  ;   CR0.CD = 0, CR0.NW = 0.
  ;
  mov     eax, cr0
  and     eax, NOT (CR0_CACHE_DISABLE + CR0_NO_WRITE)
  invd
  mov     cr0, eax
  ;
  ;   Enable No-Eviction Mode Setup State by setting
  ;   NO_EVICT_MODE  MSR 2E0h bit [0] = '1'.
  ;
  mov     ecx, NO_EVICT_MODE 
  rdmsr
  or      eax, 1
  wrmsr

  ;
  ;   One location in each 64-byte cache line of the DataStack region
  ;   must be written to set all cache values to the modified state.
  ;
  mov     edi, PcdGet32 (PcdTemporaryRamBase)
  mov     ecx, PcdGet32 (PcdTemporaryRamSize)
  shr     ecx, 6
  mov     eax, CACHE_INIT_VALUE
@@:
  mov  [edi], eax
  sfence
  add  edi, 64
  loopd  @b

  ;
  ;   Enable No-Eviction Mode Run State by setting
  ;   NO_EVICT_MODE MSR 2E0h bit [1] = '1'.
  ;
  mov     ecx, NO_EVICT_MODE
  rdmsr
  or      eax, 2
  wrmsr

  ;
  ; Finished with cache configuration
  ;
  
  ;
  ; Optionally Test the Region...
  ;  
  
  ;
  ; Test area by writing and reading
  ;
  cld
  mov     edi, PcdGet32 (PcdTemporaryRamBase)
  mov     ecx, PcdGet32 (PcdTemporaryRamSize) 
  shr     ecx, 2
  mov     eax, CACHE_TEST_VALUE
TestDataStackArea:
  stosd
  cmp     eax, DWORD PTR [edi-4]
  jnz     DataStackTestFail
  loop    TestDataStackArea 
  jmp     DataStackTestPass

  ;
  ; Cache test failed
  ;
DataStackTestFail:
  STATUS_CODE (0D0h)
  jmp     $

  ;
  ; Configuration test failed
  ;
ConfigurationTestFailed:
  STATUS_CODE (0D1h)
  jmp     $

DataStackTestPass:

  ;
  ; At this point you may continue normal execution.  Typically this would include 
  ; reserving stack, initializing the stack pointer, etc.
  ;

  ;
  ; After memory initialization is complete, please follow the algorithm in the BIOS
  ; Writer's Guide to properly transition to a normal system configuration.
  ; The algorithm covers the required sequence to properly exit this mode.
  ;

  RET_ESI

InitializeNEM    ENDP

;  STATUS_CODE (09h)
EstablishStack    PROC    NEAR    PRIVATE

  ;
  ; Enable STACK
  ;
  ; To be programmed...
If 0     ; equate not defined
  mov     esp, PcdGet32 (PcdTemporaryRamBase)
  add     esp, PcdGet32 (PcdTemporaryRamSize)
  sub     esp, 4

  ; Pass NEM address into the PEI Core
  ;  push    PhysBase

  push    PcdGet32 (PcdTemporaryRamBase)

  ; Dispatch table

  push  -(LAST_ADDRESS - offset MICROCODE_DISPATCH_DESCRIPTOR)

  ; Pass stack size into the PEI Core
  push    PcdGet32 (PcdTemporaryRamSize)
endif
  RET_ESI

EstablishStack    ENDP

;  STATUS_CODE (0Bh)
CallPeiCoreEntryPoint   PROC    NEAR    PRIVATE
  ;
  ; Set stack top pointer
  ;
  mov     esp, PcdGet32 (PcdTemporaryRamBase)
  add     esp, PcdGet32 (PcdTemporaryRamSize)

  ;
  ; Push CPU count to stack first, then AP's (if there is one)
  ; BIST status, and then BSP's
  ;

  ;
  ; Here work around for BIST
  ; mm1 is not updated with correct BSP count.
  ; setting BSP count to 1.
  ;
  ;movd    ecx, mm1
  mov     ecx, 1

  ; Save number of BSPs
  push  ecx

GetSBSPBist:
  ; Save SBSP BIST 
  movd  eax, mm0 
  push  eax

  ; Save SBSP APIC ID
  movd  eax, mm1 
  shr   eax, BSPApicIDSaveStart               ; Resume APIC ID 
  push  eax

TransferToSecStartup:


  ; Switch to "C" code
  STATUS_CODE (0Ch)
  ;jmp $

  ;
  ; Pass entry point of the PEI core
  ;
  mov     edi, PEI_CORE_ENTRY_BASE      ; 0FFFFFFE0h
  push    DWORD PTR ds:[edi]

  ;
  ; Pass BFV into the PEI Core
  ;
  mov     edi, FV_MAIN_BASE             ; 0FFFFFFFCh
  push    DWORD PTR ds:[edi]

  ; ECPoverride: SecStartup entry point needs 4 parameters
  push    PcdGet32 (PcdTemporaryRamBase)

  ;
  ; Pass stack size into the PEI Core
  ;
  push    PcdGet32 (PcdTemporaryRamSize)

  ;
  ; Pass Control into the PEI Core
  ;
  call SecStartup
CallPeiCoreEntryPoint   ENDP

StartUpAp       PROC    NEAR

  mov     esi, HPET_COMP_2
  lock    inc  byte ptr [esi]

  DISABLE_CACHE
;
; Halt the AP and wait for the next SIPI
;
Ap_Halt:
  cli
@@:
  hlt
  jmp     @B
  ret
StartUpAp       ENDP

    
CheckValidCMOS    PROC    NEAR    PRIVATE
  ;
  ; Check CMOS Status
  ;
  mov     edi, PMC_CFG_BASE + R_PCH_PMC_GEN_PMCON_B
  mov     ax,  WORD PTR [edi]
  
  ; check PWR_FLR and RTC_PWR_STS status 
  and     ax, BIT2
   
  RET_EBP
CheckValidCMOS    ENDP

MtrrInitTable   LABEL BYTE
    DW  MTRR_DEF_TYPE
    DW  MTRR_FIX_64K_00000
    DW  MTRR_FIX_16K_80000
    DW  MTRR_FIX_16K_A0000
    DW  MTRR_FIX_4K_C0000
    DW  MTRR_FIX_4K_C8000
    DW  MTRR_FIX_4K_D0000
    DW  MTRR_FIX_4K_D8000
    DW  MTRR_FIX_4K_E0000
    DW  MTRR_FIX_4K_E8000
    DW  MTRR_FIX_4K_F0000
    DW  MTRR_FIX_4K_F8000

MtrrCountFixed EQU (($ - MtrrInitTable) / 2)

    DW  MTRR_PHYS_BASE_0
    DW  MTRR_PHYS_MASK_0
    DW  MTRR_PHYS_BASE_1
    DW  MTRR_PHYS_MASK_1
    DW  MTRR_PHYS_BASE_2
    DW  MTRR_PHYS_MASK_2
    DW  MTRR_PHYS_BASE_3
    DW  MTRR_PHYS_MASK_3
    DW  MTRR_PHYS_BASE_4
    DW  MTRR_PHYS_MASK_4
    DW  MTRR_PHYS_BASE_5
    DW  MTRR_PHYS_MASK_5
    DW  MTRR_PHYS_BASE_6
    DW  MTRR_PHYS_MASK_6
    DW  MTRR_PHYS_BASE_7
    DW  MTRR_PHYS_MASK_7
    DW  MTRR_PHYS_BASE_8
    DW  MTRR_PHYS_MASK_8
    DW  MTRR_PHYS_BASE_9
    DW  MTRR_PHYS_MASK_9
MtrrCount      EQU (($ - MtrrInitTable) / 2)

align 10h
PUBLIC  BootGDTtable

;
; GDT[0]: 0x00: Null entry, never used.
;
NULL_SEL        EQU $ - GDT_BASE        ; Selector [0]
GDT_BASE:
BootGDTtable        DD  0
                    DD  0
;
; Linear data segment descriptor
;
LINEAR_SEL      EQU $ - GDT_BASE        ; Selector [0x8]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  092h                            ; present, ring 0, data, expand-up, writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; Linear code segment descriptor
;
LINEAR_CODE_SEL EQU $ - GDT_BASE        ; Selector [0x10]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  09Bh                            ; present, ring 0, data, expand-up, not-writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; System data segment descriptor
;
SYS_DATA_SEL    EQU $ - GDT_BASE        ; Selector [0x18]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  093h                            ; present, ring 0, data, expand-up, not-writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0

;
; System code segment descriptor
;
SYS_CODE_SEL    EQU $ - GDT_BASE        ; Selector [0x20]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  09Ah                            ; present, ring 0, data, expand-up, writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; Spare segment descriptor
;
SYS16_CODE_SEL  EQU $ - GDT_BASE        ; Selector [0x28]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0Eh                             ; Changed from F000 to E000.
    DB  09Bh                            ; present, ring 0, code, expand-up, writable
    DB  00h                             ; byte-granular, 16-bit
    DB  0
;
; Spare segment descriptor
;
SYS16_DATA_SEL  EQU $ - GDT_BASE        ; Selector [0x30]
    DW  0FFFFh                          ; limit 0xFFFF
    DW  0                               ; base 0
    DB  0
    DB  093h                            ; present, ring 0, data, expand-up, not-writable
    DB  00h                             ; byte-granular, 16-bit
    DB  0

;
; Spare segment descriptor
;
SPARE5_SEL      EQU $ - GDT_BASE        ; Selector [0x38]
    DW  0                               ; limit 0
    DW  0                               ; base 0
    DB  0
    DB  0                               ; present, ring 0, data, expand-up, writable
    DB  0                               ; page-granular, 32-bit
    DB  0
GDT_SIZE        EQU $ - BootGDTtable    ; Size, in bytes

GdtDesc:                                ; GDT descriptor
OffsetGDTDesc   EQU $ - Flat32Start
    DW  GDT_SIZE - 1                    ; GDT limit
    DD  OFFSET BootGDTtable             ; GDT base address

NemInitLinearAddress   LABEL   FWORD
NemInitLinearOffset    LABEL   DWORD
    DD  OFFSET ProtectedModeSECStart    ; Offset of our 32 bit code
    DW  LINEAR_CODE_SEL

;TopOfCar  DD  DATA_STACK_BASE_ADDRESS + DATA_STACK_SIZE

_TEXT_PROTECTED_MODE    ENDS
END
