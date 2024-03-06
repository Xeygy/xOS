#include "interrupts.h"
#include "print.h"
#include "asm.h"
#include "ps2.h"
#include "string.h"
#include "serial.h"
#include "page_alloc.h"

extern void* syscall_isr; // from isr.asm
extern void* isr_table[]; // from isr.asm
extern uint64_t gdt64_tss[]; // from boot.asm
extern uint64_t gdt64_tss_offset[];  // from boot.asm

/* 
* much of this code for interrupts 
* comes from https://wiki.osdev.org/8259_PIC 
*/

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1) 
#define PIC_EOI		0x20		/* End-of-interrupt command code */

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */
 
#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define GDT_OFFSET 0x08 /* not entirely sure where this comes from? */
#define IDT_INT_GATE 0xE /* runs with interrupts disabled */
#define IDT_TRAP_GATE 0xF /* runs with interrupts enabled*/
#define IDT_NUM_ENTRIES 256

/* ISR numbers */
#define ISR_DF 8  /* Double Fault */
#define ISR_GP 13 /* General Protection  */
#define ISR_PF 14 /* Page Fault */
#define ISR_SYSCALL 0x80 /* Page Fault */

#define STACK_SIZE 2048/8 /* 2kb, stack size of the tss ist stacks */

#define IRQ_TIMER 0x20
#define IRQ_KEYBOARD 0x21
#define IRQ_SERIAL_1 0x24

// Interrupt Descriptor Table Entry
typedef struct idt_entry_t {
    uint16_t tgtOffset0;
	uint16_t tgtSelector;
	uint16_t ist:3;
	uint16_t reserved0:5;
	uint16_t type:4;
	uint16_t zero:1;
	uint16_t dpl:2;
	uint16_t present:1;
	uint16_t tgtOffset1;
	uint32_t tgtOffset2;
    uint32_t reserved1;
} __attribute__((packed)) idt_entry_t;

// TSS table
typedef struct tss_t {
    uint32_t reserved0;
	uint64_t RSP0;
	uint64_t RSP1;
	uint64_t RSP2;
	uint64_t reserved1;
	uint64_t IST1;
	uint64_t IST2;
	uint64_t IST3;
	uint64_t IST4;
	uint64_t IST5;
	uint64_t IST6;
	uint64_t IST7;
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t io_map_base;
} __attribute__((packed)) tss_t;

// tss descriptor
typedef struct {
	uint16_t	limit1;
	uint16_t	base0;
    uint8_t	    base1;
    uint16_t	type:4;
	uint16_t	zero:1;
	uint16_t	dpl:2;
	uint16_t	present:1;
	uint16_t	limit2:4;
	uint16_t	avl:1;
	uint16_t	ignore:2;
	uint16_t	g:1;
    uint8_t	    base2;
    uint32_t	base3;
    uint32_t	ignoreAndZeros;

} __attribute__((packed)) tss_desc_t;

static idt_entry_t idt[IDT_NUM_ENTRIES];
static uint64_t ist1[STACK_SIZE]; // 2kb 
static uint64_t ist2[STACK_SIZE]; // 2kb
static uint64_t ist3[STACK_SIZE]; // 2kb
static idtr_t idtr; 
static tss_t tss;
static int enabled, setup;

static void PIC_sendEOI(uint8_t irq);
static void PIC_remap(int offset1, int offset2);
static void set_PIC_mask();
static void firstTimeSetup();
void generic_handler(uint64_t isr_num, uint64_t error_code);
static void keyboard_handler();
static void setupIdtEntry(idt_entry_t *entry, void * handler, uint16_t type, uint8_t ist);
static void setupAndLoadTSS();

int enable_interrupts() {
	if (!setup) 
		firstTimeSetup();
	sti();
	enabled = 1;
    return 0;
}

int disable_interrupts() {
	cli();
	enabled = 0;
	return 0;
}

/*
	returns 1 if interrupts are currently enabled,
	0 if not
*/
int interrupts_enabled() {
	return enabled;
}
 
