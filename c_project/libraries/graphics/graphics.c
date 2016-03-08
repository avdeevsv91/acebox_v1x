/*
 * graphics.h
 *
 * Графический модуль для работы с дисплеями на базе контроллера PCD8544.
 * Graphics module for work with displays based on PCD8544 controller
 */


void display_drawBitmap(uint8_t x, uint8_t y, const uint8_t *image, uint8_t width, uint8_t height) {
	uint8_t image_width = (width + 7) / 8;
	for(uint8_t j=0; j<height; j++) {
		for(uint8_t i=0; i<width; i++) {
			if(pgm_read_byte(image + j * image_width + i / 8) & (128 >> (i & 7))) {
				display_drawPixel(x+i, y+j, BLACK);
			} else {
				display_drawPixel(x+i, y+j, WHITE);
			}
		}
	}
}

void display_drawXBitmap(uint8_t x, uint8_t y, const uint8_t *image, uint8_t width, uint8_t height) {
	uint8_t image_width = (width + 7) / 8;
	for(uint8_t j=0; j<height; j++) {
		for(uint8_t i=0; i<width; i++) {
			if(pgm_read_byte(image + j * image_width + i / 8) & (1 << (i % 8))) {
				display_drawPixel(x+i, y+j, BLACK);
			} else {
				display_drawPixel(x+i, y+j, WHITE);
			}
		}
	}
}

void display_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
	if(steep) {
		swap(&x0, &y0);
		swap(&x1, &y1);
	}
	if(x0 > x1) {
		swap(&x0, &x1);
		swap(&y0, &y1);
	}
	uint8_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);
	int8_t err = dx / 2;
	int8_t ystep;
	if(y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}
	for(; x0<=x1; x0++) {
		if(steep) {
			display_drawPixel(y0, x0, color);
		} else {
			display_drawPixel(x0, y0, color);
		}
		err -= dy;
		if(err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void display_drawFastHLine(uint8_t x, uint8_t y, uint8_t width, uint8_t color) {
		display_drawLine(x, y, x+width-1, y, color);
}

void display_drawFastVLine(uint8_t x, uint8_t y, uint8_t height, uint8_t color) {
	display_drawLine(x, y, x, y+height-1, color);
}

void display_drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
	display_drawFastHLine(x, y, width, color);
	display_drawFastHLine(x, y+height-1, width, color);
	display_drawFastVLine(x, y, height, color);
	display_drawFastVLine(x+width-1, y, height, color);
}

void display_drawFillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
	for (uint8_t i=x; i<x+width; i++) {
		display_drawFastVLine(i, y, height, color);
	}
}

void display_drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color) {
	uint8_t f = 1 - radius;
	uint8_t ddF_x = 1;
	uint8_t ddF_y = -2 * radius;
	uint8_t x0 = 0;
	uint8_t y0 = radius;
	display_drawPixel(x, y+radius, color);
	display_drawPixel(x, y-radius, color);
	display_drawPixel(x+radius, y, color);
	display_drawPixel(x-radius, y, color);
	while (x0<y0) {
		if (f >= 0) {
			y0--;
			ddF_y += 2;
			f += ddF_y;
		}
		x0++;
		ddF_x += 2;
		f += ddF_x;
		display_drawPixel(x + x0, y + y0, color);
		display_drawPixel(x - x0, y + y0, color);
		display_drawPixel(x + x0, y - y0, color);
		display_drawPixel(x - x0, y - y0, color);
		display_drawPixel(x + y0, y + x0, color);
		display_drawPixel(x - y0, y + x0, color);
		display_drawPixel(x + y0, y - x0, color);
		display_drawPixel(x - y0, y - x0, color);
	}
}

void display_drawFillCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color) {
	display_drawFastVLine(x, y-radius, 2*radius+1, color);
	display_drawFastHLine(x-radius, y, 2*radius+1, color);
	display_drawFillCircleHelper(x, y, radius, 3, 0, color);
}

void display_drawCircleHelper(uint8_t x, uint8_t y, uint8_t radius, uint8_t cornername, uint8_t color) {
	uint8_t f = 1 - radius;
	uint8_t ddF_x = 1;
	uint8_t ddF_y = -2 * radius;
	uint8_t x0 = 0;
	uint8_t y0 = radius;
	while (x0<y0) {
		if (f >= 0) {
			y0--;
			ddF_y += 2;
			f += ddF_y;
		}
		x0++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4) {
			display_drawPixel(x + x0, y + y0, color);
			display_drawPixel(x + y0, y + x0, color);
		} 
		if (cornername & 0x2) {
			display_drawPixel(x + x0, y - y0, color);
			display_drawPixel(x + y0, y - x0, color);
		}
		if (cornername & 0x8) {
			display_drawPixel(x - y0, y + x0, color);
			display_drawPixel(x - x0, y + y0, color);
		}
		if (cornername & 0x1) {
			display_drawPixel(x - y0, y - x0, color);
			display_drawPixel(x - x0, y - y0, color);
		}
	}
}

