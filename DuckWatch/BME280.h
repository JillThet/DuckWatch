/*****************************************************************************
 * File:		BME280.h
 * Description:	This file contains all of the register descriptions and needed
 *				constants for using i2c with the BME280, an i2c Environmental
 *				sensor which measures temperature, humidity, and barometric
 *				pressure.
 * Created:		3/30/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 
#ifndef __BME280_H__
#define __BME280_H__

#include "i2c.h"
#include "serial.h"

/*****************************************************************************
 * Device Address
 ****************************************************************************/
#define BME280_ADDR 			0xEE

/*****************************************************************************
 * Registers
 ****************************************************************************/
/* ID register contains chip id number, should be 0x60. 
 * Can be read as soon as device finished power-on-reset (POR).
 */
#define BME280_ID 				0xD0

/* Reset register controls device reset.
 *
 * RESET_POR 	- device reset using power-on-reset procedure when 
 * 			   		this is written to register.
 */
#define BME280_RESET			0xE0
#define BME280_RESET_POR		0xB6

/* Humidity control register sets the humidity data acquisition
 * options of the device. Changes to register only become effective
 * after a write to CTRL_MEAS register.
 *
 * H_OVSM_S 	- Humidity oversampling skipped (output set to 0x8000)
 * H_OVSM_1 	- Humidity oversampling x1
 * H_OVSM_2 	- Humidity oversampling x2
 * H_OVSM_4 	- Humidity oversampling x4
 * H_OVSM_8 	- Humidity oversampling x8
 * H_OVSM_16 	- Humidity oversampling x16
 */
#define BME280_CTRL_HUM			0xF2
#define BME280_H_OVSM_S			0x00
#define	BME280_H_OVSM_1			0x01
#define BME280_H_OVSM_2			0x02
#define	BME280_H_OVSM_4			0x03
#define BME280_H_OVSM_8			0x04
#define BME280_H_OVSM_16		0x05

/* Status register indicates the status of the device.
 * 
 * STATUS_MEAS 		- Data ready status bit, 
 * 						0 = data ready, 1 = conversion running
 * STATUS_UPDATE 	- Image update status bit,
 * 						0 = copy complete, 1 = data copying
 */
#define BME280_STATUS			0xF3
#define BME280_STATUS_MEAS		3
#define BME280_STATUS_UPDATE	0

/* Measurement control register controls pressure and
 * temperature data acquisition. Must be written after
 * changing CTRL_HUM for changes to become effective.
 * 
 * P_OVSM_S 	- Pressure oversampling skipped 
 * 					(output set to 0x80000)
 * P_OVSM_1 	- Pressure oversampling x1
 * P_OVSM_2 	- Pressure oversampling x2
 * P_OVSM_4 	- Pressure oversampling x4
 * P_OVSM_8 	- Pressure oversampling x8
 * P_OVSM_16 	- Pressure oversampling x16
 * T_OVSM_S 	- Temperature oversampling skipped 
 * 					(output set to 0x80000)
 * T_OVSM_1 	- Temperature oversampling x1
 * T_OVSM_2 	- Temperature oversampling x2
 * T_OVSM_4 	- Temperature oversampling x4
 * T_OVSM_8 	- Temperature oversampling x8
 * T_OVSM_16 	- Temperature oversampling x16

 * SLEEP_MODE 	- No measurements performed and pwr 
 *					consumption is at min
 * FORCED_MODE 	- Single measurement performed, returns to 
 * 					SLEEP_MODE when finished
 * NORMAL_MODE 	- Cycles between and active measurement
 *					period and an inactive standby period.
 */
#define BME280_CTRL_MEAS		0xF4
#define BME280_P_OVSM_S			0b00000000
#define BME280_P_OVSM_1			0b00100000
#define BME280_P_OVSM_2			0b01000000
#define BME280_P_OVSM_4			0b01100000
#define	BME280_P_OVSM_8			0b10000000
#define BME280_P_OVSM_16		0b10100000

#define	BME280_T_OVSM_S			0b00000000
#define	BME280_T_OVSM_1			0b00000100
#define BME280_T_OVSM_2			0b00001000
#define	BME280_T_OVSM_4			0b00001100
#define BME280_T_OVSM_8			0b00010000
#define BME280_T_OVSM_16		0b00010100

#define BME280_SLEEP_MODE		0b00000000
#define	BME280_FORCED_MODE		0b00000001
#define BME280_NORMAL_MODE		0b00000011

