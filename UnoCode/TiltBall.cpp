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
 *
 * Parameters:	ptr_Serial	- pointer to serial object for debugging
 *				p			- the pin on which the tiltBall is connected
 ****************************************************************************/
TiltBall::TiltBall (serial *ptr_serial, uint8_t p)
{
	pin = p;					// store local copy of tilt ball pin
	p_serial = ptr_serial;		// store local copy of serial for debug
	INPUT(TILT_DDR, pin);		// Make tilt ball an input
	
	hist = 0;					// initialize the history to 0
	
	//DBG(this->p_serial, "TiltBall Constructor OK!\r\n");
}

/*****************************************************************************
 * Method:		isTilted
 * Description:	This method checks the status of the TiltBall Sensor
 *
 * Return:		bool - whether or not the sensor is in a tilted state
 *						(true = tilted, false = not tilted)
 ****************************************************************************/
bool TiltBall::isTilted (void)
{
	return TILT_PIN & (1 << pin);
}

uint8_t TiltBall::isWindy (void)
{
	uint8_t wind = 0;
	
	for (uint8_t ndx = 0; ndx < 32; ndx++)
	{
		wind +=  (hist >> ndx) & 0x01;
	}
	
	return wind > 15;
}

/*****************************************************************************
 * Method:		TiltBallTask
 * Description:	This method executes the sensor read operation for the 
 *				TiltBall sensor.
 ****************************************************************************/
void TiltBall::TiltBallTask (void)
{	
	//DBG(this->p_serial, "\r\nTiltBall Task Running\r\n");
		
	hist = (hist << 1) | (isTilted() ? 1 : 0);
		
	//DBG(this->p_serial, "\r\nWind history: %lu\r\n", hist);
	//DBG(this->p_serial, "\r\nisWindy? %d", isWindy());
	
	windy = isWindy();
}