CC=sh2-uclinux-gcc
AR=sh2-uclinux-ar
RANLIB=sh2-uclinux-ranlib

CFLAGS=-Wall -Werror -Wno-error=unused-value 
LDFLAGS=-Wl,-elf2flt

BINS := cac_test09 

all: $(BINS)

cac_test09: cac_test09.o 
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(BINS) *.o *.gdb

.PHONY: clean all
