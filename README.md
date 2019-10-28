# Operating System Development

## Running Instructions
### Dependencies
Linux i386-gcc toolchain, nasm, qemu, xorriso
### Make bootable os image and start os in qemu
```shell
make clean
make run
```
### Generate kernel debug symbol and start os for gdb debug
```shell
make debug
```
start a new terminal
```
gdb kernel.debug
(gdb) target remote :1234
```
### Register handlers
* 

## Operating System Notes (To be categorized)

A device is considered bootable if the bootsector contains the valid signature bytes 0x55 and 0xAA at offset 511 and 512 (0x1ff). 

BIOS physically searches for a boot device by loading the first 512 bytes from the bootsector of the each device into physical memory, staring at the address 0x7c00. When the valid signature bytes are detected, BIOS transfers the control to the 0x7c00 memory address (via jmp) to execute the bootsector code.

Framebuffer address starts at 0xb8000, and controls a screen of characters 80 wide by 25 high. It is indexed by (y * 80 + x) * 2. Additional VGA controller at 0x3d4 (control register) and 0x3d5 (data register)

Character code (bit 0:7), foreground color (bit 8:11), background color (bit 12:15)

Color code: 0:black, 1:blue, 2:green, 3:cyan, 4:red, 5:magenta, 6:brown, 7:light grey, 8:dark grey, 9:light blue, 10:light green, 11:light cyan, 12:light red, 13:light magneta, 14: light brown, 15: white.

x86 architecture use segmentation and paging for memory protection and virtual memory. With segmentation, every memory access is evaluated with respect to a segment. The memory address is added to the segment's base address, and checked against the segment's length. With paging, the address space is split into (usually 4KB) blocks called pages. Each page can be mapped into and unmapped from physical memory.

Segmentation can set the ring (privilege) level. Zero (most privileged) to three (least privileged). Process in ring zero is running in kernel-mode, or supervisor-mode, and can use privileged instructions such as sti and cli. 

There are 6 segmentation registers in x86, cs (code segment), ds (data segment), es (extra segment), fs, gs, ss (stack segment)

The attributes of the segments are set by the Global Descriptor Table (GDT)

```c
struct gdt_entry
{
    unsigned short limit_low;       // The lower 16 bits of the limit
    unsigned short base_low;        // The lower 16 bits of the base
    unsigned char base_middle;      // The next 8 bits of the base
    unsigned char access;           // Access flags, determine what ring this segment can be used in
    unsigned char granularity;      //
    unsigned char base_high;        // The last 8 bits of the base
} __attribute__((packed));
```

Access byte format:
Type (bit 0:3): Segment type, code/data segment
DT (bit 3:4): Descriptor type
DPL (bit 5:6): Descriptor privilege level, ring 0-3
P (bit 6:7): Is segment present, 1 = Yes, 0 = No

Granularity byte format:
Segment length (bit 0:3): segment length bit 16:19
A (bit 3:4): Available for system use, always 0
0 (bit 4:5): Should always be zero
D (bit 5:6): Operand size, 0 = 16bit, 1 = 32bit
G (bit 6:7): Granularity, 0 = 1 byte, 1 = 1K bytes

The processor can register signal handler (interrupt handler) that deal with the interrupt, then return to the code that aas running before it fired. Interrupts can be fired externally, via IRQs, or internally (int n).

The Interrupt Descriptor Table (IDT) tells the processor where to find handlers for each interrupt. Similar to the GDT, each IDT entry corresponds to an interrupt number (256 in total). If an interrupt occurs and there is no entry for it, the processor will panic and reset

The special, CPU-dedicated interrupts are shown below.

* 0 - Division by zero exception
* 1 - Debug exception
* 2 - Non maskable interrupt
* 3 - Breakpoint exception
* 4 - 'Into detected overflow'
* 5 - Out of bounds exception
* 6 - Invalid opcode exception
* 7 - No coprocessor exception
* 8 - Double fault (pushes an error code)
* 9 - Coprocessor segment overrun
* 10 - Bad TSS (pushes an error code)
* 11 - Segment not present (pushes an error code)
* 12 - Stack fault (pushes an error code)
* 13 - General protection fault (pushes an error code)
* 14 - Page fault (pushes an error code)
* 15 - Unknown interrupt exception
* 16 - Coprocessor fault
* 17 - Alignment check exception
* 18 - Machine check exception
* 19-31 - Reserved

## References and Further Reading
* http://www.osdever.net/bkerndev/index.php
* http://www.jamesmolloy.co.uk/tutorial_html/
* https://samypesse.gitbook.io/how-to-create-an-operating-system/
* https://developer.ibm.com/articles/l-linuxboot/