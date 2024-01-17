#ifndef VGA_H
#define VGA_H

typedef enum {
	VGA_BLACK,
	VGA_BLUE,
	VGA_GREEN,
	VGA_CYAN,
	VGA_RED,
	VGA_MAGENTA,
	VGA_BROWN,
	VGA_LIGHT_GREY,
	VGA_DARK_GREY,
	VGA_LIGHT_BLUE,
	VGA_LIGHT_GREEN,
	VGA_LIGHT_CYAN,
	VGA_LIGHT_RED,
	VGA_LIGHT_MAGENTA,
	VGA_LIGHT_BROWN,
	VGA_WHITE
} vga_color_t;

#define FG(color) (color)
#define BG(color) (color << (4))
#define LINE(ch_idx) (ch_idx / 80)

#endif