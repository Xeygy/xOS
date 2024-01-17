// extern void VGA_clear(void);
extern void VGA_display_char(char);
// extern void VGA_display_str(const char *);

/* kernel main function */
int kmain() {
    // make sure kmain never returns
    while (1) {
        VGA_display_char('H');
        VGA_display_char('e');
        VGA_display_char('l');
        VGA_display_char('l');
        VGA_display_char('o');
        VGA_display_char('!');
        asm( "hlt" );
    }
}