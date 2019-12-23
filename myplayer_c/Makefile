# Only for C code compile
# Simple version


TARGET := myplayer

CC := gcc
AR := ar
LIBS :=
LDFLAGS :=

DEFINES :=
INCLUDE :=

INCLUDE += -I./src/main

CFLAGS := -g -Wall -O3 $(DEFINES) $(INCLUDE)
#CXXFLAGS := $(CFLAGS) -DHAVE_CONFIG_H



SRC_DIR := src/module1 src/module2 src/main
OBJ_DIR := objs
DEP_DIR := deps

SRCS := $(foreach d,$(SRC_DIR),$(wildcard $(d)/*.c))
OBJS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))
DEPS := $(patsubst %.c, $(DEP_DIR)/%.d, $(SRCS))




.PHONY: all clean prepare

default:all

prepare:
	# create obj dirs
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(DEP_DIR)

	@for d in $(SRC_DIR) ;\
	do \
		if [[ ! -d $(OBJ_DIR)/$$d  ]]; then \
			mkdir -p $(OBJ_DIR)/$$d;\
		else \
			echo "$(OBJ_DIR)/$$d already exists";\
		fi \
	done;

	@for d in $(SRC_DIR) ;\
	do \
		if [[ ! -d $(DEP_DIR)/$$d  ]]; then \
			mkdir -p $(DEP_DIR)/$$d;\
		else \
			echo "$(DEP_DIR)/$$d already exists";\
		fi \
	done;

$(OBJS): $(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c "$<" -o "$@"


$(DEPS):$(DEP_DIR)/%.d:%.c
	@set -e; rm -f $@; $(CC) -MM $< $(INCLUDE) > $@.$$$$;\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$


all:prepare $(DEPS) $(OBJS)
	@echo "all"
	gcc -o $(TARGET) $(OBJS)
clean:
	@echo "clean"
	@-rm -rf $(DEP_DIR)
	@-rm -rf $(OBJ_DIR)
	@-rm -f $(TARGET)
