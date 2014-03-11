/* Low Pass Filter
* Passes frequencies below a user input parameter value and attenuates higher frequencies
* Written by Mike Jenic
*/

#include "lp_filter.h"
#include <stdlib.h>
#include "dbg.h"
#include "ui_serial.h"
#include "fir.h"
#include "global.h"
#include "math.h"
//Low pass filter.
//http://www.exstrom.com/journal/sigproc/tdlpf.c
//http://www.kwon3d.com/theory/filtering/fil.html
//http://web.unbc.ca/~ytang/digital_filter.pdf

/*Create filter
* Creates low pass filter, requesting parameters from ui.
* Calculates coefficients for use when applying filter
* Returns a pointer to the created filter
*/
Filter * lp_filter_create(void) {
	uint16_t i;
	float fc, d1, d2, h;
	Filter * result = (Filter *)malloc(sizeof(Filter));
	ASSERT(result != 0, "Malloc failed.");
	Lp_filter_data * private = (Lp_filter_data *)malloc(sizeof(Lp_filter_data));
	ASSERT(private != 0, "Malloc failed.");
	//Take paramaters in from UI
	private->corner = ui_gint("corner", 0, S_RATE); //High Frequency CutOff
	private->order = ui_gint("order", 0 , 128); //Number of coefficients 
	private->coeffs = (float *)malloc(sizeof(float)*private->order);
	ASSERT(private->coeffs != 0, "Malloc failed.");
	//generate parameter values
	fc = (M_PI*2*private->corner)/S_RATE; //CutOff frequency as a fraction of Pi
	d1 = ((float)private->order - 1.0)/2.0;

	for( i = 0; i < private->order; i++) { 
    	d2 = (float)i - d1;
    	h = d2 == 0 ? fc / M_PI : sin( fc * d2 ) / ( M_PI * d2 );
    	private->coeffs[i] = h; //create array of calculated coefficients
    	tty_printf("%f %f\n\r", h, M_PI);
    }
	//construct filter.
	result->privateData = (void *)private; //cast private to void pointer
	result->next = NULL;
	result->apply = &lp_filter_apply;
	result->show = &lp_filter_show;
	return result;
}

/* Filter apply
* Applies filter by using calculated coefficient values to peform FIR calculation (see fir.c)
*/

int16_t lp_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr) {
	Lp_filter_data * private = (Lp_filter_data *)(((Filter *)self)->privateData);
	return do_fir(private->coeffs, private->order, buffer, ptr);
}

/* filter show
* Write filter information to serial for debugging
*/

void lp_filter_show(void * self) {
	Lp_filter_data * private = (Lp_filter_data *)(((Filter *)self)->privateData);
	tty_printf("(Low pass filter @ 0x%x, corner %d, order %d)\r\n", self, private->corner, private->order);
}
