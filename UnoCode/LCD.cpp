/*****************************************************************************
 * File:		LCD.cpp
 * Description:	This file contains the LCD class, which allows the 
 *				microcontroller to communicate with LCD devices via the
 *				SPI protocol.
 * Created:		5/11/2016
 * Authors:		Jill Thetford, Daniel Griffith
 * 				Modified from Adafruit Library at
 *				https://github.com/adafruit/ST7565-LCD
 ****************************************************************************/ 

#include "LCD.h"
#include "shares.h"

const uint8_t PROGMEM font[] = {
	0x00, 0x00, 0x00, 0x00, 0x00,  // Ascii 0
	0x7C, 0xDA, 0xF2, 0xDA, 0x7C,  //ASC(01)
	0x7C, 0xD6, 0xF2, 0xD6, 0x7C,  //ASC(02)
	0x38, 0x7C, 0x3E, 0x7C, 0x38,
	0x18, 0x3C, 0x7E, 0x3C, 0x18,
	0x38, 0xEA, 0xBE, 0xEA, 0x38,
	0x38, 0x7A, 0xFE, 0x7A, 0x38,
	0x00, 0x18, 0x3C, 0x18, 0x00,
	0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
	0x00, 0x18, 0x24, 0x18, 0x00,
	0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
	0x0C, 0x12, 0x5C, 0x60, 0x70,
	0x64, 0x94, 0x9E, 0x94, 0x64,
	0x02, 0xFE, 0xA0, 0xA0, 0xE0,
	0x02, 0xFE, 0xA0, 0xA4, 0xFC,
	0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
	0xFE, 0x7C, 0x38, 0x38, 0x10,
	0x10, 0x38, 0x38, 0x7C, 0xFE,
	0x28, 0x44, 0xFE, 0x44, 0x28,
	0xFA, 0xFA, 0x00, 0xFA, 0xFA,
	0x60, 0x90, 0xFE, 0x80, 0xFE,
	0x00, 0x66, 0x91, 0xA9, 0x56,
	0x06, 0x06, 0x06, 0x06, 0x06,
	0x29, 0x45, 0xFF, 0x45, 0x29,
	0x10, 0x20, 0x7E, 0x20, 0x10,
	0x08, 0x04, 0x7E, 0x04, 0x08,
	0x10, 0x10, 0x54, 0x38, 0x10,
	0x10, 0x38, 0x54, 0x10, 0x10,
	0x78, 0x08, 0x08, 0x08, 0x08,
	0x30, 0x78, 0x30, 0x78, 0x30,
	0x0C, 0x1C, 0x7C, 0x1C, 0x0C,
	0x60, 0x70, 0x7C, 0x70, 0x60,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFA, 0x00, 0x00,
	0x00, 0xE0, 0x00, 0xE0, 0x00,
	0x28, 0xFE, 0x28, 0xFE, 0x28,
	0x24, 0x54, 0xFE, 0x54, 0x48,
	0xC4, 0xC8, 0x10, 0x26, 0x46,
	0x6C, 0x92, 0x6A, 0x04, 0x0A,
	0x00, 0x10, 0xE0, 0xC0, 0x00,
	0x00, 0x38, 0x44, 0x82, 0x00,
	0x00, 0x82, 0x44, 0x38, 0x00,
	0x54, 0x38, 0xFE, 0x38, 0x54,
	0x10, 0x10, 0x7C, 0x10, 0x10,
	0x00, 0x01, 0x0E, 0x0C, 0x00,
	0x10, 0x10, 0x10, 0x10, 0x10,
	0x00, 0x00, 0x06, 0x06, 0x00,
	0x04, 0x08, 0x10, 0x20, 0x40,
	0x7C, 0x8A, 0x92, 0xA2, 0x7C,
	0x00, 0x42, 0xFE, 0x02, 0x00,
	0x4E, 0x92, 0x92, 0x92, 0x62,
	0x84, 0x82, 0x92, 0xB2, 0xCC,
	0x18, 0x28, 0x48, 0xFE, 0x08,
	0xE4, 0xA2, 0xA2, 0xA2, 0x9C,
	0x3C, 0x52, 0x92, 0x92, 0x8C,
	0x82, 0x84, 0x88, 0x90, 0xE0,
	0x6C, 0x92, 0x92, 0x92, 0x6C,
	0x62, 0x92, 0x92, 0x94, 0x78,
	0x00, 0x00, 0x28, 0x00, 0x00,
	0x00, 0x02, 0x2C, 0x00, 0x00,
	0x00, 0x10, 0x28, 0x44, 0x82,
	0x28, 0x28, 0x28, 0x28, 0x28,
	0x00, 0x82, 0x44, 0x28, 0x10,
	0x40, 0x80, 0x9A, 0x90, 0x60,
	0x7C, 0x82, 0xBA, 0x9A, 0x72,
	0x3E, 0x48, 0x88, 0x48, 0x3E,
	0xFE, 0x92, 0x92, 0x92, 0x6C,
	0x7C, 0x82, 0x82, 0x82, 0x44,
	0xFE, 0x82, 0x82, 0x82, 0x7C,
	0xFE, 0x92, 0x92, 0x92, 0x82,
	0xFE, 0x90, 0x90, 0x90, 0x80,
	0x7C, 0x82, 0x82, 0x8A, 0xCE,
	0xFE, 0x10, 0x10, 0x10, 0xFE,
	0x00, 0x82, 0xFE, 0x82, 0x00,
	0x04, 0x02, 0x82, 0xFC, 0x80,
	0xFE, 0x10, 0x28, 0x44, 0x82,
	0xFE, 0x02, 0x02, 0x02, 0x02,
	0xFE, 0x40, 0x38, 0x40, 0xFE,
	0xFE, 0x20, 0x10, 0x08, 0xFE,
	0x7C, 0x82, 0x82, 0x82, 0x7C,
	0xFE, 0x90, 0x90, 0x90, 0x60,
	0x7C, 0x82, 0x8A, 0x84, 0x7A,
	0xFE, 0x90, 0x98, 0x94, 0x62,
	0x64, 0x92, 0x92, 0x92, 0x4C,
	0xC0, 0x80, 0xFE, 0x80, 0xC0,
	0xFC, 0x02, 0x02, 0x02, 0xFC,
	0xF8, 0x04, 0x02, 0x04, 0xF8,
	0xFC, 0x02, 0x1C, 0x02, 0xFC,
	0xC6, 0x28, 0x10, 0x28, 0xC6,
	0xC0, 0x20, 0x1E, 0x20, 0xC0,
	0x86, 0x9A, 0x92, 0xB2, 0xC2,
	0x00, 0xFE, 0x82, 0x82, 0x82,
	0x40, 0x20, 0x10, 0x08, 0x04,
	0x00, 0x82, 0x82, 0x82, 0xFE,
	0x20, 0x40, 0x80, 0x40, 0x20,
	0x02, 0x02, 0x02, 0x02, 0x02,
	0x00, 0xC0, 0xE0, 0x10, 0x00,
	0x04, 0x2A, 0x2A, 0x1E, 0x02,
	0xFE, 0x14, 0x22, 0x22, 0x1C,
	0x1C, 0x22, 0x22, 0x22, 0x14,
	0x1C, 0x22, 0x22, 0x14, 0xFE,
	0x1C, 0x2A, 0x2A, 0x2A, 0x18,
	0x00, 0x10, 0x7E, 0x90, 0x40,
	0x18, 0x25, 0x25, 0x39, 0x1E,
	0xFE, 0x10, 0x20, 0x20, 0x1E,
	0x00, 0x22, 0xBE, 0x02, 0x00,
	0x04, 0x02, 0x02, 0xBC, 0x00,
	0xFE, 0x08, 0x14, 0x22, 0x00,
	0x00, 0x82, 0xFE, 0x02, 0x00,
	0x3E, 0x20, 0x1E, 0x20, 0x1E,
	0x3E, 0x10, 0x20, 0x20, 0x1E,
	0x1C, 0x22, 0x22, 0x22, 0x1C,
	0x3F, 0x18, 0x24, 0x24, 0x18,
	0x18, 0x24, 0x24, 0x18, 0x3F,
	0x3E, 0x10, 0x20, 0x20, 0x10,
	0x12, 0x2A, 0x2A, 0x2A, 0x24,
	0x20, 0x20, 0xFC, 0x22, 0x24,
	0x3C, 0x02, 0x02, 0x04, 0x3E,
	0x38, 0x04, 0x02, 0x04, 0x38,
	0x3C, 0x02, 0x0C, 0x02, 0x3C,
	0x22, 0x14, 0x08, 0x14, 0x22,
	0x32, 0x09, 0x09, 0x09, 0x3E,
	0x22, 0x26, 0x2A, 0x32, 0x22,
	0x00, 0x10, 0x6C, 0x82, 0x00,
	0x00, 0x00, 0xEE, 0x00, 0x00,
	0x00, 0x82, 0x6C, 0x10, 0x00,
	0x40, 0x80, 0x40, 0x20, 0x40,
	0x3C, 0x64, 0xC4, 0x64, 0x3C,
	0x78, 0x85, 0x85, 0x86, 0x48,
	0x5C, 0x02, 0x02, 0x04, 0x5E,
	0x1C, 0x2A, 0x2A, 0xAA, 0x9A,
	0x84, 0xAA, 0xAA, 0x9E, 0x82,
	0x84, 0x2A, 0x2A, 0x1E, 0x82,
	0x84, 0xAA, 0x2A, 0x1E, 0x02,
	0x04, 0x2A, 0xAA, 0x9E, 0x02,
	0x30, 0x78, 0x4A, 0x4E, 0x48,
	0x9C, 0xAA, 0xAA, 0xAA, 0x9A,
	0x9C, 0x2A, 0x2A, 0x2A, 0x9A,
	0x9C, 0xAA, 0x2A, 0x2A, 0x1A,
	0x00, 0x00, 0xA2, 0x3E, 0x82,
	0x00, 0x40, 0xA2, 0xBE, 0x42,
	0x00, 0x80, 0xA2, 0x3E, 0x02,
	0x0F, 0x94, 0x24, 0x94, 0x0F,
	0x0F, 0x14, 0xA4, 0x14, 0x0F,
	0x3E, 0x2A, 0xAA, 0xA2, 0x00,
	0x04, 0x2A, 0x2A, 0x3E, 0x2A,
	0x3E, 0x50, 0x90, 0xFE, 0x92,
	0x4C, 0x92, 0x92, 0x92, 0x4C,
	0x4C, 0x12, 0x12, 0x12, 0x4C,
	0x4C, 0x52, 0x12, 0x12, 0x0C,
	0x5C, 0x82, 0x82, 0x84, 0x5E,
	0x5C, 0x42, 0x02, 0x04, 0x1E,
	0x00, 0xB9, 0x05, 0x05, 0xBE,
	0x9C, 0x22, 0x22, 0x22, 0x9C,
	0xBC, 0x02, 0x02, 0x02, 0xBC,
	0x3C, 0x24, 0xFF, 0x24, 0x24,
	0x12, 0x7E, 0x92, 0xC2, 0x66,
	0xD4, 0xF4, 0x3F, 0xF4, 0xD4,
	0xFF, 0x90, 0x94, 0x6F, 0x04,
	0x03, 0x11, 0x7E, 0x90, 0xC0,
	0x04, 0x2A, 0x2A, 0x9E, 0x82,
	0x00, 0x00, 0x22, 0xBE, 0x82,
	0x0C, 0x12, 0x12, 0x52, 0x4C,
	0x1C, 0x02, 0x02, 0x44, 0x5E,
	0x00, 0x5E, 0x50, 0x50, 0x4E,
	0xBE, 0xB0, 0x98, 0x8C, 0xBE,
	0x64, 0x94, 0x94, 0xF4, 0x14,
	0x64, 0x94, 0x94, 0x94, 0x64,
	0x0C, 0x12, 0xB2, 0x02, 0x04,
	0x1C, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x1C,
	0xF4, 0x08, 0x13, 0x35, 0x5D,
	0xF4, 0x08, 0x14, 0x2C, 0x5F,
	0x00, 0x00, 0xDE, 0x00, 0x00,
	0x10, 0x28, 0x54, 0x28, 0x44,
	0x44, 0x28, 0x54, 0x28, 0x10,
	0x55, 0x00, 0xAA, 0x00, 0x55,
	0x55, 0xAA, 0x55, 0xAA, 0x55,
	0xAA, 0x55, 0xAA, 0x55, 0xAA,
	0x00, 0x00, 0x00, 0xFF, 0x00,
	0x08, 0x08, 0x08, 0xFF, 0x00,
	0x28, 0x28, 0x28, 0xFF, 0x00,
	0x08, 0x08, 0xFF, 0x00, 0xFF,
	0x08, 0x08, 0x0F, 0x08, 0x0F,
	0x28, 0x28, 0x28, 0x3F, 0x00,
	0x28, 0x28, 0xEF, 0x00, 0xFF,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x28, 0x28, 0x2F, 0x20, 0x3F,
	0x28, 0x28, 0xE8, 0x08, 0xF8,
	0x08, 0x08, 0xF8, 0x08, 0xF8,
	0x28, 0x28, 0x28, 0xF8, 0x00,
	0x08, 0x08, 0x08, 0x0F, 0x00,
	0x00, 0x00, 0x00, 0xF8, 0x08,
	0x08, 0x08, 0x08, 0xF8, 0x08,
	0x08, 0x08, 0x08, 0x0F, 0x08,
	0x00, 0x00, 0x00, 0xFF, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0xFF, 0x08,
	0x00, 0x00, 0x00, 0xFF, 0x28,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0x00, 0xF8, 0x08, 0xE8,
	0x00, 0x00, 0x3F, 0x20, 0x2F,
	0x28, 0x28, 0xE8, 0x08, 0xE8,
	0x28, 0x28, 0x2F, 0x20, 0x2F,
	0x00, 0x00, 0xFF, 0x00, 0xEF,
	0x28, 0x28, 0x28, 0x28, 0x28,
	0x28, 0x28, 0xEF, 0x00, 0xEF,
	0x28, 0x28, 0x28, 0xE8, 0x28,
	0x08, 0x08, 0xF8, 0x08, 0xF8,
	0x28, 0x28, 0x28, 0x2F, 0x28,
	0x08, 0x08, 0x0F, 0x08, 0x0F,
	0x00, 0x00, 0xF8, 0x08, 0xF8,
	0x00, 0x00, 0x00, 0xF8, 0x28,
	0x00, 0x00, 0x00, 0x3F, 0x28,
	0x00, 0x00, 0x0F, 0x08, 0x0F,
	0x08, 0x08, 0xFF, 0x08, 0xFF,
	0x28, 0x28, 0x28, 0xFF, 0x28,
	0x08, 0x08, 0x08, 0xF8, 0x00,
	0x00, 0x00, 0x00, 0x0F, 0x08,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0x1C, 0x22, 0x22, 0x1C, 0x22,
	0x3E, 0x54, 0x54, 0x7C, 0x28,
	0x7E, 0x40, 0x40, 0x60, 0x60,
	0x40, 0x7E, 0x40, 0x7E, 0x40,
	0xC6, 0xAA, 0x92, 0x82, 0xC6,
	0x1C, 0x22, 0x22, 0x3C, 0x20,
	0x02, 0x7E, 0x04, 0x78, 0x04,
	0x60, 0x40, 0x7E, 0x40, 0x40,
	0x99, 0xA5, 0xE7, 0xA5, 0x99,
	0x38, 0x54, 0x92, 0x54, 0x38,
	0x32, 0x4E, 0x80, 0x4E, 0x32,
	0x0C, 0x52, 0xB2, 0xB2, 0x0C,
	0x0C, 0x12, 0x1E, 0x12, 0x0C,
	0x3D, 0x46, 0x5A, 0x62, 0xBC,
	0x7C, 0x92, 0x92, 0x92, 0x00,
	0x7E, 0x80, 0x80, 0x80, 0x7E,
	0x54, 0x54, 0x54, 0x54, 0x54,
	0x22, 0x22, 0xFA, 0x22, 0x22,
	0x02, 0x8A, 0x52, 0x22, 0x02,
	0x02, 0x22, 0x52, 0x8A, 0x02,
	0x00, 0x00, 0xFF, 0x80, 0xC0,
	0x07, 0x01, 0xFF, 0x00, 0x00,
	0x10, 0x10, 0xD6, 0xD6, 0x10,
	0x6C, 0x48, 0x6C, 0x24, 0x6C,
	0x60, 0xF0, 0x90, 0xF0, 0x60,
	0x00, 0x00, 0x18, 0x18, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x00,
	0x0C, 0x02, 0xFF, 0x80, 0x80,
	0x00, 0xF8, 0x80, 0x80, 0x78,
	0x00, 0x98, 0xB8, 0xE8, 0x48,
	0x00, 0x3C, 0x3C, 0x3C, 0x3C
};

