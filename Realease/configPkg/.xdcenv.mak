#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/ipc_1_24_03_32/packages;C:/ti/ndk_2_21_01_38/packages;C:/ti/ccsv5/ccs_base;C:/ti/dsplib_c66x_3_4_0_0/packages;C:/ti/mathlib_c66x_3_1_1_0/packages;C:/ti/mcsdk_2_01_02_05/demos;C:/ti/xdctools_3_23_04_60/packages/ti/platforms/my6678EVMhxl;C:/ti/pdk_C6678_1_1_2_6/packages;C:/ti/bios_6_33_06_50/packages
override XDCROOT = C:/ti/xdctools_3_23_04_60
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/ipc_1_24_03_32/packages;C:/ti/ndk_2_21_01_38/packages;C:/ti/ccsv5/ccs_base;C:/ti/dsplib_c66x_3_4_0_0/packages;C:/ti/mathlib_c66x_3_1_1_0/packages;C:/ti/mcsdk_2_01_02_05/demos;C:/ti/xdctools_3_23_04_60/packages/ti/platforms/my6678EVMhxl;C:/ti/pdk_C6678_1_1_2_6/packages;C:/ti/bios_6_33_06_50/packages;C:/ti/xdctools_3_23_04_60/packages;..
HOSTOS = Windows
endif
