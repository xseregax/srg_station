#ifndef UI_H
#define UI_H



PT_THREAD(ui_pt_update_display(struct pt *pt));




void ui_init_mod(void);
void ui_hello_msg(void);

extern void hd44780_init(void);

//инит экрана
inline void ui_init_lcd(void) {
    hd44780_init();
}


#endif // UI_H
