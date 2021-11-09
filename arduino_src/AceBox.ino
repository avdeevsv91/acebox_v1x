/*
  AceBox (version 0.1)
  Author: Avdeev Sergey
  E-Mail: avdeevsv91@gmail.com
  URL: https://github.com/kasitoru/acebox_v1x
*/

#define LCD_HARDWARE_SPI   // Использовать аппаратный SPI для дисплея

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
//#include <Menu_PCD8544.h>
#include <Bounce.h>
//#include <EEPROM.h>
#include <math.h>

#include "/home/soultaker/Arduino/AceBox/pitches.h"      // Ноты
#include "/home/soultaker/Arduino/AceBox/acebox_logo.c"  // Логотип

/*** Настройка выводов микроконтроллера ***/
#define PIN_MD_CHX   A1 // Канал X металлоискателя
#define PIN_MD_CHY   A0 // Канал Y металлоискателя
#define PIN_BUTTON_1 5  // Кнопка 1
#define PIN_BUTTON_2 A7 // Кнопка 2
#define PIN_BUTTON_3 8  // Кнопка 3
#define PIN_BUTTON_4 7  // Кнопка 4
#define PIN_SOUND    3  // Звуковой выход (ШИМ)
#define PIN_LCD_LED  6  // Подсветка дисплея (ШИМ)
#define PIN_LCD_RST  2  // RST вывод дисплея
#define PIN_LCD_CE   4  // CE вывод дисплея
#define PIN_LCD_DIN  11 // DIN вывод дисплея
#define PIN_LCD_DC   9  // DC вывод дисплея
#define PIN_LCD_CLK  13 // CLK вывод дисплея

/*** Настройки программы ***/

// СИСТЕМА
#define BUFFER_SIZE   0 // Размер буфера (от 20 до 200)
#define CALIBRATION_X 1 // Калибровка точки отсчета по оси X
#define CALIBRATION_Y 2 // Калибровка точки отсчета по оси Y
#define USB_DEBUG     3 // Передавать значения каналов X и Y в COM порт
#define VECTOR_MODE   4 // Векторное отображение годографа (иначе скалярное)
#define ZOOM          5 // Масштаб годограммы
int CONFIG_SYSTEM[] = {75, -37, -40, true, true, 1};

// ДИСПЛЕЙ
#define CONTRAST  0 // Уровень контраста
#define BACKLIGHT 1 // Уровень подсветки
int CONFIG_DISPLAY[] = {40, 50};

// ЗВУК
#define KEY_TONES 0 // Звук нажатия клавиш
#define NUM_TONES 1 // Количество тонов озвучки (от 1 до 12), если 0, то озвучка отключена
int CONFIG_SOUND[] = {true, 0};

// ПРОФИЛЬ
#define CURRENT_PROFILE 0 // Номер текущего профиля
int CONFIG_PROFILE[] = {5};

const int8_t CONFIG_VDI_MASK[12][2]  = {{-90,-80}, {-79,-50}, {-49,0}, {1,10}, {11,20}, {21,30}, {31,40}, {41,50}, {51,60}, {61,70}, {71,80}, {81,90}}; // Соответствие сегментов дискриминации числам VDI

//const int CONFIG_SOUND_TONES_1[12]  = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Однотональный
//const int CONFIG_SOUND_TONES_2[12]  = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Двухтональный
const int CONFIG_SOUND_TONES_3[12]  = {NOTE_F4, NOTE_F4, NOTE_F4, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5}; // Трехтональный
//const int CONFIG_SOUND_TONES_4[12]  = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Четырехтональный
//const int CONFIG_SOUND_TONES_5[12]  = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Пятитональный
//const int CONFIG_SOUND_TONES_6[12]  = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Шеститональный
//const int CONFIG_SOUND_TONES_7[12]  = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Семитональный
//const int CONFIG_SOUND_TONES_8[12]  = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Восьмитональный
//const int CONFIG_SOUND_TONES_9[12]  = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Девятитональный
//const int CONFIG_SOUND_TONES_10[12] = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Десятитональный
//const int CONFIG_SOUND_TONES_11[12] = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Одиннадцатитональный
//const int CONFIG_SOUND_TONES_12[12] = {NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3, NOTE_A3}; // Двенадцатитональный

