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

uint8_t check_phase(uint8_t flag) {
    uint8_t st;

    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        st = g_phase & flag;
        g_phase &= ~ flag;
    }

    return st;
}

void pid_Init(int16_t p_factor, int16_t i_factor, int16_t d_factor, volatile TPid *pid)
// Set up PID controller parameters
{
  // Start values for PID controller
  pid->sumError = 0;
  pid->lastProcessValue = 0;
  // Tuning constants for PID loop
  pid->P_Factor = p_factor;
  pid->I_Factor = i_factor;
  pid->D_Factor = d_factor;
  // Limits to avoid overflow
  pid->maxError = MAX_INT / (pid->P_Factor + 1);
  pid->maxSumError = MAX_I_TERM / (pid->I_Factor + 1);
}

int16_t pid_Controller(int16_t setPoint, int16_t processValue, volatile TPid *pid_st)
{
  int16_t error, p_term, d_term;
  int32_t i_term, ret, temp;

  error = setPoint - processValue;

  // Calculate Pterm and limit error overflow
  if (error > pid_st->maxError){
    p_term = MAX_INT;
  }
  else if (error < -pid_st->maxError){
    p_term = -MAX_INT;
  }
  else{
    p_term = pid_st->P_Factor * error;
  }

  // Calculate Iterm and limit integral runaway
  temp = pid_st->sumError + error;
  if(temp > pid_st->maxSumError){
    i_term = MAX_I_TERM;
    pid_st->sumError = pid_st->maxSumError;
  }
  else if(temp < -pid_st->maxSumError){
    i_term = -MAX_I_TERM;
    pid_st->sumError = -pid_st->maxSumError;
  }
  else{
    pid_st->sumError = temp;
    i_term = pid_st->I_Factor * pid_st->sumError;
  }

  // Calculate Dterm
  d_term = pid_st->D_Factor * (pid_st->lastProcessValue - processValue);

  pid_st->lastProcessValue = processValue;

  ret = (p_term + i_term + d_term) / SCALING_FACTOR;
  if(ret > MAX_INT){
    ret = MAX_INT;
  }
  else if(ret < -MAX_INT){
    ret = -MAX_INT;
  }

  return((int16_t)ret);
}

PT_THREAD(iron_pt_manage(struct pt *pt)) {
    static TIMER_T timer;

    PT_BEGIN(pt);

    TIMER_INIT(timer, SCALING_FACTOR);
    for(;;) {
        PT_WAIT_UNTIL(pt, g_data.iron.on && TIMER_ENDED(timer));
        TIMER_INIT(timer, SCALING_FACTOR);

        //PT_WAIT_UNTIL(pt, g_data.iron.on && check_phase(PHASE_IRON));

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

        if(!iron->pid.init)
            pid_Init(IRON_KP * SCALING_FACTOR, IRON_KI * SCALING_FACTOR , IRON_KD * SCALING_FACTOR, &iron->pid);

        int16_t pow = pid_Controller(iron->temp_need, iron->temp, &iron->pid);

        float x = (100.0 / UINT16_MAX);
        uint16_t pow2 =  x * (pow + MAX_INT) ;

        if(pow2 <= IRON_MIN_POWER) pow2 = IRON_MIN_POWER;
        if(pow2 > IRON_MAX_POWER) pow2 = IRON_MAX_POWER;

        iron->pid.power = pow2;

        ATOMIC_BLOCK(ATOMIC_FORCEON) {
            OCR1A = pgm_read_word(&gPowerMas[iron->pid.power]);
        }


/*
        volatile TPid *pid = &iron->pid;

        pid->error = iron->temp_need - iron->temp;

        pid->integral += pid->error;
        if(pid->integral > IRON_PID_IMAX)
            pid->integral = IRON_PID_IMAX;
        else
            if(pid->integral < IRON_PID_IMIN)
                pid->integral = IRON_PID_IMIN;

        pid->power_tmp =
                IRON_KP * pid->error +
                IRON_KI * pid->integral -
                IRON_KD * (pid->error - pid->error_old);

        pid->error_old = pid->error;

        if(pid->power_tmp < IRON_MIN_POWER) pid->power_tmp = IRON_MIN_POWER;
        if(pid->power_tmp > IRON_MAX_POWER) pid->power_tmp = IRON_MAX_POWER;

        pid->power = (uint16_t)pid->power_tmp;

        ATOMIC_BLOCK(ATOMIC_FORCEON) {
            OCR1A = pgm_read_word(&gPowerMas[pid->power]);
        }
*/
        g_data.update_screen |= UPDATE_SCREEN_VALS;

    }

    PT_END(pt);
}


void iron_init_mod(void) {

    g_data.iron.on = 0;
    g_data.iron.temp_need = IRON_TEMP_MIN;

    g_data.iron.pid.power = 0;
}



//ZCD
ISR(INT2_vect) {
    static uint8_t phase = 0;

    if(g_data.iron.on) {
        TCNT1 = 0x00;
        TCCR1B |= TIMER1A_PRESCALE; //вкл таймер 1
    }

    if(++phase == PID_STEP) {
        phase = 0;

        g_phase |= PHASE_ALL;
    }
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
