/******************************************************************************
 * Filename: radiation.c
 * Description:
 *
 * Version: 1.0
 * Created: Mar 01 2013 21:28:32
 * Last modified: Mar 01 2013 21:28:32
 *
 * Author: Ladislav Láska
 * e-mail: ladislav.laska@gmail.com
 *
 ******************************************************************************/

#include "nrf.h"
#include "radiation.h"
#include "globals.h"

unsigned char message[MSG_LENGTH];
unsigned char _rcvbuf[MSG_LENGTH];
unsigned char rad_zone;

#define INFOB __attribute__((section("infob")))
#define INFOC __attribute__((section("infoc")))
#define INFOD __attribute__((section("infod")))

#define SUM1B ((rad_counterb[0] + rad_counterb[1]) % 251)
#define SUM2B ((rad_counterb[0] + rad_counterb[1]) % 241)
#define SUM1C ((rad_counterc[0] + rad_counterc[1]) % 251)
#define SUM2C ((rad_counterc[0] + rad_counterc[1]) % 241)

unsigned long rad_counterb[2] INFOB = { MYVAL, 0 };
unsigned char rad_permb_sum1  INFOB = (MYVAL) % 251;
unsigned char rad_permb_sum2  INFOB = (MYVAL) % 241;
unsigned long rad_counterc[2] INFOC = { MYVAL, 0 };
unsigned char rad_permc_sum1  INFOC = MYVAL % 251;
unsigned char rad_permc_sum2  INFOC = MYVAL % 241;
unsigned long rad;
unsigned char counter_select INFOD = 0;

int msg_receive(){
	for (int i = 0; i < MSG_LENGTH; i++) 
		_rcvbuf[i] = 0;
	if (nrf_receive(_rcvbuf, MSG_LENGTH)) {
		if ((_rcvbuf[MSG_MAGIC] == MSG_MAGIC_VAL) && (_rcvbuf[MSG_ZERO] == 0) 
			&& ((_rcvbuf[MSG_TO] == 0) || (_rcvbuf[MSG_TO] == MYNAME)))  {
			rad_zone = _rcvbuf[MSG_RAD];
			switch (_rcvbuf[MSG_COMMAND]) {
				case CMD_COUNTER_SELECT:
					if (counter_select != _rcvbuf[MSG_VALUE1]) {
						counter_select = _rcvbuf[MSG_VALUE1];
						rad_store(0); /* Load from segment */
					}
					evlist |= EV_GREEN_BLINK;
					break;
				case CMD_COUNTER_SET:
					rad = 0;
					rad |= _rcvbuf[MSG_COUNTER1];	rad <<= 8;
					rad |= _rcvbuf[MSG_COUNTER2];	rad <<= 8;
					rad |= _rcvbuf[MSG_COUNTER3];	rad <<= 8;
					rad |= _rcvbuf[MSG_COUNTER4];
					rad_store(rad);
					evlist |= EV_GREEN_BLINK;
					break;

			}
			return 1;
		}
	}

	return 0;
}

void msg_beacon() {
	message[MSG_MAGIC] = MSG_MAGIC_VAL;
	message[MSG_ZERO] = 0;
	nrf_transmit(message, MSG_LENGTH);
}

void rad_sumb() {
	rad_permb_sum1 = SUM1B;
	rad_permb_sum2 = SUM2B;
}

void rad_sumc() {
	rad_permc_sum1 = SUM1C;
	rad_permc_sum2 = SUM2C;
}	

unsigned long rad_get() {
	return rad_counterb[counter_select];
}

void rad_store(unsigned long newrad) {
	unsigned char cs = counter_select & 1;
	/* Check segment consistency */
	unsigned char status = 0;
	if ((rad_permb_sum1 == SUM1B) && (rad_permb_sum2 == SUM2B))
		status |= 1;
	if ((rad_permc_sum1 == SUM1C) && (rad_permc_sum2 == SUM2C))
		status |= 2;
	
	if (newrad == 0) {	 /* 0 means 'restore' from backup segment */
		switch (status) {
			case 0x2: rad = rad_counterc[counter_select]; break;
			case 0x3:
			case 0x1: rad = rad_counterb[counter_select]; break;
			default:
			case 0x0: rad = 0xeeeeeeee; break;	/* Shit, set 0xee to show error */
		}
	}
	
	switch (status) {
		case 0x0:;	/* Both segments are inconsistent, shit... */
			rad = 0xbbbbbbbb;
			break;
		case 0x2:	/* Segment B is inconsistent, dump it first */
			rad_counterb[cs] = newrad;
			rad_counterb[cs ^ 1] = rad_counterc[cs ^ 1];
			rad_sumb();
			rad_counterc[cs] = newrad;
			rad_sumc();
			break;
		case 0x3:	/* Both segments are consistent, order does not matter */
		case 0x1:	/* Segment C is inconsistent, dump it first */
			rad_counterc[cs] = newrad;
			rad_counterc[cs ^ 1] = rad_counterb[cs ^ 1];
			rad_sumc();
			rad_counterb[cs] = newrad;
			rad_sumb();
			break;
	}
}
