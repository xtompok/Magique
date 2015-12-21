/******************************************************************************
 * Filename: persistent.h
 *
 * Created: 2015/12/21 14:20
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#ifndef __PERSISTENT_H__
#define __PERSISTENT_H__

#include <stdint.h>

struct persistent_data {
	uint16_t node_id;
	uint16_t mana;
	uint16_t essence;
};

#define INFOB __attribute__((section("infob")))

extern struct persistent_data node_data INFOB;

#endif
