/* parallel_filter.h
* Header for parallel_filter.c
* Written by: David Wotherspoon
*/
#include "filter.h"
//@name Parallel Filter
//@desc Allows up to 4 filters to be mixed in parallel.
//@params [float](0.00:2.00)gain0,[int](-1:128)filter0,[float](0.00:2.00)gain1,[int](-1:128)filter1,[float](0.00:2.00)gain2,[int](-1:128)filter2,[float](0.00:2.00)gain3,[int](-1:128)filter3 

#define MAX_PARALLEL 4

//private data for parallel filters.
typedef struct Parallel_filter_data {
	Filter * filters[MAX_PARALLEL]; //store sub filters
	float gains[MAX_PARALLEL]; //gains for mix percentage
} Parallel_filter_data;

Filter * parallel_filter_create(void);
int16_t parallel_filter_apply(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr);
void parallel_filter_show(void * self);
