/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

/*++

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  LegacyRegion.c

Abstract:
  
  This code provides a private implementation of the Legacy Region protocol.
  
--*/

#include "LegacyRegion.h"
#include "SaRegs.h"
#include "SaAccess.h"
#include <Library/PciLib.h>
#include <Library/UefiLib.h>
#include <SimicsDebug.h>

EFI_HANDLE   mLegacyRegion2Handle = NULL;
EFI_LEGACY_REGION2_PROTOCOL  mLegacyRegion2 = {
  LegacyRegionDecode,
  LegacyRegionLock,
  LegacyRegionBootLock,
  LegacyRegionUnlock,
  LegacyRegionGetInfo
};

// MchBar Address needed to access IMR Registers
UINT64 mMchBar;

// LegacyRegion Driver Internal Structures
// Internal Map of IMRs used for legacy region descriptors 
UINT8 mImrMap[MAX_LEGACY_REGIONS]; 

EFI_LEGACY_REGION_DESCRIPTOR mLegacyRegionDescriptor[MAX_LEGACY_REGIONS];

/*++

Routine Description:
  Find available IMRs for legacy regions 
  Assumption: IMR is available if BIMR Low and BIMR High registers are 0
  Denverton has 20 IMR regions, do linear search

Arguments:
  OUT   ImrNum   On success, contains number of the free IMR to use

Returns: 

  EFI_SUCCESS     - IMR available returned in ImrNum
  EFI_UNSUPPORTED - No IMRs available

--*/
static
EFI_STATUS
GetNextFreeImr (OUT UINT8 *ImrNum)
{
  static UINT8 Next = 1;
  UINT8  i;
  UINT32 IMRBase;
  UINT32 IMRMask;

  if (ImrNum == NULL){
	DEBUG ( (EFI_D_INFO, "ImrNum = NULL..\n") );
	return EFI_INVALID_PARAMETER;
  }

  DEBUG ( (EFI_D_INFO, "GetNextFreeImr called..\n") );

  for (i=Next;i < N_SA_MCHBAR_BIMR_NUM_OF_IMRS;i++) {
    IMRBase = MmioRead32 ((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0BASE + (i * N_SA_MCHBAR_BIMR_OFFSET)));
    IMRMask = MmioRead32 ((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0MASK + (i * N_SA_MCHBAR_BIMR_OFFSET)));
    DEBUG ( (EFI_D_INFO, "GetNextFreeImr IMRBase = 0x%x IMRMask=0x%x..\n",IMRBase,IMRMask) );
    IMRBase &= B_SA_MCHBAR_BIMR_ADDRESS_MASK;
    IMRMask &= B_SA_MCHBAR_BIMR_ADDRESS_MASK;
    if ( IMRBase == 0 && IMRMask == 0) {
      *ImrNum = i++;
      Next = i;
      DEBUG ( (EFI_D_INFO, "GetNextFreeImr returning %d, next %d..\n", *ImrNum, Next) );
      return EFI_SUCCESS;
    }
  }
  return EFI_UNSUPPORTED;
}

