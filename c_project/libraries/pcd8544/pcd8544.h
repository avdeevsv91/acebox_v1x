/*
 * pcd8544.h
 *
 * Модуль для работы с дисплеями на базе контроллера PCD8544.
 * Module for work with displays based on PCD8544 controller
 */ 


#ifndef PCD8544_H_
#define PCD8544_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/portpins.h>
#include <util/delay.h>
#include <string.h>

#include "../functions/functions.h"

// Порт и номер пина, на который выводится !RESET сигнал
// Port and Pin number for !RESET signal
#define PCD8544_RESET_PORT PORTD
#define PCD8544_RESET_PIN_DDR DDRD
#define PCD8544_RESET_PIN_NUM PD2
#define PCD8544_RESET (1 << PCD8544_RESET_PIN_NUM)

// Порт и номер пина, на который выводится !SCE (CS) сигнал
// Port and Pin number for !SCE (CS) signal
#define PCD8544_SCE_PORT PORTD
#define PCD8544_SCE_DDR DDRD
#define PCD8544_SCE_PIN_NUM PD4
#define PCD8544_SCE (1 << PCD8544_SCE_PIN_NUM)

// Порт и номер пина, на который выводится D/!C сигнал
// Port and Pin number for D/!C signal
#define PCD8544_DC_PORT PORTB
#define PCD8544_DC_DDR DDRB
#define PCD8544_DC_PIN_NUM PB1
#define PCD8544_DC (1 << PCD8544_DC_PIN_NUM)

// Порт и номер пина, на который выводится сигнал MOSI (DIN на дисплее) 
// Port and Pin number for software MOSI (DIN on display controller) signal
#define PCD8544_MOSI_PORT PORTB
#define PCD8544_MOSI_DDR DDRB
#define PCD8544_MOSI_PIN_NUM PB3
#define PCD8544_MOSI (1 << PCD8544_MOSI_PIN_NUM)

// Порт и номер пина, на который выводится сигнал SCK
// Port and Pin number for software SCK signal
#define PCD8544_SCK_PORT PORTB
#define PCD8544_SCK_DDR DDRB
#define PCD8544_SCK_PIN_NUM PB5
#define PCD8544_SCK (1 << PCD8544_SCK_PIN_NUM)

// Определить для программного формирования
// При аппаратном SPI порты PB2 и PB4 не подключаются,
// но использовать их для своих целей нельзя!
// If defined, then software output
//#define PCD8544_SOFTWARE

#define DISPLAY_WIDTH 84
#define DISPLAY_PAGES 6
#define DISPLAY_HEIGHT (DISPLAY_PAGES * 8)

#define PCD8544_EXTENDEDINSTRUCTION 0x01
#define PCD8544_SETTEMP 0x04
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETBIAS 0x10
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80
#define PCD8544_SETVOP 0x80

#define BLACK 1
#define WHITE 0

/* Инициализирует дисплей
 * Initializes the display. */
void display_init(uint8_t bias);

/* Передаёт команду дисплею 
 * Transmits a command to display controller */
void display_cmd(uint8_t t);

/* Передаёт графические данные на дисплей 
 * Transmits a graphical data byte */
void display_data(uint8_t t);

/* Устанавливает уровень контраста
 * Set contrast level */
void display_contrast(uint8_t contrast);

/* Выбирает страницу и горизонтальную позицию для вывода
 * page - страница от 0 до 5
 * x - позиция от 0 до 83 
 * 
 * Selects both page and horizontal position at once
 * */
void display_setpos(uint8_t page, uint8_t x);

/* Очищает экран, устанавливает курсор в левый верхний угол 
 * Clears the screen. Moves cursor to the left upper corner */
void display_clear(uint8_t color);

/* Обновляет изображение на экране
 * Update display image */
void display_update(uint8_t invert);

/* Рисует пиксель на экране
 * Draw pixel on the display */
void display_drawPixel(uint8_t x, uint8_t y, uint8_t color);

/* Получает значение цвета пикселя с экрана
 * Get pixel color from the display */
uint8_t display_getPixel(uint8_t x, uint8_t y);

#include "pcd8544.c"

#endif /* PCD8544_H_ */
