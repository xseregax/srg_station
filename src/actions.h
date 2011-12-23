#ifndef ACTIONS_H
#define ACTIONS_H

extern volatile TActionCmd g_action_cmd;


inline void actions_set_cmd(TActElements el, TActions act) {
    g_action_cmd.name = el;
    g_action_cmd.action = act;
    g_action_cmd.active = _ON;
}

void actions_init_mod(void);
PT_THREAD(actions_pt_check_commands(struct pt *pt));


#endif // ACTIONS_H
