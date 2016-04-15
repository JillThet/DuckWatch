/*****************************************************************************
 * File:		oneWire.h
 * Description:	This file contains the class definition for the oneWire class.
 * Created:		4/8/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/

#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

#include "serial.h"
#include <util/delay.h>

#define NO_ID				0
#define ID_SURFACE_TEMP		1
#define ID_UNDERWATER_TEMP	2

#define DATA_DDR	DDRB
#define DATA_PORT	PORTB
#define DATA_PIN	PINB

/*****************************************************************************
 * Class:		oneWire
 * Description:	The oneWire class enables the microcontroller to 
 *				communicate with other oneWire devices.
 ****************************************************************************/
class oneWire
{
	protected:
		// no protected methods or class variables
		
	private:
		// serial debug connection
		serial *p_serial;

		// oneWire data input/output pin
		uint8_t data_line;

		// unique device id
		uint8_t dev_id;
		
		// this method resets the oneWire device
		bool reset (void);

		// this method writes one byte to the oneWire device
		void write_byte (uint8_t byte);

		// this method writes a logical high to the oneWire device
		void write_1 (void);

		// this method writes a logical low to the oneWire device
		void write_0 (void);

		// this method reads one byte of data from the oneWire device
		uint8_t read_byte (void);

		// this method converts the raw temperature 
		// data to human readable data
		int16_t convert_temp (int16_t raw);
		
	public:
		// no public class variables
		
		// this constructor sets up the oneWire for use
		oneWire (serial *ptr_serial, uint8_t pin, uint8_t id);
		
		// this method runs the oneWire task
		void oneWireTask (void);
};



#endif /* __ONEWIRE_H__ */