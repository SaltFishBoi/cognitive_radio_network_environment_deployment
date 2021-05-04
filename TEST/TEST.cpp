/*
  TEST.h - Library for Radio transceiver function.
  Created by Kevin Z. Yu, March 23, 2021.
  Released into the public domain.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include "TEST.h"



// initialize actionList
//void TEST::initialize_actionList(byte id, byte* action) {
//	for (int i = 0; i < actionSize; i++) {
//		action[i] = actionsTrial[numTrial][id][i];
//	}
//}

// initialize scheduleList
void TEST::initialize_scheduleList(byte id, byte* schedule) {
	for (int i = 0; i < scheduleSize; i++) {
		schedule[i] = schedulesTrial[numTrial][id][i];;
	}
}

void TEST::report(void) {
	int a;
	byte value;

	for (a = 0; a < eepromSize; a++)
	{
		value = EEPROM.read(a);

		Serial.print(a);
		Serial.print("\t");
		Serial.print(value);
		Serial.println();

		delay(10);
	}
}

void TEST::record(int location, byte time, byte data) {
	EEPROM.write(location, time);
	EEPROM.write(location+1, data);
}


