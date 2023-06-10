C_SOURCES = $(wildcard kernel/*.c kernel/commands/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h kernel/commands/*.h drivers/*.h cpu/*.h libc/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o} 

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = gdb
LD = /usr/local/i386elfgcc/bin/i386-elf-ld
# -g: Use debugging symbols in gcc
CFLAGS = -g -ffreestanding -Wall -Wextra -fno-exceptions -m32

# First rule is run by default
os-image.bin: boot/bootsect.bin kernel.bin
	cat $^ > os-image.bin
	truncate -s 32K os-image.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: boot/kernel_entry.o ${OBJ} #drivers/ata/ata_read.o ${OBJ}
	$(LD) -o $@ -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
kernel.elf: boot/kernel_entry.o ${OBJ} #drivers/ata/ata_read.o ${OBJ}
	$(LD) -o $@ -Ttext 0x1000 $^  

run: os-image.bin
	qemu-system-i386 -fda os-image.bin

# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin kernel.elf
	#genisoimage -o os.iso os-image.bin
	#qemu-system-i386 -s -M q35 -hda os.iso -d guest_errors,int &
	#qemu-system-i386 -s -drive id=disk,file=os-image.bin,if=none -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0 -d guest_errors,int &
	#qemu-system-i386 -s -fda os-image.bin -d guest_errors,int &
	qemu-system-i386 -s -device piix3-ide,id=ide -drive id=disk,file=os-image.bin,format=raw,if=none -device ide-hd,drive=disk,bus=ide.0 -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o
