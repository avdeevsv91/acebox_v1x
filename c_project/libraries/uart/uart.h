/*
 * uart.h
 *
 * Работа с последовательным портом.
 * Working with the serial port.
 */


#ifndef UART_H_
#define UART_H_

void uart_init(long speed);
unsigned char uart_getChar();
void uart_sendChar(unsigned char chr);
void uart_sendText(char *text);
void uart_sendInt(int value);

#include "uart.c"

#endif /* UART_H_ */
