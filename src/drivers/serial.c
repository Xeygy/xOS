#include "asm.h"
#include "print.h"
#include "interrupts.h"

#define PORT 0x3f8          // Assumed COM1 Port, from https://wiki.osdev.org/Serial_ports#Port_Addresses
#define BUFF_SIZE 256   // note this means SER_write cannot be called w/a string of len > 256
#define IIR_TX_EMPTY 0b001
#define IIR_LINE 0b011
#define IIR_RECIEVED_DATA 0b010
#define IIR_CHAR_TO 0b110
#define IIR_MSR 0b000

typedef struct State {
    char buff[BUFF_SIZE];
    char *consumer, *producer;
} State;

static int SER_init(); 
static int is_transmit_empty();
static int IIR_reason();
static int add_to_state(char c);
static int hw_write();

static int initialized, busy;
static State state;

int SER_write(const char *buff, int len) {
    int offset = 0;
    int reenable_int = 0;
    if (interrupts_enabled()) {
        cli();
        reenable_int = 1;
    }
    if (SER_init()) {
        printk("failed to initialize serial");
        return 1;
    }
    while (offset < len &&
        *(buff + offset) != '\0' && 
        !add_to_state(*(buff + offset++)));
    hw_write();
    if (reenable_int) {
        sti();
    }
    return 0;
}

void SER_ISR() {
    int reason;
    reason = IIR_reason();
    if (reason == IIR_LINE) {
        // read LSR to clear, don't do anything
        is_transmit_empty();
    } else if (reason == IIR_TX_EMPTY) {
        hw_write();
    } else if (reason == IIR_RECIEVED_DATA || reason== IIR_CHAR_TO) {
        printk("iir got %d\n", reason);
        inb(PORT); // read RBR to clear
    } else if (reason == IIR_MSR) {
        printk("iir got %d\n", reason);
        inb(PORT + 6); // read MSR to clear
    } else {
        printk("unsupported serial interrupt %d\n", reason);
    }
}

/* 
Initializes serial output 
from https://wiki.osdev.org/Serial_ports
https://www.lammertbies.nl/comm/info/serial-uart
returns 0 on success, sets initialized to true.
*/
static int SER_init() {
    if (initialized)
        return 0;
    // software init
    state.consumer = &state.buff[0];
    state.producer = &state.buff[0];

    // hardware init
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if(inb(PORT + 0) != 0xAE) {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(PORT + 4, 0x0F);

    // enable interrupts 
    outb(PORT + 1, 0b110);

    initialized = 1;
    return 0;
}

// check if tx is ready to recieve
static int is_transmit_empty() {
    return inb(PORT + 5) & 0x20;
}

// returns bits 1-3 of the IIR Register
static int IIR_reason() {
    return (inb(PORT + 2) & 0b1110) >> 1;
}

/* 
producer 
returns 0 on success, 1 on error 
*/
static int add_to_state(char c) {
    // if producer is right behind consumer, it is full
    if (state.producer == state.consumer - 1 ||
        (state.consumer == &state.buff[0] && state.producer == &state.buff[BUFF_SIZE]))
        return 1;
    *state.producer = c;
    state.producer++;
    if (state.producer >= &state.buff[BUFF_SIZE])
        state.producer = &state.buff[0];
    return 0;
}

/* 
consumer, sends things in state.buff to serial tx
returns 0 on sucessful handle, error otherwise
*/
static int hw_write() {
    if (SER_init()) {
        printk("failed to initialize serial in hw_write\n");
        return 1;
    }
    if (busy) {
        while (is_transmit_empty() == 0);
        busy = 0;
    }
    // nothing to consume
    if (state.consumer == state.producer)
        return 0;
    // write out
    outb(PORT, *state.consumer);
    state.consumer++;
    if (state.consumer >= &state.buff[BUFF_SIZE])
        state.consumer = &state.buff[0];
    busy = 1;
    return 0;
}