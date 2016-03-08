/*
 * uart.c
 *
 * Работа с последовательным портом.
 * Working with the serial port.
 */

void uart_init(long speed) {
	DDRD &= ~(1 << PD0); // Пин RX как вход
	DDRD |= (1 << PD1); // Пин TX как выход
	speed = (16000000/(speed * 16))-1; // Скорость порта
	UBRR0H = (unsigned char)(speed>>8);
	UBRR0L = (unsigned char)(speed);
	UCSR0C |= (1 << UMSEL01); // Для доступа к регистру UCSRC
	UCSR0C &= ~(1 << UCSZ02); UCSR0C |= (1 << UCSZ01)|(1 << UCSZ00); // 8 бит
	UCSR0C &= ~(1 << UPM01); UCSR0C &= ~(1 << UPM00); // Без контроля четности
	UCSR0C &= ~(1 << USBS0); // 1 стоп бит
	UCSR0B = (1 << TXEN0)|(1<< RXEN0); // Разрешаем передачу данных
}

unsigned char uart_getChar() {
	// Ждем окончание приема байта
	while((UCSR0A & (1 << RXC0)) == 0);
	// Принимаем данные
	return UDR0;
}

void uart_sendChar(unsigned char chr) {
	// Ждем окончание передачи предыдущего байта
	while ((UCSR0A & (1 << UDRE0)) == 0);
	// Передаем данные
	UDR0 = chr;
}

void uart_sendText(char *text) {
	unsigned char chr;
	while((chr = *text++) != 0x00) {
		uart_sendChar(chr);
	}
}

void uart_sendInt(int value) {
	char buffer[7];
	itoa(value, buffer, 10);
	uart_sendText(buffer);
}
