#include "common.h"


/*

//обнулить состояние текущей кнопки
inline void button_init(TButtonState &button) {
    memset(&button, 0, sizeof(TButtonState));
}

//проверка нажатия кнопки
inline void button_check_press(TButtonState &button) {
    if(!button.on) { //если нажатие не точное

        if(button.cnt > BUTTON_DEBOUNCE) { //antiдребезг
            button.on = 1;
            button.cnt = 0;
            //button.repeat = BUTTON_REPEAT;
            goto butt_on;
        } else {
            button.cnt ++;
        }
    } else { //пропустили дребезг

        if(!button.plong) {
            if(button.cnt >= BUTTON_LONG) { //долго держит
                button.plong = 1;
            }
        }
        else {
            button.cnt ++;
        }

        if(button.repeat >= BUTTON_REPEAT) {
butt_on:
            if(gActions.get_free_size()) {
                TActionCmd cmd;
                cmd.name = button.name;
                cmd.action = (button.plong? ACT_PUSH_LONG: ACT_PUSH);
                gActions.push(cmd);
            }

            button.repeat = 0;
        }
        else
            button.repeat ++;
    }

}

//проверка поворота энкодера
inline void encoder_check_rotate(TButtonState &button, TActions encact) {

   if(++button.cnt < 4) return;

   if(gActions.get_free_size()) {
        TActionCmd cmd;
        cmd.name = button.name;
        cmd.action = encact;
        gActions.push(cmd);
    }

    button.name = NM_NONE;
}

//проверка кнопок и энкодера
inline void button_check(TActElements name, TButtonState &button, TActions encact) {
    if(name != NM_NONE) { //нажали кнопку

        if(name != button.name) {
            button_init(button);
            button.name = name;
        } else
            button.release = 0;

        if(name != NM_ENCROTATE)
            button_check_press(button);
        else
            encoder_check_rotate(button, encact);
    }
    else if(button.on) {
        if(button.release >= BUTTON_RELEASE)
            button_init(button);
        else
            button.release ++;
    }
}

//опрос энкодера
inline TActions encoder_poll(void) {
    static uint8_t old_val = 0;
    TActions action = ACT_NONE;
    uint8_t cur_val = P_ENCODER_VAL;

    switch(old_val) {
        case 2:
            if(cur_val == 3) action = ACT_ROTATE_RIGHT;
            if(cur_val == 0) action = ACT_ROTATE_LEFT;
            break;
        case 0:
            if(cur_val == 2) action = ACT_ROTATE_RIGHT;
            if(cur_val == 1) action = ACT_ROTATE_LEFT;
            break;
        case 1:
            if(cur_val == 0) action = ACT_ROTATE_RIGHT;
            if(cur_val == 3) action = ACT_ROTATE_LEFT;
            break;
        case 3:
            if(cur_val == 1) action = ACT_ROTATE_RIGHT;
            if(cur_val == 2) action = ACT_ROTATE_LEFT;
            break;
    }

    old_val = cur_val;

    return action;
}


//---------------------------------------------------------------------------
namespace OS {

template<> OS_PROCESS void TProcButtons::exec()
{

    TButtonState button;
    TActElements active_button;
    TActions encact;
    uint8_t button_sleep;

    button_init(button);

    button_sleep = 0;

    for(;;) {
        active_button = NM_NONE;

        encact = encoder_poll();
        if(encact != ACT_NONE) {
            active_button = NM_ENCROTATE;
        }
        else
        if(button_sleep++ > (BUTTON_SLEEP / ENCODER_SLEEP)) { //в связи с большой частотой опроса энкодера
            button_sleep = 0;

            if(ACTIVE(P_BUTTON1) && ACTIVE(P_ENCODER_BUTTON))
                active_button = NM_BUTTON1_ENC;
            else
            if(ACTIVE(P_BUTTON1))
                active_button = NM_BUTTON1;
            else
            if(ACTIVE(P_BUTTON2))
                active_button = NM_BUTTON2;
            else
            if(ACTIVE(P_BUTTON3))
                active_button = NM_BUTTON3;
            else
            if(ACTIVE(P_BUTTON4))
                active_button = NM_BUTTON4;
            else
            if(ACTIVE(P_ENCODER_BUTTON))
                active_button = NM_ENCBUTTON;
        }

        button_check(active_button, button, encact);

        sleep(ENCODER_SLEEP);
    }

} // TProcButtons::exec()

} // namespace OS







//TProcUart
//---------------------------------------------------------------------------
namespace OS {

template<> OS_PROCESS void TProcUart::exec()
{
    uint8_t c;

    for(;;) {
        gUART.receive(c);

        switch(c)
        {
        case 'r':
            ON(P_LED_RED);

            cli();
            wdt_reset();
            wdt_enable( WDTO_15MS );
            while (1) { }

            break;

        default:
            gUART.send(c);
            break;
        }
    }

} // TProcUart::exec()

} // namespace OS


//TProcTimers
//---------------------------------------------------------------------------
namespace OS {

template<> OS_PROCESS void TProcTimers::exec()
{

    for(;;) {
        CALC_STACK;

        if(gCurrStates.iron_on) {
            char buf[10];
            sprintf(buf, "I|T:%d\n", gCurrStates.iron_adc);
            gUART.send(buf);
        }

        sleep(1000 * TIME_1MS);
    }

} // TProcTimers::exec()

} // namespace OS




//ZCD
OS_INTERRUPT void INT2_vect()
{
    static uint8_t phase = 0;

    OS::TISRW isr;

    phase ++;

    if(phase == PID_STEP) {
        gEvPID.signal_isr();
        phase = 0;
    }
}


*/
