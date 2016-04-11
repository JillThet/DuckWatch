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
#include "shares.h"		// extern for global shared variables

/* initialize shared variables */
char debug[SERIAL_MAX_SEND];

int main(void)
{
	/* create serial object */
	serial ser_dev = serial(9600, 16000000);
	
	/* create a i2c object */
	i2c my_i2c = i2c(&ser_dev);

	/* create a BME280 object */
	BME280 my_BME280 = BME280(&my_i2c, &ser_dev);
	
	/* create a tilt-ball object */
	TiltBall my_TiltBall = TiltBall(&ser_dev, 2);
	
	/* create a water temperature sensor - surface */
	oneWire my_oneWire_surface_temp = oneWire(&ser_dev, 0, ID_SURFACE_TEMP);
	
	/* create a UVIndex sensor */
	UVIndex my_UVIndex = UVIndex(&ser_dev, 7);
	
    /* Replace with your application code */
    while (1) 
    {
		//my_TiltBall.TiltBallTask();
		
		sprintf(debug, "\r\n\r\n");
		ser_dev.send(debug);
		
		my_oneWire_surface_temp.oneWireTask();
		
		sprintf(debug, "\r\n\r\n");
		ser_dev.send(debug);
		
		my_UVIndex.UVIndexTask();
		
		//my_BME280.BME280Task();
		
		_delay_ms(1000);
    }
}

