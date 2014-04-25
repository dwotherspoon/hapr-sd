/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/
/* Modified by: David Wotherspoon					 */
/*-----------------------------------------------------------------------*/
#include "diskio.h"		/* FatFs lower layer API */
#include "sd_mbed.h"

/* Definitions of physical drive number for each media */
#define ATA		1
#define MMC		0
#define USB		2

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	//tty_printf("Disk init called %d\n\r", pdrv);
	DSTATUS stat;

	switch (pdrv) {
	case ATA :
		return STA_NODISK;

	case MMC :
		stat = sd_init();
		return stat;

	case USB :
		return STA_NODISK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	//DSTATUS stat;

	switch (pdrv) {
	case ATA :
		return STA_NODISK;
	case MMC :
		return 0;
	case USB :
		return STA_NODISK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{	
	//tty_printf("Reading LBA %d, %d sectors\n\r", sector, count);
	DRESULT res;
	sd_rl(1);
	switch (pdrv) {
	case ATA :
		return RES_PARERR;

	case MMC :
		if (count == 1) {
			res = sd_readblock(buff, sector);
		}
		else {
			res = sd_readblocks(buff, sector, count);
		}
		sd_rl(0);
		return res;

	case USB :
		return RES_PARERR;
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	DRESULT res;
	//tty_printf("Writing LBA %d, %d sectors\n\r", sector, count);
	sd_wl(1);	
	switch (pdrv) {
	case ATA :
		return RES_PARERR;

	case MMC :
		if (count == 1) {
			res = sd_writeblock((uint8_t *)buff, sector);
		}
		else {
			res = sd_writeblocks((uint8_t *)buff, sector, count);
		}
		sd_wl(0);
		return res;

	case USB :
		return RES_PARERR;
	}
	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	switch (pdrv) {
	case ATA :
		return RES_PARERR;

	case MMC :
		switch (cmd) {
			case CTRL_SYNC:
				return RES_OK;
			case GET_SECTOR_COUNT:
				return RES_OK;
			case GET_SECTOR_SIZE:
				*(WORD*)buff = 512;
				return RES_OK;
			case GET_BLOCK_SIZE:
				return RES_OK;
			case CTRL_ERASE_SECTOR:
				return RES_OK;
			default:
				return RES_OK;
		}
		return res;

	case USB :
		return RES_PARERR;
	}
	return RES_PARERR;
}
#endif
