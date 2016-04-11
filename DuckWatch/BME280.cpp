/*****************************************************************************
 * File:		BME280.c
 * Description:	This file contains the BME280 class which controls a BME280
 *				i2c sensor. This sensor reads the temperature, humidity, and 
 *				pressure of its environment.
 * Created:		3/30/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/
#include "BME280.h"		// header for this file
#include "shares.h"		// extern globally shared variables

/*****************************************************************************
 * Method:		BME280
 * Description:	This initializer sets up a BME280 sensor to be communicated
 *				with. The method stores a copy of an i2c pointer for use and
 *				calls the init method.
 * 
 * Parameters:	ptr_i2c - the i2c object to be used for communicating with the
 *							BME280 sensor
 ****************************************************************************/
BME280::BME280 (i2c* ptr_i2c, serial *ptr_serial)
{
	p_i2c = ptr_i2c;		// store a local copy of the i2c pointer for use
	p_serial = ptr_serial;	// store a local copy of serial pointer for debug
	
	if (p_i2c->ping(BME280_ADDR))
	{
		sprintf(debug, "BME280 <0x%X> ALIVE\r\n", BME280_ADDR);
		p_serial->send(debug);
	}
	else
	{
		sprintf(debug, "BME280 <0x%X> DEAD\r\n", BME280_ADDR);
		p_serial->send(debug);
		return;
	}

	if(init())					// initializes the sensor registers
	{
		sprintf(debug, "Init failed\r\n");
		p_serial->send(debug);
		return;
	}
	
	if (read_cal())				// read out cal registers for calculations
	{
		sprintf(debug, "read_cal failed\r\n");
		p_serial->send(debug);
		return;
	}
	
	read_data();			// initial read of data to fill sensor data
	
	sprintf(debug, "BME280 constructor ok!\r\n");
	p_serial->send(debug);
}

/*****************************************************************************
 * Method:		init
 * Description:	This method initializes the BME280 sensor to operate in the
 *				desired mode. This method sets the control and configuration
 *				registers to setup the sampling rates and mode. 
 *				The pressure, temperature, and humidity will all be set to
 *				have an oversampling rate of 1. The mode will be initialized
 *				sleep. The configuration will be set to turn the IIR filter
 *				off, set the sample interval to 1s in normal mode, and disable
 *				SPI.
 * 
 * Return:		bool - the status of the operation (false = success,
 *													true = failure)
 ****************************************************************************/
bool BME280::init (void)
{
	/* 1. Set up ctrl_hum register to oversample humidity by 1
	 * 2. Set up ctrl_meas register to oversample pressure and temperature
	 *    by 1 and set mode to sleep
	 * 3. Turn off IIR Filter and set inactive duration to 1s for normal
	 *	  mode
	 */
	return (p_i2c->write(BME280_ADDR, BME280_CTRL_HUM, BME280_H_OVSM_1)
			|| p_i2c->write(BME280_ADDR, BME280_CTRL_MEAS, 
				BME280_P_OVSM_1 | BME280_T_OVSM_16 | BME280_NORMAL_MODE)
			|| p_i2c->write(BME280_ADDR, BME280_CONFIG, 
				BME280_TSB_F | BME280_FLTR_OFF | BME280_SPI_DIS));
}

/*****************************************************************************
 * Method:		set_mode
 * Description:	This method sets the mode of operation of the BME280 to the
 *				specified mode.
 * 
 * Parameters:	mode - the mode of operation that the BME280 will be set to
 * Return:		bool - the status of the operation (false = success,
 *													true = failure)	
 ****************************************************************************/
