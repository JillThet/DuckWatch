/*
 * UVIndex.h
 *
 * Created: 4/10/2016 1:50:36 PM
 *  Author: Jill
 */ 


#ifndef __UVINDEX_H__
#define __UVINDEX_H__

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "serial.h"

#define ACTIVE_MODE		1
#define STANDBY_MODE	0

#define UVINDEX_DDR		DDRA
#define UVINDEX_PORT	PORTA
#define UVINDEX_PIN		PINA

#define UV_EN_DDR		DDRD
#define UV_EN_PORT		PORTD
#define UV_EN_PIN		PIND

class UVIndex
{
	protected:
		// no protected methods or class variables
	
	private:
		serial *p_serial;
		
		uint8_t enable_pin;
		// no private methods
		
		void init (void);
		
		void enable (void);
		
		void disable (void);
		
		int16_t read (void);
		
	public:
		// no public class variables
		
		UVIndex(serial *ptr_serial, uint8_t pin);
		
		void UVIndexTask();
};



#endif /* __UVINDEX_H__ */