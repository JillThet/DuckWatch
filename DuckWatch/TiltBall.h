/*****************************************************************************
 * File:		TiltBall.h
 * Description:	This file contains the class definition for the i2c class.
 * Created:		3/30/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/
#ifndef __TILTBALL_H__
#define __TILTBALL_H__

#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "serial.h"

#define TILT_DDR	DDRD
#define TILT_PORT	PORTD
#define TILT_PIN	PIND

/*****************************************************************************
 * Class:		TiltBall
 * Description:	The TiltBall class enables the microcontroller to 
 *				access the tilt ball sensor.
 ****************************************************************************/
class TiltBall
{
	protected:
		// No protected methods or class variables
		
	private:
		serial *p_serial;
		 //16 bit prev read register ******
		uint8_t pin;
	
	public:
		// No public class variables
		
		// this is the constructor sets up the TiltBall sensor for use
		TiltBall (serial *ptr_serial, uint8_t p);

		void TiltBallTask (void);

		bool isTilted (void);
};

#endif // __TILTBALL_H__
