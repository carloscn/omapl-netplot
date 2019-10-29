MCSDK                    = /opt/ti
CCS                      = /opt/ti
STARTWARE_INSTALL_DIR    = /opt/ti/OMAPL138_StarterWare_1_10_04_01

SYSLINK_INSTALL_DIR      = $(MCSDK)/syslink_2_21_01_05
IPC_INSTALL_DIR          = $(MCSDK)/ipc_1_25_03_15
MATHLIB_INSTALL_DIR      = $(MCSDK)/mathlib_c674x_3_0_2_0
DSPLIB_INSTALL_DIR       = $(MCSDK)/dsplib_c674x_3_1_1_1
BIOS_INSTALL_DIR         = $(CCS)/bios_6_35_04_50
XDC_INSTALL_DIR          = $(CCS)/xdctools_3_25_03_72
CGT_C674_ELF_INSTALL_DIR = $(CCS)/ccsv5/tools/compiler/c6000_7.4.4


VPATH = app ipc codec driver
srcs = main_dsp.c ad7606.c message_heap.c message_loop.c ad_workshop.c fft.c ring_buffer.c queue.c ipc_ring_buffer.c fake_ring_buffer.c
objs = $(addprefix bin/$(PROFILE)/obj/,$(patsubst %.c,%.oe674,$(srcs)))
libs = configuro/linker.cmd

all: configuro/linker.cmd
	make PROFILE=debug server_dsp.x
	#make PROFILE=release server_dsp.x

server_dsp.x: bin/$(PROFILE)/server_dsp.xe674
bin/$(PROFILE)/server_dsp.xe674: $(objs) $(libs)
	@echo "#"
	@echo "# Making $@ ..."
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

bin/$(PROFILE)/obj/%.oe674: %.h
bin/$(PROFILE)/obj/%.oe674: %.c log.h
	@echo "#"
	@echo "# Making $@ ..."
	$(CC) $(CPPFLAGS) $(CFLAGS) --output_file=$@ -fc $<

configuro/linker.cmd: dsp.cfg ../shared/config.bld
	@echo "#"
	@echo "# Making $@ ..."
	$(XDC_INSTALL_DIR)/xs --xdcpath="$(subst +,;,$(PKGPATH))"       \
            xdc.tools.configuro -o configuro                        \
            -t ti.targets.elf.C674 -c $(CGT_C674_ELF_INSTALL_DIR)   \
            -p ti.platforms.evmOMAPL138:dsp -b ../shared/config.bld \
            -r release dsp.cfg

clean::
	rm -rf configuro bin

PKGPATH := $(SYSLINK_INSTALL_DIR)/packages
PKGPATH := $(PKGPATH)+$(BIOS_INSTALL_DIR)/packages
PKGPATH := $(PKGPATH)+$(IPC_INSTALL_DIR)/packages
PKGPATH := $(PKGPATH)+$(XDC_INSTALL_DIR)/packages
PKGPATH := $(PKGPATH)+$(MATHLIB_INSTALL_DIR)/packages

CGTOOLS = $(CGT_C674_ELF_INSTALL_DIR)

CC = $(CGTOOLS)/bin/cl6x -c --gcc # gcc extensions support
AR = $(CGTOOLS)/bin/ar6x rq
LD = $(CGTOOLS)/bin/lnk6x --abi=eabi
ST = $(CGTOOLS)/bin/strip6x

CPPFLAGS =
CFLAGS = -qq $(CCPROFILE_$(PROFILE)) $(COMPILER_OPTS)     \
		 -I ./app -I ./ipc -I ./codec -I ./driver         \
		 -I$(STARTWARE_INSTALL_DIR)/include               \
		 -I$(STARTWARE_INSTALL_DIR)/include/hw            \
		 -I$(MATHLIB_INSTALL_DIR)/packages                \
		 -I$(MATHLIB_INSTALL_DIR)/inc                     \
		 -I$(DSPLIB_INSTALL_DIR)/packages                 \
		 -I$(DSPLIB_INSTALL_DIR)/inc

LDFLAGS = -w -q -c -m $(@D)/obj/$(@F).map
LDLIBS  = -l $(MATHLIB_INSTALL_DIR)/lib/mathlib.ae674									    \
          -l $(CGTOOLS)/lib/rts6740_elf.lib												    \
		  -l $(STARTWARE_INSTALL_DIR)/binary/c674x/cgt/utils/Release/utils.lib              \
		  -l Platform.lib                                                                   \
          -l $(STARTWARE_INSTALL_DIR)/binary/c674x/cgt/omapl138/drivers/Release/drivers.lib \
		  -l $(DSPLIB_INSTALL_DIR)/lib/dsplib.ae674  

CCPROFILE_debug = -D_DEBUG_=1 --symdebug:dwarf
CCPROFILE_release = -O2
COMPILER_OPTS = $(shell cat configuro/compiler.opt)

ifneq (clean,$(MAKECMDGOALS))
ifneq (,$(PROFILE))
ifeq (,$(wildcard bin/$(PROFILE)/obj))
    $(shell mkdir -p bin/$(PROFILE)/obj)
endif
endif
endif
