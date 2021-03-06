/*****************************************************************************
 * File:		oneWire.cpp
 * Description:	This file contains the oneWire class, which allows the 
 *				microcontroller to communicate with other oneWire devices 
 * 				via the Dallas One Wire protocol.
 * Created:		4/8/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 

#include "oneWire.h"
#include "shares.h"

/*****************************************************************************
 * Method:		oneWire
 * Description:	This constructor sets up the oneWire protocol on the 
 *				ATmega328P.
 * 
 * Parameters:	ptr_serial 	- a reference to the serial debug object
 *				pin 		- the pin on the ATmega328P the sensor outputs to.
 *				id 			- the unique id of the sensor
 ****************************************************************************/
oneWire::oneWire(serial *ptr_serial, uint8_t pin, uint8_t id)
{
	p_serial = ptr_serial;	// store local copy for debug
	data_line = pin;		// store local copy
	dev_id = id;			// store local copy
	
	// make the data an input
	INPUT(DATA_DDR, data_line);

	//DBG(this->p_serial, "oneWire Constructor ok! Device id: %d\r\n", dev_id);
}

/*****************************************************************************
 * Method:		reset
 * Description:	This method resets the oneWire device.
 *
 * Return:		bool - if the device is ready.
 ****************************************************************************/
bool oneWire::reset (void)
{
	bool presence_pulse;
	uint8_t retries = 125;
	
	// make pin an input
	INPUT(DATA_DDR, data_line);
	
	// wait for wire to go high
	do 
	{
		if (--retries == 0)
		{
			return false;
		}
		_delay_us(2);
	} while (!(DATA_PIN & (1 << data_line)));
	
	// set data_line low
	DATA_PORT &= ~(1 << data_line);
	
	// set data_line as output
	OUTPUT(DATA_DDR, data_line);
	
	_delay_us(480);
		
	// set data_line as input
	INPUT(DATA_DDR, data_line);
	
	_delay_us(70);
	
	presence_pulse = (bool)!((DATA_PIN & (1 << data_line)));
	
	_delay_us(410);
	
	return presence_pulse;
}

/*****************************************************************************
 * Method:		write_byte
 * Description:	This method writes one byte to the oneWire device.
 *
 * Parameters:	data - the data to write to the device
 ****************************************************************************/
void oneWire::write_byte (uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (data & 0x01)
		{
			write_1();
		}
		else
		{
			write_0();
		}
		data >>= 1;
	}
}

/*****************************************************************************
 * Method:		write_1
 * Description:	This method writes a logial high to the oneWire device.
 ****************************************************************************/
void oneWire::write_1 (void)
{
	// set data_line low
	DATA_PORT &= ~(1 << data_line);
	
	// make data_line an output
	OUTPUT(DATA_DDR, data_line);
	
	_delay_us(10);
	
	// make data_line an input
	INPUT(DATA_DDR, data_line);
	
	_delay_us(55);
}

/*****************************************************************************
 * Method:		write_0
 * Description:	This method writes a logial low to the oneWire device.
 ****************************************************************************/
void oneWire::write_0 (void)
{
	// set data_line low
	DATA_PORT &= ~(1 << data_line);
	
	// make data_line an output
	OUTPUT(DATA_DDR, data_line);
	
	_delay_us(65);
	
	// make data_line an input
	INPUT(DATA_DDR, data_line);
	
	_delay_us(5);
}

/*****************************************************************************
 * Method:		read_byte
 * Description:	This method reads one byte from the oneWire device.
 ****************************************************************************/
uint8_t oneWire::read_byte (void)
{
	uint8_t data_in;
	uint8_t data_read = 0;
	
	for (uint8_t bit_msk = 0x01; bit_msk; bit_msk <<= 1)
	{
		// make data_line low
		DATA_PORT &= ~(1 << data_line);
		// make data_line output
		OUTPUT(DATA_DDR, data_line);
		
		_delay_us(3);
		// make data_line input
		INPUT(DATA_DDR, data_line);
		_delay_us(10);
		
		data_in = DATA_PIN & (1 << data_line);
		_delay_us(53);
		
		if (data_in)
		{
			data_read |= bit_msk;
		}
	}
	
	return data_read;
}

/*****************************************************************************
 * Method:		convert_temp
 * Description:	This method converts the raw temperature data to human 
 * 				readable values
 ****************************************************************************/
int16_t oneWire::convert_temp(int16_t raw)
{
	return (raw * 6) + (raw / 4);
}

/*****************************************************************************
 * Method:		oneWireTask
 * Description:	This method runs the oneWire task
 ****************************************************************************/
void oneWire::oneWireTask (void)
{
	static uint8_t runs = 0;
	
	uint8_t low_byte, high_byte;
	int16_t temp;
	int32_t temp_f;
	
	//if ((runs % 5) == 0)
	//{
		//DBG(this->p_serial, "\r\noneWire Task Running on device: %d\r\n", dev_id);
		
		// perform temperature conversion
		reset();
		write_byte(0xCC);	// skip ROM
		write_byte(0x44);	// single temp conversion
		
		// read in scratch pad
		reset();
		write_byte(0xCC);	// skip ROM
		write_byte(0xBE);	// read scratchpad
		
		// read temperature
		low_byte = read_byte();
		high_byte = read_byte();
		temp = (high_byte << BYTE_SHIFT) | low_byte;
		temp = convert_temp(temp);
		temp_f = TEMP_C_TO_F((int32_t)temp);
		
		// update global value
		if (dev_id == ID_SURFACE_TEMP)
		{
			surf_temp = temp_f;
		}
		else if (dev_id == ID_UNDERWATER_TEMP)
		{
			sub_temp = temp_f;
		}
		
		//DBG(this->p_serial, "Temp sensor %d: %d.%02dC or %ld.%02ldF\r\n",
		//	dev_id, 
		//	(temp / 100), (temp % 100),
		//	(temp_f / 100), (temp_f % 100));
	//}
	runs++;
}