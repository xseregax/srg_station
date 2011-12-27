#ifndef INPUT_H
#define INPUT_H

#define BUTTON_SLEEP (20 * TIME_1MS) //опрос кнопок
#define BUTTON_DEBOUNCE (80 * TIME_1MS / BUTTON_SLEEP) //антидребезг
#define BUTTON_LONG (2000L * TIME_1MS / BUTTON_SLEEP) //долгое нажатие
#define BUTTON_REPEAT (500 * TIME_1MS / BUTTON_SLEEP) //автоповтор
#define BUTTON_RELEASE (200 * TIME_1MS / BUTTON_SLEEP) //отпустил

#define ENCODER_SLEEP (10 * TIME_1MS) //опрос энкодера


//для опроса кнопок
typedef struct {
    TActElements name; //какая кнопка
    unsigned on: 1; //заюзана
    unsigned plong: 1; //долго заюзана

    uint8_t cnt; //счетчик
    uint8_t repeat; //для автоповтора
    uint8_t release;//для сброса нажатия
} TButtonState;


void input_init_mod(void);
PT_THREAD(input_pt_check_inputs(struct pt *pt));

#endif // INPUT_H
