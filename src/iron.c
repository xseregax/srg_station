#include "common.h"
#include "iron.h"

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

PT_THREAD(iron_pt_manage(struct pt *pt)) {
    static TIMER_T timer;
    PT_BEGIN(pt);

    TIMER_INIT(timer, IRON_CHECK_TIME);
    for(;;) {

        PT_WAIT_UNTIL(pt, g_data.iron.on && TIMER_ENDED(timer));
        TIMER_NEXT(timer, IRON_CHECK_TIME);




    }

    PT_END(pt);
}


void iron_init_mod(void) {

    g_data.iron.on = 0;
    g_data.iron.temp_need = IRON_TEMP_MIN;
}


PGM(TTempZones gIronTempZones[]) = {
    TZ_X(TZ_XY0, TZ_XY1),
    TZ_X(TZ_XY1, TZ_XY2),
    TZ_X(TZ_XY2, TZ_XY3),
    TZ_X(TZ_XY3, TZ_XY4),
    TZ_X(TZ_XY4, TZ_XY5),
    TZ_X(TZ_XY5, TZ_XY6),
};




/*
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
