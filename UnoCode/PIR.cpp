/*****************************************************************************
 * File:		PIR.cpp
 * Description:	This file contains the PIR class, which allows the 
 *				microcontroller to communicate with PIR sensors.
 * Created:		4/13/2016
 * Authors:		Jill Thetford, Daniel Griffith
 ****************************************************************************/ 
#include "PIR.h"
#include "shares.h"

/*****************************************************************************
 * Method:		PIR
 * Description:	This constructor sets up the PIR sensor.
 * 
 * Parameters:	ptr_serial	- a reference to the serial debug object
 *				p 			- the pin on the ATmega328P the sensor outputs to
 ****************************************************************************/
PIR::PIR (serial *ptr_serial, uint8_t p)
{
	// initialize the pin and serial debug object
	pin = p;
	p_serial = ptr_serial;

	// Make PIR an Input
	INPUT(PIR_DDR, pin);
	
	switch (pin)
	{
		case LN_1_PIN:
			lane = LN_1;
			init_timer_ln_1();
			break;
		case LN_2_PIN:
			lane = LN_2;
			init_timer_ln_2();
			break;
		default:
			lane = 0;
			break;
	}
	
	//DBG(this->p_serial, "PIR for lane %d Calibrating...", lane);
	_delay_ms(30000);

	initPIR();
	//DBG(this->p_serial, "PIR Constructor OK!\r\n");
}

void PIR::init_timer_ln_1 (void)
{
	// set up timer0 compA for lane 1
	// Compare output mode, non-PWM mode, clear on compare match
	TCCR0A |= ((1 << COM0A1) & ~(1 << COM0A0));
	
	// Force output compare A
	TCCR0B |= (1 << FOC0A);
	
	// set prescaler to 1024
	TCCR0B |= (((1 << CS02) & ~(1 << CS01)) | (1 << CS00));
	
	// Clear previous timer overflow
	TIMSK0 |= (1 << OCF0A);
	
	// enable compare match
	TIMSK0 |= (1 << OCIE0A);
	
	// set timer output compare match
	OCR0A = 0xFF;
}

void PIR::init_timer_ln_2 (void)
{
	// set up timer0 compB for lane 2
	// Compare output mode, non-PWM mode, clear on compare match
	TCCR0A |= ((1 << COM0B1) & !(1 << COM0B0));
	
	// Force output compare B
	TCCR0B |= (1 << FOC0B);
	
	// set prescaler to 1024
	TCCR0B |= (((1 << CS02) & ~(1 << CS01)) | (1 << CS00));
	
	// Clear previous timer overflow
	TIMSK0 |= (1 << OCF0B);
	
	// enable compare match
	TIMSK0 |= (1 << OCIE0B);
	
	// set timer output compare match
	OCR0A = 0xFF;
}

void PIR::initPIR (void)
{
	// Enable Pin Change Interrupts on Port D
	PCICR |= 1 << PCIE2;
	
	// Enables Specific Pin as Pin Change Interrupt
	PCMSK2 |= 1 << pin; // PCINTx
	
	// Clear any previous interrupts
	PCIFR |= 1 << PCIF2;
	
	sei();
}

/*****************************************************************************
 * Method:		isActive
 * Description:	This method checks if the sensor is outputting 
 *				an active signal.
 *
 * Return:		bool - state of if an active signal was received
 ****************************************************************************/
bool PIR::isActive (void) 
{
	return lane_states & lane;
}

/*****************************************************************************
 * Method:		PIRTask
 * Description:	This method runs the PIR task.
 ****************************************************************************/
void PIR::PIRTask (void)
{
	static uint8_t runs = 0;
	
	// TODO - What does this task actually do now?
	
	/*
	// Runs once every 5 run cycles
	if ((runs % 1) == 0)
	{
		DBG(this->p_serial, "\r\nPIR Task Running\r\n");

		DBG(this->p_serial, "Sensor (%d) is ", lane);

		// check for sensor active
		if (!isActive())
		{
			DBG(this->p_serial, "NOT ");
		}
		DBG(this->p_serial, "active.\r\n");		
	}
	*/
	runs++;
}

ISR (PCINT2_vect)
{
	cli();

	changedBits = PIR_PIN ^ portd_hist;
	portd_hist = PIR_PIN;

	if (changedBits & (1 << LN_1_PIN))
	{
		// Lane 1 changed state
		// reset timer counter
		ln_1_tmr_cnt = 0;
		// set timer flag for lane 1
		ln_tmr_flg |= LN_1;
		
		// clear timer
		TIMSK0 |= (1 << OCF0A);
	}
	
	if (changedBits & (1 << LN_2_PIN))
	{
		// Lane 2 changed state
		// reset timer counter
		ln_2_tmr_cnt = 0;
		// set timer flag for lane 2
		ln_tmr_flg |= LN_2;
		
		// clear timer
		TIMSK0 |= (1 << OCF0B);
	}

	sei();
}

ISR (TIMER0_COMPA_vect)
{	
	cli();
	// check if supposed to monitor for state hold
	if (ln_tmr_flg & LN_1)
	{
		if (lane_states & LN_1)
		{
			// lane 1 was full
			if (ln_1_tmr_cnt++ >= MIN_LN_EMPTY)
			{
				// lane 1 now considered empty
				lane_states &= ~LN_1;
				// clear the flag so stop monitoring
				ln_tmr_flg &= ~LN_1;
			}
		}
		else
		{
			// lane 1 was empty
			if (ln_1_tmr_cnt++ >= MIN_LN_FULL)
			{
				// lane 1 now considered full
				lane_states |= LN_1;
				// clear the flag so stop monitoring
				ln_tmr_flg &= ~LN_1;
			}
		}
	}
	sei();
}

ISR (TIMER0_COMPB_vect)
{
	cli();
	// check if supposed to monitor for state hold
	if (ln_tmr_flg & LN_2)
	{
		if (lane_states & LN_2)
		{
			// lane 2 was full
			if (ln_2_tmr_cnt++ >= MIN_LN_EMPTY)
			{
				// lane 2 now considered empty
				lane_states &= ~LN_2;
				// clear the flag so stop monitoring
				ln_tmr_flg &= ~LN_2;
			}
		}
		else
		{
			// lane 2 was empty
			if (ln_2_tmr_cnt++ >= MIN_LN_FULL)
			{
				// lane 2 now considered full
				lane_states |= LN_2;
				// clear the flag so stop monitoring
				ln_tmr_flg &= ~LN_2;
			}
		}
	}
	sei();
}