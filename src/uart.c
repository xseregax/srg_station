#include "common.h"
#include "uart.h"

RBUF_DECLARE( uart_tx_buf, uint8_t, UART_TX_BUFF_SIZE);
RBUF_DECLARE( uart_rx_buf, uint8_t, UART_RX_BUFF_SIZE);

PT_THREAD(uart_pt_recieve(struct pt *pt)) {
  PT_BEGIN(pt);

  for(;;) {
      PT_WAIT_WHILE(pt, RBUF_EMPTY(&uart_rx_buf));

      uint8_t byte;

      ATOMIC_BLOCK(ATOMIC_FORCEON) {
          byte = RBUF_RD(&uart_rx_buf);
      }

      switch(byte)
      {
      case 'r':
          ON(P_LED_RED);

          AVR_RESET;

          break;

      default:
          uart_send_b(byte);
          uart_send_str("\r\n");
          break;
      }
  }

  PT_END(pt);
}


ISR(USART_RXC_vect) {

    uint8_t byte = UDR;

    RBUF_WR(&uart_rx_buf, byte);
}


ISR(USART_UDRE_vect) {

    if(!RBUF_EMPTY( &uart_tx_buf )) {
        uint8_t byte;

        byte = RBUF_RD(&uart_tx_buf);

        UDR = byte;
        UCSRB |= _BV(UDRIE);
    }
    else {
        UCSRB &= ~_BV(UDRIE);
    }
}


void uart_init_mod(void) {
    RBUF_INIT(&uart_tx_buf);
    RBUF_INIT(&uart_rx_buf);
}


void uart_send_b(uint8_t byte) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        RBUF_WR(&uart_tx_buf, byte);
    }

    UCSRB |= _BV(UDRIE);
}

void uart_send_b_hex(uint8_t byte) {
    static const prog_char hexdigit[] = "0123456789ABCDEF";

    uart_send_b(pgm_read_byte(&hexdigit[byte >> 4]));
    uart_send_b(pgm_read_byte(&hexdigit[byte & 0x0F]));
}

void uart_send_str_P(PGM_P pString) {
    char c;
    while( (c = pgm_read_byte(pString++)) ) {
        uart_send_b(c);
    }
}

void uart_send_str(const char* pString) {
    while( *pString ) {
        uart_send_b(*pString++);
    }
}

