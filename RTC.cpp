/**
 * 
 * THIMO IoT remote programmable termostat
 * 
 * Filename: RTC.cpp
 * Created on: 20 Sep 2021
 * Description: Thimo RTC module
 * 
 * Copyright (C) 2020-2021 Marco Rossi (aka Mark Reds) <marco@markreds.it> 
 * 
 */

#include "RTC.h"

RTCModule::RTCModule() : RTC_DS1307() {
	
}

RTCModule::~RTCModule() {
	
}

RTCModule RTC;
