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


#include <stdlib.h>
#include <math.h>

//--------------------

volatile pid_params pid;

static uint8_t yk;
static uint16_t xk_1; // PV[k-1] = Thlt[k-1]
static uint16_t xk_2; // PV[k-2] = Thlt[k-1]

#define IRON_PID_KC (30.0)
#define IRON_PID_TI (50.0)
#define IRON_PID_TD (IRON_PID_TI / 4.0)

#define IRON_PID_TS (IRON_PID_DELTA_T / 1000.0)

#define IRON_PID_K0 (IRON_PID_KC * IRON_PID_TS / IRON_PID_TI)
#define IRON_PID_K1 (IRON_PID_KC * IRON_PID_TD / IRON_PID_TS)

void init_pid4(void) {
/*------------------------------------------------------------------
Purpose : This function initialises the Allen Bradley Type A PID
controller.
Variables: p: pointer to struct containing all PID parameters
     Kc * Ts
k0 = ------- (for I-term)
        Ti
          Td
k1 = Kc * -- (for D-term)
          Ts

The LPF parameters are also initialised here:
lpf[k] = lpf1 * lpf[k-1] + lpf2 * lpf[k-2]
Returns : No values are returned
------------------------------------------------------------------*/
    pid.ts = IRON_PID_DELTA_T / 1000.0;

    if (pid.ti == 0.0) {
        pid.k0 = 0.0;
    }
    else {
        pid.k0 = pid.kc * pid.ts / pid.ti; //0.16
    }

    pid.k1 = pid.kc * pid.td / pid.ts; //150

} // init_pid3()


void pid_reg4(uint16_t xk, uint16_t tk) {
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
    int16_t ek;
    double pp, pi, pd;

    ek = (int16_t)(tk - xk);

    if(!xk_1 || !xk_2) {
        xk_1 = xk_2 = xk;
    }

    //-----------------------------------------------------------
    // Calculate PID controller:
    // y[k] = y[k-1] + Kc*(PV[k-1] - PV[k] +
    //        Ts*e[k]/Ti +
    //        Td/Ts*(2*PV[k-1] - PV[k] - PV[k-2]))
    //-----------------------------------------------------------

    pp = pid.kc * (1.0 * xk_1 - 1.0 * xk);
    pi = pid.k0 * ek;
    pd = pid.k1 * (2.0 * xk_1 - 1.0 * xk - 1.0 * xk_2);

    yk += pp + pi + pd;

    xk_2 = xk_1;
    xk_1 = xk;

    /*TPCPidInfo info;

    info.pp = pp;
    info.pi = pi;
    info.pd = pd;

    info.xk = xk;
    info.tk = tk;
    info.yk = yk;

    send_uart_msg(HI_PID, &info, sizeof(TPCPidInfo));
    */

    if (yk > POWER_MAX) {
        yk = POWER_MAX;
    }
    else
    if (yk < POWER_MIN) {
        yk = POWER_MIN;
    }

} // pid_reg4()


uint8_t pid_Controller(uint16_t temp_curr, uint16_t temp_need) {
    pid_reg4(temp_curr, temp_need);

    if(temp_curr < IRON_TEMP_SOFT && temp_curr < temp_need) {
        return POWER_MAX / 5;
    }

    return (uint8_t) ceil(yk);
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

    adc_read(ADC_PIN_IRON);

    g_data.iron.temp_need = IRON_TEMP_MIN;

    heater_iron_setpower(0);

    yk = 0;
    xk_1 = xk_2 = 0;

    if(pid.kc <= 0.001){
        pid.kc = 30;
        pid.ti = 0.001 * 50;
        pid.td = (0.001 * 50) / 4;
    }

    init_pid4();

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

            uint16_t temp = find_temp(adc, gIronTempZones, sizeof(gIronTempZones));

            if(abs(temp - iron->temp) > 1)
                ui_set_update_screen(UPDATE_SCREEN_VALS);

            iron->temp = temp;
        }

        uint8_t pow;

        pow = pid_Controller(iron->temp, iron->temp_need);

        if(pow != iron->power) {
            heater_iron_setpower(pow);
            ui_set_update_screen(UPDATE_SCREEN_VALS);
        }

        TPCTempInfo info;

        info.adc = iron->adc;
        info.temp = iron->temp;
        info.temp_need = iron->temp_need;
        info.power = iron->power;

        send_uart_msg(HI_IRON, &info, sizeof(TPCTempInfo));
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

        g_data.iron.sigma += (g_data.iron.power) + delta;
    }
}

ISR(TIMER1_COMPA_vect) {

    if(g_data.iron.on == _ON) {
        ON(P_IRON_PWM); //вкл симмистор
    }

    TCCR1B &= ~TIMER1A_PRESCALE_OFF; //выкл таймер 1
}