//const boolean CONFIG_PROFILE_1[12]  = {true,  true,  true,  true,  true,  true,  true,  true,  true, true, true, true}; // Профиль "Все металлы"
//const boolean CONFIG_PROFILE_2[12]  = {true,  true,  true,  true,  true,  true,  true,  true,  true, true, true, true}; // Профиль "Украшения"
//const boolean CONFIG_PROFILE_3[12]  = {true,  true,  true,  true,  true,  true,  true,  true,  true, true, true, true}; // Профиль "Реликвии"
//const boolean CONFIG_PROFILE_4[12]  = {true,  false,  true,  true,  true,  true,  true,  true,  true, true, true, true}; // Профиль "Монеты"
      boolean CONFIG_PROFILE_5[12]  = {true,  true,  true,  true,  true,  true,  true,  true,  true, true, true, true}; // Профиль "Пользователь"

/*** Клавиши управления ***/
Bounce ButtonMenu = Bounce(PIN_BUTTON_1, 250); // Меню
Bounce ButtonBack = Bounce(PIN_BUTTON_2, 250); // Назад
Bounce ButtonUp   = Bounce(PIN_BUTTON_3, 250); // Вверх
Bounce ButtonDown = Bounce(PIN_BUTTON_4, 250); // Вниз

/*** Дисплей ***/
#ifdef LCD_HARDWARE_SPI // Аппаратный SPI для дисплея
  Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_LCD_DC, PIN_LCD_CE, PIN_LCD_RST); 
#else // Программный SPI для дисплея
  Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_LCD_CLK, PIN_LCD_DIN, PIN_LCD_DC, PIN_LCD_CE, PIN_LCD_RST);
  #define LCD_SOFTWARE_SPI
#endif

