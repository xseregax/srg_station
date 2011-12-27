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



// These defines are needed for loop timing and PID controller timing
#define TWENTY_SECONDS (400)
#define TEN_SECONDS (200)
#define FIVE_SECONDS (100)
#define ONE_SECOND (20)
#define T_50MSEC (50) // Period time of TTimer in msec.

#define GMA_HLIM (100.0) // PID controller upper limit [%]
#define GMA_LLIM (0.0) // PID controller lower limit [%]


typedef struct _pid_params
{
    double kc; // Controller gain from Dialog Box
    double ti; // Time-constant for I action from Dialog Box
    double td; // Time-constant for D action from Dialog Box
    double ts; // Sample time [sec.] from Dialog Box
//    double k_lpf; // Time constant [sec.] for LPF filter
    double k0; // k0 value for PID controller
    double k1; // k1 value for PID controller
    double k2; // k2 value for PID controller
    double k3; // k3 value for PID controller
//    double lpf1; // value for LPF filter
//    double lpf2; // value for LPF filter
//    int    ts_ticks; // ticks for timer
//    int    pid_model; // PID Controller type [0..3]
    double pp; // debug
    double pi; // debug
    double pd; // debug
} pid_params; // struct pid_params
//--------------------

volatile pid_params pid;

static double ek_1; // e[k-1] = SP[k-1] - PV[k-1] = Tset_hlt[k-1] - Thlt[k-1]
static double ek_2; // e[k-2] = SP[k-2] - PV[k-2] = Tset_hlt[k-2] - Thlt[k-2]
static double xk_1; // PV[k-1] = Thlt[k-1]
static double xk_2; // PV[k-2] = Thlt[k-1]

static double tk_1; //SP[k-1] = Tset_hlt[k-1]
static double tk_2; //SP[k-2] = Tset_hlt[k-2]

static double yk_1; // y[k-1] = Gamma[k-1]
static double yk_2; // y[k-2] = Gamma[k-1]
static double lpf_1; // lpf[k-1] = LPF output[k-1]
static double lpf_2; // lpf[k-2] = LPF output[k-2]



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

    p->kc = 20;
    p->ti = 100;
    p->td = 0;

    p->ts = IRON_PID_DELTA_T;

/*
    p->ts_ticks = (int)(p->ts / T_50MSEC);
    if (p->ts_ticks > TWENTY_SECONDS) {
        p->ts_ticks = TWENTY_SECONDS;
    }
*/
    if (p->ti == 0.0) {
        p->k0 = 0.0;
    }
    else {
        p->k0 = p->kc * p->ts / p->ti;
    } // else

    p->k1 = p->kc * p->td / p->ts;
    //p->lpf1 = (2.0 * p->k_lpf - p->ts) / (2.0 * p->k_lpf + p->ts);
    //p->lpf2 = p->ts / (2.0 * p->k_lpf + p->ts);

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

    double ek; // e[k]

    ek = tk - xk; // calculate e[k] = SP[k] - PV[k]

    //-----------------------------------------------------------
    // Calculate PID controller:
    // y[k] = y[k-1] + Kc*(PV[k-1] - PV[k] +
    //        Ts*e[k]/Ti +
    //        Td/Ts*(2*PV[k-1] - PV[k] - PV[k-2]))
    //
    // y[k] = y[k-1] + Kc*(SP[k] - PV[k] - (SP[k-1] - PV[k-1]) +
    //        Ts*e[k]/Ti +
    //        Td/Ts*(SP[k] - PV[k] - 2*(SP[k-1] - PV[k-1]) + SP[k-2] - PV[k-2]))
    //-----------------------------------------------------------

    //set point temp not changed

    // y[k] = y[k-1] + Kc*(PV[k-1] - PV[k]) +
    p->pp = p->kc * (xk_1 - xk);

    // Kc*Ts/Ti * e[k] +
    p->pi = p->k0 * ek;

    //Kc*Td/Ts* (2*PV[k-1] - PV[k] - PV[k-2]))
    p->pd = p->k1 * (2.0 * xk_1 - xk - xk_2);

    *yk += p->pp + p->pi + p->pd;


    //set point temp changed
/*
    // y[k] = y[k-1] + Kc*(SP[k] - PV[k] - (SP[k-1] - PV[k-1])) +
    p->pp = p->kc * (tk - xk - (tk_1 - xk_1));

    // Kc*Ts/Ti * e[k] +
    p->pi = p->k0 * ek;

    //Kc*Td/Ts* (SP[k] - PV[k] - 2*(SP[k-1] - PV[k-1]) + SP[k-2] - PV[k-2]))
    p->pd = p->k1 * (tk - xk - 2.0 * (tk_1 - xk_1) + tk_2 - xk_2);

    *yk += p->pp + p->pi + p->pd;
*/

    xk_2 = xk_1; // PV[k-2] = PV[k-1]
    xk_1 = xk;   // PV[k-1] = PV[k]

  //  tk_2 = tk_1; // SP[k-2] = SP[k-1]
  //  tk_1 = tk; // SP[k-1] = SP[k]


    // limit y[k] to GMA_HLIM and GMA_LLIM
    if (*yk > GMA_HLIM) {
        *yk = GMA_HLIM;
    }
    else
    if (*yk < GMA_LLIM) {
        *yk = GMA_LLIM;
    } // else

} // pid_reg4()


uint8_t pid_Controller(uint16_t temp_need, uint16_t temp_curr) {
    double out = 0.0;

    pid_reg4((double)temp_curr, &out, (double)temp_need, &pid, 1);

    if(temp_curr < IRON_TEMP_SOFT && temp_curr < temp_need) {
        return POWER_MAX / 5;
    }

    return (uint8_t) ceil(out);
}



uint8_t pid_Controller1(uint16_t temp_need, uint16_t temp_curr) {

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
