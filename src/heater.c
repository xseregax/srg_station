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

// These defines are needed for loop timing and PID controller timing
#define TWENTY_SECONDS (400)
#define TEN_SECONDS (200)
#define FIVE_SECONDS (100)
#define ONE_SECOND (20)
#define T_50MSEC (50) // Period time of TTimer in msec.

#define GMA_HLIM (100.0) // PID controller upper limit [%]
#define GMA_LLIM (0.0) // PID controller lower limit [%]



//--------------------

volatile pid_params pid;

static double xk_1; // PV[k-1] = Thlt[k-1]
static double xk_2; // PV[k-2] = Thlt[k-1]


void init_pid4(volatile pid_params *p) {
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

    //p->kc = 5;
    //p->ti = 0;
    //p->td = 0;

    p->ts = IRON_PID_DELTA_T / 1000.0;

    if (p->ti == 0.0) {
        p->k0 = 0.0;
    }
    else {
        p->k0 = p->kc * p->ts / p->ti;
    }

    p->k1 = p->kc * p->td / p->ts;

} // init_pid3()


void pid_reg4(double xk, double *yk, double tk, volatile pid_params *p, int vrg) {
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
    *yk = (double)g_data.iron.power;

    double ek, pp, pi, pd;
    ek = tk - xk; // calculate e[k] = SP[k] - PV[k]

    //-----------------------------------------------------------
    // Calculate PID controller:
    // y[k] = y[k-1] + Kc*(PV[k-1] - PV[k] +
    //        Ts*e[k]/Ti +
    //        Td/Ts*(2*PV[k-1] - PV[k] - PV[k-2]))
    //-----------------------------------------------------------

    if(vrg) {
        // y[k] = y[k-1] + Kc*(PV[k-1] - PV[k]) +
        pp = p->kc * (xk_1 - xk);

        // Kc*Ts/Ti * e[k] +
        pi = p->k0 * ek;

        //Kc*Td/Ts* (2*PV[k-1] - PV[k] - PV[k-2]))
        pd = p->k1 * (2.0 * xk_1 - xk - xk_2);

        *yk += pp + pi + pd;
    }
    else
        *yk = pp = pi = pd = 0;

    xk_2 = xk_1; // PV[k-2] = PV[k-1]
    xk_1 = xk;   // PV[k-1] = PV[k]

    if (*yk > GMA_HLIM) {
        *yk = GMA_HLIM;
    }
    else
    if (*yk < GMA_LLIM) {
        *yk = GMA_LLIM;
    }

} // pid_reg4()


uint8_t pid_Controller(uint16_t temp_need, uint16_t temp_curr) {
    double out = 0.0;

    pid_reg4((double)temp_curr, &out, (double)temp_need, &pid, g_data.iron.out1? 1: 0);

    if(temp_curr < IRON_TEMP_SOFT && temp_curr < temp_need) {
        return POWER_MAX / 5;
    }

    return (uint8_t) ceil(out);
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

    init_pid4(&pid);

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

        //if(abs(adc - iron->adc) > 1)
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