/*++
Routine Description:
  Setup an IMR entry for the legacy segment specified

Arguments:
  Index  - Map entry to configure
  Start  - Starting address of region.
  Length - Length of region in bytes.

Returns: 

  EFI_SUCCESS - IMR allocated successfully

--*/
static
EFI_STATUS
SetupImrMap (
  IN  UINT8                       Index,
  IN  UINT32                      Start,
  IN  UINT32                      Length
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8  ImrNum;
  UINT32 IMRBase;
  UINT32 IMRMask;
  UINT64 Data;
  UINT64 DataCheck;

  DEBUG ( (EFI_D_INFO, "SetupImrMap called..(%d, 0x%x, 0x%x) \n",Index, Start, Length) );

  if (Index >= (MAX_LEGACY_REGIONS/NUM_SEGMENTS)) {
    DEBUG ( (EFI_D_INFO, "Wrong index region: %x\n",Index) );
    return EFI_INVALID_PARAMETER;
  }

  Status = GetNextFreeImr (&ImrNum);
  if (EFI_ERROR (Status)) {    
    mImrMap[Index] = mImrMap[Index+WRITE_REGION_OFFSET] = mImrMap[Index+BOOTLOCK_REGION_OFFSET] = IMR_INVALID;
    // No IMR available for the legacy segment
    return Status;
  }  

  mImrMap[Index] = mImrMap[Index+WRITE_REGION_OFFSET] = mImrMap[Index+BOOTLOCK_REGION_OFFSET] = ImrNum;

  IMRBase = IMRMask = 0;
  // Setup IMR
  IMRBase = (Start >> 10) | BIT31;
  IMRMask = (~((Length>>10)-1) ) & B_SA_MCHBAR_BIMR_ADDRESS_MASK;


  IMRMask |= BUNIT_BIMRH_ENABLE_ALL;

  DEBUG ( (EFI_D_INFO, "SetupImrMap IMRBase = 0x%x IMRMask=0x%x..\n",IMRBase,IMRMask) );

  // Write Base register (0x6870 + offset to IMR Index)
  MmioWrite32 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0BASE + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), IMRBase);
  Data = MmioRead32 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0BASE + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))));
  DEBUG ( (EFI_D_INFO, "SetupImr%d Map set 0x%x = 0x%x?=0x%x\n", ImrNum, (R_SA_MCHBAR_BIMR0BASE + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), IMRBase, (UINT32)(Data)));
  ASSERT(IMRBase==Data);
  
  // Write Mask register (0x6874 + offset to IMR Index)
  MmioWrite32 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0MASK + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), IMRMask);
  Data = MmioRead32 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0MASK + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))));
  DEBUG ( (EFI_D_INFO, "SetupImr%d Mask set 0x%x = 0x%x?=0x%x\n", ImrNum,(R_SA_MCHBAR_BIMR0MASK + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), IMRMask, (UINT32)(Data)));
  ASSERT(IMRMask==Data);

  // Disable READ
  Data = V_SA_MCHBAR_BIMRRAC_DISABLE;
  MmioWrite64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), Data);
  DataCheck = MmioRead64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))));
  DEBUG ( (EFI_D_INFO, "SetupImr%d RAC set 0x%x = 0x%lx?=0x%lx\n", ImrNum,(R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), Data, DataCheck));
  ASSERT(Data==DataCheck);

  // Disable WRITE
  Data = V_SA_MCHBAR_BIMRWAC_DISABLE;
  MmioWrite64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), Data);
  DataCheck = MmioRead64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))));
  DEBUG ( (EFI_D_INFO, "SetupImr%d WAC set 0x%x = 0x%x?=0x%lx\n", ImrNum,(R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), Data, Data, DataCheck));
  ASSERT(Data==DataCheck);

  // We have to Enable IA_UNTRUSTED mode in IMR1CP reg only for successfull LegacyRegion manipulation.
  //Callback "Imr1ConfigOnExitBootServicies" on ExitBootServicies will relax this bit
  Data = MmioRead64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0CP + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))));
  DEBUG ( (EFI_D_INFO, "SetupImrMap BIMR%dCP 0x%x = 0x%x..", ImrNum,(R_SA_MCHBAR_BIMR0CP + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), Data) );
  Data |= V_SA_MCHBAR_BIMRCP_ENABLE_IA_UNSTRUSTED; //enable bit 0 for IA_Untrusted mode
  MmioWrite64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0CP + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), Data);
  DEBUG ( (EFI_D_INFO, "<= 0x%x ", DataCheck) );
  DataCheck = MmioRead64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0CP + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))));
  DEBUG ( (EFI_D_INFO, "?= 0x%x..\n", DataCheck) );
  ASSERT(Data==DataCheck);

  DEBUG ( (EFI_D_INFO, "SetupImrMap finished - ImrNum %d\n",ImrNum) );

  return EFI_SUCCESS;
}

