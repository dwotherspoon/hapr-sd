/* dbg.c
* Function bodies for assert, warn and throw messages.
* Written by: David Wotherspoon
*/
#include "dbg.h"
#include "lpc17xx_nvic.h"

/* dbg_assert
* Print assert error and stop execution.
*/
void dbg_assert(char * msg, char * file, int line) {
	tty_printf("\x1b[31mAssert error: \x1b[93m%s:%d \x1b[0m - %s\n\r", file, line, msg);
	tty_dump_regs(); //print registers contents
	__disable_irq(); //disable interrupts
	while (1) {} //lock up
}

/* dbg_warn
* Print warn error message.
*/
void dbg_warn(char * msg, char * file, int line) {
	tty_printf("\x1b[96mWarning: \x1b[93m%s:%d \x1b[0m - %s\n\r", file, line, msg);
}

/* dbg_throw
* Print throw error and stop execution.
*/
void dbg_throw(char * msg, char * file, int line) {
	tty_printf("\x1b[31mError thrown: \x1b[93m%s:%d \x1b[0m - %s\n\r", file, line, msg);
	__disable_irq(); //disable interrupts
	while (1) {} //lock up
}