KVERS := $(shell uname -r)

obj-m += proc_sample.o

all:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules 
clean:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean
