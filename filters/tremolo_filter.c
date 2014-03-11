/* tremolo_filter.c
* Apply amplitude modulation to value.
* Written by: David Wotherspoon
*/
#include "tremolo_filter.h"
#include "math.h"
#include "dbg.h"
#include <stdlib.h>
#include "ui_serial.h"
#include "global.h"
//http://msp.ucsd.edu/techniques/v0.11/book-html/node115.html
//http://audioprograming.wordpress.com/2012/03/02/delayline-pitchshifter/

/* tremolo_filter_create
* Create tremolo filter, request parameters from ui.
* Returns: Pointer to created filter.
*/
Filter * tremolo_filter_create(void) {
	Filter * result = (Filter *)malloc(sizeof(Filter));
	Tremolo_filter_data * private = (Tremolo_filter_data *)malloc(sizeof(Tremolo_filter_data));
	private->depth = ui_gfloat("depth", 0.00, 0.50); //up to half reduce
	private->freq = ui_gint("frequency", 0, 1000);
	private->x = 0.0;
	private->inc = 6.3*private->freq*(S_INTERVAL*1e-6);;
	result->next = NULL;
	result->apply = &tremolo_filter_apply;
	result->show = &tremolo_filter_show;
	result->privateData = (void *)private;
	return result;

}

/* tremolo_filter_apply
* Increment sine (x) and reduce gain by sine and depth.
* Returns: Output value.
*/
int16_t tremolo_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr) {
	Tremolo_filter_data * private = (Tremolo_filter_data *)(((Filter *)self)->privateData);
	private->x += private->inc;
	if (private->x > 6.3) { private->x = 0.0; } //increment and roll around x

	return prev*(1.0 - (private->depth*sin(private->x)+private->depth));
}

/* tremolo_filter_show
* Write filter information to serial for debugging.
*/
void tremolo_filter_show(void * self) {
	Tremolo_filter_data * private = (Tremolo_filter_data *)(((Filter *)self)->privateData);
	tty_printf("(Tremolo filter @ 0x%x, depth: %f, freq: %d, inc: %f)\r\n", self, private->depth, private->freq, private->inc);
}
