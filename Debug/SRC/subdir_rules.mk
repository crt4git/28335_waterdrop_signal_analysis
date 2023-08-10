################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
SRC/%.obj: ../SRC/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/APP/CCS9/ccs/tools/compiler/ti-cgt-c2000_18.12.4.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="D:/DATA/CCS9Workspace/AD_DOUBLE" --include_path="D:/DATA/CCS9Workspace/AD_DOUBLE/CMD" --include_path="D:/DATA/CCS9Workspace/AD_DOUBLE/INCLUDE" --include_path="D:/APP/CCS9/ccs/tools/compiler/ti-cgt-c2000_18.12.4.LTS/include" --include_path="D:/DATA/CCS9Workspace/AD_DOUBLE/SRC" --include_path="D:/APP/CCS9/ccs/tools/compiler/ti-cgt-c2000_18.12.4.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="SRC/$(basename $(<F)).d_raw" --obj_directory="SRC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SRC/%.obj: ../SRC/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/APP/CCS9/ccs/tools/compiler/ti-cgt-c2000_18.12.4.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="D:/DATA/CCS9Workspace/AD_DOUBLE" --include_path="D:/DATA/CCS9Workspace/AD_DOUBLE/CMD" --include_path="D:/DATA/CCS9Workspace/AD_DOUBLE/INCLUDE" --include_path="D:/APP/CCS9/ccs/tools/compiler/ti-cgt-c2000_18.12.4.LTS/include" --include_path="D:/DATA/CCS9Workspace/AD_DOUBLE/SRC" --include_path="D:/APP/CCS9/ccs/tools/compiler/ti-cgt-c2000_18.12.4.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="SRC/$(basename $(<F)).d_raw" --obj_directory="SRC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


