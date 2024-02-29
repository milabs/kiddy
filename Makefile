KDIR ?= /lib/modules/$(shell uname -r)/build

-:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
