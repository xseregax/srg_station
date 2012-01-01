#include <util/crc16.h>
#include "common.h"

#include "heater.h"
#include "uart.h"

RBUF_DECLARE( uart_tx_buf, uint8_t, UART_TX_BUFF_SIZE);
RBUF_DECLARE( uart_rx_buf, uint8_t, UART_RX_BUFF_SIZE);

uint8_t check_uart_info(TPCHeader *head, void *data) {

    if(head->header != PCINFO_HEADER)
        return 0;

    if((head->sign ^ PCINFO_HEADER) != head->len)
        return 0;

    uint8_t i, *p, crc = head->type;

    p = (uint8_t *)data;
    for(i = 0; i < head->len; i++)
        crc = _crc_ibutton_update(crc, p[i]);

    if(head->crc != crc)
        return 0;

    return 1;
}

PT_THREAD(uart_pt_recieve(struct pt *pt)) {
    static uint8_t buf[sizeof(TPCHeader)];
    static uint8_t *pbuf = 0;
    static uint8_t ibuflen = 0;
    static uint8_t cmd[PCINFO_CMDSIZE];
    static uint8_t *pcmd = 0;

    PT_BEGIN(pt);

    for(;;) {
        PT_WAIT_WHILE(pt, RBUF_EMPTY(&uart_rx_buf));

        uint8_t byte;

        ATOMIC_BLOCK(ATOMIC_FORCEON) {
            byte = RBUF_RD(&uart_rx_buf);
        }

        if(ibuflen) {
            *pcmd = byte;
            pcmd ++;

            if(pcmd - cmd < ibuflen) continue;

            TPCHeader *h = (TPCHeader*)buf;
            if(check_uart_info(h, cmd)) {
                TOGGLE(P_LED_RED);

                if(h->type == HI_PID_P)
                    pid.kc = 1.0 * (*(uint16_t*)cmd);
                else
                if(h->type == HI_PID_I) {
                    pid.ti = 0.001 * (*(uint16_t*)cmd);
                    pid.td = pid.ti / 4;
                }
                else
                if(h->type == HI_PID_D)
                    pid.td = pid.td;//pid.td = 0.001 * (*(uint16_t*)cmd);

                init_pid4();
            }

            pcmd = 0;
            ibuflen = 0;

            pbuf = 0;

            continue;
        }

        //start packet sign
        if(byte == PCINFO_HEADER && !pbuf) {
            pbuf = buf;
        }

        if(!pbuf) continue;

        *pbuf = byte;
        pbuf++;

        if((uint16_t)(pbuf - buf) < sizeof(TPCHeader))
           continue;

        TPCHeader *h = (TPCHeader*)buf;

        uint8_t check = 1;
        if(h->header != PCINFO_HEADER || (h->sign ^ PCINFO_HEADER) != h->len || h->len > PCINFO_CMDSIZE)
            check = 0;

        if(!check) {
            uint8_t i = 1;
            for(i = 0; i < sizeof(TPCHeader); i++) {
                if(buf[i] == PCINFO_HEADER)
                    break;
            }

            if(buf[i] == PCINFO_HEADER) {
                pbuf = buf;
                for(; i < sizeof(TPCHeader); i++) {
                   *pbuf = buf[i];
                    pbuf++;
                }
            }
            else
                pbuf = 0;
        } else {
            ibuflen = h->len;
            pcmd = cmd;
        }


    }

    PT_END(pt);
}



void send_uart_msg(TPCHeadType type, void *data, uint8_t len) {
    TPCHeader head;

    head.header = PCINFO_HEADER;
    head.sign = PCINFO_HEADER ^ len;
    head.len = len;

    head.type = type;

    uint8_t i, *p, crc = type;

    p = (uint8_t*)data;
    for(i = 0; i < len; i++)
        crc = _crc_ibutton_update(crc, p[i]);

    head.crc = crc;

    p = (uint8_t*)&head;
    for(i = 0; i < sizeof(TPCHeader); i++)
        uart_send_b(p[i]);

    p = (uint8_t*)data;
    for(i = 0; i < len; i++)
        uart_send_b(p[i]);
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

