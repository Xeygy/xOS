#include <stdint.h>
#include "print.h"
#include "asm.h"
#include "string.h"
#include "proc.h"
#include "interrupts.h"
#include "kmalloc.h"

#define KB_BUFF_SIZE 128

#define PS2_DATA  0x60
#define PS2_CMD 0x64
#define PS2_STATUS PS2_CMD
#define PS2_STATUS_OUTPUT 1
#define PS2_STATUS_INPUT (1 << 1)
#define WAIT_FOR_OUTPUT while(!(inb(PS2_STATUS) & PS2_STATUS_OUTPUT))
#define WAIT_FOR_EMPTY_IN  while(inb(PS2_STATUS) & PS2_STATUS_INPUT)

#define LSHIFT  0x12 // scan code set 2
#define RSHIFT  0x59 // scan code set 2
#define CAPSLOCK  0x58 // scan code set 2
 
// controller config byte
typedef struct PS2_ccb {
    uint8_t port1_irq:1;
    uint8_t port2_irq:1;
    uint8_t system_flag:1;
    uint8_t zero1:1;
    uint8_t port1_clock:1;
    uint8_t port2_clock:1;
    uint8_t port1_trans:1;
    uint8_t zero2:1;
} __attribute__((packed)) PS2_ccb;

static int init_controller();
static int init_keyb();

static int init_ctlr_success, ready_to_poll, capslock_on;
static int released_key;  // true if previous code was 0xF0
/* scan code set 2 */
static char scodes2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '`', 0,
                        0, 0, 0, 0, 0, 'q', '1', 0, 0, 0, 'z', 's', 'a', 'w', '2', 0,
                        0, 'c', 'x', 'd', 'e', '4', '3', 0, 0, ' ', 'v', 'f', 't', 'r', '5', 0,
                        0, 'n', 'b', 'h', 'g', 'y', '6', 0 ,0, 0, 'm', 'j', 'u', '7', '8', 0,
                        0, ',', 'k', 'i', 'o', '0', '9', 0, 0, '.', '/', 'l', ';', 'p', '-', 0,
                        0, 0, '\'', 0, '[', '=', 0, 0, 0, 0, '\n', ']', 0, '\\', 0, 0,
                        0, 0, 0, 0, 0, 0, '\b', 0, 0, /*keypad*/ '1', 0, '4', '7', 0, 0, 0,
                        '0', '.', '2', '5', '6', '8', 0, 0, 0, '+', '3', '-', '*', '9', 0, 0};
/* uppercase values */
static char scodes2_up[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '~', 0,
                        0, 0, 0, 0, 0, 'Q', '!', 0, 0, 0, 'Z', 'S', 'A', 'W', '@', 0,
                        0, 'C', 'X', 'D', 'E', '$', '#', 0, 0, ' ', 'V', 'F', 'T', 'R', '%', 0,
                        0, 'N', 'B', 'H', 'G', 'Y', '^', 0 ,0, 0, 'M', 'J', 'U', '&', '*', 0,
                        0, '<', 'K', 'I', 'O', ')', '(', 0, 0, '>', '?', 'L', ':', 'P', '_', 0,
                        0, 0, '"', 0, '{', '+', 0, 0, 0, 0, '\n', '}', 0, '|', 0, 0,
                        0, 0, 0, 0, 0, 0, '\b', 0, 0, /*keypad*/ '1', 0, '4', '7', 0, 0, 0,
                        '0', '.', '2', '5', '6', '8', 0, 0, 0, '+', '3', '-', '*', '9', 0, 0};

/* 0 if scan code is depressed */
static char scodes_down[sizeof(scodes2)];
static thread_q *kb_block_q;
static char kb_buff[KB_BUFF_SIZE]; // circular queue
static int kb_read = 0, kb_write = 0;

/* 
    sets up ps2 controller and keyboard on port 1
    returns 0 on success, other on failure
*/
int init_ps2() {
    int error;
    if (ready_to_poll)
        return 0;
    if (!init_ctlr_success && (error = init_controller()))
        return error;
    init_ctlr_success = 1;

    if ((error = init_keyb()))
        return error;
    ready_to_poll = 1;
    return 0;
}

/* 
read from the ps2 device, assumes there exists data in the data port 
(i.e. interrupt driven) 
returns 0 if not a printable char.
*/
char ps2_read()
{
    uint8_t res;
    int error;
    char output = 0;
    // init_ps2 if not initialized
    if (!ready_to_poll && (error = init_ps2())) {
        printk("Couldn't set up ps\\2 controller: %i", error);
        return 0;
    }
    if (!(inb(PS2_STATUS) & PS2_STATUS_OUTPUT)) {
        printk("called read but nothing in ps\\2 data");
        return 0;
    }

    res = inb(PS2_DATA);
    if (res == 0xF0) {
        released_key = 1;
    } else if (released_key) {
        // handle releases
        if (res == CAPSLOCK) 
            capslock_on = !capslock_on;
        if (res < sizeof(scodes2))
            scodes_down[res] = 0;
        released_key = 0;
    } else if (res < sizeof(scodes2) && 
                !scodes_down[res]) { // only print char if key has been released 
        if (scodes_down[LSHIFT] || scodes_down[RSHIFT] || capslock_on) 
            output = scodes2_up[res];
        else
            output = scodes2[res];
        scodes_down[res] = 1;
    }
    return output;
}

