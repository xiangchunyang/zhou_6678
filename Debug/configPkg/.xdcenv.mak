#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/pdk_C6678_1_1_2_6/packages;C:/ti/ipc_1_24_03_32/packages;C:/ti/ndk_2_21_02_43/packages;C:/ti/ccsv8/ccs_base;C:/ti/dsplib_c66x_3_4_0_0/packages;C:/ti/mathlib_c66x_3_1_2_1/packages;C:/ti/bios_6_33_06_50/packages;C:/ti/mcsdk_2_01_02_06/demos;C:/ti/xdctools_3_23_04_60/packages/ti/platforms/my6678EVMhxl
override XDCROOT = C:/ti/xdctools_3_23_04_60
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/pdk_C6678_1_1_2_6/packages;C:/ti/ipc_1_24_03_32/packages;C:/ti/ndk_2_21_02_43/packages;C:/ti/ccsv8/ccs_base;C:/ti/dsplib_c66x_3_4_0_0/packages;C:/ti/mathlib_c66x_3_1_2_1/packages;C:/ti/bios_6_33_06_50/packages;C:/ti/mcsdk_2_01_02_06/demos;C:/ti/xdctools_3_23_04_60/packages/ti/platforms/my6678EVMhxl;C:/ti/xdctools_3_23_04_60/packages;..
HOSTOS = Windows
endif
