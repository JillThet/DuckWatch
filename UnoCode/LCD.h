/*****************************************************************************
 * File:		LCD.h
 * Description:	This file contains the class definition for the LCD class.
 * Created:		5/11/2016
 * Authors:		Jill Thetford, Daniel Griffith
 * 				Modified from Adafruit Library at
 *				https://github.com/adafruit/ST7565-LCD
 ****************************************************************************/
#ifndef _LCD_H_
#define _LCD_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h> 
#include <util/delay.h>

/****************************************************************************/
// Constants
/****************************************************************************/

//LCD Commands Constants
#define CMD_DISPLAY_OFF			0xAE
#define CMD_DISPLAY_ON			0xAF

#define CMD_SET_DISP_START_LINE 0x40
#define CMD_SET_PAGE			0xB0

#define CMD_SET_COLUMN_UPPER	0x10
#define CMD_SET_COLUMN_LOWER	0x00

#define CMD_SET_ADC_NORMAL		0xA0
#define CMD_SET_ADC_REVERSE		0xA1

#define CMD_SET_DISP_NORMAL		0xA6
#define CMD_SET_DISP_REVERSE	0xA7

#define CMD_SET_ALLPTS_NORMAL	0xA4
#define CMD_SET_ALLPTS_ON		0xA5
#define CMD_SET_BIAS_9			0xA2 
#define CMD_SET_BIAS_7			0xA3

#define CMD_RMW					0xE0
#define CMD_RMW_CLEAR			0xEE
#define CMD_INTERNAL_RESET		0xE2
#define CMD_SET_COM_NORMAL		0xC0
#define CMD_SET_COM_REVERSE		0xC8
#define CMD_SET_POWER_CONTROL	0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST	0x81
#define CMD_SET_VOLUME_SECOND	0
#define CMD_SET_STATIC_OFF		0xAC
#define CMD_SET_STATIC_ON		0xAD
#define CMD_SET_STATIC_REG		0x0
#define CMD_SET_BOOSTER_FIRST	0xF8
#define CMD_SET_BOOSTER_234		0
#define CMD_SET_BOOSTER_5		1
#define CMD_SET_BOOSTER_6		3
#define CMD_NOP					0xE3
#define CMD_TEST				0xF0

#define BUF_SIZE 				1024

#define LCDWIDTH 				128
#define LCDHEIGHT 				64

// SPI Constants
#define MOSI					3  // PB pin 3
#define SCK						5  // PB pin 5
#define SS						2  // PB pin 2  

// LCD Controls Constants
#define A0						2
#define RST						3

/****************************************************************************/
// Macros
/****************************************************************************/
#define swap(a, b) { uint8_t t = a; a = b; b = t; }

class LCD{
	private:
		uint8_t pagemap[8];
		
		uint8_t buff[BUF_SIZE];
		char[20] surfStr;
		char[20] subStr;
		char[20] extStr;
		char[20] humStr;
		char[20] winStr;
		char[20] uvStr;
		
		// this method draws a single pixel on the LCD
		void setpixel(uint8_t col, uint8_t row, uint8_t fill);

		// this method draws an empty rectangle on the LCD
		void drawrect(uint8_t col, uint8_t row, uint8_t width,
			uint8_t height, uint8_t fill);

		// this method draws a line of pixels on the LCD
		void drawline(uint8_t col0, uint8_t row0, uint8_t col1, 
			uint8_t row1, uint8_t fill);

		// this method draws a character on the LCD
		void drawchar(uint8_t col, uint8_t row, uint8_t data);

		// this method draws a filled rectangle on the LCD
		void fillrect(uint8_t col, uint8_t row, uint8_t width, 
			uint8_t height, uint8_t fill);

		// this method draws a string of characters on the LCD
		void drawstring(uint8_t col, uint8_t row, uint8_t *str);

		// this method clears the buffer
		void clear_buffer();

		// this method initializes the SPI interface
		void spi_init(void);

		// this method writes data to the LCD
		void spiwrite(uint8_t data);

		// this method sets up the LCD for writing
		void setup_lcd(void);

		// this method sets up the LCD to prepare it for writing
		void lcd_init(void);

		// this method sends a command to the LCD
		void lcd_command(uint8_t data);

		// this method sends raw data to the LCD
		void lcd_data(uint8_t data);

		// this method sets the brightness of the LCD
		void lcd_set_brightness(uint8_t val);

		// this method clears the LCD
		//void clear_screen(void);

		// this method writes the LCD buffer to the LCD
		void write_buffer();

	public:
		LCD (void);

		void LCDTask (void);
};

#endif