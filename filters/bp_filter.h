/* bp_filter.h
* Header for bp_filter.c.
* Written by: Mike Jenic
*/

#include "filter.h"
//@name Band Pass Filter
//@desc Butterworth band pass.
//@params [int](0:20000)corner_low,[int](0:20000)corner_high,[int](0:128)order

typedef struct Bp_filter_data {
	float gain;
	uint16_t corner_low;
	uint16_t corner_high;
	uint8_t order;
	float * coeffs;
} Bp_filter_data;

Filter * bp_filter_create(void);
int16_t bp_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr);
void bp_filter_show(void * self);
