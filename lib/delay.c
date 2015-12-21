/******************************************************************************
 * Filename: delay.c
 *
 * Created: 2013/02/02 21:12
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include "delay.h"

void delay_ms(unsigned int ms) {
	while (ms--) {
		__delay_cycles(CLOCK/1000);
	}
}

void delay_us(unsigned int us) {
	while (us--)
		__delay_cycles(CLOCK/1000000);
}
