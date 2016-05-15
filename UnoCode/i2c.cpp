/*****************************************************************************
 * File:		i2c.cpp
 * Description:	This file contains the i2c class, which allows the 
 *				microcontroller to communicate with other i2c devices via the
 *				i2c protocol.
 * Created:		3/30/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 
#include "i2c.h"
#include "shares.h"

/*****************************************************************************
 * Method:		i2c
 * Description:	This constructor sets up the i2c protocol on the ATmega328P
 *				to operate at a frequency of 200kHz
 * 
 * Parameters:	ptr_serial - a reference to the serial debug object
 ****************************************************************************/
i2c::i2c (serial *ptr_serial)
{
	p_serial = ptr_serial;	// store a local copy of serial pointer for debug
	
	// set the bit rate to 200kHz, with no prescaler
	TWSR = 0x00;
	TWBR = 32;
	
	// enable i2c protocol
	TWCR = (1 << TWEN);
	
	DBG(this->p_serial, "i2c constructor OK!\r\n");
}

/*****************************************************************************
 * Method:		ping
 * Description:	This method pings an address and checks if there is a response
 * 
 * Parameters:	addr - the address to be checked
 * Return:		bool - state of if an ACK was received
 ****************************************************************************/
bool i2c::ping (uint8_t addr)
{
	bool sign_of_life;
	start();
	sign_of_life = write_byte(addr);
	stop();
	
	return sign_of_life;
}

/*****************************************************************************
 * Method:		start
 * Description:	This method puts the start signal on the i2c bus
 * 
 * Return:		bool - status of operation (true = error, false = success)
 ****************************************************************************/
bool i2c::start (void)
{
	// put a start condition on the line
	TWCR = ((1 << TWSTA) | (1 << TWEN) | (1 << TWINT));
	
	// wait for TWINT to go low, which signals start condition completed
	for (uint16_t twcnt = 0; !(TWCR & (1 << TWINT)); twcnt++)
	{
		// check for timeout condition
		if (twcnt > TW_TIMEOUT)
		{
			// timeout occurred, error
			DBG(this->p_serial, "i2c::start FAILED - timed out\r\n");
			return true;
		}
	}
	
	// check status register
	if ((TWSR & STAT_MSK) != STAT_START)
	{
		// Status register is not a start signal, error
		DBG(this->p_serial, "i2c::start FAILED - status (0x%2X) bad\r\n", TWSR);
		return true;
	}
	
	return false;
}

/*****************************************************************************
 * Method:		restart
 * Description: This method sends a repeated start signal on the i2c bus
 * 
 * Return:		bool - status of operation (true = error, false = success)
 ****************************************************************************/
bool i2c::restart (void)
{
	// put another start condition on the line
	TWCR = ((1 << TWSTA) | (1 << TWEN) | (1 << TWINT));
	
	// wait for TWINT to go low, which signals start condition completed
	for (uint16_t twcnt = 0; !(TWCR & (1 << TWINT)); twcnt++)
	{
		// check for timeout condition
		if (twcnt > TW_TIMEOUT)
		{
			// timeout occurred, error
			DBG(this->p_serial, "i2c::restart FAILED - timed out\r\n");
			return true;
		}
	}
	
	// check status register
	if ((TWSR & STAT_MSK) != STAT_RESTART)
	{
		// Status register is not a repeated start signal, error
		DBG(this->p_serial, "i2c::restart FAILED - status (0x%2X) bad\r\n", TWSR);
		return true;
	}
	
	return false;	
}

/*****************************************************************************
 * Method:		stop
 * Description: This method sends a stop condition over the i2c bus
 * 
 * Return:		bool - status of operation (true = error, false = success)
 ****************************************************************************/
bool i2c::stop (void)
{
	TWCR = ((1 << TWINT) | (1 << TWSTO) | (1 << TWEN));
	return false;
}

/*****************************************************************************
 * Method:		write_byte
 * Description:	This method writes one byte of data to the i2c bus and checks
 *				the status code generated by writing the data
 * 
 * Parameters:	data - the byte of data being written to the bus
 * Return:		bool - status of operation (true = ack, false = nack)
 ****************************************************************************/
bool i2c::write_byte (uint8_t data)
{	
	TWDR = data;
	TWCR = ((1 << TWINT) | (1 << TWEN));
	

	// wait for TWINT to go low, which signals write condition sent
	for (uint16_t twcnt = 0; !(TWCR & (1 << TWINT)); twcnt++)
	{
		// check for timeout condition
		if (twcnt > TW_TIMEOUT)
		{
			// timeout occurred, error
			DBG(this->p_serial, "i2c::write_byte FAILED - timed out\r\n");
			return true;
		}
	}
	
	// check the ACK status of the write
	switch (TWSR & STAT_MSK)
	{
		case (STAT_WRITE_ACK):
		case (STAT_TRANS_ACK):
		case (STAT_READ_ACK):
			// An ACK was received
			return true;
		
		default:
			// a Nack of Some kind received
			return false;	
	}
	
	return false;
}

/*****************************************************************************
 * Method:		read_byte
 * Description:	This method receives a byte from the i2c bus and returns it.
 * 
 * Parameters:	ack - true if an ack is expected after the byte,
 *					  false if a nack is expected after they byte
 * Return:		uint8_t - the byte of data received, or 0xFF if error occurs
 ****************************************************************************/
