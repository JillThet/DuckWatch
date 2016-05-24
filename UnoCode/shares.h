/*****************************************************************************
 * File:		shares.h
 * Description:	This file contains variables to be shared between tasks and
 *				classes
 * Created:		4/4/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 
#ifndef __SHARES_H__
#define __SHARES_H__

// Debugging String
extern char dbg_str[];

// Commonly used shift amounts
#define BYTE_SHIFT	8

// Globally shared variables for PIR
extern volatile uint8_t lane_states;
extern volatile	uint8_t ln_tmr_flg;
extern volatile uint8_t changedBits;
extern volatile uint8_t portd_hist;
extern volatile uint8_t ln_1_tmr_cnt;
extern volatile uint8_t ln_2_tmr_cnt;

// Shared Variables for Sensor Values
extern int32_t surf_temp;
extern int32_t sub_temp;
extern int32_t ext_temp;
extern uint32_t ext_hum;
extern uint8_t windy;
extern int16_t uv_ndx;

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
								(ser)->send((char*)dbg_str);			\
							}

/****************************************************************************
#define DBG(ser, fmt, ... ) 
****************************************************************************/
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