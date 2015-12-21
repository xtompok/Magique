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
#include "uart.h"
#include "spi.h"
#include "radiation.h"

int main() {
	/* Setup clock and disable watchdog */
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	WDTCTL = WDTPW + WDTHOLD;

	/* Setup IO */
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;

	/* Setup UART */
	uart_init();
	
	/* Initialize command shell */
	while (uart_getc() != '\r');
	uart_puts("\n\rMSP-NRF bridge ready. Press H for help.\r\n");
	unsigned char zone = 1;
	while (1) {
		unsigned char ic = uart_getc();
		switch (ic) {
			case 'H':
			case 'h':
				uart_puts("Commands: \r\n");
				uart_puts(" H h   this help\r\n");
				uart_puts(" I     module initialization\r\n");
				uart_puts(" rXX  read register XX (hex)\r\n");
				break;
			case 'I':
				uart_puts("Initializing NRF module: spi,");
				spi_init();
				/* Can't go much slower, the NRF will not receive correctly. */
				//spi_speed(0x20); 
				uart_puts("nrf,");
				nrf_init();
				uart_puts("ok.\r\n STATUS=");
				unsigned int config = nrf_reg_read(0,1);
				uart_putib(_nrf_status, 8);
				uart_puts("\r\n CONFIG=");
				uart_putib(config, 8);

				/* Prepare to send */
				nrf_reg_write(NRF_REG_EN_AA, 0, 1); /* Disable Auto Acknowledgment */
				nrf_reg_write(NRF_REG_RF_CH, RF_CH, 1);
				nrf_reg_write(NRF_REG_RX_PW_P0, MSG_LENGTH, 1);
				nrf_reg_write(NRF_REG_RF_SETUP, RF_DR | LNA_HCURR, 1);

				uart_puts("\r\n PW_P0=");
				uart_putib(nrf_reg_read(NRF_REG_RX_PW_P0, 1), 8);
				uart_puts("\r\n");
				break;
			case 'T':
				uart_puts("Entering test transmit mode. Press any key to terminate.\r\n");
				unsigned char data = 0;
				while (1) {
					message[MSG_MAGIC] = MSG_MAGIC_VAL;
					message[MSG_RAD] = data;
					//for (int i = 0; i < MSG_LENGTH; i++) 
					//	message[i] = 2*i;
					for (int i = 0; i < 100; i ++) {
						msg_beacon();
						delay_ms(5);
					}
					data++;
					uart_putix(data,2);
					uart_putc('\r');
					if (uart_hasc()) break;
				}
				break;
			case 'l':
				uart_puts("Initializing log mode. Press any key to terminate.\r\n");
				nrf_setrx();
				while (1) {
					nrf_listen();
					delay_ms(1);
					nrf_nolisten();
					if (msg_receive() && (_rcvbuf[MSG_FROM] > 0)) {
						uart_puts("Got beacon from ");
						uart_putix(_rcvbuf[MSG_FROM], 3);
						uart_puts(", dosage is ");
						uart_putix(_rcvbuf[MSG_COUNTER1], 2);
						uart_putix(_rcvbuf[MSG_COUNTER2], 2);
						uart_putix(_rcvbuf[MSG_COUNTER3], 2);
						uart_putix(_rcvbuf[MSG_COUNTER4], 2);
						uart_puts(", battery is at ");
						uart_putix(_rcvbuf[MSG_BAT], 2);
						uart_puts("\r\n");
					}
					delay_ms(10);
					if (uart_hasc()) break;
				}
				nrf_settx();
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				zone = ic - '0';
				if (zone > 9) zone = zone + '0' - 'a' + 10;
				uart_puts("Setting zone to ");
				uart_putix(zone, 1);
				uart_puts("\r\n");
				break;
			case 'z':
				for (int i = 0; i < MSG_LENGTH; i++) 
					message[i] = 0;
				uart_puts("Entering zone mode. Transmitting zone ");
				uart_putix(zone, 1);
				uart_puts(". Press any key to terminate.\r\n");
				message[MSG_RAD] = zone;
				while(1) {
					msg_beacon();
					delay_ms(1);
					if (uart_hasc()) break;
				}
				break;
			case 's':;
				unsigned char m = uart_getc();
				if ((m != '0') && (m != '1'))
					break;
				for (int i = 0; i < MSG_LENGTH; i++) 
					message[i] = 0;
				message[MSG_RAD] = 0;
				message[MSG_COMMAND] = CMD_COUNTER_SELECT;
				message[MSG_VALUE1] = (m == '0') ? 0 : 1;
				uart_puts("Sending message...");
				for (int i = 0; i < 200; i ++) {
					msg_beacon();
					delay_ms(1);
				}
				uart_puts("done.\r\n");
				break;
			case 'r':;
				for (int i = 0; i < MSG_LENGTH; i++) 
					message[i] = 0;
				message[MSG_RAD] = 0;
				message[MSG_COMMAND] = CMD_COUNTER_SET;
				message[MSG_COUNTER4] = 1;
				message[MSG_VALUE1] = 1;
				uart_puts("Sending message...");
				for (int i = 0; i < 100; i ++) {
					msg_beacon();
					delay_ms(1);
				}
				uart_puts("done.\r\n");
				break;
			default:
				uart_puts("Unknown command, press H for help.\r\n");
		}
	}
}
