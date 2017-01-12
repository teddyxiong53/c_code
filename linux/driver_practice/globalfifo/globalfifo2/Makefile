KVERS := $(shell uname -r)

obj-m += globalfifo.o

all:
	make -C /lib/modules/4.2.0-16-generic/build M=$(CURDIR) modules 
clean:
	make -C /lib/modules/4.2.0-16-generic/build M=$(CURDIR) clean
