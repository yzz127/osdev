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
* time
* keyboard
* page fault

## Operating System Notes (To be categorized)

A device is considered bootable if the bootsector contains the valid signature bytes 0x55 and 0xAA at offset 511 and 512 (0x1ff). 

BIOS physically searches for a boot device by loading the first 512 bytes from the bootsector of the each device into physical memory, staring at the address 0x7c00. When the valid signature bytes are detected, BIOS transfers the control to the 0x7c00 memory address (via jmp) to execute the bootsector code.

booting sequence: BIOS->MBR->stage 1 boot loader (partition boot record)->stage 2 boot loader->kernel/initrd(initramfs)->/sbin/init(systemd, pid 1)->/etc/init.d

When GRUB loads, it presents the information from the configuration file (/boot/grub2/grub.cfg)

Many daemon processes communicate using sockets. systemd creates sockets at startup, and only starts the associated tasks when a connection request for services on that socket is received, so as to gain speed and enhance parallelism in the system startup. 

IDE drives formated for MBR use have three types of partition: primary, logical, and extended. The partition table is located in the master boot record (MBR) of a disk. 4 primary partitions, 2TB of maximum disk size are allowed limited by the size of MBR, additional primary partition needs to become an extended partition. Extended partition is a container for one or more logical partitions. Only one extended partition is allowed and it must be contiguous. Data is stored in the logical partition within the extended partition (data can not be stored without first creating a logical partition within the extended partition).

MBR partition type example
```shell
~$ sudo parted -l /dev/sda
Model: ATA SAMSUNG SSD SM87 (scsi)
Disk /dev/sda: 256GB
Sector size (logical/physical): 512B/512B
Partition Table: msdos
Disk Flags:

Number  Start   End    Size    Type      File system     Flags
 1      1049kB  222GB  222GB   primary   ext4            boot
 2      222GB   256GB  34.3GB  extended
 5      222GB   256GB  34.3GB  logical   linux-swap(v1)
```

GUID Partition Table (GPT) layout allows 128 primary partitions, and doesn't use extended or logical partitions. 

Logical Volume Manager (LVM) manages disk space using
* Physical Volumes (PVs)
* Volumes Groups (VGs)
* Logical Volumes (LVs)

PV as the unit of physical space that is aggregated into an abstraction called VG. The VG is then partitioned (4MB extent size) into LVs for use by the file system. 

MBR is the first sector on a hard drive, containing the bootstrap code, some information, 64-byte partition table, and two byte signature. The 64-byte partition table has 4 16-byte entries and starts at offset 446 (1BEH)

Partition table entry format
| Offset (hex) | Length | Description |
|----|:-:|:------------|
| 0h | 1 | Status. 80h indicates active (or bootable) partition. |
| 1h | 3 | CHS (Cylinder-Head-Sector) address of first absolute sector in partition |
| 4h | 1 | Partition type |
| 5h | 3 | CHS address of last absolute sector in partition |
| 8h | 4 | Logical Block Address (LBA) of first absolute sector in partition |
| Ch | 4 | Count of sectors in partition |

Partition types
| Value | Type |
|:---:|:-----|
| 83h | Linux |
| 82h | Linux swap |
| 05h | Extended |
Partition table entries and signature example
```
000001b0: cd10 ac3c 0075 f4c3 bdff 0800 0000 8020  ...<.u.........
000001c0: 2100 83fe ffff 0008 0000 00b0 cf19 00fe  !...............
000001d0: ffff 05fe ffff febf cf19 0270 ff03 0000  ...........p....
000001e0: 0000 0000 0000 0000 0000 0000 0000 0000  ................
000001f0: 0000 0000 0000 0000 0000 0000 0000 55aa  ..............U.
```
Active partition starts from 00000800h and extend 19cfb000h, ending sector at 2048 + 433041408 - 1 = 433043455
```shell
$ sudo fdisk -l /dev/sda
Disk /dev/sda: 238.5 GiB, 256060514304 bytes, 500118192 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x0008ffbd

Device     Boot     Start       End   Sectors   Size Id Type
/dev/sda1  *         2048 433043455 433041408 206.5G 83 Linux
/dev/sda2       433045502 500117503  67072002    32G  5 Extended
/dev/sda5       433045504 500117503  67072000    32G 82 Linux swap / Solaris
```
MBR doesn't contain partition table entries for the logical partition. Each logical partition within the extended partition contains and Extended Boot Record (EBR). EBR is 512-byte in length, and uses a partition table at offset 446 (1BEh). Only two entries in the EBR partition table are used. The first defines the offset and size of the current partition; and the second defines the offset and count to the end of the **next** logical partition (all zeros for the last logical partition).

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