/* Config register controls the rate, filter, and interface 
 * options of the device. NOTE: writes to register in normal 
 * mode may be ignored. In sleep mode writes are not ignored.
 * 
 * TSB_F	- Inactive duration in normal mode = 0.5
 * TSB_62F	- Inactive duration in normal mode = 62.5
 * TSB_125	- Inactive duration in normal mode = 125
 * TSB_250	- Inactive duration in normal mode = 250
 * TSB_500	- Inactive duration in normal mode = 500
 * TSB_1000	- Inactive duration in normal mode = 1000
 * TSB_10	- Inactive duration in normal mode = 10
 * TSB_20	- Inactive duration in normal mode = 20
 * FLTR_OFF	- Filter off
 * FLTR_2	- Filter coefficient = 2
 * FLTR_4	- Filter coefficient = 4
 * FLTR_8	- Filter coefficient = 8
 * FLTR_16	- Filter coefficient = 16 
 * SPI_EN	- Enables 3-wire SPI
 * SPI_DIS	- Disables 3-wire SPI
 */
#define	BME280_CONFIG			0xF5
#define BME280_TSB_F			0b00000000
#define BME280_TSB_62F			0b00100000
#define BME280_TSB_125			0b01000000
#define	BME280_TSB_250			0b01100000
#define BME280_TSB_500			0b10000000
#define BME280_TSB_1000			0b10100000
#define BME280_TSB_10 			0b11000000
#define BME280_TSB_20 			0b11100000

#define BME280_FLTR_OFF			0b00000000
#define BME280_FLTR_2			0b00000100
#define BME280_FLTR_4			0b00001000
#define BME280_FLTR_8			0b00001100
#define BME280_FLTR_16			0b00010000

#define	BME280_SPI_EN			0b00000001
#define BME280_SPI_DIS			0b00000000

/* Pressure register one contains the MSB of the 
 * raw pressure data.
 * Pressure[19:12]
 */
#define BME280_P_RAW_MSB		0xF7

/* Pressure register two contains the LSB of the
 * raw pressure data.
 * Pressure[11:4]
 */
#define BME280_P_RAW_LSB		0xF8

/* Pressure register three contains the xLSB of
 * the raw pressure data (bit 7, 6, 5, 4).
 * Pressure[3:0]
 */
#define	BME280_P_RAW_XLSB		0xF9		

/* Temperature register one contains the MSB of 
 * the raw temperature data.
 * Temperature[19:12]
 */
#define BME280_T_RAW_MSB		0xFA

/* Temperature register two contains the LSB of
 * the raw temperature data.
 * Temperature[11:4]
 */
#define BME280_T_RAW_LSB		0xFB

/* Temperature register three contains the xLSB 
 * of the raw temperature data (bit 7, 6, 5, 4).
 * Temperature[3:0]
 */
#define	BME280_T_RAW_XLSB		0xFC	

/* Humidity register one contains the MSB of the
 * raw humidity data.
 * Humidity[15:8]
 */
#define BME280_H_RAW_MSB		0xFD

/* Humidity register two contains the LSB of the
 * raw humidity data.
 * Humidity[7:0]
 */
#define BME280_H_RAW_LSB		0xFE

#define BME280_CAL_START_1		0x88 /* 0 */
#define	BME280_CAL_RNG_1		26

#define CAL_DIG_T1_LSB			0
#define CAL_DIG_T1_MSB			1
#define CAL_DIG_T2_LSB			2
#define CAL_DIG_T2_MSB			3
#define CAL_DIG_T3_LSB			4
#define CAL_DIG_T3_MSB			5
#define CAL_DIG_P1_LSB			6
#define CAL_DIG_P1_MSB			7
#define CAL_DIG_P2_LSB			8
#define CAL_DIG_P2_MSB			9
#define CAL_DIG_P3_LSB			10
#define CAL_DIG_P3_MSB			11
#define CAL_DIG_P4_LSB			12
#define CAL_DIG_P4_MSB			13
#define CAL_DIG_P5_LSB			14
#define CAL_DIG_P5_MSB			15
#define CAL_DIG_P6_LSB			16
#define CAL_DIG_P6_MSB			17
#define CAL_DIG_P7_LSB			18
#define CAL_DIG_P7_MSB			19
#define CAL_DIG_P8_LSB			20
#define CAL_DIG_P8_MSB			21
#define CAL_DIG_P9_LSB			22
#define CAL_DIG_P9_MSB			23
#define CAL_DIG_H1_LSB			25

#define BME280_CAL_START_2		0xE1 /* 26 */
#define BME280_CAL_RNG_2		7

#define CAL_DIG_H2_LSB			0
#define CAL_DIG_H2_MSB			1
#define CAL_DIG_H3_LSB			2
#define CAL_DIG_H4_MSB			3
#define CAL_DIG_H4_LSB			4 /* [3:0] */
#define CAL_DIG_H5_MSB			5
#define CAL_DIG_H5_LSB			4 /* [7:4] */
#define CAL_DIG_H6_LSB			6

