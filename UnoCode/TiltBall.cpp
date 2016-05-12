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
	
	DBG(this->p_serial, "TiltBall Constructor OK!\r\n");
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

/*****************************************************************************
 * Method:		TiltBallTask
 * Description:	This method executes the sensor read operation for the 
 *				TiltBall sensor.
 ****************************************************************************/
void TiltBall::TiltBallTask (void)
{
	static uint8_t runs = 0;
	
	if ((runs % 5) == 0) 
	{
		DBG(this->p_serial, "\r\nTiltBall Task Running\r\n");
		
		DBG(this->p_serial, "Sensor is ");
		if (!isTilted())
		{
			DBG(this->p_serial, "NOT ");
		}
		DBG(this->p_serial, "tilted.\r\n");
	}
	
	runs++;
}