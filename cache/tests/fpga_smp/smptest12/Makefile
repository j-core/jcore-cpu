CC=sh2-uclinux-gcc
AR=sh2-uclinux-ar
RANLIB=sh2-uclinux-ranlib

CFLAGS=-O2 -Wall -Werror -Wno-error=unused-value 
LDFLAGS=-Wl,-elf2flt

BINS := test12 

all: $(BINS)

test12: test12.o
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(BINS) *.o *.gdb

.PHONY: clean all
