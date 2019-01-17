SRC=src
INC=include
LIB=.
CLIENT=client

CC=gcc
CCFLAGS=-I$(INC) -Wall -g
LDFLAGS=-L$(LIB) -lemqtt
AR=ar

PYTHON_VER=2.7
PYTHON_SRC=src/python
PYTHON_CCFLAGS=-I$(INC) -I$(PYTHON_SRC) -I/usr/include/python$(PYTHON_VER) -Wall -g -fPIC
PYTHON_LDFLAGS=-shared

c: $(CLIENT)/pub $(CLIENT)/sub

$(CLIENT)/pub: libemqtt.a pub.o
	$(CC) pub.o -o $(CLIENT)/pub $(LDFLAGS)

pub.o: $(CLIENT)/pub.c $(INC)/libemqtt.h
	$(CC) $(CCFLAGS) -c $(CLIENT)/pub.c -o pub.o

$(CLIENT)/sub: libemqtt.a sub.o
	$(CC) sub.o -o $(CLIENT)/sub $(LDFLAGS)

sub.o: $(CLIENT)/sub.c $(INC)/libemqtt.h
	$(CC) $(CCFLAGS) -c $(CLIENT)/sub.c -o sub.o

python:

libemqtt.a: libemqtt.o
	$(AR) rcs libemqtt.a libemqtt.o

libemqtt.o: $(SRC)/libemqtt.c $(INC)/libemqtt.h
	$(CC) $(CCFLAGS) -c $(SRC)/libemqtt.c -o libemqtt.o

all: c python

clean:
	rm -f *.o libemqtt.a

dist-clean: clean
	rm -f $(CLIENT)/pub $(CLIENT)/sub $(CLIENT)/libemqtt.so

