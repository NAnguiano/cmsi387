/*
 Many thanks to the OS series at OSDever.net and JamesMolloy.co.uk for help!
 Links used in this file: 
 http://www.jamesmolloy.co.uk/tutorial_html/3.-The%20Screen.html
 http://www.osdever.net/bkerndev/Docs/creatingmain.htm
 http://www.osdever.net/bkerndev/Docs/printing.htm
*/

#ifndef __SYSTEM_H
#define __SYSTEM_H

#define BLACK         0
#define BLUE          1
#define GREEN         2
#define CYAN          3
#define RED           4
#define MAGENTA       5
#define BROWN         6
#define LIGHT_GREY    7
#define DARK_GREY     8
#define LIGHT_BLUE    9
#define LIGHT_GREEN   10
#define LIGHT_CYAN    11
#define LIGHT_RED     12
#define LIGHT_MAGENTA 13
#define LIGHT_BROWN   14
#define WHITE         15

typedef unsigned int size_t;
typedef unsigned char  u8int;

/* MAIN.C */
extern void *memcpy(void *dest, const void *src, size_t count);
extern void *memset(void *dest, char val, size_t count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);
extern size_t strlen(unsigned char *str);
extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);

/* SCREEN.C */
extern void init_video(void);
extern void puts(unsigned char *text);
extern void putch(unsigned char c);
extern void settextcolor(unsigned char forecolor, unsigned char backcolor);
extern void cls();

#endif