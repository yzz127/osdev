#include <system.h>

int timer_ticks = 0;

void timer_handler(struct regs *r)
{
    timer_ticks++;
    if (timer_ticks % 18 == 0)
    {
        puts("One second has passed!\n");
    }
}

void timer_install()
{
    irq_install_handler(0, timer_handler);
}

void timer_wait(int ticks)
{
    unsigned long eticks;

    eticks = timer_ticks + ticks;
    while (timer_ticks < eticks)
        ;
}