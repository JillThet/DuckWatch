/*****************************************************************************
 * File:		i2c.h
 * Description:	This file contains the class definition for the i2c class.
 * Created:		3/30/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/
#ifndef __I2C_H__
#define __I2C_H__

#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "serial.h"

#define TW_TIMEOUT	1024
#define READ_BIT	0x01

/* Status Masks for i2c status register - TWSR */
#define STAT_MSK		0xF8
#define STAT_START		0x08
#define STAT_RESTART	0x10
#define STAT_WRITE_ACK	0x18
#define STAT_WRITE_NACK	0x20
#define STAT_TRANS_ACK	0x28
#define STAT_TRANS_NACK	0x30
#define STAT_BAD		0x38
#define STAT_READ_ACK	0x40
#define STAT_READ_NACK	0x48
#define STAT_REC_ACK	0x50
#define STAT_REC_NACK	0x58

/*****************************************************************************
 * Class:		i2c
 * Description:	The i2c class enables the microcontroller to communicate with
 *				other i2c devices.
 ****************************************************************************/
class i2c
{
	protected:
		// No protected methods or class variables
		
	private:
		// debug serial serial connection
		serial *p_serial;
		
		// this method sends a start condition on the i2c bus
		bool start (void);
		
		// this method sends a restart condition on the i2c bus
		bool restart (void);
		
		// this method sends a stop condition on the i2c bus
		bool stop (void);
		
		// this method writes one byte to the i2c device
		bool write_byte (uint8_t data);
		
		// this method reads one byte to the i2c device
		uint8_t read_byte (bool ack);
	
	public:
		// No public class variables
		
		// this constructor sets up the i2c for use
		i2c (serial *ptr_serial);
		
		// this method checks if a sensor is alive at the given address
		bool ping (uint8_t addr);
		
		// this method writes one byte to the i2c sensor
		bool write (uint8_t addr, uint8_t reg, uint8_t data);
		
		// this method writes multiple bytes to the i2c sensor
		bool write (uint8_t addr, uint8_t reg, uint8_t* p_buff, uint8_t count);
		
		// this method reads one byte from the i2c sensor
		uint8_t read (uint8_t addr, uint8_t reg);
		
		// this method reads multiple bytes from the i2c sensor
		bool read (uint8_t addr, uint8_t reg, uint8_t* p_buff, uint8_t count);
};

#endif // __I2C_H__