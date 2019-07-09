.PHONY: all clean helloworld gslist glist gqueue ghashtable gstring \
	garray gptrarray gbytearray gtree grelation grand gthreadpool \
	gassert gthread glock gcond gprivate gasyncqueue gmainloop

CFLAGS:=`pkg-config --cflags --libs glib-2.0`

all:
	@echo "请单个测试"

helloworld:
	gcc helloworld.c $(CFLAGS)

gslist:
	gcc gslist.c $(CFLAGS)

glist:
	gcc glist.c $(CFLAGS)

gqueue:
	gcc gqueue.c $(CFLAGS)

ghashtable:
	gcc ghashtable.c $(CFLAGS)

gstring:
	gcc gstring.c $(CFLAGS)

garray:
	gcc garray.c $(CFLAGS)

gptrarray:
	gcc gptrarray.c $(CFLAGS)


gbytearray:
	gcc gbytearray.c $(CFLAGS)

gtree:
	gcc gtree.c $(CFLAGS)

grelation:
	gcc grelation.c $(CFLAGS)

grand:
	gcc grand.c $(CFLAGS)

gthreadpool:
	gcc gthreadpool.c $(CFLAGS)



gassert:
	gcc gassert.c $(CFLAGS)


gthread:
	gcc gthread.c $(CFLAGS)

glock:
	gcc glock.c $(CFLAGS)

gcond:
	gcc gcond.c $(CFLAGS)

gprivate:
	gcc gprivate.c $(CFLAGS)

gasyncqueue:
	gcc gasyncqueue.c $(CFLAGS)

gmainloop:
	gcc gmainloop.c $(CFLAGS)
clean:
	rm -rf a.out
