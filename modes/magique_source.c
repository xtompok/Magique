/******************************************************************************
 * Filename: magique_source.c
 *
 * Created: 2016/02/03 16:24
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include "magique.h"
#include "globals.h"
#include "network.h"
#include "delay.h"

#ifdef HW_BASE
#include "uart.h"
void print_blob(struct packet *pk_out) {
	/* Output blob */
	uart_puts("\rPacketBlob:");
	for (int i = 0; i < sizeof(struct packet); i++) {
		uart_putix(((char *) pk_out)[i], 2);
		uart_putc(' ');
	}
	uart_puts(".\r\n");
}
#endif

uint8_t _si = 0;

void magique_source_process(void) {
	if (_si == 0) {
		network_init( RF_ROLE_RX );
		network_arcv_start();
//		nrf_listen();
		_si++;
		_digits=0x22;
	}


	if (network_arcv(&pk_out)) {
		//network_mkpacket(&r);
		pk_in.mpm = 0xff;
		pk_in.node_to = pk_out.node_from;
		nrf_nolisten();
		network_send(&pk_in, 1);
		evlist |= EV_GREEN_BLINK;
		_digits++;
		delay_ms(3);
		network_arcv_start();
#ifdef HW_BASE
		print_blob(&pk_out);
#endif
	}

	if (flags & FL_DISPLAY) {
		//_digits = 0xff;
	}

}
