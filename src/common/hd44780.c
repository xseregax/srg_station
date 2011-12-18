/*
 *  HD44780 Lib for AVR
 *
 *  Copyright (C) 2009-2010  Kirill Persin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
 *
 *  Author: Kirill Pershin (aka NetImperia)
 *  WWW: http://www.netimperia.com
 *  E-Mail: com@netimperia.com
 *
 *  ВСЕ НАСТРОЙКИ СМОТРИЕ В ФАЙЛЕ hd44780.h
 *
 */
#include "avr/pgmspace.h"
#include "hd44780.h"

#if HD44780_CONFIG_PINMODE>0

// Конвертирует данные так чтобы они передавались на нужные порты (используется при кривом подключении)
uint8_t hd44780_pin_convert_to(uint8_t d) {

    uint8_t c = 0;

    #if HD44780_CONFIG_BIT>0
            if(d & (1 << 0)) { c |= (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D0_PIN)); }
            if(d & (1 << 1)) { c |= (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D1_PIN)); }
            if(d & (1 << 2)) { c |= (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D2_PIN)); }
            if(d & (1 << 3)) { c |= (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D3_PIN)); }
    #endif

    if(d & (1 << 4)) { c |= (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D4_PIN)); }
    if(d & (1 << 5)) { c |= (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D5_PIN)); }
    if(d & (1 << 6)) { c |= (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D6_PIN)); }
    if(d & (1 << 7)) { c |= (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D7_PIN)); }
    return c;
}




// Конвертирует из портов в данные (используется при кривом подключении)
uint8_t hd44780_pin_convert_from(uint8_t d) {	
    uint8_t c = 0;
    #if HD44780_CONFIG_BIT>0
            if(d & (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D0_PIN))) { c |= (1 << 0); }
            if(d & (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D1_PIN))) { c |= (1 << 1); }
            if(d & (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D2_PIN))) { c |= (1 << 2); }
            if(d & (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D3_PIN))) { c |= (1 << 3); }
    #endif
    if(d & (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D4_PIN))) { c |= (1 << 4); }
    if(d & (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D5_PIN))) { c |= (1 << 5); }
    if(d & (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D6_PIN))) { c |= (1 << 6); }
    if(d & (1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D7_PIN))) { c |= (1 << 7); }
    return c;
}

#endif




// Проверяет занят-ли LCD работой.
uint8_t hd44780_ready() {
    uint8_t d=0;
    #if HD44780_CONFIG_BIT==0
            uint8_t r = 0;
    #endif

    // Выставляем все биты дата порта в 0
    HD44780_SET_DATA_PORTS_NULL;

    // Говорим что все DATA работают на прием
    HD44780_SET_DATA_PORTS_READ;

    // Включает RE и E флаги
    HD44780_SET_CONTROL_PORTS(0, 1, 1);

    // Пауза
    #if HD44780_CONFIG_PAUSE_CMD_E>0
            _delay_us(HD44780_CONFIG_PAUSE_CMD_E);
    #endif

    d = HD44780_DATA_PIN_NAME;

    // Выключает RE и E флаги
    HD44780_SET_CONTROL_PORTS(0, 1, 0);


    #if HD44780_CONFIG_BIT==0

            #if HD44780_CONFIG_PINMODE>0
                    r |= hd44780_pin_convert_from(d);
            #else
                    r |= d;
            #endif


            // Выставляем все биты дата порта в 0
            HD44780_SET_DATA_PORTS_NULL;

            // Включает RE и E флаги
            HD44780_SET_CONTROL_PORTS(0, 1, 1);

            d = HD44780_DATA_PIN_NAME;

            // Пауза
            #if HD44780_CONFIG_PAUSE_CMD_E>0
                    _delay_us(HD44780_CONFIG_PAUSE_CMD_E);
            #endif

            // Выключает RE и E флаги
            HD44780_SET_CONTROL_PORTS(0, 1, 0);

            #if HD44780_CONFIG_PINMODE>0
                    r |= (hd44780_pin_convert_from(d) >> 4);
            #else
                    r |= d >> 4;
            #endif

    #endif

    // Пауза
    #if HD44780_CONFIG_PAUSE_BUSY>0
            _delay_us(HD44780_CONFIG_PAUSE_BUSY);
    #endif

    // Говорим что все DATA работают на передачу
    HD44780_SET_DATA_PORTS_WRITE;

    #if HD44780_CONFIG_BIT==0
            return r;
    #else

            #if HD44780_CONFIG_PINMODE>0
                    return hd44780_pin_convert_from(d);
            #else
                    return d;
            #endif
    #endif
}




#if HD44780_CONFIG_OPTIMIZATION<=0

