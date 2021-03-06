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

// PIR related globals
volatile uint8_t lane_states = OPEN;
volatile uint8_t ln_tmr_flg = 0x00;
volatile uint8_t changedBits = 0x00;
volatile uint8_t portd_hist = 0x00;
volatile uint8_t ln_1_tmr_cnt = 0;
volatile uint8_t ln_2_tmr_cnt = 0;

// Shared Variables for Sensor Values
int32_t surf_temp = 0;
int32_t sub_temp = 0;
int32_t ext_temp = 0;
uint32_t ext_hum = 0;
uint8_t windy = 0;
int16_t uv_ndx = 0;


int main(void)
{
	uint16_t main_runs = 0;
	
	/* create serial object */
	serial ser_dev = serial(9600, 16000000);
	
	//DBG(&ser_dev, "Creating and initializing all sensors...\r\n");
	
	// create a i2c object 
	i2c my_i2c = i2c(&ser_dev);
	
	// create a BME280 object
	BME280 my_BME280 = BME280(&my_i2c, &ser_dev, 278);
	
	// create a water temperature sensor - surface
	oneWire my_oneWire_surface_temp = oneWire(&ser_dev, 3, ID_SURFACE_TEMP);
	
	// create a water temperature sensor - surface
	oneWire my_oneWire_underwater_temp = oneWire(&ser_dev, 2, ID_UNDERWATER_TEMP);
	
	// create a tilt-ball object
	TiltBall my_TiltBall = TiltBall(&ser_dev, 0);
	
	// create a UVIndex sensor
	UVIndex my_UVIndex = UVIndex(&ser_dev, 7);
	
	// create a PIR sensor for lane 1
	PIR my_pir_ln1 = PIR(&ser_dev, LN_1_PIN);
	
	// create a PIR sensor for lane 2
	PIR my_pir_ln2 = PIR(&ser_dev, LN_2_PIN);
	
	//DBG(&ser_dev, "All Sensors created!\r\n");
	
    while (1) 
    {
		
		my_BME280.BME280Task();
		
		my_oneWire_surface_temp.oneWireTask();
		
		my_oneWire_underwater_temp.oneWireTask();
		
		my_TiltBall.TiltBallTask();
				
		my_UVIndex.UVIndexTask();
		
		my_pir_ln1.PIRTask();
		
		my_pir_ln2.PIRTask();
		
		if ((main_runs % 30) == 0)
		{
			// send update packet to WiFi board
			ser_dev.sendPkt();
		}
		
		_delay_ms(1000);
		main_runs++;
    }
}

