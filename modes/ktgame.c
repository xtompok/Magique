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
	uint8_t mpm;
};

struct game_info my_gi;

uint8_t my_attack(void) {
	switch (my_info.id & 0xf) {
		case 1: /* Nacelnik TODO: +Pocet spoluhracu */
			return 1 + my_gi.last_teammates;
		case 2: /* Strazce vlajky */
			return 5;
		case 3: /* Balvan */
			return 1;
		case 4: /* Samotar */
			if (my_gi.last_teammates == 0) {
				return 5;
			} else {
				return 1;
			}
		default: /* Pesak */
			return 1;
	}
}

uint8_t my_defence(void) {
	switch (my_info.id & 0xf) {
		case 1: /* Nacelnik TODO: +Pocet spoluhracu */
			return 1 + my_gi.last_teammates;
		case 2: /* Strazce vlajky */
			return 0;
		case 3: /* Balvan */
			return 10;
		case 4: /* Samotar */
			if (my_gi.last_teammates == 0) {
				return 5;
			} else {
				return 1;
			}
		default: /* Pesak */
			return 1;
	}
}

void initialize_my_gi(void) {
	my_gi.listen_period = 100;
	my_gi.defense = my_defence();
	my_gi.attack = 0;
	my_gi.last_teammates = my_gi.teammates;
	my_gi.teammates = 0;
	my_gi.mpm = 0;
	for (uint8_t i = 0; i < 8; i++) {
		my_gi.seen[i] = 0;
	}
}

void ktgame_process(void) {
	/* Handle short polling (16 times per second). */
	if (evlist & EV_SHORT_POLL) {
		evlist &= ~EV_SHORT_POLL;
		/* TODO: Insert magique stuff here */

		if (my_info.mana > 0) {
			evlist |= EV_YELLOW_BLINK;
			network_mkpacket(&pk_out);
			pk_out.attack = my_attack();
			pk_out.defense = my_defence();
			network_send(&pk_out, 0);
			if (flags & FL_GAME_LISTEN)
				network_arcv_start();
		}
	}

	/* Handle long polling (once per second). */
	if (evlist & EV_LONG_POLL) {
		evlist &= ~EV_LONG_POLL;
		/* TODO: Expensive network communication can be done here. */
		/* Initialize game listening period */
		flags |= FL_GAME_LISTEN;
		initialize_my_gi();
		network_arcv_start();
	}

	if (flags & FL_GAME_LISTEN) {
		while (network_arcv(&pk_in)) {
			evlist |= EV_GREEN_BLINK;
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
			if (pk_in.mpm > my_gi.mpm)
				my_gi.mpm = pk_in.mpm;
		}

		if (my_gi.listen_period == 0) {
			flags &= ~FL_GAME_LISTEN;
			/* TODO: Turn off radio */
			network_arcv_stop();

			/* TODO: Evaluate round */
			if (my_gi.attack > my_gi.defense) {
				uint16_t damage = my_gi.attack - my_gi.defense;
				if (my_info.mana > damage) my_info.mana -= damage;
				else my_info.mana = 0;
			}
			my_info.mana += my_gi.mpm;


			_digits = my_info.mana;
			//_digits = (my_gi.attack <<4) + my_gi.defense;
		} else {
			my_gi.listen_period--;
		}
	}
}