// Нужна для отпраки простых запросов (коротких) Восновном используется только при инициализации.
void hd44780_send_nibble(uint8_t cmd) {
    #if HD44780_CONFIG_BIT==0
            uint8_t r = 0;
    #endif
    // Выставляем в дата порту биты в 0 (только для дата контактов)
    HD44780_SET_DATA_PORTS_NULL;

    #if HD44780_CONFIG_BIT==0
            #if HD44780_CONFIG_PINMODE>0
                    r = hd44780_pin_convert_to(cmd & 0xF0);
            #else
                    r = cmd & ~0x0f;
            #endif
            HD44780_DATA_PORT_NAME |= r;
    #else
            #if HD44780_CONFIG_PINMODE>0
                    cmd = hd44780_pin_convert_to(cmd);
            #endif
            HD44780_DATA_PORT_NAME |= cmd;
    #endif


    // Включает E флаг
    HD44780_SET_CONTROL_PORTS(0, 0, 1);

    // Делаем паузу чтобы успел среагировать
    #if HD44780_CONFIG_PAUSE_WRITE_INIT>0
            _delay_us(HD44780_CONFIG_PAUSE_WRITE_INIT);
    #endif

    // Выключает E флаг
    HD44780_SET_CONTROL_PORTS(0, 0, 0);


    #if HD44780_CONFIG_BIT==0

            // Выставляем в дата порту биты в 0 (только для дата контактов)
            HD44780_SET_DATA_PORTS_NULL;

            #if HD44780_CONFIG_PINMODE>0
                    r = hd44780_pin_convert_to(cmd << 4);
            #else
                    r = cmd << 4;
            #endif
            HD44780_DATA_PORT_NAME |= r;

            // Включает E флаг
            HD44780_SET_CONTROL_PORTS(0, 0, 1);

            // Делаем паузу чтобы успел среагировать
            #if HD44780_CONFIG_PAUSE_WRITE_INIT>0
                    _delay_us(HD44780_CONFIG_PAUSE_WRITE_INIT);
            #endif

            // Выключает E флаг
            HD44780_SET_CONTROL_PORTS(0, 0, 0);

    #endif

    #if HD44780_CONFIG_PAUSE_AFTER_CMD >0
            _delay_us(HD44780_CONFIG_PAUSE_AFTER_CMD);
    #endif

}

#endif




// Используется для отправки команд.
#if HD44780_CONFIG_OPTIMIZATION>0

