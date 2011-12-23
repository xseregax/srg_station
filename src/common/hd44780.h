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
 *  Изначально данная библиотека писалась для себя, под экран WH1602B-YGK-CTK (16x2).
 *  Это LCD от компании Winstar. Построеного на чипе HD44780
 *
 *  Если кому-то будет полезна, буду очень рад.
 *
 */

#ifndef HD44780_H
#define HD44780_H

// Указание к каким контактам подключен LCD

// Некоторые тонкости.

// RS, RW, E можете подключать в любом порядке к любым контактам. Но они должны находится на порту с
// одной и той-же буквой.

// Шина данных тоже должна находиться на порту с одинаковой буквой.

// Совет. Если хотите сократить занимаемую память и ускорить работу используйте 8битное подключение.
// Помимо этого используйте стандартную нумерацию. PORT1 к D1, PORT2 к D2 итд...


#define HD44780_CONFIG_DATA_PORT_NAME C	// Буква порта к которому подключена шина данных (D0-D7)
#define HD44780_CONFIG_D0_PIN 0	// D0 (Если 4 битный режим то указывать не нужно)
#define HD44780_CONFIG_D1_PIN 0	// D1 (Если 4 битный режим то указывать не нужно)
#define HD44780_CONFIG_D2_PIN 0	// D2 (Если 4 битный режим то указывать не нужно)
#define HD44780_CONFIG_D3_PIN 0	// D3 (Если 4 битный режим то указывать не нужно)
#define HD44780_CONFIG_D4_PIN 3	// D4
#define HD44780_CONFIG_D5_PIN 2	// D5
#define HD44780_CONFIG_D6_PIN 1	// D6
#define HD44780_CONFIG_D7_PIN 0	// D7

#define HD44780_CONFIG_CONTROL_PORT_NAME C // Буква порта к которому подключены контрольные линии
#define HD44780_CONFIG_RS_PIN 6	// RS
#define HD44780_CONFIG_RW_PIN 5	// RW
#define HD44780_CONFIG_E_PIN 4	// E

#define HD44780_CONFIG_PINMODE 1	// Если контакты дата шины подключены вразнобой, установите тут 1
        // Но учтите что в этом случае вы увеличите занимаемую память и замедлите код.
        // Если все контакты идут подряд установите 0
        // Это значит 7 контакт должен подключаться к D7 шестой к D6 итд...
        // НАСТОЯТЕЛЬНО РЕКОМЕНДУЕТСЯ ИСПОЛЬЗОВАТЬ РЕЖИМ 0

// Дополнительне настройки которе возможно захочется поменять.

#define HD44780_CONFIG_CURSOR_BLINK 1 // Мигать курсором если он включен  (1 мигать / 0 нет)

#define HD44780_CONFIG_CURSOR_ON 0 // Показывать курсор или нет. 1 показывать / 0 нет

#define HD44780_CONFIG_CURSOR_DIRECTION 1 // Указывает направлене курсора если он включен (1 - вправо / 0 влево)

#define HD44780_CONFIG_CURSOR_LCD_SHIFT 1 // Разрешить сдвиг экрана при сдвиге курсора ( 1 - да / 0 нет)

#define HD44780_CONFIG_FONT_SIZE 1 // Какой размер шрифта (1 - 5x10 / 0 - 5x8)

#define HD44780_CONFIG_BIT 0 // Режим битности (0 - 4bit/ 1 - 8bit)
        // 4bit режим в половину медленее. И памяти ест больше. Так как приходится
        // выполнять дополнительные операции.

#define HD44780_CONFIG_LINES 2 // Сколько строк поддерживается. (0 - одна / 1 - 2 и более)

#define HD44780_CONFIG_INIT_CNT 4 // Сколько раз посылать команду инициализации.
        // Некоторым надо 4, а некоторым и 1 хватает (уменьшение значения так-же
        // освобождает занимаемую память)

#define HD44780_CONFIG_OPTIMIZATION 0 // Оптимизация. Что более критично :
        // Если установить 1 то будет меньше памяти занимать, но будет чуть медленее
        // Если установить 0 то будет занимать больше памяти но чуть быстрее работать


