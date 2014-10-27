/***************************************************************************************************
** -------------------------------------------------------------------------------------------------
** Module name: MAIN
** -------------------------------------------------------------------------------------------------
** Filename: main.c																																																					
** -------------------------------------------------------------------------------------------------	
** Status
** -------------------------------------------------------------------------------------------------
** Description: main file for Nordic Semiconductor nRF51822 Evaluation Kit 														
** 							(a.k.a. PCA10001) 3_Study_and_Development_of_UltraLowPower_Techniques: 											
**							Study and development of ultra-low-power techniques and evaluation of the measuring 
**							current consumption techniques;																																					
**																																																	
** 							Full description of this task is described here: 																				
**							https://code.google.com/p/nrf51822-bleeh/w/edit/3_Study_and_Development_of_UltraLowPower_Techniques	
**																																																	
**							The firmware to be designed will sleep the CPU until a pulse is received from AIN4 
**							(LPCOMP peripheral). This guarantee low power consumption during inactivity state 
**							but a timer must be on to measure time interval among two pulses. It's mandatory 
**							to use a low power timer/clock.
**							When the second pulse is received, the delta-time is calculated and a speed-related-
**							information could be extrapolated.
**							During test phases, the current consumption are evaluated and compared among two 
**							different solutions: 
**						   - using of ULP tecniques (e.g. sleep the CPU during inactivity state);
**							 - not using of ULP tecniques;														
** -------------------------------------------------------------------------------------------------
** Documentation:
**					- https://www.nordicsemi.com/eng/nordic/download_resource/20337/10/53074698
**					- http://infocenter.arm.com/help/topic/com.arm.doc.dui0497a/DUI0497A_cortex_m0_r0p0_generic_ug.pdf
**				  - http://www.embedded.com/design/mcus-processors-and-socs/4230085/The-basics-of-low-power-programming-on-the-Cortex-M0
**					- http://processors.wiki.ti.com/index.php/ULP_Advisor
**					- http://www.embedded.com/design/real-time-and-performance/4210470/1/Efficient-C-Code-for-ARM-Devices
** -------------------------------------------------------------------------------------------------
** Author: Luca Buccolini																																						
** -------------------------------------------------------------------------------------------------
** Date: Oct 9, 2014 																																					
** -------------------------------------------------------------------------------------------------
***************************************************************************************************/

#include <pca10001.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <nRF51.h>
#include <pca10001.h>

#include "nrf_gpio.h"
#include "nrf51_LPCOMP.h"
#include "nrf51_CLOCK.h"
#include "nrf51_RTC.h"

#include "debug\SEGGER_RTT.h"

/***************************************************************************************************
*_____________________________________________MACRO_________________________________________________
***************************************************************************************************/
#define TYRE_CIRCUMFERENCE_M 2070E-3
#define RTC0_RESOLUTION_S 30.517E-6
#define SPEED_MS(dt) ((TYRE_CIRCUMFERENCE_M)/(dt*RTC0_RESOLUTION_S))
#define SPEED_KMH(dt) (SPEED_MS(dt)*3.6)

/***************************************************************************************************
*__________________________________GLOBAL-VARIABLES-DEFINITION______________________________________
***************************************************************************************************/
	//Definition and initialization of cnt variable for debug purposes (SEGGER RTT printf function)
	uint32_t debug_cnt=0;
	//Definition and initialization of RTC0-started flag
	uint32_t IsRTC0Running=0;
	//Definition and initialization of previous counter value useful to compute delta time 
	uint32_t PreviousRTC0CounterValue=0;
	//Definition and initialization of DeltaTime and Speed variables
	uint32_t deltatime=0;
	uint32_t speed_kmh=0;

