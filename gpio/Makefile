ifneq ($(KERNELRELEASE),)
	obj-m := gpio_ex.o

else

	KERNELDIR ?= /home/kj/dev/linux
	PWD := $(shell pwd)


default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules


clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c *.mod \
		.tmp_versions modules.order Module.symvers .cache.mk \
		dio *.tmp *.log

endif