static
EFI_STATUS
SetupSegmentRegionDescriptors (
  IN  UINT8                       Index,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  IN  UINT32                      Granularity
)
/*++
Routine Description:
  Setup internal region descriptors array entry for the legacy segment specified

Arguments:
  Index  - Map entry to configure
  Start  - Starting address of region.
  Length - Length of region in bytes.
  Granularity - Granularity of region in bytes.

Returns:

  EFI_SUCCESS - IMR allocated successfully

--*/
{

  // Initialize DECODE Region Descriptor
  mLegacyRegionDescriptor[Index].Start = Start;
  mLegacyRegionDescriptor[Index].Length = Length;
  mLegacyRegionDescriptor[Index].Attribute = LegacyRegionNotDecoded;
  mLegacyRegionDescriptor[Index].Granularity = Granularity;

  // Initialize WRITE Region Descriptor
  mLegacyRegionDescriptor[Index + WRITE_REGION_OFFSET].Start = Start;
  mLegacyRegionDescriptor[Index + WRITE_REGION_OFFSET].Length = Length;
  mLegacyRegionDescriptor[Index + WRITE_REGION_OFFSET].Attribute = LegacyRegionWriteDisabled;
  mLegacyRegionDescriptor[Index + WRITE_REGION_OFFSET].Granularity = Granularity;

  // Initialize BOOTLOCK Region Descriptor
  mLegacyRegionDescriptor[Index + BOOTLOCK_REGION_OFFSET].Start = Start;
  mLegacyRegionDescriptor[Index + BOOTLOCK_REGION_OFFSET].Length = Length;
  mLegacyRegionDescriptor[Index + BOOTLOCK_REGION_OFFSET].Attribute = LegacyRegionNotLocked;
  mLegacyRegionDescriptor[Index + BOOTLOCK_REGION_OFFSET].Granularity = Granularity;

  // Finally configure IMR for segment
  return SetupImrMap(Index,Start,Length);
}

/*++

Routine Description:
  Given a region address and mode, if valid return the internal index to the region required

Arguments:

  Start   - Address of region required
  Mode    - Action to perform on an IMR region, use to find appropriate offset within internal descriptor map
  Index   - Internal index of region

Returns:

  EFI_SUCCESS - Address of IMR region specified is valid

--*/
static
EFI_STATUS
GetLegacyRegion(
  IN  UINT32      Start,
  IN  EFI_LEGACY_REGION_ATTRIBUTE Mode,
  OUT UINT8      *Index
)
{
  UINT8 i,RegionStart=0;
  DEBUG ( (EFI_D_INFO, "GetLegacyRegion called..(0x%x) \n",Start) );
  // Given Mode, begin search in correct region map
  switch (Mode) {
    case LegacyRegionDecoded:
    case LegacyRegionNotDecoded:
      RegionStart=DECODE_REGION_OFFSET;
      break;
    case LegacyRegionWriteEnabled:
    case LegacyRegionWriteDisabled:
      RegionStart=WRITE_REGION_OFFSET;
      break;
    case LegacyRegionBootLocked:
    case LegacyRegionNotLocked:
      RegionStart=BOOTLOCK_REGION_OFFSET;
      break;
  }
  for (i=RegionStart;i < RegionStart+NUM_SEGMENTS;i++) {
    if ((Start >= mLegacyRegionDescriptor[i].Start) &&
        (Start <= (mLegacyRegionDescriptor[i].Start + mLegacyRegionDescriptor[i].Length - 1))) {
        *Index = i;
        DEBUG ( (EFI_D_INFO, "GetLegacyRegion returning %d \n",i) );
        return EFI_SUCCESS;
    }
  }
  DEBUG ( (EFI_D_INFO, "GetLegacyRegion failure\n") );
  return EFI_UNSUPPORTED;
}

