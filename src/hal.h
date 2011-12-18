#ifndef HAL_H
#define HAL_H

extern void hd44780_init();


//prescaler=16MHz/128 (125kHz)
#if F_CPU == 16000000L
#define ADC_PRESCALER (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0))
#elif F_CPU == 8000000L
#define ADC_PRESCALER (_BV(ADPS2) | _BV(ADPS0))
#elif F_CPU == 4000000L
#define ADC_PRESCALER (_BV(ADPS2))
#elif F_CPU == 2000000L
#define ADC_PRESCALER (_BV(ADPS1) | _BV(ADPS0))
#elif F_CPU == 1000000L
#define ADC_PRESCALER (_BV(ADPS1))
#else
#define ADC_PRESCALER (_BV(ADPS0))
#endif






//вкл станции
inline void power_on(void) {
    DRIVER(P_POWER,OUT);
    ON(P_POWER);
}


//инит экрана
inline void init_lcd(void) {
    hd44780_init();
}


//инит светиков
inline void init_leds(void) {
    uint8_t t;

    //out, low
    t = BITMASK(P_LED_GREEN) | BITMASK(P_LED_RED);
    DDRA |= t;
    PORTA &= ~t;
}

//инит бузера
inline void init_buzer(void) {
    uint8_t t;

    //out, low
    t = BITMASK(P_BUZER);
    DDRC |= t;
    PORTC &= ~t;
}


//инит энкодера
inline void init_encoder(void) {
    uint8_t t;

    //in, pull-up
    t = BITMASK(P_ENCODER_LEFT) | BITMASK(P_ENCODER_RIGHT) | BITMASK(P_ENCODER_BUTTON);
    DDRD &= ~t;
    PORTD |= t;
}

//инит кнопочек
inline void init_buttons(void) {
    uint8_t t;

    SPCR &= ~_BV(SPE);

    //in, pull-up
    t = BITMASK(P_BUTTON1) | BITMASK(P_BUTTON2) | BITMASK(P_BUTTON3) |
            BITMASK(P_BUTTON4) | BITMASK(P_FEN_ACTIVE);
    DDRB &= ~t;
    PORTB |= t;

    //in, pull-up
    t = BITMASK(P_IRON_ACTIVE);
    DDRA &= ~t;
    PORTA |= t;
}




//инит adc
inline void init_adc(void) {
    uint8_t t;

    //in, hiz
    t = BITMASK(P_ADC_IRON) | BITMASK(P_ADC_FEN);
    DDRA &= ~t;
    PORTA &= ~t;


    //Aref = 2.494 ext stab
    ADMUX = 0b000000000;

    ADCSRA = _BV(ADEN) | ADC_PRESCALER;
    //ADCSRB = (1<<ADHSM); // high speed mode
}

//инит пинов шим
inline void init_pwm(void) {
    uint8_t t;

    //out, low
    t = BITMASK(P_DREL_PWM);
    DDRB |= t;
    PORTB &= ~t;

    //out, low
    t = BITMASK(P_IRON_PWM) | BITMASK(P_FEN_PWM) | BITMASK(P_FEN_FAN_PWM);
    DDRD |= t;
    PORTD &= ~t;
}

#endif // HAL_H
