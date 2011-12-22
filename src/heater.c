#include "common.h"
#include "hal.h"
#include "heater.h"

volatile uint8_t g_phase = 0;

//F_CPU / Timer1 rescaler / 500kHz base table
#define RECAL_TIME (F_CPU / 8 / 500000L)

//для пропуска периодов
PGM(uint16_t gPowerMas[]) = {
    0 * RECAL_TIME,          //0% 5500 а не 5000 для надежного 0
    4684 * RECAL_TIME,          //1%
    4551 * RECAL_TIME,          //2%
    4448 * RECAL_TIME,          //3%
    4361 * RECAL_TIME,          //4%
    4284 * RECAL_TIME,          //5%
    4214 * RECAL_TIME,          //6%
    4150 * RECAL_TIME,          //7%
    4089 * RECAL_TIME,          //8%
    4032 * RECAL_TIME,          //9%
    3978 * RECAL_TIME,          //10%
    3926 * RECAL_TIME,          //11%
    3876 * RECAL_TIME,          //12%
    3828 * RECAL_TIME,          //13%
    3781 * RECAL_TIME,          //14%
    3736 * RECAL_TIME,          //15%
    3692 * RECAL_TIME,          //16%
    3649 * RECAL_TIME,          //17%
    3607 * RECAL_TIME,          //18%
    3566 * RECAL_TIME,          //19%
    3526 * RECAL_TIME,          //20%
    3487 * RECAL_TIME,          //21%
    3448 * RECAL_TIME,          //22%
    3410 * RECAL_TIME,          //23%
    3372 * RECAL_TIME,          //24%
    3335 * RECAL_TIME,          //25%
    3298 * RECAL_TIME,          //26%
    3262 * RECAL_TIME,          //27%
    3227 * RECAL_TIME,          //28%
    3191 * RECAL_TIME,          //29%
    3157 * RECAL_TIME,          //30%
    3122 * RECAL_TIME,          //31%
    3088 * RECAL_TIME,          //32%
    3054 * RECAL_TIME,          //33%
    3020 * RECAL_TIME,          //34%
    2986 * RECAL_TIME,          //35%
    2953 * RECAL_TIME,          //36%
    2920 * RECAL_TIME,          //37%
    2887 * RECAL_TIME,          //38%
    2854 * RECAL_TIME,          //39%
    2822 * RECAL_TIME,          //40%
    2789 * RECAL_TIME,          //41%
    2757 * RECAL_TIME,          //42%
    2725 * RECAL_TIME,          //43%
    2693 * RECAL_TIME,          //44%
    2661 * RECAL_TIME,          //45%
    2629 * RECAL_TIME,          //46%
    2597 * RECAL_TIME,          //47%
    2565 * RECAL_TIME,          //48%
    2533 * RECAL_TIME,          //49%
    2501 * RECAL_TIME,          //50%
    2469 * RECAL_TIME,          //51%
    2438 * RECAL_TIME,          //52%
    2406 * RECAL_TIME,          //53%
    2374 * RECAL_TIME,          //54%
    2342 * RECAL_TIME,          //55%
    2310 * RECAL_TIME,          //56%
    2278 * RECAL_TIME,          //57%
    2245 * RECAL_TIME,          //58%
    2213 * RECAL_TIME,          //59%
    2181 * RECAL_TIME,          //60%
    2148 * RECAL_TIME,          //61%
    2115 * RECAL_TIME,          //62%
    2082 * RECAL_TIME,          //63%
    2049 * RECAL_TIME,          //64%
    2016 * RECAL_TIME,          //65%
    1983 * RECAL_TIME,          //66%
    1949 * RECAL_TIME,          //67%
    1915 * RECAL_TIME,          //68%
    1881 * RECAL_TIME,          //69%
    1846 * RECAL_TIME,          //70%
    1811 * RECAL_TIME,          //71%
    1776 * RECAL_TIME,          //72%
    1740 * RECAL_TIME,          //73%
    1704 * RECAL_TIME,          //74%
    1668 * RECAL_TIME,          //75%
    1630 * RECAL_TIME,          //76%
    1593 * RECAL_TIME,          //77%
    1555 * RECAL_TIME,          //78%
    1516 * RECAL_TIME,          //79%
    1477 * RECAL_TIME,          //80%
    1436 * RECAL_TIME,          //81%
    1395 * RECAL_TIME,          //82%
    1353 * RECAL_TIME,          //83%
    1311 * RECAL_TIME,          //84%
    1267 * RECAL_TIME,          //85%
    1221 * RECAL_TIME,          //86%
    1175 * RECAL_TIME,          //87%
    1127 * RECAL_TIME,          //88%
    1077 * RECAL_TIME,          //89%
    1025 * RECAL_TIME,          //90%
    970 * RECAL_TIME,           //91%
    913 * RECAL_TIME,           //92%
    853 * RECAL_TIME,           //93%
    788 * RECAL_TIME,           //94%
    718 * RECAL_TIME,           //95%
    641 * RECAL_TIME,           //96%
    554 * RECAL_TIME,           //97%
    452 * RECAL_TIME,           //98%
    319 * RECAL_TIME,           //99%
    1 * RECAL_TIME             //100%  50 а не 0 для надежного открытия симистора
};

