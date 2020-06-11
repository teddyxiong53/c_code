.PHONY: all clean
CFLAGS := -Wformat=0
LDFLAGS := -lasound


define gen_exe
	gcc $(CFLAGS) $1.c -o output/$1 $(LDFLAGS)
endef

default: all
help:
	@cat help.txt

all: prepare
	$(call gen_exe,pcm_min)
	$(call gen_exe,control)
prepare:
	if [ ! -e ./output ]; then \
		mkdir output ;\
	fi

clean:
	rm -rf output