bool BME280::set_mode (opp_Mode mode)
{
	bool stat = false;
	
	uint8_t ctrl_hum = p_i2c->read(BME280_ADDR, BME280_CTRL_HUM);
	
	if (p_i2c->write(BME280_ADDR, BME280_CTRL_HUM, ctrl_hum))
    {
		// There was a problem overwriting the humidity 
		sprintf(debug, "Set mode failed overwriting humidity.\r\n");
		p_serial->send(debug);
		
		return true;
	}
	
	switch (mode)
	{
		case FORCED:
			stat = p_i2c->write(BME280_ADDR, BME280_CTRL_MEAS, 
			 p_i2c->read(BME280_ADDR, BME280_CTRL_MEAS) | BME280_FORCED_MODE);
			break;
		case NORMAL:
			stat = p_i2c->write(BME280_ADDR, BME280_CTRL_MEAS,
			 p_i2c->read(BME280_ADDR, BME280_CTRL_MEAS) | BME280_NORMAL_MODE);
			break;
		case SLEEP:
		default:
			stat = p_i2c->write(BME280_ADDR, BME280_CTRL_MEAS,
			 p_i2c->read(BME280_ADDR, BME280_CTRL_MEAS) | BME280_SLEEP_MODE);
			break;
	}
	
	return stat;	
}

/*****************************************************************************
 * Method:		convert_pressure
 * Description:	This method converts the raw pressure data into a 
 *				human-readable format. The formulas used for converting the
 *				raw values to the human-readable format are derived from the
 *				compensation formulas in the BME280 Datasheet.
 * 
 * Return:		int32_t - the human-readable pressure reading accurate to ??
 ****************************************************************************/
int32_t BME280::convert_pressure (void)
{
	// TODO
	return 0;
}

/*****************************************************************************
 * Method:		conver_temperature
 * Description:	This method converts the raw temperature data into a 
 *				human-readable format. The formulas used for converting the
 *				raw values to the human-readable format are derived from the
 *				compensation formulas in the BME280 Datasheet.
 * 
 * Return:		int32_t - the human-readable temperature reading accurate to
 *							2 decimal places in Centigrade. A value of 5382 is
 *							equal to 53.82 degrees Centigrade
 ****************************************************************************/
int32_t BME280::convert_temperature (void)
{
	int32_t x1 = 0;
	int32_t x2 = 0;
	
	temperature = 0;
	
	/* calculate x1 */
	x1 = (
		  ((raw_temp >> 3) - ((int32_t)cal.dig_T1 << 1))
		  * ((int32_t)cal.dig_T2)
		 ) >> 11;
	
	/* calculate x2 */
	x2 = (
		  ((((raw_temp >> 4) - ((int32_t)cal.dig_T1))
		   * ((raw_temp >> 4) - ((int32_t)cal.dig_T1))
		   ) >> 12)
		  * ((int32_t)cal.dig_T3)
		 ) >> 14;
	
	cal.t_fine = x1 + x2;
	
	return ((cal.t_fine * 5 + 128) >> 8);
}

/*****************************************************************************
 * Method:		convert_humidity
 * Description:	This method converts the raw humidity data into a 
 *				human-readable format. The formulas used for converting the
 *				raw values to the human-readable format are derived from the
 *				compensation formulas in the BME280 Datasheet.
 * 
 * Return:		uint32_t - the human-readable humidity reading accurate to 3
 *							decimal places in %rH where 42313 represents
 *							42313 / 1024 = 41.321 %rH
 ****************************************************************************/
uint32_t BME280::convert_humidity (void)
{
	int32_t x1 = 0;
	
	/* Utilize t_fine calculation */
	x1 = cal.t_fine - 76800;
	
	/* Calculate x1 - part 1 */
	x1 = ((((
			 (raw_hum << 14) - (((int32_t)cal.dig_H4) << 20)
			 - (((int32_t)cal.dig_H5) * x1)
			) + ((int32_t)0x4000)) >> 15)
		  * (((((
			     ((x1 * ((int32_t)cal.dig_H6)) >> 10)
			    * (((x1 * ((int32_t)cal.dig_H3)) >> 11) 
			    + ((int32_t)0x8000))
		        ) >> 10) + ((int32_t)0x200000))
			 * ((int32_t)cal.dig_H2) + 0x2000) >> 14));
	/* Calculate x1 - part 2 */
	x1 = (x1 - (((((x1 >> 15)
				 * (x1 >> 15)) >> 7)
				* ((int32_t)cal.dig_H1)) >> 4));
	
	/* Check boundaries */
	x1 = (x1 < 0) ? 0 : ((x1 > 0x19000000) ? 0x19000000 : x1);
	
	return (uint32_t)(x1 >> 12);
}

