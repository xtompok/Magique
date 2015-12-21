/******************************************************************************
 * Filename: sr.h
 *
 * Created: 2013/02/02 21:01
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#ifndef __SR_H__
#define __SR_H__

#include <msp430.h>
#include "hardware.h"

void sr_update();
void sr_led(unsigned int led, unsigned char on);

/* Shift register configuration:
 *   0:2  unused
 *   3:5  LED's
 *   7:8  LCD multiplex transistors
 *   9:15 LCD segments
 */
extern volatile unsigned int _sr_conf;

#endif
