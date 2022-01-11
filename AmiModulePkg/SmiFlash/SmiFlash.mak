#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#**********************************************************************
# $Header: $
#
# $Revision: $
#
# $Date: $
#**********************************************************************
# Revision History
# ----------------
# $Log: $
# 
# 
#**********************************************************************


#<AMI_FHDR_START>
#-----------------------------------------------------------------------
#
# Name:			SMIFlash.mak
#
# Description:	
#				
#
#-----------------------------------------------------------------------
#<AMI_FHDR_END>
#**********************************************************************
Prepare : $(BUILD_DIR)/SMIFlashELinks.h

$(BUILD_DIR)/SMIFlashELinks.h :  $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#define SMIFLASH_IN_SMM_LIST $(SMIFlashInSmmList)$(EOL)\
#define SMIFLASH_NOT_IN_SMM_LIST $(SMIFlashNotInSmmList)$(EOL)\
#define SMIFLASH_END_UPDATE_LIST $(SMIFlashEndUpdateList)$(EOL)\
#define SMIFLASH_PRE_UPDATE_LIST $(SMIFlashPreUpdateList)$(EOL)\
#define SMIFLASH_PRE_HANDLER_LIST $(SMIFlashPreHandlerList)$(EOL)\
#define SMIFLASH_END_HANDLER_LIST $(SMIFlashEndHandlerList)$(EOL)\
#define SMIFLASH_SAVE_VARS_LIST $(SMIFlashSaveVarsList)$(EOL)\
#define SMIFLASH_SAVE_GUIDS_LIST $(SMIFlashSaveGuidsList)$(EOL)"\
> $(BUILD_DIR)/SMIFlashELinks.h

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