/*++

Routine Description:
  Called by ConfigureRegion function, checks whether that region is in BOOTLOCKED state already
  Find respective region descriptor in BOOTLOCK_REGION map, and check it's state.

Arguments:

  Index   - Internal index to a region to configure

Returns:

  TRUE  - Region already locked
  FALSE - Region not locked

--*/
static
BOOLEAN
VerifyBootLocked(
  IN UINT8 Index
  )
{
  UINT8 i;

  // Given Index, convert to it's corresponding descriptor in BOOT_LOCK (6/7/8) map
  i = BOOTLOCK_REGION_OFFSET + (Index % NUM_SEGMENTS);
  if (mLegacyRegionDescriptor[i].Attribute == LegacyRegionNotLocked)  {
    return FALSE;
  }
  return TRUE;
}
/*++

Routine Description:
  Called by LegacyRegionManipulation function, configure the Legacy region to the mode required

Arguments:

  Index   - Internal index to region to configure

  Mode    - Action to perform on a IMR region

Returns:

  EFI_SUCCESS - IMR region state modified as requested.

--*/
static
EFI_STATUS
ConfigureRegion(
  IN UINT8 Index,
  IN EFI_LEGACY_REGION_ATTRIBUTE Mode
  )
{
  UINT64 Data,DataCheck;
  UINT8  ImrNum;

  if (Index >= MAX_LEGACY_REGIONS) return EFI_UNSUPPORTED;

  DEBUG ( (EFI_D_INFO, "ConfigureRegion called..Index = %d, Mode = %d\n",Index,Mode) );
  // If descriptor already BOOTLOCKED then no changes allowed/permitted (See PI Spec)
  if (VerifyBootLocked(Index)) {
    return EFI_SUCCESS;
  }

  /* Store Mode */
  mLegacyRegionDescriptor[Index].Attribute = Mode;

  // Setup IMR
  ImrNum = mImrMap[Index];
  if (ImrNum == IMR_INVALID) {
    // NO IMRs were available, no need to continue
    return EFI_SUCCESS;
  }
  switch (Mode) {
    case LegacyRegionDecoded: /* turn on read */
      Data = BUNIT_BIMR_RAC_READ_OPEN_FOR_ALL_CORE | V_SA_MCHBAR_BIMRCP_ENABLE_IA_UNSTRUSTED;
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: Turn ON Read\n") );
      MmioWrite64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), Data);
      DataCheck = MmioRead64 ((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)));
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: set 0x%x = 0x%x..?=0x%x\n",(R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), Data, DataCheck) );
      break;
    case LegacyRegionNotDecoded: /* turn off read */
      Data = BUNIT_BIMR_RAC_READ_DISABLED;
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: Turn OFF Read\n") );
      MmioWrite64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), Data);
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: set 0x%x = 0x%x..\n",(R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), Data) );
      break;
    case LegacyRegionWriteEnabled:
      Data = BUNIT_BIMR_WAC_WRITE_OPEN_FOR_ALL_CORE | V_SA_MCHBAR_BIMRCP_ENABLE_IA_UNSTRUSTED;
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: Turn ON Write\n") );
      MmioWrite64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), Data);
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: set 0x%x = 0x%x..\n",(R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), Data) );

      DataCheck = MmioRead64 ((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)));
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: verify 0x%x = 0x%x..\n",(R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)),DataCheck) );
      ASSERT((DataCheck & BUNIT_BIMR_WAC_WRITE_OPEN_FOR_ALL_CORE) == BUNIT_BIMR_WAC_WRITE_OPEN_FOR_ALL_CORE);
      break;
    case LegacyRegionWriteDisabled:
      Data = BUNIT_BIMR_WAC_WRITE_DISABLED;
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: Turn OFF Write\n") );
      MmioWrite64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), Data);
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: set 0x%x = 0x%x..\n",(R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), Data) );

      DataCheck = MmioRead64 ((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)));
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: verify 0x%x = 0x%x..\n",(R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)),DataCheck) );
      break;

    case LegacyRegionBootLocked:
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: BOOT LOCK index = %d START\n",Index) );
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: Turn ON Read\n") );
      Data = BUNIT_BIMR_RAC_READ_OPEN_FOR_ALL_CORE | V_SA_MCHBAR_BIMRCP_ENABLE_IA_UNSTRUSTED;
      MmioWrite64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), Data);
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: set 0x%x = 0x%x..\n",(R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), Data) );

      DataCheck = MmioRead64 ((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)));
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: verify 0x%x = 0x%x..\n",(R_SA_MCHBAR_BIMR0RAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)),DataCheck) );

      Data = BUNIT_BIMR_WAC_WRITE_DISABLED;
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: Turn OFF Write\n") );
      MmioWrite64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), Data);
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: set 0x%x = 0x%x..\n",(R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), Data) );

      DataCheck = MmioRead64 ((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)));
      DEBUG ( (EFI_D_INFO, "ConfigureRegion: verify 0x%x = 0x%x..\n",(R_SA_MCHBAR_BIMR0WAC + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)),DataCheck) );

      // Disable CP reg access
      DataCheck = MmioRead64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0CP + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))));
      DEBUG ( (EFI_D_INFO, "ConfigureRegion BIMR%dCP 0x%x = 0x%x..", ImrNum,(R_SA_MCHBAR_BIMR0CP + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET)), DataCheck) );

      DataCheck &= BUNIT_BIMRCP_DISABLE_ALL;
      MmioWrite64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0CP + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))), DataCheck);
      DEBUG ( (EFI_D_INFO, "<= 0x%x..", DataCheck) );

      DataCheck = MmioRead64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR0CP + (ImrNum * N_SA_MCHBAR_BIMR_OFFSET))));
      DEBUG ( (EFI_D_INFO, "?= 0x%x..\n", DataCheck) );
      break;
  }

  DEBUG ( (EFI_D_INFO, "ConfigureRegion finished\n") );
  return EFI_SUCCESS;
}

