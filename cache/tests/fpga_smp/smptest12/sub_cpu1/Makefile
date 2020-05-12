ARCH = sh2-elf
CC = $(ARCH)-gcc
LD = $(ARCH)-ld
CFLAGS+=-Iinclude
CFLAGS+=-DTIME -DHZ=60
#CFLAGS+=-O2

EXEC = cpu1
OBJS = cpu1.o cpu1_restart_asm.o

CFLAGS += -DNO_PROTOTYPES=1

LIBGCC := $(shell $(CC) -print-libgcc-file-name)
LIBGCC += $(shell $(CC) -print-file-name=libgcc-Os-4-200.a)
LDFLAGS = -e _main -Ttext=0x14001000 ./lib/libc.a $(LIBGCC)

all: $(EXEC)

$(EXEC): $(OBJS) lib/libc.a
	$(LD) $^ $(LDFLAGS) -o $@
#	$(CC) $(LDFLAGS) -o $@ $^

romfs:
	$(ROMFSINST) /bin/$(EXEC)

lib/libc.a:
	make -C lib

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
	make -C lib clean

$(OBJS): 

