#include "stdlib.h"
#include "common.h"
#include "hal.h"
#include "uart.h"
#include "ui.h"
#include "heater.h"

TTempZones gIronTempZones[] = {
    TZ_X(TZ_PXY0, TZ_PXY1),
    TZ_X(TZ_PXY1, TZ_PXY2),
    TZ_X(TZ_PXY2, TZ_PXY3),
    TZ_X(TZ_PXY3, TZ_PXY4),
    TZ_X(TZ_PXY4, TZ_PXY5),
    TZ_X(TZ_PXY5, TZ_PXY6),
};

TTempZones gFenTempZones[] = {
    TZ_X(TZ_FXY0, TZ_FXY1),
    TZ_X(TZ_FXY1, TZ_FXY2),
    TZ_X(TZ_FXY2, TZ_FXY3),
    TZ_X(TZ_FXY3, TZ_FXY4),
    TZ_X(TZ_FXY4, TZ_FXY5),
    TZ_X(TZ_FXY5, TZ_FXY6),
};

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



uint8_t pid_Controller(uint16_t xk, uint16_t tk, uint8_t power, volatile TPid *pid) {
/*------------------------------------------------------------------
Purpose : This function implements the Takahashi PID controller,
which is a type C controller: the P and D term are no
longer dependent on the set-point, only on PV (which is Thlt).
The D term is NOT low-pass filtered.
This function should be called once every TS seconds.
Variables:
xk : The input variable x[k] (= measured temperature)
*yk : The output variable y[k] (= gamma value for power electronics)
tset : The setpoint value for the temperature
*p : Pointer to struct containing PID parameters
vrg: Release signal: 1 = Start control, 0 = disable PID controller
Returns : No values are returned
------------------------------------------------------------------*/
    int16_t ek, yk, pp, pi, pd;

    ek = (int16_t)(tk - xk);

    //-----------------------------------------------------------
    //pid http://www.vandelogt.nl/htm/regelen_pid_uk.htm
    // Calculate PID controller:
    // y[k] = y[k-1] + Kc*(PV[k-1] - PV[k] +
    //        Ts*e[k]/Ti +
    //        Td/Ts*(2*PV[k-1] - PV[k] - PV[k-2]))
    //-----------------------------------------------------------

    pp = pid->kc * (pid->xk_1 - xk);
    pi = pid->k0 * ek;
    pd = pid->k1 * (2 * pid->xk_1 - xk - pid->xk_2);

    pid->xk_2 = pid->xk_1;
    pid->xk_1 = xk;

    yk = power + pp + pi + pd;

    if (yk > POWER_MAX) {
        yk = POWER_MAX;
    }
    else
    if (yk < POWER_MIN) {
        yk = POWER_MIN;
    }

    return yk & 0xFF;
}


void heater_setpower(uint8_t pow) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {

        if(g_data.menu == MENU_IRON)
            OFF(P_IRON_PWM);
        else
            OFF(P_FEN_PWM);

        g_data.heater->power = pow;
        g_data.heater->sigma = POWER_MAX;
    }
}

void heater_on(void) {
    if(g_data.menu == MENU_IRON)
        g_data.heater = &g_data.iron;
    else
    if(g_data.menu == MENU_FEN)
        g_data.heater = &g_data.fen;
    else
        return;

    adc_read(g_data.heater->def_adc_pin);

    g_data.heater->temp_need = g_data.heater->def_t_min;
    //g_data.heater->pid.xk_1 = g_data.heater->pid.xk_2 = 0;

    heater_setpower(0);

    g_data.heater->on = _ON;
}

void heater_off(void) {
    if(!g_data.heater) return;

    g_data.heater->on = _OFF;

    heater_setpower(0);

    g_data.heater = NULL;
}

