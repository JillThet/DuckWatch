/*****************************************************************************
 * File:		serial.h
 * Description:	This file contains the declaration of a serial class which
 *				allows the microcontroller to communicate via a USART serial
 *				connection.
 * Created:		4/3/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 
#ifndef __SERIAL_H__
#define __SERIAL_H__

#define F_CPU 16000000UL

#include <stdlib.h>		// include standard library
#include <avr/io.h>		// input-output ports, special registers
#include <stdio.h>		// include standard I/O

#define SERIAL_TIMEOUT	500

#define SERIAL_MAX_SEND	255

/*****************************************************************************
 * Class:		serial
 * Description:	The serial class enables the microcontroller to communicate 
 *				over a USART serial connection.
 ****************************************************************************/
class serial
{
	protected:
		// no protected methods or variables
		
	private:
		uint16_t baud_rate;		// the baud rate for serial connection
		uint32_t clk_speed;		// the clock speed of the microcontroller
		
		// this method initializes the USART Connection
		void init (void);
		
		// this method sends a single byte of data over the TX line
		bool send (uint8_t data);
	
	public:
		// this is the constructor to set up the serial connection
		serial (uint16_t baud, uint32_t clk);
		
		// this method sends a string of data over the TX line
		uint8_t send (char *str);
		
		// this method receives a byte of data from the RX line
		uint8_t receive (void);
		
		// this method checks if there is data available on the RX line
		bool data_available (void);
	
};
#endif /* __SERIAL_H__ */