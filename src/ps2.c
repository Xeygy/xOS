typedef unsigned char uint8_t;
typedef unsigned int uint16_t;

#define PS2_DATA  0x60
#define PS2_CMD 0x64
#define PS2_STATUS PS2_CMD
#define PS2_STATUS_OUTPUT 1
#define PS2_STATUS_INPUT (1 << 1)
#define WAIT_FOR_OUTPUT while(!(inb(PS2_STATUS) & PS2_STATUS_OUTPUT))
#define WAIT_FOR_EMPTY_IN  while(inb(PS2_STATUS) & PS2_STATUS_INPUT)

 
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

static inline void outb(uint16_t port, uint8_t val);
static inline uint8_t inb(uint16_t port);
static inline void to_ccb(PS2_ccb *ccb, uint8_t value);
static inline uint8_t from_ccb(PS2_ccb *ccb);
static int init_controller();
static int init_keyb();

static int init_ctlr_success, ready_to_poll; 
/* scan code set 2 */
static char scodes2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '`', 0,
                        0, 0, 0, 0, 0, 'q', '1', 0, 0, 0, 'z', 's', 'a', 'w', '2', 0,
                        0, 'c', 'x', 'd', 'e', '4', '3', 0, 0, ' ', 'v', 'f', 't', 'r', '5', 0,
                        0, 'n', 'b', 'h', 'g', 'y', '6', 0 ,0, 0, 'm', 'j', 'u', '7', '8', 0,
                        0, ',', 'k', 'i', 'o', '0', '9', 0, 0, '.', '/', 'l', ';', 'p', '-', 0,
                        0, 0, '\'', 0, '[', '=', 0, 0, 0, 0, 0, ']', 0, '\\', 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, /*keypad*/ '1', 0, '4', '7', 0, 0, 0,
                        '0', '.', '2', '5', '6', '8', 0, 0, 0, '+', '3', '-', '*', '9', 0, 0};
/* 
    sets up ps2 controller and keyboard on port 1
    returns 0 on success, 
*/
int init_ps2() {
    int res;
    if (!init_ctlr_success && (res = init_controller()))
        return res;
    init_ctlr_success = 1;
    printk("scodes %lu\n", sizeof(scodes2));
    if (res = init_keyb())
        return res;
    ready_to_poll = 1;
    return 0;
}

/* poll the ps2 device */
/*static*/ char ps2_poll_read(void)
{
    char res;
    // init_ps2 if not initialized
    if (ready_to_poll || init_ps2()) {
        WAIT_FOR_OUTPUT;
        res = inb(PS2_DATA);
        if (res < sizeof(scodes2))
            printk("Got %c\n", scodes2[res]);
        
        return res;
    }
    return '\0';
}

/*
    Sets up keyboard on port 1
    returns 0 on success, other on failure
*/
static int init_keyb() {
    uint8_t test_res;
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
        printk("PS/2 port 1 device reset success! got %hx\n", test_res);
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
        printk("set scancode 2 success! got %hx\n", test_res);
    }  
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
    to_ccb(&ccb, inb(PS2_DATA));
    
    // disable interrupts and translation
    ccb.port1_irq = ccb.port2_irq = ccb.port1_trans = 0;
    // for port clock, enabled = 0
    is_dual_channel = !ccb.port1_clock;
    // write to the ccb
    WAIT_FOR_EMPTY_IN;
    outb(PS2_CMD, 0x60);
    outb(PS2_DATA, from_ccb(&ccb)); 

    // perform controller self test
    outb(PS2_CMD, 0xAA);
    // wait for status output to be true
    WAIT_FOR_OUTPUT;
    test_res = inb(PS2_DATA);
    if (test_res != 0x55) {
        printk("PS/2 self test failed, expected 0x55, got %hx\n", test_res);
        return 1;
    } else {
        printk("PS/2 self test success! got %hx\n", test_res);
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
        printk("PS/2 port 1 test success! got %hx\n", test_res);
    } 
    // port 2
    if (is_dual_channel) {
        outb(PS2_CMD, 0xA9);
        WAIT_FOR_OUTPUT;
        test_res = inb(PS2_DATA);   
        if (test_res != 0x00) {
            printk("PS/2 port 2 test failed, expected 0x00, got %hx\n", test_res);
        } else {
            printk("PS/2 port 2 test success! got %hx\n", test_res);
        } 
    }
    return 0;
}

/*
    recieves 8 bits from a i/o port
*/
static inline uint8_t inb(uint16_t port) {
      uint8_t ret;
      asm volatile ( "inb %1, %0"
                    : "=a"(ret)
                    : "Nd"(port) );
return ret; 
}

/* 
    fills the given ccb with the values from value 
*/
static inline void to_ccb(PS2_ccb *ccb, uint8_t value)
{
    ccb->port1_irq        = value & 1;
    ccb->port2_irq        = value >> 1 & 1;
    ccb->system_flag      = value >> 2 & 1;
    ccb->zero1            = value >> 3 & 1;
    ccb->port1_clock      = value >> 4 & 1;
    ccb->port2_clock      = value >> 5 & 1;
    ccb->port1_trans      = value >> 6 & 1;
    ccb->zero2            = value >> 7 & 1;
}

static inline uint8_t from_ccb(PS2_ccb *ccb)
{
    return ccb->port1_irq       
        | ccb->port2_irq     << 1
        | ccb->system_flag   << 2
        | ccb->zero1         << 3
        | ccb->port1_clock   << 4
        | ccb->port2_clock   << 5
        | ccb->port1_trans   << 6
        | ccb->zero2         << 7;
}
/*
    sends 8 bits to an i/o port
*/
static inline void outb(uint16_t port, uint8_t val) {
      asm volatile ( "outb %0, %1" 
                    : 
                    : "a"(val), "Nd"(port) );
}