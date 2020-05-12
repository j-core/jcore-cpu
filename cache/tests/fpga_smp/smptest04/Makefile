CC=sh2-uclinux-gcc
AR=sh2-uclinux-ar
RANLIB=sh2-uclinux-ranlib

CFLAGS=-Wall -Werror -Wno-error=unused-value 
LDFLAGS=-Wl,-elf2flt

BINS := test04 

all: $(BINS)

test04: test04.o get_lock_cpu0.o
	$(CC) $^ $(LDFLAGS) -o $@

get_lock_cpu0_stub.s : get_lock_cpu0_stub.c
	sh2-uclinux-gcc $(CFLAGS) -O -S get_lock_cpu0_stub.c

get_lock_cpu0.o : get_lock_cpu0.s
	sh2-uclinux-gcc $(CFLAGS) -c -o get_lock_cpu0.o get_lock_cpu0.s

clean:
	rm -f $(BINS) *.o *.gdb

.PHONY: clean all
