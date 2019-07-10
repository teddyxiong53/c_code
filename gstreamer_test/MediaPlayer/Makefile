.PHONY: all clean

CXXFLAGS:=`pkg-config --cflags --libs gstreamer-1.0`
all:
	g++ test_MediaPlayer.cpp MediaPlayer.cpp -o test $(CXXFLAGS) -I../include

clean:
	rm -rf test