/*****************************************************************************
 * Method:		reset
 * Description:	This method resets the BME280 to its factory conditions.
 * 
 * Return:		bool - the status of the operation (false = success,
 *													true = failure)	
 ****************************************************************************/
bool BME280::reset (void)
{
	return p_i2c->write(BME280_ADDR, BME280_RESET, BME280_RESET_POR);
}

/*****************************************************************************
 * Method:		read_data
 * Description:	This method reads all of the raw data from the BME280 sensor
 *				in one contiguous i2c read operation and stores the raw data
 *				in class variables if the read operation was successful. Then
 *				it performs the appropriate calculations and conversions on the
 *				data.
 * 
 * Return:		bool - the status of the operation (false = success,
 *													true = failure)
 ****************************************************************************/
bool BME280::read_data (void)
{
	uint8_t data[NUM_DATA_REG];		// array to hold all of raw data from read

// 	put BME280 into forced mode to read data
// 		if (set_mode(FORCED))
// 		{
// 			sprintf(debug, "read_data set_mode(%d) failed\r\n", FORCED);
// 			p_serial->send(debug);
// 			return true;
// 		}
	
	// Check status, and wait for completed read
// 	while (p_i2c->read(BME280_ADDR, BME280_STATUS) & (1 << BME280_STATUS_MEAS))
// 	{
// 		;	// Wait for register to signal a complete read
// 	}
	
	// read in the data from the registers
	if (p_i2c->read(BME280_ADDR, BME280_P_RAW_MSB, data, NUM_DATA_REG))
	{
		// There was an error in the read operation, propagate this message
		sprintf(debug, "read_data read registers failed\r\n");
		p_serial->send(debug);
		return true;
	}
	
	// store data in class variables
	raw_pres = (int32_t)(
				((uint32_t)data[NDX_P_MSB]  << P_T_MSB_SHIFT)
			  | ((uint32_t)data[NDX_P_LSB]  << P_T_LSB_SHIFT)
			  | (((uint32_t)data[NDX_P_XLSB] >> P_T_XLSB_SHIFT) & 0x0F)
				);
	
	raw_temp = (int32_t)(
						((uint32_t)data[NDX_T_MSB]  << P_T_MSB_SHIFT)
						| ((uint32_t)data[NDX_T_LSB]  << P_T_LSB_SHIFT)
						| (((uint32_t)data[NDX_T_XLSB] >> P_T_XLSB_SHIFT) & 0x0F)
						);
			  
	raw_hum  = (int32_t)(
				((uint32_t)data[NDX_H_MSB] << BYTE_SHIFT)
			  | ((uint32_t)data[NDX_H_LSB])
				);
	
	// convert the raw values into human readable format
	pressure = convert_pressure();
	temperature = convert_temperature();
	humidity = convert_humidity();
	
	return false;
}

/*****************************************************************************
 * Method:		read_cal
 * Description:	This method reads in the calibration data from the BME280
 *				sensor. These values are static and are used to convert the 
 *				raw data into a human readable format.		
 * 
 * Return:		bool - the status of the operation (false = success,
 *													true = failure)
 ****************************************************************************/
