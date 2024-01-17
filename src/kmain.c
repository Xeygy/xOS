extern void VGA_clear(void);
extern void VGA_display_char(char);
extern void VGA_display_str(const char *);

/* kernel main function */
int kmain() {
    // make sure kmain never returns
    int i, j;
    for (i = 0; i < 10; i++) {
        VGA_display_str("Hello World!\n");
    }
    asm( "hlt" );
}