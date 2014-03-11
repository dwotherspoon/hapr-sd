#include "stdlib.h"
#include "lpc_types.h"
#include "tty_mbed.h"
#include "chanfs/ff.h"
#include "dbg.h"
#include "dac_mbed.h"
#include "tim_mbed.h"

uint8_t tbuf[8];
uint32_t total_samples;
uint32_t samplerate;
volatile uint16_t cur_s = 0; //current sample counter
uint8_t samples[1024];
FIL fsrc; 
uint8_t res;

void TIMER0_IRQHandler(void) { //Timer to send samples
	dac_setval(samples[cur_s]<<2);
	tim_clearint(0);
	if (cur_s == 1023) {
		cur_s = 0;
	}
	else {
		cur_s++;
	}
}

int read_wav(char * fpath) { //https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
	int read, i;
	uint32_t chunksize;
	uint32_t subchunk1size;
	uint16_t audioformat;
	uint16_t numchannels;
	uint32_t byterate;
	uint16_t blockalign;
	uint16_t samplewidth;

	uint32_t subchunk2size;

	if (f_open(&fsrc, fpath, FA_READ | FA_OPEN_EXISTING) == 0) {
		f_read(&fsrc, tbuf, 4, &read); //Chunk ID
		ASSERT((tbuf[0] == 'R') && (tbuf[1] == 'I') && (tbuf[2] == 'F') && (tbuf[3] == 'F'), "No RIFF header.");

		f_read(&fsrc, tbuf, 4, &read); //Chunk size
		chunksize = tbuf[3]<<24 | tbuf[2]<<16 | tbuf[1]<<8 | tbuf[0];
		tty_printf("Chunk size %lu\n\r", chunksize);
		
		f_read(&fsrc, tbuf, 4, &read); //Format
		ASSERT((tbuf[0] == 'W') && (tbuf[1] == 'A') && (tbuf[2] == 'V') && (tbuf[3] == 'E'), "Not a  WAVE file."); 
	
		f_read(&fsrc, tbuf, 4, &read); //SubChunk1ID
		ASSERT((tbuf[0] == 'f') && (tbuf[1] == 'm') && (tbuf[2] == 't') && (tbuf[3] == ' '), "No fmt chunk.");  

		f_read(&fsrc, tbuf, 4, &read); //SubChunk1Size
		subchunk1size = tbuf[3]<<24 | tbuf[2]<<16 | tbuf[1]<<8 | tbuf[0];
		tty_printf("Sub chunk 1(fmt) size: %lu\n\r", subchunk1size); 

		f_read(&fsrc, tbuf, 2, &read); //Audio format
		audioformat = tbuf[1]<<8 | tbuf[0];
		tty_printf("Audio format: %d\n\r", audioformat);
		ASSERT(audioformat == 1, "Audio is not PCM encoded.");

		f_read(&fsrc, tbuf, 2, &read); //NumChannels
		numchannels = tbuf[1]<<8 | tbuf[0];
		tty_printf("Number of channels: %d\n\r", numchannels);
		ASSERT(numchannels == 1, "Audio has more than one channel.");

		f_read(&fsrc, tbuf, 4, &read); //SampleRate
		samplerate = tbuf[3]<<24 | tbuf[2]<<16 | tbuf[1]<<8 | tbuf[0];
		tty_printf("Sample rate is: %lu\n\r", samplerate); 

		f_read(&fsrc, tbuf, 4, &read); //ByteRate
		byterate = tbuf[3]<<24 | tbuf[2]<<16 | tbuf[1]<<8 | tbuf[0];
		tty_printf("Byte rate is: %lu\n\r", byterate); 
		
		f_read(&fsrc, tbuf, 2, &read); //Block align
		blockalign = tbuf[1]<<8 | tbuf[0];
		
		f_read(&fsrc, tbuf, 2, &read); //Sample width
		samplewidth = tbuf[1]<<8 | tbuf[0];
		tty_printf("Sample width: %d\n\r", samplewidth);
		ASSERT(samplewidth == 8, "Sample width must be 8 bit.")
		
		for (i = 16; i < subchunk1size; i++) { 
			f_read(&fsrc, tbuf, 1, &read); //don't want to risk an overflow
		} //read excess
		//Data subchunk.
		f_read(&fsrc, tbuf, 4, &read); //Format
		ASSERT((tbuf[0] == 'd') && (tbuf[1] == 'a') && (tbuf[2] == 't') && (tbuf[3] == 'a'), "No data chunk."); 
		
		f_read(&fsrc, tbuf, 4, &read); //Subchunk 2 size
		subchunk2size = tbuf[3]<<24 | tbuf[2]<<16 | tbuf[1]<<8 | tbuf[0];
		tty_printf("Sub chunk 2 size: %lu\n\r", subchunk2size); 
		//TODO: Compute sample number.
	}
	else {
		char error[64];
		snprintf(error, sizeof(error), "File %s does not exist.", fpath);
		error[sizeof(error)-1] = 0;
		THROW(error);	
	}
	return 0;	
}

int main(void) {
	FATFS fs;         /* Work area (file system object) for logical drive */
	tty_init();
	dac_init();
	tty_printf("Mounting file system: %d\n\r", f_mount(&fs, "0:", 1));
	tty_printf("Enter file to play:\n\r");
	read_wav(tty_gets());
	f_read(&fsrc, samples, 512, &res); //prefill bottom
	tim_init(0, 1); //init LPC_TIM0, no prescale
	tim_configm(0, 0, 1, 0, 1, (int)(1000000/(samplerate))); //Config match for dev, chan, irq, stop, rst, val
	NVIC_EnableIRQ(TIMER0_IRQn);
	tim_state(0, 1); //enable int
	while (1) {
			f_read(&fsrc, (uint8_t *)(&samples[512]), 512, &res);	 //fill top
			WARN(res < 512, "Under-read.");
			while(cur_s < 512); //spin
			f_read(&fsrc, (uint8_t *)(samples), 512, &res);	 //fill bottom
			WARN(res < 512, "Under-read.");
			while (cur_s > 512); //spin
	}
	return 0;
}
