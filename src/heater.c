#include "common.h"
#include "hal.h"
#include "uart.h"
#include "ui.h"
#include "heater.h"


TTempZones gIronTempZones[] = {
    TZ_X(TZ_XY0, TZ_XY1),
    TZ_X(TZ_XY1, TZ_XY2),
    TZ_X(TZ_XY2, TZ_XY3),
    TZ_X(TZ_XY3, TZ_XY4),
    TZ_X(TZ_XY4, TZ_XY5),
    TZ_X(TZ_XY5, TZ_XY6),
};


#include <util/crc16.h>
void send_uart_info(TPCInfo *info) {
    info->header = PCINFO_HEADER;

    uint8_t i, crc = 0;
    uint8_t *p = (uint8_t*)info;

    for(i = 0; i < sizeof(TPCInfo) - 1; i++)
        crc = _crc_ibutton_update(crc, p[i]);

    info->crc = crc;

    for(i = 0; i < sizeof(TPCInfo); i++)
        uart_send_b(p[i]);
}

//читалка adc c пина
uint16_t adc_read(uint8_t adc_pin)
{
    ADMUX = (ADMUX & 0b011111000) | adc_pin;
    _delay_us(125);

    ADCSRA |= _BV(ADSC);         // start single convertion
    loop_until_bit_is_set(ADCSRA, ADSC); // Wait for the AD conversion to complete

    uint16_t temp;

    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        temp = ADCW;
    }
    return temp;
}

//конверт adc в темп по калиброванным значениям
uint16_t find_temp(uint16_t adc, const TTempZones* tempzones, uint8_t count) {
    uint8_t i;
    for(i = 0; i < count; i++ ) {
        if(adc <= tempzones[i].y1)
            break;
    }

    uint16_t temp = ((adc - tempzones[i].y0) * tempzones[i].a) / TZ_AMUL +
            tempzones[i].x0;

    return temp;
}




#define IRON_PID_KP (1.0 * PID_SCALING)
#define IRON_PID_KI (0 * PID_SCALING)
#define IRON_PID_KD (0 * PID_SCALING)

#define IRON_PID_MAXI (POWER_MAX / 2)

#define IRON_PID_MAX_ERROR (POWER_MAX / (IRON_PID_KP + 1))
#define IRON_PID_MIN_ERROR 0

#define IRON_PID_MAX_SUMERROR (IRON_PID_MAXI / (IRON_PID_KI + 1))
#define IRON_PID_MIN_SUMERROR 0

volatile uint16_t pid_p = IRON_PID_KP;
volatile uint16_t pid_i = IRON_PID_KI;
volatile uint16_t pid_d = IRON_PID_KD;



volatile uint8_t send_stat = 1;

static volatile int16_t pre_error = 0;
static volatile int16_t integ = 0;

void pid_init(void) {
    pre_error = integ = 0;
}



#include <stdlib.h>
#include <math.h>
uint8_t pid_Controller(uint16_t temp_need, uint16_t temp_curr) {

    int16_t error, i_val, d_val, power;
    uint16_t p_val;

    error = (int16_t)(temp_need - temp_curr);

    if(error > IRON_PID_MAX_ERROR)
        p_val = POWER_MAX;
    else
    if(error <= IRON_PID_MIN_ERROR)
        p_val = POWER_MIN;
    else
        p_val = IRON_PID_KP * error;

    integ += error;

    if(integ > IRON_PID_MAX_SUMERROR) {
        integ = IRON_PID_MAX_SUMERROR;
        i_val = IRON_PID_MAXI;
    }
    else
    if(integ < -IRON_PID_MAX_SUMERROR) {
        integ = -IRON_PID_MAX_SUMERROR;
        i_val = -IRON_PID_MAXI;
    }
    else {
        i_val = IRON_PID_KI * integ;
    }


    d_val = IRON_PID_KD * (error - pre_error);

    pre_error = error;

    if(temp_curr < IRON_TEMP_SOFT && error > 0) {
        return POWER_MAX / 5;
    }

    power = (p_val + i_val + d_val) / PID_SCALING;

    if(power < POWER_MIN)
        power = POWER_MIN;
    else
    if(power > POWER_MAX) {
        power = POWER_MAX;
    }

    return power;
}

