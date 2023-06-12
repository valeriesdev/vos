C_SOURCES = $(wildcard kernel/*.c kernel/commands/*.c drivers/*.c cpu/*.c libc/*.c stock/tedit/*.c)
HEADERS = $(wildcard kernel/*.h kernel/commands/*.h drivers/*.h cpu/*.h libc/*.h stock/tedit/*.h)
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o} 

CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = gdb
LD = /usr/local/i386elfgcc/bin/i386-elf-ld
CFLAGS = -g -ffreestanding -Wall -Wextra -fno-exceptions -m32

os-image.bin: boot/bootsect.bin kernel.bin
	cat $^ > os-image.bin
	truncate -s 32K os-image.bin

kernel.bin: boot/kernel_entry.o ${OBJ}
	$(LD) -o $@ -Ttext 0x1000 $^ --oformat binary

kernel.elf: boot/kernel_entry.o ${OBJ}
	$(LD) -o $@ -Ttext 0x1000 $^  

run: os-image.bin
	qemu-system-i386 -fda os-image.bin

debug: os-image.bin kernel.elf
	qemu-system-i386 -s -device piix3-ide,id=ide -drive id=disk,file=os-image.bin,format=raw,if=none -device ide-hd,drive=disk,bus=ide.0 -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o
