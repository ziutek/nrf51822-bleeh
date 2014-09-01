/* ---------------------------------------------------------------------------
** Filename: main.c
**
** Description: main file for Nordic Semiconductor nRF51822 Evaluation Kit 
** (a.k.a. PCA10001)blinky example. This example file is derived from 
** Nordic Semiconductor's "blinky_example" using a custom delay module.
** This file contains the source code for a sample application using GPIO to
** drive LEDs.
**
** Author: Luca Buccolini
**
** -------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>
#include <nRF51.h>
#include "nrf_gpio.h"
#include <pca10001.h>
#include "nrf_delay_l85.h"

/**
 * Main function
 */
int main(void)
{
    // Configure LED-pins as outputs
    nrf_gpio_cfg_output(LED_0);
    nrf_gpio_cfg_output(LED_1);
  
    // LED 0 and LED 1 blink alternately.
    while (true)
    {
        nrf_gpio_pin_clear(LED_0);
        nrf_gpio_pin_set(LED_1);
    
			nrf_delay(100); // notice they're NOT ms!!
    
        nrf_gpio_pin_clear(LED_1);
        nrf_gpio_pin_set(LED_0);
    
        nrf_delay(100); // notice they're NOT ms!!
    }
}

