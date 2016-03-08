/*
 * interface.c
 *
 * Функции создания интерфейса.
 * Functions to create the interface.
 */

void draw_questionBox(char *text, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t selected) {
	if(width<80) width = 80; // Ограничение минимальной ширины
	if(height<26) height = 26; // Ограничение максимальной высоты
	// Рамка
	display_drawFillRoundRect(x, y, width, height, 5, WHITE);
	display_drawRoundRect(x, y, width, height, 5, BLACK);
	// Иконка
	display_drawBitmap(x+2, y+(height-18)/2, questionIcon, 22, 18);
	// Надпись
	display_drawText(text, x+25, y+3, 1, BLACK, WHITE);
	// Кнопка "Нет"
	display_drawFillRect(x+26+((width-27-46-3)/2), y+height-14, 21, 11, (selected?WHITE:BLACK));
	display_drawText("Нет", x+28+((width-27-46-3)/2), y+height-12, 1, (selected?BLACK:WHITE), (selected?WHITE:BLACK));
	display_drawRect(x+26+((width-27-46-3)/2), y+height-14, 21, 11, BLACK);
	// Кнопка "Да"
	display_drawFillRect(x+51+((width-27-46-3)/2), y+height-14, 21, 11, (selected?BLACK:WHITE));
	display_drawText("Да", x+56+((width-27-46-3)/2), y+height-12, 1, (selected?WHITE:BLACK), (selected?BLACK:WHITE));
	display_drawRect(x+51+((width-27-46-3)/2), y+height-14, 21, 11, BLACK);
}