/*++

Routine Description:
  Modify IMR registers for region specified to MODE state.

Arguments:

  This    - Pointer to EFI_LEGACY_REGION_PROTOCOL instance.

  Start   - Starting address of a memory region within range C0000h - FFFFFh.

  Length  - Memory region length.

  Mode    - Action to perform on a IMR region

  Granularity - Granularity of region in bytes.

Returns: 

  EFI_SUCCESS - IMR region(s) state modified as requested.

--*/
EFI_STATUS
LegacyRegionManipulation (
  IN  EFI_LEGACY_REGION2_PROTOCOL *This,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  IN  EFI_LEGACY_REGION_ATTRIBUTE Mode,
  OUT UINT32                      *Granularity  OPTIONAL
  )
{
  EFI_STATUS Status = EFI_SUCCESS;  
  UINT8      Index;
  UINT32     TempAddr;

  DEBUG ( (EFI_D_INFO, "LegacyRegionManipulation called..(0x%p, 0x%x, %d) \n",Start,Length,Mode) );

  // Find region descriptor index from internal table
  Status = GetLegacyRegion(Start, Mode, &Index);
  if (EFI_ERROR (Status)) {
    return Status;
  }  

  DEBUG ( (EFI_D_INFO, "LegacyRegionManipulation Index=%d\n",Index) );
  
  // Configure all descriptor regions within the address range required
  TempAddr = mLegacyRegionDescriptor[Index].Start;
  while (TempAddr < (Start + Length -1)) {
    DEBUG ( (EFI_D_INFO, "LegacyRegionManipulation TempAddr = 0x%x Index=%d\n",TempAddr, Index) );

    ConfigureRegion(Index,Mode);

    TempAddr += mLegacyRegionDescriptor[Index].Granularity;

  if (Granularity) {
      *Granularity = mLegacyRegionDescriptor[Index].Granularity;
    }
    Index++;
  }
  DEBUG ( (EFI_D_INFO, "LegacyRegionManipulation finished..\n") );
  return EFI_SUCCESS;
}