#define HD44780_CONFIG_INITLSTART 2 // Нужна только при работе в 4bit ном режиме. И отвечает за правильную инициализацию.
        // В разных экранах по разному реализовано.
        // 0 означает что линии инициализации идут по одной (это по стандарту)
        // 1 линии инициализации идут по 2 запроса (деление данных на 2 запроса)
        // 2 первая линия это 1 запрос, а потом по 2

// Настройка пауз (если указать 0 то пауза убирается совсем. и уменьшает размер занимаемой памяти)
// Все паузы в наносекундах (1ms = 1000us)
// Я старался делать все паузы по документации. Понятно что чем паузы меньше тем быстрее работает LCD
// Но если выставить их в сличком маленькие числа, возможны глюки. Поэтому если хотите быстрый экран. Подбирайте методом тыка.

#define HD44780_CONFIG_PAUSE_CMD_E 10 //10 Пауза между поднятием и опусканием E флага (нужно чтобы HD44780 замечал команды)

#define HD44780_CONFIG_PAUSE_AFTER_CMD 0 //300 Пауза после любой команды (может быть необходимо если медленный LCD)

#define HD44780_CONFIG_PAUSE_BUSY 0 //300 Пауза для чтения busy флага. В теории и без этой паузы должно работать.

// Паузы влияющие только на скорость запуска.

#define HD44780_CONFIG_PAUSE_CMD_START 1000 //1000 Пауза после открытия портов. Для некоторых LCD это необходимо. 1ms

#define HD44780_CONFIG_PAUSE_START 40000 //40000 Пауза перед инициализацией. По документации надо 40ms

#define HD44780_CONFIG_PAUSE_WRITE_INIT 10 //10 Пауза между поднятием и опусканием E флага при инициализации.

#define HD44780_CONFIG_PAUSE_INIT_STEPONE 100 //4100 Пауза между первой и второй командами инициализации (в документации
        // сказано минимум 4.1ms)

#define HD44780_CONFIG_PAUSE_INIT_STEPTWO 100 //100 Пауза между второй и третьей командами инициализации (в документации
        // сказано минимум 100us)

#define HD44780_CONFIG_PAUSE_INIT_STEPTHREE 100 //100 Пауза между третьей и четверой командами инициализации (в документации
        // сказано минимум 100us)


/*
--------------------------------------------------------------------------------------------------------------------------
    ДАЛЬШЕ ТРОГАТЬ НЕ НУЖНО       ДАЛЬШЕ ТРОГАТЬ НЕ НУЖНО       ДАЛЬШЕ ТРОГАТЬ НЕ НУЖНО       ДАЛЬШЕ ТРОГАТЬ НЕ НУЖНО
--------------------------------------------------------------------------------------------------------------------------
*/

// Дефайны для работы с avr портами итд.

#define HD44780_GLUE(a, b) a##b
#define HD44780__GLUE(a, b, c) a##b##c
#define HD44780_PORT(x) HD44780_GLUE(PORT, x)
#define HD44780__PORTPIN(x, y) HD44780__GLUE(P, x , y)
#define HD44780_PIN(x) HD44780_GLUE(PIN, x)
#define HD44780_DDR(x) HD44780_GLUE(DDR, x)
#define HD44780__DDR_PIN(x, y) HD44780__GLUE(DD, x, y)
#define HD44780_DDR_CONTROL_PIN(x) HD44780__DDR_PIN(HD44780_CONFIG_CONTROL_PORT_NAME, x)
#define HD44780_DDR_DATA_PIN(x) HD44780__DDR_PIN(HD44780_CONFIG_DATA_PORT_NAME, x)
#define HD44780_PORT_CONTROL_PIN(x) HD44780__PORTPIN(HD44780_CONFIG_CONTROL_PORT_NAME, x)
#define HD44780_PORT_DATA_PIN(x) HD44780__PORTPIN(HD44780_CONFIG_DATA_PORT_NAME, x)
#define HD44780_DATA_PORT_NAME HD44780_PORT(HD44780_CONFIG_DATA_PORT_NAME)
#define HD44780_CONTROL_PORT_NAME HD44780_PORT(HD44780_CONFIG_CONTROL_PORT_NAME)
#define HD44780_DATA_DDR_NAME HD44780_DDR(HD44780_CONFIG_DATA_PORT_NAME)
#define HD44780_CONTROL_DDR_NAME HD44780_DDR(HD44780_CONFIG_CONTROL_PORT_NAME)
#define HD44780_DATA_PIN_NAME HD44780_PIN(HD44780_CONFIG_DATA_PORT_NAME)

