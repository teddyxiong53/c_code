csrc = $(wildcard src/*.c) \
		$(wildcard src/mod1/*.c)

ccsrc = $(wildcard src/*.cpp) \
		$(wildcard src/mod1/*.cpp)
obj = $(csrc:.c=.o) $(ccsrc:.cpp=.o)
dep = $(obj:.o=.d)

CXX=g++
CC=gcc
CPP=g++

LDFLAGS = -lpthread

TARGET = test

$(TARGET): $(obj)
	$(CXX) -o $@ $^ $(LDFLAGS)

-include $(dep)

%.d:%.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.d:%.cpp
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@


.PHONY: clean cleandep distclean

distclean:clean cleandep

clean:
	rm -f $(obj) $(TARGET)

cleandep:
	rm -rf $(dep)

