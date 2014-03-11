/* Band Pass Filter
* Combination of low pass and high pass filter
* Passes frequencies between two user input parameter values
* Written by: Mike Jenic
*/

#include "bp_filter.h"
#include <stdlib.h>
#include "dbg.h"
#include "ui_serial.h"
#include "fir.h"
#include "global.h"
#include "math.h"

//Band pass filter.
//http://www.exstrom.com/journal/sigproc/tdbpf.c
//http://www.kwon3d.com/theory/filtering/fil.html
//http://web.unbc.ca/~ytang/digital_filter.pdf

/*Create filter
* Creates band pass filter, requesting parameters from ui.
* Calculates coefficients for use when applying filter
* Returns a pointer to the created filter
*/

Filter * bp_filter_create(void) {
	uint16_t i;
	float fc1, fc2, d1, d2, h;
	Filter * result = (Filter *)malloc(sizeof(Filter));
	ASSERT(result != 0, "Malloc failed.");
	Bp_filter_data * private = (Bp_filter_data *)malloc(sizeof(Bp_filter_data));
	ASSERT(private != 0, "Malloc failed.");
	//Take paramaters in from UI
	private->corner_low = ui_gint("corner_low", 0, S_RATE); //Low Frequency CutOff
	private->corner_high = ui_gint("corner_high", 0, S_RATE); //High Frequency CutOff
	private->order = ui_gint("order", 0 , 128); //Number of coefficients
	private->coeffs = (float *)malloc(sizeof(float)*private->order);
	ASSERT(private->coeffs != 0, "Malloc failed.");
	//generate parameter values
	fc1 = (M_PI*2*private->corner_low)/S_RATE; //Low frequency as a fraction of Pi
	fc2 = (M_PI*2*private->corner_high)/S_RATE; //High frequency as a fraction of Pi	
	d1 = ((float)private->order - 1.0)/2.0;  

	for( i = 0; i < private->order; i++) { 
    	d2 = (float)i - d1;
    	h = d2 == 0 ? (fc2 - fc1) / M_PI : (sin(fc2 * d2) - sin(fc1 * d2 )) / ( M_PI * d2);
    	private->coeffs[i] = h; //create array of calculated coefficients
    }
	//construct filter.
	result->privateData = (void *)private; //cast private to void pointer
	result->next = NULL;
	result->apply = &bp_filter_apply;
	result->show = &bp_filter_show;
	return result;
}

/* Filter apply
* Applies filter by using calculated coefficient values to peform FIR calculation (see fir.c)
*/

int16_t bp_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr) {
	Bp_filter_data * private = (Bp_filter_data *)(((Filter *)self)->privateData);
	return do_fir(private->coeffs, private->order, buffer, ptr);
}

/* filter show
* Write filter information to serial for debugging
*/

void bp_filter_show(void * self) {
	Bp_filter_data * private = (Bp_filter_data *)(((Filter *)self)->privateData);
	tty_printf("(Band pass filter @ 0x%x, corner_high %d, corner_low %d, order %d)\r\n", self, private->corner_low, private->corner_high, private->order);
}
