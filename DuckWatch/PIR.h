/*****************************************************************************
 * File:		PIR.h
 * Description:	This file contains the class definition for the PIR class.
 * Created:		4/13/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/
#ifndef __PIR_H__
#define __PIR_H__

#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "serial.h"
#include "shares.h"

#define PIR_DDR		DDRB
#define PIR_PORT 	PORTB
#define PIR_PIN 	PINB

/*****************************************************************************
 * Class:		PIR
 * Description:	The PIR class enables the microcontroller to 
 *				access the PIR sensor.
 ****************************************************************************/
class PIR
{
	protected:
		// No protected methods or class variables

	private:
		serial *p_serial;
		uint8_t pin;

	public:
		// No public class variables
		PIR (serial *ptr_serial, uint8_t p);

		void PIRTask (void);

		bool isActive (void);
};

#endif // __PIR_H__