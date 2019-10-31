#include <paging.h>

unsigned int *frames;
unsigned int nframes;

extern unsigned int start_address;

#define INDEX_FROM_BIT(a)   (a / (8 * 4))
#define OFFSET_FROM_BIT(a)  (a % (8 * 4))

static void set_frame(unsigned int frame_addr)
{
    unsigned int frame = frame_addr / 0x1000;
    unsigned int idx = INDEX_FROM_BIT(frame);
    unsigned int off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

static void clear_frame(unsigned frame_addr)
{
    unsigned int frame = frame_addr / 0x1000;
    unsigned int idx = INDEX_FROM_BIT(frame);
    unsigned int off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

static void test_frame(unsigned frame_addr)
{
    unsigned int frame = frame_addr / 0x1000;
    unsigned int idx = INDEX_FROM_BIT(frame);
    unsigned int off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

static unsigned int first_frame()
{
    unsigned int i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF)
        {
            for (j = 0; j < 32; j++)
            {
                unsigned int toTest = 0x1 << j;
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
        unsigned int idx = first_frame();
        if (idx == (unsigned int)-1)
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
    unsigned int frame;
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