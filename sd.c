/* sd.c
* Contains functions for SD card and reading wave files.
* Written by: David Wotherspoon
*/
#include "sd.h"
#include "chanfs/ff.h"
#include "chanfs/diskio.h"
#include "global.h"
#include "dbg.h"
#include "stdio.h"

/* Globals */
FIL fsrc; // File data struct (ChanFS)
FATFS fs; //Work area (file system object) for logical drive
uint8_t sd_buf[512]; //scratch pad for buffer read bytes.
UINT read; //ChanFS UINT type
uint16_t lastb = 0; //last block filled in buffer.

/* sd_start
* Mount file system with chanFS
*/
void sd_start(void) {
	f_mount(&fs, "0:", 1);
}

/* sd_do
* Fills the sample buffer with sd card samples. Always fills the next 512 to be played.
* Assumes buffer has length multiple of 512.
*/
inline void sd_do(volatile int16_t * buf, volatile uint16_t ptr) { //sd buffer loop code
	static uint16_t bp; //previous block pointer
	if ((ptr > lastb)) { //fill next block when previous block is being played.
		f_read(&fsrc, sd_buf, 512, &read);
		lastb = ((lastb+512) == BUF_LEN) ? 0 : (lastb + 512);
		for (bp = 0; bp < 512; bp++) { //copy from sd_buf to main delay line
			buf[lastb+bp] = (sd_buf[bp]<<4)-2048;
		}
	}
}

/* wav_read
* Reads through header on wav file. Returns sample rate or throws an error if file is not valid.
*/
int wav_read(char * fpath) { //https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
	uint16_t i;
	uint32_t chunksize;
	uint32_t subchunk1size;
	uint16_t audioformat;
	uint16_t numchannels;
	uint32_t byterate;
	uint16_t blockalign;
	uint16_t samplewidth;
	uint16_t samplerate = 0;
	uint8_t tbuf[8];

	uint32_t subchunk2size;

	if (f_open(&fsrc, fpath, FA_READ | FA_OPEN_EXISTING) == 0) { //open file
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
	return samplerate;	
}