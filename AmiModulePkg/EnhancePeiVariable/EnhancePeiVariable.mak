Prepare : $(BUILD_DIR)/EnhancePeiVariable.inf

$(BUILD_DIR)/EnhancePeiVariable.inf : $(BUILD_DIR)/Token.h $(EnhancePeiVariable_DIR)/EnhancePeiVariable.mak
	$(ECHO) \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = EnhancePeiVariable$(EOL)\
  FILE_GUID                      = E03E6451-297A-4FE9-B1F7-639B70327C52$(EOL)\
  MODULE_TYPE                    = PEIM$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  PI_SPECIFICATION_VERSION       = 0x0001000A$(EOL)\
  ENTRY_POINT                    = EnhancePeiVariableEntry$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  ../$(subst \,/,$(EnhancePeiVariable_DIR))/EnhancePeiVariable.c$(EOL)\
  ../$(subst \,/,$(EnhancePeiVariable_DIR))/EnhancePeiVariable.h$(EOL)\
  ../$(subst \,/,$(EnhancePeiVariable_DIR))/CacheVariablePei/CacheVariablePei.c$(EOL)\
  ../$(subst \,/,$(EnhancePeiVariable_DIR))/CacheVariablePei/CacheVariablePei.h$(EOL)\
  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(ENHANCE_PEI_VARIABLE_SOURCES)))$(EOL)\
$(EOL)\
[Packages]$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
$(EOL)\
[LibraryClasses]$(EOL)\
  PeimEntryPoint$(EOL)\
  PeiServicesLib$(EOL)\
  HobLib$(EOL)\
  AmiPeiLib$(EOL)\
$(EOL)\
[Ppis]$(EOL)\
  gEfiPeiReadOnlyVariable2PpiGuid$(EOL)\
  gEfiPeiMemoryDiscoveredPpiGuid$(EOL)\
$(EOL)\
[Depex]$(EOL)\
  TRUE$(EOL)"\
> $(BUILD_DIR)/EnhancePeiVariable.inf