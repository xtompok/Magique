/******************************************************************************
 * Filename: usbif.c
 *
 * Created: 2012/12/14 23:29
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include <legacymsp430.h>
#include "delay.h"
#include "nrf.h"
#include "spi.h"
#include "radiation.h"
#include "network.h"

int main() {
	/* Setup clock and disable watchdog */
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
	WDTCTL = WDTPW + WDTHOLD;
	
	/* Setup SR ports */
	P2DIR |= SR_DATA | SR_CLEAR | SR_CLK | SR_LATCH;
	P2OUT &= ~(SR_DATA | SR_CLEAR | SR_CLK | SR_LATCH);
	
	/* Using XIN/XOUT as I/O */
	P2SEL &= ~(BIT6 + BIT7);

	/* Initialize command shell */
	spi_init();
	nrf_init();
	
	show_digit(ZONE, 0);
	sr_update(0);
	delay_ms(1000);
	show_digit(0, 2);
	sr_update(0);

	/* Prepare to send */
	nrf_reg_write(NRF_REG_EN_AA, 0, 1); /* Disable Auto Acknowledgment */
	nrf_reg_write(NRF_REG_RF_CH, RF_CH, 1);
	nrf_reg_write(NRF_REG_RX_PW_P0, MSG_LENGTH, 1);
	nrf_reg_write(NRF_REG_RF_SETUP, RF_PWR1 | RF_DR | LNA_HCURR, 1);
	message[MSG_RAD] = ZONE;
	while(1) {
		msg_beacon();
		delay_us(50);
	}
}
