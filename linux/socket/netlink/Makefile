obj-m += netlink_kernel.o


all:  modules 

modules:
	$(CROSS_COMPILE)gcc netlink_user.c -o netlink_user
	make -C $(KERNEL_DIR) M=$$PWD modules


	
	
clean:
	rm -rf *.o  .*.cmd  *.ko *.mod.c .tmp_versions  modules.order Module.symvers  *.tmp *.log
	