/* gain_filter.h
* Header for gain_filter.c
* Written by: David Wotherspoon
*/
#include "filter.h"
//@name Gain Filter
//@desc Simple linear gain filter.
//@params [float](0.00:2.00)gain

//private data for gain filter.
typedef struct Gain_filter_data {
	float gain;
} Gain_filter_data;

Filter * gain_filter_create(void);
int16_t gain_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr);
void gain_filter_show(void * self);