/* 
	initializes the PIC and idt on startup
*/
static void firstTimeSetup() {
	int i, ist_idx;
	PIC_remap(0x20, 0x28);
	set_PIC_mask();
	// tss
	setupAndLoadTSS();

	// idt
	for (i = 0; i<IDT_NUM_ENTRIES; i++) {
		if (i == ISR_DF) {
			ist_idx = 1;
		} else if (i == ISR_GP) {
			ist_idx = 2;
		} else if (i == ISR_PF) {
			ist_idx = 3;
		} else {
			ist_idx = 0;
		}
		if (i == ISR_SYSCALL) {
			// special syscall case
			setupIdtEntry(&idt[i], &syscall_isr, IDT_INT_GATE, ist_idx);
		} else {
			setupIdtEntry(&idt[i], isr_table[i], IDT_INT_GATE, ist_idx);
		}
	}
	idtr.base = (uintptr_t)&idt[0];
	idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_NUM_ENTRIES - 1;
	// load interrupts
	lidt(&idtr);

	setup = 1;
}

/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
    https://wiki.osdev.org/PIC#Initialisation
*/
static void PIC_remap(int offset1, int offset2) {
	uint8_t a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	outb(PIC2_DATA, ICW4_8086);
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

static void set_PIC_mask() {
	uint16_t pic1_mask = 0xff - 0b10010; // unmask serial 1, keyb
	uint16_t pic2_mask = 0xff;
	outb(PIC1_DATA, pic1_mask);
    outb(PIC2_DATA, pic2_mask);
}

/*
	sets the offset of the entry to the address of the given
	handler function
*/
static void setupIdtEntry(idt_entry_t *entry, void *handler, uint16_t type, uint8_t ist) {
	uint64_t handCast = (uint64_t)handler;
	entry->tgtOffset0 = handCast & 0xFFFF;
	entry->tgtOffset1 = (handCast >> 16) & 0xFFFF;
	entry->tgtOffset2 = (handCast >> 32) & 0xFFFFFFFF;
	entry->present = 1;
	entry->tgtSelector = GDT_OFFSET;
	entry->type = type;
	entry->ist = ist & 0b111;
}

/*
	sets up the tss and calls ltr to load it
*/
static void setupAndLoadTSS() {
	tss_desc_t desc;
	uint64_t tssAddr = (uint64_t) &tss;

	// tss table (start from the other end of the arr)
	tss.IST1 = (uint64_t) ist1 + STACK_SIZE - 1;
	tss.IST2 = (uint64_t) ist2 + STACK_SIZE - 1;
	tss.IST3 = (uint64_t) ist3 + STACK_SIZE - 1;
	// tss descriptor
	desc.limit1 = 0x68; // tss table size
	desc.base0 = tssAddr & 0xFFFF;
	desc.base1 = (tssAddr >> 16) & 0xFF;
	desc.base2 = (tssAddr >> 24) & 0xFF;
	desc.base3 = (tssAddr >> 32) & 0xFFFFFFFF;
	desc.type = 0b1001; 
	desc.dpl = 0;
	desc.present = 1;
	desc.g = 1;  // maybe change to 0
	// copy tss descriptor into gdt
	memcpy(gdt64_tss, &desc, sizeof(desc)); 
	// load
	ltr(gdt64_tss_offset);
}

void generic_handler(uint64_t isr_num, uint64_t error_code) {
	switch (isr_num) {
		case IRQ_KEYBOARD:
			keyboard_handler();
			break;
		case IRQ_SERIAL_1:
			SER_ISR();
			break;
		case ISR_PF:
			pf_isr(error_code & 0xFF);
			break;
		default:
			printk("Interrupt 0x%lx not handled, stopping..., error code %ld\n", isr_num, error_code & 0xFF);
			disable_interrupts();
			asm volatile ("hlt");
	}

	if (isr_num >= 0x20 && isr_num <= 0x2f) {
		// is a hardware interrupt, notify PIC that it's handled
		PIC_sendEOI(isr_num - 0x20);
	}
}

void syscall_handler(uint64_t syscall_num) {
	printk("syscall: %ld\n", syscall_num);
	switch(syscall_num) {
		case 1:
			// yield
			break;
		case 2:
			// exit
			break;
	}
}

/* 
	handles interrupts from the keyboard device
*/
static void keyboard_handler() {
	char kb_input;
	kb_input = ps2_read();
	if (kb_input)
		printk("%c", kb_input);
}

/*
	Tell PIC interrupt finished processing
*/
static void PIC_sendEOI(uint8_t irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
 
	outb(PIC1_COMMAND,PIC_EOI);
}