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
	uint8_t type;
	uint8_t team;
	uint8_t id;
	uint8_t units;
	uint8_t mode;
	uint16_t vbat;
};

#define TYPE_PLAYER 1
#define TYPE_CITY 2
#define TYPE_SOURCE 3
#define TYPE_BASE 4

#define MODE_ATTACK 1
#define MODE_DEFENSE 2
#define MODE_AUTO 3

#define MY_UNITS 128

#define ESSENCE_RED     EV_RED_BLINK
#define ESSENCE_YELLOW  EV_YELLOW_BLINK
#define ESSENCE_GREEN   EV_GREEN_BLINK

extern struct node my_info;

#endif
