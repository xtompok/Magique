/******************************************************************************
 * Filename: ktgame.c
 *
 * Created: 2016/01/24 23:33
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include "magique.h"
#include "globals.h"
#include "network.h"

struct game_info {
	uint8_t listen_period;
	uint8_t seen[8];
	uint16_t attack;
	uint16_t defense;
	uint8_t teammates;
	uint8_t last_teammates;
};

struct game_info my_gi;

void ktgame_process(void) {
	my_info.mana = 100;

	/* Handle short polling (16 times per second). */
	if (evlist & EV_SHORT_POLL) {
		evlist &= ~EV_SHORT_POLL;
		/* TODO: Insert magique stuff here */

		if (my_info.mana > 0) {

			network_mkpacket(&pk_out);
			switch (my_info.id & 0xf) {
				case 1: /* Náčelník; TODO: +Počet členů */
					pk_out.attack = 1+my_gi.last_teammates;
					pk_out.defense = 1+my_gi.last_teammates;
					break;
				case 2: /* Strážce vlajky */
					pk_out.attack = 5;
					pk_out.defense = 0;
					break;
				case 3: /* Balvan */
					pk_out.attack = 1;
					pk_out.defense = 10;
					break;
				case 4: /* Samotář */
					pk_out.attack = 1;
					pk_out.defense = 1;
					if (my_gi.last_teammates == 0) {
						pk_out.attack += 4;
						pk_out.defense += 3;
					}
					break;
				default: /* Pěšák */
					pk_out.attack = 1;
					pk_out.defense = 1;
					break;
			}
			network_send(&pk_out, 0);
		}
	}

	/* Handle long polling (once per second). */
	if (evlist & EV_LONG_POLL) {
		evlist &= ~EV_LONG_POLL;
		/* TODO: Expensive network communication can be done here. */
		/* Initialize game listening period */
		flags |= FL_GAME_LISTEN;
		my_gi.listen_period = 50;
		my_gi.defense = 0;
		my_gi.attack = 0;
		for (uint8_t i = 0; i < 8; i++) {
			my_gi.seen[i] = 0;
		}
	}

	if (flags & FL_GAME_LISTEN) {
		my_gi.listen_period--;
		while (network_rcv(&pk_in, 30)) {
			uint8_t _bop_index = pk_in.node_from & 0x7;
			uint8_t _bop_bit = 1 << ((pk_in.node_from & 0xf8) >> 3);
			if (!(my_gi.seen[_bop_index] & _bop_bit)) {
				my_gi.seen[_bop_index] |= _bop_bit;
				uint8_t team = pk_in.node_from & 0x20;
				uint8_t my_team = my_info.id & 0x20;
				if (team == my_team) {
					my_gi.defense += pk_in.defense;
					my_gi.teammates++;
				} else {
					my_gi.attack += pk_in.attack;
				}
			}
		}

		if (my_gi.listen_period == 0) {
			flags &= ~FL_GAME_LISTEN;
			/* TODO: Turn off radio */

			/* TODO: Evaluate round */
			if (my_gi.attack > my_gi.defense) {
				uint16_t damage = my_gi.attack - my_gi.defense;
				if (my_info.mana > damage) my_info.mana -= damage;
				else my_info.mana = 0;

			}
		}
	}
}