void heater_iron_setpower(uint8_t pow) {

    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        OFF(P_IRON_PWM);

        g_data.iron.power = pow;
        g_data.iron.sigma = POWER_MAX;
    }
}

void heater_iron_on(void) {
    memset((void*) &g_data.iron, 0, sizeof(TIron));

    g_data.iron.temp_need = IRON_TEMP_MIN;

    heater_iron_setpower(0);

    g_data.iron.on = _ON;
}

void heater_iron_off(void) {
    g_data.iron.on = _OFF;

    heater_iron_setpower(0);
}

void heater_fen_on(void) {
    //if(g_data.fen.on  == _ON) return;

}

void heater_fen_off(void) {
    //if(g_data.fen.on == _OFF) return;

}

PT_THREAD(iron_pt_manage(struct pt *pt)) {
    static TIMER_T timer;

    PT_BEGIN(pt);

    TIMER_INIT(timer, IRON_PID_DELTA_T);
    for(;;) {
        PT_WAIT_UNTIL(pt, g_data.iron.on == _ON && TIMER_ENDED(timer));
        TIMER_INIT(timer, IRON_PID_DELTA_T);

        volatile TIron *iron = &g_data.iron;

        uint16_t adc = adc_read(ADC_PIN_IRON);

        if(adc >= IRON_ADC_ERROR) {
            heater_iron_setpower(0);

            ui_set_update_screen(UPDATE_SCREEN_ERROR);
            continue;
        }

        if(adc >= IRON_ADC_HOT) {
            heater_iron_setpower(0);

            BEEP(1000); //beep and reset with watchdog
        }

        if(adc != iron->adc) {
            iron->adc = adc;

            iron->temp = find_temp(adc, gIronTempZones, sizeof(gIronTempZones));

            ui_set_update_screen(UPDATE_SCREEN_VALS);
        }

        uint8_t pow;

        pow = pid_Controller(iron->temp_need, iron->temp);

        if(pow != iron->power) {

            heater_iron_setpower(pow);
            ui_set_update_screen(UPDATE_SCREEN_VALS);
        }

        if(send_stat) {
            TPCInfo info;

            info.type = PCINFO_TYPE_IRON;

            info.value1 = iron->temp;
            info.value2 = iron->power;
            info.value3 = iron->temp_need;
            info.value4 = iron->adc;

            send_uart_info(&info);

        }
    }

    PT_END(pt);
}


PT_THREAD(fen_pt_manage(struct pt *pt)) {
    PT_BEGIN(pt);
    PT_END(pt);
}


PT_THREAD(heater_pt_manage(struct pt *pt)) {
    static struct pt pt_iron, pt_fen;

    PT_BEGIN(pt);

    PT_INIT(&pt_iron);
    PT_INIT(&pt_fen);

    PT_WAIT_THREAD(pt,
           iron_pt_manage(&pt_iron) &
           fen_pt_manage(&pt_fen)
          );

    PT_END(pt);
}


void heater_init_mod(void) {

    heater_iron_off();
    heater_fen_off();
}


//ZCD
ISR(INT2_vect) {

#if FULL_PERIOD == 1
    static uint8_t numperiod = 0;
    if(numperiod++ & 0b01) return;
#endif

    if(g_data.iron.on == _ON) {
         int8_t delta;

         if(g_data.iron.sigma > POWER_MAX) {
            delta = -POWER_MAX;

            TCNT1 = 0x00;
            TCCR1B |= TIMER1A_PRESCALE; //вкл таймер 1
        } else {
            delta = 0;

            OFF(P_IRON_PWM);
        }

        g_data.iron.sigma += g_data.iron.power + delta;
    }
}

ISR(TIMER1_COMPA_vect) {

    if(g_data.iron.on == _ON) {
        ON(P_IRON_PWM); //вкл симмистор
    }

    TCCR1B &= ~TIMER1A_PRESCALE_OFF; //выкл таймер 1
}
