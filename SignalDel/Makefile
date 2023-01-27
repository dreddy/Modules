KERNDIR := /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

obj-m  += signal_kernel.o

default:
	$(MAKE) -C $(KERNDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KERNDIR) M=$(shell pwd) clean
