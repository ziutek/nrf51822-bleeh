/* ---------------------------------------------------------------------------
** This file is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Filename: nrf_delay_l85.c
**
** Description: source file for delay functions
**
** Author: Luca Buccolini
**
** -------------------------------------------------------------------------*/

#include "nrf_delay_l85.h"

void nrf_delay(uint32_t t_delay) 
	{
		uint32_t i = t_delay*0.001*__SYSTEM_CLOCK;
		while(--i);
		}
