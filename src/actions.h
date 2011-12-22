#ifndef ACTIONS_H
#define ACTIONS_H

volatile TActionCmd g_action_cmd;


void actions_init_mod(void);
PT_THREAD(actions_pt_check_commands(struct pt *pt));



#define NAME_BT1_BT2_BT3_BT4_BTE_ENCR_BT1ENC(bt1,bt2,bt3,bt4,bte,encr,bt1enc) {bt1,bt2,bt3,bt4,bte,encr,bt1enc}
#define ACTS_NONE_PUSH_PUSHL_ROTL_ROTR(none,push,pushl,rotl,rotr) {none,push,pushl,rotl,rotr}
#define MENU_SEL_IRON_FEN_DRL(sel,iron,fen,drl) {sel,iron,fen,drl}

typedef void (*TVFunc)(void);
/*
[TActElements][TActions][TMenuStates][TVFunc]
*/


#endif // ACTIONS_H
