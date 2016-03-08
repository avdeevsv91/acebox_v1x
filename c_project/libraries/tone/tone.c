/*
 * tone.h
 *
 * Работа со звуком
 * Working with the sound.
 */

volatile long _sound_toggle_count;

void tone(unsigned int frequency, unsigned long duration) {
	// Настройка ШИМ
        TCCR2A = 0; TCCR2B = 0;
	TCCR2A |= (1<<WGM21);
	TCCR2B |= (1<<CS20);
	// Рассчитываем частоту и режим тактирования
	uint32_t ocr = F_CPU / frequency / 2 - 1;
	uint8_t prescalarbits = 0b001;
	if(ocr > 255) {
        	ocr = F_CPU / frequency / 2 / 8 - 1;
	        prescalarbits = 0b010;
	        if(ocr > 255) {
			ocr = F_CPU / frequency / 2 / 32 - 1;
			prescalarbits = 0b011;
			if(ocr > 255) {
				ocr = F_CPU / frequency / 2 / 64 - 1;
				prescalarbits = 0b100;
				if(ocr > 255) {
					ocr = F_CPU / frequency / 2 / 128 - 1;
					prescalarbits = 0b101;
					if(ocr > 255) {
						ocr = F_CPU / frequency / 2 / 256 - 1;
						prescalarbits = 0b110;
						if(ocr > 255) {
							// can't do any better than /1024
							ocr = F_CPU / frequency / 2 / 1024 - 1;
							prescalarbits = 0b111;
						}
					}
				}
			}
		}
	}
	// Устанавливаем частоту и режим тактирования ШИМ
	TCCR2B = prescalarbits;
	OCR2A = ocr; // Частота
	// Запускаем таймер
	if(duration > 0) {
		_sound_toggle_count = 2 * frequency * duration / 1000;
	} else {
		_sound_toggle_count = -1;
	}
        TIMSK2 = (1 << OCIE2A);
}

void noTone() {
	_sound_toggle_count = 0;
}

// Таймер (для отключения ШИМ по истичению времени)
ISR(TIMER2_COMPA_vect) {
	if(_sound_toggle_count != 0) {
		PORTD ^= (1<<PD3);
		if(_sound_toggle_count > 0)
			_sound_toggle_count--;
	} else {
		// Отключаем таймер
	        TIMSK2 &= ~(1 << OCIE2A);
		// Отключаем ШИМ
		TCCR2A = (1 << WGM20);
		TCCR2B = (TCCR2B & 0b11111000) | (1 << CS22);
	        OCR2A = 0;
		// Обесточиваем порт
		PORTD &= ~ (1 << PD3);
	}
}
