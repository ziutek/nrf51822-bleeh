/*********************************************************************************************************
** -----------------------------------------------------------------------------------------------------**
** Filename: nrf51_LPCOMP.c																																							**												
** -----------------------------------------------------------------------------------------------------**	
** Description: comparator driver for Nordic Semiconductor nRF51822 to use with SoftDevice S110	7.1.0		**
** -----------------------------------------------------------------------------------------------------**
** Author: Luca Buccolini																																								**
** -----------------------------------------------------------------------------------------------------**
** Start date: Sep 25, 2014 																																						**
** -----------------------------------------------------------------------------------------------------**
** Finish date:	Oct 06, 2014    																																				**
** -----------------------------------------------------------------------------------------------------**
** Modified to gain SoftDevice compatibility on:	Dec 10, 2014   																				**
** -----------------------------------------------------------------------------------------------------**
*********************************************************************************************************/

#include "speed_sensor\speed_sensor.h"
#include <nrf51_bitfields.h>
#include "nrf_soc.h"
#include "boards\pca10001.h"
#include "segger_debugger\segger_RTT.h"
#include "softdevice_handler.h"

/***************************************************************************************************
*_____________________________________________MACRO_________________________________________________
***************************************************************************************************/
#define TYRE_CIRCUMFERENCE_M 2070E-3
#define RTC0_RESOLUTION_S 30.517E-6
#define SPEED_MS(dt) ((TYRE_CIRCUMFERENCE_M)/(dt*RTC0_RESOLUTION_S))
#define SPEED_KMH(dt) (SPEED_MS(dt)*3.6)

/* 	it seems the LPCOMP works well without SW_DELAY before TASK_START. Uncomment if not!
		hints about possible improper working was found here:
		https://devzone.nordicsemi.com/question/15153/s110-lpcomp-interrupt-not-working/
#define SW_DELAY() 																						\
									{																						\
									__nop();__nop();__nop();__nop();__nop();		\
									__nop();__nop();__nop();__nop();__nop();		\
									__nop();__nop();__nop();__nop();__nop();		\
									__nop();__nop();__nop();__nop();__nop();		\
									__nop();__nop();__nop();__nop();__nop();		\
									__nop();__nop();__nop();__nop();__nop();		\
									}
*/

/***************************************************************************************************
*__________________________________GLOBAL-VARIABLES-DEFINITION______________________________________
***************************************************************************************************/

//Definition and initialization of previous counter value useful to compute delta time 
static uint32_t PreviousRTC0CounterValue=0;
//Definition and initialization of DeltaTime and Speed variables
static uint32_t deltatime=0;
static uint32_t speed_kmh;
					
/***************************************************************************************************
*__________________________________APIs EXPORTED TO TASK LEVEL______________________________________
***************************************************************************************************/
/**
 * Initialize the comparator
 *
 * @param  none
 * @return none
 *
 * @brief  Configures and enables the LPCOMP through SoftDevice module
 *
 */
//------------------------------------------------------------------------------------------------- 
 void LPCOMP_init (void)
	{
		
	uint32_t err_code;
		
	//Enable interrupt on LPCOMP UPWARD-CROSSING event
	NRF_LPCOMP->INTENSET = LPCOMP_INTENSET_UP_Msk;
	//Set the priority level of the interrupt through SoftDevice module
	err_code=sd_nvic_SetPriority(LPCOMP_IRQn, NRF_APP_PRIORITY_LOW);
	APP_ERROR_CHECK(err_code);
	//Enable the device-specific interrupt in the NVIC interrupt controller through SoftDevice module
	err_code = sd_nvic_ClearPendingIRQ(LPCOMP_IRQn);
  APP_ERROR_CHECK(err_code);
	err_code=sd_nvic_EnableIRQ(LPCOMP_IRQn);
	APP_ERROR_CHECK(err_code);

	//Configure LPCOMP - set reference input source to AIN pin 4 (P0.03)
	NRF_LPCOMP->PSEL |= (LPCOMP_PSEL_PSEL_AnalogInput4 << LPCOMP_PSEL_PSEL_Pos);
	//Configure LPCOMP - set input source to AVDD*4/8 (= 1.5 V)
	NRF_LPCOMP->REFSEL |= (LPCOMP_REFSEL_REFSEL_SupplyFourEighthsPrescaling << LPCOMP_REFSEL_REFSEL_Pos);

	//Enable and start the low power comparator
	NRF_LPCOMP->ENABLE = LPCOMP_ENABLE_ENABLE_Enabled;	
	NRF_LPCOMP->POWER = 1;			
	/*ATTENTION!!! If no event are delivered, put a delay here (1ms it's ok). Uncomment following line.
	SW_DELAY();		//information about this delay here: https://devzone.nordicsemi.com/question/15153/s110-lpcomp-interrupt-not-working/
	*/
 	NRF_LPCOMP->TASKS_START = 1;
		
	/*
	//Debug code: print the init status
	SEGGER_RTT_printf(0,"LPCOMP intializated\r\n");
	*/
	}
	
/**
 * Get the speed_kmh variable
 *
 * @param  none
 * @return The computed speed in [km/h]
 *
 * @brief  Compute the speed in km/h and return it
 *
 */
//------------------------------------------------------------------------------------------------- 
uint32_t get_speed(void)
	{
		//Compute Speed in km/h from Delta Time in RTC0 ticks;
		return SPEED_KMH(deltatime);
	}
	
	
//-------------------------------------------------------------------------------------------------
	
/***************************************************************************************************
*____________________________________________LPCOMP_ISR_____________________________________________
***************************************************************************************************/

/**
 * Interrupt handler for LPCOMP
 *
 * @param  none
 * @return none
 *
 * @brief  	ISR for LPCOMP: 
 * 					clear event's flag, 
 */
//-------------------------------------------------------------------------------------------------
	 /* Interrupt handler for LPCOMP */
void LPCOMP_IRQHandler(void)
	{
	// Clear event
	NRF_LPCOMP->EVENTS_UP = 0;

	//compute delta-time between two LPCOM upward crossing event
	deltatime=abs(NRF_RTC0->COUNTER - PreviousRTC0CounterValue);    
	
	//update value of PreviuousRTC0CounterValue
	PreviousRTC0CounterValue=NRF_RTC0->COUNTER;
		
	//Print to SEGGER_RTT-debugger-tool speed in km/h
	/*++++++++++++++++++++++++++++___START-OF-DEBUG-CODE___+++++++++++++++++++++++++++++++++++++*/
	//
	SEGGER_RTT_printf(0, "deltaTime=%u\r\n", deltatime);
	SEGGER_RTT_printf(0, "speed_kmh=%.2u\r\n", speed_kmh);
	//
	/*++++++++++++++++++++++++++++++___END-OF-DEBUG-CODE___+++++++++++++++++++++++++++++++++++++*/
	
		
	//Debug code: print the interrupt has happened and print the result of the comparator
	SEGGER_RTT_printf(0,"LPCOMP interrupt! Result=%d \r\n",NRF_LPCOMP->RESULT);
	static uint8_t LPCOMP_interrupt_counter=0;
	SEGGER_RTT_printf(0,"LPCOME interrupt happened %d times\r\n", ++LPCOMP_interrupt_counter);
	SEGGER_RTT_printf(0,"RTC1->COUNTER=%d \r\n", NRF_RTC0->COUNTER);


	//release external crystal
	sd_clock_hfclk_release();
	}
//-------------------------------------------------------------------------------------------------
