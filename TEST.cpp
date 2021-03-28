/*
  TEST.h - Library for Radio transceiver function.
  Created by Kevin Z. Yu, March 23, 2021.
  Released into the public domain.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include "TEST.h"



// initialize actionList
void TEST::initialize_actionList(byte id, byte* action) {
	for (int i = 0; i < actionSize; i++) {
		action[i] = actionsTrial[trialNum][id][i];
	}
}

// initialize actionList
void TEST::initialize_scheduleList(byte id, byte* schedule) {
	for (int i = 0; i < actionSize; i++) {
		schedule[i] = schedulesTrial[trialNum][id][i];;
	}
}

void TEST::report(void) {
	int a = 0;
	byte value;

	value = EEPROM.read(a);

	Serial.print(a);
	Serial.print("\t");
	Serial.print(value);
	Serial.println();

	a = a + 1;

	if (a == eepromSize)
		a = 0;

	delay(500);
}


