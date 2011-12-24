#include "common.h"
#include "hd44780.h"
#include "ui.h"

volatile uint8_t g_ui_update_screen;
volatile TMenuStates g_ui_menu;

PGMSTR(str_iron, "iron");
PGMSTR(str_fen, "fen");
PGMSTR(str_drel, "drel");

PGMSTR(str_select_mode, "Select mode:");
PGMSTR(str_curr_temp, "Temp ");

inline void menu_display_select(void) {
/*

    *******!!*******
    Select mode:
            iron
    *******!!*******

*/
    const char *curr;
    char buf[16+1];

    if(g_ui_update_screen & UPDATE_SCREEN_MENU) {
        lcd_clear();

        lcd_xy(0, 0);
        lcd_str_P(str_select_mode);
    }

    if(g_ui_update_screen & UPDATE_SCREEN_VALS) {
        if(g_data.temp == 1)
            curr = str_fen;
        else if(g_data.temp == 2)
            curr = str_drel;
        else
            curr = str_iron;

        sprintf(buf, "%-4S", curr);
        lcd_xy(1, 8);
        lcd_str(buf);
    }
}


inline void menu_display_iron(void) {
/*
    CurrTemp    TempNeed
    IRON        SLEEP

    ******** ********
    Temp 150 / 350 °C
    iron            W
    ******** ********
*/
    char buf[16+1];

    if(g_ui_update_screen & UPDATE_SCREEN_MENU) {
        lcd_clear();

        lcd_xy(0, 0);
        //lcd_str_P(str_curr_temp);

        lcd_xy(1, 0);
        //lcd_str_P(str_iron);
    }

    if(g_ui_update_screen & UPDATE_SCREEN_VALS) {

        sprintf(buf, "%04d / %04d \337C", g_data.iron.temp, g_data.iron.temp_need);

        lcd_xy(0, 0);
        lcd_str(buf);

        lcd_xy(1, 0);
        sprintf(buf, "%04d / %04d %%", g_data.iron.adc, g_data.iron.power);
        lcd_str(buf);

        //lcd_xy(1, 15);
        //lcd_char('H');
    }

    if(g_ui_update_screen & UPDATE_SCREEN_ERROR) {
        lcd_clear();

        lcd_xy(0, 0);
        lcd_str("ERROR");

        lcd_xy(1, 0);
        lcd_str("INSERT IRON!");
    }
}

PT_THREAD(ui_pt_update_display(struct pt *pt)) {
    static TIMER_T timer;

    PT_BEGIN(pt);

    TIMER_INIT(timer, UI_UPDATE_TIME);
    for(;;) {
        PT_WAIT_UNTIL(pt, !(g_ui_update_screen & UPDATE_SCREEN_CLEAR) &&
                      TIMER_ENDED(timer));
        TIMER_INIT(timer, UI_UPDATE_TIME);

        switch(g_ui_menu) {
            case MENU_IRON:
                menu_display_iron();
                break;

            case MENU_FEN:
                //menu_display_fen();
                break;

            case MENU_DREL:
                //menu_display_drel();
                break;

            case MENU_SELECT:
                menu_display_select();
                break;

            default:
                break;
        }


        g_ui_update_screen &= ~UPDATE_SCREEN_CLEAR;
    }

    PT_END(pt);
}




void ui_init_mod(void) {
    //обновить экран первый раз
    ui_set_update_screen(UPDATE_SCREEN_ALL);

    g_ui_menu = MENU_SELECT;
}


PGMSTR(str_hello, "SRG @ STATION");
PGMSTR(str_version, "*** v"VERSION" ***");

//первоначальная заставка
void ui_hello_msg(void) {

    lcd_xy(0, 1);
    lcd_str_P(str_hello);

    lcd_xy(1, 3);
    lcd_str_P(str_version);

    //пискнем
    BEEP(1000);

    //очистим экран
    lcd_clear();
}
