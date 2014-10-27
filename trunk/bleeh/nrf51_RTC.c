/***************************************************************************************************
** -------------------------------------------------------------------------------------------------
** Filename: nrf51_RTC.c																																																	
** -------------------------------------------------------------------------------------------------	
** Description: Real Time Clock initialization functions for Nordic Semiconductor nRF51822 																
** -------------------------------------------------------------------------------------------------
** Author: Luca Buccolini																																						
** -------------------------------------------------------------------------------------------------
** Date: Oct 17, 2014 																																				
** -------------------------------------------------------------------------------------------------
***************************************************************************************************/

#include "nrf51_RTC.h"

#include <nrf51.h>
#include <nrf51_bitfields.h>


void RTC0_init(void)
{
//+++++++++++++++++++++++++++++++++++++++++++++++...EVENTS...+++++++++++++++++++++++++++++++++++++
	// enable/disable EVENTS routed to PPI
/*all events are disabled by default (nRF51 ref. manual v2.1, chapter 18.2.1, EVTEN register). Uncomment (and modify) this is you want to enable any event.
	NRF_RTC0->EVTEN = (RTC_EVTEN_TICK_Disabled     << RTC_EVTEN_TICK_Pos    ) |
										(RTC_EVTEN_OVRFLW_Disabled   << RTC_EVTEN_OVRFLW_Pos  ) |
										(RTC_EVTEN_COMPARE0_Disabled << RTC_EVTEN_COMPARE0_Pos) |
										(RTC_EVTEN_COMPARE1_Disabled << RTC_EVTEN_COMPARE1_Pos) |
										(RTC_EVTEN_COMPARE2_Disabled << RTC_EVTEN_COMPARE2_Pos) |
										(RTC_EVTEN_COMPARE3_Disabled << RTC_EVTEN_COMPARE3_Pos) ;
*/

//+++++++++++++++++++++++++++++++++++++++++++++...PRESCALER...++++++++++++++++++++++++++++++++++++++
	// initialize PRESCALER value
	/*prescaler is set to 0 by default 	(nRF51 ref. manual v2.1, chapter 18.2.5, PRESCALER register). Uncomment (and modify) this is you want to set different value.
	// 12-bit prescaler for COUNTER frequency (32768/(PRESCALER+1)). Must be written when RTC is STOPed.
	NRF_RTC0->PRESCALER = (0);
	*/
}

