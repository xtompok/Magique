/******************************************************************************
 * Filename: network.h
 *
 * Created: 2015/11/16 21:28
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <stdint.h>

/* Keep it 32B long, as we can provide backward compatibility/multiple packet
 * types later */
struct packet {
	/*  2B */
	uint16_t node_from;
	/*  4B */
	uint16_t node_to;	/* Node 0 means broadcast */
	/*  6B */
	uint16_t vbat;
	/*  8B */
	uint16_t vcc;

	/* 32B */
	uint8_t _pad[12];
};

#define RF_CH 122
#define RF_SETUP (RF_PWR0 | RF_PWR1)


/* Initialized NRF for network communication. */
void network_init();


#endif
