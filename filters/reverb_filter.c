/* echo_filter.c
* Reverb filter, create, apply and show.
* Written by: David Wotherspoon, Alex Petherick
*/
#include "reverb_filter.h"
#include "ui_serial.h"
#include <stdlib.h>
#include "dbg.h"
#include "global.h"

/* reverb_filter_create
* Creates a reverb filter, requesting parameters from ui.
* Returns: Pointer to created filter.
* Written By: David Wotherspoon
*/
Filter * reverb_filter_create(void) {
	Filter * ret = (Filter *)malloc(sizeof(Filter));
	ASSERT(ret != 0, "Malloc failed.");
	Reverb_filter_data * private = (Reverb_filter_data *)malloc(sizeof(Reverb_filter_data)); 
	ASSERT(private != 0, "Malloc failed.");
	private->gain = ui_gfloat("gain", 0.00, 2.00);
	private->delay = ui_gint("delay", 0, 4095);
	ret->next = NULL;
	ret->apply = &reverb_filter_apply;
	ret->privateData = (void *)private;
	ret->show = &reverb_filter_show;
	return ret;
}

/* echo_filter_apply
* Apply the filter,  takes delay samples back and mixes it in at gain percentage, also writing back into the delay line to create a reverb
* Returns: output value.
* Written by: Alex Petherick
*/
int16_t reverb_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr) {
	Reverb_filter_data * private = (Reverb_filter_data *)(((Filter *)self)->privateData);
	uint16_t d_ptr;
	if (ptr >= private->delay) {
		d_ptr = (ptr - private->delay);
	}
	else {
		d_ptr = (BUF_LEN - 1) - (private->delay - ptr);	
	}
	buffer[ptr] = (int16_t)((1.0 - private->gain)*(float)prev)+(private->gain*(float)buffer[d_ptr]); //write into buffer
	return buffer[ptr];
}

/* echo_filter_show
* Write filter information to serial for debugging.
* Written By : David Wotherspoon
*/
void reverb_filter_show(void * self) {
	Reverb_filter_data * private = (Reverb_filter_data *)(((Filter *)self)->privateData);
	tty_printf("(Reverb filter @ 0x%x, delay %d, gain %f)\r\n", self, private->delay, private->gain);
}
