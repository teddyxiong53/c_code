.PHONY: all clean helloworld gslist

CFLAGS:=`pkg-config --cflags --libs glib-2.0`

all:
	echo "make all"

helloworld:
	gcc helloworld.c $(CFLAGS)

gslist:
	gcc gslist.c $(CFLAGS)
	
clean:
	rm -rf a.out
