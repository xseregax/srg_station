#include "common.h"
#include "uart.h"


PT_THREAD(uart_pt_recieve(struct pt *pt)) {
  PT_BEGIN(pt);


  PT_END(pt);
}


ISR(USART_RXC_vect) {
    uint8_t byte = UDR;

    //RxBuffer.push(byte);
}


ISR(USART_UDRE_vect) {
    if(1) { // TxBuffer.get_count()) {
        uint8_t byte;
  //      TxBuffer.pop(byte);
        //UDR = byte;

        UCSRB |= _BV(UDRIE);
    }
    else {
        UCSRB &= ~_BV(UDRIE);
    }
}


void uart_init_mod(void) {


}


void uart_send_b(uint8_t byte)
{
    // TxBuffer.push(byte);
    UCSRB |= _BV(UDRIE);
}

void uart_send_b_hex(uint8_t byte)
{
    static const prog_char hexdigit[] = "0123456789ABCDEF";

    uart_send_b(pgm_read_byte(&hexdigit[byte >> 4]));
    uart_send_b(pgm_read_byte(&hexdigit[byte & 0x0F]));
}

void uart_send_str_P(PGM_P pString)
{
    char c;
    while( (c = pgm_read_byte(pString++)) ) {
        uart_send_b(c);
    }
}

void uart_send_str(const char* pString)
{
    while( *pString ) {
        uart_send_b(*pString++);
    }
}

/*

//TProcUart
//---------------------------------------------------------------------------
namespace OS {

template<> OS_PROCESS void TProcUart::exec()
{
    uint8_t c;

    for(;;) {
        gUART.receive(c);

        switch(c)
        {
        case 'r':
            ON(P_LED_RED);

            cli();
            wdt_reset();
            wdt_enable( WDTO_15MS );
            while (1) { }

            break;

        default:
            gUART.send(c);
            break;
        }
    }

} // TProcUart::exec()

} // namespace OS
*/
