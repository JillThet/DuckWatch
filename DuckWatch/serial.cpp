/*****************************************************************************
 * File:		serial.cpp
 * Description:	This file contains a serial class which allows the 
 *				microcontroller to communicate via a USART serial connection.
 * Created:		4/3/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/
#include "serial.h"		// header for this file
#include "shares.h"		// extern globally shared variables

/*****************************************************************************
 * Method:		serial
 * Description:	This constructor method sets up and initializes a serial
 *				connection.
 * 
 * Parameters:	baud - the baud rate to establish
 *				clk  - the clock speed of the device
 ****************************************************************************/
serial::serial (uint16_t baud, uint32_t clk)
{
	baud_rate = baud;		// store a local copy
	clk_speed = clk;		// store a local copy
	
	init();
}

/*****************************************************************************
 * Method:		init
 * Description:	This method initializes the registers on the ATmega328P for
 *				establishing a USART connection.
 * 
 * Return:		bool - the status of the operation (true  = success,
 *													false = failure)
 ****************************************************************************/
void serial::init (void)
{
	/* calculate ubrr for desired baud rate */
	uint32_t ubrr = ((clk_speed / 16UL) / baud_rate) - 1;
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;
	
	/* Enable receiver and transmitter */
	UCSR0B = ((1 << RXEN0) | (1 << TXEN0));
	
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = ((1 << USBS0) | (3 << UCSZ00));
	UCSR0A &= ~(1 << U2X0);
}

/*****************************************************************************
 * Method:		send
 * Description:	This method sends a byte of data over the TX line. The send
 *				operation may fail if it takes too long.
 * 
 * Parameters:	data - the byte of data to transmit
 * Return:		bool - the status of the operation (true  = success,
 *													false = failure)
 ****************************************************************************/
bool serial::send (uint8_t data)
{
	uint16_t cnt = 0;
	
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1 << UDRE0)))// && cnt < SERIAL_TIMEOUT)
	{
		cnt++;
	}
	/*
	if (cnt >= SERIAL_TIMEOUT)
	{
		// send timed out
		return false;
	}
	*/
	
	/* Put data into buffer, sends the data */
	UDR0 = data;
	
	return true;
}

/*****************************************************************************
 * Method:		send
 * Description:	This method sends a string of data over the TX line. A maximum
 *				of 255 bytes will be sent at a time. The total number of bytes
 *				transmitted will be returned.
 * 
 * Parameters:	str - the string of data to be sent, maximum 255 bytes
 * Return:		uint8_t - the total number of characters successfully sent
 ****************************************************************************/
uint8_t serial::send (char *str)
{
	uint8_t sent = 0;
	
	while (*str && sent < SERIAL_MAX_SEND)
	{
		if (send(*str++))
		{
			sent++;
		}
		else
		{
			break;
		}
	}
	
	return sent;
}

/*****************************************************************************
 * Method:		receive
 * Description:	This method receives a byte of data from the RX line, if there
 *				is data available, and returns it.
 * 
 * Return:		uint8_t - the data received from the RX line, or 0xFF if an
 *				error occurs (i.e. data not ready)
 ****************************************************************************/
uint8_t serial::receive (void)
{
	uint16_t cnt = 0;
	
	/* Wait for data to be received */
	while ( !(UCSR0A & (1 << RXC0)) && cnt < SERIAL_TIMEOUT)
	{
		cnt++;
	}
	
	if (cnt >= SERIAL_TIMEOUT)
	{
		// receive timed out
		return 0xFF;
	}
	
	/* Get and return received data from buffer */
	return (uint8_t)UDR0;
}

/*****************************************************************************
 * Method:		data_avaialable
 * Description:	This method checks if there is data available on the RX line.
 * 
 * Return:		bool - if there is data available  (true  = available, 
 *													false = not available) 
 ****************************************************************************/
bool serial::data_available (void)
{
	return (UCSR0A & (1 << RXC0));
}