#include <stddef.h>
#include "vga.h"
#include "string.h"
#include "asm.h"
#include "interrupts.h"

#define VGA_BASE 0xb8000
#define FG(color) (color)
#define BG(color) (color << (4))
#define LINE(ch_idx) (ch_idx / 80)

static unsigned short *vgaBuff = (unsigned short*)VGA_BASE;
static int width = 80;
static int height = 25;
static int cursor = 0;
static unsigned char color = FG(VGA_WHITE) | BG(VGA_BROWN);

void scroll();
void set_cursor(int offset);

/* 
    Clears the screen.
*/
void VGA_clear(void) 
{
    int r, c;
    for (r = 0; r < height; r++) {
        for (c = 0; c < width; c++) {
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
    int defer_enable = 0;
	if (interrupts_enabled) {
		defer_enable = 1;
		disable_interrupts();
	}

    if (c == '\n') {
        cursor = (LINE(cursor) + 1) * width;
        if (cursor >= width*height) {
            scroll(); 
            cursor -= width;
        }            
    }
    else if (c == '\r')
        cursor = LINE(cursor);
    else {
        vgaBuff[cursor] = (color << 8) | c;
        if ( (cursor % width) < (width - 1))
            cursor++; 
    }
    set_cursor(cursor);

    if (defer_enable)
        enable_interrupts();
}

/*
    Displays chars starting at p until it reaches a null char.
*/
void VGA_display_str(const char *p) {
    char curr = *p;
    while (curr != '\0') {
        VGA_display_char(curr);
        curr = *(++p);
    }
}

/*
    Changes the text output color
*/
void VGA_set_color(vga_color_t fg, vga_color_t bg) {
    color = FG(fg) | BG(bg);
}

/*
    scrolls the entire screen up by 1 row. deletes the first row.
    does not move cursor.
*/
void scroll() {
    int r;
    unsigned short *curr_r, *next_r;
    for (r = 0; r < height - 1; r++) {
        curr_r = vgaBuff + r * width;
        next_r = vgaBuff + (r + 1) * width;
        memcpy(curr_r, next_r, width * sizeof(unsigned short)); 
    }
    memset(next_r, 0, width * sizeof(unsigned short));
}

/*
    sets cursor position at location.
    see: https://wiki.osdev.org/Text_Mode_Cursor#Moving_the_Cursor_2
*/
void set_cursor(int offset)
{
    outb(0x3D4, 0x0F);
    outb(0x3D5, (char) (offset & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (char) ((offset >> 8) & 0xFF));
}
