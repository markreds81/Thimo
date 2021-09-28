/**
 * 
 * THIMO IoT remote programmable termostat
 * 
 * Filename: Thimo.cpp
 * Created on: 20 Sep 2021
 * Description: Thimo main module
 * 
 * Copyright (C) 2020-2021 Marco Rossi (aka Mark Reds) <marco@markreds.it> 
 * 
 */

#include "Thimo.h"

ThimoClass::ThimoClass() {
	
}

ThimoClass::~ThimoClass() {
	
}

void ThimoClass::begin() {
	if (!RTC.isrunning()) {
		Serial.println("RTC is not running");
		RTC.adjust(DateTime(__DATE__, __TIME__));
	}
	
	for (int i = 0; i < 24; i++) {
		m_timetable[i] = RTC.readnvram(i);
		if (m_timetable[i] > 30) {
			m_timetable[i] = 0;
		}
	}

	pinMode(RELAY_PIN, OUTPUT);
	
	m_backlightTimer = millis();
}

void ThimoClass::loop() {
	Environment env;
	
	/* DHT sensor read */
	if (DHT.readSensor(&env) == DHTModule::ERROR_NONE) {
		if (!isnan(env.temperature)) {
			m_temperature = env.temperature;
		}
		if (!isnan(env.humidity)) {
			m_humidity = env.humidity;
		}
		toggleRelay();
	}
	
	/* LCD menu control selection */
	if (ButtonN.toggled() && ButtonN.read() == Button::PRESSED) {
		Serial.println("next");
		Thimo.menuNext();
	}

	if (ButtonP.toggled() && ButtonP.read() == Button::PRESSED) {
		Serial.println("previous");
		Thimo.menuPrevious();
	}

	if (ButtonS.toggled() && ButtonS.read() == Button::PRESSED) {
		Serial.println("select");
		Thimo.menuSelect();
	}
	
	/* LCD backlight control state */
	if ((millis() - m_backlightTimer) > LCD_BACKLIGHT_DURATION) {
		LCD.noBacklight();
	} else {
		LCD.backlight();
	}
	
	/* refresh display data every 1s */
	if ((millis() - m_refreshTimer) > LCD_REFRESH_TIME) {
		refresh();
	}
}

void ThimoClass::menuNext() {
	if ((millis() - m_backlightTimer) < 10000UL) {
		LCD.clear();
		if (++m_view > TIMETABLE5) {
			m_view = 0;
		}
		refresh();
	}
	m_backlightTimer = millis();
}

void ThimoClass::menuPrevious() {
	if ((millis()  - m_backlightTimer) < 10000UL) {
		LCD.clear();
		if (--m_view > TIMETABLE5) {
			m_view = TIMETABLE5;
		}
		refresh();
	}
	m_backlightTimer = millis();
}

void ThimoClass::menuSelect() {
	if ((millis()  - m_backlightTimer) < 10000UL) {
		switch (m_view) {
			case MANUAL:
				editManual();
				break;
			case CLOCK:
				editClock();
				break;
			case TIMETABLE1:
				editTimetable(0, 4);
				break;
			case TIMETABLE2:
				editTimetable(5, 9);
				break;
			case TIMETABLE3:
				editTimetable(10, 14);
				break;
			case TIMETABLE4:
				editTimetable(15, 19);
				break;
			case TIMETABLE5:
				editTimetable(20, 23);
				break;
		}
	}
	m_backlightTimer = millis();
}

void ThimoClass::refresh() {
	LCD.home();

	switch (m_view) {
		case ENVIRONMENT:
			displayEnvironment();
			break;
		case MANUAL:
			displayManual();
			break;
		case CLOCK:
			displayClock();
			break;
		case TIMETABLE1:
			displayTimetable(0, 4);
			break;
		case TIMETABLE2:
			displayTimetable(5, 9);
			break;
		case TIMETABLE3:
			displayTimetable(10, 14);
			break;
		case TIMETABLE4:
			displayTimetable(15, 19);
			break;
		case TIMETABLE5:
			displayTimetable(20, 23);
			break;
		default:
			LCD.print("View: ");
			LCD.print(m_view);
	}

	m_refreshTimer = millis();
}

