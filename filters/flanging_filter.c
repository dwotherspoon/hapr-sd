/* flanging_filter.c
* Flanging filter (sinusoidal delay).
* Written by: David Wotherspoon, Alex Petherick
*/
#include "flanging_filter.h"
#include "math.h"
#include <stdlib.h>
#include "dbg.h"
#include "global.h"
#include "ui_serial.h"

//http://www.mathworks.co.uk/help/dsp/examples/audio-flanging.html
/* flanging_filter_create
* Create a flanging filter, request parameters from UI.
* Returns: Pointer to created filter.
* Written By:  David Wotherspoon
*/
Filter * flanging_filter_create(void) { //manufacture a pointer to a new test filter.
	Filter * result = (Filter *)malloc(sizeof(Filter));
	ASSERT(result != 0, "Malloc failed.");
	Flanging_filter_data * private = (Flanging_filter_data *)malloc(sizeof(Flanging_filter_data));
	ASSERT(private != 0, "Malloc failed.");
	private->gain = ui_gfloat("gain", 0.00, 2.00);
	private->delay = ui_gint("delay", 0, 4095);
	private->frequency = ui_gint("frequency", 0, 1000);
	private->amplitude = ui_gint("amplitude", 0, 2048);
	private->x = 0.0;
	private->inc = 6.3*private->frequency*(S_INTERVAL*1e-6);
	result->next = NULL;
	result->apply = &flanging_filter_apply;
	result->show = &flanging_filter_show;
	result->privateData = (void *)private;
	return result;
}

/* flanging_filter_apply
* Apply the filter, increment x (sine) and mix in delayed sample.
* Returns: output value.
* Written By: David Wotherspoon, Alex Petherick
*/
int16_t flanging_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr) {
	Flanging_filter_data * private = (Flanging_filter_data *)(((Filter *)self)->privateData);
	uint16_t dptr = private->delay + (private->amplitude * sin(private->x));
	private->x += private->inc;
	if (private->x > 6.3) { private->x = 0.0; } //increment and roll around x
	
	if (ptr >= dptr) { //roll around ptr
		dptr = ptr - dptr;
	} 
	else {
		dptr = (BUF_LEN - 1) - (dptr -ptr);
	}
	return (1.0 - private->gain)*prev + (private->gain) * buffer[dptr];
}

/* flanging_filter_debug
* Write filter information to serial for debugging.
* Written by: David Wotherspoon
*/
void flanging_filter_show(void * self) {
	Flanging_filter_data * private = (Flanging_filter_data *)(((Filter *)self)->privateData);
	tty_printf("(Flanging filter @ 0x%x, delay: %d, gain %f, freq %f, sweep %f, inc %f)\r\n", self, private->delay, private->gain, private->frequency, private->amplitude, private->inc);
}
