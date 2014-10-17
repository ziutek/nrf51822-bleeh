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
#include <nRF51.h>
#include <pca10001.h>

#include "nrf_gpio.h"
#include "nrf51_LPCOMP.h"
#include "nrf51_CLOCK.h"

/***************************************************************************************************
 * Main function
 *
 * @param  none
 * @return none
 *
 * @brief  Configures and enables the LPCOMP
 *
 **************************************************************************************************/

int main(void)
{
	
/*+++++++++++++++++++++++++++++++++__START-OF-POWER-CONFIGURATION__+++++++++++++++++++++++++++++++*/
	
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

/*+++++++++++++++++++++++++++++++++__END-OF-POWER-CONFIGURATION+++++++++++++++++++++++++++++++++++*/

	// Configure LED-pins as outputs
	nrf_gpio_cfg_output(LED_0);
	nrf_gpio_cfg_output(LED_1);
	
	//Initialize LPCOMP
	LPCOMP_init();
	
	//Intitialize Low Frequency Clock
	LFCLOCK_init();

	while (true)
	{
		__WFI(); 
	}
}

