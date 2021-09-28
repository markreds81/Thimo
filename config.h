/**
 * 
 * THIMO IoT remote programmable termostat
 * 
 * Filename: config.h
 * Created on: 20 Sep 2021
 * Description: Thimo configuration header file
 * 
 * Copyright (C) 2020-2021 Marco Rossi (aka Mark Reds) <marco@markreds.it> 
 * 
 */

#ifndef _THIMO_CONFIG_H_
#define _THIMO_CONFIG_H_

#define LCD_I2C_ADDRESS				0x27
#define LCD_BACKLIGHT_DURATION		10000UL	// turn off backlight after 10"
#define LCD_REFRESH_TIME			1000UL	// refresh display every 1"

#define DHT_PIN						23
#define RELAY_PIN					2

#define BUTTON_S_PIN				26
#define BUTTON_N_PIN				27
#define BUTTON_P_PIN				25

#endif
