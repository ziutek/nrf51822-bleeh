/*****************************************************************************************************
** -------------------------------------------------------------------------------------------------**
** Filename: main.c																																									**												
** -------------------------------------------------------------------------------------------------**	
** Description: main file for Nordic Semiconductor nRF51822 Evaluation Kit 													**	
** 							(a.k.a. PCA10001) 2b_Development_of_Comparator_Driver project: 											**
**							development of comparator's driver using an external button 												**
**							to trigger the comparator during development and experimental												**		
**							phases. 																																						**
**																																																	**
** 							Full sedcription of this task is described here: 																		**		
**							https://code.google.com/p/nrf51822-bleeh/wiki/2b_Development_of_Comparator_Driver		**
**																																																	**
**							This example shows the functionality of the low power comparator (LPCOMP)						**
**							To test, connect a voltage source (or a button to Vcc) to pin P0.03 (AIN4). 				**
**							When the voltage goes above Vcc/2=1.5 V, (LED0) is set.															**
** 							When the voltage goes below Vcc/2=1.5 V, (LED0) is cleared.													**
**							When the voltage on AIN6 either goes above or below the specified reference 				**
**							voltage, (LED1) is toggled to show that the LPCOMP interrupt handler is executed.		**																																								**
**																																																	**
** -------------------------------------------------------------------------------------------------**
** Author: Luca Buccolini																																						**
** -------------------------------------------------------------------------------------------------**
** Start date: Sep 3, 2014 																																					**
** -------------------------------------------------------------------------------------------------**
** Finish date:	work in progress																																		**
** -------------------------------------------------------------------------------------------------**
*****************************************************************************************************/

#include <pca10001.h>
#include <stdbool.h>
#include <stdint.h>
#include <nRF51.h>
#include <pca10001.h>

#include "nrf_gpio.h"
#include "nrf51_LPCOMP.h"

/*****************************************************************************************************
 * Main function
 *
 * @param  none
 * @return none
 *
 * @brief  Configures and enables the LPCOMP
 *
 ****************************************************************************************************/

int main(void)
{
	// Configure LED-pins as outputs
	nrf_gpio_cfg_output(LED_0);
	nrf_gpio_cfg_output(LED_1);

	//Initialize LPCOMP
	LPCOMP_init();
		
  
	// LED 0 and LED 1 blink alternately.
	while (true)
	{
		//Put CPU to sleep while waiting for interrupt to save power
		__WFI();
	}
}

