#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#/** @file Sb.mak
#    This make file builds south bridge SEC,PEI & DXE components and 
#    link them to respective binary. 
#
#*/

Prepare : $(BUILD_DIR)/SbDxeInitElink.h

$(BUILD_DIR)/SbDxeInitElink.h :  $(BUILD_DIR)\token.mak
	$(ECHO) \
"#define OEM_HDA_VERB_TABLE $(OEM_HDA_VERB_TABLE)$(EOL)\
#define SAVE_RESTORE_CALLBACK_LIST $(SbTimerSaveRestoreRegistersCallbacks)$(EOL)"\
> $@

AMI_CSP_LIB_INCLUDE_FILES := \
 $(SB_INCLUDE_DIR)$(PATH_SLASH)Sb.h \
 $(SB_LIBRARY_INCLUDE_DIR)$(PATH_SLASH)SbCspLib.h \
 $(AmiChipsetPkg_Include_Library_DIR)$(PATH_SLASH)AmiChipsetIoLib.h \
$(AMI_CSP_LIB_INCLUDE_FILES)

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
