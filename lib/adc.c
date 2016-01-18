/******************************************************************************
 * Filename: adc.c
 *
 * Created: 2013/02/06 23:24
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>

#include "delay.h"
#include "adc.h"

void adc_init() {
	/* ADC Mode: Vref to GND, 64 clock sample, 2.5V internal reference */
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REF2_5V + ADC10ON;
	ADC10CTL1 = ADC10DIV_7 + ADC10SSEL_3;
}

unsigned int adc_read(unsigned int channel) {
	ADC10CTL0 |= REFON;
	ADC10CTL1 &= 0xfff;
	ADC10CTL1 |= channel << 12;
	delay_us(30); /* TODO: is it needed? */
	ADC10CTL0 |= ENC + ADC10SC;

	while (!(ADC10CTL0 & ADC10IFG));
	ADC10CTL0 &= ~(ADC10IFG + ENC);
	ADC10CTL0 &= ~REFON;
	return ADC10MEM;
}
