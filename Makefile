#!/usr/bin/make -f
# Makefile for config-parse
# Author: Stefan Misik (mail@stefanmisik.eu)


# Project properties
OUT = config_parse
SRC = main.c config_parse.c

# Flags
CFLAGS   = -c
LDFLAGS  =
LDLIBS   = 
################################################################################
# Debug flags
ifeq ($(DBG),y)
	CFLAGS   += -D_DEBUG -ggdb -Wall
	LDFLAGS  += -ggdb
endif

# Object files
OBJ := $(addsuffix .o,$(basename $(SRC))) $(RES:.rc=.o)
	
# Dependencies
DEP := $(addsuffix .d,$(basename $(SRC)))
################################################################################

all: $(OUT)

# Pull in dependency info
-include $(DEP)

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@ $(LDLIBS)

%.o: %.c	
	$(CC) $(CFLAGS) $< -o $@
	
%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@

clean:
	$(RM) $(OUT) $(OBJ) $(DEP)


.PHONY: FORCE all clean

FORCE:
