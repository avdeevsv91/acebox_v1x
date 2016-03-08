/*
 * functions.h
 *
 * Различные вспомогательные функции.
 * The various support functions.
 */


#ifndef MYFUNCTIONS_H_
#define MYFUNCTIONS_H_

#include <avr/interrupt.h> // Прерывания
#include <avr/pgmspace.h> // PROGMEM

#define bool uint8_t
#define true 1
#define false 0
#define TRUE 1
#define FALSE 0

#define PI 3.14159265358979323846

// Чтение INT из PROGMEM
#define pgm_read_int(address_short) __LPM((uint16_t)(address_short))

long map(long, long, long, long, long);
void swap(uint8_t *x, uint8_t *y);

void init_millis();
uint64_t get_millis();

#include "functions.c"

#endif /* MYFUNCTIONS_H_ */
