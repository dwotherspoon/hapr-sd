/* echo_filter.c
* Echo filter (no feedback), create, apply and show.
* Written by: David Wotherspoon
*/
#include "echo_filter.h"
#include <stdlib.h>
#include "dbg.h"
#include "global.h"
#include "ui_serial.h"

/* echo_filter_create
* Creates an echo filter, requesting parameters from ui.
* Returns: Pointer to created filter.
*/
Filter * echo_filter_create(void) {
	Filter * ret = (Filter *)malloc(sizeof(Filter));
	ASSERT(ret != 0, "Malloc failed.");
	Echo_filter_data * private = (Echo_filter_data *)malloc(sizeof(Echo_filter_data));
	ASSERT(private != 0, "Malloc failed.");
	private->gain = ui_gfloat("gain", 0.00, 2.00);
	private->delay = ui_gint("delay", 0, 4095);
	ret->next = NULL;
	ret->apply = &echo_filter_apply;
	ret->privateData = (void *)private;
	ret->show = &echo_filter_show;
	return ret;
}

/* echo_filter_apply
* Apply the filter, mix delayed sample with chain value.
* Returns: output value.
*/
int16_t echo_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr) {
	Echo_filter_data * private = (Echo_filter_data *)(((Filter *)self)->privateData);
	static uint16_t d_ptr;
	if (ptr >= private->delay) { //get delayed sample.
		d_ptr = (ptr - private->delay);
	}
	else {
		d_ptr = (BUF_LEN - 1) - (private->delay - ptr);	
	}
	return (int16_t)((1.0 - private->gain)*(float)prev)+(private->gain*(float)buffer[d_ptr]); //mix and return
}

/* echo_filter_show
* Write filter information to serial for debugging.
*/
void echo_filter_show(void * self) {
	Echo_filter_data * private = (Echo_filter_data *)(((Filter *)self)->privateData);
	tty_printf("(Echo filter @ 0x%x, delay %d, gain %f)\r\n", self, private->delay, private->gain);
}
