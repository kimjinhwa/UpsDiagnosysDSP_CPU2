################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
fatfs/src/%.obj: ../fatfs/src/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-c2000_20.2.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --include_path="C:/ti/C2000Ware_3_04_00_00_Software/device_support/f2837xd/headers/include" --include_path="C:/ti/C2000Ware_3_04_00_00_Software/libraries/flash_api/f2837xd/include" --include_path="C:/ti/C2000Ware_3_04_00_00_Software/device_support/f2837xd/common/include" --include_path="E:/ti/workspaceV1029/DiagnosysUps_CPU02" --include_path="E:/ti/workspaceV1029/DiagnosysUps_CPU02/device" --include_path="C:/ti/C2000Ware_3_04_00_00_Software/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-c2000_20.2.2.LTS/include" --define=_FLASH --define=CPU2 --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="fatfs/src/$(basename $(<F)).d_raw" --include_path="E:/ti/workspaceV1029/DiagnosysUps_CPU02/CPU2_FLASH/syscfg" --obj_directory="fatfs/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


