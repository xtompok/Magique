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

void network_init() {
	/* Configure as receiver */
	nrf_reg_write(NRF_REG_CONFIG, EN_CRC | PWR_UP | PRIM_RX | CRCO, 1);
	nrf_reg_write(NRF_REG_EN_AA, 0, 1); /* Disable Auto Acknowledgment */
	nrf_reg_write(NRF_REG_RF_CH, RF_CH, 1);
	nrf_reg_write(NRF_REG_RX_PW_P0, sizeof(struct packet), 1);
	//nrf_reg_write(NRF_REG_RX_PW_P0, 1, 1);
	//nrf_reg_write(NRF_REG_EN_RXADDR, ERX_P0 | ERX_P1, 1);
}