/*++
Routine Description:
  Enable/Disable decoding of the given region

Arguments:
  Start  - Starting address of region.
  Length - Length of region in bytes.
  On     - 0 = Disable decoding, 1 = Enable decoding.

Returns:

  EFI_SUCCESS - Decoding change affected.

--*/
EFI_STATUS
EFIAPI
LegacyRegionDecode (
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity,
  IN  BOOLEAN                      *On
  )
{
  EFI_STATUS Status;

  if (!On) {
    return EFI_INVALID_PARAMETER;
  }

  if (*On == TRUE) {
    Status = LegacyRegionManipulation (This, Start, Length, LegacyRegionDecoded, Granularity);
  } else {
    Status = LegacyRegionManipulation (This, Start, Length, LegacyRegionNotDecoded, Granularity);
  }

  return Status;
}

/*++

Routine Description:
  Make the indicated region read from RAM / write to ROM.

Arguments:
  Start       - Starting address of region.
  Length      - Length of region in bytes.
  Granularity - Granularity of region in bytes.

Returns:

  EFI_SUCCESS - Region locked or made R/O.

--*/
EFI_STATUS
EFIAPI
LegacyRegionBootLock (
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity  OPTIONAL
  )
{
  return LegacyRegionManipulation (This, Start, Length, LegacyRegionBootLocked, Granularity);
}

/*++

Routine Description:
  Make the indicated region read from RAM / write to ROM.

Arguments:
  Start       - Starting address of region.
  Length      - Length of region in bytes.
  Granularity - Granularity of region in bytes.

Returns:

  EFI_SUCCESS - Region locked or made R/O.

--*/
EFI_STATUS
EFIAPI
LegacyRegionLock (
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity  OPTIONAL
  )

{
  return LegacyRegionManipulation (This, Start, Length, LegacyRegionWriteDisabled, Granularity);
}

/*++

Routine Description:
  Make the indicated region read from RAM / write to RAM.

Arguments:
  Start       - Starting address of region.
  Length      - Length of region in bytes.
  Granularity - Granularity of region in bytes.

Returns:

  EFI_SUCCESS - Region unlocked or made R/W.

--*/
EFI_STATUS
EFIAPI
LegacyRegionUnlock (
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity  OPTIONAL
  )

{
  return LegacyRegionManipulation(This, Start, Length, LegacyRegionWriteEnabled, Granularity);
}
/*++

Routine Description:
  Return the information about the legacy region.

Arguments:
  DescriptorCount - Number of descriptors.
  Descriptor      - Descriptor of the legacy region.

Returns: 

  EFI_SUCCESS - Region descriptor is returned.

--*/
EFI_STATUS
EFIAPI
LegacyRegionGetInfo (
  IN  EFI_LEGACY_REGION2_PROTOCOL   *This,
  OUT UINT32                        *DescriptorCount,
  OUT EFI_LEGACY_REGION_DESCRIPTOR  **Descriptor
  )
{
  if (!DescriptorCount || !Descriptor) {
    return EFI_INVALID_PARAMETER;
  }

  *DescriptorCount = sizeof(mLegacyRegionDescriptor) / sizeof(EFI_LEGACY_REGION_DESCRIPTOR);
  *Descriptor      = mLegacyRegionDescriptor;

  return EFI_SUCCESS;
}

/*++

Routine Description:
  Callback on ReadyToBoot event. This function will relax IA_UNTRUSTED bit for IMR1CP register.

  Reason why this CallBack function was registered - IMR1 Region that covers LegacyRegion that must be unlocked for OpRom loads (leaved like this at DXE)
   and must be locked once OpRom are loaded.
--*/


VOID
EFIAPI
Imr1ConfigOnExitBootServicies(
		IN EFI_EVENT    Event,
		IN VOID         *Context
		)
{
  UINT64 Data;
  UINT64 DataCheck;

  if (Event != NULL) {
	  gBS->CloseEvent (Event);
  } else {
	  DEBUG ( (EFI_D_ERROR, "Event is NULL!\n") );
	  CpuDeadLoop();
  }

  Data = MmioRead64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR1CP)));
  Data &= (~(V_SA_MCHBAR_BIMRCP_ENABLE_IA_UNSTRUSTED | V_SA_MCHBAR_BSMR_SMM_OPEN_FOR_IA));
  MmioWrite64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR1CP)), Data);
  DataCheck = MmioRead64 (((UINTN)(mMchBar + R_SA_MCHBAR_BIMR1CP)));

  if(Data != DataCheck) {
	  DEBUG ( (EFI_D_ERROR, "Can't change IMR1CP register\n") );
	  CpuDeadLoop();
  }
}

