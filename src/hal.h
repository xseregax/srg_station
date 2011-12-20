#ifndef HAL_H
#define HAL_H

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

    //IRON PWM
    //page 109, Normal mode
    TCCR1A = 0;

    //table 47, mode 0, prescaler 8
    TCCR1B = 0x02;

    TCNT1H=0x00;
    TCNT1L=0x00;

    ICR1H=0x00;
    ICR1L=0x00;

    OCR1AH=0x00;
    OCR1AL=0x00;

    OCR1BH=0x00;
    OCR1BL=0x32;

    TIMSK |= _BV(OCIE1A);
}

//инит прерываний
inline void hal_init_isr(void) {
    //int2 - для ZCD
    uint8_t t;

    //in, hiz
    t = BITMASK(P_ZCD);
    DDRB &= ~t;
    PORTB &= ~t;

    MCUCSR &= ~_BV(ISC2); //Set to falling edge interrupt
    GICR |= _BV(INT2); //Enable external int0

}

#endif // HAL_H
