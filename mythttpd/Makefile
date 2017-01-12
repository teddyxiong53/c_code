
.PHONY : all clean

default: all

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=mythttpd

all:$(OBJS)
	gcc -o  $(TARGET) $^
	
clean:
	rm *.o $(TARGET) core
	
