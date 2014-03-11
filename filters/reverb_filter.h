/* echo_filter.h
* Header for reverb_filter.c
* Written by: David Wotherspoon
*/
#include "filter.h"
//@name Reverb Filter
//@desc Filter adding reverberation.
//@params [float](0.00:2.00)gain,[int](0:4095)delay

//struct for reverb filter data.
typedef struct Reverb_filter_data {
	float gain; //gains for mix percentage
	uint16_t delay;
} Reverb_filter_data;

Filter * reverb_filter_create(void);
int16_t reverb_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr);
void reverb_filter_show(void * self);
