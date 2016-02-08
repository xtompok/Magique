/******************************************************************************
 * Filename: magique.h
 *
 * Created: 2016/01/24 13:47
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#ifndef __MAGIQUE_H__
#define __MAGIQUE_H__

#include <stdint.h>

/* Node specific data */
struct node {
	uint16_t id;
	uint8_t mode;
	uint8_t mode_adv;
	uint16_t mana;
	uint8_t  essences;
	uint16_t vbat;
};

#define MODE_KTGAME 1
#define MODE_MAGIQUE_SOURCE 2
#define MODE_MAGIQUE_STONE 3
#define MODE_KTGAME_FLAG 4

#define ESSENCE_RED     EV_RED_BLINK
#define ESSENCE_YELLOW  EV_YELLOW_BLINK
#define ESSENCE_GREEN   EV_GREEN_BLINK

extern struct node my_info;

#endif
