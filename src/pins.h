#ifndef PINS_H
#define PINS_H



/*
  PA0 - ACD temp iron
  PA1 - ACD temp fen
  PA2 - Iron Active
  PA3 - ACD voltage drel
  PA4 - LED green
  PA5 - LED red
  PA6 -
  PA7 -

  PB0 - fen active (gerkon)
  PB1 - power on
  PB2 - ZCD
  PB3 - PWM drel
  PB4 - Button 1 / SS
  PB5 - Button 2 / MOSI
  PB6 - Button 3 / MISO
  PB7 - Button 4 / SCK

  PC0 - Display 1
  PC1 - Display 2
  PC2 - Display 3
  PC3 - Display 4
  PC4 - Display 5
  PC5 - Display 6
  PC6 - Display 7
  PC7 - Buzzer

  PD0 - UART RX
  PD1 - UART TX
  PD2 - Encoder
  PD3 - Encoder
  PD4 - Encoder - button
  PD5 - PWM iron
  PD6 - PWM fen
  PD7 - PWM fan

*/


#define P_POWER B,1,L //вкл-выкл питания

#define P_LED_GREEN A,4,H //зеленый лед
#define P_LED_RED A,5,H //красный лед

#define P_BUTTON1 B,4,L //кнопка 1
#define P_BUTTON2 B,5,L //кнопка 2
#define P_BUTTON3 B,6,L //кнопка 3
#define P_BUTTON4 B,7,L //кнопка 4

#define P_ENCODER_PIN PIND //для опроса состояния энкодера
#define P_ENCODER_SHIFT 2
#define P_ENCODER_LEFT D,2,L //энкодер пин 1
#define P_ENCODER_RIGHT D,3,L //энкодер пин 2
#define P_ENCODER_BUTTON D,4,L //энкодер кнопка

//#define P_ENCODER_VAL ((P_ENCODER_PIN & (ACTIVE(P_ENCODER_LEFT) | ACTIVE(P_ENCODER_RIGHT))) >> P_ENCODER_SHIFT)
#define P_ENCODER_VAL ((P_ENCODER_PIN & (0b01100)) >> P_ENCODER_SHIFT) //состояние энкодера

#define P_BUZER C,7,H //пищалка


#define P_ZCD B,2,H  //ZCD


#define P_IRON_PWM D,5,H //вкл-выкл тэны паяльника
#define P_FEN_PWM D,6,H //вкл-выкл тэны фена
#define P_FEN_FAN_PWM D,7,H //вкл-выкл кулера в фене
#define P_DREL_PWM B,3,H //вкл-выкл дрельки

#define P_IRON_ACTIVE A,2,L //пин активности паялки
#define P_FEN_ACTIVE B,0,L //пин активности фена

#define P_ADC_IRON A,0,L //пин адс паялки
#define P_ADC_FEN A,1,L //пин адс фена

#define ADC_PIN_IRON 0 //пин adc температуры паяльника
#define ADC_PIN_FEN 1  //пин adc температуры фена
//#define ADC_PIN_DREL 3 //пин аdc оборотов дрели



#endif // PINS_H
