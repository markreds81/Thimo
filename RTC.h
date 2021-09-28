/**
 * 
 * THIMO IoT remote programmable termostat
 * 
 * Filename: RTC.h
 * Created on: 20 Sep 2021
 * Description: Thimo RTC module
 * 
 * Copyright (C) 2020-2021 Marco Rossi (aka Mark Reds) <marco@markreds.it> 
 * 
 */

#ifndef _THIMO_RTC_H_
#define _THIMO_RTC_H_

#include <RTClib.h>

class RTCModule : public RTC_DS1307 {
public:
	RTCModule();
	virtual ~RTCModule();
};

extern RTCModule RTC;

#endif
