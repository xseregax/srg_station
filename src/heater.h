#ifndef IRON_H
#define IRON_H


#define SIMISTOR_TIME_ON 8 //время для вкл симистора, ms

#define PID_STEP 500L //интервал измерений, ms


//пид и фаза паяльника

#define SCALING_FACTOR 128L

#define IRON_ERROR_ADC 800L //не подключено термосопротивление

#define IRON_PID_KP (5 * SCALING_FACTOR)
#define IRON_PID_KI (0 * SCALING_FACTOR * (PID_STEP / 1000))
#define IRON_PID_KD (0 * SCALING_FACTOR / (PID_STEP / 1000))

#define IRON_PID_MIN 0
#define IRON_PID_MAX (100 * SCALING_FACTOR)

#define IRON_PID_MIN_ERROR 0
#define IRON_PID_MAX_ERROR (IRON_PID_MAX / (IRON_PID_KP + 1))

#define IRON_PID_IMAX (IRON_PID_MAX / 2)
#define IRON_PID_MAX_SUM_ERROR (IRON_PID_IMAX / (IRON_PID_KI + 1))

#define IRON_TEMP_SOFT 50 //температура, до которой нагрев идет в 50% мощности, гр
#define IRON_TEMP_MIN 50 //мин температура, гр
#define IRON_TEMP_MAX 450L //макс температура, гр
#define IRON_TEMP_STEP 50 //шаг регулировки температуры, гр


//апроксимация температуры по контрольным точкам
typedef struct {
    uint16_t y1; //adc
    uint16_t x0; //гр
    uint16_t y0; //adc
    uint32_t a;  //
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

#define TZ_AMUL _BV(14) //float to 32bit

#define TZ_X_(X0,Y0,X1,Y1) { (uint16_t)Y1, (uint16_t)X0, (uint16_t)Y0, (uint32_t)((((X1 - X0) / (Y1 - Y0))) * TZ_AMUL) }
#define TZ_X(x,y) TZ_X_(x,y)


void heater_iron_on(void);
void heater_iron_off(void);

void heater_fen_on(void);
void heater_fen_off(void);

void heater_init_mod(void);
PT_THREAD(heater_pt_manage(struct pt *pt));

#endif // IRON_H
