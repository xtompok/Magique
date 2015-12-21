/******************************************************************************
 * Filename: spi.c
 *
 * Created: 2013/02/03 18:31
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include "spi.h"
#include "hardware.h"

void spi_init() {
#ifdef __MSP430_HAS_USCI__
	UCB0CTL1 = UCSWRST;

  	P1SEL  |= BIT5 + BIT6 + BIT7;
  	P1SEL2 |= BIT5 + BIT6 + BIT7;

    // (3) 3-pin, 8-bit SPI master
    UCB0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC; 
	UCB0CTL1 |= UCSSEL_2; /* Set clock to SMCLK */

	UCB0CTL1 &= ~UCSWRST; 
#else
	/* Set MISO as input, CLK and MOSI as output */
	SPI_PDIR &= ~SPI_MISO;
	SPI_PDIR |= SPI_CLK + SPI_MOSI;
/*
	USICTL0 = USISWRST;
	
  	//P1SEL |= BIT5 + BIT6 + BIT7;
	//P2SEL &= ~(BIT5 + BIT6 + BIT7);

	//Enable pins, master mode, SMCLK clock
	USICTL0 |= USIPE5 + USIPE6 + USIPE7 + USIMST + USIOE;
	USICTL1 = USICKPH;
	USICKCTL = USISSEL_2;

	USICTL0 &= ~USISWRST;
*/
#endif
}

#ifdef __MSP430_HAS_USCI__
void spi_mode(unsigned char mode) {
	UCB0CTL1 |= UCSWRST;
    UCB0CTL0 &= ~(UCCKPH + UCCKPL);
	UCB0CTL0 |= mode & (UCCKPH + UCCKPL);
	UCB0CTL1 &= ~UCSWRST; 
}

void spi_speed(unsigned int speed) {
	UCB0CTL1 |= UCSWRST;
	UCB0BR0 = speed & 0xff;
	UCB0BR1 = (speed >> 8) & 0xff;
	UCB0CTL1 &= ~UCSWRST; 
}
#endif

unsigned char spi_xfer_byte(unsigned char data) {
#ifdef __MSP430_HAS_USCI__
  	UCB0TXBUF = data; 

	while (!(IFG2 & UCB0TXIFG));
	while (!(IFG2 & UCB0RXIFG));

	return UCB0RXBUF;
#else 
	unsigned char rcv = 0;
	SPI_POUT &= ~SPI_CLK;
	/* Shift out, MSB first */
	for (int i = 7; i >= 0; i--)  {
		if (data & (1 << i))
			SPI_POUT |= SPI_MOSI;
		else 
			SPI_POUT &= ~SPI_MOSI;
		/* Do clock */
		SPI_POUT |= SPI_CLK;
		__delay_cycles(5);
		/* Sample input */
		if (SPI_PIN & SPI_MISO)
			rcv |= (1 << i);
		__delay_cycles(5);
		SPI_POUT ^= SPI_CLK;
		__delay_cycles(5);
	}
	SPI_POUT &= ~SPI_CLK;
	return rcv;
/* USCI/USI swaps MISO and MOSI, can't be used... 
	USISRL = data;
	USICNT = 8;

	while (!(USIIFG & USICTL1));

	return USISRL;
*/
#endif
}
