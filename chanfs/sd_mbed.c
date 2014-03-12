/*  sd_mbed.c
* Contains SD initialisation, read block(s) and write block(s).
* Written by: David Wotherspoon
*/
#include "sd_mbed.h"

SSP_DATA_SETUP_Type sd_data;
uint16_t const SD_RETRY = 400;
int sd_type;
SSP_CFG_Type sspcfg;

/* ssp_init
* Initialise SSP/SPI device.
*/
void ssp_init() { //initalise ssp for the SD
	sd_cs(1); //disable the card. (active low)
	PINSEL_CFG_Type pcfg; //pin configuration

	pcfg.Portnum = SD_PORT;
	pcfg.OpenDrain = 0;
	pcfg.Pinmode = 0;
	pcfg.Funcnum = SD_FUNC;
	
	//SD_SDO
	pcfg.Pinnum = SD_SDO;
	PINSEL_ConfigPin(&pcfg);

	//SD_SDI
	pcfg.Pinnum = SD_SDI;
	PINSEL_ConfigPin(&pcfg);

	//SD_CLK
	pcfg.Pinnum = SD_CLK;
	PINSEL_ConfigPin(&pcfg);

	//SD_CS
	pcfg.Funcnum = 0; //GPIO
	pcfg.Pinnum = SD_CS;
	PINSEL_ConfigPin(&pcfg);

	GPIO_SetDir(SD_PORT, (1<<SD_CS), 1); //GPIO for CS
	//SD_CD - Not on UoY Host board, just hope SD is inserted.
	

	SSP_ConfigStructInit(&sspcfg); //Configure struct
	sspcfg.ClockRate = 1000000; //set clock rate.

	SSP_Init(SD_DEV, &sspcfg);
	SSP_Cmd(SD_DEV, ENABLE);
}

/* spi_tx
* Transmit data to SD card.
*/
void spi_tx(uint16_t data) { 
	SSP_SendData(SD_DEV, data);
}

/* spi_rx
* Receive data from SD card.
* Returns: Data received.
*/
uint16_t spi_rx(void) {
	return SSP_ReceiveData(SD_DEV);
}

/* sd_cmd
* Send a command to SD card and read response.
* Returns: R1 response.
*/
uint8_t sd_cmd(uint8_t cmd, uint32_t arg, uint8_t crc) { //send a command
	int c;
	uint8_t r1;
	uint8_t buf[] = { cmd | 0x40, arg>>24, arg>>16, arg>>8, arg, crc }; //always 6
	spi_readwrite(0xFF);//dummy
	sd_cs(0); //enable

	for (c = 0; c < 6; c++) { //use rxtx?
			spi_tx(buf[c]);
	}
	//read

	for (c = 0; c < SD_RETRY; c++) {
		if ((r1 = spi_readwrite(0xFF)) != 0xFF) {
			break;		
		}	
	}
	sd_cs(1); //disable
	spi_readwrite(0xFF); //dummy
	return r1; //spi_rx();
}

/* sd_cmdh
* Send a command to SD card, read response and hold /CS low.
* Returns: R1 response.
*/
uint8_t sd_cmdh(uint8_t cmd, uint32_t arg, uint8_t crc) { //send a command and hold cs
	int c;
	uint8_t r1;
	uint8_t buf[] = { cmd | 0x40, arg>>24, arg>>16, arg>>8, arg, crc }; //always 6
	spi_readwrite(0xFF);//dummy
	sd_cs(0); //enable

	for (c = 0; c < 6; c++) { //use rxtx?
			spi_tx(buf[c]);
	}
	//read

	for (c = 0; c < SD_RETRY; c++) {
		if ((r1 = spi_readwrite(0xFF)) != 0xFF) {
			break;		
		}	
	}
	return r1; 
}

/* spi_readwrite
* Send data to SD card and read response.
* Returns: reponse.
*/
uint16_t spi_readwrite(uint8_t in) {
	while (SSP_GetStatus(SD_DEV, SSP_STAT_BUSY) ==  SET); //wait for ready
	SSP_SendData(SD_DEV, in);
	while (SSP_GetStatus(SD_DEV, SSP_STAT_RXFIFO_NOTEMPTY) == RESET); //wait for empty
	return SSP_ReceiveData(SD_DEV);
}

