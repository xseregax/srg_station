#ifndef IRON_H
#define IRON_H


#define SIMISTOR_TIME_ON 8 //время для вкл симистора, ms

//пид и фаза паяльника

#define IRON_ADC_ERROR 800L //не подключено термосопротивление

#define IRON_PID_DELTA_T 100

#define IRON_PID_KP 1.3
#define IRON_PID_KI 0
#define IRON_PID_KD 0

#define IRON_PID_MIN 0
#define IRON_PID_MAX 100


#define IRON_TEMP_SOFT 50 //температура, до которой нагрев идет в 50% мощности, гр
#define IRON_TEMP_MIN 150 //мин температура, гр
#define IRON_TEMP_MAX 450L //макс температура, гр
#define IRON_TEMP_STEP 10 //шаг регулировки температуры, гр


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
