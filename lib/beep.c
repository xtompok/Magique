/******************************************************************************
 * Filename: beep.c
 *
 * Created: 2013/02/18 23:33
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include "beep.h"
#include "hardware.h"
#include "globals.h"
#include "delay.h"

void beep(int freq, int length, int pause) {
	/* Setup SPK as output, enable TACTL */
	SPK_DIR |= SPK;
	TACTL |= MC0;

	/* Start beeping */
	flags |= FL_BEEP;
	TACCR0 = freq/8;
	delay_ms(length);
	flags ^= FL_BEEP;

	/* Quit beeping, change SPK back to input and disable counter */
	SPK_OUT &= ~SPK;
	SPK_DIR &= ~SPK;
	TACTL &= ~MC0;

	delay_ms(pause);
}

