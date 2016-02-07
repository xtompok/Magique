
ifndef MODE
  MODE=0
endif

ifndef ID
  ID=0xbeef
endif

CC=msp430-gcc
CPU_MAGI=-mmcu=msp430g2352
CPU_BASE=-mmcu=msp430g2553
MYVAL:=0x11111111
MYNAME:=0x80

CFLAGS_COMMON=-std=c99 -Os -Wall -g -Ilib -I. -DHWDEBUG=1 -DMY_ID=$(ID) -DMY_MODE=$(MODE)

# Magique stone build
CFLAGS_MAGI=$(CFLAGS_COMMON) -mmcu=msp430g2352
OBJS_MAGI=lib/nrf.o lib/sr.o lib/delay.o lib/segment.o lib/spi.o lib/beep.o globals.o lib/adc.o network.o modes/ktgame.o modes/magique_source.o modes/magique_stone.o

# Base station build
CFLAGS_BASE=$(CFLAGS_COMMON) -mmcu=msp430g2553 -DHW_BASE=1
OBJS_BASE=lib/nrf.o lib/delay.o lib/spi.o globals.o lib/adc.o lib/uart.o network.o modes/magique_source.o

# Decide if we are building magique stone or base station
ifeq (1,$(BASE))
  CFLAGS=$(CFLAGS_BASE)
  OBJS=$(OBJS_BASE)
else
  CFLAGS=$(CFLAGS_MAGI)
  OBJS=$(OBJS_MAGI)
endif


magique.elf: $(OBJS) magique.o
	$(CC) $(CFLAGS) -o $@ magique.o $(OBJS)

emitter.elf: $(OBJS) emitter.o
	$(CC) $(CFLAGS) -o $@ emitter.o $(OBJS)

all: clean magique.elf

base:
	make clean
	make base.elf BASE=1

base-prog: base
	mspdebug rf2500 "prog base.elf"

base.elf: $(OBJS_BASE) base.o
	$(CC) $(CFLAGS_BASE) -o $@ base.o $(OBJS_BASE)

usbif.elf: $(OBJS) usbif.o lib/uart.o
	$(CC) $(CFLAGS) -o $@ usbif.o $(OBJS) lib/uart.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -fr *.elf $(OBJS) *.o

size:
	msp430-size *.o lib/*.o modes/*.o *.elf

prog: magique.elf
	mspdebug rf2500 "prog magique.elf"

prog_r: magique.elf
	msp430-objcopy -R infob -R infoc magique.elf magique.prog
	mspdebug rf2500 "prog magique.prog"

.PHONY: prog all size usbif magique.elf clean
