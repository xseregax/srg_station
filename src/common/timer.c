
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "timer.h"


static volatile TIMER_T timer_timer0_millis = 0;

ISR(TIMER0_COMP_vect) {
    TCNT0 = 0;
    timer_timer0_millis ++;
}


TIMER_T timer_millis(void) {
    TIMER_T m;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        m = timer_timer0_millis;
    }
    return m;
}


void timer_init_timer(void) {
    // Set CTC mode, prescaler to 64
    TCCR0 |= _BV(WGM01) | _BV(CS01) | _BV(CS00);
    TCNT0 = 0;
    OCR0 = TIMER_ISR_COMPARE;

    // Enable timer 0 compare
    TIMSK |= _BV(OCIE0);
}

