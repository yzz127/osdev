#ifndef KHEAP_H
#define KHEAP_H

#include <system.h>

uint32_t kmalloc_internel(uint32_t size, int align, uint32_t *phys);
uint32_t kmalloc_a(uint32_t size);
uint32_t kmalloc_p(uint32_t size, uint32_t *phys);
uint32_t kmalloc_ap(uint32_t size, uint32_t *phys);
uint32_t kmalloc(uint32_t size);

#endif