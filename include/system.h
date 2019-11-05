#ifndef __SYSTEM_H
#define __SYSTEM_H

#define NULL 0

typedef signed char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef void * type_t;

/* MAIN.C */
extern uint8_t *memcpy(uint8_t *dest, const uint8_t *src, int count);
extern uint8_t *memset(uint8_t *dest, uint8_t val, int count);
extern uint16_t *memsetw(uint16_t *dest, uint16_t val, int count);
extern int strlen(const char *str);
extern uint8_t inportb(uint16_t _port);
extern void outportb(uint16_t _port, uint8_t _data);
extern void panic(const char *message, const char *file);

#define PANIC(msg) panic(msg, __FILE__);

extern void cls();
extern void putch(uint8_t c);
extern void puts(uint8_t *str);
extern void settextcolor(uint8_t forecolor, uint8_t backcolor);
extern void init_video();

extern void gdt_install();
extern void idt_install();
extern void isrs_install();

typedef struct regs
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} register_t;

void irq_install_handler(int irq, void (*handler)(register_t *r));
void irq_uninstall_handler(int irq);
void irq_install();

void irs_install_handler(int irs, void (*handler)(register_t *r));

void timer_install(uint32_t frequency);

void keyborad_install();

#endif