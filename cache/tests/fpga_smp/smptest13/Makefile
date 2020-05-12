CC=sh2-uclinux-gcc
AR=sh2-uclinux-ar
RANLIB=sh2-uclinux-ranlib

CFLAGS=-O2 -Wall -Werror -Wno-error=unused-value 
LDFLAGS=-Wl,-elf2flt

BINS := test13

all: $(BINS)

test13: test13.o
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(BINS) *.o *.gdb

.PHONY: clean all