/*** Меню ***/
/* FIXME
#define NUM_MENU_BUFFSIZE_VALUES 19
MVal menuBuffSizeValues[NUM_MENU_BUFFSIZE_VALUES] = {{"20",20},{"30",30},{"40",40},{"50",50},{"60",60},{"70",70},{"80",80},{"90",90},{"100",100},{"110",110},{"120",120},{"130",130},{"140",140},{"150",150},{"160",160},{"170",170},{"180",180},{"190",190},{"200",200}};

#define NUM_MENU_USBDEBUG_VALUES 2
MVal menuUSBdebugValues[NUM_MENU_USBDEBUG_VALUES] = {{"Disable",0},{"Enable",1}};

#define NUM_MENU_VECTORMODE_VALUES 2
MVal menuVectorModeValues[NUM_MENU_VECTORMODE_VALUES] = {{"Disable",0},{"Enable",1}};

#define NUM_MENU_CALIBRATIONX_VALUES 21
MVal menuCalibrationXValues[NUM_MENU_CALIBRATIONX_VALUES] = {{"-10",-10},{"-9",-9},{"-8",-8},{"-7",-7},{"-6",-6},{"-5",-5},{"-4",-4},{"-3",-3},{"-2",-2},{"-1",-1},{"0",0},{"+1",1},{"+2",2},{"+3",3},{"+4",4},{"+5",5},{"+6",6},{"+7",7},{"+8",8},{"+9",9},{"+10",10}};

#define NUM_MENU_CALIBRATIONY_VALUES 21
MVal menuCalibrationYValues[NUM_MENU_CALIBRATIONY_VALUES] = {{"-10",-10},{"-9",-9},{"-8",-8},{"-7",-7},{"-6",-6},{"-5",-5},{"-4",-4},{"-3",-3},{"-2",-2},{"-1",-1},{"0",0},{"+1",1},{"+2",2},{"+3",3},{"+4",4},{"+5",5},{"+6",6},{"+7",7},{"+8",8},{"+9",9},{"+10",10}};

#define NUM_MENU_CONTRAST_VALUES 13
MVal menuContrastValues[NUM_MENU_CONTRAST_VALUES] = {{"40%",40},{"45%",45},{"50%",50},{"55%",55},{"60%",60},{"65%",65},{"70%",70},{"75%",75},{"80%",80},{"85%",85},{"90%",90},{"95%",95},{"100%",100}};

#define NUM_MENU_BACKLIGHT_VALUES 11
MVal menuBacklightValues[NUM_MENU_BACKLIGHT_VALUES] = {{"Disable",0},{"10%",10},{"20%",20},{"30%",30},{"40%",40},{"50%",50},{"60%",60},{"70%",70},{"80%",80},{"90%",90},{"100%",100}};

#define NUM_MENU_KEYTONES_VALUES 2
MVal menuKeyTonesValues[NUM_MENU_KEYTONES_VALUES] = {{"Disable",0},{"Enable",1}};

#define NUM_MENU_VDITONES_VALUES 13
MVal menuVDItonesValues[NUM_MENU_VDITONES_VALUES] = {{"Disable",0},{"1-tone",1},{"2-tones",2},{"3-tones",3},{"4-tones",4},{"5-tones",5},{"6-tones",6},{"7-tones",7},{"8-tones",8},{"9-tones",9},{"10-tones",10},{"11-tones",11},{"12-tones",13}};

#define NUM_MENU_CURRPROFILE_VALUES 5
MVal menuCurrProfileValues[NUM_MENU_CURRPROFILE_VALUES] = {{"All metals",1},{"Jewelry",2},{"Relics",3},{"Coins",4},{"Custom",5}};

#define NUM_MENU_ITEMS 17
MItm MenuItems[NUM_MENU_ITEMS] = {
  MItm("MENU", 0),
    MItm("System", 1, 0),
      MItm("Buffer size", 10, 1, NULL, menuBuffSizeValues, NUM_MENU_BUFFSIZE_VALUES, CFG_SYS_BUFF_SIZE),
      MItm("USB debug",   11, 1, NULL, menuUSBdebugValues, NUM_MENU_USBDEBUG_VALUES, CFG_SYS_COM_XY),
      MItm("Vector mode", 12, 1, NULL, menuVectorModeValues, NUM_MENU_VECTORMODE_VALUES, CFG_HODO_VECTOR),
      MItm("Calibration", 13, 1),
        MItm("Channel X", 130, 13, NULL, menuCalibrationXValues, NUM_MENU_CALIBRATIONX_VALUES, CFG_HODO_X_CALIBRATION),
        MItm("Channel Y", 131, 13, NULL, menuCalibrationYValues, NUM_MENU_CALIBRATIONY_VALUES, CFG_HODO_Y_CALIBRATION),
    MItm("Display", 2, 0),
      MItm("Contrast",  20, 2, NULL, menuContrastValues, NUM_MENU_CONTRAST_VALUES, CFG_DSP_CONTRAST),
      MItm("Backlight", 21, 2, NULL, menuBacklightValues, NUM_MENU_BACKLIGHT_VALUES, CFG_DSP_BACKLIGHT),
    MItm("Sound", 3, 0),
      MItm("Key tones", 30, 3, NULL, menuKeyTonesValues, NUM_MENU_KEYTONES_VALUES, CFG_SND_BUTTONS),
      MItm("VDI tones", 31, 3, NULL, menuVDItonesValues, NUM_MENU_VDITONES_VALUES, CFG_SND_VDI_CURRENT_TONE),
    MItm("Profile", 4, 0),
      MItm("Current profile", 40, 4, NULL, menuCurrProfileValues, NUM_MENU_CURRPROFILE_VALUES, CFG_PRF_CURRENT_PROFILE),
      MItm("Edit profile",    41, 4, MenuEditProfileCallback),
    MItm("About...", 5, 0, MenuAboutCallback)
};
Menu_PCD8544 menu(MenuItems, NUM_MENU_ITEMS, &display, MenuOpenCallback, MenuCloseCallback, MenuConfigChangeCallback);

// Вызывается при открытии меню
void MenuOpenCallback() {
  return;
}

// Вызывается при закрытии меню
void MenuCloseCallback() {
  saveSettings();
  return;
}

// Вызывается при изменении настроек
void MenuConfigChangeCallback(int item, int value) {
  switch(item) {
    
    default: // none
  }
  return;
}

// Вызывается при открытии пункта "Edit profile"
void MenuEditProfileCallback() {
  return;
}

// Вызывается при открытии пункта "About"
void MenuAboutCallback() {
  return;
}
*/

