/* hp_filter.h
* Header for hp_filter.c.
* Written by: Mike Jenic
*/

#include "filter.h"
//@name High Pass Filter
//@desc Butterworth high pass.
//@params [int](0:20000)corner,[int](0:128)order

typedef struct Hp_filter_data {
	float gain;
	uint16_t corner;
	uint8_t order;
	float * coeffs;
} Hp_filter_data;

Filter * hp_filter_create(void);
int16_t hp_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr);
void hp_filter_show(void * self);
