#ifndef UART_H
#define UART_H


#define UART2X 1

#if UART2X == 1
    #define UART_DIV2X 8
#else
    #define UART_DIV2X 16
#endif

#define UART_BAUDRATE 57600L
#define UART_DIVIDER ((F_CPU + (UART_DIV2X * UART_BAUDRATE) / 2 ) / (UART_DIV2X * UART_BAUDRATE) - 1)

#define UART_RX_BUFF_SIZE 32
#define UART_TX_BUFF_SIZE 32


inline void uart_init_uart(void) {
    UCSRB = 0;

   //set baud rate
   UBRRH = UART_DIVIDER >> 8;
   UBRRL = UART_DIVIDER;

#ifdef UART2X
   UCSRA |= _BV(U2X);
#endif

   //tx, rx, UDRE
   UCSRB =  _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);

   //8 bits, 1 stop
   UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
}

void uart_init_mod(void);
PT_THREAD(uart_pt_recieve(struct pt *pt));

void uart_send_b(uint8_t byte);
void uart_send_b_hex(uint8_t byte);
void uart_send_str_P(PGM_P pString);
void uart_send_str(const char* pString);



#endif // UART_H
