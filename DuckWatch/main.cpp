/*****************************************************************************
 * File:		main.cpp
 * Description:	TODO
 *
 * Created:		3/30/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 

#include <avr/io.h>
#include "i2c.h"
#include "BME280.h"
#include "serial.h"
#include "TiltBall.h"
#include "oneWire.h"
#include "UVIndex.h"
#include "PIR.h"
#include "shares.h"		// extern for global shared variables

/* initialize shared variables */
char dbg_str[SERIAL_MAX_SEND];

int main(void)
{
	/* create serial object */
	serial ser_dev = serial(9600, 16000000);
	
	DBG(&ser_dev, "Creating and initializing all sensors...\r\n");
	
	// create a i2c object 
	i2c my_i2c = i2c(&ser_dev);
	
	// create a BME280 object
	BME280 my_BME280 = BME280(&my_i2c, &ser_dev, 278);
	
	// create a water temperature sensor - surface
	oneWire my_oneWire_surface_temp = oneWire(&ser_dev, 0, ID_SURFACE_TEMP);
	
	// create a tilt-ball object
	TiltBall my_TiltBall = TiltBall(&ser_dev, 0);
	
	// create a UVIndex sensor
	UVIndex my_UVIndex = UVIndex(&ser_dev, 7);
	
	// create a PIR sensor 
	PIR my_pir_ln1 = PIR(&ser_dev, LN_1_PIN);
	
	DBG(&ser_dev, "All Sensors created!\r\n");
	
    while (1) 
    {
		
		my_BME280.BME280Task();
		
		my_oneWire_surface_temp.oneWireTask();
		
		my_TiltBall.TiltBallTask();
				
		my_UVIndex.UVIndexTask();
		
		my_pir_ln1.PIRTask();
		
		_delay_ms(1000);
    }
}

