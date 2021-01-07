#include <masterTM1639.h>
static byte dataPin = 8;  //Передача данных
static byte clockPin = 9; //Синхронизация передачи данных
static byte strobePin = 10; //Chip select,выбор микросхемы, разрешение/запрет записи данных

void initTM1639(byte dataPin1, byte clockPin1, byte strobePin1) {
	dataPin = dataPin1;  //Передача данных
	clockPin = clockPin1; //Синхронизация передачи данных
	strobePin = strobePin1; //Chip select,выбор микросхемы, разрешение/запрет записи данных

	pinMode(dataPin, OUTPUT); //Настроим пины как выходы
	pinMode(clockPin, OUTPUT);
	pinMode(strobePin, OUTPUT);

	digitalWrite(strobePin, HIGH);  //Запретим запись в TM1639
	digitalWrite(clockPin, HIGH); //Тактовый пин в 1
	delay(1); //Время на запуск TM1639 после подачи питания

	//////////////////////////////////////// инициализация
	sendCommand(0x40);  //Аналогично B01000000 - Запись данных для отображения
	sendCommand(B10000111); //Яркость дисплея 2/16, без включения
	////////////////////////////////////////

	//////////////////////////////////////// очистка буфера микросхемы
	digitalWrite(strobePin, LOW);
	send(0x00); //Команда из нулей (первое число после strobePin в 0 - воспринмается как команда)
	for (int i = 0; i < 16; i++) {  //Перебираем 16 байт что бы обнулить состояние всех 8 сегментов
		send(0x00); //Отправляем 16 байт, т.к. 1 сегмент управляется 2 полубайтами
	}
	digitalWrite(strobePin, HIGH);
}

void sendCommand(byte cmd)  //Функция передачи команды
{
	digitalWrite(strobePin, LOW); //Разрешим запись выбрав микросхему
	send(cmd);  //Данные для передачи в микросхему
	digitalWrite(strobePin, HIGH);  //Запретим запись
}

void send(byte data)  //Функция передачи данных
{
	for (int i = 0; i < 8; i++) { //Переберем биты отправляемого байта
		digitalWrite(clockPin, LOW); //Установим синхр. вывод в 0
		digitalWrite(dataPin, data & 1 ? HIGH : LOW); //В зависимости от значения бита выставим 0/1 на выводе Data
		data >>= 1; //Выберем следующий бит
		digitalWrite(clockPin, HIGH); //TM1639 считывает данные по фронту сигнала синхр. вывода
		delay(1);
	}
}

void sendData(byte address, byte data)  //Передадим данные по адресу
{
	digitalWrite(strobePin, LOW);
	send(0x00 | address); //Побитовое "или", по сути задаем адрес сегмента
	send(data); //Передаем данные для конкретного адреса
	digitalWrite(strobePin, HIGH);
}

