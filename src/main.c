#include "common.h"

#include "hal.h"


int main(void) {
    //запрещаем прерывания, откл собаку
    cli();
    wdt_reset();
    wdt_disable();

    _delay_ms(1);

    //отключаем jtag
    MCUCSR |= _BV(JTD);
    MCUCSR |= _BV(JTD);

    //включаем станцию
    power_on();

    //неиспользуемые пины в pull-up
    uint8_t t;

    t = _BV(PIN6) | _BV(PIN7);
    DDRA |= t;
    PORTA &= ~t;

/*
    // запускаем системный таймер
    SYSTEM_TIMER_CS_REG = SYS_CLOCK;
    SYSTEM_TIMER_IE_REG |= _BV(SYSTEM_TIMER_IE_MASK);

    //инитим глобальные переменные
    init_curstates();

    //инитим уарт
    gUART.init();
*/
    //инитим экран
    init_lcd();
/*
    //инитим светики
    init_leds();

    //инитим пищалку
    init_buzer();

    //инитим adc
    init_adc();

    //инитим кнопки
    init_buttons();

    //инитим энкодер
    init_encoder();

    //инитим пины для шим
    init_pwm();

    //инитим прерывания
    init_isr();

    _delay_ms(1);

    //заставку на экран
    hello_msg();

    //вкл прерывания и собаки
    //wdt_enable(WDTO_15MS);
    enable_interrupts();

    //go go go
    OS::run();
*/


}
