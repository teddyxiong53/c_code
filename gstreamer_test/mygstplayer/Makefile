.PHONY: all clean test

CFLAGS := `pkg-config --cflags --libs gstreamer-1.0`
all:
	gcc main.c -o mygstplayer $(CFLAGS)
test:
	gcc test.c -o test $(CFLAGS)

clean:
	rm -rf *.o mygstplayer test

