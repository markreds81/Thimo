/**
 * 
 * THIMO IoT remote programmable termostat
 * 
 * Filename: Thimo.ino
 * Created on: 11 Nov 2020
 * Description: Arduino IDE sketch file
 * 
 * Copyright (C) 2020-2021 Marco Rossi (aka Mark Reds) <marco@markreds.it>
 * 
 * THIMO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * THIMO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with THIMO. If not, see <http://www.gnu.org/licenses/>.
 * 
 * ARDUINO IDE SETTINGS 
 * 
 * Board: ESP32 Arduino > ESP32 Dev Module
 * Upload speed: 115200
 * CPU Frequency: 240 MHz
 * Flash Frequency: 80 MHz
 * Flash Mode: QIO
 * Flash Size: 4 MB (32 Mb)
 * Porta: /dev/cu.usb.serial-0001 
 * 
 */

#include <Wire.h>
#include "Thimo.h"

/**
 * main initializatione routine
 */
void setup() {
	/*/ Serial initialization (for debug porpouse only) */
	Serial.begin(9600);

	/* OneWire library initialization (used by RTC and LCD modules) */
	Wire.begin();

	/* RTC module initialization */
	RTC.begin();

	/* LCD module initialization */
	LCD.begin(16, 2);

	/* DHT module initialization */
	DHT.begin();
	
	/* Buttons initialization */
	ButtonS.begin();
	ButtonN.begin();
	ButtonP.begin();

	/* Thimo module initialization */
	Thimo.begin();
}

/**
 * main loop function
 */
void loop() {
	Thimo.loop();
}
