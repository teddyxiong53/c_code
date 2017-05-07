
.PHONY: all clean

all:
	gcc receiver.c -o receiver -lrt
	gcc sender.c -o sender -lrt
clean:
	-rm -rf *.o sender receiver
	