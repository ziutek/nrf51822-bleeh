/*****************************************************************************************************
** -------------------------------------------------------------------------------------------------**
** Filename: nrf51_LPCOMP.c																																					**												
** -------------------------------------------------------------------------------------------------**	
** Description: comparator driver for Nordic Semiconductor nRF51822 																**
** -------------------------------------------------------------------------------------------------**
** Author: Luca Buccolini																																						**
** -------------------------------------------------------------------------------------------------**
** Start date: Sep 25, 2014 																																				**
** -------------------------------------------------------------------------------------------------**
** Finish date:	Oct 06, 2014    																																		**
** -------------------------------------------------------------------------------------------------**
** Modified to obtain SoftDevice compatibility on:	Dec 10, 2014   																	**
** -------------------------------------------------------------------------------------------------**
*****************************************************************************************************/

#include <nrf51.h>
#include <nrf51_bitfields.h>
#include "nrf_soc.h"
#include "lpcomp\nrf51_LPCOMP.h"
#include "boards\pca10001.h"
#include "segger_debugger\segger_RTT.h"
#include "Include\nrf_delay.h"

/**
 * Initialize the comparator
 *
 * @param  none
 * @return none
 *
 * @brief  Configures and enables the LPCOMP through SoftDevice module
 *
 */
 
 void LPCOMP_init (void)
	{
	//Enable interrupt on LPCOMP CROSS event
	NRF_LPCOMP->INTENSET = LPCOMP_INTENSET_CROSS_Msk;
	//Set the priority level of the interrupt through SoftDevice module
	sd_nvic_SetPriority(LPCOMP_IRQn, NRF_APP_PRIORITY_LOW);
	//Enable the device-specific interrupt in the NVIC interrupt controller through SoftDevice module
	sd_nvic_EnableIRQ(LPCOMP_IRQn);

	//Configure LPCOMP - set reference input source to AIN pin 4 (P0.03)
	NRF_LPCOMP->PSEL |= (LPCOMP_PSEL_PSEL_AnalogInput4 << LPCOMP_PSEL_PSEL_Pos);
	//Configure LPCOMP - set input source to AVDD*4/8 (= 1.5 V)
	NRF_LPCOMP->REFSEL |= (LPCOMP_REFSEL_REFSEL_SupplyFourEighthsPrescaling << LPCOMP_REFSEL_REFSEL_Pos);

	//Enable and start the low power comparator
	NRF_LPCOMP->ENABLE = LPCOMP_ENABLE_ENABLE_Enabled;	
	NRF_LPCOMP->POWER = 1;			//ATTENTION!!! If no event are delivered, put a delay here (1ms it's ok).
	nrf_delay_ms(1);
 	NRF_LPCOMP->TASKS_START = 1;
	//Debug code: print the init status
	SEGGER_RTT_printf(0,"LPCOMP intializated\n");
	}
	 
/**
 * Interrupt handler for LPCOMP
 *
 * @param  none
 * @return none
 *
 * @brief  	ISR for LPCOMP: 
 * 					clear event's flag, 
 *					sample LPCOMP and write result to LED_0 
 *					toggle LED_1 to indicate triggering of event	
 */
//---------------------------------------------------------------------------------------------------------------------------------
	 /* Interrupt handler for LPCOMP */
void LPCOMP_IRQHandler(void)
	{
	// Clear event
	NRF_LPCOMP->EVENTS_CROSS = 0;

	// Sample the LPCOMP stores its state in the RESULT register. 
	// RESULT==0 means lower than reference voltage, 
	// RESULT==1 means higher than reference voltage
	NRF_LPCOMP->TASKS_SAMPLE = 1;
	
	//Debug code: print the interrupt has happened and print the result of the comparator
	SEGGER_RTT_printf(0,"LPCOMP interrupt! Result= %d\n",NRF_LPCOMP->RESULT);
	static uint8_t LPCOMP_interrupt_counter=0;
	SEGGER_RTT_printf(0,"LPCOME interrupt happened &d times\n", LPCOMP_interrupt_counter);
		
	//release external crystal
	sd_clock_hfclk_release();
	}
//---------------------------------------------------------------------------------------------------------------------------------
