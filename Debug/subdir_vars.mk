################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../clientAndSRIO.cfg 

C_SRCS += \
../SRIO_MSM_init.c \
../SRIO_TransmitToFPGA.c \
../SRIO_loopbackDioIsr.c \
../SRIO_loopbackDioIsr_osal.c \
../client.c \
../device_srio_loopback.c \
../idleFunctions_appuser.c \
../platform_osal.c \
../processTask.c \
../resourcemgr.c \
../srio_drv.c \
../task_TCP_Svr.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./SRIO_MSM_init.d \
./SRIO_TransmitToFPGA.d \
./SRIO_loopbackDioIsr.d \
./SRIO_loopbackDioIsr_osal.d \
./client.d \
./device_srio_loopback.d \
./idleFunctions_appuser.d \
./platform_osal.d \
./processTask.d \
./resourcemgr.d \
./srio_drv.d \
./task_TCP_Svr.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./SRIO_MSM_init.obj \
./SRIO_TransmitToFPGA.obj \
./SRIO_loopbackDioIsr.obj \
./SRIO_loopbackDioIsr_osal.obj \
./client.obj \
./device_srio_loopback.obj \
./idleFunctions_appuser.obj \
./platform_osal.obj \
./processTask.obj \
./resourcemgr.obj \
./srio_drv.obj \
./task_TCP_Svr.obj 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

OBJS__QUOTED += \
"SRIO_MSM_init.obj" \
"SRIO_TransmitToFPGA.obj" \
"SRIO_loopbackDioIsr.obj" \
"SRIO_loopbackDioIsr_osal.obj" \
"client.obj" \
"device_srio_loopback.obj" \
"idleFunctions_appuser.obj" \
"platform_osal.obj" \
"processTask.obj" \
"resourcemgr.obj" \
"srio_drv.obj" \
"task_TCP_Svr.obj" 

C_DEPS__QUOTED += \
"SRIO_MSM_init.d" \
"SRIO_TransmitToFPGA.d" \
"SRIO_loopbackDioIsr.d" \
"SRIO_loopbackDioIsr_osal.d" \
"client.d" \
"device_srio_loopback.d" \
"idleFunctions_appuser.d" \
"platform_osal.d" \
"processTask.d" \
"resourcemgr.d" \
"srio_drv.d" \
"task_TCP_Svr.d" 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

C_SRCS__QUOTED += \
"../SRIO_MSM_init.c" \
"../SRIO_TransmitToFPGA.c" \
"../SRIO_loopbackDioIsr.c" \
"../SRIO_loopbackDioIsr_osal.c" \
"../client.c" \
"../device_srio_loopback.c" \
"../idleFunctions_appuser.c" \
"../platform_osal.c" \
"../processTask.c" \
"../resourcemgr.c" \
"../srio_drv.c" \
"../task_TCP_Svr.c" 


