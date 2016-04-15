/*****************************************************************************
 * File:		TiltBall.h
 * Description:	This file contains the class definition for the TiltBall class.
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
 *				access a tilt ball sensor.
 ****************************************************************************/
class TiltBall
{
	protected:
		// No protected methods or class variables
		
	private:
		// pointer to serial object for debugging
		serial *p_serial;

		// pin on port D which connects the TiltBall to the microcontroller
		uint8_t pin;
	
	public:
		// No public class variables
		
		// this is the constructor sets up the TiltBall sensor for use
		TiltBall (serial *ptr_serial, uint8_t p);

		// this method checks if the sensor is tilted
		bool isTilted (void);

		// this method runs once every time the main loop runs
		void TiltBallTask (void);
};

#endif // __TILTBALL_H__
