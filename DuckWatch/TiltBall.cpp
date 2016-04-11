/*****************************************************************************
 * File:		TiltBall.cpp
 * Description:	This file contains the TiltBall class, which allows the 
 *				microcontroller to communicate with the tilt ball sensor.
 * Created:		4/4/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/
#include "TiltBall.h"
#include "shares.h"

/*****************************************************************************
 * Method:		TiltBall
 * Description:	This constructor sets up the tiltBall sensor on the ATmega328P
 ****************************************************************************/
TiltBall::TiltBall (serial *ptr_serial, uint8_t p)
{
	pin = p;					// store local copy of tilt ball pin
	p_serial = ptr_serial;		// store local copy of serial for debug
	TILT_DDR &= ~(1 << pin);	// Make tilt ball an input
	
	sprintf(debug, "TiltBall Constructor OK!\r\n");
	p_serial->send(debug);
}

bool TiltBall::isTilted ()
{
	
	sprintf(debug, "isTilted: Tilt_PIN 0x%02X, 1 << pin 0x%02X\r\n", 
		TILT_PIN, 1 << pin);
	p_serial->send(debug);
	
	return TILT_PIN & (1 << pin);
}

void TiltBall::TiltBallTask ()
{
	static uint8_t runs = 0;
	
	if ((runs % 5) == 0) 
	{
		sprintf(debug, "TiltBall Task\r\n");
		p_serial->send(debug);
		if (isTilted())
		{
			sprintf(debug, "Tilted!\r\n");
			p_serial->send(debug);
		}
	}
	
	runs++;
}