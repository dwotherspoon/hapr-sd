/* fir.h
* Header for fir.c.
* Written by: David Wotherspoon
*/
#include "lpc_types.h"

int16_t do_fir(float * coeffs, uint16_t length, volatile int16_t * buffer, volatile uint16_t ptr);
