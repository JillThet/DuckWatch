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
#include <avr/interrupt.h>
#include <util/delay.h>

#include "serial.h"
#include "shares.h"

#define PIR_DDR		DDRD
#define PIR_PORT 	PORTD
#define PIR_PIN 	PIND

#define LN_1_PIN	5
#define LN_2_PIN	6	

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
		// serial debug connection
		serial *p_serial;

		// PIR data out pin
		uint8_t pin;

	public:
		// No public class variables

		// this constructor sets up the PIR for use
		PIR (serial *ptr_serial, uint8_t p);

		// this method runs the PIR task
		void PIRTask (void);

		// this method checks if the sensor has put a high signal on the pin
		bool isActive (void);
};

#endif // __PIR_H__