/*** Инициализация настроек ***/
void setup() {
  // Загружаем настройки из EEPROM
  loadSettings();
  // Каналы X и Y
  pinMode(PIN_MD_CHX, INPUT);
  pinMode(PIN_MD_CHY, INPUT);
  // Клавиши управления
  pinMode(PIN_BUTTON_1, INPUT);
  pinMode(PIN_BUTTON_2, INPUT);
  pinMode(PIN_BUTTON_3, INPUT);
  pinMode(PIN_BUTTON_4, INPUT);
  // Звуковой выход
  pinMode(PIN_SOUND, OUTPUT);
  // Дисплей
  pinMode(PIN_LCD_LED, OUTPUT);
  analogWrite(PIN_LCD_LED, map(CONFIG_DISPLAY[BACKLIGHT], 0, 100, 0, 255)); // Устанавливаем подсветку
  display.begin();
  #ifdef LCD_HARDWARE_SPI // Настройки аппаратного SPI
    SPI.setClockDivider(SPI_CLOCK_DIV8);
  #endif
  display.clearDisplay();
  display.setContrast(map(CONFIG_DISPLAY[CONTRAST], 0, 100, 0, 127)); // Устанавливаем контраст
  display.drawBitmap(0, 0, acebox_logo, 84, 48, BLACK);
  // COM порт
  Serial.begin(9600);
}