// Пины для данных

#if HD44780_CONFIG_BIT > 0

	// 8Bit
        #define HD44780_SYS_DATA_DDR_PINS (( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D0_PIN) ) | ( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D1_PIN) ) | ( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D2_PIN) ) | ( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D3_PIN) ) | ( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D4_PIN) ) | ( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D5_PIN) ) | ( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D6_PIN) ) | ( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D7_PIN) ))
        #define HD44780_SYS_DATA_PORT_PINS (( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D0_PIN) ) | ( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D1_PIN) ) | ( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D2_PIN) ) | ( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D3_PIN) ) | ( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D4_PIN) ) | ( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D5_PIN) ) | ( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D6_PIN) ) | ( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D7_PIN) ))

#else

	// 4 bit
        #define HD44780_SYS_DATA_DDR_PINS (( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D4_PIN) ) | ( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D5_PIN) ) | ( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D6_PIN) ) | ( 1 << HD44780_DDR_DATA_PIN(HD44780_CONFIG_D7_PIN) ))
        #define HD44780_SYS_DATA_PORT_PINS (( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D4_PIN) ) | ( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D5_PIN) ) | ( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D6_PIN) ) | ( 1 << HD44780_PORT_DATA_PIN(HD44780_CONFIG_D7_PIN) ))

#endif

#define HD44780_SYS_CONTROL_DDR_PINS (( 1 << HD44780_DDR_CONTROL_PIN(HD44780_CONFIG_RS_PIN) ) | ( 1 << HD44780_DDR_CONTROL_PIN(HD44780_CONFIG_RW_PIN) ) | ( 1 << HD44780_DDR_CONTROL_PIN(HD44780_CONFIG_E_PIN) ))
#define HD44780_SYS_CONTROL_PORT_PINS (( 1 << HD44780_PORT_CONTROL_PIN(HD44780_CONFIG_RS_PIN) ) | ( 1 << HD44780_PORT_CONTROL_PIN(HD44780_CONFIG_RW_PIN) ) | ( 1 << HD44780_PORT_CONTROL_PIN(HD44780_CONFIG_E_PIN) ))
#define HD44780_GET_CONTROL_PORTS(rs, rw, e) ((HD44780_CONTROL_PORT_NAME & ~HD44780_SYS_CONTROL_PORT_PINS) | (( rs << HD44780_PORT_CONTROL_PIN(HD44780_CONFIG_RS_PIN) ) | ( rw << HD44780_PORT_CONTROL_PIN(HD44780_CONFIG_RW_PIN) ) | ( e << HD44780_PORT_CONTROL_PIN(HD44780_CONFIG_E_PIN) ))  )
#define HD44780_GET_CONTROL_DDRS(rs, rw, e) ((HD44780_CONTROL_DDR_NAME & ~HD44780_SYS_CONTROL_DDR_PINS) | (( rs << HD44780_DDR_CONTROL_PIN(HD44780_CONFIG_RS_PIN) ) | ( rw << HD44780_DDR_CONTROL_PIN(HD44780_CONFIG_RW_PIN) ) | ( e << HD44780_DDR_CONTROL_PIN(HD44780_CONFIG_E_PIN) ))  )
#define HD44780_SET_CONTROL_PORTS(rs, rw, e) HD44780_CONTROL_PORT_NAME=HD44780_GET_CONTROL_PORTS(rs, rw, e)
#define HD44780_SET_CONTROL_DDRS(rs, rw, e) HD44780_CONTROL_DDR_NAME=HD44780_GET_CONTROL_DDRS(rs, rw, e)
#define HD44780_SET_DATA_PORTS_NULL HD44780_DATA_PORT_NAME=((HD44780_DATA_PORT_NAME & ~HD44780_SYS_DATA_PORT_PINS) ) // | HD44780_SYS_DATA_PORT_PINS)
#define HD44780_SET_DATA_PORTS_WRITE HD44780_DATA_DDR_NAME=((HD44780_DATA_DDR_NAME & ~HD44780_SYS_DATA_DDR_PINS) | HD44780_SYS_DATA_DDR_PINS)
#define HD44780_SET_DATA_PORTS_READ HD44780_DATA_DDR_NAME=(HD44780_DATA_DDR_NAME & ~HD44780_SYS_DATA_DDR_PINS)

