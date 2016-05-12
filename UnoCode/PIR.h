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

#define LN_1_PIN	7
#define LN_2_PIN	4	

#define OPEN	0x00
#define LN_1	0x01
#define LN_2	0x02

// TODO - change these as necessary
#define MIN_LN_EMPTY	155
#define MIN_LN_FULL		100

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
		
		// Lane Identifier
		uint8_t lane;
		
		// this method sets up the timer for lane 1
		void init_timer_ln_1 (void);
		
		// this method sets up the timer for lane 2
		void init_timer_ln_2 (void);
		
	public:
		// No public class variables

		// this constructor sets up the PIR for use
		PIR (serial *ptr_serial, uint8_t p);

		// this method sets up the PIR for use
		void initPIR (void);
		
		// this method runs the PIR task
		void PIRTask (void);

		// this method checks if the sensor has put a high signal on the pin
		bool isActive (void);
};

#endif // __PIR_H__