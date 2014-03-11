/* lp_filter.h
* Header for lp_filter.c.
* Written By: Mike Jenic
*/

#include "filter.h"
//@name Low Pass Filter
//@desc Butterworth low pass.
//@params [int](0:20000)corner,[int](0:128)order

typedef struct Lp_filter_data {
	float gain;
	uint16_t corner;
	uint8_t order;
	float * coeffs;
} Lp_filter_data;

Filter * lp_filter_create(void);
int16_t lp_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr);
void lp_filter_show(void * self);
