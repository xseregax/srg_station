#include "common.h"
#include "heater.h"
#include "actions.h"

volatile TActionCmd g_action_cmd;

//обнулить состояние текущей комманды
inline void actions_init_cmd() {
    //memset((void*)&g_action_cmd, 0, sizeof(TActionCmd));
    g_action_cmd.active = 0;
}

inline void menu_buttons_select(volatile const TActionCmd *action) {

    if(action->name == NM_ENCROTATE) {

        switch(action->action) {
        case ACT_ROTATE_LEFT:
            if(g_data.temp == 0)
                g_data.temp = 2;
            else
                g_data.temp -= 1;
            break;

        case ACT_ROTATE_RIGHT:
            if(g_data.temp == 2)
                g_data.temp = 0;
            else
                g_data.temp += 1;
            break;

        default:
            return;
        }
    }
    else
    if(action->name == NM_BUTTON1) {

        switch(action->action) {
        case ACT_PUSH: {
            switch(g_data.temp) {
                case 1:
                    g_data.menu = MENU_FEN;
                    //g_data.fen.on = 1;
                    g_data.iron.on = 0;
                    //g_data.drel.on = 0;
                    break;
                case 2:
                    g_data.menu = MENU_DREL;
                    //g_data.drel.on = 1;
                    g_data.iron.on = 0;
                    //g_data.fen.off = 1;
                    break;
                default:
                    g_data.menu = MENU_IRON;
                    g_data.iron.on = 1;
                    //g_data.drel.on = 0;
                    //g_data.fen.off = 1;
                    break;
            }
            g_data.update_screen |= UPDATE_SCREEN_ALL;
            break;
        }
        default:
            return;
        }
    }
    else
        return;

    g_data.update_screen |= UPDATE_SCREEN_VALS;
}

inline void menu_buttons_iron(volatile const TActionCmd *action) {

    if(action->name == NM_ENCROTATE) {

        switch(action->action) {
        case ACT_ROTATE_LEFT:

            if(g_data.iron.temp_need > IRON_TEMP_MIN + IRON_TEMP_STEP)
                g_data.iron.temp_need -= IRON_TEMP_STEP;
            else
                g_data.iron.temp_need = IRON_TEMP_MIN;

            break;

        case ACT_ROTATE_RIGHT:

            if(g_data.iron.temp_need < IRON_TEMP_MAX - IRON_TEMP_STEP)
                g_data.iron.temp_need += IRON_TEMP_STEP;
            else
                g_data.iron.temp_need = IRON_TEMP_MAX;

            break;

        default:
            return;
        }

    }
    else
    if(action->name == NM_ENCBUTTON) {

        switch(action->action) {
        case ACT_PUSH_LONG:

            g_data.menu = MENU_SELECT;
            g_data.temp = 0;
            g_data.update_screen |= UPDATE_SCREEN_ALL;
            g_data.iron.on = 0;
            break;

        default:
            return;
        }
    }
    else
    if(action->name == NM_BUTTON4) {
        switch(action->action) {
        case ACT_PUSH:
            TOGGLE(P_LED_RED);
            //iron_power_on();

            //g_data.fen.on = !g_data.fen.on;
            break;

        default:
            return;
        }

    }
    else
        return;

    g_data.update_screen |= UPDATE_SCREEN_VALS;
}

PT_THREAD(actions_pt_check_commands(struct pt *pt)) {
    PT_BEGIN(pt);

    for(;;) {
        PT_WAIT_UNTIL(pt, g_action_cmd.active);

        switch(g_data.menu) {
        case MENU_IRON:
            menu_buttons_iron(&g_action_cmd);
            break;
        case MENU_FEN:
            //menu_buttons_fen(&action);
            break;
        case MENU_DREL:
            //menu_buttons_drel(&action);
            break;
        case MENU_SELECT:
            menu_buttons_select(&g_action_cmd);
            break;
        default:
            break;
        }

        g_action_cmd.active = 0;
    }


    PT_END(pt);
}



void actions_init_mod(void) {
    actions_init_cmd();
}


