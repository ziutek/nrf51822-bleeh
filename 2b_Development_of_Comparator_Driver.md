

# Introduction #
Here is described the development of LPCOMP driver, the Low Power Comparator peripheral drivers of nRF51822. The LPCOMP compares an input voltage (VIN+), which comes from an analog input pin selected through the PSEL register against a reference voltage (VIN-) selected through the REFSEL and EXTREFSEL registers. In this application is compared (VIN+) with VDD/2 (VDD\*4/8).

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/LPCOMP.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/LPCOMP.png)

Every time VIN+ rises above VIN- (the button is pressed->upward crossing), LED 0 of the board will change his state. According to this specifications a simple circuit was build.


---


## Circuit setup ##

To emulate the voltage pulse due to the crossing of the magnet above the coil was build a simple circuit. The circuit consists of a button (SW1) to trigger a positive voltage (Vcc=3 V) on a MCU's comparator pin (P0.03) and a one-pole RC circuit ([R1](https://code.google.com/p/nrf51822-bleeh/source/detail?r=1) and C1) that remove the button's bounces.
P3 is the on-board 2x5 male pin header of the PCA10001.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/debouncingRCcircuit.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/debouncingRCcircuit.png)

The R-C circuit has a charge-time-constant tc=10 ms and a discharge-time-constant td=20 ms thus the mechanical bounces are bypassed and button presses could be detected.
Once the button is pressed, a positive 3 V  voltage is at pin P0.03 and the upward crossing will generate an UP event of LPCOMP. This event will change the state of LED 0.
_NOTE: P0.03 correspond to the ADC/LPCOMP input 4 (AIN4)._


---


## Comparator's driver ##

