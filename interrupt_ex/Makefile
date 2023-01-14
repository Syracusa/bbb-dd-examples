obj-m := interrupt_ex.o

KERNELDIR ?= /home/kj/dev/linux
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c *.mod \
		.tmp_versions modules.order Module.symvers .cache.mk \
		dio *.tmp *.log
