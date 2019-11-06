#include <system.h>

uint16_t *textmemptr;
int32_t attrib = 0x0F;
int32_t csr_x = 0; 
int32_t csr_y = 0;

void scroll(void)
{
    uint16_t blank, temp;

    blank = 0x20 | (attrib << 8);

    if (csr_y >= 25)
    {
        temp = csr_y - 25 + 1;
        memcpy(textmemptr, textmemptr + temp * 80, (25 - temp) * 80 * 2);
        memsetw(textmemptr + (25 - temp) * 80, blank, 80);
        csr_y = 25 - 1;
    }
}

void move_csr(void)
{
    uint16_t temp;
    temp = csr_y * 80 + csr_x;

    outportb(0x3D4, 14);
    outportb(0x3D5, temp >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, temp);
}

void cls()
{
    uint16_t blank;
    int32_t i;

    blank = 0x20 | (attrib << 8);

    for (i = 0; i < 25; i++)
        memsetw(textmemptr + i * 80, blank, 80);

    csr_x = 0;
    csr_y = 0;
    move_csr();
}

void putch(char c)
{
    uint16_t *where;
    uint16_t att = attrib << 8;

    if (c == 0x08)
    {
        if (csr_x != 0)
            csr_x--;
    }
    else if (c == 0x09)
    {
        csr_x = (csr_x + 8) & ~(8 - 1);
    }
    else if (c == '\r')
    {
        csr_x = 0;
    }
    else if (c == '\n')
    {
        csr_x = 0;
        csr_y++;
    }
    else if (c >= ' ')
    {
        where = textmemptr + (csr_y * 80 + csr_x);
        *where = c | att;
        csr_x++;
    }

    if (csr_x >= 80)
    {
        csr_x = 0;
        csr_y++;
    }

    scroll();
    move_csr();
}

void puts(char *text)
{
    int32_t i;
    for (i = 0; i < strlen(text); i++)
        putch(text[i]);
}

void write_hex(uint32_t n)
{
    int32_t tmp;
    puts("0x");

    char noZeroes = 1;

    int32_t i;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && noZeroes != 0)
        {
            continue;
        }

        if (tmp >= 0xA)
        {
            noZeroes = 0;
            putch(tmp - 0xA + 'a');
        }
        else
        {
            noZeroes = 0;
            putch(tmp + '0');
        }
    }

    tmp = n & 0xF;
    if (tmp >= 0xA)
    {
        putch(tmp - 0xA + 'a');
    }
    else
    {
        putch(tmp + '0');
    }   
}

void write_dec_str(uint32_t n)
{
    if (n == 0)
    {
        puts('0');
        return;
    }

    int32_t acc = n;
    char c[32];
    int32_t i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc % 10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    int32_t j = 0;
    while (i >= 0)
    {
        c2[i--] = c[j++];
    }
    puts(c2);
}

void settextcolor(uint8_t forecolor, uint8_t backcolor)
{
    attrib = (backcolor << 4) | (forecolor & 0x0F);
}

void init_video(void)
{
    textmemptr = (uint16_t *)0xB8000;
    cls();
}