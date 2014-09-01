/* ---------------------------------------------------------------------------
** This file is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Filename: nrf_delay_l85.h
**
** Description: header file for delay functions
**
** Author: Luca Buccolini
**
** -------------------------------------------------------------------------*/


#ifndef NRF_DELAY_L85_H
#define NRF_DELAY_L85_H

#include <stdint.h>
#define __SYSTEM_CLOCK      (16000000UL)     /*!< nRF51 devices use a fixed System Clock Frequency of 16MHz */

void nrf_delay(uint32_t t_delay);

#endif 