/*** Основной цикл ***/
void loop() {
  // Объявление переменных
  static int     ChannelsData[200][2];     // Массив данных для каналов X и Y
  static uint8_t cdPoint          = 0;     // Указатель ячейки массива данных
  static boolean ChannelsDataFull = false; // Флаг насыщения массива
  static int8_t vdi = 0;                   // Значение VDI
  static int vector_max = 0;               // Максимальное значение длины вектора
  // Получаем значения с каналов X и Y и вписываем их в нужные нам границы
  ChannelsData[cdPoint][0] = CONFIG_SYSTEM[CALIBRATION_X] + analogRead(PIN_MD_CHX)-512; // Меняeтся в пределах -511...+512
  if(ChannelsData[cdPoint][0]<-511) ChannelsData[cdPoint][0] = -511;
  if(ChannelsData[cdPoint][0]> 512) ChannelsData[cdPoint][0] =  512;
  ChannelsData[cdPoint][1] = CONFIG_SYSTEM[CALIBRATION_Y] + abs(analogRead(PIN_MD_CHY)-512); // Менятся в пределах 0...+512
  if(ChannelsData[cdPoint][1]< 0  ) ChannelsData[cdPoint][1] =  0;
  if(ChannelsData[cdPoint][1]> 512) ChannelsData[cdPoint][1] =  512;
  // Определяем длину вектора
  int vector = sqrt(pow(ChannelsData[cdPoint][0], 2) + pow(ChannelsData[cdPoint][1], 2));
  // Отправляем их на COM порт
  if(CONFIG_SYSTEM[USB_DEBUG]==true) {
    Serial.print(ChannelsData[cdPoint][0]);
    Serial.print(":");
    Serial.print(ChannelsData[cdPoint][1]);
    Serial.println("");
  }
  // Работаем, только после насыщения массива
  if(ChannelsDataFull==true) {
    /*** Получаем состояние кнопок ***/
    // Кнопка "Меню"
    static boolean ButtonMenuPushed = false;
    ButtonMenu.update(); if(ButtonMenu.read()==HIGH && ButtonMenuPushed==false) {
      ButtonMenuPushed = true;
      if(CONFIG_SOUND[KEY_TONES]==true) {
        tone(PIN_SOUND, NOTE_AS4, 20);
      }
    } else if(ButtonMenu.read()==LOW && ButtonMenuPushed==true) {
      ButtonMenuPushed = false;
    }
    // Кнопка "Назад"
    static boolean ButtonBackPushed = false;
    ButtonBack.update(); if(ButtonBack.read()==HIGH && ButtonBackPushed==false) {
      ButtonBackPushed = true;
      if(CONFIG_SOUND[KEY_TONES]==true) {
        tone(PIN_SOUND, NOTE_AS4, 20);
      }
    } else if(ButtonBack.read()==LOW && ButtonBackPushed==true) {
      ButtonBackPushed = false;
    }
    // Кнопка "Вверх"
    static boolean ButtonUpPushed = false;
    ButtonUp.update(); if(ButtonUp.read()==HIGH && ButtonUpPushed==false) {
      ButtonUpPushed = true;
      if(CONFIG_SOUND[KEY_TONES]==true) {
        tone(PIN_SOUND, NOTE_AS4, 20);
      }
    } else if(ButtonUp.read()==LOW && ButtonUpPushed==true) {
      ButtonUpPushed = false;
    }
    // Кнопка "Вниз"
    static boolean ButtonDownPushed = false;
    ButtonDown.update(); if(ButtonDown.read()==HIGH && ButtonDownPushed==false) {
      ButtonDownPushed = true;
      if(CONFIG_SOUND[KEY_TONES]==true) {
        tone(PIN_SOUND, NOTE_AS4, 20);
      }
    } else if(ButtonDown.read()==LOW && ButtonDownPushed==true) {
      ButtonDownPushed = false;
    }
    // Пользователь находится в меню
    if(false) { //if(menu.current_menu>-1) {
      /*** Обработка кнопок ***/
      /* FIXME
      // Нажата клавиша "Меню"
      if(ButtonMenuPushed==true) {
        menu.enter();
        ButtonMenu.write(LOW);
      }
      // Нажата клавиша "Назад"
      if(ButtonBackPushed==true) {
        menu.back();
        ButtonBack.write(LOW);
      }
      // Нажата клавиша "Вверх"
      if(ButtonUpPushed==true) {
        menu.up();
        ButtonUp.write(LOW);
      }
      // Нажата клавиша "Вниз"
      if(ButtonDownPushed==true) {
        menu.down();
        ButtonDown.write(LOW);
      }
      */
    } else { // Пользователь в режиме годографа
      /*** Обработка кнопок ***/
      // Нажата клавиша "Меню"
      if(ButtonMenuPushed==true) {
        //menu.open(); FIXME
        ButtonMenu.write(LOW);
      }
      // Нажата клавиша "Назад"
      if(ButtonBackPushed==true) {
        /* FIXME
        if(CONFIG_PROFILE[CURRENT_PROFILE]<5) { // Если текущий профиль не последний
          CONFIG_PROFILE[CURRENT_PROFILE] = CONFIG_PROFILE[CURRENT_PROFILE] + 1; // Меняем профиль на следующий
        } else  { // Если текущий профиль последний
          CONFIG_PROFILE[CURRENT_PROFILE] = 1; // Меняем профиль на первый
        }
        */
        ButtonBack.write(LOW);
      }
      // Нажата клавиша "Вверх"
      if(ButtonUpPushed==true) {
        if(CONFIG_SYSTEM[ZOOM]<9) { // Масштаб меньше 9
          CONFIG_SYSTEM[ZOOM] = CONFIG_SYSTEM[ZOOM] + 1; // Увеличиваем масштаб
          display.clearDisplay();
        }
        ButtonUp.write(LOW);
      }
      // Нажата клавиша "Вниз"
      if(ButtonDownPushed==true) {
        if(CONFIG_SYSTEM[ZOOM]>1) { // Масштаб больше 1
          CONFIG_SYSTEM[ZOOM] = CONFIG_SYSTEM[ZOOM] - 1; // Уменьшаем масштаб
          display.clearDisplay();
        }
        ButtonDown.write(LOW);
      }
      /*** Автоматический масштаб ***/
      // TODO
      /*** Рисуем значки ***/
      // Профиль
      display.fillRect(0, 0, 13, 9, BLACK);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(1, 1);
      display.print("P");
      display.print(CONFIG_PROFILE[CURRENT_PROFILE]);
      // Масштаб
      display.fillRect(70, 0, 13, 9, BLACK);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(71, 1);
      display.print(CONFIG_SYSTEM[ZOOM]);
      display.print("x");
      
      /*** Рисуем годограф ***/
      // Рисуем разметку годографа
      display.drawFastVLine(41, 15, 32, BLACK); // Вертикальная
      display.drawFastHLine(0, 47, 83, BLACK);  // Горизонтальная
      // Определяем координаты конца отрезка
      uint8_t dxB = map(ChannelsData[cdPoint][0], -511/CONFIG_SYSTEM[ZOOM], 512/CONFIG_SYSTEM[ZOOM], 0,  82);
      uint8_t dyB = map(ChannelsData[cdPoint][1], 0,                        512/CONFIG_SYSTEM[ZOOM], 47, 15);
      uint8_t cdOldPoint;
      // Отрисовка годограммы одним из выбранных в настройках методом
      if(CONFIG_SYSTEM[VECTOR_MODE]==true) { // Векторное отображение
        if(cdPoint>0) { cdOldPoint = cdPoint - 1; } 
        else { cdOldPoint = CONFIG_SYSTEM[BUFFER_SIZE]-1; }
        uint8_t dxA = map(ChannelsData[cdOldPoint][0], -511/CONFIG_SYSTEM[ZOOM], 512/CONFIG_SYSTEM[ZOOM], 0,  82);
        uint8_t dyA = map(ChannelsData[cdOldPoint][1], 0,                        512/CONFIG_SYSTEM[ZOOM], 47, 15);
        display.drawLine(dxA, dyA, dxB, dyB, BLACK);
        // Стираем устаревший вектор
        if(cdPoint>=CONFIG_SYSTEM[BUFFER_SIZE]-1) { cdOldPoint = 0; } 
        else { cdOldPoint = cdPoint + 1; }
        dxA = map(ChannelsData[cdOldPoint][0], -511/CONFIG_SYSTEM[ZOOM], 512/CONFIG_SYSTEM[ZOOM], 0,  82);
        dyA = map(ChannelsData[cdOldPoint][1], 0,                        512/CONFIG_SYSTEM[ZOOM], 47, 15);
        if(cdOldPoint>=CONFIG_SYSTEM[BUFFER_SIZE]-1) { cdOldPoint = 0; } 
        else { cdOldPoint = cdOldPoint + 1; }
        dxB = map(ChannelsData[cdOldPoint][0], -511/CONFIG_SYSTEM[ZOOM], 512/CONFIG_SYSTEM[ZOOM], 0,  82);
        dyB = map(ChannelsData[cdOldPoint][1], 0,                        512/CONFIG_SYSTEM[ZOOM], 47, 15);
        display.drawLine(dxA, dyA, dxB, dyB, WHITE);
      } else { // Скалярное отображение
        // Рисуем новую точку
        display.drawPixel(dxB, dyB, BLACK);
        // Стираем устаревшую точку
        if(cdPoint>=CONFIG_SYSTEM[BUFFER_SIZE]-1) { cdOldPoint = 0; } 
        else { cdOldPoint = cdPoint + 1; }
        uint8_t dxA = map(ChannelsData[cdOldPoint][0], -511/CONFIG_SYSTEM[ZOOM], 512/CONFIG_SYSTEM[ZOOM], 0,  82);
        uint8_t dyA = map(ChannelsData[cdOldPoint][1], 0,                        512/CONFIG_SYSTEM[ZOOM], 47, 15);
        display.drawPixel(dxA, dyA, WHITE);
      }
      
      /*** Вычисляем VDI ***/
      unsigned int vector_mean = 0; // Среднее значение длины вектора
      for(uint8_t i=0;i<CONFIG_SYSTEM[BUFFER_SIZE];i++) {
        //vector_mean = vector_mean + ChannelsData[i][1];
        vector_mean = vector_mean + sqrt(pow(ChannelsData[i][0], 2) + pow(ChannelsData[i][1], 2));
      }
      vector_mean = vector_mean/CONFIG_SYSTEM[BUFFER_SIZE]; // Среднее значение
      // Рассчитываем VDI, если это необходимо
      if(vector_mean>5 && cdPoint>0) { // Вычисляем VDI только при наличии сигнала (длина вектора больше 0)
        if(vector>vector_max) { // Если текущий вектор длинее предыдущего
          vector_max = vector; // Запоминаем длину как максимальную
          vdi = round(atan2(ChannelsData[cdPoint][0], ChannelsData[cdPoint][1])*(180/PI)); // Рассчитываем VDI
        }
      } else { // Если сигнал отсутствует (дина вектора нулевая)
        vdi = 0; // Сбрасываем значение VDI на ноль
        vector_max = 0; // И забываем значение максимальной длины вектора
      }
      // Отображаем число VDI на экране
      display.fillRect(22, 0, 34, 14, WHITE);
      display.setTextSize(2);
      display.setTextColor(BLACK);
      if(vdi<=-10) { // Отрицательные десятки
        display.setCursor(22, 0);
      } else if(vdi<0 && vdi>-10) { // Отрицательные единицы
        display.setCursor(30, 0);
      } else if(vdi>0 && vdi<10) { // Положительные единицы
        display.setCursor(30, 0);
        display.print("+");
      } else if(vdi>=10) { // Положительные десятки
        display.setCursor(22, 0);
        display.print("+");
      } else { // Ноль
        display.setCursor(37, 0);
      }
      display.print(vdi);
      /*** Звуковое сопровождение сигналов ***/
      // TODO: отключение звука при отсутствии сигнала
      if(CONFIG_SOUND[NUM_TONES]>0) {
        uint8_t vdiMask = getMaskByVDI(vdi); // Определяем сегмент дискриминации по числу VDI
        // Получаем состояние этого сегмента на основе текущей маски дискриминации
        boolean enableVDIsound = true;
        if(vdiMask>0) {
          switch(CONFIG_PROFILE[CURRENT_PROFILE]) {
            // FIXME
            //case 2:   enableVDIsound = CONFIG_PROFILE_2[vdiMask-1]; break;
            //case 3:   enableVDIsound = CONFIG_PROFILE_3[vdiMask-1]; break;
            //case 4:   enableVDIsound = CONFIG_PROFILE_4[vdiMask-1]; break;
            case 5:   enableVDIsound = CONFIG_PROFILE_5[vdiMask-1]; break;
            //default:  enableVDIsound = CONFIG_PROFILE_1[vdiMask-1];
          }
        } else { enableVDIsound = false; }
        // На основе полученных данных воспроизводим звук
        if(enableVDIsound) { // Если текущий сегмент активен
          // Определение частоты проигрываемого сигнала
          int playingVDItone = 0;
          switch(CONFIG_SOUND[NUM_TONES]) {
            // FIXME
            //case 2:  playingVDItone = CONFIG_SOUND_TONES_2[vdiMask-1];  break;
            case 3:  playingVDItone = CONFIG_SOUND_TONES_3[vdiMask-1];  break;
            //case 4:  playingVDItone = CONFIG_SOUND_TONES_4[vdiMask-1];  break;
            //case 5:  playingVDItone = CONFIG_SOUND_TONES_5[vdiMask-1];  break;
            //case 6:  playingVDItone = CONFIG_SOUND_TONES_6[vdiMask-1];  break;
            //case 7:  playingVDItone = CONFIG_SOUND_TONES_7[vdiMask-1];  break;
            //case 8:  playingVDItone = CONFIG_SOUND_TONES_8[vdiMask-1];  break;
            //case 9:  playingVDItone = CONFIG_SOUND_TONES_9[vdiMask-1];  break;
            //case 10: playingVDItone = CONFIG_SOUND_TONES_10[vdiMask-1]; break;
            //case 11: playingVDItone = CONFIG_SOUND_TONES_11[vdiMask-1]; break;
            //case 12: playingVDItone = CONFIG_SOUND_TONES_12[vdiMask-1]; break;
            //default: playingVDItone = CONFIG_SOUND_TONES_13[vdiMask-1];
          }
          tone(PIN_SOUND, playingVDItone); // Проигрываем сигнал
        } else { // Если текущий сегмент отключен
          noTone(PIN_SOUND); // Прерываем сигнал
        }
      }
    }
  } else { // Если массив данных еще не заполнен
    // Показываем полосу загрузки
    display.fillRect(2, 45, ((cdPoint+1)*79/CONFIG_SYSTEM[BUFFER_SIZE])+1, 2, BLACK);
  }
  // Управление указетелем ячейки массива
  if(cdPoint<CONFIG_SYSTEM[BUFFER_SIZE]-1) {
    cdPoint = cdPoint + 1;
  } else {
    cdPoint = 0;
    if(ChannelsDataFull==false) {
      // Небольшая задержка заставки
      display.display();
      delay(500);
      // Звук приветствия
      // TODO
      // Убираем заставку
      display.clearDisplay();
      ChannelsDataFull = true;
    }
  }
  // Обновляем экран
  display.display();
}

