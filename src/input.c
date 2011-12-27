#include "common.h"
#include "actions.h"
#include "input.h"

volatile TButtonState g_button_state;

//обнулить состояние текущей кнопки
inline void button_init() {
    memset((void*)&g_button_state, 0, sizeof(TButtonState));
}


//проверка нажатия кнопки
inline void button_check_press() {
    if(g_button_state.on == _OFF) { //если нажатие не точное

        if(g_button_state.cnt > BUTTON_DEBOUNCE) { //antiдребезг
            g_button_state.on = _ON;
            g_button_state.cnt = 0;

            goto butt_on;
        } else {
            g_button_state.cnt ++;
        }
    } else { //пропустили дребезг

        if(g_button_state.plong == _OFF) {
            if(g_button_state.cnt >= BUTTON_LONG) { //долго держит
                g_button_state.plong = _ON;
            }
            else
                g_button_state.cnt ++;
        }

        if(g_button_state.repeat >= BUTTON_REPEAT) {
butt_on:
            //добавим действие
            actions_set_cmd(g_button_state.name,
                (g_button_state.plong == _ON? ACT_PUSH_LONG: ACT_PUSH));

            g_button_state.repeat = 0;
        }
        else
            g_button_state.repeat ++;
    }

}

//проверка поворота энкодера
inline void encoder_check_rotate(TActions encact) {

   //1 оборот, 4 импульса
   if(++g_button_state.cnt < 4) return;

    //добавим действие
    actions_set_cmd(g_button_state.name, encact);

    g_button_state.name = NM_NONE;
}

//опрос энкодера
static inline TActions encoder_poll(void) {
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
        default:
            break;
    }

    old_val = cur_val;

    return action;
}

//проверка кнопок и энкодера
inline void button_check(TActElements name, TActions encact) {
    if(name != NM_NONE) { //нажали кнопку

        if(name != g_button_state.name) {

            button_init();
            g_button_state.name = name;

        } else {
            g_button_state.release = 0;
        }

        if(name != NM_ENCROTATE) {
            button_check_press();
        }
        else {
            encoder_check_rotate(encact);
        }
    }
    else if(g_button_state.on == _ON) {
        //отпустили кнопень
        if(g_button_state.release >= BUTTON_RELEASE) {
            button_init();
        }
        else {
            g_button_state.release ++;
        }
    }
}

PT_THREAD(input_pt_check_encoder(struct pt *pt)) {
    static TIMER_T timer;

    PT_BEGIN(pt);

    TIMER_INIT(timer, ENCODER_SLEEP);
    for(;;) {
        PT_WAIT_UNTIL(pt, TIMER_ENDED(timer));
        TIMER_NEXT(timer, ENCODER_SLEEP);

        TActions encact = encoder_poll();
        if(encact != ACT_NONE) {
            button_check(NM_ENCROTATE, encact);
        }
    }

    PT_END(pt);
}


PT_THREAD(input_pt_check_buttons(struct pt *pt)) {
    static TIMER_T timer;

    PT_BEGIN(pt);

    TIMER_INIT(timer, BUTTON_SLEEP);
    for(;;) {
        PT_WAIT_UNTIL(pt, TIMER_ENDED(timer));
        TIMER_NEXT(timer, BUTTON_SLEEP);

        TActElements active_button = NM_NONE;

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

        button_check(active_button, ACT_NONE);
    }

    PT_END(pt);
}



PT_THREAD(input_pt_check_inputs(struct pt *pt)) {
    static struct pt pt_encoder, pt_buttons;

    PT_BEGIN(pt);

    PT_INIT(&pt_encoder);
    PT_INIT(&pt_buttons);

    PT_WAIT_THREAD(pt,
           input_pt_check_encoder(&pt_encoder) &
           input_pt_check_buttons(&pt_buttons)
          );

    PT_END(pt);
}

void input_init_mod(void) {
    button_init();
}
