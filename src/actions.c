#include "actions.h"

/*


inline void menu_buttons_select(const TActionCmd &action) {

    if(action.name == NM_ENCROTATE) {

        switch(action.action) {
        case ACT_ROTATE_LEFT:
            if(gCurrStates.temp == 0)
                gCurrStates.temp = 2;
            else
                gCurrStates.temp -= 1;
            break;

        case ACT_ROTATE_RIGHT:
            if(gCurrStates.temp == 2)
                gCurrStates.temp = 0;
            else
                gCurrStates.temp += 1;
            break;

        default:
            return;
        }
    }
    else
    if(action.name == NM_BUTTON1) {

        switch(action.action) {
        case ACT_PUSH: {
            switch(gCurrStates.temp) {
                case 1:
                    gCurrStates.menu = MENU_FEN;
                    gCurrStates.fen_on = 1;
                    gCurrStates.iron_on = 0;
                    gCurrStates.drel_on = 0;
                    break;
                case 2:
                    gCurrStates.menu = MENU_DREL;
                    gCurrStates.drel_on = 1;
                    gCurrStates.iron_on = 0;
                    gCurrStates.fen_off = 1;
                    break;
                default:
                    gCurrStates.menu = MENU_IRON;
                    gCurrStates.iron_on = 1;
                    gCurrStates.drel_on = 0;
                    gCurrStates.fen_off = 1;
                    break;
            }
            gCurrStates.changed_menu = 1;
            break;
        }
        default:
            return;
        }
    }
    else
        return;

    gEvChanged.signal();
}

inline void menu_buttons_iron(const TActionCmd &action) {

    if(action.name == NM_ENCROTATE) {

        switch(action.action) {
        case ACT_ROTATE_LEFT:

            if(gCurrStates.iron_temp_need > IRON_TEMP_MIN + IRON_TEMP_STEP)
                gCurrStates.iron_temp_need -= IRON_TEMP_STEP;
            else
                gCurrStates.iron_temp_need = IRON_TEMP_MIN;

            break;

        case ACT_ROTATE_RIGHT:

            if(gCurrStates.iron_temp_need < IRON_TEMP_MAX - IRON_TEMP_STEP)
                gCurrStates.iron_temp_need += IRON_TEMP_STEP;
            else
                gCurrStates.iron_temp_need = IRON_TEMP_MAX;

            break;

        default:
            return;
        }
    }
    else
    if(action.name == NM_ENCBUTTON) {

        switch(action.action) {
        case ACT_PUSH_LONG:

            gCurrStates.menu = MENU_SELECT;
            gCurrStates.temp = 0;
            gCurrStates.changed_menu = 1;
            gCurrStates.iron_on = 0;
            break;

        default:
            return;
        }
    }
    else
    if(action.name == NM_BUTTON4) {
        switch(action.action) {
        case ACT_PUSH:
            TOGGLE(P_LED_RED);
            //iron_power_on();

            gCurrStates.fen_on = !gCurrStates.fen_on;
            break;

        default:
            return;
        }

    }
    else
        return;

    gEvChanged.signal();
}

inline void menu_buttons_fen(const TActionCmd &action) {

    if(action.name == NM_ENCBUTTON) {

        switch(action.action) {
        case ACT_PUSH_LONG:

            gCurrStates.menu = MENU_SELECT;
            gCurrStates.temp = 1;
            gCurrStates.changed_menu = 1;
            gCurrStates.fen_off = 1;
            break;

        default:
            return;
        }
    }
    else
        return;

    gEvChanged.signal();
}

inline void menu_buttons_drel(const TActionCmd &action) {

    if(action.name == NM_ENCBUTTON) {

        switch(action.action) {
        case ACT_PUSH_LONG:

            gCurrStates.menu = MENU_SELECT;
            gCurrStates.temp = 2;
            gCurrStates.changed_menu = 1;
            gCurrStates.drel_on = 0;
            break;

        default:
            return;
        }
    }
    else
        return;

    gEvChanged.signal();
}





//TProcActions
//---------------------------------------------------------------------------
namespace OS {

template<> OS_PROCESS void TProcActions::exec()
{
    TActionCmd action;

    for(;;) {
        gActions.pop(action);

        switch(gCurrStates.menu) {
        case MENU_IRON:
            menu_buttons_iron(action);
            break;
        case MENU_FEN:
            menu_buttons_fen(action);
            break;
        case MENU_DREL:
            menu_buttons_drel(action);
            break;
        case MENU_SELECT:
            menu_buttons_select(action);
            break;
        default:
            break;
        }

    }

} // TProcActions::exec()

} // namespace OS

//---------------------------------------------------------------------------


*/
