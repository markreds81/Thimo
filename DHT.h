/**
 * 
 * THIMO IoT remote programmable termostat
 * 
 * Filename: DHT.h
 * Created on: 21 Sep 2021
 * Description: Thimo DHT module
 * 
 * Based on:
 *   DHT Temperature & Humidity Sensor library for Arduino & ESP32
 *   Written by Mark Ruys, mark@paracas.nl
 *   Updated to work with ESP32 by Bernd Giesecke, bernd@giesecke.tk
 *   https://github.com/beegee-tokyo/arduino-DHTesp
 * 
 * Copyright (C) 2020-2021 Marco Rossi (aka Mark Reds) <marco@markreds.it> 
 * 
 */

#ifndef _THIMO_DHT_H_
#define _THIMO_DHT_H_

#include <Arduino.h>
#include "config.h"

// Reference: http://epb.apogee.net/res/refcomf.asp (References invalid)
enum ComfortState {
	Comfort_OK = 0,
	Comfort_TooHot = 1,
	Comfort_TooCold = 2,
	Comfort_TooDry = 4,
	Comfort_TooHumid = 8,
	Comfort_HotAndHumid = 9,
	Comfort_HotAndDry = 5,
	Comfort_ColdAndHumid = 10,
	Comfort_ColdAndDry = 6
};

// References https://en.wikipedia.org/wiki/Dew_point ==> Relationship to human comfort
enum PerceptionState {
	Perception_Dry = 0,
	Perception_VeryComfy = 1,
	Perception_Comfy = 2,
	Perception_Ok = 3,
	Perception_UnComfy = 4,
	Perception_QuiteUnComfy = 5,
	Perception_VeryUnComfy = 6,
	Perception_SevereUncomfy = 7
};

struct Environment {
	float temperature;
	float humidity;
};

struct ComfortProfile {
	//Represent the 4 line equations:
	//dry, humid, hot, cold, using the y = mx + b formula
	float m_tooHot_m, m_tooHot_b;
	float m_tooCold_m, m_tooHCold_b;
	float m_tooDry_m, m_tooDry_b;
	float m_tooHumid_m, m_tooHumid_b;

	inline bool isTooHot(float temp, float humidity) { return (temp > (humidity * m_tooHot_m + m_tooHot_b)); }
	inline bool isTooHumid(float temp, float humidity) { return (temp > (humidity * m_tooHumid_m + m_tooHumid_b)); }
	inline bool isTooCold(float temp, float humidity) { return (temp < (humidity * m_tooCold_m + m_tooHCold_b)); }
	inline bool isTooDry(float temp, float humidity) { return (temp < (humidity * m_tooDry_m + m_tooDry_b)); }

	inline float distanceTooHot(float temp, float humidity) { return temp - (humidity * m_tooHot_m + m_tooHot_b); }
	inline float distanceTooHumid(float temp, float humidity) { return temp - (humidity * m_tooHumid_m + m_tooHumid_b); }
	inline float distanceTooCold(float temp, float humidity) { return (humidity * m_tooCold_m + m_tooHCold_b) - temp; }
	inline float distanceTooDry(float temp, float humidity) { return (humidity * m_tooDry_m + m_tooDry_b) - temp; }
};

class DHTModule {
public:
	typedef enum {
		AUTO_DETECT,
		DHT11,
		DHT22,
		AM2302, // Packaged DHT22
		RHT03	// Equivalent to DHT22
	} Model;

	typedef enum {
		ERROR_NONE = 0,
		ERROR_RETRY,
		ERROR_TIMEOUT,
		ERROR_CHECKSUM
	} Status;

	DHTModule(uint8_t pin, Model model);
	virtual ~DHTModule();

	void begin();
	Status readSensor(Environment *env);

	int minimumSamplingPeriod();
	int8_t numberOfDecimalsTemperature();
	int8_t lowerBoundTemperature();
	int8_t upperBoundTemperature();
	int8_t numberOfDecimalsHumidity();
	int8_t lowerBoundHumidity();
	int8_t upperBoundHumidity();

	static float toFahrenheit(float fromCelcius);
	static float toCelsius(float fromFahrenheit);

	float computeHeatIndex(float temperature, float percentHumidity, bool isFahrenheit = false);
	float computeDewPoint(float temperature, float percentHumidity, bool isFahrenheit = false);
	float comfortRatio(ComfortState &destComfStatus, float temperature, float percentHumidity, bool isFahrenheit = false);
	ComfortProfile comfortProfile();
	void comfortProfile(ComfortProfile &c);
	bool isTooHot(float temp, float humidity);
	bool isTooHumid(float temp, float humidity);
	bool isTooCold(float temp, float humidity);
	bool isTooDry(float temp, float humidity);
	byte computePerception(float temperature, float percentHumidity, bool isFahrenheit = false);
	float computeAbsoluteHumidity(float temperature, float percentHumidity, bool isFahrenheit = false);

private:
	uint8_t m_pin;
	Model m_model;
	ComfortProfile m_comfort;
	unsigned long m_lastReadTime;
};

extern DHTModule DHT;

#endif
