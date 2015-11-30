

# Introduction #

Here is described the setup of a blank project (see [this](http://www2.keil.com/docs/default-source/default-document-library/mdk5-getting-started.pdf) useful guide) and the development of a "Blinky example" using a custom module (nRF\_delay\_l85.h and nrf\_delay\_l85.c).
Finally will be described the steps to download the code into the board.


---


## Setup of a blank project ##

In uVision 5.10, you can create a new project clicking on _Project->New uVision Project..._. A window will shown. Chose the project folder (create one if no exists) and create the project file (with extension .uvprojx).

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/select_target.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/select_target.png)

Now select the device nRF51822\_xxAA and click ok. If the device is not available, probably you've not correctly installed the nRF51 Software Development Kit (SDK). For more information about software packs on MDK 5, see [here](http://www2.keil.com/docs/default-source/default-document-library/mdk5-getting-started.pdf#page=14).
After selecting the device, the Manage Run-Time Environment window shows the related software components for this device.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/ManageRunTimeEnvironment.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/ManageRunTimeEnvironment.png)

To build a blank project select only _CMSIS/CORE_ and _Device/Startup_ .

### CMSIS ###

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/CMSIS.jpg](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/CMSIS.jpg)

The Cortex Microcontroller Software Interface Standard (CMSIS) provides a ground-up software framework for embedded applications that run on Cortex-M based microcontrollers. The CMSIS enables consistent and simple software interfaces to the processor and the peripherals, simplifying software reuse, reducing the learning curve for microcontroller developers (see [here](http://www2.keil.com/docs/default-source/default-document-library/mdk5-getting-started.pdf#page=17)).

**CMSIS-CORE**: Defines the API for the Cortex-M processor core and peripherals and includes a consistent system startup code. The software components CMSIS/CORE and /Device/Startup are all you need to create and run applications on the native processor that uses exceptions, interrupts, and device peripherals.

**Using CMSIS-CORE**

A native Cortex-M application with CMSIS uses the software component CMSIS/CORE, which should be used together with the software component Device/Startup. These components provide the following central files:
  * The startup\_nRF51.s file with reset handler and exception vectors.
  * The system\_nRF51.c configuration file for basic device setup (clock and memory BUS).
  * The nRF51.h include file for user code access to the microcontroller device.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/UsingCMSIS.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/UsingCMSIS.png)

The nRF51.h header file is included in C source files and defines:

  * Peripheral Access with standardized register layout.
  * Access to Interrupts and Exceptions and the Nested Interrupt Vector Controller (NVIC).
  * Intrinsic Functions to generate special instructions, for example to activate sleep mode.
  * Systick Timer (SYSTICK) functions to configure and start a periodic timer interrupt.
  * Debug Access for printf-style I/O and ITM communication via on-chip CoreSight.

**Adding Software Components to the Project**

The files for the components CMSIS/CORE and Device/Startup are added to a project using the µVision dialog Manage Run-Time Environment. Just select the software components.
The µVision environment adds the related files.

### Project folders arrangement ###
The starting project arrangement is the following:

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/ProjectFoldersArrangement.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/ProjectFoldersArrangement.png)

Now create _Source_ folder and _Include_ folder inside the project's main folder. Populate the _Include_ folder with required header file (.h) and the _Source_ folder with required source code files (library, .c or .cpp).
To start with a new project are useful the following header files:

  * `compiler_abstraction.h`
  * `nrf.h`
  * `nfr_gpio.h`
  * `nrf51.h`
  * `nrf51:deprecated.h`
  * `pca10001.h`
  * `system_nrf51.h`

Copy the files from the _Include_ folder in the nRF51 SDK installation path (typically in `C:\Keil_v5\ARM\Device\Nordic\nrf51822\Include`) into new project's _Include_ folder.
In uVision Project window, right-click on _Target1_ folder and select _Manage Project Items_

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/ManageProjectItems.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/ManageProjectItems.png)

In Project Items tab, you can rename the project targets _Target1_ into _bleeh_ and add a new groups named _Lib_. Here will be placed the source library files.
Finally rename _Source Group 1_ to _Source_. Here will be placed the main source .c or .cpp files besides the header .h files.


---


## Setup of Blinky example from blank project ##

To create a working project add a new main.c file into the main project's folder. In uVision, right-click on _bleeh/Source_ folder and select _Add New Item to Group 'Source'_. Select _C file (.c)_ and name it _main.c_.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/AddNewItem.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/AddNewItem.png)

The main.c file will be created into selected location (typically the main's project folder) and is linked into Source uVision project's folder.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/AddNewItemtoGroupSource.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/AddNewItemtoGroupSource.png)

Now let's go to write the main source code for a Blinky DIY example: copy the following code into your empty _main.c_ file.

```
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
```

Main.c use modules from Nordic Semiconductor's SDK. Nordic's SDK provide also a delay module, let's show how to add this modules (header and source files) to the project. For example, let's write a custom nrf\_delay module:
  * In uVision, add new item to group source, select header file, name it "nrf\_delay\_l85.h" and place it into "include" folder of the project.
  * Add new item to group lib, select c file, name it "nrf\_delay\_l85.c" and place it into source folder of the project.
Fill _nrf\_delay\_l85.h_ file with:
```
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

```

and fill _nrf\_delay\_l85.c_ file with:
```
/* ---------------------------------------------------------------------------
** This file is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Filename: nrf_delay_l85.c
**
** Description: source file for delay functions
**
** Author: Luca Buccolini
**
** -------------------------------------------------------------------------*/

#include "nrf_delay_l85.h"

void nrf_delay(uint32_t t_delay) 
	{
		uint32_t i = t_delay*0.001*__SYSTEM_CLOCK;
		while(--i);
		}

```

Now save and rebuild all!

The complete project is available [here](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F2a_BlankProject).


---


## Download code to flash memory ##

Finally go to download code into nRF51822.
Select _Flash->Configure Flash Tools..._ and click on _Debug_ tab. In the right part of the window chose to use J-LINK / J-TRACE Cortex.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/OptionForTarget.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/OptionForTarget.png)

Then click on _Settings_ button. A new window show the Evaluation Kit on-board J-LINK properties.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/JLinkSetup.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/JLinkSetup.png)

Chose SW as port and click on _Flash Download_ tab.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/JLinkSetup_Reset%26Run.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/JLinkSetup_Reset%26Run.png)

Here tick _Reset and Run_ and click ok.
Finally, in main window of uVision, press the _Load_ button ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/downloadButton.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/downloadButton.png) to download the code into the nRF51822.
The leds will start to blink!!

Working code of this task is in  [svn/tags/2a\_BlankProject](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F2a_BlankProject).