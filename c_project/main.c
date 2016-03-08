#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/portpins.h>
#include <util/delay.h>
#include <string.h>

#include "libraries/functions/functions.h" // Различные функции
#include "libraries/pcd8544/pcd8544.h" // Библиотека для работы с дисплеем
#include "libraries/graphics/graphics.h" // Графика для дисплея
#include "libraries/interface/interface.h" // Интерфейс
#include "libraries/uart/uart.h" // Последовательный порт
#include "libraries/tone/tone.h" // Работа со звуком

uint8_t getMaskByVDI(int8_t vdi); // Определяет сегмент дискриминации по числу VDI

#include "images.h"  // Изображения

#define BUFFER_SIZE 75 // Размер буфера

// СИСТЕМА
#define CALIBRATION_X 0 // Калибровка точки отсчета по оси X
#define CALIBRATION_Y 1 // Калибровка точки отсчета по оси Y
#define USB_DEBUG     2 // Передавать значения каналов X и Y в COM порт
#define VECTOR_MODE   3 // Векторное отображение годографа (иначе скалярное)
#define ZOOM          4 // Масштаб годограммы
int CONFIG_SYSTEM[] = {-37, -40, false, true, 1};

// ДИСПЛЕЙ
#define CONTRAST  0 // Уровень контраста
#define BACKLIGHT 1 // Уровень подсветки
#define INVERTED 2 // Инверсия цветов
int CONFIG_DISPLAY[] = {40, 0, false};

// ЗВУК
#define KEY_TONES 0 // Звук нажатия клавиш
#define NUM_TONES 1 // Количество тонов озвучки (от 1 до 12), если 0, то озвучка отключена
int CONFIG_SOUND[] = {true, 3};

// ПРОФИЛЬ
#define CURRENT_PROFILE 0 // Номер текущего профиля
int CONFIG_PROFILE[] = {5};

const int8_t CONFIG_VDI_MASK[12][2]  = {{-90,-80}, {-79,-50}, {-49,0}, {1,10}, {11,20}, {21,30}, {31,40}, {41,50}, {51,60}, {61,70}, {71,80}, {81,90}}; // Соответствие сегментов дискриминации числам VDI

const int CONFIG_SOUND_TONES_1 [12] PROGMEM = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Однотональный
const int CONFIG_SOUND_TONES_2 [12] PROGMEM = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Двухтональный
const int CONFIG_SOUND_TONES_3 [12] PROGMEM = {NOTE_F4, NOTE_F4, NOTE_F4, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5}; // Трехтональный
const int CONFIG_SOUND_TONES_4 [12] PROGMEM = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Четырехтональный
const int CONFIG_SOUND_TONES_5 [12] PROGMEM = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Пятитональный
const int CONFIG_SOUND_TONES_6 [12] PROGMEM = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Шеститональный
const int CONFIG_SOUND_TONES_7 [12] PROGMEM = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Семитональный
const int CONFIG_SOUND_TONES_8 [12] PROGMEM = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Восьмитональный
const int CONFIG_SOUND_TONES_9 [12] PROGMEM = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Девятитональный
const int CONFIG_SOUND_TONES_10[12] PROGMEM = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Десятитональный
const int CONFIG_SOUND_TONES_11[12] PROGMEM = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Одиннадцатитональный
const int CONFIG_SOUND_TONES_12[12] PROGMEM = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Двенадцатитональный

const bool CONFIG_PROFILE_1[12] PROGMEM = {true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true }; // Профиль "Все металлы"
const bool CONFIG_PROFILE_2[12] PROGMEM = {false, false, false, true,  true,  true,  true,  true,  true,  true,  true,  true }; // Профиль "Украшения"
const bool CONFIG_PROFILE_3[12] PROGMEM = {false, false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true }; // Профиль "Реликвии"
const bool CONFIG_PROFILE_4[12] PROGMEM = {false, false, false, false, true,  true,  false, true,  true,  true,  true,  true }; // Профиль "Монеты"
      bool CONFIG_PROFILE_5[12]         = {false, false, false, false, false, false, false, false, false, false, false, false}; // Профиль "Пользователь"

