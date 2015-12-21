/******************************************************************************
 * Filename: network.c
 *
 * Created: 2015/12/21 14:15
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include "network.h"
#include "adc.h"
#include "persistent.h"

/* Initializes packet */
void packet_init(struct packet *p) {
	adc_on();
	p->vcc = adc_read(11)*(5000.0/1023); /* Internal channel (vcc - vss) /2 */
	adc_off();

	p->node_from = node_data.node_id;
}
