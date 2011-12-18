#ifndef HAL_H
#define HAL_H

#include "hd44780.h"











//вкл станции
inline void power_on(void) {
    DRIVER(P_POWER,OUT);
    ON(P_POWER);
}


//инит экрана
inline void init_lcd(void) {
    hd44780_init();
}






#endif // HAL_H
