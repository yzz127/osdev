#ifndef PAGING_H
#define PAGING_H

#include <system.h>

typedef struct page
{
    uint32_t present    : 1;    // Page present
    uint32_t rw         : 1;    // 0 for read-only, 1 for read-write
    uint32_t user       : 1;    // 0 for user-mode, 1 for kernel-mode
    uint32_t accessed   : 1;    // Check page has been accessed since last refresh
    uint32_t dirty      : 1;    // Check page has been written since last refresh
    uint32_t unused     : 7;    // Unused and reserved bits
    uint32_t frame      : 20;   // Frame address
} page_t;

typedef struct page_table
{
    page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
    page_table_t *tables[1024];
    uint32_t tablesPhysical[1024];
    uint32_t physicalAddr;
} page_directory_t;

void init_paging();

void switch_page_directory(page_directory_t *new);

struct page *get_page(uint32_t address, int make, page_directory_t *dir);

void page_fault(register_t regs);

#endif