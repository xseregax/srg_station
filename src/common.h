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

#include "ul_rbuf.h"
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


#define AVR_RESET do { cli(); wdt_reset(); wdt_enable( WDTO_15MS ); while(1); } while (0)

#define _ON 1
#define _OFF 0


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


typedef struct {
    unsigned on: 1; //вкл-выкл паяльника

    uint16_t adc; //последние значение с adc

    uint16_t power; //текущая мощность
    uint16_t temp; //текущая температура тены
    uint16_t temp_need; //требуемая температура

    uint16_t out1;
} TIron;


typedef struct {
    uint8_t temp; //для временного юзания

    TIron iron;

} TGlobalData;



extern volatile TGlobalData g_data;


#endif // COMMON_H
