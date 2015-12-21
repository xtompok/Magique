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
	//WDTCTL = WDTPW + WDTHOLD;
	flags |= FL_BEEP;
	TACCR0 = freq/8;
	delay_ms(length);
	flags ^= FL_BEEP;
	SPK_OUT &= ~SPK;
	//WDTCTL = WDTPW + WDTTMSEL + WDTIS0; /* SMCLK/8192 */
	//WDTCTL = WDTPW + WDTTMSEL; /* SMCLK/32768 */
	delay_ms(pause);
}

