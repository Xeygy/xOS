// extern void VGA_clear(void);
// extern void VGA_display_char(char);
// extern void VGA_display_str(const char *);
#include "vga.h"

#define VGA_BASE 0xb8000
static unsigned short *vgaBuff = (unsigned short*)VGA_BASE;
static int width = 80;
static int height = 20;
static int cursor = 0;
static unsigned char color = FG(VGA_WHITE) | BG(VGA_BROWN);

void VGA_display_char(char c)
{
    if (c == '\n') 
    {
        cursor = (LINE(cursor) + 1) * width;
        //if (cursor >= width*height)
        //    scroll(); 
    }
    else if (c == '\r')
        cursor = LINE(cursor);
    else {
        vgaBuff[cursor] = (color << 8) | c;
        if ( (cursor % width) < (width - 1))
            cursor++; 
    }
}