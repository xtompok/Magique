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
#include "uart.h"
#include "delay.h"
#include "spi.h"
#include "nrf.h"
#include "adc.h"
#include "network.h"
#include "magique.h"
#include "modes/magique_source.h"

static volatile unsigned int _jiffies;
static volatile unsigned int _wakeup;
static volatile unsigned int _timeout;
static void (*_timeout_fn)(void);

struct node my_info;

void sleep_ms(unsigned int ms) {
	if (ms < 5) ms = 5;
	_wakeup = _jiffies + ms/5;
	LPM3;
}

void timeout_ms(unsigned int ms, void (*callback)(void)) {
	if (ms < 5) ms = 5;
	_timeout = _jiffies - 1;
	_timeout_fn = callback;
	_timeout = _jiffies + ms/5;
}

int main() {
	/* Setup clock and disable watchdog */
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
	BCSCTL3 = LFXT1S1; /* Activate VLOCLK, approx. 12kHz */
	WDTCTL = WDTPW + WDTTMSEL + WDTIS0 + WDTIS1 + WDTSSEL; /* VLOCLK/64; a tick every ~5ms */
	IE1 |= WDTIE;

	/* Setup output */
	P2DIR |= BIT3;
	//P2REN |= BIT3;
	//P2OUT &= ~BIT3;
	/* Setup input */
	P2DIR &= ~(BIT4 | BIT5);

	/* Setup timer: SMCLK/1, Up mode */
	//TACTL = TASSEL_2 | ID_3 | MC_1;
	//TACCR0 = 10000; /* 1s = 1 000 000 ticks, 1ms=1 000, 20 ms = 20 000 */
	//CCTL0 = CCIE;


	delay_ms(300);

	/* Setup UART */
	_BIS_SR(GIE);
	uart_init();

	uart_puts("Initializing Sensor BaseStation: ");


	uart_puts("spi ");
	spi_init();
	uart_puts("nrf \r\n");
	/* Setup the NRF24L01 */

	nrf_init();
	nrf_powerup();
	nrf_reg_write(NRF_REG_RF_SETUP, RF_SETUP, 1);

	uart_puts("NRF status register: ");
	uart_putib(_nrf_status, 8);
	uart_putc('\r');
	uart_putc('\n');
	uart_puts("NRF config register: ");
	uart_putib(nrf_reg_read(NRF_REG_CONFIG, 1), 8);
	uart_putc('\r');
	uart_putc('\n');

	uart_puts("Entering control loop.\r\n");

	nrf_reg_write(NRF_REG_RF_CH, RF_CH, 1);
	nrf_reg_write(NRF_REG_RX_PW_P0, sizeof(struct packet), 1);
	nrf_setrx();
	nrf_listen();
	while (1) {
		magique_source_process();
	}
}


interrupt(WDT_VECTOR) WDT_ISR(void) {
	_jiffies++;
	//if (!(_jiffies & 0xfff)) LPM3_EXIT;
	if (_jiffies == _wakeup) LPM3_EXIT;
	if (_jiffies == _timeout) _timeout_fn();
//	uart_putix(_jiffies, 4);
//		uart_putc('\r');
//		uart_putc('\n');
}