void hd44780_send_command(uint8_t cmd, uint8_t rs, uint8_t pause) {
    #if HD44780_CONFIG_BIT==0
            uint8_t r = 0;
    #endif
    // Ожидаем пока LCD освободится и начнет принимать команды
    if(pause>0) { while (hd44780_ready() & HD44780_FLAG_BUSY ) {} }

#else

void hd44780_send_command(uint8_t cmd, uint8_t rs) {
    #if HD44780_CONFIG_BIT==0
            uint8_t r = 0;
    #endif
    // Ожидаем пока LCD освободится и начнет принимать команды
    while (hd44780_ready() & HD44780_FLAG_BUSY) {}
#endif

//	_delay_ms(100);

    // Выставляем в дата порту биты в 0 (только для дата контактов)
    HD44780_SET_DATA_PORTS_NULL;

    #if HD44780_CONFIG_BIT==0
            #if HD44780_CONFIG_PINMODE>0
                    r = hd44780_pin_convert_to(cmd & 0xF0);
            #else
                    r = cmd & ~0x0f;
            #endif
            HD44780_DATA_PORT_NAME |= r;
    #else
            #if HD44780_CONFIG_PINMODE>0
                    cmd = hd44780_pin_convert_to(cmd);
            #endif
            HD44780_DATA_PORT_NAME |= cmd;
    #endif

    // Включает E флаг
    HD44780_SET_CONTROL_PORTS(rs, 0, 1);


    // Пауза
    #if HD44780_CONFIG_PAUSE_CMD_E>0
            _delay_us(HD44780_CONFIG_PAUSE_CMD_E);
    #endif

    // Выключает E флаг
    HD44780_SET_CONTROL_PORTS(rs, 0, 0);


    #if HD44780_CONFIG_BIT==0

            // Выставляем в дата порту биты в 0 (только для дата контактов)
            HD44780_SET_DATA_PORTS_NULL;

            #if HD44780_CONFIG_PINMODE>0
                    r = hd44780_pin_convert_to(cmd << 4);
            #else
                    r = cmd << 4;
            #endif
            HD44780_DATA_PORT_NAME |= r;

            // Включает E флаг
            HD44780_SET_CONTROL_PORTS(rs, 0, 1);

            // Делаем паузу чтобы успел среагировать
            #if HD44780_CONFIG_PAUSE_CMD_E>0
                    _delay_us(HD44780_CONFIG_PAUSE_CMD_E);
            #endif

            // Выключает E флаг
            HD44780_SET_CONTROL_PORTS(rs, 0, 0);

    #endif


    #if HD44780_CONFIG_PAUSE_AFTER_CMD >0
            _delay_us(HD44780_CONFIG_PAUSE_AFTER_CMD);
    #endif

}




// Инициализация 
void hd44780_init(void) {

    // Требуется по документации минимум 40ms
    #if HD44780_CONFIG_PAUSE_START>0
            _delay_us(HD44780_CONFIG_PAUSE_START);
    #endif

    // Выставляем в контрольных портах все биты в 0
    HD44780_SET_CONTROL_PORTS(0, 0, 0);

    // Включаем контрольные порты на запись в HD44780
    HD44780_SET_CONTROL_DDRS(1, 1, 1);

    // Во все дата порты записываем 0 (на всякий пожарный чтобы ничего не случилось)
    HD44780_SET_DATA_PORTS_NULL;

    // Включаем дата порты на запись в HD44780
    HD44780_SET_DATA_PORTS_WRITE;

    // Пауза после открытия портов. Для некоторых LCD это необходимо.
    #if HD44780_CONFIG_PAUSE_CMD_START>0
            _delay_us(HD44780_CONFIG_PAUSE_CMD_START);
    #endif

    // Нужно 4 раза отправить. Некоторые LCD работают и после одного. А некоторым 4 подавай.
    // Указывает какой режим работы. 4битный (в нем используются только D4,D5,D6,D7)
    // или 8битный (в нем используются все)  Так-же указывает сколько строк у LCD.
    // 1 означает одна, 0 означает две и более.
    // Последнее значение это какой шрифт использовать. Если 1 то 5x10 иначе 5x8
    // Для инициализации. (8bit mode 1/0, two lines 1/0, font 5x10 1/0)
    #if HD44780_CONFIG_BIT==0 && HD44780_CONFIG_INITLSTART==2

            // Выставляем в дата порту биты в 0 (только для дата контактов)
            HD44780_SET_DATA_PORTS_NULL;

            // Включает E флаг
            HD44780_SET_CONTROL_PORTS(0, 0, 1);


            #if HD44780_CONFIG_PINMODE>0
                    HD44780_DATA_PORT_NAME |= hd44780_pin_convert_to(HD44780_SET_FUNCTIONEX(HD44780_CONFIG_BIT, HD44780_CONFIG_LINES, HD44780_CONFIG_FONT_SIZE) );
            #else
                    HD44780_DATA_PORT_NAME |= HD44780_SET_FUNCTIONEX(HD44780_CONFIG_BIT, HD44780_CONFIG_LINES, HD44780_CONFIG_FONT_SIZE);
            #endif

            // Делаем паузу чтобы успел среагировать
            #if HD44780_CONFIG_PAUSE_WRITE_INIT>0
                    _delay_us(HD44780_CONFIG_PAUSE_WRITE_INIT);
            #endif

            // Выключает E флаг
            HD44780_SET_CONTROL_PORTS(0, 0, 0);


            #if HD44780_CONFIG_PAUSE_AFTER_CMD >0
                    _delay_us(HD44780_CONFIG_PAUSE_AFTER_CMD);
            #endif

    #else
            HD44780_SEND_FUNCTION(HD44780_CONFIG_BIT, HD44780_CONFIG_LINES, HD44780_CONFIG_FONT_SIZE);
    #endif


    #if HD44780_CONFIG_INIT_CNT>1

            // Пауза между первой и второй инициализацией
            #if HD44780_CONFIG_PAUSE_INIT_STEPONE>0
                    _delay_us(HD44780_CONFIG_PAUSE_INIT_STEPONE);
            #endif

            // Еще раз посылаем инициализацию
            HD44780_SEND_FUNCTION(HD44780_CONFIG_BIT, HD44780_CONFIG_LINES, HD44780_CONFIG_FONT_SIZE);

    #endif


    #if HD44780_CONFIG_INIT_CNT>2

            // Пауза между второй и третьей инициализацией
            #if HD44780_CONFIG_PAUSE_INIT_STEPTWO>0
                    _delay_us(HD44780_CONFIG_PAUSE_INIT_STEPTWO);
            #endif

            // Еще раз посылаем инициализацию
            HD44780_SEND_FUNCTION(HD44780_CONFIG_BIT, HD44780_CONFIG_LINES, HD44780_CONFIG_FONT_SIZE);

    #endif


    #if HD44780_CONFIG_INIT_CNT>3

            // Пауза между второй и третьей инициализацией
            #if HD44780_CONFIG_PAUSE_INIT_STEPTHREE>0
                    _delay_us(HD44780_CONFIG_PAUSE_INIT_STEPTHREE);
            #endif

            // Еще раз посылаем инициализацию
            HD44780_SEND_FUNCTION(HD44780_CONFIG_BIT, HD44780_CONFIG_LINES, HD44780_CONFIG_FONT_SIZE);

    #endif

    // Отправлем команду которая устанавливает курсор в начало. (Должна идти первой)
    HD44780_SEND_CMD_HOME;

    // Включаем экран и устанавливаем параметры курсора
    HD44780_SEND_LCD_CTRL(1, HD44780_CONFIG_CURSOR_ON, HD44780_CONFIG_CURSOR_BLINK);

    // Очищаем экран
    HD44780_SEND_CMD_CLEAR;

    // Входим в режим работы и устанавливем направление курсора и сдвиг экрана.
    HD44780_SEND_ENTRY_MODE(HD44780_CONFIG_CURSOR_DIRECTION, HD44780_CONFIG_CURSOR_LCD_SHIFT);

}


