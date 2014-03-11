/* ngate_filter.c
* Noise Gate Filter (Naive) just cuts of after a threshold
* Written by: Alexander Petherick
*/
#include "ngate_filter.h"
#include <stdlib.h>
#include "dbg.h"
#include "string.h"
#include "ui_serial.h"

/* ngate_filter_create
* Creates a noise gate filter, requesting parameters from ui.
* Returns: Pointer to created filter.
*/
Filter * ngate_filter_create(void) {
	Filter * result = (Filter *)malloc(sizeof(Filter));
	Ngate_filter_data * private = (Ngate_filter_data *)malloc(sizeof(Ngate_filter_data)); 
	private->threshold = ui_gint("threshold", 0, 2048);
	private->reduction = ui_gfloat("reduction", 0, 1);
	result->privateData = (void *)private; //cast private to void pointer
	result->next = NULL;
	result->apply = &ngate_filter_apply;
	result->show = &ngate_filter_show;
	return result;
}

/* ngate_filter_apply
* Apply the filter, returns reduced sample if below threshold
* Returns: output value.
*/
int16_t ngate_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr) {
	Ngate_filter_data * private = (Ngate_filter_data *)(((Filter *)self)->privateData);
	if ((prev > (private->threshold)) || (prev < (-(private->threshold)))) {
		return prev;	
	}
	else {
		return private->reduction*prev;
	}
}

/* ngate_filter_show
* Write filter information to serial for debugging.
*/
void ngate_filter_show(void * self) {
	Ngate_filter_data * private = (Ngate_filter_data *)(((Filter *)self)->privateData);
	tty_printf("(Noise gate filter @ 0x%x, threshold %d, reduction %f)\r\n", self, private->threshold, private->reduction);
}
