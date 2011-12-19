#include "common.h"
#include "iron.h"

volatile uint8_t g_phase = 0;


//для пропуска периодов
PGM(uint16_t gPowerMas[]) = {
    5500,          //0% 5500 а не 5000 для надежного 0
    4684,          //1%
    4551,          //2%
    4448,          //3%
    4361,          //4%
    4284,          //5%
    4214,          //6%
    4150,          //7%
    4089,          //8%
    4032,          //9%
    3978,          //10%
    3926,          //11%
    3876,          //12%
    3828,          //13%
    3781,          //14%
    3736,          //15%
    3692,          //16%
    3649,          //17%
    3607,          //18%
    3566,          //19%
    3526,          //20%
    3487,          //21%
    3448,          //22%
    3410,          //23%
    3372,          //24%
    3335,          //25%
    3298,          //26%
    3262,          //27%
    3227,          //28%
    3191,          //29%
    3157,          //30%
    3122,          //31%
    3088,          //32%
    3054,          //33%
    3020,          //34%
    2986,          //35%
    2953,          //36%
    2920,          //37%
    2887,          //38%
    2854,          //39%
    2822,          //40%
    2789,          //41%
    2757,          //42%
    2725,          //43%
    2693,          //44%
    2661,          //45%
    2629,          //46%
    2597,          //47%
    2565,          //48%
    2533,          //49%
    2501,          //50%
    2469,          //51%
    2438,          //52%
    2406,          //53%
    2374,          //54%
    2342,          //55%
    2310,          //56%
    2278,          //57%
    2245,          //58%
    2213,          //59%
    2181,          //60%
    2148,          //61%
    2115,          //62%
    2082,          //63%
    2049,          //64%
    2016,          //65%
    1983,          //66%
    1949,          //67%
    1915,          //68%
    1881,          //69%
    1846,          //70%
    1811,          //71%
    1776,          //72%
    1740,          //73%
    1704,          //74%
    1668,          //75%
    1630,          //76%
    1593,          //77%
    1555,          //78%
    1516,          //79%
    1477,          //80%
    1436,          //81%
    1395,          //82%
    1353,          //83%
    1311,          //84%
    1267,          //85%
    1221,          //86%
    1175,          //87%
    1127,          //88%
    1077,          //89%
    1025,          //90%
    970,           //91%
    913,           //92%
    853,           //93%
    788,           //94%
    718,           //95%
    641,           //96%
    554,           //97%
    452,           //98%
    319,           //99%
    50             //100%  50 а не 0 для надежного открытия симистора
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
    _delay_us(125);

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

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        st = g_phase & flag;
    }

    return st;
}

PT_THREAD(iron_pt_manage(struct pt *pt)) {

    PT_BEGIN(pt);

    for(;;) {

        PT_WAIT_UNTIL(pt, g_data.iron.on && check_phase(PHASE_IRON));

        TIron *iron = &g_data.iron;

        uint16_t adc = adc_read(ADC_PIN_IRON);
        if(adc != iron->adc) {
            iron->adc = adc;
            iron->temp = find_temp(adc, gIronTempZones, sizeof(gIronTempZones));
        }

        TPid *pid = &iron->pid;

        pid.error = iron->temp_need - iron->temp;
        if(pid.power > IRON_MIN_POWER && pid->power < IRON_MAX_POWER)
            pid->integral += pid->error;

        pid->power_tmp =
                IRON_KP * pid->error +
                IRON_KI * pid->integral +
                IRON_KD * (pid->error - pid->error_old);

        pid->error_old = pid->error;

        if(pid->power_tmp < IRON_MIN_POWER) pid->power_tmp = IRON_MIN_POWER;
        if(pid->power_tmp > IRON_MAX_POWER) pid->power_tmp = IRON_MAX_POWER;

        pid->power = (uint16_t)pid->power_tmp;

        uint16_t tmp = pgm_read_word(&gPowerMas[pid->power]);

        OCR1AH = tmp >> 8;
        OCR1AL = tmp;
    }

    PT_END(pt);
}


void iron_init_mod(void) {

    g_data.iron.on = 0;
    g_data.iron.temp_need = IRON_TEMP_MIN;
}



//ZCD
ISR(INT2_vect) {
    static uint8_t phase = 0;

    if(++phase == PID_STEP) {
        phase = 0;

        g_phase |= PHASE_ALL;
    }
}