const uint8_t pagemap[] = { 3, 2, 1, 0, 7, 6, 5, 4 };

/*****************************************************************************
 * Method:		LCD
 * Description:	This constructor sets up the LCD and the SPI protocol 
 *				on the ATmega328P			
 ****************************************************************************/
LCD::LCD (void)
{

	DDRB |= (1 << MOSI) | (1 << SCK) | (1 << SS);
	DDRC |= (1 << A0) | (1 << RST);

	clear_buffer();

	spi_init();
	setup_lcd();

	EICRA = 0x03;
	EIMSK = 0x01;
	EIFR = 0x01;
}

/*****************************************************************************
 * Method:		setup_lcd
 * Description:	This method sets up the LCD to display output
 ****************************************************************************/
void LCD::setup_lcd(void){
	lcd_init();

	lcd_command(CMD_DISPLAY_ON);

	lcd_set_brightness(0x18);

	write_buffer();
}

/*****************************************************************************
 * Method:		lcd_init
 * Description:	This method initializes the LCD for proper functionality
 ****************************************************************************/
void LCD::lcd_init(void){
	// toggle RST low to reset; SS low so LCD will listen
	PORTB &= ~(1 << SS);
	PORTC &= ~(1 << RST);
	_delay_ms(500);
	
	PORTC |= (1 << RST);
	PORTB |= (1 << SS);

	// ADC select
	lcd_command(CMD_SET_ADC_NORMAL);
	// SHL select
	lcd_command(CMD_SET_COM_NORMAL);
	// LCD bias select
	lcd_command(CMD_SET_BIAS_7);
	// Initial display line
	lcd_command(CMD_SET_DISP_START_LINE);

	// turn on voltage converter (VC=1, VR=0, VF=0)
	lcd_command(CMD_SET_POWER_CONTROL | 0x4);
	// wait for 50% rising
	_delay_ms(50);

	// turn on voltage regulator (VC=1, VR=1, VF=0)
	lcd_command(CMD_SET_POWER_CONTROL | 0x6);
	// wait >=50ms
	_delay_ms(75);

	// turn on voltage follower (VC=1, VR=1, VF=1)
	lcd_command(CMD_SET_POWER_CONTROL | 0x7);
	// wait
	_delay_ms(10);

	// set lcd operating voltage (regulator resistor, ref voltage resistor)
	lcd_command(CMD_SET_RESISTOR_RATIO | 0x4);
}

