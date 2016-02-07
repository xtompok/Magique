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
#include "nrf.h"

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

	/* Game data; 10B */
	uint8_t attack;
	uint8_t defense;

	/* Stones & sources; 12B */
	int8_t mpm;	/* Mana per minute, shall only be emitted by the source and can be negative */
	uint8_t ess; /* Essencess available, shall only be emitted by the source */

	/* Mode switching; 13B */
	uint8_t mode_adv;

	/* 32B */
	uint8_t _pad[19];
};

extern struct packet pk_in;
extern struct packet pk_out;

#define RF_CH 110
#define RF_SETUP 0
//(RF_PWR0 | RF_PWR1)
#define RF_ROLE_TX 0
#define RF_ROLE_RX PRIM_RX

/* Initialized NRF for network communication. */
void network_init(uint8_t role);

/* Make a basic packet with our stats and id */
void network_mkpacket(struct packet *p);

/* Sends a packet, and returns true if ack was sent. Always returns false if
 * req_ack was false. */
uint8_t network_send(struct packet *p, uint8_t req_ack);

/*
 * Receives packet if one is available untile timeout expires.
 *
 * If timeout is 0, packet is returned only if it's already available at the
 * time of execution.
 *
 */
uint8_t network_rcv(struct packet *p, uint16_t timeout);

void network_arcv_start(void);
void network_arcv_stop(void);
uint8_t network_arcv(struct packet *p);

#endif

