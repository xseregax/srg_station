#include "common.h"
#include "hal.h"
#include "uart.h"
#include "ui.h"
#include "heater.h"

typedef struct {
    uint8_t header;

    uint8_t type;
    uint16_t value1;
    uint16_t value2;
    uint16_t value3;
    uint16_t value4;

    uint8_t crc;
} TPCInfo;

#define PCINFO_HEADER 0xDE
#define PCINFO_TYPE_IRON 0x01

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



//для регулирования мощности
PGM(uint16_t gPowerMas[]) = {
    0,	//0%, 0ms, 0gr
    2342,	//1%, 9.368ms, 168.624gr
    2276,	//2%, 9.102ms, 163.836gr
    2224,	//3%, 8.896ms, 160.128gr
    2181,	//4%, 8.722ms, 156.996gr
    2142,	//5%, 8.568ms, 154.224gr
    2107,	//6%, 8.428ms, 151.704gr
    2075,	//7%, 8.3ms, 149.4gr
    2045,	//8%, 8.178ms, 147.204gr
    2016,	//9%, 8.064ms, 145.152gr
    1989,	//10%, 7.956ms, 143.208gr
    1963,	//11%, 7.852ms, 141.336gr
    1938,	//12%, 7.752ms, 139.536gr
    1914,	//13%, 7.656ms, 137.808gr
    1891,	//14%, 7.562ms, 136.116gr
    1868,	//15%, 7.472ms, 134.496gr
    1846,	//16%, 7.384ms, 132.912gr
    1825,	//17%, 7.298ms, 131.364gr
    1804,	//18%, 7.214ms, 129.852gr
    1783,	//19%, 7.132ms, 128.376gr
    1763,	//20%, 7.052ms, 126.936gr
    1744,	//21%, 6.974ms, 125.532gr
    1724,	//22%, 6.896ms, 124.128gr
    1705,	//23%, 6.82ms, 122.76gr
    1686,	//24%, 6.744ms, 121.392gr
    1668,	//25%, 6.67ms, 120.06gr
    1649,	//26%, 6.596ms, 118.728gr
    1631,	//27%, 6.524ms, 117.432gr
    1614,	//28%, 6.454ms, 116.172gr
    1596,	//29%, 6.382ms, 114.876gr
    1579,	//30%, 6.314ms, 113.652gr
    1561,	//31%, 6.244ms, 112.392gr
    1544,	//32%, 6.176ms, 111.168gr
    1527,	//33%, 6.108ms, 109.944gr
    1510,	//34%, 6.04ms, 108.72gr
    1493,	//35%, 5.972ms, 107.496gr
    1477,	//36%, 5.906ms, 106.308gr
    1460,	//37%, 5.84ms, 105.12gr
    1444,	//38%, 5.774ms, 103.932gr
    1427,	//39%, 5.708ms, 102.744gr
    1411,	//40%, 5.644ms, 101.592gr
    1395,	//41%, 5.578ms, 100.404gr
    1379,	//42%, 5.514ms, 99.252gr
    1363,	//43%, 5.45ms, 98.1gr
    1347,	//44%, 5.386ms, 96.948gr
    1331,	//45%, 5.322ms, 95.796gr
    1315,	//46%, 5.258ms, 94.644gr
    1299,	//47%, 5.194ms, 93.492gr
    1283,	//48%, 5.13ms, 92.34gr
    1267,	//49%, 5.066ms, 91.188gr
    1251,	//50%, 5.002ms, 90.036gr
    1235,	//51%, 4.938ms, 88.884gr
    1219,	//52%, 4.876ms, 87.768gr
    1203,	//53%, 4.812ms, 86.616gr
    1187,	//54%, 4.748ms, 85.464gr
    1171,	//55%, 4.684ms, 84.312gr
    1155,	//56%, 4.62ms, 83.16gr
    1139,	//57%, 4.556ms, 82.008gr
    1123,	//58%, 4.49ms, 80.82gr
    1107,	//59%, 4.426ms, 79.668gr
    1091,	//60%, 4.362ms, 78.516gr
    1074,	//61%, 4.296ms, 77.328gr
    1058,	//62%, 4.23ms, 76.14gr
    1041,	//63%, 4.164ms, 74.952gr
    1025,	//64%, 4.098ms, 73.764gr
    1008,	//65%, 4.032ms, 72.576gr
    992,	//66%, 3.966ms, 71.388gr
    975,	//67%, 3.898ms, 70.164gr
    958,	//68%, 3.83ms, 68.94gr
    941,	//69%, 3.762ms, 67.716gr
    923,	//70%, 3.692ms, 66.456gr
    906,	//71%, 3.622ms, 65.196gr
    888,	//72%, 3.552ms, 63.936gr
    870,	//73%, 3.48ms, 62.64gr
    852,	//74%, 3.408ms, 61.344gr
    834,	//75%, 3.336ms, 60.048gr
    815,	//76%, 3.26ms, 58.68gr
    797,	//77%, 3.186ms, 57.348gr
    778,	//78%, 3.11ms, 55.98gr
    758,	//79%, 3.032ms, 54.576gr
    739,	//80%, 2.954ms, 53.172gr
    718,	//81%, 2.872ms, 51.696gr
    698,	//82%, 2.79ms, 50.22gr
    677,	//83%, 2.706ms, 48.708gr
    656,	//84%, 2.622ms, 47.196gr
    634,	//85%, 2.534ms, 45.612gr
    611,	//86%, 2.442ms, 43.956gr
    588,	//87%, 2.35ms, 42.3gr
    564,	//88%, 2.254ms, 40.572gr
    539,	//89%, 2.154ms, 38.772gr
    513,	//90%, 2.05ms, 36.9gr
    485,	//91%, 1.94ms, 34.92gr
    457,	//92%, 1.826ms, 32.868gr
    427,	//93%, 1.706ms, 30.708gr
    394,	//94%, 1.576ms, 28.368gr
    359,	//95%, 1.436ms, 25.848gr
    321,	//96%, 1.282ms, 23.076gr
    277,	//97%, 1.108ms, 19.944gr
    226,	//98%, 0.904ms, 16.272gr
    160,	//99%, 0.638ms, 11.484gr
    1,	//100%, 0.002ms, 0.036gr
};

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
    _delay_us(10);

    ADCSRA |= _BV(ADSC);         // start single convertion
    loop_until_bit_is_set(ADCSRA,ADSC); // Wait for the AD conversion to complete

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

    uint16_t temp = ((adc - tempzones[i].y0) * tempzones[i].a) / TZ_AMUL + tempzones[i].x0;

    return temp;
}

