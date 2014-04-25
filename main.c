/* main.c
* Contains sampling IRQ handler (TIMER 0), setup for onboard devices and user
* interface.
* Written by: David Wotherspoon, Alex Petherick, Mike Jenic
*/
#include "stdlib.h"
#include "lpc_types.h"
#include "dbg.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_wdt.h"
#include "queue.h"
#include "ui_serial.h"
#include "global.h"
//sd
#include "sd.h"
#define SD_EN

#define TIM_DEV LPC_TIM0
#define START()	TIM_Cmd(TIM_DEV, 1)
#define STOP()	TIM_Cmd(TIM_DEV, 0)

/* Globals */
volatile int16_t buffer[BUF_LEN]; //main buffer
volatile uint16_t bptr = 0; //buffer pointer
volatile int16_t avals[3]; //store channel values
volatile uint8_t pass = 0; //pass through flag
volatile uint32_t chain_time = 0; //time chain took.


/* TIMER0_IRQHandler
* Reads 3 ADC channels, selects the median and applies filter chain.
*/
void TIMER0_IRQHandler(void) {
	WDT_Feed(); //refill WDT
	avals[0] = ADC_ChannelGetData(LPC_ADC, 0);
	avals[1] = ADC_ChannelGetData(LPC_ADC, 4);
	avals[2] = ADC_ChannelGetData(LPC_ADC, 5);

	//find the median of the 3 avals
	#ifndef SD_EN
	   if(avals[0] > avals[1]) {
	        if(avals[1] > avals[2]) { buffer[bptr] = avals[1]-2048; }
	        else if(avals[0] > avals[2]) { buffer[bptr] = avals[2]-2048; }
	        else { buffer[bptr] = avals[0]-2048; }
	    } else {
	        if(avals[1] < avals[2]) { buffer[bptr] = avals[1]-2048; }
	        else if(avals[0] < avals[2]) { buffer[bptr] = avals[2]-2048; }
	        else { buffer[bptr] = avals[0]-2048; }
	    }
		if (buffer[bptr] > 1900) { //try and remove saturation spikes/use previous
			buffer[bptr] = buffer[(bptr == 0) ? BUF_LEN : (bptr-1)];	
		}
	#endif

	DAC_UpdateValue(LPC_DAC, ((pass ? buffer[bptr] : chain_apply(buffer, bptr))+2048)>>2); //move back up and shift
	bptr = (bptr == (BUF_LEN - 1)) ? 0 : bptr + 1; //increment the pointer.
	TIM_ClearIntPending(TIM_DEV, TIM_MR0_INT); //clear IRQ
	chain_time = WDT_GetCurrentCount();
	WDT_ClrTimeOutFlag(); //clear timeout
}

/* setup
* Initalise serial, match timer, DAC and ADC.
*/
void setup(void) { //preform setup routines for onboard devices
	PINSEL_CFG_Type pcfg;	
	TIM_TIMERCFG_Type tim_cfg;
	TIM_MATCHCFG_Type mcfg;

	tty_init();
	//Drive other ADC inputs low (GPIO).
	pcfg.OpenDrain = 0;
	pcfg.Pinmode = 0;
	pcfg.Portnum = 0;
	pcfg.Funcnum = 0;
	pcfg.Pinnum = 24; //mBed 16 AD1
	PINSEL_ConfigPin(&pcfg);
	pcfg.Pinnum = 25; //mBed 17 AD2
	PINSEL_ConfigPin(&pcfg);	
	GPIO_SetDir(0, (1 << 24 | 1 <<25), 1); //port 0 ADC to digital out
	GPIO_SetValue(0, (1 << 24 | 1 <<25));
	//configure ADC
	pcfg.Pinnum = 23; //mBed 15
	pcfg.Funcnum = 1;
	PINSEL_ConfigPin(&pcfg); //ADC Pin AD00
	pcfg.Pinnum = 30;
	pcfg.Portnum = 1;
	pcfg.Funcnum = 3;
	PINSEL_ConfigPin(&pcfg); //ADC Pin AD04
	pcfg.Portnum = 1; //mBed 20 AD5
	pcfg.Pinnum = 31;
	PINSEL_ConfigPin(&pcfg); //ADC Pin AD05
	//init match timer
	tim_cfg.PrescaleOption = TIM_PRESCALE_USVAL; //use microseconds
	tim_cfg.PrescaleValue = 1;
	TIM_Init(TIM_DEV, TIM_TIMER_MODE, &tim_cfg);//Allow setting of mode one day	
	mcfg.MatchChannel = 0;
	// Enable interrupt when MR0 matches the value in TC register
	mcfg.IntOnMatch   = 1;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	mcfg.ResetOnMatch = 1;
	//Stop on MR0 if MR0 matches it
	mcfg.StopOnMatch  = 0;
	//Toggle MR0.0 pin if MR0 matches it
	mcfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	// Set Match value, count value of 10000 (10000 * 100uS = 1000000us = 1s --> 1 Hz)
	mcfg.MatchValue = S_INTERVAL;
	TIM_ConfigMatch(TIM_DEV, &mcfg);
	STOP();
	//init ADC
	ADC_Init(LPC_ADC, ADC_RATE);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, RESET); //clear interrupts
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN4, RESET); 
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN5, RESET);
	ADC_ChannelCmd(LPC_ADC, 0, ENABLE);
	ADC_ChannelCmd(LPC_ADC, 4, ENABLE);
	ADC_ChannelCmd(LPC_ADC, 5, ENABLE);
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01); //ADC_START_CONTINUOUS);
	ADC_BurstCmd(LPC_ADC, ENABLE);
	//init DAC
	pcfg.Pinmode = 0;
	pcfg.Funcnum = 2;
	pcfg.Pinnum = 26;
	pcfg.Portnum = 0;
	PINSEL_ConfigPin(&pcfg);
	DAC_SetBias(LPC_DAC, 0);
	DAC_Init(LPC_DAC);
	WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_INT_ONLY); //init watch dog timer.
	NVIC_EnableIRQ(TIMER0_IRQn);
}


#ifndef SD_EN
	/* main (for ADC playout.)
	* Calls setup, user interface and starts sample timer.
	* Written by: David Wotherspoon, Alex Petherick, Mike Jenic
	*/
	int main(void) {
		setup(); //configure onboard devices.
		ui_flist(); //prints filter list
		ui_buildchain(); //begin chain configuration
		WDT_Start(0); //will mean feed is 0xFF + 0
		START(); //start the sample timer.
		chain_debug(); //print chain config
		while (1) {
			ui_do(&pass, &chain_time);
		}
	}
#else
	/* main (for SD card playout.)
	* Calls setup, reads SD card and starts sample timer.
	* Written by: David Wotherspoon
	*/
	int main(void) {
		setup();
		sd_start();
		wav_read("demo.wav", buffer);
		ui_flist();
		ui_buildchain();
		START(); //start sample timer.
		chain_debug();
		while (1) {
			sd_do(buffer, bptr);
		}
	}
#endif
