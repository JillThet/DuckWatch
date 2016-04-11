/*****************************************************************************
 * File:		shares.h
 * Description:	This file contains variables to be shared between tasks and
 *				classes
 * Created:		4/4/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 
#ifndef __SHARES_H__
#define __SHARES_H__

extern char debug[];

// Macro for making a pin an input
#define INPUT(ddr, pin) ((ddr) &= ~(1 << (pin)))

// Macro for making a pin an output
#define OUTPUT(ddr, pin) ((ddr) |= (1 << (pin)))

#define BYTE_SHIFT	8

/* TODOS
 * Read_byte - trans ack always the status
 * Calibration - Actually implement the read, dummy
 * print out raw values to see if change actually occurring
 */

#endif /* __SHARES_H__ */