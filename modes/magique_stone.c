/******************************************************************************
 * Filename: magique_stone.c
 *
 * Created: 2016/02/03 16:27
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include "magique.h"
#include "globals.h"
#include "network.h"

struct magique_info {
	uint8_t mpm;
	uint16_t mana;
};

struct magique_info my_mi = {0, 0};

struct packet p;

void magique_stone_process(void) {
	/* We request source statuses in the long poll */
	if (evlist & EV_LONG_POLL) {
		network_init( RF_ROLE_TX );
		network_mkpacket(&p);
		if (network_send(&p, 1)) {
			if (network_rcv(&p, 50)) {
				my_mi.mpm = p.mpm;
				evlist |= EV_GREEN_BLINK;
			}
			evlist |= EV_RED_BLINK;
		}

		my_mi.mana += my_mi.mpm;
	}

	if (flags & FL_DISPLAY) {
		//_digits = my_mi.mpm;
		_digits = (my_mi.mana >> 8);
	}
}

