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

/* 
 * All functions in radiation.c shall be deprecated in favor of new interface,
 * combining persistent.c and network.c functions. 
 */


int msg_receive(){
	return 0;
}

void msg_beacon() {
	return;
}

void rad_sumb() {
}

void rad_sumc() {
}	

unsigned long rad_get() {
	return 0;
}

void rad_store(unsigned long newrad) {
	return;
}
