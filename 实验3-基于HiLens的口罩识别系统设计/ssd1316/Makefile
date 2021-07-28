obj-m:=ssd1316.o

KERNELDIR:= ../../../gw3399-linux/kernel

PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *.order .*.cmd *.ko *.mod.c *.symvers *~

