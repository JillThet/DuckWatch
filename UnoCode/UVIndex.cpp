/*****************************************************************************
 * File:		UVIndex.cpp
 * Description:	This file contains the UVIndex class. A UVIndex sensor 
 *				captures and returns the UV index of its environment
 *
 * Created:		4/10/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 

#include "UVIndex.h"
#include "shares.h"

/*****************************************************************************
 * Method:		
 * Description:	
 *
 * Parameters:	ptr_serial	- pointer to serial object for debugging
 *				pin			- the pin to be associated with the enable pin
 ****************************************************************************/
UVIndex::UVIndex (serial *ptr_serial, uint8_t pin)
{
	p_serial = ptr_serial;
	enable_pin = pin;
	
	init();	
}

/*****************************************************************************
 * Method:		init
 * Description:	This method initializes the internal Analog to Digital 
 *				Converter (ADC) to use a prescaler of 128 with no auto trigger
 *				and a gain of 1X
 ****************************************************************************/
void UVIndex::init (void)
{
	ADCSRA = 0x87;	//Turn On ADC and set prescaler (CLK/128)
	ADCSRB = 0x00;	//Set gain & turn off auto trigger
	ADMUX = 0x00;   //Set ADC channel ADC0 with 1X gain
}

/*****************************************************************************
 * Method:		enable
 * Description:	This method sets the enable pin high to enable the UVIndex 
 *				sensor
 ****************************************************************************/
void UVIndex::enable (void)
{
	// set enable pin high
	UV_EN_PORT |= (1 << enable_pin);
	OUTPUT(UV_EN_DDR, enable_pin);
}

/*****************************************************************************
 * Method:		disable
 * Description:	This method sets the enable pin to low to disable the UVIndex
 *				sensor
 ****************************************************************************/
void UVIndex::disable (void)
{
	// set enable pin low
	UV_EN_PORT &= ~(1 << enable_pin);
	OUTPUT(UV_EN_DDR, enable_pin);
}

/*****************************************************************************
 * Method:		read
 * Description:	This method reads a single value from the UVIndex Sensor. The
 *				sensor is first enabled, then a delay is included for the 
 *				stabilization period. Once a reading has been collected, the
 *				sensor is disabled.
 *
 * Return:		int16_t - the digital value of the analog read of the UV Index
 *					where a value of 302 is equivalent to 3.02
 ****************************************************************************/
int16_t UVIndex::read (void)
{
	int16_t value;
	
	enable();
	
	// stabilization period
	_delay_ms(1);
	
	// start conversion
	ADCSRA = 0xC7;
	
	// wait for conversion to finish
	_delay_us(260);
	
	value = ADC & 0x3FF;
	
	disable();
	
	return value;
}

/*****************************************************************************
 * Method:		UVIndexTask
 * Description:	This method is run once every loop of the main control and 
 *				triggers the sensor to take a reading and store the value so
 *				it can be used by other objects.
 ****************************************************************************/
void UVIndex::UVIndexTask (void)
{
	static uint8_t runs  = 0;
	
	// update global value
	uv_ndx = read();
	
	/*
	if ((runs % 5) == 0)
	{
		DBG(this->p_serial, "\r\nUVIndex Task Running\r\n")
		DBG(this->p_serial, "UV Index reading: %d\r\n", read());
	}
	*/
	runs++;
}