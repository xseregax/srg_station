#include "common.h"
#include "ui.h"
#include "actions.h"

volatile TActionCmd g_action_cmd;

//обнулить состояние текущей комманды
inline void actions_init_cmd() {
    g_action_cmd.active = _OFF;
}


void heater_dec_temp(void) {
    volatile THeater *heater = g_data.heater;

    if(heater->temp_need > heater->def_t_min + heater->def_t_step)
        heater->temp_need -= heater->def_t_step;
    else
        heater->temp_need = heater->def_t_min;

    ui_set_update_screen(UPDATE_SCREEN_VALS);
}

void heater_inc_temp(void) {
    volatile THeater *heater = g_data.heater;

    if(heater->temp_need < heater->def_t_max - heater->def_t_step)
        heater->temp_need += heater->def_t_step;
    else
        heater->temp_need = heater->def_t_max;

    ui_set_update_screen(UPDATE_SCREEN_VALS);
}

void menu_select_main(void) {
    heater_off();

    g_data.menu = MENU_SELECT;
    g_data.temp = 0;

    ui_set_update_screen(UPDATE_SCREEN_ALL);
}

void avr_reset(void) {
    AVR_RESET;
}

void menu_select_rotate_left(void) {
    if(g_data.temp == 0)
        g_data.temp = 2;
    else
        g_data.temp -= 1;

    ui_set_update_screen(UPDATE_SCREEN_ALL);
}

void menu_select_rotate_right(void) {
    if(g_data.temp == 2)
        g_data.temp = 0;
    else
        g_data.temp += 1;

    ui_set_update_screen(UPDATE_SCREEN_ALL);
}

void menu_select_mode(void) {
    switch(g_data.temp) {
        case 1:
            g_data.menu = MENU_FEN;

            heater_on();
            break;
        case 2:
            g_data.menu = MENU_DREL;

            break;
        default:
            g_data.menu = MENU_IRON;

            heater_on();
            break;
    }

    ui_set_update_screen(UPDATE_SCREEN_ALL);
}

/*

typedef void (*TVFunc)(void);

#define NAME_BT1_BT2_BT3_BT4_BTE_ENCR_BT1ENC(bt1,bt2,bt3,bt4,bte,encr,bt1enc) {bt1,bt2,bt3,bt4,bte,encr,bt1enc}
#define ACTS_NONE_PUSH_PUSHL_ROTL_ROTR(none,push,pushl,rotl,rotr) {none,push,pushl,rotl,rotr}
#define MENU_SEL_IRON_FEN_DRL(sel,iron,fen,drl) {sel,iron,fen,drl}


// [TActElements][TActions][TMenuStates][TVFunc]

PGM(TVFunc actions_acts[][7][5][4]) = {
    NAME_BT1_BT2_BT3_BT4_BTE_ENCR_BT1ENC(
        ACTS_NONE_PUSH_PUSHL_ROTL_ROTR( //NAME_BUTTON1
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_NONE
            MENU_SEL_IRON_FEN_DRL(avr_reset, avr_reset, avr_reset, avr_reset), //ACT_PUSH
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_PUSH_LONG
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_ROTATE_LEFT
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0)  //ACT_ROTATE_RIGHT
        ),
        ACTS_NONE_PUSH_PUSHL_ROTL_ROTR( //NAME_BUTTON2
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_NONE
            MENU_SEL_IRON_FEN_DRL(menu_select_mode, 0, 0, 0), //ACT_PUSH
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_PUSH_LONG
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_ROTATE_LEFT
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0)  //ACT_ROTATE_RIGHT
        ),
        ACTS_NONE_PUSH_PUSHL_ROTL_ROTR( //NAME_BUTTON3
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_NONE
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_PUSH
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_PUSH_LONG
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_ROTATE_LEFT
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0)  //ACT_ROTATE_RIGHT
        ),
        ACTS_NONE_PUSH_PUSHL_ROTL_ROTR( //NAME_BUTTON4
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_NONE
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_PUSH
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_PUSH_LONG
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_ROTATE_LEFT
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0)  //ACT_ROTATE_RIGHT
        ),
        ACTS_NONE_PUSH_PUSHL_ROTL_ROTR( //NM_ENCBUTTON
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_NONE
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_PUSH
            MENU_SEL_IRON_FEN_DRL(0, menu_select_main, 0, 0), //ACT_PUSH_LONG
            MENU_SEL_IRON_FEN_DRL(menu_select_rotate_left, iron_dec_temp, 0, 0), //ACT_ROTATE_LEFT
            MENU_SEL_IRON_FEN_DRL(menu_select_rotate_right, iron_inc_temp, 0, 0)  //ACT_ROTATE_RIGHT
        ),
        ACTS_NONE_PUSH_PUSHL_ROTL_ROTR( //NM_ENCROTATE
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_NONE
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_PUSH
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_PUSH_LONG
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_ROTATE_LEFT
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0)  //ACT_ROTATE_RIGHT
        ),
        ACTS_NONE_PUSH_PUSHL_ROTL_ROTR( //NAME_BUTTON1_ENC
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_NONE
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_PUSH
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_PUSH_LONG
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0), //ACT_ROTATE_LEFT
            MENU_SEL_IRON_FEN_DRL(0, 0, 0, 0)  //ACT_ROTATE_RIGHT
        )
    )
};
*/

