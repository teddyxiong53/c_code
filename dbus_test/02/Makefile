.PHON: all clean
CFLAGS := $(shell pkg-config --cflags dbus-1)
LDFLAGS := $(shell pkg-config --libs dbus-1)
define gen_exe
	gcc $(CFLAGS) $1.c -o $1 $(LDFLAGS)
endef
all:
	$(call gen_exe,signal-send)
	$(call gen_exe,signal-recv)

clean:
	rm -rf  signal-send signal-recv



