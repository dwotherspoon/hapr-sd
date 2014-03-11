/* flanging_filter.h
* Header for flanging_filter.h
* Written by: David Wotherspoon
*/
#include "filter.h"
//@name Flanging/Chorus Filter
//@desc Flanging or chorus effect.
//@params [float](0.00:2.00)gain,[int](0:4095)delay,[int](0:1000)frequency,[int](0:2048)amplitude

//flanging filter private data
typedef struct Flanging_filter_data {
	uint16_t delay;
	float gain; //feedback
	float frequency; //Rate
	float amplitude; //Sweep depth
	float x; //current pos in sine wave
	float inc; // increment, based on freq
} Flanging_filter_data;


Filter * flanging_filter_create(void);
int16_t flanging_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr);
void flanging_filter_show(void * self);