/* sd_init
* Initialise SPI bus and SD card.
* Returns: 0 if successful. 
*/
uint8_t sd_init(void) { //http://elm-chan.org/docs/mmc/mmc_e.html
	int i;
	int r1;
	uint8_t buf[6] = {0};
	ssp_init();
	//tty_printf("SPI bus initialised.\n\r");
	
	for (i = 0; i < 10; i++) { //send 74 clocks (80)
		spi_readwrite(0xFF);
	}
		
	for (i = 0; i < SD_RETRY; i++) { //try until retry limit
		r1 = sd_cmd(CMD0, 0x00, 0x95);
		if (r1 == 0x01) {
			//card is idle.		
			break;
		}	
	}
	ASSERT(r1 == 0x01, "Card could not be soft reset (is there a card?)");
	
	r1 = sd_cmdh(CMD8, 0x1AA, 0x87);
	
	if (r1 == 0x01) { //SD card idle, read CMD8 response
		buf[0] = spi_readwrite(0xFF);
		buf[1] = spi_readwrite(0xFF);
		buf[2] = spi_readwrite(0xFF);
		buf[3] = spi_readwrite(0xFF);
		
		sd_cs(1);//disable now we've read the response
		ASSERT((buf[0] == 0x00 || buf[1] == 0x00 || buf[2] == 0x01 || buf[3] == 0xAA), "SD card not compatible.");
		//tty_printf("Card responded to CMD8, assuming SDv2.\n\r");
		//Now try and init.
		for (i = 0; i < SD_RETRY; i++) { //try until retry limit
			r1 = sd_cmd(CMD55, 0x00, 0x01);
			if (r1 == 0x01) {
				r1 = sd_cmd(ACMD41, 0x40000000, 0x01);
				if (r1 == 0x00) {
					break;				
				}
			}
			else {
				THROW("CMD55 gave an invalid response.");			
			}
		}
		ASSERT(r1 == 0x00, "ACMD41 init failed!");
		//tty_printf("Card initialised (%d retries)\n\r", i);
		/* Read OCR */
		r1 = sd_cmdh(CMD58, 0x00, 0x01);
		ASSERT(r1 == 0x00, "Invalid response to 0x00.");
		buf[0] = spi_readwrite(0xFF);
		buf[1] = spi_readwrite(0xFF);
		buf[2] = spi_readwrite(0xFF);
		buf[3] = spi_readwrite(0xFF);
		sd_cs(1); //disable

		//tty_printf("CMD58 returned; %x, %x, %x, %x\n\r", buf[0], buf[1], buf[2], buf[3]);
		if (buf[0] & 0x40) {
			sd_type = CARDTYPE_SDV2HC;
			//tty_puts("Card is SD v2 HC\r\n");
			return 0x00;
		}
		else {
			sd_type = CARDTYPE_SDV2;
			//tty_puts("Card is SD v2\r\n");
		}
	}
	else if (r1 == 0x05) { //illegal
		sd_cs(1); //disable card (no response to read)
		tty_printf("Card did not respond to CMD8, assuming SDv1.\n\r");
		sd_type = CARDTYPE_SDV1;
		THROW("SD v1.");
	}
	else { //something went wrong
		THROW("Invalid response to CMD8.");	
	}
	return 0x02; //no medium error
}

/* sd_rl
* Light read light.
*/
void sd_rl(int state) {
	if (state) {
		GPIO_SetValue(1, (1<<18));
	}
	else {
		GPIO_ClearValue(1, (1<<18));
	}
}

/* sd_wl
* Light write light.
*/
void sd_wl(int state) {
	if (state) {
		GPIO_SetValue(1, (1<<23));
	}
	else {
		GPIO_ClearValue(1, (1<<23));
	}
}

/* sd_cs
* Set SD card /CS line state.
*/
void sd_cs(int state) { //chipselect
	if (state) {
		GPIO_SetValue(SD_PORT, (1<<SD_CS));
	}
	else {
		GPIO_ClearValue(SD_PORT, (1<<SD_CS));
	}
}

/* sd_readblock
* Read a single block at lba into buf.
* Returns: 0 on success.
*/
uint8_t sd_readblock(uint8_t * buf, uint32_t lba) {
	uint8_t r1;
	int i;

	if (sd_type != CARDTYPE_SDV2HC) {
		lba <<= 9; //shift lba up 512 (block size)
	}

	r1 = sd_cmdh(CMD17, lba, 0x01);
	
	ASSERT(r1 == 0x00, "CMD17 failed.");

	//tty_printf("ACCEPTED\n\r");
	for (i = 0; i< SD_RETRY; i++) { //wait for 0xFE
		r1 = spi_readwrite(0xFF);
		if (r1 == 0xFE) {
			break;
		}
	}
	ASSERT(r1 == 0xFE, "Read timeout.");
	
	for (i=0; i< (SD_BLOCK); i++) {
		*buf++ = spi_readwrite(0xFF);
	}
	//IGNORE CRC (2 bytes)
	spi_readwrite(0xFF);
	spi_readwrite(0xFF);
	sd_cs(1);
	spi_readwrite(0xFF);
	//DONE
	return 0x00;
}

