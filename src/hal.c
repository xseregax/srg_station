#include "common.h"
#include "hal.h"

/*

//выкл станции
inline void power_off(void) {
    OFF(P_POWER);
}



//инит глобальных переменных
inline void init_curstates(void) {
    memset((void*)&gCurrStates, 0, sizeof(TCurrStates));

    gCurrStates.menu = MENU_SELECT; //активный пункт меню
    gCurrStates.temp = 0; //

    gCurrStates.changed_menu = 1; //обновить весь экран

    gCurrStates.iron_temp_need = IRON_TEMP_MIN; //нуна будет брать запомненные значения
    gCurrStates.fen_temp_need = FEN_TEMP_MIN; // - // -
}

//---------------------------------------------------------------------------


void iron_power_on(void) {
    ON(P_IRON_PWM);
    _delay_us(100);
    OFF(P_IRON_PWM);
}

*/
