obj-m += ldt.o

KERNEL_DIR := /lib/modules/$(shell uname -r )/build


all: modules dio

modules:
	make -C $(KERNEL_DIR) M=$$PWD modules
	
dio:
	
	
clean:
	rm -rf *.o  .*.cmd  *.ko *.mod.c .tmp_versions  modules.order Module.symvers dio *.tmp *.log
	