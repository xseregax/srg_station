#ifndef ACTIONS_H
#define ACTIONS_H

volatile TActionCmd g_action_cmd;


void actions_init_mod(void);
PT_THREAD(actions_pt_check_commands(struct pt *pt));


#endif // ACTIONS_H
