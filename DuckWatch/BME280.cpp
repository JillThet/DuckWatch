/*****************************************************************************
 * File:		BME280.c
 * Description:	This file contains the BME280 class which controls a BME280
 *				i2c sensor. This sensor reads the temperature, humidity, and 
 *				pressure of its environment.
 * Created:		3/30/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/
#include "BME280.h"		// header for this file
#include "shares.h"		// extern globally shared variables and macros

/*****************************************************************************
 * Method:		BME280
 * Description:	This initializer sets up a BME280 sensor to be communicated
 *				with. The method stores a copy of an i2c pointer for use and
 *				calls the init method.
 * 
 * Parameters:	ptr_i2c		- pointer to an i2c object to be used for 
 *								communicating with the BME280 sensor
 *				ptr_serial	- pointer to a serial object to be used for
 *								debugging
 ****************************************************************************/
BME280::BME280 (i2c* ptr_i2c, serial *ptr_serial, int32_t temperature_cal)
{
	p_i2c = ptr_i2c;			// store local copy of i2c pointer for use
	p_serial = ptr_serial;		// store local copy of serial pointer for debug
	temp_cal = temperature_cal;	// store local copy of temp cal
	
	if (p_i2c->ping(BME280_ADDR))
	{
		DBG(this->p_serial, "BME280 <0x%X> ALIVE\r\n", BME280_ADDR);
	}
	else
	{
		DBG(this->p_serial, "BME280 <0x%X> DEAD\r\n", BME280_ADDR);
		return;
	}

	// initialize the sensor registers
	if(init())
	{
		DBG(this->p_serial, "BME280::BME280 FAILED BME280::init\r\n");
		return;
	}
	
	// read out cal registers for calculations
	if (read_cal())
	{
		DBG(this->p_serial, "BME280::BME280 FAILED BME280::read_cal\r\n");
		return;
	}
	
	// initial read of data to fill sensor data
	read_data();
	
	DBG(this->p_serial, "BME280 Constructor OK!\r\n");
}

/*****************************************************************************
 * Method:		init
 * Description:	This method initializes the BME280 sensor to operate in the
 *				desired mode. This method sets the control and configuration
 *				registers to setup the sampling rates and mode. 
 *				The pressure, temperature, and humidity will all be set to
 *				have an oversampling rate of 1. The mode will be initialized
 *				normal mode. The configuration will be set to turn the IIR 
 *				filter off, set the sample interval to 1s in normal mode, and 
 *				disable SPI.
 * 
 * Return:		bool - the status of the operation (false = success,
 *													true = failure)
 ****************************************************************************/
bool BME280::init (void)
{
	/* 1. Set up ctrl_hum register to oversample humidity by 1
	 * 2. Set up ctrl_meas register to oversample pressure and temperature
	 *    by 1 and set mode to normal
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
bool BME280::set_mode (op_Mode mode)
{
	bool stat = false;
	
	uint8_t ctrl_hum = p_i2c->read(BME280_ADDR, BME280_CTRL_HUM);
	
	if (p_i2c->write(BME280_ADDR, BME280_CTRL_HUM, ctrl_hum))
    {
		// There was a problem overwriting the humidity 
		DBG(this->p_serial,
			"BME280::set_mode - FAILED overwriting humidity.\r\n");
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
 *				compensation formulas in the BME280 Datasheet 
 *				(https://cdn-shop.adafruit.com/product-files/2652/2652.pdf).
 * 
 * Return:		int32_t - the human-readable pressure reading accurate to
 *							2 decimal places in Pascals. A value of 5382 is
 *							equal to 53.82 Pa
 ****************************************************************************/
int32_t BME280::convert_pressure (void)
{
	int64_t var1, var2, p;
	
	raw_pres >>= 4;
	
	var1 =	((int64_t)cal.t_fine) - 128000;
	
	var2 =	var1 * var1 * (int64_t)cal.dig_P6;
	var2 =	var2 + ((var1*(int64_t)cal.dig_P5) << 17);
	var2 =	var2 + (((int64_t)cal.dig_P4) << 35);
	
	var1 =	((var1 * var1 * (int64_t)cal.dig_P3) >> 8) +
			((var1 * (int64_t)cal.dig_P2)<<12);
	var1 =	(((((int64_t)1) << 47) + var1)) * ((int64_t)cal.dig_P1) >> 33;
	 
	if (var1 == 0) {
		return 0;  // avoid exception caused by division by zero 
	}
	
	p = 1048576 - raw_pres;
	p = (((p << 31) - var2) * 3125) / var1;
	
	var1 = (((int64_t)cal.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t)cal.dig_P8) * p) >> 19; 
	
	p = ((p + var1 + var2) >> 8) + (((int64_t)cal.dig_P7) << 4);
	 
	return p >> 8;
}

