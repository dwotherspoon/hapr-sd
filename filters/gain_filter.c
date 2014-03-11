/* gain_filter.c
* Linear gain filter.
* Written by: David Wotherspoon, Mike Jenic
*/
#include "gain_filter.h"
#include <stdlib.h>
#include "dbg.h"
#include "ui_serial.h"
//Basic linear gain filter.

/* gain_filter_create
* Create a linear gain filter, request parameters from ui.
* Returns: Pointer to created filter.
* Written By: David Wotherspoon
*/
Filter * gain_filter_create(void) {
	Filter * result = (Filter *)malloc(sizeof(Filter));
	ASSERT(result != 0, "Malloc failed.");
	Gain_filter_data * private = (Gain_filter_data *)malloc(sizeof(Gain_filter_data));
	ASSERT(private != 0, "Malloc failed.");
	private->gain = ui_gfloat("gain", 0.00, 2.00);
	result->privateData = (void *)private; //cast private to void pointer
	result->next = NULL;
	result->apply = &gain_filter_apply;
	result->show = &gain_filter_show;
	return result;
}

/* gain_filter_apply
* Applies gain to input value.
* Returns: output value.
* Written By: Mike Jenic
*/
int16_t gain_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr) {
	Gain_filter_data * private = (Gain_filter_data *)(((Filter *)self)->privateData);
	return (private->gain*(float)prev); //to wirte back or not?
}

/* gain_filter_show
* Write filter information to serial for debugging.
* Written By: David Wotherspoon
*/
void gain_filter_show(void * self) {
	Gain_filter_data * private = (Gain_filter_data *)(((Filter *)self)->privateData);
	tty_printf("(Gain filter @ 0x%x, gain %f)\r\n", self, private->gain);
}
