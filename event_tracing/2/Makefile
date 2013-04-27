obj-m += tp.o
KDIR := /usr/src/kernels/$(shell uname -r)/
PWD := $(shell pwd)
INCLUDE := -I/usr/include
default:
	@echo ’ Building target module 2.6 kernel.’
	@echo ’ PLEASE IGNORE THE ”Overriding SUBDIRS” WARNING’
	$(MAKE) -C $(KDIR) $(INCLUDE) SUBDIRS=$(PWD) modules
clean:
	rm -f -r *.o *.ko .*cmd .tmp* core *.i modules.order  Module.symvers *.mod.c

