/******************************************************************************
 * Filename: spi.h
 *
 * Created: 2013/02/03 19:09
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#ifndef __SPI_H__
#define __SPI_H__

void spi_init();
#ifdef __MSP430_HAS_USCI__
void spi_mode(unsigned char mode);
void spi_speed(unsigned int speed);
#endif

unsigned char spi_xfer_byte(unsigned char data);

#endif
