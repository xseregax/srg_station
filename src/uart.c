#include "common.h"
#include "uart.h"

RBUF_DECLARE( uart_tx_buf, uint8_t, UART_TX_BUFF_SIZE);
RBUF_DECLARE( uart_rx_buf, uint8_t, UART_RX_BUFF_SIZE);

#include "heater.h"
#include <stdlib.h>

PT_THREAD(uart_pt_recieve(struct pt *pt)) {
  static uint8_t buf[16];
  static uint8_t *ptr = buf;
  static uint8_t mode = 0;

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
          mode = 0;

          ON(P_LED_RED);

          AVR_RESET;

          break;
      case 's':
          mode = 0;
          send_stat = send_stat?0:1;
          break;
      case 'p':
          mode = 'p';
          ptr = buf;

          uart_send_b(byte);
          break;

      case 'i':
          mode = 'i';
          ptr = buf;

          uart_send_b(byte);
          break;

      case 'd':
          mode = 'd';
          ptr = buf;

          uart_send_b(byte);
          break;

          case '\r':

          if(mode != 0) {
              *ptr = 0;

              if(ptr - buf >= 1) {
                  uint8_t x = atoi(buf);
                  if(x > 0 && x < 255) {
                      if(mode == 'p')
                          pid_p = x;
                      else
                      if(mode == 'i')
                          pid_i = x;
                      if(mode == 'd')
                          pid_d = x;
                      pid_init();
                      uart_send_str("\r\n");
                      uart_send_b(mode);
                      uart_send_str(": ");
                      uart_send_str(buf);
                      uart_send_str("\r\n");
                  } else {
                      uart_send_str("err x\r\n");
                  }
              } else {
                  uart_send_str("small\r\n");
              }
          } else {
              uart_send_str("empty\r\n");
          }
          mode = 0;
          break;

      case '0': case '1': case '2': case '3': case '4': case '5':
      case '6': case '7': case '8': case '9':
          *ptr = byte; ptr ++;

          uart_send_b(byte);

          if(ptr - buf > 15) { mode = 0; ptr = buf; uart_send_str("clear\r\n"); }

          break;

      default:
          mode = 0;

          uart_send_str("def,");
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

