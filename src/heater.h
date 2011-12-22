#ifndef IRON_H
#define IRON_H


#define SIMISTOR_TIME_ON 8

#define PID_STEP 500 //интервал измерений, PID_STEP (1 = 10msec (1/50HZ/2), based on ZCD)


//пид и фаза паяльника

#define SCALING_FACTOR 128

#define IRON_PID_KP (5 * SCALING_FACTOR)
#define IRON_PID_KI (0 * SCALING_FACTOR)
#define IRON_PID_KD (0 * SCALING_FACTOR)

#define IRON_PID_MIN 0
#define IRON_PID_MAX (100 * SCALING_FACTOR)

#define IRON_PID_MIN_ERROR 0
#define IRON_PID_MAX_ERROR (IRON_PID_MAX / (IRON_PID_KP + 1))

#define IRON_PID_IMAX (IRON_PID_MAX / 2)
#define IRON_PID_MAX_SUM_ERROR (IRON_PID_IMAX / (IRON_PID_KI + 1))

#define PHASE_IRON _BV(1)
#define PHASE_FEN _BV(2)
#define PHASE_ALL (PHASE_IRON | PHASE_FEN)

//паяльник
#define IRON_CHECK_TIME (5 * TIME_1MS)

#define IRON_TEMP_MIN 50 //мин температура
#define IRON_TEMP_MAX 450L //макс температура
#define IRON_TEMP_STEP 50 //шаг регулировки температуры

#define IRON_MIN_POWER 0
#define IRON_MAX_POWER 100




//апроксимация температуры по контрольным точкам
typedef struct {
    uint16_t y1;
    uint16_t x0;
    uint16_t y0;
    uint32_t a;
} TTempZones;

/*
данные калибровки
x - °C, y - adc
a = (x1 - x0) / (y1 - y0)
x = (y - y0) * a + x0
*/
//измеренные °C - ADC

#define TZ_XY0 0.0,   0.0
#define TZ_XY1 30.0,  375.0
#define TZ_XY2 50.0,  396.0
#define TZ_XY3 100.0, 440.0
#define TZ_XY4 150.0, 480.0
#define TZ_XY5 183.0, 509.0
#define TZ_XY6 500.0, 1024.0

#define TZ_AMUL 10000L //float to 32bit

#define TZ_X_(X0,Y0,X1,Y1) { (uint16_t)Y1, (uint16_t)X0, (uint16_t)Y0, (uint32_t)((((X1 - X0) / (Y1 - Y0))) * TZ_AMUL) }
#define TZ_X(x,y) TZ_X_(x,y)


void heater_iron_on(void);
void heater_iron_off(void);

void heater_init_mod(void);
PT_THREAD(heater_pt_manage(struct pt *pt));

#endif // IRON_H
