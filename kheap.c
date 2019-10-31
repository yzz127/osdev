#include <system.h>
#include <kheap.h>

extern uint32_t end;
uint32_t start_address = (uint32_t)&end;

uint32_t kmalloc_internel(uint32_t size, int align, uint32_t *phys)
{
    if (align == 1 && (start_address & 0xFFFFF000))
    {
        start_address &= 0xFFFFF000;
        start_address += 0x1000;
    }
    if (phys)
    {
        *phys = start_address;
    }
    uint32_t tmp = start_address;
    start_address += size;
    return tmp;
}

uint32_t kmalloc_a(uint32_t size) {
    return kmalloc_internel(size, 1, NULL);
}

uint32_t kmalloc_p(uint32_t size, uint32_t *phys)
{
    return kmalloc_internel(size, 0, phys);
}

uint32_t kmalloc_ap(uint32_t size, uint32_t *phys)
{
    return kmalloc_internel(size, 1, phys);
}

uint32_t kmalloc(uint32_t size)
{
    return kmalloc_internel(size, 0, NULL);
}
