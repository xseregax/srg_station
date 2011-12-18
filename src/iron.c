#include "common.h"


/*
данные калибровки
x - °C, y - adc
a = (x1 - x0) / (y1 - y0)
x = (y - y0) * a + x0
*/
//измеренные °C - ADC
/*
#define TZ_XY0 0.0,   0.0
#define TZ_XY1 30.0,  375.0
#define TZ_XY2 50.0,  396.0
#define TZ_XY3 100.0, 440.0
#define TZ_XY4 150.0, 480.0
#define TZ_XY5 183.0, 509.0
#define TZ_XY6 500.0, 1024.0

#define TZ_AMUL 10000 //float to 32bit

#define TZ_X_(X0,Y0,X1,Y1) { (uint16_t)Y1, (uint16_t)X0, (uint16_t)Y0, (uint32_t)((((X1 - X0) / (Y1 - Y0))) * TZ_AMUL) }
#define TZ_X(x,y) TZ_X_(x,y)

PGM(TTempZones gIronTempZones[]) = {
    TZ_X(TZ_XY0, TZ_XY1),
    TZ_X(TZ_XY1, TZ_XY2),
    TZ_X(TZ_XY2, TZ_XY3),
    TZ_X(TZ_XY3, TZ_XY4),
    TZ_X(TZ_XY4, TZ_XY5),
    TZ_X(TZ_XY5, TZ_XY6),
};




//TProc0
//---------------------------------------------------------------------------
namespace OS {

template<> OS_PROCESS void TProc0::exec()
{
    uint16_t adc;

    for(;;) {

        //если паяльник вкл, опросим температуру
        if(gCurrStates.iron_on && gEvPID.is_signaled()) {

            adc = adc_read(ADC_PIN_IRON);
            if(adc != gCurrStates.iron_adc) {
                gCurrStates.iron_adc = adc;
                gCurrStates.iron_temp = find_temp(adc, gIronTempZones, sizeof(gIronTempZones));
            }

            gCurrStates.iron_error = gCurrStates.iron_temp_need - gCurrStates.iron_temp;
            if(gCurrStates.iron_power > IRON_MIN_POWER && gCurrStates.iron_power < IRON_MAX_POWER)
                gCurrStates.iron_integral += gCurrStates.iron_error;

            gCurrStates.iron_power_tmp =
                    IRON_KP * gCurrStates.iron_error +
                    IRON_KI * gCurrStates.iron_integral +
                    IRON_KD * (gCurrStates.iron_error - gCurrStates.iron_error_old);

            gCurrStates.iron_error_old = gCurrStates.iron_error;

            if(gCurrStates.iron_power_tmp < IRON_MIN_POWER) gCurrStates.iron_power_tmp = IRON_MIN_POWER;
            if(gCurrStates.iron_power_tmp > IRON_MAX_POWER) gCurrStates.iron_power_tmp = IRON_MAX_POWER;

            gCurrStates.iron_power = (uint16_t)gCurrStates.iron_power_tmp;

            uint16_t tmp = pgm_read_word(&gPowerMas[gCurrStates.iron_power]);

            OCR1AH = tmp >> 8;
            OCR1AL = tmp;

            gEvChanged.signal();
        }

        //если фен вкл, опросим температуру
        if(gCurrStates.fen_on) {
            //gCurrStates.fen_adc = adc_read(ADC_PIN_FEN);
        }
        //if(gCurrStates.drel_on) {
           // gCurrStates.drel_adc = adc_read(ADC_PIN_DREL);
        //}

        if(gEvPID.is_signaled())
            gEvPID.clear();

        sleep(TIME_1MS);
    }

} // TProc0::exec()

} // namespace OS

*/
