#include "io.h"
#include "idt.h"

#define FB_BLACK         0
#define FB_BLUE          1
#define FB_GREEN         2  
#define FB_CYAN          3
#define FB_RED           4
#define FB_MAGENTA       5
#define FB_BROWN         6
#define FB_LIGHT_GREY    7
#define FB_DARK_GREY     8 
#define FB_LIGHT_BLUE    9
#define FB_LIGHT_GREEN   10
#define FB_LIGHT_CYAN    11
#define FB_LIGHT_RED     12
#define FB_LIGHT_MAGENTA 13
#define FB_LIGHT_BROWN   14
#define FB_WHITE         15

/* The I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

//unsigned int characterLocation = 0;

#define DEBUG 0
#define INFO  1
#define ERROR 2

/* The I/O ports */

/* All the I/O ports are calculated relative to the data port. This is because
 * all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
 * order, but they start at different values.
 */

#define SERIAL_COM1_BASE                0x3F8      /* COM1 base port */

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/* The I/O port commands */

/* SERIAL_LINE_ENABLE_DLAB:
 * Tells the serial port to expect first the highest 8 bits on the data port,
 * then the lowest 8 bits will follow
 */
#define SERIAL_LINE_ENABLE_DLAB         0x80

 /* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */
 
#define ICW1_ICW4   0x01        /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02        /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04        /* Call address interval 4 (8) */
#define ICW1_LEVEL  0x08        /* Level triggered (edge) mode */
#define ICW1_INIT   0x10        /* Initialization - required! */
 
#define ICW4_8086   0x01        /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO   0x02        /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08        /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C        /* Buffered mode/master */
#define ICW4_SFNM   0x10        /* Special fully nested (not) */

#define PIC1        0x20        /* IO base address for master PIC */
#define PIC2        0xA0        /* IO base address for slave PIC */
#define PIC1_COMMAND    PIC1
#define PIC1_DATA   (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA   (PIC2+1)

 struct gdt {
    unsigned int base;
    unsigned int limit;
    unsigned int type;
} __attribute__((packed));

/* Defines an IDT entry */
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;        /* Our kernel segment goes here! */
    unsigned char always0;     /* This will ALWAYS be set to 0! */
    unsigned char flags;       /* Set using the above table! */
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

/* Declare an IDT of 256 entries. Although we will only use the
*  first 32 entries the rest exists as a bit
*  of a trap. If any undefined IDT entry is hit, it normally
*  will cause an "Unhandled Interrupt" exception. Any descriptor
*  for which the 'presence' bit is cleared (0) will generate an
*  "Unhandled Interrupt" exception */
struct idt_entry idt[256];
struct idt_ptr idtp;


/** serial_configure_baud_rate:
 *  Sets the speed of the data being sent. The default speed of a serial
 *  port is 115200 bits/s. The argument is a divisor of that number, hence
 *  the resulting speed becomes (115200 / divisor) bits/s.
 *
 *  @param com      The COM port to configure
 *  @param divisor  The divisor
 */
void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
    outb(SERIAL_LINE_COMMAND_PORT(com),
         SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DATA_PORT(com),
         (divisor >> 8) & 0x00FF);
    outb(SERIAL_DATA_PORT(com),
         divisor & 0x00FF);
}

/** serial_configure_line:
 *  Configures the line of the given serial port. The port is set to have a
 *  data length of 8 bits, no parity bits, one stop bit and break control
 *  disabled.
 *
 *  @param com  The serial port to configure
 */
void serial_configure_line(unsigned short com)
{
    /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
     * Content: | d | b | prty  | s | dl  |
     * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
     */
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

/** serial_is_transmit_fifo_empty:
 *  Checks whether the transmit FIFO queue is empty or not for the given COM
 *  port.
 *
 *  @param  com The COM port
 *  @return 0 if the transmit FIFO queue is not empty
 *          1 if the transmit FIFO queue is empty
 */
int serial_is_transmit_fifo_empty(unsigned int com)
{
    /* 0x20 = 0010 0000 */
    return inb(SERIAL_LINE_STATUS_PORT(com)) && 0x20;
}

void serial_write(char *buf, unsigned int len) {
    serial_configure_baud_rate(SERIAL_COM1_BASE, 1);
    serial_configure_line(SERIAL_COM1_BASE);
    unsigned int i;
    for(i = 0; i < len; i++) {
        while(!serial_is_transmit_fifo_empty(SERIAL_COM1_BASE));
        outb(SERIAL_COM1_BASE, buf[i]);
    }
}

void log(char *buf, unsigned int len, unsigned int type) {
    switch(type) {
        case DEBUG:
            serial_write("\nDEBUG: ", 8);
            break;
        case INFO:
            serial_write("\nINFO: ", 7);
            break;
        case ERROR:
            serial_write("\nERROR: ", 8);
            break;
        default:
            serial_write("\nUNKNOWN TYPE: ", 15);
            break;
    }
    serial_write(buf, len);
}

void *memcpy(void *dest, const void *src, unsigned int count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--){
        *dp++ = *sp++;
    }
    return dest;
}


