#ifndef CITIES_H
#define CITIES_H

void cities_init(void);
void cities_broadcast(void);
void cities_process_broadcast(void);
void cities_process_action(void);

#define ACTION_BROADCAST 1
#define ACTION_ATTACK 2
#define ACTION_TRANSFER 3
#define ACTION_MASK 0x0F
#define STAGE_INIT (1<<4)
#define STAGE_INACK (2<<4)
#define STAGE_DO (3<<4)
#define STAGE_DONE (4<<4)
#define STAGE_SHIFT 4
#define STAGE_MASK 0xF0

#define AUTO_ATTACK_TRESHOLD 20

#endif
