#Link for latest sourcery: https://sourcery.mentor.com/GNUToolchain/package12190/public/arm-none-eabi/arm-2013.11-24-arm-none-eabi-i686-pc-linux-gnu.tar.bz2

PKG=/usr/local/pkg
SOURCERY=~/hapr/arm-2013.11
ARCH=arm-none-eabi
CC=$(SOURCERY)/bin/$(ARCH)-gcc
HCC	= /usr/bin/gcc
OBJCOPY=$(SOURCERY)/bin/$(ARCH)-objcopy

USER:=$(shell whoami)

GNU_VERSION=4.8.1
THUMB2GNULIB=$(SOURCERY)/$(ARCH)/lib/thumb2
THUMB2GNULIB2=$(SOURCERY)/lib/gcc/$(ARCH)/$(GNU_VERSION)/thumb2

# "Cortex Microcontroller Software Interface Standard" Startup files
CMSIS=$(PKG)/lpc1700-cmsis-lite-2011.01.26-i686-1
CMSISINCLUDES=-I$(CMSIS)/include
CMSISFL=$(CMSIS)/lib/core_cm3.o \
	$(CMSIS)/lib/system_LPC17xx.o \
	$(CMSIS)/lib/startup_LPC17xx.o
LDSCRIPT = $(CMSIS)/lib/ldscript_rom_gnu.ld

CFLAGS=-Wextra -Wall -O2 -ffast-math -mcpu=cortex-m3 -mthumb  -mapcs-frame  -D__thumb2__=1 \
  -msoft-float  -gdwarf-2  -mno-sched-prolog  -fno-hosted  -mtune=cortex-m3 \
  -march=armv7-m  -mfix-cortex-m3-ldrd -ffunction-sections -fdata-sections \
  -D__RAM_MODE__=0 $(CMSISINCLUDES) -I. -Wno-unused-parameter

LDFLAGS=$(CMSISFL) -static -mcpu=cortex-m3 -mthumb -mthumb-interwork \
	   -Wl,--start-group -L$(THUMB2GNULIB) -L$(THUMB2GNULIB2) \
           -lc -lg -lstdc++ -lsupc++  -lgcc -lm  -Wl,--end-group \
	   -Xlinker -Map -Xlinker bin/lpc1700.map -Xlinker -T $(LDSCRIPT)

LDFLAGS+=-L$(CMSIS)/lib -lDriversLPC17xxgnu

EXECNAME	= bin/proj

FILTERS = filters/test_filter.o filters/gain_filter.o filters/parallel_filter.o filters/filter_table.o filters/reverb_filter.o filters/echo_filter.o filters/ngate_filter.o filters/flanging_filter.o filters/tremolo_filter.o filters/lp_filter.o filters/bp_filter.o filters/hp_filter.o

OBJ		= main.o tty_mbed.o dbg.o queue.o ui_serial.o fir.o

SDOBJ = chanfs/diskio.o chanfs/ff.o chanfs/sd_mbed.o sd.o

all: main
	@echo "Build finished."

main: filters $(OBJ) sd
	$(CC) -o $(EXECNAME) $(OBJ) $(FILTERS) $(SDOBJ) $(LDFLAGS)
	$(OBJCOPY) -I elf32-little -O binary $(EXECNAME) $(EXECNAME).bin
	$(OBJCOPY) -I elf32-little -O ihex $(EXECNAME) bin/mp2.hex
#create filter symbol table and then build filters
filters: ftable $(FILTERS) 
	@echo "\033[96mNote:\033[0m Filters built."
ftable:	
	@python filters/build_table.py
sd: $(SDOBJ)
	@echo "\033[96mNote:\033[0m ChanFS built."
# clean out the source tree ready to re-build
clean:
	rm -f `find . | grep \~`
	rm -f *.swp *.o */*.o */*/*.o  *.log
	rm -f *.d */*.d *.srec */*.a bin/*.map
	rm -f *.elf *.wrn bin/*.bin log *.hex
	rm -f filters/*.o
	rm -f $(EXECNAME)
# install software to board, remember to sync the file systems
install:
	@echo "Copying " $(EXECNAME) "to the MBED file system."
	cp $(EXECNAME).bin /media/MBED 
	sync
	@echo "Now press the reset button."
