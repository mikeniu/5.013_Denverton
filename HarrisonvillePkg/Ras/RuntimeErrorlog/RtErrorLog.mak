Prepare : $(BUILD_DIR)/RtErrorlogElink.h

$(BUILD_DIR)/RtErrorlogElink.h : $(BUILD_DIR)/token.mak
	$(ECHO) \
"#define MEM_ERROR_HANDLER_FUNCTION_LIST $(MemErrorHandlers)$(EOL)\
#define PCI_ERROR_HANDLER_FUNCTION_LIST $(PciErrorHandlers)$(EOL)\
#define CSI_ERROR_HANDLER_FUNCTION_LIST $(CsiErrorHandlers)$(EOL)\
$(EOL)"\
> $@