The [nRF51 Series Reference Manual](https://www.nordicsemi.com/eng/nordic/download_resource/20337/10/53074698), in chapter 31, describe the Low Power Comparator characteristics. Thus, I develop the driver following this manual.

Starting from the blank project of task [2a](https://code.google.com/p/nrf51822-bleeh/wiki/2a_Setup_of_a_Blank_Project), let's to create two new files:
  * nrf51\_LPCOMP.c
  * nrf51\_LPCOMP.h
place the first in the main project's directory and the second in the Include directory, then, in uVision, add nrf51\_LPCOMP.c to project.

The hardware abstraction layer CMSIS provide different solutions. One of these is a software structure to easily interact with comparator:
```
/* ================================================================================ */
/* ================                     LPCOMP                     ================ */
/* ================================================================================ */


/**
  * @brief Low power comparator. (LPCOMP)
  */

typedef struct {                                    /*!< LPCOMP Structure                                                      */
  __O  uint32_t  TASKS_START;                       /*!< Start the comparator.                                                 */
  __O  uint32_t  TASKS_STOP;                        /*!< Stop the comparator.                                                  */
  __O  uint32_t  TASKS_SAMPLE;                      /*!< Sample comparator value.                                              */
  __I  uint32_t  RESERVED0[61];
  __IO uint32_t  EVENTS_READY;                      /*!< LPCOMP is ready and output is valid.                                  */
  __IO uint32_t  EVENTS_DOWN;                       /*!< Input voltage crossed the threshold going down.                       */
  __IO uint32_t  EVENTS_UP;                         /*!< Input voltage crossed the threshold going up.                         */
  __IO uint32_t  EVENTS_CROSS;                      /*!< Input voltage crossed the threshold in any direction.                 */
  __I  uint32_t  RESERVED1[60];
  __IO uint32_t  SHORTS;                            /*!< Shortcuts for the LPCOMP.                                             */
  __I  uint32_t  RESERVED2[64];
  __IO uint32_t  INTENSET;                          /*!< Interrupt enable set register.                                        */
  __IO uint32_t  INTENCLR;                          /*!< Interrupt enable clear register.                                      */
  __I  uint32_t  RESERVED3[61];
  __I  uint32_t  RESULT;                            /*!< Result of last compare.                                               */
  __I  uint32_t  RESERVED4[63];
  __IO uint32_t  ENABLE;                            /*!< Enable the LPCOMP.                                                    */
  __IO uint32_t  PSEL;                              /*!< Input pin select.                                                     */
  __IO uint32_t  REFSEL;                            /*!< Reference select.                                                     */
  __IO uint32_t  EXTREFSEL;                         /*!< External reference select.                                            */
  __I  uint32_t  RESERVED5[4];
  __IO uint32_t  ANADETECT;                         /*!< Analog detect configuration.                                          */
  __I  uint32_t  RESERVED6[694];
  __IO uint32_t  POWER;                             /*!< Peripheral power control.                                             */
} NRF_LPCOMP_Type;
```

The above structure is a part of file nrf51.h, a CMSIS:CORE component. It's a good help to write the comparator's initialization routine.
From the device datasheet, in the LPCOMP peripheral chapter, it-s clear how to init the peripheral:

_"...The PSEL, REFSEL, ...registers must be configured before the LPCOMP is enabled through the
ENABLE register..."_

```
   //Configure LPCOMP - set reference input source to AIN pin 4 (P0.03)
   NRF_LPCOMP->PSEL |= (LPCOMP_PSEL_PSEL_AnalogInput4 << LPCOMP_PSEL_PSEL_Pos);
   //Configure LPCOMP - set input source to AVDD*4/8 (= 1.5 V)
   NRF_LPCOMP->REFSEL |= (LPCOMP_REFSEL_REFSEL_SupplyFourEighthsPrescaling << LPCOMP_REFSEL_REFSEL_Pos);
```
_"...The LPCOMP is started by triggering the START task..."_
```
   //Enable and start the low power comparator
   NRF_LPCOMP->ENABLE = LPCOMP_ENABLE_ENABLE_Enabled;	
   NRF_LPCOMP->TASKS_START = 1;
```

However it's also essential to enable interrupt on LPCOMP CROSS event and enable the device specific interrupt in the NVIC interrupt controller so the whole init routine is:
```
 void LPCOMP_init (void)
   {
    //Enable interrupt on LPCOMP CROSS event
    NRF_LPCOMP->INTENSET = LPCOMP_INTENSET_CROSS_Msk;
    //Enable the device-specific interrupt in the NVIC interrupt controller
    NVIC_EnableIRQ(LPCOMP_IRQn);

    //Configure LPCOMP - set reference input source to AIN pin 4 (P0.03)
    NRF_LPCOMP->PSEL |= (LPCOMP_PSEL_PSEL_AnalogInput4 << LPCOMP_PSEL_PSEL_Pos);
    //Configure LPCOMP - set input source to AVDD*4/8 (= 1.5 V)
    NRF_LPCOMP->REFSEL |= (LPCOMP_REFSEL_REFSEL_SupplyFourEighthsPrescaling << LPCOMP_REFSEL_REFSEL_Pos);

    //Enable and start the low power comparator
    NRF_LPCOMP->ENABLE = LPCOMP_ENABLE_ENABLE_Enabled;	
    NRF_LPCOMP->TASKS_START = 1;
   }
```

To handle the interrupt is predefined a special ISR function. The function's name can be found in Device:Startup Component of CMSIS-CORE namely in the file **arm\_startup\_nrf51.s**, at row 93:
```
DCD      LPCOMP_IRQHandler ;LPCOMP
```
Finally in the ISR, before user code, is mandatory clear the event to capture following LPCOMP's interrupt.
```

void LPCOMP_IRQHandler(void)
       {
	// Clear event
	NRF_LPCOMP->EVENTS_CROSS = 0;

	// Sample the LPCOMP stores its state in the RESULT register. 
	// RESULT==0 means lower than reference voltage, 
	// RESULT==1 means higher than reference voltage
	NRF_LPCOMP->TASKS_SAMPLE = 1;
	nrf_gpio_pin_write(LED_0, NRF_LPCOMP->RESULT);

	// Toggle pin to indicate triggering of event
	nrf_gpio_pin_toggle(LED_1);
       }
```

In main, remember to initialize other peripheral or ports and initialize comparator's peripheral before use it.

Working code of this task is in  [svn/tags/2b\_LPCOMP\_driver](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F2b_LPCOMP_driver).