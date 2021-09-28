/**
 * 
 * THIMO IoT remote programmable termostat
 * 
 * Filename: LCD.h
 * Created on: 20 Sep 2021
 * Description: Thimo LCD module
 * 
 * Copyright (C) 2020-2021 Marco Rossi (aka Mark Reds) <marco@markreds.it> 
 * 
 */

#ifndef _THIMO_LCD_H_
#define _THIMO_LCD_H_

#include <Arduino.h>
#include <Wire.h>

// commands
#define LCD_CLEARDISPLAY			0x01
#define LCD_RETURNHOME				0x02
#define LCD_ENTRYMODESET			0x04
#define LCD_DISPLAYCONTROL			0x08
#define LCD_CURSORSHIFT				0x10
#define LCD_FUNCTIONSET				0x20
#define LCD_SETCGRAMADDR			0x40
#define LCD_SETDDRAMADDR			0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT				0x00
#define LCD_ENTRYLEFT				0x02
#define LCD_ENTRYSHIFTINCREMENT		0x01
#define LCD_ENTRYSHIFTDECREMENT		0x00

// flags for display on/off control
#define LCD_DISPLAYON				0x04
#define LCD_DISPLAYOFF				0x00
#define LCD_CURSORON				0x02
#define LCD_CURSOROFF				0x00
#define LCD_BLINKON					0x01
#define LCD_BLINKOFF				0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE				0x08
#define LCD_CURSORMOVE				0x00
#define LCD_MOVERIGHT				0x04
#define LCD_MOVELEFT				0x00

// flags for function set
#define LCD_8BITMODE				0x10
#define LCD_4BITMODE				0x00
#define LCD_2LINE					0x08
#define LCD_1LINE					0x00
#define LCD_5x10DOTS				0x04
#define LCD_5x8DOTS					0x00

// flags for backlight control
#define LCD_BACKLIGHT				0x08
#define LCD_NOBACKLIGHT				0x00

#define En 							B00000100 // Enable bit
#define Rw							B00000010 // Read/Write bit
#define Rs							B00000001 // Register select bit

class LCDModule : public Print
{
public:
	LCDModule(uint8_t addr);

	void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
	
	void clear();
	void home();

	void noDisplay();
	void display();
	void noBlink();
	void blink();
	void noCursor();
	void cursor();
	void scrollDisplayLeft();
	void scrollDisplayRight();
	void leftToRight();
	void rightToLeft();
	void noAutoscroll();
	void autoscroll();	
	void noBacklight();
	void backlight();
	void shiftIncrement();
	void shiftDecrement();
	void printLeft();
	void printRight();
	
	void setRowOffsets(int row1, int row2, int row3, int row4);
	void createChar(uint8_t, uint8_t[]);
	void setCursor(uint8_t, uint8_t);
	virtual size_t write(uint8_t);
	void command(uint8_t);
	
	using Print::write;
private:
	void send(uint8_t, uint8_t);
	void write4bits(uint8_t);
	void expanderWrite(uint8_t);
	void pulseEnable(uint8_t);
	
	uint8_t m_addr;
	uint8_t m_displayFunction;
	uint8_t m_displayControl;
	uint8_t m_displayMode;
	uint8_t m_numlines;
	uint8_t m_backlight;
	uint8_t m_rowOffsets[4];	
};

extern LCDModule LCD;

#endif