// Переменные с командами HD44780


// Очистка экрана и установка адресного счетчика DDRAM в 0
// 1ms
#define HD44780_INSTRUCTION_CLEAR 0x01

// Устанавливает адресный счетчик DDRAM в 0. Помимо этого, возвращает экран в оригинальное положениее.
// Если он был сдвинут. Содержимое DDRAM остается неизменным.
// 1.52ms
#define HD44780_INSTRUCTION_HOME 0x02

// Позволяет установить направление движения курсора(DB1) и разрешать/запрещать сдвиг экрана(DB0)
// 37 us
#define HD44780_INSTRUCTION_ENTRY_MODE 0x04

// Управление экраном. Включен/выключен (DB2) , Показывать курсор или нет (DB1), Мигать курсором или нет (DB0)
// 37 us
#define HD44780_INSTRUCTION_LCD_CTRL 0x08

// Двигает курсор и сдвигает экран без изменения содержимого DDRAM (DB3 что / DB2 направление)
// 37 us
#define HD44780_INSTRUCTION_SHIFT 0x10

// Функция инициализации. Используется при пером запуске. Потом она не работает.
// Она так-же устанавливает битность (DB4 1=8bit/0=4bit)
// Количество строк на экране (DB3 1=2строки/0=1строка)
// Размер шрифта (DB2 1=5x10 / 0=5x8)
// 37 us
#define HD44780_INSTRUCTION_FUNCTION_SET 0x20

// Устанавливает CGRAM адрес. Обмен данными(приём/передача) происходят сразу после этой команды.
// В DB0-DB5 адрес.
// 37 us
#define HD44780_INSTRUCTION_CGRAM_ADDRESS 0x40

// Устанавливает DDRAM адрес. Обмен данными(приём/передача) происходят сразу после этой команды.
// В DB0-DB6 адрес.
// 37 us
#define HD44780_INSTRUCTION_DDRAM_ADDRESS 0x80


// Флаги (необходимы для работы других дефайнов) HD44780


// Флаг готовности LCD.(принимает-ли он команды или еще нет) Так-же возвращается ссылка на содержимое адресного счетчика.
// Передается вместе с RW
// 0 us
#define HD44780_FLAG_BUSY 0x80   //(1<<HD44780_CONFIG_D7_PIN) // 0x80

// Направление курсора вправо
#define HD44780_FLAG_CURSOR_DIRECTION 0x02

// Разрешить сдвиг экрана со сдвигом курсора
#define HD44780_FLAG_DISPLAY_SHIFT 0x01

// Включение экрана
#define HD44780_FLAG_DISPLAY_ACTIVE 0x04

// Разрешает показ курсора на экране
#define HD44780_FLAG_CURSOR_ACTIVE 0x02

// Мигающий курсор
#define HD44780_FLAG_CURSOR_BLINK 0x01

// Сдвиг экрана
#define HD44780_FLAG_SHIFT_DISPLAY 0x08

// Движение экрана или курсора вправо
#define HD44780_FLAG_SHIFT_DIRECTION 0x04

// Включение 8 битного режима
#define HD44780_FLAG_EIGHT_BIT 0x10

// Включение двух и более строчного режима
#define HD44780_FLAG_TWO_LINES 0x08

// Размер шрифта 5x10
#define HD44780_FLAG_FONT_SIZE 0x04


// Дефайн функции для удобной установки опций


#if HD44780_CONFIG_OPTIMIZATION>0
#define HD44780_INIT_SEND_OPT(a) hd44780_send_command(a,0,0)
#define HD44780_CMD_SEND_OPT(a,b) hd44780_send_command(a,b,1)
#else
#define HD44780_INIT_SEND_OPT(a) hd44780_send_nibble(a)
#define HD44780_CMD_SEND_OPT(a,b) hd44780_send_command(a,b)
#endif