#include <stdlib.h>
#include <math.h>
uint16_t pid_Controller(uint16_t temp_need, uint16_t temp_curr) {
    static float pre_error = 0;
    static float integral = 0;

    float error, deriv, out;

    error = temp_need - temp_curr;

    if(abs(error) > 0.01)
        integral += error * (IRON_PID_DELTA_T / 1000.0);

    deriv = (error - pre_error) / (IRON_PID_DELTA_T / 1000.0);

    out = IRON_PID_KP * error + IRON_PID_KI * integral + IRON_PID_KD * deriv;

    pre_error = error;

    if(out < IRON_PID_MIN)
        out = IRON_PID_MIN;
    else
    if(out > IRON_PID_MAX) {
        out = IRON_PID_MAX;
    }

    return ceil(out);
}

void heater_iron_setpower(uint16_t pow) {
    g_data.iron.power = pow;

    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        OCR1A = pgm_read_word(&gPowerMas[pow]);
    }
}

void heater_iron_on(void) {
    if(g_data.iron.on == _ON) return;

    memset((void*) &g_data.iron, 0, sizeof(TIron));

    g_data.iron.temp_need = IRON_TEMP_MIN;

    g_data.iron.on = _ON;
}

void heater_iron_off(void) {
    if(g_data.iron.on == _OFF) return;

    heater_iron_setpower(0);

    g_data.iron.on = _OFF;
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

        if(adc != iron->adc) {
            iron->adc = adc;

            iron->temp = find_temp(adc, gIronTempZones, sizeof(gIronTempZones));
            ui_set_update_screen(UPDATE_SCREEN_VALS);
        }

        uint16_t pow;

        if(iron->temp < IRON_TEMP_SOFT && iron->temp < iron->temp_need) {
            pow = IRON_PID_MAX / 4;
        }
        else
            pow = pid_Controller(iron->temp_need, iron->temp);

        if(pow != iron->power) {

            heater_iron_setpower(pow);

            ui_set_update_screen(UPDATE_SCREEN_VALS);
        }


        if(g_ui_update_screen & UPDATE_SCREEN_VALS) {
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
}



//ZCD
ISR(INT2_vect) {

    if(g_data.iron.on == _ON) {
        TCNT1 = 0x00;
        TCCR1B |= TIMER1A_PRESCALE; //вкл таймер 1
    }
}

ISR(TIMER1_COMPA_vect) {

    //выкл таймер 1
    TCCR1B &= ~TIMER1_PRESCALE_OFF;

    if(g_data.iron.on == _ON) {
       ON(P_IRON_PWM);
       _delay_us(SIMISTOR_TIME_ON);
       OFF(P_IRON_PWM);
    }
}
