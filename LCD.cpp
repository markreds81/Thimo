/**
 * 
 * THIMO IoT remote programmable termostat
 * 
 * Filename: LCD.cpp
 * Created on: 20 Sep 2021
 * Description: Thimo LCD module
 * 
 * Copyright (C) 2020-2021 Marco Rossi (aka Mark Reds) <marco@markreds.it> 
 * 
 */

#include "LCD.h"
#include "config.h"

LCDModule::LCDModule(uint8_t addr) : m_addr(addr) {
	m_displayFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	m_backlight = LCD_NOBACKLIGHT;
}

void LCDModule::begin(uint8_t cols, uint8_t rows, uint8_t dotsize) {
	Wire.begin();
	
	if (rows > 1) {
		m_displayFunction |= LCD_2LINE;
	}

	m_numlines = rows;

	setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols); 
	
	// for some 1 line displays you can select a 10 pixel high font
	if (dotsize != LCD_5x8DOTS && rows == 1) {
		m_displayFunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50);

	// Now we pull both RS and R/W low to begin commands
	expanderWrite(m_backlight); // reset expander and turn backlight off (Bit 8 =1)
	delay(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// second try
	write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// third go!
	write4bits(0x03 << 4);
	delayMicroseconds(150);

	// finally, set to 4-bit interface
	write4bits(0x02 << 4);

	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | m_displayFunction);

	// turn the display on with no cursor or blinking default
	m_displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();

	// clear it off
	clear();

	// Initialize to default text direction (for roman languages)
	m_displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	command(LCD_ENTRYMODESET | m_displayMode);
}

void LCDModule::clear() {
	command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
	delayMicroseconds(2000);   // this command takes a long time!
}

void LCDModule::home() {
	command(LCD_RETURNHOME); // set cursor position to zero
	delayMicroseconds(2000); // this command takes a long time!
}

void LCDModule::setCursor(uint8_t col, uint8_t row) {
	const size_t maxlines = sizeof(m_rowOffsets) / sizeof(*m_rowOffsets);
	
	if (row > maxlines)	{
		row = maxlines - 1;		// we count rows starting w/0
	}

	if (row > m_numlines) {
		row = m_numlines - 1;	// we count rows starting w/0
	}
	
	command(LCD_SETDDRAMADDR | (col + m_rowOffsets[row]));
}

void LCDModule::noDisplay() {
	m_displayControl &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | m_displayControl);
}

void LCDModule::display() {
	m_displayControl |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | m_displayControl);
}

void LCDModule::noCursor() {
	m_displayControl &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | m_displayControl);
}

void LCDModule::cursor() {
	m_displayControl |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | m_displayControl);
}

void LCDModule::noBlink() {
	m_displayControl &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | m_displayControl);
}

void LCDModule::blink() {
	m_displayControl |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | m_displayControl);
}

void LCDModule::scrollDisplayLeft() {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LCDModule::scrollDisplayRight() {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LCDModule::leftToRight() {
	m_displayMode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | m_displayMode);
}

void LCDModule::rightToLeft() {
	m_displayMode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | m_displayMode);
}

void LCDModule::noAutoscroll() {
	m_displayMode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | m_displayMode);
}

void LCDModule::autoscroll() {
	m_displayMode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | m_displayMode);
}

void LCDModule::backlight(void) {
	m_backlight = LCD_BACKLIGHT;
	expanderWrite(0);
}

void LCDModule::noBacklight(void) {
	m_backlight = LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void LCDModule::setRowOffsets(int row0, int row1, int row2, int row3) {
  m_rowOffsets[0] = row0;
  m_rowOffsets[1] = row1;
  m_rowOffsets[2] = row2;
  m_rowOffsets[3] = row3;
}

void LCDModule::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i = 0; i < 8; i++) {
		write(charmap[i]);
	}
}

inline void LCDModule::command(uint8_t value) {
	send(value, 0);
}

inline size_t LCDModule::write(uint8_t value) {
	send(value, Rs); return 1;
}

void LCDModule::send(uint8_t value, uint8_t mode) {
	uint8_t hinib = value & 0xf0;
	uint8_t lonib = (value << 4) & 0xf0;
	write4bits((hinib) | mode);
	write4bits((lonib) | mode);
}

void LCDModule::write4bits(uint8_t value) {
	expanderWrite(value);
	pulseEnable(value);
}

void LCDModule::expanderWrite(uint8_t value) {
	Wire.beginTransmission(m_addr);
	Wire.write((int)(value) | m_backlight);
	Wire.endTransmission();
}

void LCDModule::pulseEnable(uint8_t value) {
	expanderWrite(value | En); // En high
	delayMicroseconds(1);	   // enable pulse must be >450ns

	expanderWrite(value & ~En); // En low
	delayMicroseconds(50);		// commands need > 37us to settle
}

LCDModule LCD(LCD_I2C_ADDRESS);
