/**
 * 
 * THIMO IoT remote programmable termostat
 * 
 * Filename: Thimo.h
 * Created on: 20 Sep 2021
 * Description: Thimo main module
 * 
 * Copyright (C) 2020-2021 Marco Rossi (aka Mark Reds) <marco@markreds.it> 
 * 
 */

#ifndef _THIMO_H_
#define _THIMO_H_

#include "LCD.h"
#include "RTC.h"
#include "DHT.h"
#include "Button.h"

enum {
	ENVIRONMENT,
	MANUAL,
	CLOCK,
	TIMETABLE1,
	TIMETABLE2,
	TIMETABLE3,
	TIMETABLE4,
	TIMETABLE5
}	View;

class ThimoClass {
public:
	ThimoClass();
	virtual ~ThimoClass();

	void begin();
	void loop();
	void refresh();
	void menuNext();
	void menuPrevious();
	void menuSelect();
private:
	uint8_t m_view = CLOCK;
	uint8_t m_timetable[24];
	bool m_manualMode = false;
	unsigned long m_sensorTimer = 0UL;
	unsigned long m_refreshTimer = 0UL;
	unsigned long m_backlightTimer = 0UL;
	float m_humidity = 0.0f;
	float m_temperature = 0.0f;

	void displayEnvironment();
	void displayManual();
	void displayClock();
	void displayTimetable(int hfrom, int hto);
	void editManual();
	void editClock();
	void editTimetable(int hfrom, int hto);
	void toggleRelay();
	float potTemperature();
};

extern ThimoClass Thimo;

#endif
