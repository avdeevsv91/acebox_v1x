/*
 * graphics.h
 *
 * Графический модуль для работы с дисплеями на базе контроллера PCD8544.
 * Graphics module for work with displays based on PCD8544 controller
 */


#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <stdlib.h>
#include "../functions/functions.h"
#include "../pcd8544/pcd8544.h"

#include "glcdfont.h"

void display_drawBitmap(uint8_t x, uint8_t y, const uint8_t *image, uint8_t width, uint8_t height); // Изображение
void display_drawXBitmap(uint8_t x, uint8_t y, const uint8_t *image, uint8_t width, uint8_t height); // Изображение GIMP
void display_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color); // Линия
void display_drawFastHLine(uint8_t x, uint8_t y, uint8_t width, uint8_t color); // Горизонтальная линия
void display_drawFastVLine(uint8_t x, uint8_t y, uint8_t height, uint8_t color); // Вертикальная линия
void display_drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color); // Прямоугольник
void display_drawFillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color); // Закрашенный прямоугольник
void display_drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color); // Ромб
void display_drawFillCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color); // Закрашенный ромб
void display_drawCircleHelper(uint8_t x, uint8_t y, uint8_t radius, uint8_t cornername, uint8_t color); // Неполный ромб
void display_drawFillCircleHelper(uint8_t x, uint8_t y, uint8_t radius, uint8_t cornername, uint8_t delta, uint8_t color); // Неполный закрашенный ромб
void display_drawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color); // Прямоугольник с закругленными углами
void display_drawFillRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color); // Закрашенный прямоугольник с закругленными углами
void display_drawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color); // Треугольник
void display_drawFillTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color); // Закрашенный треугольник
void display_drawChar(unsigned char chr, uint8_t x, uint8_t y, uint8_t size, uint8_t color, uint8_t background); // Один символ
void display_drawText(char *text, uint8_t x, uint8_t y, uint8_t size, uint8_t color, uint8_t background); // Текстовая надпись
void display_drawInt(int value, uint8_t x, uint8_t y, uint8_t size, uint8_t color, uint8_t background); // Число

#include "graphics.c"

#endif /* GRAPHICS_H_ */
