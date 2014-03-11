/* echo_filter.h
* Header for echo_filter.c
* Written by: David Wotherspoon
*/
#include "filter.h"
//@name Echo Filter
//@desc Filter adding some echo.
//@params [float](0.00:2.00)gain,[int](0:4095)delay

//struct for echo filter data.
typedef struct Echo_filter_data {
	float gain; //gains for mix percentage
	uint16_t delay;
} Echo_filter_data;

Filter * echo_filter_create(void); 
int16_t echo_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr);
void echo_filter_show(void * self);
