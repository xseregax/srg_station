#ifndef COMMON_H
#define COMMON_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "pin_macros.h"
#include "pins.h"

//#include "hd44780.h"
//#include "pt.h"



/*
#include "actions.h"
#include "hal.h"
#include "input.h"
#include "iron.h"
#include "pid.h"
#include "ui.h"
*/

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
    Предделитель
    CS02 CS01 CS00
    000 - off, 001 - 1, 010 - 8, 011 - 64, 100 - 256, 101 - 1024
    110 - от ножки Т0 на переходе с 1 на 0
    111 - от ножки Т0 на переходе с 0 на 1
*/
/*
#define SYS_DIVIDER 8

#if SYS_DIVIDER == 64
    #define SYS_CLOCK (_BV(CS01) | _BV(CS00))
#elif SYS_DIVIDER == 8
    #define SYS_CLOCK (_BV(CS01))
#else
    #define SYS_CLOCK (_BV(CS00))
#endif

#define TIME_1MS ((F_CPU/SYS_DIVIDER/256) / 1000) //задержка для 1мс


//prescaler=16MHz/128 (125kHz)
#if F_CPU == 16000000L
#define ADC_PRESCALER (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0))
#elif F_CPU == 8000000L
#define ADC_PRESCALER (_BV(ADPS2) | _BV(ADPS0))
#elif F_CPU == 4000000L
#define ADC_PRESCALER (_BV(ADPS2))
#elif F_CPU == 2000000L
#define ADC_PRESCALER (_BV(ADPS1) | _BV(ADPS0))
#elif F_CPU == 1000000L
#define ADC_PRESCALER (_BV(ADPS1))
#else
#define ADC_PRESCALER (_BV(ADPS0))
#endif
*/


/*



#define BUTTON_SLEEP (20 * TIME_1MS) //опрос кнопок
#define BUTTON_DEBOUNCE (100 * TIME_1MS / BUTTON_SLEEP) //антидребезг
#define BUTTON_LONG (2000 * TIME_1MS / BUTTON_SLEEP) //долгое нажатие
#define BUTTON_REPEAT (500 * TIME_1MS / BUTTON_SLEEP) //автоповтор
#define BUTTON_RELEASE (500 * TIME_1MS / BUTTON_SLEEP) //отпустил

#define ENCODER_SLEEP (TIME_1MS) //опрос энкодера


#define MY_ACTIONS_SIZE 8L //кол-во комманд в очереди

#define PID_STEP 10 //интервал измерений, 20msec

//паяльник
#define IRON_TEMP_MIN 150 //мин температура
#define IRON_TEMP_MAX 350 //макс температура
#define IRON_TEMP_STEP 5 //шаг регулировки температуры

#define IRON_MIN_POWER 0
#define IRON_MAX_POWER 100


//пид и фаза паяльника
#define IRON_KP 10
#define IRON_KI 0.3 * PID_STEP
#define IRON_KD 0.1 / PID_STEP

//фен
#define FEN_TEMP_MIN 150 //мин температура
#define FEN_TEMP_MAX 350 //макс температура
#define FEN_TEMP_STEP 5 //шаг регулировки температуры




//
//      Process types
//
typedef OS::process<OS::pr0, 120> TProc0;
typedef OS::process<OS::pr1, 120> TProcButtons; //buttons
typedef OS::process<OS::pr2, 120> TProcActions; //actions
typedef OS::process<OS::pr3, 150> TProcGui; //lcd, leds

typedef OS::process<OS::pr4, 150> TProcTimers; //timers
typedef OS::process<OS::pr5, 60> TProcUart; //uart cmd

//вкл или выкл станции
enum TPowerState { PWR_ON, PWR_OFF };

//список элементов в меню
enum TMenuStates { MENU_SELECT, MENU_IRON, MENU_FEN, MENU_DREL };

//кнопки и энкодер
enum TActElements { NM_NONE, NM_BUTTON1, NM_BUTTON2, NM_BUTTON3, NM_BUTTON4,
                    NM_ENCBUTTON, NM_ENCROTATE, NM_BUTTON1_ENC };

//действия
enum TActions { ACT_NONE, ACT_PUSH, ACT_PUSH_LONG, ACT_ROTATE_LEFT, ACT_ROTATE_RIGHT };

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


//для опроса кнопок
struct TButtonState {
    TActElements name; //какая кнопка
    unsigned on: 1; //заюзана
    unsigned plong: 1; //долго заюзана

    uint8_t cnt; //счетчик
    uint8_t repeat; //для автоповтора
    uint8_t release;//для сброса нажатия
};

//для очереди комманд на выполнение
struct TActionCmd {
    TActElements name; //кто
    TActions action; //действие
};

//апроксимация температуры по контрольным точкам
struct TTempZones {
    uint16_t y1;
    uint16_t x0;
    uint16_t y0;
    uint32_t a;
};



//ISR(INT0_vect);
ISR(INT2_vect);


*/



#endif // COMMON_H
