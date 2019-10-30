#include <system.h>

unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    while (count > 0) {
        *dp++ = *sp++;
        count--;
    }
    return dest;

}

unsigned char *memset(unsigned char *dest, unsigned char val, int count)
{
    char *temp = (char *)dest;
    while (count > 0) {
        *temp++ = val;
        count--;
    }
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count)
{
    unsigned short *temp = (unsigned short *)dest;
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

unsigned char inportb(unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb(unsigned short _port, unsigned char _data)
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