bool BME280::read_cal (void)
{
	uint8_t data1[BME280_CAL_RNG_1];	// hold calibration data from range 1
	uint8_t data2[BME280_CAL_RNG_2];	// hold calibration data from range 2
	
	if (p_i2c->read(BME280_ADDR, BME280_CAL_START_1, data1, BME280_CAL_RNG_1)
	 || p_i2c->read(BME280_ADDR, BME280_CAL_START_2, data2, BME280_CAL_RNG_2))
	{
		// There was an error in the read operation, propagate this message
		sprintf(debug, "read_cal read registers failed\r\n");
		p_serial->send(debug);
		return true;
	}
	
	cal.dig_T1 = (uint16_t)(((uint16_t)data1[CAL_DIG_T1_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_T1_LSB]);
	cal.dig_T2 = (int16_t)(((uint16_t)data1[CAL_DIG_T2_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_T2_LSB]);
	cal.dig_T3 = (int16_t)(((uint16_t)data1[CAL_DIG_T3_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_T3_LSB]);
	
	cal.dig_P1 = (uint16_t)(((uint16_t)data1[CAL_DIG_P1_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_P1_LSB]);
	cal.dig_P2 = (int16_t)(((uint16_t)data1[CAL_DIG_P2_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_P2_LSB]);
	cal.dig_P3 = (int16_t)(((uint16_t)data1[CAL_DIG_P3_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_P3_LSB]);
	cal.dig_P4 = (int16_t)(((uint16_t)data1[CAL_DIG_P4_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_P4_LSB]);
	cal.dig_P5 = (int16_t)(((uint16_t)data1[CAL_DIG_P5_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_P5_LSB]);
	cal.dig_P6 = (int16_t)(((uint16_t)data1[CAL_DIG_P6_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_P6_LSB]);
	cal.dig_P7 = (int16_t)(((uint16_t)data1[CAL_DIG_P7_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_P7_LSB]);
	cal.dig_P8 = (int16_t)(((uint16_t)data1[CAL_DIG_P8_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_P8_LSB]);
	cal.dig_P9 = (int16_t)(((uint16_t)data1[CAL_DIG_P9_MSB] << BYTE_SHIFT)
							| data1[CAL_DIG_P9_LSB]);
	
	cal.dig_H1 = (uint8_t)data1[CAL_DIG_H1_LSB];
	cal.dig_H2 = (int16_t)(((uint16_t)data2[CAL_DIG_H2_MSB] << BYTE_SHIFT)
							| data2[CAL_DIG_H2_LSB]);
	cal.dig_H3 = (uint8_t)data2[CAL_DIG_H3_LSB];
	cal.dig_H4 = (int16_t)(((uint16_t)data2[CAL_DIG_H4_MSB] << NIB_SHIFT)
							| (data2[CAL_DIG_H4_LSB] & CAL_DIG_H_MSK));
	cal.dig_H5 = (int16_t)(((uint16_t)data2[CAL_DIG_H5_MSB] << NIB_SHIFT)
							| ((data2[CAL_DIG_H5_LSB] >> NIB_SHIFT) 
								& CAL_DIG_H_MSK));
	cal.dig_H6 = (int8_t)data2[CAL_DIG_H6_LSB];
	
	return false;
}

/*****************************************************************************
 * Method:		get_pressure
 * Description:	This getter method returns the latest human-readable pressure
 *				reading from the BME280 sensor		
 * 
 * Return:		int32_t - latest pressure reading
 ****************************************************************************/
int32_t BME280::get_pressure (void)
{
	return pressure;
}

/*****************************************************************************
 * Method:		get_humidity
 * Description:	This getter method returns the latest human-readable 
 *				temperature reading from the BME280 sensor			
 * 
 * Return:		int32_t - latest temperature reading
 ****************************************************************************/
int32_t BME280::get_temperature (void)
{
	return temperature;
}

/*****************************************************************************
 * Method:		get_humidity
 * Description:	This getter method returns the latest human-readable humidity
 *				reading from the BME280 sensor			
 * 
 * Return:		int32_t - latest humidity reading
 ****************************************************************************/
int32_t BME280::get_humidity (void)
{
	return humidity;
}

void BME280::BME280Task (void)
{
	sprintf(debug, "Reading BME280 data...\r\n");
	p_serial->send(debug);
	read_data();
	
	sprintf(debug, "Humidity reading: %lu (raw: 0x%08lX)\r\n",
	(uint32_t)get_humidity(), get_raw_humidity());
	p_serial->send(debug);
	
	sprintf(debug, "Temperature reading: %ld (raw: 0x%08lX)\r\n",
	(int32_t)get_temperature(),  get_raw_temperature());
	p_serial->send(debug);
	
	return;
}