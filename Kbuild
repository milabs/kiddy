MODNAME		?= kiddy

include		$(src)/src/khook/Makefile.khook

obj-m		+= $(MODNAME).o
ccflags-y	+= $(KHOOK_CCFLAGS) -std=gnu99
ldflags-y	+= $(KHOOK_LDFLAGS)

include		$(src)/Makefile.version

ccflags-y	+= -DLINUX_VERSION_A=$(linuxVersionA) \
		   -DLINUX_VERSION_B=$(linuxVersionB) \
		   -DLINUX_VERSION_C=$(linuxVersionC)

ifneq ($(PRESET),)
include		$(src)/presets/$(PRESET)
endif

$(MODNAME)-objs += src/mod.o src/mod_syslog.o src/mod_uname.o src/mod_vdso.o src/mod_fs.o $(KHOOK_GOALS)
