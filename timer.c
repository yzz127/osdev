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

void timer_install(uint32_t frequency)
{
    // irq_install_handler(0, timer_handler);
    
    uint32_t divisor = 1193180 / frequency;
    outportb(0x43, 0x36);
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

    outportb(0x40, l);
    outportb(0x40, h);
    
}

void timer_wait(int32_t ticks)
{
    uint64_t eticks;

    eticks = timer_ticks + ticks;
    while (timer_ticks < eticks)
        ;
}