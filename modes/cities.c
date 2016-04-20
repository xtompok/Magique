#include <msp430.h>
#include "magique.h"
#include "globals.h"
#include "network.h"
#include "delay.h"
#include "cities.h"
#include "beep.h"

uint8_t teams[10];
uint8_t times[10]; 
uint8_t others[40];
uint16_t succesfull;

inline void respond(uint8_t id,uint8_t action){
	pk_out.units = my_info.units;
	pk_out.action = action;
	pk_out.node_to = id;
	network_arcv_stop();
	network_send(&pk_out,0);
	network_arcv_start();
		
}


static void transfer_start(uint8_t id){
	respond(id,ACTION_TRANSFER | STAGE_INIT);
}
static void transfer_process(void){
	uint8_t stage;
	uint8_t id;
	id = pk_in.node_from;
	stage = pk_in.action >> STAGE_SHIFT;
	switch (stage){
		case STAGE_INIT:
			respond(id,ACTION_TRANSFER | STAGE_INACK);
			break;
		case STAGE_INACK:
			my_info.units--;
			respond(id,ACTION_TRANSFER | STAGE_DO);
			break;
		case STAGE_DO:
			my_info.units++;
			respond(id,ACTION_TRANSFER | STAGE_DONE);
			break;
		case STAGE_DONE: 	
			beep(500,10,0);
			succesfull++; 
			break;
	}
	
}

static void attack_start(uint8_t id){
	respond(id,ACTION_ATTACK | STAGE_INIT);
}

static void attack_process(void){
	beep(500,10,0);
	uint8_t stage;
	uint8_t id;
	id = pk_in.node_from;
	stage = pk_in.action & STAGE_MASK;
	switch (stage){
		case STAGE_INIT:
			respond(id,ACTION_TRANSFER | STAGE_INACK);
			break;
		case STAGE_INACK:
			my_info.units--;
			respond(id,ACTION_TRANSFER | STAGE_DO);
			break;
		case STAGE_DO:
			my_info.units--;
			respond(id,ACTION_TRANSFER | STAGE_DONE);
			break;
		case STAGE_DONE: 	
			beep(500,10,0);
			succesfull++; 
			break;
	}
	
}

void cities_init(void){
	network_init( RF_ROLE_RX );
	network_arcv_start();
	pk_out.team = my_info.team;
	pk_out.node_from = my_info.id;
	pk_out.type = my_info.type;
}

void cities_broadcast(void){	
	uint8_t i;
	pk_out.node_to = 0;
	pk_out.action = ACTION_BROADCAST;
	pk_out.units = my_info.units;
	if (my_info.type == TYPE_CITY){
		for (i=0;i<10;i++){
			pk_out.teams[i]=teams[i];
			pk_out.times[i]=times[i];	
		}
	}
	network_arcv_stop();
	network_send(&pk_out,0);
	network_arcv_start();
}

void cities_process_broadcast(void){
	//beep(1000,10,0);
	// My team
	if (pk_in.team == my_info.team){
		switch (my_info.mode){
			case MODE_ATTACK:
				// Do nothing
				break;
			case MODE_AUTO:
				// Don't do anything if oponent has units more or like me
				if ((pk_in.units < 250) && (pk_in.units + 2 >= my_info.units)){
					break;
				}
			case MODE_DEFENSE:
				transfer_start(pk_in.node_from);
				break;
		}
		
	// Other team
	} else {
		switch (my_info.mode){
			case MODE_AUTO:
				// Don't do anything if I don't have much units
				if (my_info.units < AUTO_ATTACK_TRESHOLD){
					break;
				}
			case MODE_ATTACK:
				attack_start(pk_in.node_from);
				break;
			case MODE_DEFENSE:
				// Do nothing
				break;
		}
	
	}
	
}

void cities_process_action(void){
	uint8_t action;
	action = pk_in.action & ACTION_MASK;
	switch (action){
		case ACTION_TRANSFER:
			transfer_process();
			break;
		case ACTION_ATTACK:
			attack_process();
			break;
	}
	
}

