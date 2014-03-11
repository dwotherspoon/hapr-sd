/* tremolo_filter.h
* Header for tremolo_filter.c.
*/
#include "filter.h"
//@name Tremolo Filter
//@desc Filter adding tremolo/modulation.
//@params [float](0.00:0.50)depth,[int](0:1000)frequency

//tremolo filter private data.
typedef struct Tremolo_filter_data {
	float depth;
	uint16_t freq;
	float x;
	float inc;
} Tremolo_filter_data;

Filter * tremolo_filter_create(void);
int16_t tremolo_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr);
void tremolo_filter_show(void * self);
