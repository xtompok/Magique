/******************************************************************************
 * Filename: sr.c
 *
 * Created: 2013/02/02 21:01
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include "sr.h"

volatile unsigned int _sr_conf = 0xffff;

void sr_out(unsigned int data) {
	/* Clear old data */
	SR_OUT &= ~SR_CLEAR;
	SR_OUT ^= SR_CLEAR;
	SR_OUT &= ~SR_LATCH;

	/* Write 2 bytes, MSB first */
	for (int i = 15; i >= 0; i--)  {
		if (data & (1 << i))
			SR_OUT |= SR_DATA;
		else 
			SR_OUT &= ~SR_DATA;
		/* Do clock */
		SR_OUT |= SR_CLK;
		SR_OUT ^= SR_CLK;
	}
	/* Latch data out */
	SR_OUT &= ~SR_DATA;
	SR_OUT |= SR_LATCH;
	SR_OUT &= ~SR_LATCH;
}

void sr_update() {
	sr_out(_sr_conf);
}

void sr_led(unsigned int led, unsigned char on) {
	/* Leds are ON when LOW */
	if (on)
		_sr_conf &= ~led;
	else
		_sr_conf |= led;
}
