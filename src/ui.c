#include "ui.h"

/*

PGMSTR(str_iron, "iron");
PGMSTR(str_fen, "fen");
PGMSTR(str_drel, "drel");

PGMSTR(str_select_mode, "Select mode:");
PGMSTR(str_curr_temp, "Temp ");

inline void menu_display_select(void) {
/ *

    *******!!*******
    Select mode:
            iron
    *******!!*******

* /
    const char *curr;
    char buf[16+1];

    if(gCurrStates.temp == 1)
        curr = str_fen;
    else if(gCurrStates.temp == 2)
        curr = str_drel;
    else
        curr = str_iron;

    if(gCurrStates.changed_menu) {
        gCurrStates.changed_menu = 0;

        lcd_clear();

        lcd_xy(0, 0);
        lcd_str_P(str_select_mode);
    }

    sprintf(buf, "%-4S", (wchar_t*)curr);
    lcd_xy(1, 8);
    lcd_str(buf);
}

inline void menu_display_iron(void) {
/ *
    CurrTemp    TempNeed
    IRON        SLEEP

    ******** ********
    Temp 150 / 350 °C
    iron            W
    ******** ********
* /
    char buf[16+1];

    if(gCurrStates.changed_menu) {
        gCurrStates.changed_menu = 0;

        lcd_clear();

        lcd_xy(0, 0);
        lcd_str_P(str_curr_temp);

        lcd_xy(1, 0);
        lcd_str_P(str_iron);
    }



    sprintf(buf, "%04d / %04d \337C", gCurrStates.iron_temp, gCurrStates.iron_adc);

    lcd_xy(0, 0);
    lcd_str(buf);

    lcd_xy(1, 15);
    lcd_char('W');
}

inline void menu_display_fen(void) {

    if(gCurrStates.changed_menu) {
        gCurrStates.changed_menu = 0;

        lcd_clear();


        lcd_xy(1, 0);
        lcd_str_P(str_fen);
    }
}

inline void menu_display_drel(void) {

    if(gCurrStates.changed_menu) {
        gCurrStates.changed_menu = 0;

        lcd_clear();


        lcd_xy(1, 0);
        lcd_str_P(str_drel);
    }
}


//TProcGui
//---------------------------------------------------------------------------
namespace OS {

template<> OS_PROCESS void TProcGui::exec()
{
    for(;;) {

        gEvChanged.wait();

        switch(gCurrStates.menu) {
            case MENU_IRON:
                menu_display_iron();
                break;

            case MENU_FEN:
                menu_display_fen();
                break;

            case MENU_DREL:
                menu_display_drel();
                break;

            case MENU_SELECT:
                menu_display_select();
                break;

            default:
                break;
        }
    }

} // TProcGui::exec()

} // namespace OS


*/
