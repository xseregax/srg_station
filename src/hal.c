#include "common.h"
#include "hd44780.h"

#include "hal.h"

/*

//выкл станции
inline void power_off(void) {
    OFF(P_POWER);
}


//инит прерываний
inline void init_isr(void) {
    //int2 - для ZCD

    uint8_t t;

    //in, hiz
    t = BITMASK(P_ZCD);
    DDRB &= ~t;
    PORTB &= ~t;

    MCUCSR &= ~_BV(ISC2); //Set to falling edge interrupt
    GICR |= _BV(INT2); //Enable external int0

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

PGMSTR(str_hello, "SRG @ STATION");
PGMSTR(str_version, "*** v"VERSION" ***");

//первоначальная заставка
inline void hello_msg(void) {

    lcd_xy(0, 1);
    lcd_str_P(str_hello);

    lcd_xy(1, 3);
    lcd_str_P(str_version);

    //пискнем
    ON(P_BUZER);
    _delay_ms(1000);
    OFF(P_BUZER);

    //очистим экран
    lcd_clear();
}



void iron_power_on(void) {
    ON(P_IRON_PWM);
    _delay_us(100);
    OFF(P_IRON_PWM);
}

*/
