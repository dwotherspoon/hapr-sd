/* Test Filter
* Passthrough filter for testing purposes
* Written by David Wotherspoon, Mike Jenic
*/

#include "test_filter.h"
#include <stdlib.h>
#include "dbg.h"


/*Test Filter Create
* Creates test filter and returns a pointer to the created filter
*/

Filter * test_filter_create(void) { //manufacture a pointer to a new test filter.
	Filter * result = (Filter *)malloc(sizeof(Filter));
	result->next = NULL;
	result->privateData = NULL;
	result->apply = &test_filter_apply;
	result->show = &test_filter_show;
	return result;
}

/* Apply filter
* Applies filter, has no effect and pass values through
*/

int16_t test_filter_apply(void * a, int16_t prev, volatile int16_t * b, volatile uint16_t c) {
	return prev;
}

/* filter show
* Write filter information to serial for debugging
*/

void test_filter_show(void * self) {
	tty_printf("(Test filter @ 0x%x)\r\n", self);
}
