#include "PIR.h"
#include "shares.h"

PIR::PIR (serial *ptr_serial, uint*_t p)
{
	pin = p;
	p_serial = ptr_serial;
	PIR_DDR &= ~(1 << pin);

	DBG(this->p_serial, "PIR Constructor OK!\r\n");
}

bool PIR::isActive (void) 
{
	return PIR_PIN & (1 << pin);
}

void PIR::PIRTask (void)
{
	static uint8_t runs = 0;

	if ((runs & 5) == 0)
	{
		DBG(this->p_serial, "\r\nPIR Task Running\r\n");

		DBG(this->p_serial, "Sensor is ");
		if (!isActive())
		{
			DBG(this->p_serial, "NOT");
		}
		DBG(this->p_serial, "active.\r\n");
	}

	runs++;
}