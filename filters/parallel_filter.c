/* parallel_filter.c
* Implements parallel filters.
* Written by: David Wotherspoon
*/
#include "filter_table.h"
#include "stdio.h"
#include "stdlib.h"
#include "dbg.h"
#include "ui_serial.h"
//Filter for parallel filter mixing
char tbuf[10]; //used for param requests

/* parallel_filter_create
* Create up to MAX_PARALLEL filters and gains, requesting from ui.
* Returns: Pointer to created filter.
*/
Filter * parallel_filter_create(void) {
	uint8_t i;	
	int16_t in;

	Filter * result = (Filter *)malloc(sizeof(Filter));
	Parallel_filter_data * private = (Parallel_filter_data *)malloc(sizeof(Parallel_filter_data)); 
	for (i = 0; i < MAX_PARALLEL; i++) { //init sub filters
		sprintf(tbuf, "filter%d", i);
		in = ui_gint(tbuf, -1, FTABLE_LEN); //request filter uid.
		if (in == -1) { //-1 to denote NULL.
			private->filters[i] = NULL;
		}
		else {
			private->filters[i] = (ftable_creators[in])(); //create the new sub filter.
		}
		sprintf(tbuf, "gain%d", i);
		private->gains[i] = ui_gfloat(tbuf, 0.00, 2.00);
	}
	result->privateData = (void *)private; //cast private
	result->next = NULL;
	result->apply = &parallel_filter_apply;
	result->show = &parallel_filter_show;
	return result;
}

/* parallel_filter_apply
* Apply each sub filter.
* Returns: Output value.
*/
int16_t parallel_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr) {
	uint8_t i;
	int16_t initial = buffer[ptr];
	Parallel_filter_data * private = (Parallel_filter_data *)((Filter *)self)->privateData;
	int16_t result = 0;
	int16_t temp; //passed along subchains
	Filter * cur;
	for (i = 0; i < MAX_PARALLEL; i++) { //apply sub filters
		if (private->filters[i] != NULL) {
			cur = private->filters[i];
			temp = initial;
			temp = (cur->apply)(cur, temp, buffer, ptr); //apply the filter
			result += temp*(private->gains[i]);
			buffer[ptr] = initial; //restore for next subchain
		}
	}
	return result;
}

/* parallel_filter_show
* Write filter information to serial for debugging.
*/
void parallel_filter_show(void * self) {
	uint8_t i;
	Parallel_filter_data * private = (Parallel_filter_data *)((Filter *)self)->privateData;
	tty_printf("(Parallel filter @ 0x%x)\r\n", self);
	for (i = 0; i < MAX_PARALLEL; i++) {
		if (private->filters[i] != NULL) {
			tty_printf("\t%d@%f: ", i, private->gains[i]);
			(*private->filters[i]->show)(private->filters[i]); //run filter's show function
		}
	}
}
