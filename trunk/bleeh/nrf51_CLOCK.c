/***************************************************************************************************
** -------------------------------------------------------------------------------------------------
** Filename: nrf51_CLOCK.c																																																	
** -------------------------------------------------------------------------------------------------	
** Description: clock initialization functions for Nordic Semiconductor nRF51822 																
** -------------------------------------------------------------------------------------------------
** Author: Luca Buccolini																																						
** -------------------------------------------------------------------------------------------------
** Date: Oct 16, 2014 																																				
** -------------------------------------------------------------------------------------------------
***************************************************************************************************/

#include <nrf51.h>
#include <nrf51_bitfields.h>
#include "nrf51_CLOCK.h"

void LFCLOCK_init(void)
{
	//Select the preferred low frequency clock source as Cryatal Quartz
	NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
	//Clear LFCLKSTARTED event
	NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
	//Trigger LFCLKSTART task	-> start Low Frequency Clock
	NRF_CLOCK->TASKS_LFCLKSTART = 1;
	// Wait for the low frequency clock to start
	while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) 
	{
	}
}
