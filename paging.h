#ifndef PAGING_H
#define PAGING_H

#include <system.h>

typedef struct page
{
    unsigned int present    : 1;    // Page present
    unsigned int rw         : 1;    // 0 for read-only, 1 for read-write
    unsigned int user       : 1;    // 0 for user-mode, 1 for kernel-mode
    unsigned int accessed   : 1;    // Check page has been accessed since last refresh
    unsigned int dirty      : 1;    // Check page has been written since last refresh
    unsigned int unused     : 7;    // Unused and reserved bits
    unsigned int frame      : 20;   // Frame address
} page_t;

typedef struct page_table
{
    page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
    page_table_t *tables[1024];
    unsigned int tablesPhysical[1024];
    unsigned int physicalAddr;
} page_directory_t;

void init_paging();

void switch_page_directory(page_directory_t *new);

struct page *get_page(unsigned int address, int make, page_directory_t *dir);

void page_fault(register_t regs);

#endif