/*****************************************************************************
 * Method:		conver_temperature
 * Description:	This method converts the raw temperature data into a 
 *				human-readable format. The formulas used for converting the
 *				raw values to the human-readable format are derived from the
 *				compensation formulas in the BME280 Datasheet
 *				(https://cdn-shop.adafruit.com/product-files/2652/2652.pdf).
 * 
 * Return:		int32_t - the human-readable temperature reading accurate to
 *							2 decimal places in Centigrade. A value of 5382 is
 *							equal to 53.82 degrees Centigrade
 ****************************************************************************/
int32_t BME280::convert_temperature (void)
{
	int32_t var1 = 0;
	int32_t var2 = 0;
	
	temperature = 0;
	
	/* calculate x1 */
	var1 =	((((raw_temp>>3) - ((int32_t)cal.dig_T1 <<1))) *
		 ((int32_t)cal.dig_T2)) >> 11;
	
	/* calculate x2 */
	var2 =	(((((raw_temp>>4) - ((int32_t)cal.dig_T1)) *
			   ((raw_temp>>4) - ((int32_t)cal.dig_T1))) >> 12) *
			((int32_t)cal.dig_T3)) >> 14;
	
	cal.t_fine = var1 + var2;
	
	return (((cal.t_fine * 5 + 128) >> 8) - temp_cal);
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
	int32_t var1;
	
	/* Utilize t_fine calculation */
	var1 = (cal.t_fine - ((int32_t)76800));
	
	/* Calculate var1 - part 1	 */
	var1 =	(((((raw_hum << 14) - (((int32_t)cal.dig_H4) << 20) -
				(((int32_t)cal.dig_H5) * var1)) + ((int32_t)16384)) >> 15) *
			 (((((((var1 * ((int32_t)cal.dig_H6)) >> 10) *
				  (((var1 * ((int32_t)cal.dig_H3)) >> 11)
				   + ((int32_t)32768))) >> 10) +	
				((int32_t)2097152)) * ((int32_t)cal.dig_H2) + 8192) >> 14));
	
	/* Calculate var1 - part 2 */
	var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7)
			* ((int32_t)cal.dig_H1)) >> 4));
	
	
	/* Check boundaries */	
	var1 = (var1 < 0 ? 0 : var1);
	var1 = (var1 > 419430400 ? 419430400 : var1);
	
	return (uint32_t)((var1 >> 12));
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
	
	// read in the data from the registers
	if (p_i2c->read(BME280_ADDR, BME280_P_RAW_MSB, data, NUM_DATA_REG))
	{
		// There was an error in the read operation, propagate this message
		DBG(this->p_serial, "BME280::read_data FAILED to read data registers\r\n");
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
		DBG(this->p_serial, "BME280::read_cal FAILED to read cal reigsters\r\n");
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

void BME280::BME280Task (void)
{
	static uint8_t runs = 0;
	int32_t temp_f;
	
	if ((runs % 5) == 0)
	{
		DBG(this->p_serial, "\r\nBME280 Task Running\r\n");
		
		read_data();
		
		temp_f = TEMP_C_TO_F(temperature);	
		
		DBG(this->p_serial, "Temperature: %ld.%02ldC or %ld.%02ldF\r\n",
				(temperature / 100), (temperature % 100),
				(temp_f / 100), (temp_f % 100));
		
		DBG(this->p_serial, "Humidity: %lu.%lu%%\r\n",
			(humidity / 1024), (humidity % 1024));
			
		DBG(this->p_serial, "Pressure: %luPa\r\n",
			pressure);
	}
	
	runs++;
	return;
}