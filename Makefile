C_SOURCES := $(wildcard src/*.c src/*/*.c src/*/*/*.c src/*/*/*/*.c)
B_SOURCES := $(wildcard src/*.o src/*/*.o src/*/*/*.o src/*/*/*/*.o)
HEADERS := $(wildcard include/*.h src/*/*.h src/*/*/*.h src/*/*/*/*.h)
OBJ = ${C_SOURCES:.c=.o binary/interrupt.o} 

CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = gdb
LD = /usr/local/i386elfgcc/bin/i386-elf-ld
LDOBJ = /usr/local/i386elfgcc/bin/i386-elf-objcopy
CFLAGS = -g -ffreestanding -Wall -Wextra -Wno-unused-parameter -fno-exceptions -m32 -Iinclude -fvar-tracking

binary/os-image.bin: binary/bootsect.bin binary/kernel.bin
	cat $^ > binary/os-image.bin
	truncate -s 128K binary/os-image.bin

#binary/kernel.bin: binary/kernel_entry.o ${OBJ}
#	$(LD) -o $@ -Ttext 0x1000 $^ --oformat binary
binary/kernel.bin: binary/kernel.elf
	$(LDOBJ) -O binary $^ $@

binary/kernel.elf: binary/kernel_entry.o ${OBJ}
	$(LD) -o $@ -Ttext 0x1000 $^  

run: binary/os-image.bin
	qemu-system-i386 -fda binary/os-image.bin

debug: binary/os-image.bin binary/kernel.elf
	qemu-system-i386 -s -device piix3-ide,id=ide -drive id=disk,file=binary/os-image.bin,format=raw,if=none -device ide-hd,drive=disk,bus=ide.0 -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file binary/kernel.elf"

binary/%.o: %.c# ${HEADERS}
	${CC} ${CFLAGS} -c $< -o$@

binary/%.o: src/boot/%.asm
	nasm $< -f elf -o $@

binary/interrupt.o: src/cpu/interrupt.asm
	nasm $< -f elf -o $@

binary/%.bin: src/boot/%.asm src/cpu/interrupt.asm
	nasm $< -f bin -o $@

clean:
	rm -rf binary/*.bin binary/*.dis binary/*.o binary/os-image.bin binary/*.elf
	rm -rf $(B_SOURCES)
	clear
