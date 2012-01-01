#ifndef IRON_H
#define IRON_H

#define FULL_PERIOD 1 //пропуск полного периода
#define PID_SCALING 128

#define POWER_MIN 0 //минимальная мощность
#define POWER_MAX 100 //максимальная мощность

#define HEATER_TEMP_SOFT 50 //температура, до которой нагрев идет в 25% мощности, гр

#define HEATER_ADC_HOT 850L //чет жаримс, adc
#define HEATER_ADC_ERROR 900L //не подключен паяльник/фен, adc

#define HEATER_PID_DELTA_T 100 //обновление пид

//пид паяльника
#define IRON_PID_KC (20) // °C/%
#define IRON_PID_TI (48) // msec
#define IRON_PID_TD (IRON_PID_TI / 4) //msec

#define IRON_PID_K0 (IRON_PID_KC * HEATER_PID_DELTA_T / IRON_PID_TI)
#define IRON_PID_K1 (IRON_PID_KC * IRON_PID_TD / HEATER_PID_DELTA_T)

#define IRON_TEMP_MIN 150 //мин температура, гр
#define IRON_TEMP_MAX 450L //макс температура, гр
#define IRON_TEMP_STEP 10 //шаг регулировки температуры, гр


//пид фена
#define FEN_PID_KC (20) // °C/%
#define FEN_PID_TI (48) // msec
#define FEN_PID_TD (FEN_PID_TI / 4) //msec

#define FEN_PID_K0 (FEN_PID_KC * HEATER_PID_DELTA_T / FEN_PID_TI)
#define FEN_PID_K1 (FEN_PID_KC * FEN_PID_TD / HEATER_PID_DELTA_T)

#define FEN_TEMP_MIN 150 //мин температура, гр
#define FEN_TEMP_MAX 450L //макс температура, гр
#define FEN_TEMP_STEP 10 //шаг регулировки температуры, гр




typedef struct {
    uint16_t adc;
    uint16_t temp;
    uint16_t temp_need;
    uint8_t power;
} TPCTempInfo;

typedef struct {
    uint16_t xk;
    uint16_t tk;
    double pp;
    double pi;
    double pd;
    double yk;
} TPCPidInfo;


//апроксимация температуры по контрольным точкам
typedef struct {
    uint16_t y1; //adc
    uint16_t x0; //гр
    uint16_t y0; //adc
    uint32_t a;  //
} TTempZones;


typedef struct {
    uint16_t xk_1;
    uint16_t xk_2;

    uint16_t kc;
    uint16_t k0;
    uint16_t k1;
} TPid;

typedef struct {
    unsigned on: 1; //вкл-выкл

    uint16_t adc; //последние значение с adc

    uint8_t power; //текущая мощность
    uint8_t sigma; //для алг. Брезенхема

    uint16_t temp; //текущая температура тены
    uint16_t temp_need; //требуемая температура

    TPid pid;


    uint16_t def_t_min;
    uint16_t def_t_max;
    uint8_t def_t_step;
    TTempZones *def_zones;
    uint8_t def_adc_pin;

} THeater;


/*
данные калибровки
x - °C, y - adc
a = (x1 - x0) / (y1 - y0)
x = (y - y0) * a + x0
*/

//измеренные °C - ADC
//паяльник
#define TZ_PXY0 0.0,   0.0
#define TZ_PXY1 20.0,  350.0
#define TZ_PXY2 160.0, 540.0
#define TZ_PXY3 215.0, 600.0
#define TZ_PXY4 300.0, 700.0
#define TZ_PXY5 400.0, 800.0
#define TZ_PXY6 600.0, 1024.0

//fen
#define TZ_FXY0 0.0,   0.0
#define TZ_FXY1 20.0,  350.0
#define TZ_FXY2 160.0, 540.0
#define TZ_FXY3 215.0, 600.0
#define TZ_FXY4 300.0, 700.0
#define TZ_FXY5 400.0, 800.0
#define TZ_FXY6 600.0, 1024.0


#define TZ_AMUL _BV(14) //float to 32bit

#define TZ_X_(X0,Y0,X1,Y1) { (uint16_t)Y1, (uint16_t)X0, (uint16_t)Y0, (uint32_t)((((X1 - X0) / (Y1 - Y0))) * TZ_AMUL) }
#define TZ_X(x,y) TZ_X_(x,y)

void heater_on(void);
void heater_off(void);

void heater_init_mod(void);
PT_THREAD(heater_pt_manage(struct pt *pt));

#endif // IRON_H
