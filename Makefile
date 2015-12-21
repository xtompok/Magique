CC=msp430-gcc
#CPU=2553
CPU_MAGI=-mmcu=msp430g2352
CPU_USBIF=-mmcu=msp430g2553
MYVAL:=0x11111111
MYNAME:=0x80
CFLAGS=-std=c99 -Os -Wall -g $(CPU_MAGI) -Ilib -I. -DHWDEBUG=1 -DZONE=$(ZONE) -DMYNAME=$(MYNAME) -DMYVAL=$(MYVAL)
#CFLAGS=-std=c99 -Os -Wall -g $(CPU_MAGI) -Ilib -I. -DHWDEBUG=1
#CFLAGS=-std=c99 -Os -Wall -g $(CPU_USBIF) -Ilib -I. -DHWDEBUG=1 -DSWDEBUG=1 -DMYNAME=0 -D MYVAL=0

OBJS=lib/nrf.o lib/sr.o lib/delay.o lib/segment.o lib/spi.o lib/beep.o globals.o lib/adc.o radiation.o

magique.elf: $(OBJS) magique.o
	$(CC) $(CFLAGS) -o $@ magique.o $(OBJS)

emitter.elf: $(OBJS) emitter.o
	$(CC) $(CFLAGS) -o $@ emitter.o $(OBJS)

all: usbif.elf magique.elf

usbif.elf: $(OBJS) usbif.o lib/uart.o
	$(CC) $(CFLAGS) -o $@ usbif.o $(OBJS) lib/uart.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -fr *.elf $(OBJS) *.o

size:
	msp430-size *.elf

prog: magique.elf
	mspdebug rf2500 "prog magique.elf"

prog_r: magique.elf
	msp430-objcopy -R infob -R infoc magique.elf magique.prog
	mspdebug rf2500 "prog magique.prog"

.PHONY: prog all size usbif magique
