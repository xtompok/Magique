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
	/* Handle short polling (16 times per second). */
	if (evlist & EV_SHORT_POLL) {
		evlist &= ~EV_SHORT_POLL;
		/* TODO: Insert magique stuff here */
		if (flags & FL_GAME_LISTEN) {
			my_gi.listen_period--;
		}
		if (my_info.mana > 0) {
			struct packet p;
			network_mkpacket(&p);
			switch (my_info.id & 0xf) {
				case 1: /* Náčelník; TODO: +Počet členů */
					p.attack = 1+my_gi.last_teammates;
					p.defense = 1+my_gi.last_teammates;
					break;
				case 2: /* Strážce vlajky */
					p.attack = 5;
					p.defense = 0;
					break;
				case 3: /* Balvan */
					p.attack = 1;
					p.defense = 10;
					break;
				case 4: /* Samotář */
					p.attack = 1;
					p.defense = 1;
					if (my_gi.last_teammates == 0) {
						p.attack += 4;
						p.defense += 3;
					}
					break;
				default: /* Pěšák */
					p.attack = 1;
					p.defense = 1;
					break;
			}
			network_send(&p, 0);
		}
	}

	/* Handle long polling (once per second). */
	if (evlist & EV_LONG_POLL) {
		evlist &= ~EV_LONG_POLL;
		/* TODO: Expensive network communication can be done here. */
		/* Initialize game listening period */
		flags |= FL_GAME_LISTEN;
		my_gi.listen_period = 2;
		my_gi.defense = 0;
		my_gi.attack = 0;
	}
}