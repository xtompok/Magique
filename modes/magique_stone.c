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

uint8_t mpm;
uint8_t ess;
uint8_t listen_period;

void magique_stone_process(void) {
	if (evlist & EV_LONG_POLL) {
		/* Blink LEDs if essences are offered */
		if (ess)
			evlist |= ess & 0x7;
		/* Start listening for mana sources */
		network_arcv_start();
		ess = mpm = 0;
		listen_period = 30;
		flags |= FL_STONE_LISTEN;

	}

	if (flags & FL_STONE_LISTEN) {
		/* Attempt receiving packet */
		while (network_arcv(&pk_in)) {
			/* Remember only last mpm */
			if (mpm < pk_in.mpm)
				mpm = pk_in.mpm;

			/* Remember offered essences*/
			ess |= pk_in.ess;

			/* Handle mode switch here */
			if (pk_in.mode_adv)
				my_info.mode_adv = pk_in.mode_adv;

			/* Blink green, as we had received mana */
			if (mpm)
				evlist |= EV_GREEN_BLINK;
		}

		listen_period--;
		/* If period is over, evaluate mana increase */
		if (listen_period == 0) {
			flags &= ~FL_STONE_LISTEN;

			my_info.mana += mpm;
			network_arcv_stop();
		}
	}

	if (flags & FL_BUTTON) {
		/* TODO: If some essences were offered, take them. */

	}

	if (flags & FL_DISPLAY) {
		_digits = my_info.mana;
	}
}

