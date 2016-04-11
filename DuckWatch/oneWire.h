/*
 * oneWire.h
 *
 * Created: 4/8/2016 4:12:42 PM
 *  Author: Jill
 */ 


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

class oneWire
{
	protected:
		// no protected methods or class variables
		
	private:
		serial *p_serial;
		uint8_t data_line;
		uint8_t dev_id;
		
		bool reset (void);
		void write_byte (uint8_t byte);
		void write_1 (void);
		void write_0 (void);
		uint8_t read_byte (void);
		int16_t convert_temp (int16_t raw);
		
	public:
		// no public class variables
		
		// Constructor
		oneWire (serial *ptr_serial, uint8_t pin, uint8_t id);
		
		// Task Method for running device
		void oneWireTask (void);
};



#endif /* __ONEWIRE_H__ */