PT_THREAD(actions_pt_check_commands(struct pt *pt)) {
    PT_BEGIN(pt);

    for(;;) {
        PT_WAIT_UNTIL(pt, g_action_cmd.active);

        g_action_cmd.active = _OFF;

//        TVFunc *cmd = (TVFunc*)pgm_read_word(&actions_acts[g_action_cmd.name][g_action_cmd.action][g_data.menu]);
//        if(cmd) (*cmd)();

        if(g_action_cmd.name == NM_BUTTON1) {

            if(g_action_cmd.action == ACT_PUSH) {
                ON(P_LED_RED);

                avr_reset();

            } //ACT_PUSH

        } //NM_BUTTON1
        else
        if(g_action_cmd.name == NM_BUTTON2) {

            if(g_action_cmd.action == ACT_PUSH) {

                if(g_data.menu == MENU_SELECT) {
                    menu_select_mode();
                } //MENU_SELECT
            } //ACT_PUSH


        } //NM_BUTTON2
        else
        if(g_action_cmd.name == NM_BUTTON3) {

            if(g_action_cmd.action == ACT_PUSH) {


            } //ACT_PUSH
        } //NM_BUTTON3
        else
        if(g_action_cmd.name == NM_ENCBUTTON) {

            if(g_action_cmd.action == ACT_PUSH_LONG) {

                if(g_data.menu != MENU_SELECT) {
                    menu_select_main();
                } //NOT MENU_SELECT

            } //ACT_PUSH_LONG


        } //NM_ENCBUTTON
        else
        if(g_action_cmd.name == NM_ENCROTATE) {

            if(g_action_cmd.action == ACT_ROTATE_LEFT) {

                if(g_data.menu == MENU_IRON || g_data.menu == MENU_FEN) {
                    heater_dec_temp();
                } //MENU_IRON
                else
                if(g_data.menu == MENU_SELECT) {
                    menu_select_rotate_left();
                } //MENU_SELECT

            } //ACT_ROTATE_LEFT
            else
            if(g_action_cmd.action == ACT_ROTATE_RIGHT) {

                if(g_data.menu == MENU_IRON || g_data.menu == MENU_FEN) {
                    heater_inc_temp();
                } //MENU_IRON
                else
                if(g_data.menu == MENU_SELECT) {
                    menu_select_rotate_right();
                } //MENU_SELECT

            } //ACT_ROTATE_LEFT

        } //NM_ENCROTATE

    }

    PT_END(pt);
}



void actions_init_mod(void) {
    actions_init_cmd();
}