#define CAL_DIG_H_MSK			0x0F

/*****************************************************************************
 * I2C Timing
 ****************************************************************************/
/* NOTE: SF = standard and fast modes, HS = high speed mode
 * NOTE: All times are in ns.
 */
#define	BME280_SDI_SETUP_SF		160
#define BME280_SDI_SETUP_HS		30

#define BME280_SDI_HOLD_SF		90 /* or 80 if bus capacitance is <= 100pF */
#define BME280_SDI_HOLD_HS		24 /* or 24 if bus capacitance is <= 100pF */

// HS mode only (Not sure why)
#define BME280_SCK_LOW_PULSE_HI	160
#define BME280_SCK_LOW_PULSE_LO	210			

// Reading Raw Data Index Positions
#define NUM_DATA_REG	8
#define NDX_P_MSB		0
#define NDX_P_LSB		1
#define NDX_P_XLSB		2
#define NDX_T_MSB		3
#define NDX_T_LSB		4
#define NDX_T_XLSB		5
#define NDX_H_MSB		6
#define NDX_H_LSB		7

// Shifting Values
#define P_T_MSB_SHIFT	12
#define P_T_LSB_SHIFT	4
#define P_T_XLSB_SHIFT	4
#define BYTE_SHIFT		8
#define NIB_SHIFT		4

// Operation Modes
enum op_Mode {SLEEP, FORCED, NORMAL};

// Calibration data structure
struct BME280_Cal_Data {
	uint16_t dig_T1;		//SEE TABLE ON PG 22 FOR REGISTER BREAKDOWN
	int16_t  dig_T2;
	int16_t  dig_T3;

	uint16_t dig_P1;
	int16_t  dig_P2;
	int16_t  dig_P3;
	int16_t  dig_P4;
	int16_t  dig_P5;
	int16_t  dig_P6;
	int16_t  dig_P7;
	int16_t  dig_P8;
	int16_t  dig_P9;

	uint8_t  dig_H1;
	int16_t  dig_H2;
	uint8_t  dig_H3;
	int16_t  dig_H4;
	int16_t  dig_H5;
	int8_t 	 dig_H6;

	int32_t	 t_fine;
};

/*****************************************************************************
 * Class:		BME280
 * Description:	The BME280 class controls a BME280 Pressure Temperature and
 *				Humidity sensor that communicates via the i2c protocol.
 ****************************************************************************/
class BME280
{
	protected:
		// No protected class variables or methods
		
	private:
		i2c* p_i2c;				// pointer to i2c object for communication
		serial* p_serial;		// pointer to serial object for debugging
		
		int32_t temp_cal;		// temperature calibration constant

		int32_t raw_pres;		// raw pressure data from BME280
		int32_t raw_temp;		// raw temperature data from BME280
		int16_t raw_hum;		// raw humidity data from BME280
		
		int32_t pressure;		// scaled and shifted pressure reading
		int32_t temperature;	// scaled and shifted temperature reading
		uint32_t humidity;		// scaled and shifted humidity reading

		BME280_Cal_Data cal; 	// calibration data from BME280

		// this method gets the calibration data of the BME280
		bool get_calibration(void);
		
		// this method sets the mode of operation of the BME280
		bool set_mode (op_Mode mode);
		
		// these methods convert the raw data into human readable formats
		int32_t convert_pressure (void);
		int32_t convert_temperature (void);
		uint32_t convert_humidity (void);

		// this method reads in the static calibration data
		bool read_cal (void);
		
	public:
		// No public class variables

		// this constructor sets up a BME280 sensor
		BME280 (i2c* ptr_i2c, serial* ptr_serial, int32_t temperature_cal);

		// this method initializes the BME280
		bool init (void);
		
		// this method resets the sensor
		bool reset (void);
		
		// this method reads the raw data from the sensor
		bool read_data (void);
		
		// Getter Methods
		int32_t get_pressure (void)			{ return pressure;		};
		int32_t get_temperature (void)		{ return temperature;	};
		int32_t get_humidity (void)			{ return humidity;		};
		int32_t get_temp_cal (void)			{ return temp_cal;		};

		int32_t get_raw_pressure(void)		{ return raw_pres;		};
		int32_t get_raw_temperature(void)	{ return raw_temp;		};
		uint32_t get_raw_humidity(void)		{ return raw_hum;		};
			
		// setter methods
		void set_temp_cal (int32_t new_temp_cal);
			
		// task method
		void BME280Task (void);
};

#endif /* __BME280_H__ */