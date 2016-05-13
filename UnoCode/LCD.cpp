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

/*****************************************************************************
 * Method:		LCD
 * Description:	This constructor sets up the LCD and the SPI protocol 
 *				on the ATmega328P			
 ****************************************************************************/
LCD::LCD (void)
{

	DDRB |= (1 << MOSI) | (1 << SCK) | (1 << SS);
	DDRC |= (1 << A0) | (1 << RST);

	spi_init(void);
	setup_lcd(void);

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

	write_buffer(void);
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
    
	    drawchar(buff, col, row, str[0]);
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
void clear_buffer(void) {
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










