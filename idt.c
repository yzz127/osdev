#include <system.h>

typedef struct idt_entry
{
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__ ((packed)) idt_entry_t;

typedef struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed)) idt_ptr_t;

idt_entry_t idt[256];
idt_ptr_t idtp;

extern void idt_load();

void idt_set_gate(uint8_t num, unsigned long base, uint16_t sel, uint8_t flags)
{
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].always0 = 0;
    idt[num].sel = sel;
    idt[num].flags = flags;
}

void idt_install()
{
    idtp.limit = (sizeof(idt_entry_t) * 256) - 1;
    idtp.base = &idt;

    memset(&idt, 0, sizeof(idt_entry_t) * 256);

    idt_load();
}