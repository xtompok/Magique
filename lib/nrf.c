/******************************************************************************
 * Filename: nrf.c
 *
 * Created: 2013/02/02 19:33
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include "spi.h"
#include "hardware.h"
#include "delay.h"
#include "nrf.h"

/* NRF command words */
#define NRF_CMD_R_REGISTER 0x00
#define NRF_CMD_W_REGISTER 0x20
#define NRF_CMD_R_RX_PAYLOAD 0x61
#define NRF_CMD_W_TX_PAYLOAD 0xA0
#define NRF_CMD_FLUSH_TX 0xE1
#define NRF_CMD_FLUXH_RX 0xE2

#define NRF_CMD_NOP 0xff


unsigned char _nrf_status = 0xAA;

void nrf_csh() { NRF_POUT |= NRF_CS; }
void nrf_csl() { NRF_POUT &= ~NRF_CS; }
void nrf_ceh() { NRF_POUT |= NRF_CE; }
void nrf_cel() { NRF_POUT &= ~NRF_CE; }

unsigned int nrf_reg_read(unsigned char reg, unsigned char size) {
	nrf_csl();
	/* Send command byte, status register is always returned. */
	_nrf_status = spi_xfer_byte(NRF_CMD_R_REGISTER | (reg & 0x1f));

	/* Read result */
	unsigned int out = spi_xfer_byte(NRF_CMD_NOP);
	if (size > 1) out |= (spi_xfer_byte(NRF_CMD_NOP) & 0xff) << 8;
	nrf_csh();
	return out;
}

void nrf_reg_write(unsigned char reg, unsigned int data, unsigned char size) {
	nrf_csl();
	_nrf_status = spi_xfer_byte(NRF_CMD_W_REGISTER | (reg & 0x1f));
	spi_xfer_byte(data & 0xff);
	if (size > 1) spi_xfer_byte( (data >> 8) & 0xff );
	nrf_csh();
}

void nrf_transmit(unsigned char *data, unsigned char len) {
	/* Clear transmit buffer. */
	nrf_csl();
	_nrf_status = spi_xfer_byte(NRF_CMD_FLUSH_TX);
	nrf_csh();

	/* Send new data. */
	nrf_csl();
	_nrf_status = spi_xfer_byte(NRF_CMD_W_TX_PAYLOAD);
	for (int i = 0; i < len; i++) spi_xfer_byte(data[i]);
	nrf_csh();

	/* Transmit. */
	nrf_ceh();
	delay_us(10*len);
	nrf_cel();
}

void nrf_listen() {
	nrf_ceh();
}

void nrf_nolisten() {
	nrf_cel();
}

int nrf_receive(unsigned char *data, unsigned char len) {
	/* First check if data is ready */
	nrf_csl();
	_nrf_status = spi_xfer_byte(NRF_CMD_NOP);
	nrf_csh();

	if (!(_nrf_status & RX_DR)) return 0;
	/* TODO: Clear bit? */

	/* Read data */
	nrf_csl();
	_nrf_status = spi_xfer_byte(NRF_CMD_R_RX_PAYLOAD);
	for (int i = 0; i < len; i++) {
		data[i] = spi_xfer_byte(NRF_CMD_NOP);
	}

	/* Clear RX_DR */
	nrf_reg_write(NRF_REG_STATUS, _nrf_status & (~RX_DR), 1);

	/* I may want to return pipe number in the future */
	return 1;
}

void nrf_init() {
	delay_ms(20); /* 10.3ms should be enough */
	NRF_PDIR |= NRF_CS + NRF_CE;
	NRF_POUT &= NRF_CE;
	nrf_csh();
	nrf_powerup();
}

void nrf_powerup() {
	int config = nrf_reg_read(NRF_REG_CONFIG, 1);
	nrf_reg_write(NRF_REG_CONFIG, config | PWR_UP | CRCO, 1);
	delay_ms(2); /* 1.5ms should be enough */
}

void nrf_powerup_nodelay() {
	int config = nrf_reg_read(NRF_REG_CONFIG, 1);
	nrf_reg_write(NRF_REG_CONFIG, config | PWR_UP | CRCO, 1);
}

void nrf_powerdown() {
	nrf_reg_write(NRF_REG_CONFIG, 0, 1);
}

void _nrf_setrx(unsigned char prx) {
	unsigned char config = nrf_reg_read(NRF_REG_CONFIG, 1);
	if (prx) config |= PRIM_RX;
	else config &= ~PRIM_RX;
	nrf_reg_write(NRF_REG_CONFIG, config, 1);
}
