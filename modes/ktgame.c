/******************************************************************************
 * Filename: ktgame.c
 *
 * Created: 2016/01/24 23:33
 * Authors: Ladislav Laska, Petr Petrous
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include "adc.h"
#include "hardware.h"
#include "ktgame.h"
#include "magique.h"
#include "globals.h"
#include "network.h"
#include "beep.h"

struct game_info {
	uint8_t display_mode;
	uint8_t listen_period;
	uint8_t seen[8];
	uint8_t special_attack;
	uint16_t attack;
	uint16_t defense;
	uint8_t teammates;
	uint8_t last_teammates;
	uint8_t mpm;
	uint8_t flag_holder;
	uint8_t flag_attender;
	uint8_t button_counter;
	uint16_t score;
};

struct game_info my_gi;

uint8_t my_attack(void) {
	switch (my_info.id & PLAYER_BITS) {
	case PLAYER_KING: /* Radoby kral */
		return (1 + my_gi.last_teammates) << 2;
	case PLAYER_KNIGHT: /* Rytir*/
		return 16;
	case PLAYER_FAT_KNIGHT: /* Tlusty rytir*/
		return 0;
	case PLAYER_POLITICAL_KNIGHT: /* Politicky rytir*/
		if (my_gi.last_teammates > 0) {
			return 0;
		} else {
			return 6;
		}
	default: /* Pesak */
		return 8;
	}
}

uint8_t my_defence(void) {
	switch (my_info.id & PLAYER_BITS) {
	case PLAYER_KING: /* Radoby kral */
		return 1 + my_gi.last_teammates;
	case PLAYER_KNIGHT: /* Rytir */
		return 1;
	case PLAYER_FAT_KNIGHT: /* Tlusty rytir */
		return 8;
	case PLAYER_POLITICAL_KNIGHT: /* Politicky rytir */
		if (my_gi.last_teammates > 0) {
			return 1;
		} else {
			return 5;
		}
	default: /* Pesak */
		return 2;
	}
}

void ktgame_init(void){
	my_info.mana = MANA_MAX;
	my_gi.display_mode = DISP_MANA;

	if (my_info.mode == MODE_KTGAME_FLAG) {
		my_gi.display_mode = DISP_DEFAULT;
	}
	/* flag */
	my_gi.flag_holder = FLAG_HOLDER_NOONE;
	my_gi.flag_attender = FLAG_HOLDER_NOONE;
	my_gi.button_counter = 0;
}

void initialize_my_gi_common(void) {
	my_gi.listen_period = 100;
	my_gi.teammates = 0;
	for (uint8_t i = 0; i < 8; i++) {
		my_gi.seen[i] = 0;
	}
}

void initialize_my_gi(void) {
	my_gi.defense = my_defence();
	my_gi.attack = 0;
	my_gi.last_teammates = my_gi.teammates;
	my_gi.mpm = 0;
	my_gi.special_attack = 0;
	initialize_my_gi_common();
}

void display_score(void) {
	_digits = my_gi.score;
	_digits |= DIGIT_DOTL;
	if (my_gi.score > 0xff) {
		_digits = my_gi.score >> 4;
		_digits |= DIGIT_DOTR;
	}
}

void display_node_status(void) {
	switch(my_gi.display_mode) {
	case DISP_MANA:
		if (my_gi.listen_period == 0) _digits = my_info.mana;
#ifndef NETDEBUG
		evlist |= EV_GREEN_BLINK;
#endif
		break;
	case DISP_DEFENSE:
		if (my_gi.listen_period == 0) _digits = my_gi.defense;
#ifndef NETDEBUG
		evlist |= EV_YELLOW_BLINK;
#endif
		break;
	case DISP_ATTACK:
		if (my_gi.listen_period == 0) _digits = my_gi.attack;
#ifndef NETDEBUG
		evlist |= EV_RED_BLINK;
#endif
		break;
		/* Flags */
	case DISP_RED:
		display_score();
		if (my_gi.button_counter > 0)
			_digits = my_gi.button_counter;
		evlist |= EV_RED_BLINK;
		break;
	case DISP_GREEN:
		display_score();
		if (my_gi.button_counter > 0)
			_digits = my_gi.button_counter;
		evlist |= EV_GREEN_BLINK;
		break;
	default:
		_digits = my_gi.button_counter;
		break;
	}
}

