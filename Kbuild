MODNAME		?= kiddy

include		$(src)/src/khook/Makefile.khook

obj-m		+= $(MODNAME).o
ccflags-y	+= $(KHOOK_CCFLAGS)
ldflags-y	+= $(KHOOK_LDFLAGS)

ifneq ($(PRESET),)
include		$(src)/presets/$(PRESET)
endif

$(MODNAME)-objs += src/mod.o src/mod_syslog.o src/mod_uname.o src/mod_vdso.o src/mod_fs.o $(KHOOK_GOALS)
