.PHONY: all clean

CXXFLAGS:=`pkg-config --cflags --libs gstreamer-1.0`
CXXFLAGS+=-std=c++11
CXXFLAGS+=-fpermissive
all:
	g++ test.cpp GstMediaPlayer.cpp GstPlayer.cpp GstUtils.cpp Context.cpp -o test $(CXXFLAGS) -I./ -lgstapp-1.0 -DUSE_GLOOP

clean:
	rm -rf test
