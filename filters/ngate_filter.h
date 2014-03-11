#include "filter.h"
//@name Noise gate
//@desc Noise gate filter.
//@params [int](0:2048)threshold,[float](0:1)reduction
//http://news.beatport.com/blog/2008/10/29/tutorial-how-to-use-a-noise-gate/
//Written By: Alex Petherick

typedef struct Ngate_filter_data {
	uint16_t threshold;
	float reduction; 
} Ngate_filter_data;

Filter * ngate_filter_create(void);
int16_t ngate_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr);
void ngate_filter_show(void * self);