void ThimoClass::displayEnvironment() {
	LCD.print("Temper. : ");
	LCD.setCursor(10,0);
	LCD.print(m_temperature, 1);
	LCD.print(char(223));
	LCD.print("C");		
	LCD.setCursor(0,1);
	LCD.print("Humidity: ");
	LCD.setCursor(10,1);
	LCD.print(m_humidity, 1);
	LCD.print(" %");
}

void ThimoClass::displayManual() {
	float pt = potTemperature();
	
	LCD.print("Mode:");
	if (m_manualMode) {
		LCD.setCursor(10, 0);
		LCD.print("MANUAL");
	} else {
		LCD.setCursor(12, 0);
		LCD.print("AUTO");
	}
	LCD.setCursor(0, 1);
	LCD.print("Temp:");
	LCD.setCursor(10, 1);
	if (pt < 10.0f) {
		LCD.print(" ");
	}
	LCD.print(pt, 1);
	LCD.print(char(223));
	LCD.print("C");
}

void ThimoClass::displayClock() {
	DateTime now = RTC.now();
	
	// print current date
	LCD.setCursor(3,0);
	if (now.day() < 10) {
		LCD.print("0");
	}
	LCD.print(now.day());
	LCD.print("/");
	if (now.month() < 10) {
		LCD.print("0");
	}
	LCD.print(now.month());
	LCD.print("/");
	LCD.print(now.year());
	
	// print current time
	LCD.setCursor(4,1);
	if (now.hour() < 10) {
		LCD.print("0");
	}
	LCD.print(now.hour());
	LCD.print(":");
	if (now.minute() < 10) {
		LCD.print("0");
	}
	LCD.print(now.minute());
	LCD.print(":");
	if (now.second() < 10) {
		LCD.print("0");
	}
	LCD.print(now.second());
}

void ThimoClass::displayTimetable(int hfrom, int hto) {
	int col = 2;
	
	LCD.setCursor(0, 0);
	LCD.print("H ");
	LCD.setCursor(0, 1);
	LCD.print("T ");
	
	for (int h = hfrom; h <= hto; h++) {
		LCD.setCursor(col, 0);
		if (h < 10) {
			LCD.print("0");
		}
		LCD.print(h);
		LCD.setCursor((m_timetable[h] < 9) ? col + 1 : col, 1);
		LCD.print(m_timetable[h]);
		col += 3;
	}
}

void ThimoClass::editManual() {
	LCD.setCursor(10, 0);
	if (m_manualMode) {
		LCD.print(" ");
		LCD.print(" ");
		LCD.print("AUTO");
		m_manualMode = false;
	} else {
		LCD.print("MANUAL");
		m_manualMode = true;
	}
}

