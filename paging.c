#include <paging.h>
#include <kheap.h>
#include <system.h>

page_directory_t *kernel_directory = 0;
page_directory_t *current_directory = 0;

uint32_t *frames;
uint32_t nframes;

extern uint32_t start_address;
extern heap_t *kheap;

#define INDEX_FROM_BIT(a)   (a / (8 * 4))
#define OFFSET_FROM_BIT(a)  (a % (8 * 4))

static void set_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

static void clear_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

static uint32_t test_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

static uint32_t first_frame()
{
    uint32_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF)
        {
            for (j = 0; j < 32; j++)
            {
                uint32_t toTest = 0x1 << j;
                if (!(frames[i] & toTest))
                {
                    return i * 4 * 8 + j;
                }
            }
        }
    }
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
    if (page->frame != 0)
    {
        return;
    }
    else
    {
        uint32_t idx = first_frame();
        if (idx == (uint32_t)-1)
        {
            PANIC("No free frames!");
        }
        set_frame(idx * 0x1000);
        page->present = 1;
        page->rw = (is_writeable)? 1:0;
        page->user = (is_kernel)? 0:1;
        page->frame = idx;
    }
    
}

void free_frame(page_t *page)
{
    uint32_t frame;
    if (!(frame = page->frame))
    {
        return;
    }
    else
    {
        clear_frame(frame);
        page->frame = 0x0;
    }
}

void init_paging()
{
    uint32_t mem_end_page = 0x1000000; // The size of the physical memory is assumed to be 16MB

    nframes = mem_end_page / 0x1000;
    frames = (uint32_t *)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));

    kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;

    int i = 0;
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000)
        get_page(i, 1, kernel_directory);

    i = 0;
    while (i < start_address + 0x1000)
    {
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }

    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000)
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);

    irs_install_handler(14, &page_fault_handler);

    switch_page_directory(kernel_directory);

    kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    __asm__ __volatile__("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;  // Enable paging
    __asm__ __volatile__("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir)
{
    address /= 0x1000;
    uint32_t table_idx = address / 1024;
    if (dir->tables[table_idx])
    {
        return &dir->tables[table_idx]->pages[address % 1024];
    }
    else if (make)
    {
        uint32_t tmp;
        dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(dir->tables[table_idx], 0, 0x1000);
        dir->tablesPhysical[table_idx] = tmp | 0x7; // Page present, rw, user-mode
        return &dir->tables[table_idx]->pages[address % 1024];
    }
    else
    {
        return 0;
    }
}

void page_fault_handler(register_t regs)
{
    puts("Calling page fault handler");
    uint32_t faulting_address;
    __asm__ __volatile__("mov %%cr2, %0" : "=r" (faulting_address));

    int present = !(regs.err_code & 0x1);
    int rw = regs.err_code & 0x2;
    int us = regs.err_code & 0x4;
    int reserved = regs.err_code & 0x8;
    int id = regs.err_code & 0x10;

    puts("Page fault! (");
    if (present)
        puts("present ");
    if (rw)
        puts("read-only ");
    if (us)
        puts("user-mode ");
    if (reserved)
        puts("reserved ");
    puts(") at 0x");
    // puts(faulting_address);
    puts("\n");
    PANIC("Page fault");
}