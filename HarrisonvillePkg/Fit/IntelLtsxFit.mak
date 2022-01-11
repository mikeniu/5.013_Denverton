#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
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
# $Header: /Alaska/Tools/template.mak 6     1/13/10 2:13p Felixp $
#
# $Revision: 6 $
#
# $Date: 1/13/10 2:13p $
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	<ComponentName>.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************

End : RunFitUtility

OUTPUT_DIR = $(BUILD_DIR)\$(PLATFORM_NAME)\$(TARGET)_$(TOOL_CHAIN_TAG)
export OUTPUT_DIR


.PHONY : RunFitUtility
RunFitUtility:
ifeq ("$(target)","MultiPlatformRom")
#	@$(ECHO) No need to Run FIT Utility
else
	@$(ECHO) $(OUTPUT_DIR)
	@$(ECHO) Run FIT Utility
	@copy /Y $(OUTPUT_DIR)\FV\AMIROM.fd $(BIOS_TAG).ROM
	@$(ECHO) $(OUTPUT_DIR)
	@if not exist $(BUILD_DIR)\FitUtility.exe @copy /Y $(FIT_UTILITY) $(BUILD_DIR)\FitUtility.exe
	@$(ECHO) $(OUTPUT_DIR)
#	@if not exist $(BUILD_DIR)\IntelLtsxFit.com @copy /Y $(FIT_FILE) $(BUILD_DIR)\IntelLtsxFit.com
#	@$(ECHO) $(OUTPUT_DIR)

	@$(ECHO) Updating the MICROCODE patch on $(BIOS_TAG).ROM
	$(BUILD_DIR)\FitUtility.exe $(FitTokens)

	@copy /Y $(BIOS_TAG).ROM $(OUTPUT_DIR)\FV\AMIROM.fd

ifeq ($(SecureMod_SUPPORT),1)	
	if exist $(BIOS_TAG).ROM $(RM) $(BIOS_TAG).ROM
endif	
endif

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************