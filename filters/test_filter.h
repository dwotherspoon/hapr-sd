/* test_filter.h
* Header for test_filter.c.
* Written by: David Wotherspoon, Mike Jenic
*/

#include "filter.h"
//@name Test Filter
//@desc Has absolutely no effect.
//@params 

Filter * test_filter_create(void); 
int16_t test_filter_apply(void * a, int16_t prev, volatile int16_t * b, volatile uint16_t c);
void test_filter_show(void * self);
