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

#/** @file Nb.mak
#    This make file builds north bridge SEC,PEI & DXE components and 
#    link them to respective binary. 
#
#*/

Prepare : 

AMI_CSP_LIB_INCLUDE_FILES := \
 $(NB_INCLUDE_DIR)$(PATH_SLASH)Nb.h \
 $(NB_LIBRARY_INCLUDE_DIR)$(PATH_SLASH)NbCspLib.h \
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
