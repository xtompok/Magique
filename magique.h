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
	uint16_t mode;
	uint16_t mana;
	uint8_t  essences;
};

#define MODE_KTGAME 1

#define ESSENCE_RED     (1 << 0)
#define ESSENCE_YELLOW  (1 << 1)
#define ESSENCE_GREEN   (1 << 2)

extern struct node my_info;

#endif
