/******************************************************************************
 * Filename: radiation.h
 * Description:
 *
 * Version: 1.0
 * Created: Mar 01 2013 22:00:18
 * Last modified: Mar 01 2013 22:00:18
 *
 * Author: Ladislav Láska
 * e-mail: ladislav.laska@gmail.com
 *
 ******************************************************************************/
#ifndef _RADIATION_H_
#define _RADIATION_H_

#define MSG_MAGIC 0
#define MSG_FROM 1
#define MSG_TO 2
#define MSG_BAT 3
#define MSG_RAD 4
#define MSG_COUNTER1 5
#define MSG_COUNTER2 6
#define MSG_COUNTER3 7
#define MSG_COUNTER4 8
#define MSG_COMMAND 9
#define MSG_VALUE1 10
#define MSG_VALUE2 11
#define MSG_CHECKSUM 12
#define MSG_RESERVED 13
#define MSG_ZERO 14

#define CMD_COUNTER_SELECT 11
#define CMD_COUNTER_SET	12

#define MSG_LENGTH 15

#define MSG_MAGIC_VAL 0xAE

extern unsigned char message[MSG_LENGTH];
extern unsigned char _rcvbuf[MSG_LENGTH];
extern unsigned long rad;
extern unsigned long rad_perm;
extern unsigned char rad_zone;
extern unsigned char counter_select;

void msg_beacon();
int msg_receive();
void rad_store(unsigned long rad);
unsigned long rad_get();

#endif
