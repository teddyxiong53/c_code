obj-m := mymisc.o

PWD := $(shell pwd)

KERNEL_DIR := /lib/modules/$(shell uname -r)/build
.PHONY: all clean test

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
test:
	gcc test.c