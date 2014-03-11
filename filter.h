/* filter.h
* Type define for Filter linked queue and filter functions.
* Written by: David Wotherspoon, Alexander Petherick, Mike Jenic
*/
#include "lpc_types.h"
#ifndef F_H //stop double typedefs
#define F_H

typedef int16_t (*applyFilter)(void * self, int16_t prev, volatile int16_t * buffer, volatile uint16_t ptr);
typedef void (*showFilter)(void *  self);
/*
* Written by: David Wotherspoon, Alexander Petherick, Mike Jenic
*/
typedef struct Filter {
	void *privateData; //some private data of arbitary type
	struct Filter * next; //pointer to next filter in chain
	applyFilter apply; //pointer to filter function
	showFilter show; //pointer to show function (debugging)
} Filter;

typedef Filter * (*createFilter)(void);

#endif
