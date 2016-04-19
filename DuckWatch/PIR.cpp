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
	//OUTPUT(PIR_DDR, pin);
	
	initPIR();

	// Make PIR an Input
	INPUT(PIR_DDR, pin);
	
	DBG(this->p_serial, "PIR Calibrating...");
	_delay_ms(30000);
	DBG(this->p_serial, "PIR Constructor OK!\r\n");
}

void PIR::initPIR (void)
{
	PCICR |= 1 << PCIE2;
	PCMSK2 |= 1 << pin; // PCINTx
	PCIFR = 1 << PCIF2;
	sei();
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
	/*
	DBG(this->p_serial, "PIR_PIN Reg: 0x%02X\r\n", PIR_PIN);
	DBG(this->p_serial, "Pin location: %d\r\n", pin);
	*/
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
	if ((runs % 5) == 0)
	{
		DBG(this->p_serial, "\r\nPIR Task Running\r\n");

		DBG(this->p_serial, "Sensor is ");

		// check for sensor active
		if (!isActive())
		{
			DBG(this->p_serial, "NOT ");
		}
		DBG(this->p_serial, "active.\r\n");
		
		/*
		if (isActive())
		{
			DBG(this->p_serial, "1");
		}
		else
		{
			DBG(this->p_serial, "0");
		}
		*/
	}

	runs++;
}

ISR (PCINT2_vect)
{
	uint8_t changedBits;
	cli();

	changedBits = PIR_PIN ^ portd_hist;
	portd_hist = PIR_PIN;

	if (changedBits & (1 << LN_1_PIN))
	{
		// Lane 1 changed state

		// clear timer
	}
	else if (changedBits & (1 << LN_2_PIN))
	{
		// Lane 2 changed state

		// clear timer
	}

	sei();
}
