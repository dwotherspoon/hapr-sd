/* queue.h
* Header for queue.c.
* Written by: David Wotherspoon
*/
#include "lpc_types.h"
#include "filter.h"

void enqueue(uint16_t id);
void dequeue(void);
int16_t chain_apply(volatile int16_t * buf, volatile uint16_t ptr);
void make_queue(Filter * in);
void chain_debug(void);
void dequeuep(uint16_t pos);
int16_t find_filter(void * apply);
void replace(uint16_t pos, uint16_t id);