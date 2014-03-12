/* fir.c
* FIR calculation function.
* Written by: David Wotherspoon, Mike Jenic
*/
#include "fir.h"
#include "global.h"

/* do_fir *
* Carry out FIR calculation.
* Returns: FIR calculation result.
* Written by: David Wotherspoon, Mike Jenic
*/
int16_t do_fir(float * coeffs, uint16_t length, volatile int16_t * buffer, volatile uint16_t ptr) {
	uint16_t i;
	int16_t result = 0;
	for (i = 0; i < length; i++) { //for each coefficent.
		result += coeffs[i]*buffer[ptr]; //sum
		ptr = (ptr == 0) ? (BUF_LEN - 1) : ptr - 1; //inc
	}
	return result;
}
