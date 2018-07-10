################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt"
GEN_CMDS__FLAG := -l"configPkg/linker.cmd"

ORDERED_OBJS += \
"./SRIO_MSM_init.obj" \
"./SRIO_TransmitToFPGA.obj" \
"./SRIO_loopbackDioIsr.obj" \
"./SRIO_loopbackDioIsr_osal.obj" \
"./client.obj" \
"./device_srio_loopback.obj" \
"./idleFunctions_appuser.obj" \
"./platform_osal.obj" \
"./processTask.obj" \
"./resourcemgr.obj" \
"./srio_drv.obj" \
"./task_TCP_Svr.obj" \
$(GEN_CMDS__FLAG) \
-l"C:/ti/pdk_C6678_1_1_2_6/packages/ti/csl/lib/ti.csl.intc.ae66" \
-l"C:/ti/pdk_C6678_1_1_2_6/packages/ti/csl/lib/ti.csl.ae66" \
-l"C:/ti/dsplib_c66x_3_4_0_0/packages/ti/dsplib/lib/dsplib.ae66" \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
SocketEchoServerAndSRIO_evmc6678l_2_releaseV5_mm.out \

EXE_OUTPUTS__QUOTED += \
"SocketEchoServerAndSRIO_evmc6678l_2_releaseV5_mm.out" \


# All Target
all: SocketEchoServerAndSRIO_evmc6678l_2_releaseV5_mm.out

# Tool invocations
SocketEchoServerAndSRIO_evmc6678l_2_releaseV5_mm.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C6000 Linker'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/bin/cl6x" -mv6600 --abi=eabi -O1 -g --display_error_number --diag_warning=225 -z -m"SocketEchoServerAndSRIO_evmc6678l_2_releaseV5_mm.map" -i"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/lib" -i"C:/ti/pdk_C6678_1_1_2_6/packages/ti/csl/lib" -i"C:/ti/mathlib_c66x_3_1_1_0/packages/ti/mathlib/lib" -i"C:/ti/dsplib_c66x_3_4_0_0/packages/ti/dsplib/lib" -i"C:/ti/ccsv8/tools/compiler/ti-cgt-c6000_8.2.2/include" --reread_libs --warn_sections --xml_link_info="SocketEchoServerAndSRIO_evmc6678l_2_releaseV5_mm_linkInfo.xml" --rom_model -o "SocketEchoServerAndSRIO_evmc6678l_2_releaseV5_mm.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_CMDS__QUOTED)$(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)$(GEN_OPTS__QUOTED)
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-$(RM) "SRIO_MSM_init.obj" "SRIO_TransmitToFPGA.obj" "SRIO_loopbackDioIsr.obj" "SRIO_loopbackDioIsr_osal.obj" "client.obj" "device_srio_loopback.obj" "idleFunctions_appuser.obj" "platform_osal.obj" "processTask.obj" "resourcemgr.obj" "srio_drv.obj" "task_TCP_Svr.obj" 
	-$(RM) "SRIO_MSM_init.d" "SRIO_TransmitToFPGA.d" "SRIO_loopbackDioIsr.d" "SRIO_loopbackDioIsr_osal.d" "client.d" "device_srio_loopback.d" "idleFunctions_appuser.d" "platform_osal.d" "processTask.d" "resourcemgr.d" "srio_drv.d" "task_TCP_Svr.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

