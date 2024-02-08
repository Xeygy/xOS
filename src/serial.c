#include "asm.h"
#include "print.h"
#include "interrupts.h"

#define PORT 0x3f8          // Assumed COM1 Port, from https://wiki.osdev.org/Serial_ports#Port_Addresses
#define BUFF_SIZE 256

typedef struct State {
    char buff[BUFF_SIZE];
    char *consumer, *producer;
} State;

static int SER_init(); 
static int is_transmit_empty();
static int add_to_state(char c);
static void hw_write();

static int initialized, busy;
static State state;

int SER_write(const char *buff, int len) {
    int offset = 0;
    if (!initialized && SER_init()) {
        printk("failed to initialize serial");
        return 1;
    }
    while (offset < len &&
        *(buff + offset) != '\0' && 
        !add_to_state(*(buff + offset++)));
    hw_write();
    return 0;
}

void SER_ISR() {
    // TX interrupt
    hw_write();
}

/* 
Initializes serial output 
from https://wiki.osdev.org/Serial_ports
https://www.lammertbies.nl/comm/info/serial-uart
*/
static int SER_init() {
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
   return 0;
}

static int is_transmit_empty() {
    return inb(PORT + 5) & 0x20;
}

/* 
producer 
returns 0 on success, 1 on error 
*/
static int add_to_state(char c) {
    // if producer is right behind consumer, it is full
    if (state.producer == state.consumer - 1 ||
        state.consumer == &state.buff[0] && state.producer == &state.buff[BUFF_SIZE])
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
static void hw_write() {
    if (!initialized) {
        printk("called hw_write before init");
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