/*
 * UVIndex.cpp
 *
 * Created: 4/10/2016 2:06:39 PM
 *  Author: Jill
 */ 

#include "UVIndex.h"
#include "shares.h"

UVIndex::UVIndex (serial *ptr_serial, uint8_t pin)
{
	p_serial = ptr_serial;
	enable_pin = pin;
	
	init();	
}

void UVIndex::init (void)
{
	ADCSRA = 0x87;	//Turn On ADC and set prescaler (CLK/128)
	ADCSRB = 0x00;	//Set gain & turn off autotrigger
	ADMUX = 0x00;   //Set ADC channel ADC0 with 1X gain
}

void UVIndex::enable (void)
{
	// set enable pin high
	UV_EN_PORT |= (1 << enable_pin);
	OUTPUT(UV_EN_DDR, enable_pin);
}

void UVIndex::disable (void)
{
	// set enable pin low
	UV_EN_PORT &= ~(1 << enable_pin);
	OUTPUT(UV_EN_DDR, enable_pin);
}

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