/*
    Sets up keyboard on port 1
    returns 0 on success, other on failure
*/
static int init_keyb() {
    uint8_t test_res;
    PS2_ccb ccb;

    if (!init_ctlr_success) 
        return 1;
    // enable device on port 1
    outb(PS2_CMD, 0xAE);
    // reset device
    WAIT_FOR_EMPTY_IN;
    outb(PS2_DATA, 0xFF);
    WAIT_FOR_OUTPUT;
    test_res = inb(PS2_DATA);   
    if (test_res != 0xFA) {
        printk("PS/2 port 1 device reset failed, expected 0xFA, got %hx\n", test_res);
        return 1;
    } else {
        dprintk(DPRINT_DETAILED, "PS/2 port 1 device reset success! got %hx\n", test_res);
    }  

    // set scancode to 2 (default)
    WAIT_FOR_EMPTY_IN;
    outb(PS2_DATA, 0xF0);
    WAIT_FOR_EMPTY_IN;
    outb(PS2_DATA, 0x02);
    // check that it's 2
    WAIT_FOR_EMPTY_IN;
    outb(PS2_DATA, 0xF0);
    WAIT_FOR_EMPTY_IN;
    outb(PS2_DATA, 0x00);
    WAIT_FOR_OUTPUT;
    do {
        test_res = inb(PS2_DATA);
    }
    while (test_res == 0xfa);
    if (test_res != 0x02) {
        printk("failed to set scancode 2, expected 0x02, got %hx\n", test_res);
        return 1;
    } else {
        dprintk(DPRINT_DETAILED, "set scancode 2 success! got %hx\n", test_res);
    }  

    // enable interrupts on device 1
    outb(PS2_CMD, 0x20); 
    test_res = inb(PS2_DATA);
    memcpy(&ccb, &test_res, 1); // copy in byte to ccb
    ccb.port1_irq = 1;
    WAIT_FOR_EMPTY_IN;
    outb(PS2_CMD, 0x60);
    outb(PS2_DATA, *(uint8_t *) &ccb); // convert ccb to 1 byte
    return 0;
}

/* 
    set up ps2 controller, leaves both ports disabled
    returns 0 on success, other on failure
    following steps from 
    https://wiki.osdev.org/%228042%22_PS/2_Controller#Initialising_the_PS.2F2_Controller
*/
static int init_controller() {
    PS2_ccb ccb;
    int is_dual_channel;
    uint8_t test_res;

    // disable devices
    outb(PS2_CMD, 0xAD); // disable first port
    outb(PS2_CMD, 0xA7); // disable second port

    // flush the output buffer
    inb(PS2_DATA); // is this correct?

    // set the controller configuration byte
    outb(PS2_CMD, 0x20); 
    test_res = inb(PS2_DATA);
    memcpy(&ccb, &test_res, 1); // copy in byte to ccb
    
    // disable interrupts and translation
    ccb.port1_irq = ccb.port2_irq = ccb.port1_trans = 0;
    // for port clock, enabled = 0
    is_dual_channel = !ccb.port1_clock;
    // write to the ccb
    WAIT_FOR_EMPTY_IN;
    outb(PS2_CMD, 0x60);
    outb(PS2_DATA, *(uint8_t *) &ccb); // convert ccb to 1 byte

    // perform controller self test
    outb(PS2_CMD, 0xAA);
    // wait for status output to be true
    WAIT_FOR_OUTPUT;
    test_res = inb(PS2_DATA);
    if (test_res != 0x55) {
        printk("PS/2 self test failed, expected 0x55, got %hx\n", test_res);
        return 1;
    } else {
        dprintk(DPRINT_DETAILED, "PS/2 self test success! got %hx\n", test_res);
    }

    // perform interface tests
    // port 1
    outb(PS2_CMD, 0xAB);
    WAIT_FOR_OUTPUT;
    test_res = inb(PS2_DATA);   
    if (test_res != 0x00) {
        printk("PS/2 port 1 test failed, expected 0x00, got %hx\n", test_res);
        return 1;
    } else {
        dprintk(DPRINT_DETAILED, "PS/2 port 1 test success! got %hx\n", test_res);
    } 
    // port 2
    if (is_dual_channel) {
        outb(PS2_CMD, 0xA9);
        WAIT_FOR_OUTPUT;
        test_res = inb(PS2_DATA);   
        if (test_res != 0x00) {
            printk("PS/2 port 2 test failed, expected 0x00, got %hx\n", test_res);
        } else {
            dprintk(DPRINT_DETAILED, "PS/2 port 2 test success! got %hx\n", test_res);
        } 
    }
    return 0;
}

/* 
	handles interrupts from the keyboard device
*/
void keyboard_handler() {
	char kb_input;
	kb_input = ps2_read();
    // add to buffer if readable (not 0) and buff not full
    if (kb_input && 
        (kb_write+1 != kb_read || (kb_write==KB_BUFF_SIZE-1 && kb_read==0))) {
        kb_buff[kb_write] = kb_input;
        kb_write = (kb_write + 1) % KB_BUFF_SIZE;
    }
    // wake up one blocked process
    if (kb_read != kb_write) 
	    PROC_unblock_head(kb_block_q);
}
/* 
blocks calling thread and returns when kb 
has a char
*/
char getc() {
    char res;
    cli();
    while (kb_write == kb_read) {
        if (kb_block_q == 0) {
            kb_block_q = kmalloc(sizeof(thread_q));
        }
        PROC_block_on(kb_block_q, 1);
        cli();
    }
    res = kb_buff[kb_read];
    kb_read = (kb_read + 1) % KB_BUFF_SIZE; 
    sti();
    return res;
}