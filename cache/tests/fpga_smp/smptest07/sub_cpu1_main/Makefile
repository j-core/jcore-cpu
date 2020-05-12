CC=sh2-uclinux-gcc
AR=sh2-uclinux-ar
RANLIB=sh2-uclinux-ranlib

CFLAGS=-Wall -Werror -Wno-error=unused-value 
LDFLAGS=-Wl,-elf2flt

BINS := cpu1_main 

all: $(BINS)

cpu1_main: cpu1_main.o
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(BINS) *.o *.gdb

.PHONY: clean all
