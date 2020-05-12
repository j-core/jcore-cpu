CC=sh2-uclinux-gcc
AR=sh2-uclinux-ar
RANLIB=sh2-uclinux-ranlib

CFLAGS=-Wall -Werror -Wno-error=unused-value 
LDFLAGS=-Wl,-elf2flt


BINS := smp_test07

all: $(BINS)

smp_test07: smp_test07.o get_vbr.o
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(BINS) *.o *.gdb

.PHONY: clean all
