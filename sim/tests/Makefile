CC = sh2-elf-gcc
LD = sh2-elf-ld
AR = sh2-elf-ar
OBJCOPY = sh2-elf-objcopy
RANLIB = sh2-elf-ranlib
LIBGCC = `sh2-elf-gcc -print-file-name=libgcc.a`
CFLAGS = -m2 -g -Os -Wall -Iinclude
LDFLAGS = -T sh32.x -Map main.map

all: interrupts.img rte.img

interrupts.elf: interrupts.o
rte.elf: rte.o

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.elf:
	$(LD) $(LDFLAGS) $^ $(LIBGCC) -o $@

%.img: %.elf
	$(OBJCOPY) -v -S -O binary --srec-forceS3 $< $@

clean:
	rm -f *.o *.elf *.img *.map

.PHONY: clean
