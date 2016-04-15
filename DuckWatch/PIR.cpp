/*****************************************************************************
 * File:		PIR.cpp
 * Description:	This file contains the PIR class, which allows the 
 *				microcontroller to communicate with PIR sensors.
 * Created:		4/13/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 
#include "PIR.h"
#include "shares.h"

/*****************************************************************************
 * Method:		PIR
 * Description:	This constructor sets up the PIR sensor.
 * 
 * Parameters:	ptr_serial	- a reference to the serial debug object
 *				p 			- the pin on the ATmega328P the sensor outputs to
 ****************************************************************************/
PIR::PIR (serial *ptr_serial, uint8_t p)
{
	// initialize the pin and serial debug object
	pin = p;
	p_serial = ptr_serial;

	// sets the pin on the PIR_DDR to an output
	PIR_DDR &= ~(1 << pin);
	DBG(this->p_serial, "Callibrating...\r\n");
	_delay_ms(30000);
	DBG(this->p_serial, "PIR Constructor OK!\r\n");
}

/*****************************************************************************
 * Method:		isActive
 * Description:	This method checks if the sensor is outputting 
 *				an active signal.
 *
 * Return:		bool - state of if an active signal was received
 ****************************************************************************/
bool PIR::isActive (void) 
{
	return PIR_PIN & (1 << pin);
}

/*****************************************************************************
 * Method:		PIRTask
 * Description:	This method runs the PIR task.
 ****************************************************************************/
void PIR::PIRTask (void)
{
	static uint8_t runs = 0;

	// Runs once every 5 run cycles
	if (true)//(runs % 5) == 0)
	{
		/*
		DBG(this->p_serial, "\r\nPIR Task Running\r\n");

		DBG(this->p_serial, "Sensor is ");

		// check for sensor active
		if (!isActive())
		{
			DBG(this->p_serial, "NOT ");
		}
		DBG(this->p_serial, "active.\r\n");
		*/
		
		if (isActive())
		{
			DBG(this->p_serial, "1");
		}
		else
		{
			DBG(this->p_serial, "0");
		}
	}

	runs++;
}
