// extern void VGA_clear(void);
// extern void VGA_display_char(char);
// extern void VGA_display_str(const char *);
#include "vga.h"

#define VGA_BASE 0xb8000
#define FG(color) (color)
#define BG(color) (color << (4))
#define LINE(ch_idx) (ch_idx / 80)

static unsigned short *vgaBuff = (unsigned short*)VGA_BASE;
static int width = 80;
static int height = 20;
static int cursor = 0;
static unsigned char color = FG(VGA_WHITE) | BG(VGA_BROWN);

/* 
    Clears the screen
*/
void VGA_clear(void) 
{
    int r, c;
    for (r = 0; r < height; c++) {
        for (c = 0; c < width; r++) {
            vgaBuff[r * width + c] = 0;
        }
    }
    cursor = 0;
}

/* 
    Displays char c on the next available space on the
    current line. If the line is not wide enough, overwrites
    the existing last character with c. Supports \n and \r
*/
void VGA_display_char(char c) {
    if (c == '\n') {
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