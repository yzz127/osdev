#include <system.h>

unsigned int kmalloc_internel(unsigned int size, int align, unsigned int *phys)
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
    unsigned int tmp = start_address;
    start_address += size;
    return tmp;
}

unsigned int kmalloc_a(unsigned int size) {
    return kmalloc_internel(size, 1, NULL);
}

unsigned int kmalloc_p(unsigned int size, unsigned int *phys)
{
    return kmalloc_internel(size, 0, phys);
}

unsigned int kmalloc_ap(unsigned int size, unsigned int *phys)
{
    return kmalloc_internel(size, 1, phys);
}

unsigned int kmalloc(unsigned int size)
{
    return kmalloc_internel(size, 0, NULL);
}
