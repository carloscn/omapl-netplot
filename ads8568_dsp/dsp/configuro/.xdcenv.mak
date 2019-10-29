#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /opt/ti/syslink_2_21_01_05/packages;/opt/ti/bios_6_35_04_50/packages;/opt/ti/ipc_1_25_03_15/packages;/opt/ti/xdctools_3_25_03_72/packages;/opt/ti/mathlib_c674x_3_0_2_0/packages
override XDCROOT = /opt/ti/xdctools_3_25_03_72
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /opt/ti/syslink_2_21_01_05/packages;/opt/ti/bios_6_35_04_50/packages;/opt/ti/ipc_1_25_03_15/packages;/opt/ti/xdctools_3_25_03_72/packages;/opt/ti/mathlib_c674x_3_0_2_0/packages;/opt/ti/xdctools_3_25_03_72/packages;..
HOSTOS = Linux
endif
