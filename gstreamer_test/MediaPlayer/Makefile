.PHONY: all clean

CXXFLAGS:=`pkg-config --cflags --libs gstreamer-1.0`
CXXFLAGS+="-std=c++11"

all:
	g++ test.cpp GstMediaPlayer.cpp GstPlayer.cpp GstUtils.cpp Context.cpp -o test $(CXXFLAGS) -I./ -lgstapp-1.0

clean:
	rm -rf test
