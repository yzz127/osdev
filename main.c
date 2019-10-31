#include <system.h>
#include <paging.h>

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

void panic(const char *message, const char *file)
{
    __asm__ __volatile__("cli");

    puts("PANIC(");
    puts(message);
    puts(") at ");
    puts(file);
    puts("\n");
    for(;;);
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
    init_paging();
    __asm__ __volatile__("sti");
    puts("System Start!\n");

    uint32_t *ptr = (uint32_t *)0xA0000000;
    uint32_t do_page_fault = *ptr;
    puts(do_page_fault);

    // int i = 8 / 0;
    // puts(i);
    // asm volatile ("int $0x3");
    for(;;);
}