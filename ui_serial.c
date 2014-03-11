/* ui_serial.c
* UI functionality for receiving variables and building filter chain.
* Written by: David Wotherspoon, Alex Petherick
*/
#include "ui_serial.h"
#include "filters/filter_table.h"
#include "dbg.h"
#include "queue.h"
#include "global.h"
#include "lpc17xx_nvic.h"


/* ui_flist
* List the filters avaliable (from filter_table) in JSON format.
* Written by: Alex Petherick
*/
void ui_flist(void) {
	uint8_t i = 0;
	tty_printf("S"); //Start character
	for (i = 0; i < (FTABLE_LEN); i++) {
		//{"id":1, "name":"Filter Name", "desc":"Filter Description", "params":"[int](0:50)delay,[float](0.00:50.00)gain" }\n
		//tty_printf("%s @ 0x%x\n\r\t-%s\n\r\t-%s\n\r", ftable_names[i], ftable_creators[i], ftable_descs[i], ftable_params[i]);
		//tty_printf("%d:\"%s\":\"%s\":%s;\n",i, ftable_names[i], ftable_descs[i], ftable_params[i]);
		tty_printf("{\"id\":\"%d\", \"name\":\"%s\", \"desc\":\"%s\", \"params\":\"%s\"}\n\r", i, ftable_names[i], ftable_descs[i], ftable_params[i]);
	}
	tty_printf("%c\r\n",0x003);
}

/* ui_buildchain
* Build the filter chain, terminating on -1.
* Written by: David Wotherspoon
*/
void ui_buildchain(void) { //build filter chain
 int16_t in = 0;
 while (in != -1) {
 	in = ui_gint("fid", -1, (FTABLE_LEN-1));
 	if (in != -1) {
 		enqueue(in);
 	}
 }
}

/* ui_gfloat 
* Receive a float via scanf between bounds.
* Returns: Float from user between min and max.
* Written by: Alex Petherick
*/
float ui_gfloat(char * name, float min1, float max1) { //request a float between min and max
	float ret;
	uint8_t valid = 0;
	while (!valid) {
		tty_printf("@%s;\n", name);
		valid = (tty_scanf("%f;", &ret)) && (ret >= min1) && (ret <= max1);
	}
	return ret;
}

/* ui_gint
* Receive a signed integer via scanf between bounds.
* Returns: Integer from user between min and max.
* Written by: Alex Petherick
*/
int ui_gint(char * name, int min, int max) { //request an integer between min and max
	int ret;
	uint8_t valid = 0;
	while (!valid) {
		tty_printf("@%s;\n", name);
		valid = (tty_scanf("%d;", &ret)) && (ret >= min) && (ret <= max);
	}
	return ret;
}

/* ui_do
* Main UI loop, accepts l and p to print load or switch to passthrough.
* Written by: David Wotherspoon
*/
void ui_do(volatile uint8_t * pass, volatile uint32_t * ctime) {
	static char fst;
	fst = tty_getchar(); //get char
	switch (fst) {
		case 'a': //add filter(s)
			ui_buildchain();
			break;
		case 'd': //print debug
			chain_debug();
			break;
		case 'l': //print load
			tty_printf("%f\n\r", ((float)(0xFF-(*ctime))/(float)S_INTERVAL)); //calculate load
			break;
		case 'p': //passthrough
			//tty_puts("passthrough\n\r"); 
			*pass = !(*pass);
			break;
		case 'r': //reset
			NVIC_SystemReset();
			break;
		default:
			break;
	}
}