/*** Работа с профилями ***/
// Определяет сегмент дискриминации по числу VDI
uint8_t getMaskByVDI(int8_t vdi) {
  for(uint8_t i=0;i<12;i++) {
   if(vdi >= CONFIG_VDI_MASK[i][0] && vdi <= CONFIG_VDI_MASK[i][1]) {
     return i+1;
   }
  }
  return 0;
}

/*** Работа с настройками ***/
// Загрузка настроек
void loadSettings() {
  /*
  int currentAddress = 0;
  // Проверяем версию настроек, сохраненных в EEPROM, и если они актуальные
  if(EEPROM.read(currentAddress++)==(sizeof(CONFIG_SYSTEM)+sizeof(CONFIG_DISPLAY)+sizeof(CONFIG_SOUND)+sizeof(CONFIG_PROFILE))/2+sizeof(CONFIG_PROFILE_5)) { // То считываем их
    // Настройки системы
    for(int i=0;i<sizeof(CONFIG_SYSTEM)/2;i++) {
      CONFIG_SYSTEM[i] = EEPROM_ReadInt(currentAddress);
      currentAddress += 2;
    }
    // Настройки дисплея
    for(int i=0;i<sizeof(CONFIG_DISPLAY)/2;i++) {
      CONFIG_DISPLAY[i] = EEPROM_ReadInt(currentAddress);
      currentAddress += 2;
    }
    // Настройки звука
    for(int i=0;i<sizeof(CONFIG_SOUND)/2;i++) {
      CONFIG_SOUND[i] = EEPROM_ReadInt(currentAddress);
      currentAddress += 2;
    }
    // Настройки профиля
    for(int i=0;i<sizeof(CONFIG_PROFILE)/2;i++) {
      CONFIG_PROFILE[i] = EEPROM_ReadInt(currentAddress);
      currentAddress += 2;
    }
    // Пользовательский профиль
    for(int i=0;i<sizeof(CONFIG_PROFILE_5);i++) {
      CONFIG_PROFILE_5[i] = EEPROM.read(currentAddress);
      currentAddress++;
    }
  } else { // Если настройки в EEPROM неатуальные
    // Форматируем память EEPROM
    for(int i=0;i<1024;i++) EEPROM.write(i, 255);
    // И записываем туда стандартные настройки
    saveSettings();
  }
  */
}