void display_drawFillCircleHelper(uint8_t x, uint8_t y, uint8_t radius, uint8_t cornername, uint8_t delta, uint8_t color) {
	uint8_t f = 1 - radius;
	uint8_t ddF_x = 1;
	uint8_t ddF_y = -2 * radius;
	uint8_t x0 = 0;
	uint8_t y0 = radius;
	while (x0<y0) {
		if (f >= 0) {
			y0--;
			ddF_y += 2;
			f += ddF_y;
		}
		x0++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x1) {
			display_drawFastVLine(x+x0, y-y0, 2*y0+1+delta, color);
			display_drawFastVLine(x+y0, y-x0, 2*x0+1+delta, color);
		}
		if (cornername & 0x2) {
			display_drawFastVLine(x-x0, y-y0, 2*y0+1+delta, color);
			display_drawFastVLine(x-y0, y-x0, 2*x0+1+delta, color);
		}
	}
}

void display_drawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color) {
	display_drawFastHLine(x+radius, y, width-2*radius, color);
	display_drawFastHLine(x+radius, y+height-1, width-2*radius, color);
	display_drawFastVLine(x, y+radius, height-2*radius, color);
	display_drawFastVLine(x+width-1, y+radius, height-2*radius, color);
	display_drawCircleHelper(x+radius, y+radius, radius, 1, color);
	display_drawCircleHelper(x+width-radius-1, y+radius, radius, 2, color);
	display_drawCircleHelper(x+width-radius-1, y+height-radius-1, radius, 4, color);
	display_drawCircleHelper(x+radius, y+height-radius-1, radius, 8, color);
}

void display_drawFillRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color) {
	display_drawFillRect(x+radius, y, width-2*radius, height, color);
	display_drawFillCircleHelper(x+width-radius-1, y+radius, radius, 1, height-2*radius-1, color);
	display_drawFillCircleHelper(x+radius, y+radius, radius, 2, height-2*radius-1, color);
}

void display_drawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color) {
	display_drawLine(x0, y0, x1, y1, color);
	display_drawLine(x1, y1, x2, y2, color);
	display_drawLine(x2, y2, x0, y0, color);
}

void display_drawFillTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color) {
	uint8_t a, b, y, last;
	if (y0 > y1) {
		swap(&y0, &y1); swap(&x0, &x1);
	}
	if (y1 > y2) {
		swap(&y2, &y1); swap(&x2, &x1);
	}
	if (y0 > y1) {
		swap(&y0, &y1); swap(&x0, &x1);
	}
	if(y0 == y2) {
		a = b = x0;
		if(x1 < a) a = x1;
		else if(x1 > b) b = x1;
		if(x2 < a) a = x2;
		else if(x2 > b) b = x2;
		display_drawFastHLine(a, y0, b-a+1, color);
		return;
	}
	int8_t
		dx01 = x1 - x0,
		dy01 = y1 - y0,
		dx02 = x2 - x0,
		dy02 = y2 - y0,
		dx12 = x2 - x1,
		dy12 = y2 - y1;
	int16_t
		sa = 0,
		sb = 0;
	if(y1 == y2) last = y1;
	else last = y1-1;
	for(y=y0; y<=last; y++) {
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		if(a > b) swap(&a, &b);
		display_drawFastHLine(a, y, b-a+1, color);
	}
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) {
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if(a > b) swap(&a, &b);
		display_drawFastHLine(a, y, b-a+1, color);
	}
}

void display_drawChar(unsigned char chr, uint8_t x, uint8_t y, uint8_t size, uint8_t color, uint8_t background) {
	if((x < 0) || (y < 0) || (x >= DISPLAY_WIDTH) || (y >= DISPLAY_HEIGHT)) {
		return;
	}
	for (int8_t i=0; i<6; i++ ) {
		uint8_t line;
		if (i == 5) {
			line = 0x0;
		} else {
			line = pgm_read_byte(font+(chr*5)+i);
		}
		for (int8_t j = 0; j<8; j++) {
			if (line & 0x1) {
				if (size == 1) { // default size
					display_drawPixel(x+i, y+j, color);
				} else {  // big size
					display_drawFillRect(x+(i*size), y+(j*size), size, size, color);
				} 
			} else if (background != color) {
				if (size == 1) // default size
					display_drawPixel(x+i, y+j, background);
				else {  // big size
					display_drawFillRect(x+i*size, y+j*size, size, size, background);
				}
			}
			line >>= 1;
		}
	}
}

void display_drawText(char *text, uint8_t x, uint8_t y, uint8_t size, uint8_t color, uint8_t background) {
	uint8_t i = 0;
	unsigned char chr;
	unsigned char line=0;
	while((chr = *text++) != 0x00) {
		if((chr == 0xA) || (chr == 0xD)) { // Перенос строки
			line++;
			i = 0;
		} else {
			display_drawChar(chr, x+i*(6*size), y+(line*8), size, color, background);
			i++;
		}
	}
}

void display_drawInt(int value, uint8_t x, uint8_t y, uint8_t size, uint8_t color, uint8_t background) {
	char buffer[7];
	itoa(value, buffer, 10);
	display_drawText(buffer, x, y, size, color, background);
}
