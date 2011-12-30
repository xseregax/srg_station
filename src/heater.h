#ifndef IRON_H
#define IRON_H

#define FULL_PERIOD 1 //пропуск полного периода
#define PID_SCALING 128

#define POWER_MIN 0 //минимальная мощность
#define POWER_MAX 100 //максимальная мощность

//пид паяльника
#define IRON_ADC_HOT 850L //чет жаримс, adc
#define IRON_ADC_ERROR 900L //не подключено термосопротивление, adc

#define IRON_PID_DELTA_T 50

#define IRON_TEMP_SOFT 50 //температура, до которой нагрев идет в 25% мощности, гр
#define IRON_TEMP_MIN 50 //мин температура, гр
#define IRON_TEMP_MAX 450L //макс температура, гр
#define IRON_TEMP_STEP 10 //шаг регулировки температуры, гр


typedef struct {
    uint16_t adc;
    uint16_t temp;
    uint16_t temp_need;
    uint8_t power;
} TPCTempInfo;

typedef struct _pid_params
{
    double kc; // Controller gain from Dialog Box
    double ti; // Time-constant for I action from Dialog Box
    double td; // Time-constant for D action from Dialog Box
    double ts; // Sample time [sec.] from Dialog Box

    double k0; // k0 value for PID controller
    double k1; // k1 value for PID controller
    double k2; // k2 value for PID controller
    double k3; // k3 value for PID controller
} pid_params; // struct pid_params

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

void heater_iron_setpower(uint8_t pow);

void heater_fen_on(void);
void heater_fen_off(void);

void heater_init_mod(void);
PT_THREAD(heater_pt_manage(struct pt *pt));

extern volatile pid_params pid;
void init_pid4(volatile pid_params *p);

#endif // IRON_H
