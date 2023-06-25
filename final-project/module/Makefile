#Makefile for a basic kernel module

obj-m   := filemanager.o
filemanager-objs := device/device.o driver/driver.o timer/timer.o interrupt/interrupt.o

KDIR    :=/work/achroimx_kernel
PWD     :=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm
push:
	adb push filemanager.ko /data/local/tmp
clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf fpga_test_fnd
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .fnd*
	rm -rf .tmp*
	rm -rf */*.o