// Установки движения курсора
// Направление движения курсора (direction 0 влево / 1 вправо )
// Сдвиг экрана при перемещении курсора (displayshift 0 запретить / 1 разрешить)
#define HD44780_SET_ENTRY_MODE(direction, displayshift) ( HD44780_INSTRUCTION_ENTRY_MODE | (direction ? HD44780_FLAG_CURSOR_DIRECTION : 0) | (displayshift ? 0 : HD44780_FLAG_DISPLAY_SHIFT) )

// Установки работы экрана
// Состояние экрана (active  0 выключен / 1 включен)
// Показывать или нет курсор (cursor 0 скрыть / 1 показывать)
// Мигать курсором ( 0 нет / 1 да)  Если cursor установлено в 0 то мигание тоже отключается
#define HD44780_SET_LCD_CTRL(active, cursor, blink) ( HD44780_INSTRUCTION_LCD_CTRL | (active ? HD44780_FLAG_DISPLAY_ACTIVE : 0) | (cursor ? HD44780_FLAG_CURSOR_ACTIVE : 0) | (blink ? ( cursor ? HD44780_FLAG_CURSOR_BLINK : 0 ) : 0) )

// Позволяет двигать экран или курсор
// Указывает что двигать (what 0 двигаем курсор / 1 двигаем экран)
// В каком направлении двигаем (direction 0 влево / 1 вправо)
#define HD44780_SET_SHIFT(what, direction) ( HD44780_INSTRUCTION_SHIFT | (what ? HD44780_FLAG_SHIFT_DISPLAY : 0) | (direction ? HD44780_FLAG_SHIFT_DIRECTION : 0) )

// Используется для инициализации. Устанавливает режим битности, количество строк и размер шрифта.
// После инициализации изменить нельзя.
// Режим битности (bitmode 0 = 4bit / 1 = 8bit)
// Количество строк (lines 0 = 1 строка / 1 = 2 и более)
// Размер шрифта (fontsize 0 = 5x8 / 1 = 5x10)
#define HD44780_SET_FUNCTION(bitmode, lines, fontsize) ( HD44780_INSTRUCTION_FUNCTION_SET | ( bitmode ? HD44780_FLAG_EIGHT_BIT : 0 ) | ( lines ? HD44780_FLAG_TWO_LINES : 0 ) | ( fontsize ? HD44780_FLAG_FONT_SIZE : 0 ) )
#define HD44780_SET_FUNCTIONEX(bitmode, lines, fontsize) ( HD44780_INSTRUCTION_FUNCTION_SET | ( bitmode ? HD44780_FLAG_EIGHT_BIT : 0 ) | ( fontsize ? HD44780_FLAG_FONT_SIZE : 0 ) )

// Устанавливает адрес CGRAM. В addr адрес который нужно установить.
#define HD44780_SET_CGRAM_ADDRESS(addr) (HD44780_INSTRUCTION_CGRAM_ADDRESS | addr )

// Устанавливает адрес DDRAM. В addr адрес который нужно установить.
#define HD44780_SET_DDRAM_ADDRESS(addr) (HD44780_INSTRUCTION_DDRAM_ADDRESS | addr )

// Подсчитывает позицию курсора
#define	HD44780_GET_CURSOR_POS(x, y) (x*0x40+y)
//#define HD44780_GET_CURSOR_POS(x, y) (((x & 0×01)<<6) + (( x & 0×02) * 0x0a) + y)

//	Обертки для функций.


// Отправляет установки движения курсора
// Направление движения курсора (direction 0 влево / 1 вправо )
// Сдвиг экрана при перемещении курсора (displayshift 0 запретить / 1 разрешить)
#define HD44780_SEND_ENTRY_MODE(direction, displayshift) HD44780_CMD_SEND_OPT(HD44780_SET_ENTRY_MODE(direction, displayshift), 0)

// Отправляет установки работы экрана
// Состояние экрана (active  0 выключен / 1 включен)
// Показывать или нет курсор (cursor 0 скрыть / 1 показывать)
// Мигать курсором ( 0 нет / 1 да)  Если cursor установлено в 0 то мигание тоже отключается
#define HD44780_SEND_LCD_CTRL(active, cursor, blink) HD44780_CMD_SEND_OPT(HD44780_SET_LCD_CTRL(active, cursor, blink), 0)