/*++

Routine Description:
  Install Driver to produce Legacy Region protocol.

Arguments:
  (Standard EFI Image entry - EFI_IMAGE_ENTRY_POINT)

Returns:

  EFI_SUCCESS - Legacy Region protocol installed

  Other       - No protocol installed, unload driver.

--*/
EFI_STATUS
LegacyRegionInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  UINTN  McD0BaseAddress;
  UINT32 Data;
  EFI_STATUS Status;
  EFI_EVENT ExitBootServicesEvent;

  //
  // Workaround
  // In internal builds the cores do not have rights to change the IMR registers. It is assumed
  // that this is because internal builds use a very old PMC version that doesn't support core
  // access to any IMR beyond IMR0. THis workaround can be removed once the reason for limited 
  // IMR access is fixed
  //
  #if !defined(SPS_SUPPORT) || (SPS_SUPPORT == 0)
  {
      volatile UINT8 i = 1;
      DEBUG ( (EFI_D_ERROR, "internal builds - IMR CP register is not changable (see PMC)\n") );
      if (i!=0) return EFI_SUCCESS;
  }
  #endif //SPS_SUPPORT

  if (DXE_IN_SIMICS) {
    DEBUG ( (EFI_D_ERROR, "SIMICS IMR not available\n") );
    return EFI_SUCCESS;
  }
  
  McD0BaseAddress = MmPciBase (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN);

  mMchBar = MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_LO) & ~BIT0;
#ifndef FSP_FLAG
  mMchBar   += (UINT64)LShiftU64(MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_HI), 32);
#endif

  // C/D/E/F Segment
  Status = SetupSegmentRegionDescriptors(CDEF_SEGMENT,0xc0000,0x40000,64 * 1024 *4);
  if (EFI_ERROR(Status)) {
    DEBUG ( (EFI_D_ERROR, "IMR not available\n") );
  }

  //Ensure E & F segments are routed to DRAM
  Data = MmioRead32 ((UINTN)(mMchBar + R_SA_MCHBAR_BMISC));
  Data |= (V_SA_MCHBAR_BMISC_RESDRAM | V_SA_MCHBAR_BMISC_RFSDRAM);
  MmioWrite32 ((UINTN)(mMchBar + R_SA_MCHBAR_BMISC), Data);
  DEBUG ( (EFI_D_ERROR, "BMISC(%x) %x ?= %x\n", (mMchBar + R_SA_MCHBAR_BMISC), Data, (MmioRead32 ((UINTN)(mMchBar + R_SA_MCHBAR_BMISC)))) );

  mLegacyRegion2.Decode            = LegacyRegionDecode;
  mLegacyRegion2.Lock              = LegacyRegionLock;
  mLegacyRegion2.BootLock          = LegacyRegionBootLock;
  mLegacyRegion2.UnLock            = LegacyRegionUnlock;
  mLegacyRegion2.GetInfo           = LegacyRegionGetInfo;

  //
  // Register a Ready to boot event to Relax IA_UNTRUSTED mode in IMR1CP Reg and Assign 0 to IMR1WAC
  //

  Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
					Imr1ConfigOnExitBootServicies,
                    NULL,
                    &gEfiEventExitBootServicesGuid,
                    &ExitBootServicesEvent
                    );

  //
  // Make a new handle and install the protocol
  //
  return gBS->InstallProtocolInterface (
                &mLegacyRegion2Handle,
                &gEfiLegacyRegion2ProtocolGuid, 
                EFI_NATIVE_INTERFACE,
                &mLegacyRegion2
                );

}