PT_THREAD(iron_pt_manage(struct pt *pt)) {
    static TIMER_T timer;

    PT_BEGIN(pt);

    TIMER_INIT(timer, HEATER_PID_DELTA_T);
    for(;;) {
        PT_WAIT_UNTIL(pt, g_data.heater && g_data.heater->on == _ON && TIMER_ENDED(timer));
        TIMER_INIT(timer, HEATER_PID_DELTA_T);

        uint16_t adc = adc_read(g_data.heater->def_adc_pin);

        if(adc >= HEATER_ADC_ERROR) {
            heater_setpower(0);

            ui_set_update_screen(UPDATE_SCREEN_ERROR);
            continue;
        }

        if(adc >= HEATER_ADC_HOT) {
            heater_setpower(0);

            BEEP(1000); //beep and reset with watchdog
        }

        if(adc != g_data.heater->adc) {
            g_data.heater->adc = adc;

            uint16_t temp = find_temp(adc, g_data.heater->def_zones, sizeof(*g_data.heater->def_zones));

            if(abs(temp - g_data.heater->temp) > 1)
                ui_set_update_screen(UPDATE_SCREEN_VALS);

            g_data.heater->temp = temp;
        }

        uint8_t pow = pid_Controller(g_data.heater->temp, g_data.heater->temp_need, g_data.heater->power, &g_data.heater->pid);

        if(g_data.heater->temp < HEATER_TEMP_SOFT && g_data.heater->temp < g_data.heater->temp_need) {
            pow = POWER_MAX / 5;
        }

        if(pow != g_data.heater->power) {
            heater_setpower(pow);
            ui_set_update_screen(UPDATE_SCREEN_VALS);
        }

        TPCTempInfo info;

        info.adc = g_data.heater->adc;
        info.temp = g_data.heater->temp;
        info.temp_need = g_data.heater->temp_need;
        info.power = g_data.heater->power;

        send_uart_msg(HI_HEATER, &info, sizeof(TPCTempInfo));
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

    volatile THeater *heater;

    //начальные значения для паяльника
    heater = &g_data.iron;

    heater->def_adc_pin = ADC_PIN_IRON;
    heater->pid.kc = IRON_PID_KC;
    heater->pid.k0 = IRON_PID_K0;
    heater->pid.k1 = IRON_PID_K1;

    heater->def_t_min = IRON_TEMP_MIN;
    heater->def_t_max = IRON_TEMP_MAX;
    heater->def_t_step = IRON_TEMP_STEP;

    heater->def_zones = gIronTempZones;


    //начальные значения для фена
    heater = &g_data.fen;

    heater->def_adc_pin = ADC_PIN_FEN;
    heater->pid.kc = FEN_PID_KC;
    heater->pid.k0 = FEN_PID_K0;
    heater->pid.k1 = FEN_PID_K1;

    heater->def_t_min = FEN_TEMP_MIN;
    heater->def_t_max = FEN_TEMP_MAX;
    heater->def_t_step = FEN_TEMP_STEP;

    heater->def_zones = gFenTempZones;

    //неактивен
    g_data.heater = NULL;
}


//ZCD
ISR(INT2_vect) {

    if(!g_data.heater || g_data.heater->on == _OFF)
        return;

#if FULL_PERIOD == 1
    static uint8_t numperiod = 0;
    if(numperiod++ & 0b01) return;
#endif

     int8_t delta;

     if(g_data.heater->sigma > POWER_MAX) {
        delta = -POWER_MAX;

        TCNT1 = 0x00;
        TCCR1B |= TIMER1A_PRESCALE; //вкл таймер 1
    } else {
        delta = 0;

        if(g_data.menu == MENU_IRON)
            OFF(P_IRON_PWM);
        else
            OFF(P_FEN_PWM);
    }

    g_data.heater->sigma += g_data.heater->power + delta;
}

ISR(TIMER1_COMPA_vect) {

    if(g_data.heater && g_data.heater->on == _ON) {
        //вкл симмистор
        if(g_data.menu == MENU_IRON)
            ON(P_IRON_PWM);
        else
            ON(P_FEN_PWM);
    }

    TCCR1B &= ~TIMER1A_PRESCALE_OFF; //выкл таймер 1
}