// Отправляет команду перемещения экрана или курсора
// Указывает что двигать (what 0 двигаем курсор / 1 двигаем экран)
// В каком направлении двигаем (direction 0 влево / 1 вправо)
#define HD44780_SEND_SHIFT(what, direction) HD44780_CMD_SEND_OPT(HD44780_SET_SHIFT(what, direction), 0)

// Отправляет режим битности, количество строк и размер шрифта.
// После инициализации изменить нельзя.
// Режим битности (bitmode 0 = 4bit / 1 = 8bit)
// Количество строк (lines 0 = 1 строка / 1 = 2 и более)
// Размер шрифта (fontsize 0 = 5x8 / 1 = 5x10)
#define HD44780_SEND_FUNCTION(bitmode, lines, fontsize) HD44780_INIT_SEND_OPT(HD44780_SET_FUNCTION(bitmode, lines, fontsize))

//#if HD44780_CONFIG_BIT==0 && HD44780_CONFIG_INITLSTART==2
//#define HD44780_SEND_FUNCTIONEX(bitmode, lines, fontsize) hd44780_send_nibbleex(HD44780_SET_FUNCTION(bitmode, lines, fontsize))
//#endif

// Устанавливает адрес CGRAM. В addr адрес который нужно установить.
#define HD44780_SEND_CGRAM_ADDRESS(addr) HD44780_CMD_SEND_OPT(HD44780_SET_CGRAM_ADDRESS(addr) ,0)

// Устанавливает адрес DDRAM. В addr адрес который нужно установить.
#define HD44780_SEND_DDRAM_ADDRESS(addr) HD44780_CMD_SEND_OPT(HD44780_SET_DDRAM_ADDRESS(addr) ,0)

// Двигаем курсор
#define HD44780_SEND_SHIFT_CURSOR(direction) HD44780_SEND_SHIFT(0, direction)
#define HD44780_SEND_CURSOR_LEFT HD44780_SEND_SHIFT_CURSOR(0)
#define HD44780_SEND_CURSOR_RIGHT HD44780_SEND_SHIFT_CURSOR(1)

// Двигаем экран
#define HD44780_SEND_SHIFT_DISPLAY(direction) HD44780_SEND_SHIFT(1, direction)
#define HD44780_SEND_DISPLAY_LEFT HD44780_SEND_SHIFT_DISPLAY(0)
#define HD44780_SEND_DISPLAY_RIGHT HD44780_SEND_SHIFT_DISPLAY(1)

// 
#define HD44780_SEND_CMD_HOME HD44780_CMD_SEND_OPT(HD44780_INSTRUCTION_HOME,0)

// Очистка экрана
#define HD44780_SEND_CMD_CLEAR HD44780_CMD_SEND_OPT(HD44780_INSTRUCTION_CLEAR,0)

// Отправка символа
#define HD44780_SEND_CHAR(a) HD44780_CMD_SEND_OPT(a,1)

// Устанавливает курсор по координатам x, y
#define HD44780_SEND_CURSOR_POS(x, y) HD44780_CMD_SEND_OPT(HD44780_SET_DDRAM_ADDRESS(HD44780_GET_CURSOR_POS(x, y)),0)


// Подключаем основные хеадеры
#include <avr/io.h>
#include <util/delay.h>

// Функции

#if HD44780_CONFIG_PINMODE>0
    uint8_t hd44780_pin_convert_to(uint8_t);
    uint8_t hd44780_pin_convert_from(uint8_t);
#endif

uint8_t hd44780_ready();

#if HD44780_CONFIG_OPTIMIZATION<=0
    void hd44780_send_nibble(uint8_t);
#endif


#if HD44780_CONFIG_OPTIMIZATION>0
    void hd44780_send_command(uint8_t,uint8_t,uint8_t);
#else
    void hd44780_send_command(uint8_t,uint8_t);
#endif


//SRG MY

void hd44780_init(void);

inline void lcd_xy(const uint8_t x, const uint8_t y) {
    HD44780_SEND_CURSOR_POS(x, y);
}

inline void lcd_clear(void) {
    HD44780_SEND_CMD_CLEAR;
}

inline void lcd_char(const char c) {
    HD44780_SEND_CHAR(c);
}

void lcd_str(const char *s);
void lcd_str_P(PGM_P pString);
void lcd_hex(const uint8_t byte);


#endif //HD44780_H
