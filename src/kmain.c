extern void VGA_clear(void);
extern void VGA_display_char(char);
extern void VGA_display_str(const char *);

/* kernel main function */
int kmain() {
    int i, j;
    char num1, num2;
    if (sizeof(int) == 4) {
        VGA_display_str("eight!");
    }
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 10; j++) {
            num1 = i % 10 + 48;
            num2 = j % 10 + 48;
            VGA_display_str("Hello World ");
            VGA_display_char(num1);
            VGA_display_char(num2);
            VGA_display_str("\n");
        }
    }
    asm( "hlt" );
}