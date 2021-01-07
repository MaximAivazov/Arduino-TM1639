#include <masterTM1639.h>
static byte dataPin = 8;  //�������� ������
static byte clockPin = 9; //������������� �������� ������
static byte strobePin = 10; //Chip select,����� ����������, ����������/������ ������ ������

void initTM1639(byte dataPin1, byte clockPin1, byte strobePin1) {
	dataPin = dataPin1;  //�������� ������
	clockPin = clockPin1; //������������� �������� ������
	strobePin = strobePin1; //Chip select,����� ����������, ����������/������ ������ ������

	pinMode(dataPin, OUTPUT); //�������� ���� ��� ������
	pinMode(clockPin, OUTPUT);
	pinMode(strobePin, OUTPUT);

	digitalWrite(strobePin, HIGH);  //�������� ������ � TM1639
	digitalWrite(clockPin, HIGH); //�������� ��� � 1
	delay(1); //����� �� ������ TM1639 ����� ������ �������

	//////////////////////////////////////// �������������
	sendCommand(0x40);  //���������� B01000000 - ������ ������ ��� �����������
	sendCommand(B10000111); //������� ������� 2/16, ��� ���������
	////////////////////////////////////////

	//////////////////////////////////////// ������� ������ ����������
	digitalWrite(strobePin, LOW);
	send(0x00); //������� �� ����� (������ ����� ����� strobePin � 0 - ������������� ��� �������)
	for (int i = 0; i < 16; i++) {  //���������� 16 ���� ��� �� �������� ��������� ���� 8 ���������
		send(0x00); //���������� 16 ����, �.�. 1 ������� ����������� 2 �����������
	}
	digitalWrite(strobePin, HIGH);
}

void sendCommand(byte cmd)  //������� �������� �������
{
	digitalWrite(strobePin, LOW); //�������� ������ ������ ����������
	send(cmd);  //������ ��� �������� � ����������
	digitalWrite(strobePin, HIGH);  //�������� ������
}

void send(byte data)  //������� �������� ������
{
	for (int i = 0; i < 8; i++) { //��������� ���� ������������� �����
		digitalWrite(clockPin, LOW); //��������� �����. ����� � 0
		digitalWrite(dataPin, data & 1 ? HIGH : LOW); //� ����������� �� �������� ���� �������� 0/1 �� ������ Data
		data >>= 1; //������� ��������� ���
		digitalWrite(clockPin, HIGH); //TM1639 ��������� ������ �� ������ ������� �����. ������
		delay(1);
	}
}

void sendData(byte address, byte data)  //��������� ������ �� ������
{
	digitalWrite(strobePin, LOW);
	send(0x00 | address); //��������� "���", �� ���� ������ ����� ��������
	send(data); //�������� ������ ��� ����������� ������
	digitalWrite(strobePin, HIGH);
}

void printTM1639(byte a1, byte b1, byte a2, byte b2, byte a3, byte b3, byte a4, byte b4, byte a5, byte b5, byte a6, byte b6, byte a7, byte b7, byte a8, byte b8) {
	/* �1, a2 � �.�. ����� ��� ������, ������� � ������� �������, ���� ����� ������ 9, �� ��������� �������� ������, b1, b2 � �.�. ����� �����, ������� � ������� ����������,
	���� ������ 0 - ����� �� �����, ���� ����� ������ �����, �� �����*/
	byte info[] = { B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 };	//��������������� ������ ��� ������ ������ �� �������
	byte data[] = { B11111100, B01100000, B11011010, B11110010, B01100110, B10110110, B10111110, B11100000, B11111110, B11110110, B00000000 };	//��������� ��������� ��� ��������� ���� �� 0 �� 9

	for (int j = 0; j < 8; j++) {	//�������� ��������������� ������ ��� ������ ������
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

	//����� �� �����
	for (int i = 0; i < 8; i++) {
		int pos = B11000000;  //������� �������� ������ ����������� ������ ��� �����������
		int data; //��������������� ���������� 1
		int data2;  //��������������� ���������� 2
		for (int a = 0; a < 4; a++) { //��������� ������ 4 �������� �����
			bitWrite(data, a, bitRead(info[a], i)); //� ������� �� � ������ ��������������� ����������
		}
		for (int a = 4; a < 8; a++) {
			bitWrite(data2, a - 4, bitRead(info[a], i));  //� ������ �� ������ ����������
		}
		sendData((pos + i * 2), data);  //�������� ������ �������� ������ �� ������� ������ ��������
		sendData((pos + i * 2 + 1), data2); //�������� ������ ������� �� ������� ������ ��������
	}
	sendCommand(0x40);  //���������� B01000000 - ������ ������ ��� �����������
	sendCommand(B10001010); //������� "�������� �������"*/
}

void lightTM1639(byte light1) {
	byte light[] = { B10001000, B10001001, B10001010, B10001011, B10001100, B10001101, B10001110, B10001111 };	//�������� ���������
	if (light1 == 0) light1 = 1;
	if (light1 > 8) light1 = 8;
	sendCommand(light[light1 - 1]); //������� �������
}

void controlTM1639(bool a) {
	if (a == 1) sendCommand(B10001011); //������� "�������� �������" ������� "3"
	if (a == 0) sendCommand(B10000000); //������� "��������� �������"
}

byte buttonstatusTM1639(bool b) {
	digitalWrite(strobePin, LOW);
	send(0x42);  //������� �� ������ ��������� ������
	pinMode(dataPin, INPUT);

	byte buttonstatus1 = B00000000;
	byte buttonstatus2 = B00000000;

	for (int i = 0; i < 16; i++) {  //���������� 2 �����
		digitalWrite(clockPin, LOW);
		if (i < 8) bitWrite(buttonstatus1, i, digitalRead(dataPin));	//��������� ������ ����
		else bitWrite(buttonstatus2, i - 8, digitalRead(dataPin));	//��������� ������ ����
		digitalWrite(clockPin, HIGH);
	}

	pinMode(dataPin, OUTPUT);
	digitalWrite(strobePin, HIGH);

	if (b == 0) return buttonstatus1;
	if (b == 1) return buttonstatus2;
}