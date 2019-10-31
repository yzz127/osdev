#include <system.h>

int timer_ticks = 0;

void timer_handler(register_t *r)
{
    timer_ticks++;
    if (timer_ticks % 18 == 0)
    {
        puts("One system second has passed!\n");
    }
}

void timer_install(unsigned int frequency)
{
    irq_install_handler(0, timer_handler);
    
    unsigned int divisor = 1193180 / frequency;
    outportb(0x43, 0x36);
    unsigned char l = (unsigned char)(divisor & 0xFF);
    unsigned char h = (unsigned char)((divisor >> 8) & 0xFF);

    outportb(0x40, l);
    outportb(0x40, h);
    
}

void timer_wait(int ticks)
{
    unsigned long eticks;

    eticks = timer_ticks + ticks;
    while (timer_ticks < eticks)
        ;
}