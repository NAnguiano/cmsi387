// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials 
// Source: http://www.jamesmolloy.co.uk/tutorial_html/2.-Genesis.html

#include "monitor.h"
#include "descriptortables.h"
#include "timer.h"
#include "keyboard.h"

int main(struct multiboot *mboot_ptr)
{
    // Initialise all the ISRs and segmentation
    init_descriptor_tables();
    // Initialise the screen (by clearing it)
    monitor_clear();
    // Write out a sample string
    monitor_write("Hello, world!\n");

    //asm volatile("int $0x3");
    //asm volatile("int $0x4");

    //init_timer(50);
    keyboard_install();

    return 0;
}