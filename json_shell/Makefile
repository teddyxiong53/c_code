.PHONY: prepare all test clean

SRCS:=`ls jshn_src/*.c`
default: all

prepare:
	# should install libjson-c-dev
	if [ ! -f ./jshn ]; then \
		sudo apt-get install libjson-c-dev -y \
	fi

all:
	#echo $(SRCS)
	gcc $(SRCS) -o jshn -ljson-c
clean:
	rm ./jshn

test:
	./test.sh