```c
struct idt_entry
{
    unsigned short base_lo;     // The lower 16 bits of the address to jump to when the interrupt fires
    unsigned short sel;         // Kernel segment selector
    unsigned char always0;      // Always zero
    unsigned char flags;
    unsigned short base_hi;     // The upper 16 bits of the address to jump to
}
```

Flags byte format:
00110 (bit 0:4): always 00110 (decimal 10)
DPL (bit 5:6): privilege level
P (bit 7): An entry to handle interrupt is present. Any descriptor with this bit clear will cause a "Interrupt Not Handled" exception

Interrupt Requests (IRQs)

Two methods for os to communicate with external devices
* Polling: Spin in a loop, occasionally checking if the device is ready
* Interrupts: Causing a CPU interrupt when device is ready, and interrupt handler will run

All devices that are interrupt-capable have line connecting them to the PIC. The PIC is the only device that is directly connected to the CPU's interrupt pin. Two PICs (master and slave) are in modern PC, serving a total of 15 interruptable devices (one line is sued to signal the slave PIC)

PIC mappings can be remapped, default mappings are:
* IRQ 0:7 - INT 0x8:0xF
* IRQ 8:15 - INT 0x70:0x77

To avoid conflict with CPU signal exceptions, IRQs 0:15 usually gets remapped to ISRs 32:47

Communicating with PICs:
* Master: control port (0x20), data port (0x21)
* Slave: control port (0xA0), data port (0xA1)

Programmable Interval Timer (PIT) is a chip connected to IRQ0. It can interrupt the CPU at a user-defined rate (18.2HZ and 1.1931MHZ). The PIT is the primary method used for implementing a system clock. Internal clock oscillates at around 1.1931MHz, and is fed with frequency divider to module the final output frequency.

Three channels:
* Channel 0: Output is connected to IRQ0
* Channel 1: Used to control refresh rates for DRAM (no longer implemented)
* Channel 2: Controls the PC speaker

Communicating with PIT:
control port (0x43), channel 0 data port (0x40), channel 1 data port (0x41), channel 2 data port (0x42)

Paging is used for memory protection and virtual memory. Virtual address space is usually split into 4K (0x1000) blocks called pages. Pages can be mapped on to frames of same block size to physical memory.

Page table entry format:
P: (bit 0): Set if the page is present in memory
R/W: (bit 1): Set if the page is writeable, unset if the page is read-only. Not applied when code is running in kernel-mode
U/S (bit 2): Set if the page is in user-mode, unset if the page is in supervisor(kernel)-mode. User-mode code can not write to or read from kernel-mode pages
Reserved (bit 3:4): Used by CPU internally
A (bit 5): Set if the page has been accessed (set by the CPU)
D (bit 6): Set if the page has been written to (become dirty)
Reserved (bit 7:8): Used by CPU internally
AVAIL (bit 9:11): Unused and available for kernel-use
Page frame address (bit 12:31): The high 20 bits of the frame address in physical memory.

Enabling paging:
1. Copy the location of the page directory into the CR3 register
2. Set the PG bit in the CR0 register

Page faults can be caused:
* Reading from or writing to an area of memory that is not mapped ('present' flag is not set)
* The process is in user-mode and tries to write to a read-only page
* The process is in user-mode and tries to access a kernel-only page
* The page table entry is corrupted (the reserved bits have been overwritten)

The page fault interrupt number is 14, the address that caused the fault will be saved to CR2 register. The error code represents:
* bit 0: the fault was caused by page not present (0) or some other reasons (1)
* bit 1: the fault was caused by operation of writing (1), or reading (0)
* bit 2: the processor was running in user-mode (1), or kernel-mode (0)
* bit 3: the fault was caused by reserved bits being overwritten (1)
* bit 4: the fault occurred during an instruction fetch (1)

Sysfs is a virtual filesystem that the Linux kernel uses to export information about kernel objects to processes running in user space. As a virtual filesystem, sysfs is an in-memory filesystem that is mounted at /sys

Shared libraries
```shell
$ ldd /bin/ln
	linux-vdso.so.1 =>  (0x00007fffde75d000) # Making the fastest syscall available on the system, either through interrupt or Fast System Call (SYSENTER, SYSEXIT)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f74b8336000) # Dynamically linked library
	/lib64/ld-linux-x86-64.so.2 (0x00007f74b8700000) # ld binary executable for dynamic loading and linking
```

## References and Further Reading
* http://www.osdever.net/bkerndev/index.php
* http://www.jamesmolloy.co.uk/tutorial_html/
* https://samypesse.gitbook.io/how-to-create-an-operating-system/
* https://developer.ibm.com/technologies/linux/series/learn-linux-101