/* sd_readblocks
* Read num blocks into buf, starting at lba.
* Returns: 0 on success.
*/
uint8_t sd_readblocks(uint8_t * buf, uint32_t lba, uint32_t num) {
	uint8_t r1;
	uint32_t i, pkt, b;
	if (sd_type != CARDTYPE_SDV2HC) {
		lba <<= 9;	
	}
	r1 = sd_cmdh(CMD18, lba, 0x01);
	ASSERT(r1 == 0x00, "CMD18 failed.");

	for (pkt = 0; pkt < num; pkt++) {
		for (i = 0; i < (SD_RETRY); i++) { //wait for 0xFE (packet start data token)
			r1 = spi_readwrite(0xFF);
			if (r1 == 0xFE) {
				break;
			}
		}
		ASSERT(r1 == 0xFE, "CMD18 block wait timed out.");
		for (b = 0; b < (SD_BLOCK); b++) { //read this block
			*buf++ = spi_readwrite(0xFF);
		}
		spi_readwrite(0xFF); //read CRC
		spi_readwrite(0xFF);
	}
	r1 =  sd_cmdh(CMD12, 0x00, 0x01); //STOP
	
	for (i = 0; i< SD_RETRY; i++) { //wait for response from cmd
		r1 = spi_readwrite(0xFF);
		if (r1 == 0x00) {
			break;		
		}		
	}
	ASSERT(r1 == 0x00, "Card did not respond to CMD12.");
	/* for (i = 0; i < SD_RETRY; i++) {
		r1 = spi_readwrite(0xFF);
		if (r1 == 0xFF) {
			break;		
		}
	} */
	while (spi_readwrite(0xFF) != 0xFF) { } //wait until card is not busy
	sd_cs(1);
	spi_readwrite(0xFF);
	return 0;
}

/* sd_writeblock
* Write block from buf to lba.
* Returns: 0 on success.
*/
uint8_t sd_writeblock(uint8_t * buf, uint32_t lba) {
	uint8_t r1;
	int i;

	if (sd_type != CARDTYPE_SDV2HC) {
		lba <<= 9; //shift lba up 512 (block size)
	}

	r1 = sd_cmd(CMD24, lba, 0x01);
	ASSERT(r1 == 0x00, "CMD24 failed.");
	spi_readwrite(0xFF); //flush
	sd_cs(0);

	spi_readwrite(0xFF); //wait 8 before transfer
	spi_readwrite(0xFE); //start of packet
	
	for (i=0; i< (SD_BLOCK); i++) { //write data
		spi_readwrite(buf[i]);
	}
	
	//CRC (2 bytes)
	spi_readwrite(0xFF);
	spi_readwrite(0xFF);

	for (i = 0; i < SD_RETRY; i++) { //poll until we get non-0xFF (trans response)
		r1 = spi_readwrite(0xFF);
		if (r1 != 0xFF) {
			break;		
		}	
	}
	ASSERT((r1 & 0x0F) == 0x05, "Card rejected block."); 
	while (spi_readwrite(0xFF) != 0xFF) { } //wait until card is not busy
	sd_cs(1); //disable
	spi_readwrite(0xFF);
	//DONE
	return 0x00;
}

/* sd_writeblocks
* Write num blocks from buf starting at lba.
* Returns: 0 on success.
*/
uint8_t sd_writeblocks(uint8_t * buf, uint32_t lba, uint32_t num) {
	uint8_t r1;
	uint32_t i, pkt;
	if (sd_type != CARDTYPE_SDV2HC) {
		lba <<= 9;	
	}
	r1 = sd_cmdh(CMD25, lba, 0x01);
	ASSERT(r1 == 0x00, "CMD25 failed.");
	
	spi_readwrite(0xFF); //write 1 byte (as spec)
	spi_readwrite(0xFF); //write 1 byte (as spec)

	for (pkt = 0; pkt < num; pkt++) { //start writing bytes		
		spi_readwrite(0xFC); //data token
		for (i = 0; i<(SD_BLOCK); i++) { //data
			spi_readwrite(*buf++);
		}
		spi_readwrite(0xFF);
		spi_readwrite(0xFF); //dummy crc
		
		for (i = 0; i < SD_RETRY; i++) { //poll until we get non-0xFF (trans response)
			r1 = spi_readwrite(0xFF);
			if (r1 != 0xFF) {
				break;		
			}	
		}
		ASSERT((r1&0x0F) == 0x05, "Card rejected block.");
		while (spi_readwrite(0xFF) != 0xFF) { } //wait until card is not busy
	}
	
	spi_readwrite(0xFD); //stop tran
	while (spi_readwrite(0xFF) == 0xFF) { } //get to busy?
	while (spi_readwrite(0xFF) != 0xFF) { } //wait until card is not busy
	sd_cs(1); //disable
	spi_readwrite(0xFF); //done
	return 0x00;
}