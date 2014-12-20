/*********************************************************************************************************
** -----------------------------------------------------------------------------------------------------**
** Filename: speed_sensor.c																																							**												
** -----------------------------------------------------------------------------------------------------**	
** Description: comparator driver for Nordic Semiconductor nRF51822 to use with SoftDevice S110	7.1.0		**
**							and SDK 6.0.0																																						**
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
#include <nrf51.h>
#include <nrf51_bitfields.h>
#include "nrf_soc.h"
#include "boards\pca10001.h"
#include "segger_debugger\segger_RTT.h"
#include <stdlib.h>

/***************************************************************************************************
*_________________________________________DECLARATIONs______________________________________________
***************************************************************************************************/
void speed_event_handler(uint32_t speed_data);


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
*_____________________________________VARIABLES DECLARATION_________________________________________
***************************************************************************************************/
// My first event handler stuffs

/***************************************************************************************************
*________________________________________EVENT HANDLERS_____________________________________________
***************************************************************************************************/

//static void myeventhandler(void* data,uint16_t size)
//{
//	SEGGER_RTT_WriteString(0,"on my custom event handler\n\r");
//	SEGGER_RTT_printf(0,"Speed = %.2u\n\r",data);
//}

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
void LPCOMP_init (void)
{
	//Enable interrupt on LPCOMP UPWARD-CROSSING event
	NRF_LPCOMP->INTENSET = LPCOMP_INTENSET_UP_Msk;
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
	 /* Interrupt handler for LPCOMP */
void LPCOMP_IRQHandler(void)
{
	//____________________________LOCAL-VARIABLES-DEFINITION_________________________________________

	//Declaration and initialization of previous counter value useful to compute delta time: it must have memory
	static uint32_t PreviousRTC0CounterValue=0;
	//Declaration of DeltaTime and Speed variables
	uint32_t deltatime;
	static uint32_t old_speed_kmh;
	
	//____________________________SERVICE-ROUTINES___________________________________________________
		
	// Clear event
	NRF_LPCOMP->EVENTS_UP = 0;

	//______________________________COMPUTATION______________________________________________________
		
	//compute actual delta time between two LPCOMP upward crossing event
	deltatime=abs(NRF_RTC0->COUNTER - PreviousRTC0CounterValue);   
	//update value of PreviuousRTC0CounterValue
	PreviousRTC0CounterValue=NRF_RTC0->COUNTER;
	//Compute Speed in km/h from Delta Time in RTC0 ticks;
	uint32_t actual_speed_kmh=SPEED_KMH(deltatime);
	
	
	
	if(old_speed_kmh!=actual_speed_kmh)		//speed has changed: generate an event and send value through BLE to central device
	{
		old_speed_kmh=actual_speed_kmh;
				
		app_speed_sensor_evt_schedule(speed_event_handler,actual_speed_kmh);
		
		//app_sched_event_put(&speed_data,sizeof(speed_data),myeventhandler);

		//app_speed_sensor_evt_schedule(/*app_speed_sensor_handler_t ss_handler*/, actual_speed_kmh);
		
		//insert an event into the scheduler's queue.
		//app_sched_event_put(,,);			// per l'app buttone è: app_sched_event_put(&buttons_event, sizeof(buttons_event), app_button_evt_get);
		// i parametri di ingresso dovrebbero essere, in ordine: (void* p_evt_data, uint16_t event_size, app_sched_evt_handler handler)
		//dove &button_event è il puntatore alla struttura dati che contiene i dati relativi all'evento (ad es. l'handler, il pin del pulsante premuto, l'azione effettuata)
		//event_size è la dimensione della struttura dati relativa all'evento (event_size == sizeof(app_button_event_t));		
		//handler è l'indirizzo della funzione di gestione dell'evento. 
	}

	
	/*++++++++++++++++++++++++++++___START-OF-DEBUG-CODE___+++++++++++++++++++++++++++++++++++++*/
	//	Print to SEGGER_RTT-debugger-tool speed in km/h
	SEGGER_RTT_printf(0, "deltaTime=%u\r\n", deltatime);
	SEGGER_RTT_printf(0, "speed_kmh=%.2u\r\n", actual_speed_kmh);
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

