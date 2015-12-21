/******************************************************************************
 * Filename: hardware.h
 *
 * Created: 2013/02/02 21:06
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include <msp430.h>

/* SMCLK clock speed. */
#define CLOCK 8000000

/* Shift register ports */
#define SR_OUT P2OUT
#define SR_DATA BIT7
#define SR_CLEAR BIT3
#define SR_CLK BIT4
#define SR_LATCH BIT5

/* Shift register peripherals */
#define SR_O_GREEN (1 << 13)
#define SR_O_YELLOW (1 << 14)
#define SR_O_RED (1 << 15)

/* Segments in registers */
#define A (1 << 0)
#define B (1 << 6)
#define _C (1 << 5)
#define D (1 << 2)
#define E (1 << 3)
#define F (1 << 7)
#define G (1 << 1)
#define DOT (1 << 4)
/* This is the position of first bit of two that drives a digit */
#define SMPLEX_START 11
#define SMPLEX_MASK (A | B | _C | D | E | F | G | DOT | (0x03 << SMPLEX_START))
#define SMPLEX_DIGIT_MASK (A | B | _C | D | E | F | G | DOT)

/* Speaker */
#define SPK_OUT P2OUT
#define SPK BIT6

/* NRF ports */
#define NRF_POUT P2OUT
#define NRF_PDIR P2DIR
#define NRF_CE BIT0
#define NRF_CS BIT1
/* IRQ not used */

/* Soft SPI (mimics USCI) */
#define SPI_POUT P1OUT
#define SPI_PIN P1IN
#define SPI_PDIR P1DIR
#define SPI_CLK BIT5
#define SPI_MISO BIT6
#define SPI_MOSI BIT7

/* Button */
#define BUTTON_PIN BIT2 

#endif
