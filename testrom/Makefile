OBJS := entry.o gdb.o sh2.o main.o version.o
OBJS += uartlite.o
#OBJS += uart16550.o

TESTS_OBJS := testbra.o
TESTS_OBJS += testmov.o testmov2.o
TESTS_OBJS += testalu.o
TESTS_OBJS += testshift.o
TESTS_OBJS += testmul.o testmulu.o testmuls.o testmull.o testdmulu.o testdmuls.o testmulconf.o
TESTS_OBJS += testdiv.o
TESTS_OBJS += testmacw.o testmacl.o
TESTS_OBJS := $(addprefix tests/,$(TESTS_OBJS))

CC = sh2-elf-gcc
LD = sh2-elf-ld
AR = sh2-elf-ar
RANLIB = sh2-elf-ranlib
ECHO = /bin/echo
LDFLAGS = -T startup/sh32.x -Map main.map
LIBGCC = $(shell $(CC) -print-libgcc-file-name)

CFLAGS := -m2 -g -Os -Wall

DDR ?= ddr16
ifeq ($(DDR), lpddr)
	CFLAGS += -D LPDDR
else ifeq ($(DDR), ddr8)
	CFLAGS += -D DDR_BL4
endif

all: main.elf

tests/libtests.a: $(TESTS_OBJS)
	$(AR) -cur $@ $^
	$(RANLIB) $@
	@echo Built test library


main.elf: $(OBJS) tests/libtests.a
	$(LD) $(LDFLAGS) $(OBJS) -Ltests -ltests $(LIBGCC) -o $@

main.o: main.c
	$(CC) $(CFLAGS) -fno-inline -c $<

.s.o:
	$(CC) $(CFLAGS) -c $< -o $@

version.c:
	@printf "char version_string[] = \"revision: open\\\\nbuild: $(shell date)\\\\n\";\n" > $@

clean:
	rm -f *.[oa] tests/*.o tests/libtests.a
	rm -f version.c
	rm -f main.srec main.elf *.map a.out

.PHONY: clean all
