#include <system.h>

uint8_t *memcpy(uint8_t *dest, const uint8_t *src, int count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    while (count > 0) {
        *dp++ = *sp++;
        count--;
    }
    return dest;

}

uint8_t *memset(uint8_t *dest, uint8_t val, int count)
{
    char *temp = (char *)dest;
    while (count > 0) {
        *temp++ = val;
        count--;
    }
    return dest;
}

uint16_t *memsetw(uint16_t *dest, uint16_t val, int count)
{
    uint16_t *temp = (uint16_t *)dest;
    while (count > 0) {
        *temp++ = val;
        count--;
    }
    return dest;
}

int strlen(const char *str)
{
    int retval;
    for (retval = 0; *str != '\0'; str++)
        retval++;
    return retval;
}

uint8_t inportb(uint16_t _port)
{
    uint8_t rv;
    __asm__ __volatile__("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb(uint16_t _port, uint8_t _data)
{
    __asm__ __volatile__("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void main()
{
    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
    timer_install(10);
    keyborad_install();
    init_video();
    __asm__ __volatile__("sti");
    puts("System Start!\n");
    // int i = 8 / 0;
    // puts(i);
    // asm volatile ("int $0x3");
    // asm volatile ("int $0x4");
    for(;;);
}