void *memset(void *dest, char val, unsigned int count)
{
// note: unsigned int is equivalent to size_t ( type of result of sizeof operator)
    char *temp = (char *)dest;
    for( ; count != 0; count--){
        *temp++ = val;
    }
    return dest;
}


void encodeGdtEntry(unsigned char *target, struct gdt source)
{
    if (source.limit > 65536) {
        // Adjust granularity if required
        source.limit = source.limit >> 12;
        target[6] = 0xC0;
    } else {
        target[6] = 0x40;
    }
 
    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0xF;
 
    // Encode the base 
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
 
    // And... Type
    target[5] = source.type;
}

void gdt_load() {
    struct gdt source[4];
    source[0].base=0;
    source[0].limit=0;
    source[0].type=0;                     // Selector 0x00 cannot be used
    source[1].base=0;
    source[1].limit=0xffffffff;
    source[1].type=0x9A;         // Selector 0x08 will be our code
    source[2].base=0;
    source[2].limit=0xffffffff;
    source[2].type=0x92;         // Selector 0x10 will be our data

    unsigned long global_descriptor_table[4];
    int i;
    for (i = 0; i < 4; i++) {
        encodeGdtEntry((unsigned char*)&global_descriptor_table[i], source[i]);
    }
    setGdt(&(global_descriptor_table[0]), sizeof(global_descriptor_table[0]));
}

void create_idt() {
 
    struct idt_entry idt[256];
    struct idt_ptr idtp;

    load_idt(&idtp);

/**   unsigned int idt[512];
    unsigned int value = &interrupt_handler_4;
    int i = 0;
    for(i = 0; i < 21; i = i + 2) {
        idt[i] = ((value & 0xFFFF0000) + 0x00008E00);
        idt[i + 1] = ((value & 0x0000FFFF) + 0x00080000);
    }
    load_idt(&idt);
*/
}



/* Installs the IDT */
void idt_install()
{
    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = &idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(struct idt_entry) * 256);
    
    /* Add any new ISRs to the IDT here using idt_set_gate */

    /* Points the processor's internal register to the new IDT */
    load_idt(&idtp); /* note that all idt_load should be load_idt() because that is what we called our function. */
}

/*
arguments:
    offset1 - vector offset for master PIC
        vectors on the master become offset1..offset1+7
    offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2)
{
    unsigned char a1, a2;
 
    a1 = inb(PIC1_DATA);                      // save masks
    a2 = inb(PIC2_DATA);
 
    outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
    outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
    outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
    outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
 
    outb(PIC1_DATA, a1);   // restore saved masks.
    outb(PIC2_DATA, a2);
}

int kmain() {
    char str1[] = "The GDT has been loaded. Awww yeah.";
    unsigned int size_str1 = sizeof(str1) - 1;
    gdt_load();
    fb_write(str1, size_str1, FB_BLACK, FB_WHITE);
    char str2[] = "The PIC has been remapped. Awww yeah.";
    unsigned int size_str2 = sizeof(str2) - 1;
    fb_write(str2, size_str2, FB_CYAN, FB_DARK_GREY);
    char str3[] = "The IDT has been loaded. Awww yeah.";
    unsigned int size_str3 = sizeof(str3) - 1;
    create_idt();
    PIC_remap(0x20, 0x28);
    fb_write(str3, size_str3, FB_BROWN, FB_LIGHT_RED);
    int x = 0x7FFFFFFF;
    int y = 1;
    int z = x + y;
    z = z;
    return 0;
}

