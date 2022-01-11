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
# Revision History
# ----------------
# $Log: /Alaska/Tools/template.mak $
# 
# 6     1/13/10 2:13p Felixp
# 
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	<ComponentName>.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************

Prepare: DeleteRomFile

DeleteRomFile: 
	del /q /f spi_image*.BIN
	if exist $(PROJECT_TAG).BIN $(RM) $(PROJECT_TAG).BIN 
	if exist $(PROJECT_TAG).CAP $(RM) $(PROJECT_TAG).CAP
	if exist $(AMI_ROM) $(RM) $(AMI_ROM)	

End : CreateBinFile 

OUTPUT_DIR = $(BUILD_DIR)\$(PLATFORM_NAME)\$(TARGET)_$(TOOL_CHAIN_TAG)
export OUTPUT_DIR
export CRB_PLATFORM_SELECT
export CML_GBE_NCSI_SUPPORT

.PHONY : CreateBinFile 
CreateBinFile:
	@$(ECHO) Generating spi image
ifeq ($(SecureMod_SUPPORT),1)
ifeq ($(FWCAPSULE_FILE_FORMAT),0)
	@copy /Y $(FWCAPSULE_FILE_NAME) $(OUTPUT_DIR)\FV\AMIROM.fd
endif
else
	@copy /Y $(OUTPUT_DIR)\FV\AMIROM.fd $(AMI_ROM)
endif
	@call HarrisonvillePkg\BuildImage.bat
ifeq ($(CRB_PLATFORM_SELECT),1)
	$(CP) $(OUTPUT_DIR)\ROM\spi_image_HV.bin spi_image_HV.bin
endif	
	
ifeq ($(CRB_PLATFORM_SELECT),2)
	$(CP) $(OUTPUT_DIR)\ROM\spi_image_CL.bin spi_image_CL.bin
endif

ifeq ($(CRB_PLATFORM_SELECT),3)
	$(CP) $(OUTPUT_DIR)\ROM\spi_image_AC.bin spi_image_AC.bin
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
