/* tty_mbed.h
* Header for tty_mbed.c
* Written by: David Wotherspoon
*/
#include "lpc_types.h"

#define TTY_BUF_LEN 1024 //size of the buffer

void tty_init();

void tty_dump_regs(void);

void tty_printf(char *fmt, ...);

int tty_scanf(char *fmt, ...);

uint8_t tty_getchar(void);

char * tty_gets(void);

void tty_putchar(char c);

void tty_puts(char *c);
