#ifndef COMMON_H
#define COMMON_H

#ifndef __AVR_ATmega16A__
    #define __AVR_ATmega16A__
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>

#include "pt.h"
#include "timer.h"

#include "pin_macros.h"
#include "pins.h"


#define VERSION "0.1"


//#define PSTRX(x, y) static const char x[] PROGMEM = (y)
#define PGM(a) extern const a PROGMEM; const a
#define PGMSTR(name,value) \
    extern const char name[] PROGMEM; const char name[] = (value)



#define PN_PIN_(port,bit,val) (PIN##port)
#define PN_PORT_(port,bit,val) (PORT##port)
#define PN_DDR_(port,bit,val) (DDR##port)

#define PN_PIN(x) PN_PIN_(x)
#define PN_PORT(x) PN_PORT_(x)
#define PN_DDR(x) PN_DDR_(x)






//список элементов в меню
//uses for TGlobalData_t.menu
typedef enum { MENU_SELECT, MENU_IRON, MENU_FEN, MENU_DREL } TMenuStates;

//действия
typedef enum { ACT_NONE, ACT_PUSH, ACT_PUSH_LONG, ACT_ROTATE_LEFT, ACT_ROTATE_RIGHT } TActions;

//кнопки и энкодер
typedef enum { NM_NONE, NM_BUTTON1, NM_BUTTON2, NM_BUTTON3, NM_BUTTON4,
                    NM_ENCBUTTON, NM_ENCROTATE, NM_BUTTON1_ENC } TActElements;


//комманда на выполнение
typedef struct {
    unsigned active: 1;
    TActElements name; //кто
    TActions action; //действие
} TActionCmd;



//uses for TGlobalData_t.update_screen
#define UPDATE_SCREEN_CLEAR 0xFF

#define UPDATE_SCREEN_MENU _BV(1)
#define UPDATE_SCREEN_VALS _BV(2)
#define UPDATE_SCREEN_FLASH _BV(3)

#define UPDATE_SCREEN_ALL UPDATE_SCREEN_MENU|UPDATE_SCREEN_VALS



struct TIron_t {
    unsigned on: 1; //вкл-выкл паяльника
    uint16_t adc; //последние значение с adc
    uint16_t temp; //текущая температура тены
    uint16_t temp_need; //требуемая температура

    uint16_t power; //расчетная мощность
    int32_t error; // iron_temp_need - iron_temp
    int32_t error_old; //предыдущее значение error
    int32_t integral; //сумма ошибок
    float power_tmp; //расчетное значение мощности
};
typedef struct TIron_t TIron;


struct TGlobalData_t {

    TMenuStates menu; //текущий пункт меню
    uint8_t update_screen; //обновить экран

    uint8_t temp; //для временного юзания

    TIron iron;


};
typedef struct TGlobalData_t TGlobalData;



volatile TGlobalData g_data;








/*
//отправить по уарту данные о стеке
#if 0
    #define CALC_STACK do { \
        uint8_t i; char buf[16]; \
        for(i = 0; i < PROCESS_COUNT; i++) { \
            uint8_t stack = get_proc(i)->stack_slack() * sizeof(stack_item_t); \
            sprintf(buf, "Pr:%d | S:%d\n", \
                scmRTOS_PRIORITY_ORDER == 0 ? i : PROCESS_COUNT - i, stack); \
            gUART.send(buf); \
        } } while(0)
#else
    #define CALC_STACK ;
#endif

*/



/*



//
struct TCurrStates {
    TMenuStates menu; //текущий пункт меню

    unsigned changed_menu: 1; //перерисовать меню

    uint8_t temp; //для временного юзания

    uint8_t phase; //счетчик полупериодов

    unsigned iron_on: 1; //вкл-выкл паяльника
    uint16_t iron_adc; //последние значение с adc
    uint16_t iron_temp; //текущая температура тены
    uint16_t iron_temp_need; //требуемая температура

    uint16_t iron_power; //расчетная мощность
    int32_t iron_error; // iron_temp_need - iron_temp
    int32_t iron_error_old; //предыдущее значение error
    int32_t iron_integral; //сумма ошибок
    float iron_power_tmp; //расчетное значение мощности


    unsigned fen_on: 1; //вкл-выкл паяльника
    unsigned fen_off: 1; //нужно отключить (для охлаждения до 50гр)
    uint16_t fen_adc; //последние значение с adc
    uint16_t fen_temp; //текущая температура тены
    uint16_t fen_temp_need; //требуемая температура
    uint16_t fen_pow; //расчетная мощность

    unsigned fan_on: 1; //вкл-выкл кулера в фене
    uint16_t fan_rpm; //текущие обороты
    uint16_t fan_rpm_need; //требуемые обороты
    uint16_t fan_pwm; //шим на кулер

    unsigned drel_on: 1; //вкл-выкл дрели
    uint16_t drel_adc; //последние значение с adc
    uint16_t drel_rpm; //текущие обороты
    uint16_t drel_rpm_need; //требуемые обороты
    uint16_t drel_pwm; //шим на дрель
};








//ISR(INT0_vect);
ISR(INT2_vect);


*/



#endif // COMMON_H
