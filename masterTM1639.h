#pragma once
#include <Arduino.h>

	void initTM1639(byte dataPin1, byte clockPin1, byte strobePin1);	//������������� �������� TM1639
	void sendCommand(byte cmd);
	void send(byte data);
	void sendData(byte address, byte data);
	void printTM1639(byte a1, byte b1, byte a2, byte b2, byte a3, byte b3, byte a4, byte b4, byte a5, byte b5, byte a6, byte b6, byte a7, byte b7, byte a8, byte b8);	//����� ������ �� �������
	void lightTM1639(byte light1);	//������������� ������ ���������
	void controlTM1639(bool a);	//��������� � ���������� �������
	byte buttonstatusTM1639(bool b);	//������ ��������� ����������
