/******************************************************************************
 * Filename: usbif.c
 *
 * Created: 2012/12/14 23:29
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include "uart.h"
#include "delay.h"
#include "spi.h"
#include "nrf.h"
#include "adc.h"
#include "network.h"
#include "magique.h"


struct node my_info;

int main() {
	/* Setup clock and disable watchdog */
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
	BCSCTL3 = LFXT1S1; /* Activate VLOCLK, approx. 12kHz */
	WDTCTL = WDTPW + WDTTMSEL + WDTIS0 + WDTIS1 + WDTSSEL; /* VLOCLK/64; a tick every ~5ms */
	IE1 |= WDTIE;

	/* Setup output */
	P2DIR |= BIT3;
	/* Setup input */
	P2DIR &= ~(BIT4 | BIT5);

	delay_ms(300);

	/* Setup UART */
	_BIS_SR(GIE);
	uart_init();

	uart_puts("Initializing Sensor BaseStation: ");
	uart_puts("spi ");
	spi_init();

	uart_puts("nrf \r\n");
	nrf_init();
	network_init( RF_ROLE_RX );
	nrf_setrx();
	nrf_cmd_flush_rx();
	nrf_listen();
	uint8_t mode = 'm', newmode = 0;
	uint8_t mpm = 5, mode_adv = 0;
	while (1) {
		if (uart_hasc())
			newmode = uart_getc();
		switch (mode) {
			case 'l':
				network_arcv_start();
				if (nrf_receive((unsigned char *) &pk_in, sizeof(struct packet))) {
					/* Output blob */
					uart_puts("\rPacketBlob:");
					for (int i = 0; i < sizeof(struct packet); i++) {
						uart_putix(((char *) &pk_in)[i], 2);
						uart_putc(' ');
					}
					uart_puts("\r\n");
				}
				delay_ms(100);
				break;
			case 'm':
				if (newmode) {
					if ((newmode > '0') && (newmode <= '9')) {
						mpm = newmode - '0';
						newmode = 0;
					}
				}
				nrf_nolisten();
				nrf_settx();
				network_mkpacket(&pk_out);
				pk_out.mpm = mpm;
				network_send(&pk_out, 0);
				uart_puts("m(");
				uart_putix(mpm, 2);
				uart_puts(")");
				delay_ms(100);
				break;
			case 's':
				if (newmode) {
					if ((newmode > '0') && (newmode <= '9')) {
						mode_adv = newmode - '0';
						newmode = 0;
					}
				}
				nrf_nolisten();
				nrf_settx();
				network_mkpacket(&pk_out);
				pk_out.mode_adv = mode_adv;
				network_send(&pk_out, 0);
				uart_puts("s(");
				uart_putix(mode_adv, 2);
				uart_puts(")");
				delay_ms(100);
			default:;
		}
		if (newmode) {
			uart_putc(mode);
			uart_puts(" -> ");
			uart_putc(newmode);
			uart_puts("\r\n");
			mode = newmode;
			newmode = 0;
		}
	}
}

void __attribute__((interrupt(WDT_VECTOR))) WDT_ISR(void) {

}
