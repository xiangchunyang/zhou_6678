################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
SRIO_MSM_init.obj: ../SRIO_MSM_init.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="SRIO_MSM_init.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SRIO_TransmitToFPGA.obj: ../SRIO_TransmitToFPGA.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="SRIO_TransmitToFPGA.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SRIO_loopbackDioIsr.obj: ../SRIO_loopbackDioIsr.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="SRIO_loopbackDioIsr.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SRIO_loopbackDioIsr_osal.obj: ../SRIO_loopbackDioIsr_osal.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="SRIO_loopbackDioIsr_osal.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

client.obj: ../client.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="client.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1943613617:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-1943613617-inproc

build-1943613617-inproc: ../clientAndSRIO.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/xdctools_3_23_04_60/xs" --xdcpath="C:/ti/pdk_C6678_1_1_2_6/packages;C:/ti/ipc_1_24_03_32/packages;C:/ti/ndk_2_21_02_43/packages;C:/ti/ccsv8/ccs_base;C:/ti/dsplib_c66x_3_4_0_0/packages;C:/ti/mathlib_c66x_3_1_2_1/packages;C:/ti/bios_6_33_06_50/packages;C:/ti/mcsdk_2_01_02_06/demos;C:/ti/xdctools_3_23_04_60/packages/ti/platforms/my6678EVMhxl;" xdc.tools.configuro -o configPkg -t ti.targets.elf.C66 -p ti.platforms.my6678EVMhxl -r debug -c "C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-1943613617 ../clientAndSRIO.cfg
configPkg/compiler.opt: build-1943613617
configPkg/: build-1943613617

device_srio_loopback.obj: ../device_srio_loopback.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="device_srio_loopback.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

idleFunctions_appuser.obj: ../idleFunctions_appuser.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="idleFunctions_appuser.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

platform_osal.obj: ../platform_osal.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="platform_osal.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

processTask.obj: ../processTask.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="processTask.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

resourcemgr.obj: ../resourcemgr.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="resourcemgr.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

srio_drv.obj: ../srio_drv.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="srio_drv.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

task_TCP_Svr.obj: ../task_TCP_Svr.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --include_path="D:/workspace_v8/zhou/20180703" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/qmss" --include_path="C:/ti/pdk_C6678_1_1_2_6/packages/ti/drv/cppi" --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="task_TCP_Svr.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


