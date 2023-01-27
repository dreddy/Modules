KERNDIR := /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

obj-$(sendipi)   := sendipi/
obj-$(scratch)   += scratch/
obj-$(dumpidt)   += dumpidt/
obj-$(rust_hello) += rust_hello/
obj-$(sysfs_hello) += sysfs_hello/

default:
	$(MAKE) -C $(KERNDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNDIR) M=$(PWD) clean
