/*****************************************************************************
 * File:		UVIndex.h
 * Description:	This header file contains the class definition for a UV Index
 *				sensor as well as the expected port information for the enable
 *				pin and output of the sensor.
 *
 * Created:		4/10/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 

#ifndef __UVINDEX_H__
#define __UVINDEX_H__

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "serial.h"

#define ACTIVE_MODE		1
#define STANDBY_MODE	0

#define UVINDEX_DDR		DDRA
#define UVINDEX_PORT	PORTA
#define UVINDEX_PIN		PINA

#define UV_EN_DDR		DDRD
#define UV_EN_PORT		PORTD
#define UV_EN_PIN		PIND

/*****************************************************************************
 * Class:		UVIndex
 * Description:	The UVIndex class controls an analog sensor that detects the
 *				UV index of its environment.
 ****************************************************************************/
class UVIndex
{
	protected:
		// no protected methods or class variables
	
	private:
		serial *p_serial;	// pointer to serial object for debugging
		
		uint8_t enable_pin;	// enable pin that is used to turn on/off the sensor
		
		// This method initializes the internal ADC and sets the sensor
		// to standby mode
		void init (void);
		
		// This method enables the sensor to start taking readings by putting
		// the sensor into active mode
		void enable (void);
		
		// This method disables the sensor from taking readings by putting the
		// sensor into standby mode
		void disable (void);
		
		// This method collects a data reading from the UVIndex
		int16_t read (void);
		
	public:
		// no public class variables
		
		// This constructor creates a UVIndex sensor object
		UVIndex(serial *ptr_serial, uint8_t pin);
		
		// This method collects a sample from the UVIndex sensor when called
		// from main
		void UVIndexTask();
};



#endif /* __UVINDEX_H__ */