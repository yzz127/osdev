#include <system.h>

typedef struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__ ((packed)) gdt_ptr_t;

gdt_entry_t gdt[5];
gdt_ptr_t gp;

extern void gdt_flush();

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install()
{
    gp.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gp.base = &gdt;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment

    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment, 0x08 offset from GDT base

    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment, 0x10 offset from GDT base

    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment

    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

    gdt_flush();
}