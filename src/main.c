/*
  xseregax
  srg_station

  */

#include "common.h"

#include "hal.h"
#include "heater.h"
#include "input.h"
#include "actions.h"
#include "ui.h"
#include "uart.h"


volatile TGlobalData g_data;

inline void init_global_data(void) {
    memset((void *)&g_data, 0, sizeof(TGlobalData));
}

uint8_t main(void) {

    //запрещаем прерывания, откл собаку
    cli();
    wdt_reset();
    wdt_disable();

    _delay_ms(100);

    //отключаем jtag
    MCUCSR |= _BV(JTD);
    MCUCSR |= _BV(JTD);

    //включаем станцию
    hal_power_on();

    //неиспользуемые пины в pull-up
    uint8_t t;

    t = _BV(PIN6) | _BV(PIN7);
    DDRA |= t;
    PORTA &= ~t;

    //инит систем таймер
    timer_init_timer();

    //инитим светики
    hal_init_leds();

    //инитим пищалку
    hal_init_buzer();

    //инитим adc
    hal_init_adc();

    //инитим кнопки
    hal_init_buttons();

    //инитим энкодер
    hal_init_encoder();

    //инитим пины для шим
    hal_init_pwm();

    //инитим прерывания
    hal_init_isr();

    //инитим экран
    ui_init_lcd();

    //инитим уарт
    uart_init_uart();

    init_global_data();

    struct pt pt_heater_manage,
            pt_input_buttons, pt_actions_actions, pt_ui_display, pt_uart_receive;

    PT_INIT(&pt_heater_manage);
    PT_INIT(&pt_input_buttons);
    PT_INIT(&pt_actions_actions);
    PT_INIT(&pt_ui_display);
    PT_INIT(&pt_uart_receive);

    heater_init_mod();
    actions_init_mod();
    input_init_mod();
    ui_init_mod();
    uart_init_mod();

    _delay_ms(100);

    //заставку на экран
    ui_hello_msg();

    //вкл прерывания и собаки
    wdt_reset();
    wdt_enable(WDTO_15MS);
    sei();

    while(1) {
        //
        heater_pt_manage(&pt_heater_manage);

        //проверим нажатие кнопок
        input_pt_check_inputs(&pt_input_buttons);

        //проверим комманды
        actions_pt_check_commands(&pt_actions_actions);

        //обновим изображение на экране
        ui_pt_update_display(&pt_ui_display);

        //проверим, пришло ли что нить по уарту
        uart_pt_recieve(&pt_uart_receive);

        //пинаем собачку
        wdt_reset();
    }

}