/*****************************************************************************
 * Method:		spi_init
 * Description:	This method initializes the SPI Protocol using the appropriate
 *				registers
 ****************************************************************************/
void LCD::spi_init(void){
	SPCR = (1 << SPE) | (1 << MSTR);
	PORTB = (1 << SS);						// make sure SS is high
}

/*****************************************************************************
 * Method:		spiwrite
 * Description:	This method writes data to a SPI device.
 * 
 * Parameters:	data - data to write using the SPI protocol
 ****************************************************************************/
inline void LCD::spiwrite(uint8_t data) {
	PORTB &= ~(1 << SS);
	SPDR = data;
	while (!(SPSR & (1<<SPIF)));
	PORTB |= (1 << SS);
}

/*****************************************************************************
 * Method:		lcd_command
 * Description:	This method sends a command to the LCD
 * 
 * Parameters:	data - the command to be executed
 ****************************************************************************/
void LCD::lcd_command(uint8_t data) {
	PORTC &= ~(1 << A0);

	spiwrite(data);
}

/*****************************************************************************
 * Method:		lcd_set_brightness
 * Description:	This method sets the brightness of the LCD
 * 
 * Parameters:	val - the new bightness value of the LCD
 ****************************************************************************/
void LCD::lcd_set_brightness(uint8_t val) {
	lcd_command(CMD_SET_VOLUME_FIRST);
	lcd_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

/*****************************************************************************
 * Method:		lcd_data
 * Description:	This method sends data to the LCD
 * 
 * Parameters:	data - the data being sent to the LCD
 ****************************************************************************/
void LCD::lcd_data(uint8_t data) {
	PORTC |= (1 << A0);

	spiwrite(data);
}

/*****************************************************************************
 * Method:		write_buffer
 * Description:	This method writes the buffer to the LCD
 ****************************************************************************/
void LCD::write_buffer(/*uint8_t *buffer*/) {
	uint8_t data, page;

	for(page = 0; page < 8; page++) {

		lcd_command(CMD_SET_PAGE | pagemap[page]);
		lcd_command(CMD_SET_COLUMN_LOWER | (0x0 & 0xf));
		lcd_command(CMD_SET_COLUMN_UPPER | ((0x0 >> 4) & 0xf));
		lcd_command(CMD_RMW);
		lcd_data(0xff);
		
		for(data = 0; data < 128; data++) {
			lcd_data(buff[(128*page)+data]);
		}
	}
}

/*****************************************************************************
 * Method:		setpixel
 * Description:	This method sets a single pixel on the LCD
 * 
 * Parameters:	col 	- the column the pixel is in
 *				row 	- the row the pixel is in
 *				fill 	- the fill value of the pixel, 1 = black, 0 = clear
 ****************************************************************************/
void LCD::setpixel(uint8_t col, uint8_t row, uint8_t fill) {
  	if ((col >= LCDWIDTH) || (row >= LCDHEIGHT))
  	{
    	return;
    }

  	if (fill) 
  	{
    	buff[col+ (row/8)*128] |= (1 << (7-(row%8)));  
    }
	else
	{
		buff[col+ (row/8)*128] &= ~(1 << (7-(row%8)));
	}
}

/*****************************************************************************
 * Method:		drawstring
 * Description:	This method draws a string of characters on the LCD
 * 
 * Parameters:	col 	- the column the string starts at
 *				row 	- the row the string starts at
 *				*str 	- the string to be sent to the LCD
 ****************************************************************************/
void LCD::drawstring(uint8_t col, uint8_t row, uint8_t *str){
	while (str[0] != 0) {
    
	    drawchar(col, row, str[0]);
	    str++;
	    col += 6; // 6 pixels wide
	    
	    if (col + 6 >= LCDWIDTH) {
	      col = 0;    // ran out of this line
	      row++;
	    }
	    
	    if (row >= (LCDHEIGHT/8))
	    {
	      return;        // ran out of space :(
	  	}
  	}
}

/*****************************************************************************
 * Method:		drawchar
 * Description:	This method draws a single character on the LCD
 * 
 * Parameters:	col 	- the column the character is at
 *				row 	- the row the character is at
 *				data 	- the character to be sent to the LCD
 ****************************************************************************/
void LCD::drawchar(/*uint8_t *buff,*/ uint8_t col, uint8_t row, uint8_t data) {
  	uint8_t i;

  	for (i =0; i<5; i++ ) {
    	buff[col + (row*128) ] = pgm_read_byte(font+(data*5)+i);
    	col++;
  	}
}

/*****************************************************************************
 * Method:		drawline
 * Description:	This method draws a line between two pixels on the LCD
 *				Uses Bresenham's Algorithm
 * 
 * Parameters:	col0 - the column the line starts at
 *				row0 - the row the line starts at
 *				col1 - the column the line ends at
 *				row1 - the row the line ends at
 *				fill - the fill value of the pixel, 1 = black, 0 = clear
 ****************************************************************************/
void LCD::drawline(/*uint8_t *buff,*/ uint8_t col0, uint8_t row0, 
              uint8_t col1, uint8_t row1, uint8_t fill) {

  uint8_t dx, dy, err, rowStep;
  uint8_t steep = abs(row1 - row0) > abs(col1 - col0);
  
  if (steep) {
    swap(col0, row0);
    swap(col1, row1);
  }

  if (col0 > col1) {
    swap(col0, col1);
    swap(row0, row1);
  }

  dx = col1 - col0;
  dy = abs(row1 - row0);

  err = dx / 2;

  if (row0 < row1) {
    rowStep = 1;
  } else {
    rowStep = -1;}

  for (; col0 < col1; col0++) {
    if (steep) {
      setpixel(row0, col0, fill);
    } else {
      setpixel(col0, row0, fill);
    }
    err -= dy;
	
    if (err < 0) {
      row0 += rowStep;
      err += dx;
    }
  }
}

/*****************************************************************************
 * Method:		fillrect
 * Description:	This method fills a rectangle of an arbitrary size on the LCD
 * 
 * Parameters:	col 	- the column the bottom left pixel of the 
 *						rectangle is at
 *				row 	- the row the bottom left pixel of the 
 *						rectangle is at
 *				width 	- the width of the rectangle
 *				height 	- the height of the rectangle
 *				fill 	- the fill value of the pixel, 1 = black, 0 = clear
 ****************************************************************************/
void LCD::fillrect(uint8_t col, uint8_t row, uint8_t width, 
	uint8_t height, uint8_t fill) {
  uint8_t colNdx;

  for (colNdx = col; colNdx < (col + width); colNdx++)
  {
	  drawline(colNdx, row, colNdx, row + height, fill);
  }
}

/*****************************************************************************
 * Method:		drawrect
 * Description:	This method draws a rectangle of an arbitrary size on the LCD
 * 
 * Parameters:	col 	- the column the bottom left pixel of the 
 *						rectangle is at
 *				row 	- the row the bottom left pixel of the 
 *						rectangle is at
 *				width 	- the width of the rectangle
 *				height 	- the height of the rectangle
 *				fill 	- the fill value of the pixel, 1 = black, 0 = clear
 ****************************************************************************/
void LCD::drawrect(uint8_t col, uint8_t row, uint8_t width, 
	uint8_t height, uint8_t fill) {
				  
  drawline(col, row, col + width, row, fill);
  drawline(col + width, row, col + width, row + height, fill);
  drawline(col, row, col, row + height, fill);
  drawline(col, row + height, col + width, row + height, fill);		  
}

/*****************************************************************************
 * Method:		clear_buffer
 * Description:	This method clears the buffer
 ****************************************************************************/
void LCD::clear_buffer(void) {
  memset(buff, 0, 1024);
}

/*void clear_screen(void) {
	uint8_t page, data;
	
	for(page = 0; page < 8; page++) {

		lcd_command(CMD_SET_PAGE | page);
		for(data = 0; data < 129; data++) {
			lcd_command(CMD_SET_COLUMN_LOWER | (data & 0xf));
			lcd_command(CMD_SET_COLUMN_UPPER | ((data >> 4) & 0xf));
			lcd_data(0x0);
		}
	}
}*/









