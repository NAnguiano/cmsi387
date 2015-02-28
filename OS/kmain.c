#include "io.h"

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

unsigned int characterLocation = 0;

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

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg){
    char *fb = (char *) 0x000B8000;
    fb[i] = c;
    fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}                                           

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void fb_move_cursor(unsigned short pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

void fb_write(char *buf, unsigned int len, unsigned char bg, unsigned char fg) {
    unsigned int i;
    for(i = 0; i < len; i++) {
        fb_write_cell(characterLocation, buf[i], bg, fg);
        characterLocation += 2;
    }
    fb_move_cursor(characterLocation/2);
}

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

int kmain() {
    gdt_load();
    char str[] = "Loaded the GDT.";
    unsigned int size_str = sizeof(str) - 1;
    fb_write(str, size_str, FB_CYAN, FB_DARK_GREY);
    log(str, size_str, DEBUG);
    return 0;
}

