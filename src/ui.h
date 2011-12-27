#ifndef UI_H
#define UI_H

#define UI_UPDATE_TIME 250

#define BEEP(delay) do { ON(P_BUZER); _delay_ms(delay); OFF(P_BUZER); } while(0)

//uses for g_ui_update_screen
#define UPDATE_SCREEN_BITS 0xFF

#define UPDATE_SCREEN_MENU _BV(1)
#define UPDATE_SCREEN_VALS _BV(2)
#define UPDATE_SCREEN_FLASH _BV(3)
#define UPDATE_SCREEN_ERROR _BV(4)

#define UPDATE_SCREEN_ALL (UPDATE_SCREEN_MENU|UPDATE_SCREEN_VALS)


extern void hd44780_init(void);


void ui_init_mod(void);
PT_THREAD(ui_pt_update_display(struct pt *pt));
void ui_hello_msg(void);


//инит экрана
inline void ui_init_lcd(void) {
    hd44780_init();
}

//установить флаг вывода на экран
inline void ui_set_update_screen(uint8_t flag) {
    g_data.update_screen |= flag;
}



#endif // UI_H