int main() {

	/*** Объявление переменных ***/
	static int ChannelsData[BUFFER_SIZE][2]; // Массив данных для каналов X и Y
	static uint8_t cdPoint = 0; // Указатель ячейки массива данных
	static bool ChannelsDataFull = false; // Флаг насыщения массива	
	static int8_t vdi = 0; // Значение VDI
	static int vector_max = 0; // Максимальное значение длины вектора
	static int8_t ResetSettings = -1; // Режим восстановления настроек
	static unsigned int MenuButton=0; static unsigned int MenuButtonMs=0;
	static unsigned int BackButton=0; static unsigned int BackButtonMs=0;
	static unsigned int UpButton=0; static unsigned int UpButtonMs=0;
	static unsigned int DownButton=0; static unsigned int DownButtonMs=0;
	init_millis(); // Инициализация счетчика миллисекунд

	/*** Настройка железа ***/
	sei(); // Разрешаем прерывания
	// Инициализация дисплея
	display_init(0x04);
	display_contrast(0x37);
	display_clear(WHITE);
	display_drawBitmap(0, 0, acebox_logo, 84, 48);
	// Настройка подсветки
	DDRD |= (1 << PD6); // Пин подсветки как выход
	TCCR0A |= (1 << COM0A1); // set none-inverting mode
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // set fast PWM Mode
	TCCR0B |= (1 << CS01); // set prescaler to 8 and starts PWM
	// Кнопки управления
	#define BUTTON_BOUNCE 500
	DDRD &= ~(1 << PD5); // Пин кнопки "Меню" как вход
	DDRC &= ~(1 << PC5); // Пин кнопки "Назад" как вход
	DDRB &= ~(1 << PB0); // Пин кнопки "Вверх" как вход
	DDRD &= ~(1 << PD7); // Пин кнопки "Вниз" как вход
	// Звук
	DDRD |= (1 << PD3); // Пин звука как выход
	// АЦП
	DDRC &= ~(1 << PC1); // Пин канала X как вход
	DDRC &= ~(1 << PC0); // Пин канала Y как вход
	ADCSRA |= (1 << ADEN); // Включаем АЦП
	ADCSRA |= (1 << ADPS1)|(1 << ADPS0); // Предделитель на 8
	ADMUX &= ~(1 << REFS1);	ADMUX |= (1 << REFS0); // Опорное напряжение Vin
	// COM порт
	uart_init(9600);
	/*** Применение настроек для железа ***/
	display_contrast(map(CONFIG_DISPLAY[CONTRAST], 0, 100, 0, 127)); // Контраст дисплея
	OCR0A = map(CONFIG_DISPLAY[BACKLIGHT], 0, 100, 0, 255); // Подсветка дисплея

	/*** Основной цикл ***/
	while(true) {

		/*** Опрос нажатия кнопок ***/
		// Нажата кнопка "Меню"
		if((1 << PD5) & PIND) {
			if (get_millis()-MenuButtonMs > BUTTON_BOUNCE) {
				if(CONFIG_SOUND[KEY_TONES]==true && MenuButton==0) {
					tone(NOTE_AS4, 20);
				}
				MenuButtonMs = get_millis();
				MenuButton++;
			}
		} else { MenuButton = 0; }
		// Нажата кнопка "Назад"
		if(((1 << PC5) & PINC)) {
			if (get_millis()-BackButtonMs > BUTTON_BOUNCE) {
				if(CONFIG_SOUND[KEY_TONES]==true && BackButton==0) {
					tone(NOTE_AS4, 20);
				}
				BackButtonMs = get_millis();
				BackButton++;
			}
		} else { BackButton = 0; }
		// Нажата кнопка "Вверх"
		if((1 << PB0) & PINB) {
			if (get_millis()-UpButtonMs > BUTTON_BOUNCE) {
				if(CONFIG_SOUND[KEY_TONES]==true && UpButton==0) {
					tone(NOTE_AS4, 20);
				}
				UpButtonMs = get_millis();
				UpButton++;
			}
		} else { UpButton = 0; }
		// Нажата кнопка "Вниз"
		if((1 << PD7) & PIND) {
			if (get_millis()-DownButtonMs > BUTTON_BOUNCE) {
				if(CONFIG_SOUND[KEY_TONES]==true && DownButton==0) {
					tone(NOTE_AS4, 20);
				}
				DownButtonMs = get_millis();
				DownButton++;
			}
		} else { DownButton = 0; }

		// Данные с канала X
		ADMUX &= ~(1 << MUX3); ADMUX &= ~(1 << MUX2); ADMUX &= ~(1 << MUX1); ADMUX |= (1 << MUX0); // Выбираем порт ADC1
		ADCSRA |= (1 << ADSC); // Начинаем преобразование
		while ((ADCSRA & (1 << ADIF)) == 0); // Ждем окончания преобразования
		ChannelsData[cdPoint][0] = CONFIG_SYSTEM[CALIBRATION_X] + (ADCL|ADCH << 8) - 512; // Меняeтся в пределах -511...+512
		if(ChannelsData[cdPoint][0]<-511) ChannelsData[cdPoint][0] = -511;
		if(ChannelsData[cdPoint][0]> 512) ChannelsData[cdPoint][0] =  512;

		// Данные с канала Y
		ADMUX &= ~(1 << MUX3); ADMUX &= ~(1 << MUX2); ADMUX &= ~(1 << MUX1); ADMUX &= ~(1 << MUX0); // Выбираем порт ADC0
		ADCSRA |= (1 << ADSC); // Начинаем преобразование
		while ((ADCSRA & (1 << ADIF)) == 0); // Ждем окончания преобразования
		ChannelsData[cdPoint][1] = CONFIG_SYSTEM[CALIBRATION_Y] + abs((ADCL|ADCH << 8)-512); // Менятся в пределах 0...+512
		if(ChannelsData[cdPoint][1]< 0  ) ChannelsData[cdPoint][1] =  0;
		if(ChannelsData[cdPoint][1]> 512) ChannelsData[cdPoint][1] =  512;

		// Работаем, только после насыщения массива и НЕ в режиме восстановления
		if(ChannelsDataFull == true && ResetSettings == -1) {

			/*** Обработка действия кнопок ***/
			if(BackButton>0) { // Кнопка "Назад"
				if(CONFIG_PROFILE[CURRENT_PROFILE]<5) { // Если текущий профиль не последний
					CONFIG_PROFILE[CURRENT_PROFILE] = CONFIG_PROFILE[CURRENT_PROFILE] + 1; // Меняем профиль на следующий
				} else  { // Если текущий профиль последний
					CONFIG_PROFILE[CURRENT_PROFILE] = 1; // Меняем профиль на первый
				}
				BackButton = 0;
			}

			if(UpButton>0) { // Кнопка "Вверх"
				if(CONFIG_SYSTEM[ZOOM]<99) { // Масштаб меньше 9
					CONFIG_SYSTEM[ZOOM] = CONFIG_SYSTEM[ZOOM] + 1; // Увеличиваем масштаб
					display_clear(CONFIG_DISPLAY[INVERTED]); // Очищаем дисплей, чтобы стереть линии годографа
				}
				UpButton = 0;
			}

			if(DownButton>0) { // Кнопка "Вниз"
				if(CONFIG_SYSTEM[ZOOM]>1) { // Масштаб больше 1
					CONFIG_SYSTEM[ZOOM] = CONFIG_SYSTEM[ZOOM] - 1; // Уменьшаем масштаб
					display_clear(CONFIG_DISPLAY[INVERTED]); // Очищаем дисплей, чтобы стереть линии годографа
				}
				DownButton = 0;
			}

			// Отправляем данные с каналов на COM порт
			if(CONFIG_SYSTEM[USB_DEBUG]==true) {
				uart_sendInt(ChannelsData[cdPoint][0]);
				uart_sendText(":");
				uart_sendInt(ChannelsData[cdPoint][1]);
				uart_sendText("\r\n");
			}

			// Определяем длину вектора
			int vector = sqrt(pow(ChannelsData[cdPoint][0], 2) + pow(ChannelsData[cdPoint][1], 2));

				uart_sendInt(vector);
				uart_sendText("\r\n");

			/*** Рисуем значки ***/
			// Иконка профиля
			display_drawFillRect(0, 0, 13, 9, BLACK);
			switch(CONFIG_PROFILE[CURRENT_PROFILE]) {
				case 2: // Профиль "Украшения"
					display_drawText("УК", 1, 1, 1, WHITE, BLACK);
					break;
				case 3: // Профиль "Реликвии"
					display_drawText("РЕ", 1, 1, 1, WHITE, BLACK);
					break;
				case 4: // Профиль "Монеты"
					display_drawText("МО", 1, 1, 1, WHITE, BLACK);
					break;
				case 5: // Профиль "Пользователь"
					display_drawText("ПО", 1, 1, 1, WHITE, BLACK);
					break;
				default: // По умолчанию "Все металлы"
					display_drawText("ВМ", 1, 1, 1, WHITE, BLACK);
			}

			// Иконка масштаба
			display_drawFillRect(64, 0, 19, 9, BLACK);
			if(CONFIG_SYSTEM[ZOOM]<10) {
				display_drawInt(0, 65, 1, 1, WHITE, BLACK);
				display_drawInt(CONFIG_SYSTEM[ZOOM], 71, 1, 1, WHITE, BLACK);
			} else {
				display_drawInt(CONFIG_SYSTEM[ZOOM], 65, 1, 1, WHITE, BLACK);
			}
			display_drawText("х", 77, 1, 1, WHITE, BLACK);

			/*** Рисуем годограф ***/
			// Рисуем разметку годографа
			display_drawFastVLine(41, 15, 32, BLACK); // Вертикальная полоса
			display_drawFastHLine(0, 47, 83, BLACK);  // Горизонтальная полоса
			// Определяем координаты конца отрезка
			uint8_t dxB = map(ChannelsData[cdPoint][0], -511/CONFIG_SYSTEM[ZOOM], 512/CONFIG_SYSTEM[ZOOM], 0,  82);
			uint8_t dyB = map(ChannelsData[cdPoint][1], 0,                        512/CONFIG_SYSTEM[ZOOM], 47, 15);
			uint8_t cdOldPoint;
			// Отрисовка годограммы одним из выбранных в настройках методом
			if(CONFIG_SYSTEM[VECTOR_MODE]==true) { // Векторное отображение
				if(cdPoint>0) { cdOldPoint = cdPoint - 1; } 
				else { cdOldPoint = BUFFER_SIZE-1; }
				uint8_t dxA = map(ChannelsData[cdOldPoint][0], -511/CONFIG_SYSTEM[ZOOM], 512/CONFIG_SYSTEM[ZOOM], 0,  82);
				uint8_t dyA = map(ChannelsData[cdOldPoint][1], 0,                        512/CONFIG_SYSTEM[ZOOM], 47, 15);
				display_drawLine(dxA, dyA, dxB, dyB, BLACK);
				// Стираем устаревший вектор
				if(cdPoint>=BUFFER_SIZE-1) { cdOldPoint = 0; } 
				else { cdOldPoint = cdPoint + 1; }
				dxA = map(ChannelsData[cdOldPoint][0], -511/CONFIG_SYSTEM[ZOOM], 512/CONFIG_SYSTEM[ZOOM], 0,  82);
				dyA = map(ChannelsData[cdOldPoint][1], 0,                        512/CONFIG_SYSTEM[ZOOM], 47, 15);
				if(cdOldPoint>=BUFFER_SIZE-1) { cdOldPoint = 0; } 
				else { cdOldPoint = cdOldPoint + 1; }
				dxB = map(ChannelsData[cdOldPoint][0], -511/CONFIG_SYSTEM[ZOOM], 512/CONFIG_SYSTEM[ZOOM], 0,  82);
				dyB = map(ChannelsData[cdOldPoint][1], 0,                        512/CONFIG_SYSTEM[ZOOM], 47, 15);
				display_drawLine(dxA, dyA, dxB, dyB, WHITE);
				} else { // Скалярное отображение
				// Рисуем новую точку
				display_drawPixel(dxB, dyB, BLACK);
				// Стираем устаревшую точку
				if(cdPoint>=BUFFER_SIZE-1) { cdOldPoint = 0; } 
				else { cdOldPoint = cdPoint + 1; }
				uint8_t dxA = map(ChannelsData[cdOldPoint][0], -511/CONFIG_SYSTEM[ZOOM], 512/CONFIG_SYSTEM[ZOOM], 0,  82);
				uint8_t dyA = map(ChannelsData[cdOldPoint][1], 0,                        512/CONFIG_SYSTEM[ZOOM], 47, 15);
				display_drawPixel(dxA, dyA, WHITE);
			}

			/*** Вычисляем VDI ***/
			unsigned int chYmean = 0; // Среднее значение в канале Y
			for(uint8_t i=0;i<BUFFER_SIZE;i++) { // Просматриваем весь массив значений
				chYmean = chYmean + ChannelsData[i][1]; // И суммируем канал Y
			}
			chYmean = chYmean/BUFFER_SIZE; // Вычисляем среднее арифметическое
			// Рассчитываем VDI, если это в канале Y присутствует сигнал
			if(chYmean>1 && cdPoint>0) { // Вычисляем VDI только при наличии сигнала в канале Y
				if(vector>vector_max) { // Если текущий вектор длинее предыдущего
					vector_max = vector; // Запоминаем длину текущего вектора как максимальную
					vdi = round(atan2(ChannelsData[cdPoint][0], ChannelsData[cdPoint][1])*(180/PI)); // Рассчитываем VDI
				}
			} else { // Если сигнал в канале Y отсутствует
				vdi = 0; // Сбрасываем значение VDI на ноль
				vector_max = 0; // И забываем значение максимальной длины вектора
			}
			// Отображаем число VDI на экране TODO: сделать отображение перегруза
			display_drawFillRect(22, 0, 34, 14, WHITE); // Стираем предыдущее значение
			uint8_t x_coord; // Координата X, по котрой будем рисовать значение VDI
			// Определяем горизонтальное положение надписи для числа VDI
			if(vdi<=-10) { // Отрицательные десятки
				x_coord = 22;
			} else if(vdi<0 && vdi>-10) { // Отрицательные единицы
				x_coord = 30;
			} else if(vdi>0 && vdi<10) { // Положительные единицы
				x_coord = 42;
				display_drawText("+", 30, 0, 2, BLACK, WHITE);
			} else if(vdi>=10) { // Положительные десятки
				x_coord = 34;
				display_drawText("+", 22, 0, 2, BLACK, WHITE);
			} else { // Ноль
				x_coord = 37;
			}
			// Рисуем число VDI
			display_drawInt(vdi, x_coord, 0, 2, BLACK, WHITE);

			/*** Звуковое сопровождение сигналов ***/
			if(CONFIG_SOUND[NUM_TONES]>0) { // Если озвучка включена
				uint8_t vdiMask = getMaskByVDI(vdi); // Определяем сегмент дискриминации по числу VDI
				// Получаем состояние озвучки этого сегмента на основе текущей маски дискриминации
				bool enableVDIsound = false;
				if(vdi!=0 && vdiMask>0) {
					switch(CONFIG_PROFILE[CURRENT_PROFILE]) {
						case 2:   enableVDIsound = pgm_read_int(&CONFIG_PROFILE_2[vdiMask-1]); break; // Профиль "Украшения"
						case 3:   enableVDIsound = pgm_read_int(&CONFIG_PROFILE_3[vdiMask-1]); break; // Профиль "Реликвии"
						case 4:   enableVDIsound = pgm_read_int(&CONFIG_PROFILE_4[vdiMask-1]); break; // Профиль "Монеты"
						case 5:   enableVDIsound = CONFIG_PROFILE_5[vdiMask-1]; break; // Профиль "Пользователь"
						default:  enableVDIsound = pgm_read_int(&CONFIG_PROFILE_1[vdiMask-1]); // По умолчанию "Все металлы"
					}
        			} else { enableVDIsound = false; }
        			// На основе полученных данных воспроизводим звук
        			if(enableVDIsound == true && ChannelsData[cdPoint][1]>1 && _sound_toggle_count==0) { // Если текущий сегмент активен
          				// Определение частоты проигрываемого сигнала
	          			int playingVDItone = 0;
	          			switch(CONFIG_SOUND[NUM_TONES]) {
						case 1:  playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_1[vdiMask-1]);  break; // Один тон
						case 2:  playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_2[vdiMask-1]);  break; // Два тона
						case 4:  playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_4[vdiMask-1]);  break; // Четыре тона
						case 5:  playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_5[vdiMask-1]);  break; // Пять тонов
						case 6:  playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_6[vdiMask-1]);  break; // Шесть тонов
						case 7:  playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_7[vdiMask-1]);  break; // Семь тонов
						case 8:  playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_8[vdiMask-1]);  break; // Восемь тонов
						case 9:  playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_9[vdiMask-1]);  break; // Девять тонов
						case 10: playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_10[vdiMask-1]); break; // Десять тонов
						case 11: playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_11[vdiMask-1]); break; // Одиннадцать тонов
						case 12: playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_12[vdiMask-1]); break; // Двенадцать тонов
						default: playingVDItone = pgm_read_int(&CONFIG_SOUND_TONES_3[vdiMask-1]); // По умолчанию трех-тоналка
					}
					tone(playingVDItone, 150); // Проигрываем сигнал
				}
			}

		} else { // Если массив данных еще не заполнен
			// При включении зажата кнопка "Меню" или активирован режим восстановления
			if((cdPoint==0 && ((1 << PC5) & PINC)) || ResetSettings != -1) {
				// Первоначальный вход в режим восстановления
				if(ResetSettings == -1) {
					ResetSettings = 0; // Отмечаем, что находимся в этом режиме
					display_clear(CONFIG_DISPLAY[INVERTED]); // Очищаем экран
					draw_questionBox("\n  Сброс?", 0, 4, 84, 40, ResetSettings); // Отображаем диалог сброса
				}
				// Нажата кнопка "Меню"
				if(MenuButton>0) {
					MenuButton = 0; // Сбрасываем флаг нажатия кнопки
					if(ResetSettings) {
						// Сохраняем стандартные настройки в память TODO
					}
					_delay_ms(25); // Чтобы успел проиграться звук нажатия кнопки
					ATmegaReset(); // Перезагружаем МК
				}
				// Нажата кнопка "Вверх" или "Вниз"
				if((UpButton>0)||(DownButton>0)) {
					UpButton = 0; DownButton = 0; // Сбрасываем флаг нажатия кнопок
					ResetSettings = !ResetSettings; // Меняем выбранное действие
					display_clear(CONFIG_DISPLAY[INVERTED]); // Очищаем экран
					draw_questionBox("\n  Сброс?", 0, 4, 84, 40, ResetSettings); // Отображаем диалог сброса
				}
			} else { // Обычная загрузка прибора
				if(cdPoint==0) { // Первый виток цикла
					// TODO загружаем настройки из памяти
				}
				// Показываем (или увеличиваем) полосу загрузки
				display_drawFillRect(2, 45, ((cdPoint+1)*79/BUFFER_SIZE)+1, 2, BLACK);
			}
		}
		// Управление указетелем ячейки массива
		if(cdPoint<BUFFER_SIZE-1) { // Если не дошли до конца массива
			cdPoint = cdPoint + 1; // Увеличиваем значение указателя
		} else { // Если дошли до конца массива
			cdPoint = 0; // Обнуляем значение указателя
			// Если массив не заполнен и мы не в режиме восстановления
			if(ChannelsDataFull == false && ResetSettings == -1) {
				display_update(CONFIG_DISPLAY[INVERTED]); // Обновлянм экран
				_delay_ms(500); // Небольшая задержка заставки
				display_clear(CONFIG_DISPLAY[INVERTED]); // Очищаем экран
				ChannelsDataFull = true; // Устанавливаем флаг насыщения массива в истину
			}
		}
		// Обновляем экран
		display_update(CONFIG_DISPLAY[INVERTED]);
	}
	return 0; // Конец программы
}

// Определяет сегмент дискриминации по числу VDI
uint8_t getMaskByVDI(int8_t vdi) {
	for(uint8_t i=0;i<12;i++) {
		if(vdi >= CONFIG_VDI_MASK[i][0] && vdi <= CONFIG_VDI_MASK[i][1]) {
			return i+1;
		}
	}
	return 0;
}