PGM(TTempZones gIronTempZones[]) = {
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
    _delay_us(10);

    ADCSRA |= _BV(ADSC);         // start single convertion
    loop_until_bit_is_set(ADCSRA,ADSC); // Wait for the AD conversion to complete

    return ADCW;
}

//конверт adc в темп по калиброванным значениям
uint16_t find_temp(uint16_t adc, const TTempZones* tempzones, uint8_t count) {
    uint8_t i;
    for(i = 0; i < count; i++ ) {
        if(adc <= pgm_read_word(&tempzones[i].y1))
            break;
    }

    uint16_t temp = ((adc - pgm_read_word(&tempzones[i].y0)) * pgm_read_dword(&tempzones[i].a)) / TZ_AMUL +
            pgm_read_word(&tempzones[i].x0);

    return temp;
}
/*
uint8_t check_phase(uint8_t flag) {
    uint8_t st;

    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        st = g_phase & flag;
        g_phase &= ~ flag;
    }

    return st;
}
*/

int16_t pid_Controller(uint16_t temp_need, uint16_t temp_curr, volatile TPid *pid_st) {

    uint16_t ret, p_term;
    int16_t error, i_term, temp, d_term;

    error = temp_need - temp_curr;

    if (error >= IRON_PID_MAX_ERROR) {
        p_term = IRON_PID_MAX;
    }
    else if (error <= IRON_PID_MIN) {
        p_term = IRON_PID_MIN;
    }
    else {
        p_term = IRON_PID_KP * error;
    }

    temp = pid_st->error_sum + error;

    if(temp > IRON_PID_MAX_SUM_ERROR) {
        i_term = IRON_PID_IMAX;

        pid_st->error_sum = IRON_PID_MAX_SUM_ERROR;
    }
    else if(temp < -IRON_PID_MAX_SUM_ERROR) {
        i_term = -IRON_PID_IMAX;

        pid_st->error_sum = -IRON_PID_MAX_SUM_ERROR;
    }
    else {
        pid_st->error_sum = temp;

        i_term = IRON_PID_KI * pid_st->error_sum;
    }

    d_term = IRON_PID_KD * (pid_st->temp_last - temp_curr);

    pid_st->temp_last = temp_curr;

    ret = p_term + i_term + d_term;

    if(ret > IRON_PID_MAX) {
        ret = IRON_PID_MAX;
    }

    return ret / SCALING_FACTOR;
}


void heater_iron_on(void) {
    memset((void*) &g_data.iron, 0, sizeof(TIron));

    g_data.iron.temp_need = IRON_TEMP_MIN;

    g_data.iron.on = 1;
}

void heater_iron_off(void) {
    g_data.iron.on = 0;
}

PT_THREAD(iron_pt_manage(struct pt *pt)) {
    static TIMER_T timer;

    PT_BEGIN(pt);

    TIMER_INIT(timer, PID_STEP);
    for(;;) {
        PT_WAIT_UNTIL(pt, g_data.iron.on && TIMER_ENDED(timer));
        TIMER_INIT(timer, PID_STEP);

        volatile TIron *iron = &g_data.iron;

        uint16_t adc = adc_read(ADC_PIN_IRON);
        if(1 || adc != iron->adc) {
            iron->adc = adc;

            if(iron->temp < iron->temp_need) iron->temp++;
            else
            if(iron->temp > iron->temp_need) iron->temp--;

            //iron->temp = find_temp(adc, gIronTempZones, sizeof(gIronTempZones));

            g_data.update_screen |= UPDATE_SCREEN_VALS;
        }

        uint16_t pow = pid_Controller(iron->temp_need, iron->temp, &iron->pid);

        if(pow != iron->power) {

            ATOMIC_BLOCK(ATOMIC_FORCEON) {
                OCR1A = pgm_read_word(&gPowerMas[pow]);
            }

            iron->power = pow;

            g_data.update_screen |= UPDATE_SCREEN_VALS;
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
}



//ZCD
ISR(INT2_vect) {
    //static uint8_t phase = 0;

    if(g_data.iron.on) {
        TCNT1 = 0x00;
        TCCR1B |= TIMER1A_PRESCALE; //вкл таймер 1
    }

    /*if(++phase == 10) {
        phase = 0;

        g_phase |= PHASE_ALL;
    }*/
}

ISR(TIMER1_COMPA_vect) {

    //выкл таймер 1
    TCCR1B &= ~TIMER1_PRESCALE_OFF;

    if(g_data.iron.on) {
       ON(P_IRON_PWM);
       _delay_us(SIMISTOR_TIME_ON);
       OFF(P_IRON_PWM);
    }
}
