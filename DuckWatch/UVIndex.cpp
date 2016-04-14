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
 * Parameters:
 * Return:
 ****************************************************************************/
UVIndex::UVIndex (serial *ptr_serial, uint8_t pin)
{
	p_serial = ptr_serial;
	enable_pin = pin;
	
	init();	
}

/*****************************************************************************
 * Method:		
 * Description:	
 *
 * Parameters:
 * Return:
 ****************************************************************************/
void UVIndex::init (void)
{
	ADCSRA = 0x87;	//Turn On ADC and set prescaler (CLK/128)
	ADCSRB = 0x00;	//Set gain & turn off autotrigger
	ADMUX = 0x00;   //Set ADC channel ADC0 with 1X gain
}

/*****************************************************************************
 * Method:		
 * Description:	
 *
 * Parameters:
 * Return:
 ****************************************************************************/
void UVIndex::enable (void)
{
	// set enable pin high
	UV_EN_PORT |= (1 << enable_pin);
	OUTPUT(UV_EN_DDR, enable_pin);
}

/*****************************************************************************
 * Method:		
 * Description:	
 *
 * Parameters:
 * Return:
 ****************************************************************************/
void UVIndex::disable (void)
{
	// set enable pin low
	UV_EN_PORT &= ~(1 << enable_pin);
	OUTPUT(UV_EN_DDR, enable_pin);
}

/*****************************************************************************
 * Method:		
 * Description:	
 *
 * Parameters:
 * Return:
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
	
	if ((runs % 4) == 0)
	{
		sprintf(debug, "UV Index reading: %d\r\n", read());
		p_serial->send(debug);
	}
	
	runs++;
}