uint8_t i2c::read_byte (bool ack)
{
	//uint8_t expected_response;
	
	if (ack) // an ack is expected, so we are expecting more data after this
	{
		TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWEA));
		//expected_response = STAT_TRANS_ACK; // changed from STAT_REC_ACK
	}
	else
	{
		TWCR = ((1 << TWINT) | (1 << TWEN));
		//expected_response = STAT_REC_NACK;
	}
	
	// wait for TWINT to go low, which signals read complete
	for (uint16_t twcnt = 0; !(TWCR & (1 << TWINT)); twcnt++)
	{
		// check for timeout condition
		if (twcnt > TW_TIMEOUT)
		{
			// timeout occurred, error
			DBG(this->p_serial, "i2c::read_byte FAILED - timed out\r\n");
			return 0xFF;
		}
	}
	// everything ok, return the data, which is stored in TWDR
	return TWDR;
}

/*****************************************************************************
 * Method:		write
 * Description:	This method writes a byte of data to a specified register on 
 *				an i2c device with a given address
 * 
 * Parameters:	addr - the address of the i2c device being written to
 *				reg	 - the register to be written to
 *				data - the data being written
 * Return:		bool - status of operation (true = error, false = success)
 ****************************************************************************/
bool i2c::write (uint8_t addr, uint8_t reg, uint8_t data)
{
	start();
	if (!write_byte(addr) || !write_byte(reg) || !write_byte(data))
	{
		// an error occurred, one of these had a NACK
		DBG(this->p_serial,
			"NACK on write <addr:0x%2X, reg:0x%2X, data:0x%2X>\r\n",
			addr, reg, data);
		return true;
	}
	stop();
	
	// all good, success!
	return false;
}

/*****************************************************************************
 * Method:		write
 * Description:	This method writes multiple bytes to the specified device
 * 
 * Parameters:	addr   - the address of the device being written to
 *				reg    - the starting register of the write
 *				p_buff - the buffer containing the data to be written
 *				count  - the number of bytes to write
 * Return:		bool   - status of operation (true = error, false = success)
 ****************************************************************************/
bool i2c::write (uint8_t addr, uint8_t reg, uint8_t* p_buff, uint8_t count)
{
	start();
	if (!write_byte(addr) || !write_byte(reg))
	{
		// an error occurred, one of these had a NACK
		DBG(this->p_serial, "NACK on write <addr:0x%2X, reg:0x%2X>\r\n",
			addr, reg);
		return true;
	}
	// write the data one byte at a time
	for (uint8_t ndx = 0; ndx < count; ndx++)
	{
		if (!write_byte(*p_buff++))
		{
			// a NACK happened too early
			DBG(this->p_serial, "NACK on write data %u\r\n", ndx);
			return true;
		}
	}
	stop();
	
	// all good, success
	return false;
}

/*****************************************************************************
 * Method:		read
 * Description:	This method reads a byte of data from the specified register
 *				on the specified device
 * 
 * Parameters:	addr    - the address of the i2c device being read from
 *				reg     - the register being read
 * Return:		uint8_t - the byte that was read from the device, or 0xFF if
 *						  an error occurred
 ****************************************************************************/
uint8_t i2c::read (uint8_t addr, uint8_t reg)
{
	uint8_t data;
	
	start();
	// Write the register address that you are trying to read
	if (!write_byte(addr) || !write_byte(reg))
	{
		// an error occurred, one of these had a NACK
		DBG(this->p_serial, "Write NACK on read <addr:0x%2X, reg:0x%2X>\r\n",
			addr, reg);
		return 0xFF;
	}
	stop();
	restart();
	// read the info
	if (!write_byte(addr | READ_BIT))
	{
		// an error occurred, a NACK was received
		DBG(this->p_serial, "NACK on read <addr:0x%2X>\r\n",
			addr | READ_BIT);
		return 0xFF;
	}
	// read byte with a NACK expected
	data = read_byte(false);
	stop();
	
	return data;
}

/*****************************************************************************
 * Method:		read
 * Description:	This method reads multiply bytes of data from an i2c device
 *				starting at the specified register
 * 
 * Parameters:	addr   - the address of the i2c device being read from
 *				reg    - the starting register
 *				p_buff - the buffer to hold the data being read
 *				count  - the total number of bytes to read
 * Return:		bool   - status of operation (true = error, false = success)
 ****************************************************************************/
bool i2c::read (uint8_t addr, uint8_t reg, uint8_t* p_buff, uint8_t count)
{
		start();
		if (!write_byte(addr) || !write_byte(reg))
		{
			// an error occurred, one of these had a NACK
			DBG(this->p_serial, "Write NACK on read <addr:0x%2X, reg:0x%2X>\r\n",
				addr, reg);
			return true;
		}
		stop();
		restart();	// change?
		// read the info
		if (!write_byte(addr | READ_BIT))
		{
			// an error occurred, a NACK was received
			DBG(this->p_serial, "NACK on read <addr:0x%2X>\r\n",
				addr | READ_BIT);
			return 0xFF;
		}
		
		// Read the data
		for (uint8_t ndx = 1; ndx < count; ndx++)
		{
			// still have more to read, so expect an ACK
			*p_buff++ = read_byte(true);
		}
		// Read last bit of data, so expect a NACK
		*p_buff++ = read_byte(false);
		stop();
		
		return false;
}