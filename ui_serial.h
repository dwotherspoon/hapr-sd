/* ui_serial.h
* Header for ui_serial.c
* Written by: David Wotherspoon, Alex Petherick
*/
#include "lpc_types.h"

void ui_flist(void);
float ui_gfloat(char * name, float min1, float max1);
int ui_gint(char * name, int min, int max);
void ui_buildchain(void);
void ui_do(volatile uint8_t * pass, volatile uint32_t * ctime);