/***************************************************************************************************
*______________________________________________MAIN_________________________________________________
***************************************************************************************************/
int main(void)
{
	
/*++++++++++++++++++++++++++++++++___START-OF-POWER-CONFIGURATION___++++++++++++++++++++++++++++++*/
	
	//Disable internal DC/DC step down
	NRF_POWER->DCDCEN=POWER_DCDCEN_DCDCEN_Disabled;	
	
	//The Low Power mode is selected by triggering the LOWPWR task but when the system enters ON mode, 
	//it will, by default, reside in the Low Power sub power mode.	
	//Configure Low_power Mode (When the system enters ON mode [__WFI()  or __WFE() instructions], it will, by default, reside in the Low Power sub power mode)
	NRF_POWER->TASKS_LOWPWR = 1;
	
	// Switch-off unused RAM blocks
	NRF_POWER->RAMON= (POWER_RAMON_ONRAM0_RAM0On   <<	POWER_RAMON_ONRAM0_Pos )  |		// Ram block 0  ON  in ON-MODE
										(POWER_RAMON_ONRAM1_RAM1Off  <<	POWER_RAMON_ONRAM1_Pos )  |		// Ram block 1 OFF  in ON-MODE
										(POWER_RAMON_OFFRAM0_RAM0Off <<	POWER_RAMON_OFFRAM0_Pos)  |		// Ram block 0 OFF in OFF-MODE	
										(POWER_RAMON_OFFRAM1_RAM1Off <<	POWER_RAMON_OFFRAM1_Pos)	;		// Ram block 1 OFF in OFF-MODE
	
/*++++++++++++++++++++++++++++++++___END-OF-POWER-CONFIGURATION___++++++++++++++++++++++++++++++++*/
	
/*+++++++++++++++++++++++++++___START-OF-PERIPEHERALS-INITIALIZATION___+++++++++++++++++++++++++++*/
	
	//Initialize LPCOMP
	LPCOMP_init();
	
	//Intitialize Low Frequency Clock
	LFCLOCK_init();
	
	//Initialize RTC0 counter
	RTC0_init();	//nothing to initialize: default value are OK!
		
/*+++++++++++++++++++++++++++++___END-OF-PERIPEHERALS-INITIALIZATION___+++++++++++++++++++++++++++*/
	

/*+++++++++++++++++++++++++++++++++___START-OF-INFINITE-LOOP___+++++++++++++++++++++++++++++++++++*/
	while (true)
	{
		__WFI(); 	// Wait-For-Interupt (CPU in sleep, Power-ON/Low Power Mode)
		//Following code is executed when LPCOMP wake-up the CPU (up-crossing event triggered)
		
		if(!IsRTC0Running)	//Is first RTC0 counting, RTC0 is not running
		{
			//Start RTC0 counter
			NRF_RTC0->TASKS_START=1;
			//Clear flag
			IsRTC0Running=1;
		}
		else	//RTC0 was already running
		{
			//compute delta time between two LPCOM upward crossing event
			deltatime=abs(NRF_RTC0->COUNTER - PreviousRTC0CounterValue);	
			
			//update value of PreviuousRTC0CounterValue
			PreviousRTC0CounterValue=NRF_RTC0->COUNTER;
			
			//Compute Speed in km/h from Delta Time in RTC0 ticks;
			speed_kmh=SPEED_KMH(deltatime);
			
			
			//Print to SEGGER_RTT-debugger-tool speed in km/h
			/*++++++++++++++++++++++++++++___START-OF-DEBUG-CODE___+++++++++++++++++++++++++++++++++++++*/
			//
			SEGGER_RTT_printf(0, "debug_cnt=%d\r\n", debug_cnt++);
			SEGGER_RTT_printf(0, "deltaTime=%u\r\n", deltatime);
			SEGGER_RTT_printf(0, "speed_kmh=%.2u\r\n", speed_kmh);
			//
			/*++++++++++++++++++++++++++++++___END-OF-DEBUG-CODE___+++++++++++++++++++++++++++++++++++++*/
			
		}
		
	

	}
/*+++++++++++++++++++++++++++++++++++___END-OF-INFINITE-LOOP___+++++++++++++++++++++++++++++++++++*/
}

