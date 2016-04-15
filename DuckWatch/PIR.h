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
		// debug serial serial connection
		serial *p_serial;

		// PIR data out pin
		uint8_t pin;

	public:
		// No public class variables

		// this is the constructor sets up the PIR for use
		PIR (serial *ptr_serial, uint8_t p);

		// this method runs the PIR task
		void PIRTask (void);

		// this method checks if the sensor has put a high signal on the pin
		bool isActive (void);
};

#endif // __PIR_H__