void printTM1639(byte a1, byte b1, byte a2, byte b2, byte a3, byte b3, byte a4, byte b4, byte a5, byte b5, byte a6, byte b6, byte a7, byte b7, byte a8, byte b8) {
	/* а1, a2 и т.п. числа для вывода, начиная с первого разряда, если число больше 9, то индикатор остается пустым, b1, b2 и т.п. вывод точки, начиная с первого индикатора,
	если вводим 0 - точка не горит, если любое другое число, то горит*/
	byte info[] = { B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 };	//Вспомогательный массив для вывода данных на дисплеи
	byte data[] = { B11111100, B01100000, B11011010, B11110010, B01100110, B10110110, B10111110, B11100000, B11111110, B11110110, B00000000 };	//Состояние сегментов для индикации цифр от 0 до 9

	for (int j = 0; j < 8; j++) {	//Обнуляем вспомогательный массив для вывода данных
		info[j] = B00000000;
	}

	if (a1 > 10) a1 = 10;
	if (a2 > 10) a2 = 10;
	if (a3 > 10) a3 = 10;
	if (a4 > 10) a4 = 10;
	if (a5 > 10) a5 = 10;
	if (a6 > 10) a6 = 10;
	if (a7 > 10) a7 = 10;
	if (a8 > 10) a8 = 10;

	for (int i = 0; i < 8; i++) {
		bitWrite(info[i], 0, bitRead(data[a1], 7 - i));
		bitWrite(info[i], 1, bitRead(data[a2], 7 - i));
		bitWrite(info[i], 2, bitRead(data[a3], 7 - i));
		bitWrite(info[i], 3, bitRead(data[a4], 7 - i));
		bitWrite(info[i], 4, bitRead(data[a5], 7 - i));
		bitWrite(info[i], 5, bitRead(data[a6], 7 - i));
		bitWrite(info[i], 6, bitRead(data[a7], 7 - i));
		bitWrite(info[i], 7, bitRead(data[a8], 7 - i));
	}

	if (b1 != 0) bitWrite(info[7], 0, 1);
	if (b2 != 0) bitWrite(info[7], 1, 1);
	if (b3 != 0) bitWrite(info[7], 2, 1);
	if (b4 != 0) bitWrite(info[7], 3, 1);
	if (b5 != 0) bitWrite(info[7], 4, 1);
	if (b6 != 0) bitWrite(info[7], 5, 1);
	if (b7 != 0) bitWrite(info[7], 6, 1);
	if (b8 != 0) bitWrite(info[7], 7, 1);

	//Вывод на экран
	for (int i = 0; i < 8; i++) {
		int pos = B11000000;  //Команда указания адреса загружаемых данных для отображения
		int data; //Вспомогательная переменная 1
		int data2;  //Вспомогательная переменная 2
		for (int a = 0; a < 4; a++) { //Переберем первые 4 значащих байта
			bitWrite(data, a, bitRead(info[a], i)); //и запишем их в первую вспомогательную переменную
		}
		for (int a = 4; a < 8; a++) {
			bitWrite(data2, a - 4, bitRead(info[a], i));  //а вторые во вторую переменную
		}
		sendData((pos + i * 2), data);  //отправим первый полубайт данных по первому адресу сегмента
		sendData((pos + i * 2 + 1), data2); //отправим второй полубай по второму адресу сегмента
	}
	sendCommand(0x40);  //Аналогично B01000000 - Запись данных для отображения
	sendCommand(B10001010); //Команда "Включить дисплей"*/
}

void lightTM1639(byte light1) {
	byte light[] = { B10001000, B10001001, B10001010, B10001011, B10001100, B10001101, B10001110, B10001111 };	//Градации подсветки
	if (light1 == 0) light1 = 1;
	if (light1 > 8) light1 = 8;
	sendCommand(light[light1 - 1]); //Яркость дисплея
}

void controlTM1639(bool a) {
	if (a == 1) sendCommand(B10001011); //Команда "Включить дисплей" яркость "3"
	if (a == 0) sendCommand(B10000000); //Команда "Выключить дисплей"
}

byte buttonstatusTM1639(bool b) {
	digitalWrite(strobePin, LOW);
	send(0x42);  //Команда на запрос состояния кнопок
	pinMode(dataPin, INPUT);

	byte buttonstatus1 = B00000000;
	byte buttonstatus2 = B00000000;

	for (int i = 0; i < 16; i++) {  //Перебираем 2 байта
		digitalWrite(clockPin, LOW);
		if (i < 8) bitWrite(buttonstatus1, i, digitalRead(dataPin));	//Считываем первый байт
		else bitWrite(buttonstatus2, i - 8, digitalRead(dataPin));	//Считываем второй байт
		digitalWrite(clockPin, HIGH);
	}

	pinMode(dataPin, OUTPUT);
	digitalWrite(strobePin, HIGH);

	if (b == 0) return buttonstatus1;
	if (b == 1) return buttonstatus2;
}