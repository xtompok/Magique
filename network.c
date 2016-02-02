/******************************************************************************
 * Filename: network.c
 *
 * Created: 2016/01/18 10:07
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include "network.h"
#include "lib/nrf.h"
#include "lib/delay.h"

void network_init(uint8_t role) {
	nrf_powerup();
	// Setup the radio: default addresses, 0dBm tx power, 1Mbit
	nrf_reg_write(NRF_REG_CONFIG, EN_CRC | PWR_UP | (role & PRIM_RX) | CRCO, 1);
	nrf_reg_write(NRF_REG_RF_CH, RF_CH, 1);
	nrf_reg_write(NRF_REG_RX_PW_P0, sizeof(struct packet), 1);
	nrf_reg_write(NRF_REG_RF_SETUP, RF_SETUP, 1);
}

void network_mkpacket(struct packet *p) {

}

/* Attempt to send a packet. Waits for ACK if req_ack is non-zero
 *
 * Arguments:
 * 		*p			packet to send
 * 		req_ack		nonzero if ACK is required
 *
 * Return value:
 * 		0 			failure
 * 		1			success
 * */
uint8_t network_send(struct packet *p, uint8_t req_ack) {
	/* TODO: Wake radio from sleep mode */
	nrf_settx();
	nrf_reg_write(NRF_REG_STATUS, MAX_RT | TX_DS, 1);
	nrf_transmit((unsigned char *) &p, sizeof(struct packet));
	uint8_t status = 0;
	if (req_ack) {
		/* TODO: timeout should never be reach, it's just to be on the safe side */
		for (uint8_t timeout = 16; timeout > 0; timeout--) {
			status = nrf_reg_read(NRF_REG_STATUS, 1) & (MAX_RT | TX_DS | TX_FULL);
			if (status) break;
			delay_us(250); /* 250us is the standard retransmit timeout */
		}
		if (status == 0) status = BIT7; /* Bit 7 is reserved; this indicates a timeout */
		if (status & TX_DS) {			/* TX_DS means success, in this case, we need to return 0; */
			status = 0;
		}
		nrf_reg_write(NRF_REG_STATUS, MAX_RT | TX_DS, 1);
	}
	return !status;
}

/* Attempts to receive packet into preallocated buffer.
 *
 * If timeout is 0, radio is not turned on, and only already received packet is
 * read.
 *
 * Arguments:
 * 		*p 			pointer to 32B of space for packet data
 * 		timeout		how long should we listen (in ms)
 *
 * Return value:
 * 		0 			failure
 * 		1			success
 */
uint8_t network_rcv(struct packet *p, uint16_t timeout) {
	nrf_setrx();
	uint8_t result = 0;
	while (timeout && !(result = nrf_receive((unsigned char *) p, sizeof(struct packet)))) {
		nrf_listen();
		delay_ms(1);
		timeout--;
	}
	nrf_nolisten();
	return result; /* Result is 1 only if the last nrf_receive succeeded */
}
