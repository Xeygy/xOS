#include "interrupts.h"
#include "print.h"
#include "asm.h"
#include "isr.h"

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
extern void* isr_stub_table[];


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

static idt_entry_t idt[IDT_NUM_ENTRIES];
static idtr_t idtr; 

static void PIC_remap(int offset1, int offset2);
static void PIC_mask_all();
void generic_handler(void* error);
static void setupEntry(idt_entry_t *entry, void * handler);

int enable_interrupts() {
    // int gdb=1;
    // while(gdb);
	int i;
	PIC_remap(0x20, 0x28);
	PIC_mask_all();

	for (i = 0; i<IDT_NUM_ENTRIES; i++) {
		setupEntry(&idt[i], &isr0);
	}
	idtr.base = (uintptr_t)&idt[0];
	idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_NUM_ENTRIES - 1;
	// load interrupts
	lidt(&idtr);
	
	sti();
    return 0;
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

static void PIC_mask_all() {
	outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

/*
	sets the offset of the entry to the address of the given
	handler function
*/
static void setupEntry(idt_entry_t *entry, void *handler) {
	uint64_t handCast = (uint64_t)handler;
	entry->tgtOffset0 = handCast & 0xFFFF;
	entry->tgtOffset1 = (handCast >> 16) & 0xFFFF;
	entry->tgtOffset2 = (handCast >> 32) & 0xFFFFFFFF;
	entry->present = 1;
	entry->tgtSelector = GDT_OFFSET;
	entry->type = IDT_INT_GATE;
}

void generic_handler(void* error) {
	while (1) 
		asm volatile ("cli; hlt");
}