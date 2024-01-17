extern void VGA_clear(void);
extern void VGA_display_char(char);
// extern void VGA_display_str(const char *);

/* kernel main function */
int kmain() {
    // make sure kmain never returns
    int i, j;
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 80; j++) {
            VGA_display_char(j % 10 + 48);
        }   
        VGA_display_char('\n');
    }
    //VGA_clear();
    asm( "hlt" );
}