/******************************************************************************
 * Filename: adc.h
 *
 * Created: 2013/02/06 23:43
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#ifndef __ADC_H__
#define __ADC_H__

void adc_init();
unsigned int adc_read(unsigned int channel);
unsigned int adc_rand(unsigned int bits);
void adc_on(void);
void adc_off(void);

#endif