void ThimoClass::editClock() {
	DateTime now = RTC.now();
	uint8_t day = now.day();
	uint8_t month = now.month();
	uint16_t year = now.year();
	uint8_t hour = now.hour();
	uint8_t minute = now.minute();
	uint8_t second = now.second();
	bool changed = false;
	
	LCD.setCursor(4,0);
	LCD.blink();

	// adjust day
	while (!ButtonS.toggled() || ButtonS.read() != Button::PRESSED) {
		if (ButtonN.toggled() && ButtonN.read() == Button::PRESSED) {
			if (++day > 31) {
				day = 1;
			}
			changed = true;
		}
		if (ButtonP.toggled() && ButtonP.read() == Button::PRESSED) {
			if (--day > 31) {
				day = 31;
			}
			changed = true;
		}
		if (changed) {
			LCD.setCursor(3,0);
			if (day < 10) {
				LCD.print("0");
			}
			LCD.print(day);
			LCD.setCursor(4,0);
			changed = false;
		}
	}

	// adjust month
	LCD.setCursor(7,0);
	while (!ButtonS.toggled() || ButtonS.read() != Button::PRESSED) {
		if (ButtonN.toggled() && ButtonN.read() == Button::PRESSED) {
			if (++month > 12) {
				month = 1;
			}
			changed = true;
		}
		if (ButtonP.toggled() && ButtonP.read() == Button::PRESSED) {
			if (--month > 12) {
				month = 12;
			}
			changed = true;
		}
		if (changed) {
			LCD.setCursor(6,0);
			if (month < 10) {
				LCD.print("0");
			}
			LCD.print(month);
			LCD.setCursor(7,0);
			changed = false;
		}
	}

	// adjust year
	LCD.setCursor(12,0);
	while (!ButtonS.toggled() || ButtonS.read() != Button::PRESSED) {
		if (ButtonN.toggled() && ButtonN.read() == Button::PRESSED) {
			if (++year > 2049) {
				year = 2000;
			}
			changed = true;
		}
		if (ButtonP.toggled() && ButtonP.read() == Button::PRESSED) {
			if (--year < 2000) {
				year = 2049;
			}
			changed = true;
		}
		if (changed) {
			LCD.setCursor(9,0);
			LCD.print(year);
			LCD.setCursor(12,0);
			changed = false;
		}
	}

	// adjust hour
	LCD.setCursor(5,1);
	while (!ButtonS.toggled() || ButtonS.read() != Button::PRESSED) {
		if (ButtonN.toggled() && ButtonN.read() == Button::PRESSED) {
			if (++hour > 23) {
				hour = 0;
			}
			changed = true;
		}
		if (ButtonP.toggled() && ButtonP.read() == Button::PRESSED) {
			if (--hour > 23) {
				hour = 23;
			}
			changed = true;
		}
		if (changed) {
			LCD.setCursor(4,1);
			if (hour < 10) {
				LCD.print("0");
			}
			LCD.print(hour);
			LCD.setCursor(5,1);
			changed = false;
		}
	}

	// adjust minute
	LCD.setCursor(8,1);
	while (!ButtonS.toggled() || ButtonS.read() != Button::PRESSED) {
		if (ButtonN.toggled() && ButtonN.read() == Button::PRESSED) {
			if (++minute > 59) {
				minute = 0;
			}
			changed = true;
		}
		if (ButtonP.toggled() && ButtonP.read() == Button::PRESSED) {
			if (--minute > 59) {
				minute = 59;
			}
			changed = true;
		}
		if (changed) {
			LCD.setCursor(7,1);
			if (minute < 10) {
				LCD.print("0");
			}
			LCD.print(minute);
			LCD.setCursor(8,1);
			changed = false;
		}
	}

	// adjust second
	LCD.setCursor(11,1);
	while (!ButtonS.toggled() || ButtonS.read() != Button::PRESSED) {
		if (ButtonN.toggled() && ButtonN.read() == Button::PRESSED) {
			if (++second > 59) {
				second = 0;
			}
			changed = true;
		}
		if (ButtonP.toggled() && ButtonP.read() == Button::PRESSED) {
			if (--second > 59) {
				second = 59;
			}
			changed = true;
		}
		if (changed) {
			LCD.setCursor(10,1);
			if (second < 10) {
				LCD.print("0");
			}
			LCD.print(second);
			LCD.setCursor(11,1);
			changed = false;
		}
	}

	RTC.adjust(DateTime(year, month, day, hour, minute, second));
	LCD.noBlink();
}

void ThimoClass::editTimetable(int hfrom, int hto) {
	int col = 2;
	boolean changed = false;
	
	LCD.blink();

	for (int h = hfrom; h <= hto; h++) {
		LCD.setCursor(col, 1);
		while (!ButtonS.toggled() || ButtonS.read() != Button::PRESSED) {
			if (ButtonN.toggled() && ButtonN.read() == Button::PRESSED) {
				if (++m_timetable[h] > 30) {
					m_timetable[h] = 0;
				}
				changed = true;
			}
			if (ButtonP.toggled() && ButtonP.read() == Button::PRESSED) {
				if (--m_timetable[h] > 30) {
					m_timetable[h] = 30;
				}
				changed = true;
			}
			if (changed) {
				LCD.setCursor(col, 1);
				if (m_timetable[h] < 10) {
					LCD.print(" ");
				}
				LCD.print(m_timetable[h]);
				LCD.setCursor(col, 1);
				changed = false;
			}
		}
		RTC.writenvram(h, m_timetable[h]);
		col += 3;
	}
	
	LCD.noBlink();
}

void ThimoClass::toggleRelay() {
	uint8_t s;
	
	if (m_manualMode) {
		s = potTemperature() > m_temperature ? HIGH : LOW;
	} else {
		DateTime now = RTC.now();
		s = m_timetable[now.hour()] > (uint8_t)m_temperature ? HIGH : LOW;
	}

	if (s != digitalRead(RELAY_PIN)) {
		m_backlightTimer = millis();
	}

	digitalWrite(RELAY_PIN, s);
}

float ThimoClass::potTemperature() {
	return 30.0f;
}

ThimoClass Thimo;