void ktgame_process(void) {
	/* Handle short polling (16 times per second). */
	if (evlist & EV_SHORT_POLL) {
		evlist &= ~EV_SHORT_POLL;

		/* Send packet */
		if (my_info.mana > 0) {
#ifdef NETDEBUG
			evlist |= EV_YELLOW_BLINK;
#endif
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
		my_info.vbat = (adc_read(0xb) >> 2);
		/* Expensive network communication can be done here. */
		/* Initialize game listening period */
		flags |= FL_GAME_LISTEN;
		initialize_my_gi();
		network_arcv_start();
	}

	if (flags & FL_GAME_LISTEN) {
		while (network_arcv(&pk_in)) {
			if (pk_in.mode_adv)
				my_info.mode_adv = pk_in.mode_adv;
#ifdef NETDEBUG
			evlist |= EV_GREEN_BLINK;
#endif
			uint8_t _bop_index = pk_in.node_from & 0x7;
			uint8_t _bop_bit = 1 << ((pk_in.node_from & 0xf8) >> 3);

			/* base station sends mana - no attack/defense computation */
			if (pk_in.mpm > 0) {
				if (pk_in.mpm > my_gi.mpm)
					my_gi.mpm = pk_in.mpm;
			} else if (!(my_gi.seen[_bop_index] & _bop_bit)) {
				my_gi.seen[_bop_index] |= _bop_bit;
				uint8_t team = pk_in.node_from & TEAM_BIT;
				uint8_t my_team = my_info.id & TEAM_BIT;
				if (team == my_team) {
					my_gi.defense += pk_in.defense;
					my_gi.teammates++;
				} else {
					my_gi.attack += pk_in.attack;
					if ((pk_in.node_from & PLAYER_BITS) == PLAYER_POLITICAL_KNIGHT) {
						my_gi.special_attack |= PLAYER_POLITICAL_KNIGHT;
					}
				}
				if (pk_in.node_from & DRAGON_BIT) {
					my_gi.special_attack |= PLAYER_DRAGON_HELPER;
				}
			}
		}

		if (my_gi.listen_period == 0) {
			flags &= ~FL_GAME_LISTEN;
			/* Turn off radio */
			network_arcv_stop();

			/* Evaluate round */
			if (my_gi.mpm > 0) {
				if (my_info.mana + my_gi.mpm <= MANA_MAX) {
					my_info.mana += my_gi.mpm;
				} else {
					my_info.mana = MANA_MAX;
				}
			} else if (my_info.mana > 0) {
				uint16_t damage = my_gi.attack / my_gi.defense;

				/* Political knight damages king and knight */
				if (my_gi.special_attack & PLAYER_POLITICAL_KNIGHT) {
					uint8_t myself = my_info.id & PLAYER_BITS;
					if (myself == PLAYER_KING || myself == PLAYER_KNIGHT) {
						damage = POLITICAL_KNIGHT_MASTER_KILL;
					}
				}
				if (my_gi.special_attack & PLAYER_DRAGON_HELPER) {
					damage = DRAGON_INSTANT_KILL;
				}
				/* Dragons can't die */
				if (!(my_info.id & DRAGON_BIT)) {
					if (my_info.mana > damage) my_info.mana -= damage;
					else my_info.mana = 0;
				}
				if (my_info.mana == 0) {
					beep(1000, 100, 1000);
					beep(1000, 100, 1000);
					beep(1000, 100, 1000);
				}
			}
		} else {
			my_gi.listen_period--;
		}
	}

	/* Handle button pressed event */
	if (flags & FL_BUTTON) {
		flags &= ~FL_BUTTON;
		my_gi.display_mode++;
		if (my_gi.display_mode > DISP_DEFENSE) {
			my_gi.display_mode = DISP_MANA;
		}
	}

	/* Indicate display mode */
	display_node_status();
}


void ktgame_process_flag(void) {
	/* Handle short polling (16 times per second). */
	if (evlist & EV_SHORT_POLL) {
		evlist &= ~EV_SHORT_POLL;
		/* Magique stuff here */
	}

	/* Handle long polling (once per second). */
	if (evlist & EV_LONG_POLL) {
		evlist &= ~EV_LONG_POLL;
		/* Expensive network communication can be done here. */
		display_node_status();
		flags |= FL_GAME_LISTEN;
		initialize_my_gi_common();
		network_arcv_start();

		/* Button pressed checker */
		if (!(P2IN & BUTTON_PIN)) {
			/* Initialize game listening period */
			my_gi.button_counter++;
		} else {
			my_gi.button_counter = 0;
		}
	}

	if (flags & FL_GAME_LISTEN) {
		while (network_arcv(&pk_in)) {
#ifdef NETDEBUG
			evlist |= EV_YELLOW_BLINK;
#endif
			uint8_t team = !!(pk_in.node_from & TEAM_BIT);
			uint8_t _bop_index = pk_in.node_from & 0x7;
			uint8_t _bop_bit = 1 << ((pk_in.node_from & 0xf8) >> 3);
			if (team != my_gi.flag_attender) {
				my_gi.button_counter = 0;
				my_gi.flag_attender = team;
			}
			if (team == my_gi.flag_holder) {
				if (!(my_gi.seen[_bop_index] & _bop_bit)) {
					my_gi.seen[_bop_index] |= _bop_bit;
					my_gi.teammates++;
				}
			}
		}

		if (my_gi.listen_period == 0) {
			flags &= ~FL_GAME_LISTEN;
			/* Turn off radio */
			network_arcv_stop();
			/* Evaluate round */

			if (my_gi.button_counter >= FLAG_CONQUER_PERIOD && my_gi.flag_attender != FLAG_HOLDER_NOONE) {
				my_gi.flag_holder = my_gi.flag_attender;
				my_gi.flag_attender = FLAG_HOLDER_NOONE;
				my_gi.button_counter = 0;
				if (my_gi.flag_holder == TEAM_RED) {
					my_gi.display_mode = DISP_RED;
				}
				if (my_gi.flag_holder == TEAM_GREEN) {
					my_gi.display_mode = DISP_GREEN;
				}
				my_gi.score = 0;
			}

			if (my_gi.teammates < 2)
				my_gi.score++;
		} else {
			my_gi.listen_period--;
		}
	}
}
