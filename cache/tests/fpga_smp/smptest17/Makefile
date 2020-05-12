CC=sh2-uclinux-gcc
AR=sh2-uclinux-ar
RANLIB=sh2-uclinux-ranlib

CFLAGS=-O2 -Wall -Werror -Wno-error=unused-value 
LDFLAGS=-Wl,-elf2flt

BINS := test17

all: $(BINS)

test17: test17.o
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(BINS) *.o *.gdb

.PHONY: clean all