// Сохранение настроек
void saveSettings() {
  /*
  int currentAddress = 0;
  // Сохраняем в ячейку 0 данные о версии настроек
  EEPROM.write(currentAddress++, (sizeof(CONFIG_SYSTEM)+sizeof(CONFIG_DISPLAY)+sizeof(CONFIG_SOUND)+sizeof(CONFIG_PROFILE))/2+sizeof(CONFIG_PROFILE_5));
  // Сохраняем в EEPROM остальные настройки
  // Настройки системы
  for(int i=0;i<sizeof(CONFIG_SYSTEM)/2;i++) {
    EEPROM_WriteInt(currentAddress, CONFIG_SYSTEM[i]);
    currentAddress += 2;
  }
  // Настройки дисплея
  for(int i=0;i<sizeof(CONFIG_DISPLAY)/2;i++) {
    EEPROM_WriteInt(currentAddress, CONFIG_DISPLAY[i]);
    currentAddress += 2;
  }
  // Настройки звука
  for(int i=0;i<sizeof(CONFIG_SOUND)/2;i++) {
    EEPROM_WriteInt(currentAddress, CONFIG_SOUND[i]);
    currentAddress += 2;
  }
  // Настройки профиля
  for(int i=0;i<sizeof(CONFIG_PROFILE)/2;i++) {
    EEPROM_WriteInt(currentAddress, CONFIG_PROFILE[i]);
    currentAddress += 2;
  }
  // Пользовательский профиль
  for(int i=0;i<sizeof(CONFIG_PROFILE_5);i++) {
    EEPROM_WriteInt(currentAddress, CONFIG_PROFILE_5[i]);
    currentAddress++;
  }
  */
}

/*** Работа с EEPROM ***/
//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
/*
void EEPROM_WriteInt(int p_address, int p_value) {
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);
  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}
*/

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
/*
unsigned int EEPROM_ReadInt(int p_address) {
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}
*/

