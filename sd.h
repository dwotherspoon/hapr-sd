/* sd.h
* Header file for sd.c
* Written by: David Wotherspoon
*/
#include "lpc_types.h"

void sd_start(void);
inline void sd_do(volatile int16_t * buf, volatile uint16_t ptr);
int wav_read(char * fpath, volatile int16_t * buf);