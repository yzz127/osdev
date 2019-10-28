OBJECTS = start.o scrn.o gdt.o idt.o isrs.o irq.o timer.o kb.o main.o
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -I./include -c
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

all: kernel

kernel: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel

kerndev.iso: kernel
	cp kernel boot
	grub-mkrescue -o kerndev.iso .

run: kerndev.iso
	qemu-system-i386 -cdrom kerndev.iso

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm *.o kernel boot/kernel kerndev.iso
