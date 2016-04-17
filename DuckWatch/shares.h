/*****************************************************************************
 * File:		shares.h
 * Description:	This file contains variables to be shared between tasks and
 *				classes
 * Created:		4/4/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 
#ifndef __SHARES_H__
#define __SHARES_H__

extern char dbg_str[];

// Commonly used shift amounts
#define BYTE_SHIFT	8

/*****************************************************************************
 * MACRO:		INPUT
 * Description:	Sets the given pin to an input on the specified port
 * Parameters:	ddr	- the DDR register associated with the desired port
 *				p	- the pin on the port to be set to an input
 ****************************************************************************/
#define INPUT(ddr, p) ((ddr) &= ~(1 << (p)))

/*****************************************************************************
 * MACRO:		OUTPUT
 * Description:	Sets the given pin to an output on the specified port
 * Parameters:	ddr	- the DDR register associated with the desired port
 *				p	- the pin on the port to be set to an output
 ****************************************************************************/
#define OUTPUT(ddr, p) ((ddr) |= (1 << (p)))

/*****************************************************************************
 * MACRO:		DBG
 * Description:	Prints via serial, the debugging message
 * Parameters:	ser - the serial device to print to
 *				fmt - the format string
 *				... - any additional params for the format string
 ****************************************************************************/ 
#define DBG(ser, fmt, ... )	{											\
								sprintf(dbg_str, fmt, ##__VA_ARGS__);	\
								ser->send((char*)dbg_str);				\
							}

/*****************************************************************************
 * MACRO:		TEMP_C_TO_F
 * Description:	Converts a temperature in C to its equivalent in F.
 *				The C temp will be in a format such that 2204 is 22.04C
 *				The F temp will be in the same format -> 7170 is 71.70F
 * Parameters:	temp	- input temperature in C
 * Return:		temp_f	- the converted temperature in F
 ****************************************************************************/ 
#define TEMP_C_TO_F(temp)	((((temp) * 9) + 16000) / 5)

#endif /* __SHARES_H__ */