/*  sd_mbed.h
* Header for sd_mbed.c
* Written by: David Wotherspoon
*/
#include "lpc17xx_ssp.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc_types.h"
#include "tty_mbed.h"
#include "dbg.h"

/* pins */
	#define SD_PORT 0
	#define SD_FUNC 2
	
	#define SD_DEV LPC_SSP1
	#define SD_SDO 9
	#define SD_SDI 8
	#define SD_CLK 7

	#define SD_CS  11 //chip sel GPIO


/* SD SPI Command Set from http://elm-chan.org/docs/mmc/mmc_e.html */

#define CMD0   0 //Software reset.
#define CMD1   1 //Initiate init.
#define CMD8   8 //SDCv2; Voltage range check.
#define CMD9   9 //Read CSD register.
#define CMD10 10 //Read CID register.
#define CMD12 12 //Stop transmission.
#define CMD16 16 //Set block size.
#define CMD17 17 //Read a block.
#define CMD18 18 //Read multiple blocks.
#define CMD23 23 //MMC; Set blocks for next multiblock (r/w) command.
#define CMD24 24 //Write a block.
#define CMD25 25 //Write multiple blocks.
#define CMD55 55 //APP Cmd (leader for ACMDs)
	#define ACMD23 23 //SDC; Define block number for next multiblock write cmd.
	#define ACMD41 41 //SDC; Initiate init.
#define CMD58 58 //Read OCR

/* SD Card types */

#define CARDTYPE_MMC    0x00
#define CARDTYPE_SDV1   0x01
#define CARDTYPE_SDV2   0x02
#define CARDTYPE_SDV2HC 0x04

/* Misc */
#define SD_BLOCK 512

/* Prototypes */
uint8_t sd_init(void); //initialise sd card

uint8_t sd_readblock(uint8_t * buf, uint32_t lba);
uint8_t sd_readblocks(uint8_t * buf, uint32_t lba, uint32_t num);
uint8_t sd_writeblock(uint8_t *buf, uint32_t lba);
uint8_t sd_writeblocks(uint8_t * buf, uint32_t lba, uint32_t num);
void sd_cs(int state);
void sd_wl(int state);
void sd_rl(int state);
void ssp_init(void);
uint16_t spi_readwrite(uint8_t in);

