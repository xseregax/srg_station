#ifndef HAL_H
#define HAL_H

#define TIMER1A_PRESCALE (_BV(CS11) | _BV(CS10))
#define TIMER1_PRESCALE_OFF (_BV(CS12) | _BV(CS11) | _BV(CS10))

//вкл станции
inline void hal_power_on(void) {
    DRIVER(P_POWER,OUT);
    ON(P_POWER);
}

//инит светиков
inline void hal_init_leds(void) {
    uint8_t t;

    //out, low
    t = BITMASK(P_LED_GREEN) | BITMASK(P_LED_RED);
    DDRA |= t;
    PORTA &= ~t;
}

//инит бузера
inline void hal_init_buzer(void) {
    uint8_t t;

    //out, low
    t = BITMASK(P_BUZER);
    DDRC |= t;
    PORTC &= ~t;
}


//инит энкодера
inline void hal_init_encoder(void) {
    uint8_t t;

    //in, pull-up
    t = BITMASK(P_ENCODER_LEFT) | BITMASK(P_ENCODER_RIGHT) | BITMASK(P_ENCODER_BUTTON);
    DDRD &= ~t;
    PORTD |= t;
}

//инит кнопочек
inline void hal_init_buttons(void) {
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
inline void hal_init_adc(void) {
    uint8_t t;

    //in, hiz
    t = BITMASK(P_ADC_IRON) | BITMASK(P_ADC_FEN);
    DDRA &= ~t;
    PORTA &= ~t;


    //Aref = 2.494 ext stab
    ADMUX = 0b000000000;

    //prescaler=16MHz/128 (125kHz)
    ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    //ADCSRB = (1<<ADHSM); // high speed mode
}

//инит пинов шим
inline void hal_init_pwm(void) {
    uint8_t t;

    //out, low
    t = BITMASK(P_DREL_PWM);
    DDRB |= t;
    PORTB &= ~t;

    //out, low
    t = BITMASK(P_IRON_PWM) | BITMASK(P_FEN_PWM) | BITMASK(P_FEN_FAN_PWM);
    DDRD |= t;
    PORTD &= ~t;

    //FIM for iron (timers)
    //timer1, CTC, OCR = power

    //prescale off
    TCCR1B &= ~TIMER1_PRESCALE_OFF;

    //mode normal
    //TCCR1A = 0;

    //mode 4, CTC, prescaler to 64 (250kHz), def off
    TCCR1B |= _BV(WGM12);

    //текущий счетчик
    TCNT1 = 0;
    //для сравнения
    OCR1A = 0;

    //вкл прерыв. таймер1
    TIMSK |= _BV(OCIE1A);

    //end FIM (timers)
}

//инит прерываний
inline void hal_init_isr(void) {
    //int2 - для ZCD
    uint8_t t;

    //in, hiz
    t = BITMASK(P_ZCD);
    DDRB &= ~t;
    PORTB &= ~t;

    //для ZCD
    MCUCSR &= ~_BV(ISC2); //ниспадающий фронт
    GICR |= _BV(INT2); //вкл прерывание

}

#endif // HAL_H
