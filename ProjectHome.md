

**Table of contents**


# What is this? #
This is a free and open source project which I use to learn about embedded programming on an evaluation kit kindly provided by [Nordic Semiconductor](https://www.nordicsemi.com/). I'm talking about the [nRF51822-EK](https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822-Evaluation-Kit) Evaluation kit that is a stand-alone platform for evaluation and initial prototyping of [Bluetooth® low energy](http://www.bluetooth.com/Pages/low-energy-tech-info.aspx) and 2.4GHz proprietary designs with the [nRF51822 SoC](https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822).
The SoC (not the evaluation kit) will be a part of a remote speed sensor of a bicycle. This sensor will not need of batteries because it can harvest the energy by the movement of the front wheel of a bicycle. It send speed information at remote devices via [Bluetooth® low energy](http://www.bluetooth.com/Pages/low-energy-tech-info.aspx) wireless technology.

# The starting point #
It started some time ago. Looking at my bicycle's speedometer I thought: why it use a battery? There isn't enough energy we can harvest from the wheel's rotation?
Indeed, a rider can produce up to 200 W of power with his motion during pedaling.
My idea was to take a little part of this power, convert it into electrical power to supply an ultra-low-consumption speed sensor.
This hard work was definitely done some month ago obtaining, when the biker is travelling at 5 km/h, enough energy to elaborate the speed signal and transmit it via wireless proprietary protocol ([SimpliciTI®](http://processors.wiki.ti.com/index.php/SimpliciTI), by Texas Instruments). From TI's [application note](http://www.ti.com/lit/an/slaa378d/slaa378d.pdf) I derive the energy to transmit a single byte of data with a transmission period of 1.49 s (inversely proportional to the speed of 5 km/h); it's equal to 18.72 nW\*h, while the measured available harvested energy is 93.33 nW\*h for each pulse (wheel's rotation).
Ok, I've got enough energy, I can throw away the battery and start to design a self-powered speed sensor which transmits information via [Bluetooth® low energy](http://www.bluetooth.com/Pages/low-energy-tech-info.aspx) standard to external devices like smartphones.  Obviously BLE® is much more common than SimplitiTI® (or other wireless protocols like     [ANT+®](http://www.thisisant.com/)) so I decided use it. However to be hoped that it does not consume more than [SimpliciTI®](http://processors.wiki.ti.com/index.php/SimpliciTI).

# Main Tools #
To develop on this board I need the following tools:
  * ## nRF51822-EK ##
It is the [Evaluation Kit](https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822-Evaluation-Kit) provided by Nordic Nemiconductor.

![https://www.nordicsemi.com/var/ezwebin_site/storage/images/media/images/press-releases/nrf51-evkit/213706-1-eng-GB/nRF51-evkit_imagelarge.jpg](https://www.nordicsemi.com/var/ezwebin_site/storage/images/media/images/press-releases/nrf51-evkit/213706-1-eng-GB/nRF51-evkit_imagelarge.jpg)

> The nRF51822 Evaluation kit is a stand-alone platform for evaluation and initial prototyping of Bluetooth® low energy and 2.4GHz proprietary designs with the nRF51822 SoC. The kit gives access to all GPIO pins via pin headers and incorporates a coin-cell battery holder for portability enabling in-situ evaluation and test. Each board has 2 buttons, 2 LEDs, DC/DC converter circuit (optionally enabled in software), power supply and current measurement pins and a Segger J-Link device which enables program, debug and UART communication with the nRF51822 device over USB. A range of software examples from the nRF518 SDK can be used with the evaluation kit.

  * ### Standalone module ###
In alternative is possible to develop the firmware on a [stand alone nRF51822 module](https://www.nordicsemi.com/eng/Products/3rd-Party-Bluetooth-low-energy-Modules). I found some on [Ebay](http://www.ebay.it/itm/121113398808?ssPageName=STRK:MESINDXX:IT&_trksid=p3984.m1436.l2649) and [Aliexpress](http://www.aliexpress.com/item/NRF51822-2-4G-Wireless-Module-Wireless-Communication-Module-Bluetooth-module-zigbee-module-DMX512/1863345795.html) for a few euros.
> > |![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/nRF51822_standalone_module_photo_70%25.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/nRF51822_standalone_module_photo_70%25.png)|![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/nrf51822_standalone_module_sch_50%25.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/nrf51822_standalone_module_sch_50%25.png)|
|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

The module consist in a small pcb (24mm\*32mm) with the nRF51822 SoC, an embedded 2.4 GHz antenna with his matching network and a crystal oscillator at 16 MHz. Furthermore, via a pin header, are available from port 0.00 to port 0.29 of SoC (all the GPIO pins).
  * ### Programmer/Debugger ###
In this case, it's necessary a programmer/debbugger: it's a good (and cheap) choice the [Segger JLink EDU](http://www.segger.com/j-link-edu.html):

![http://www.segger.com/admin/uploads/imageBox/Product_J-Link_EDU_140x.jpg](http://www.segger.com/admin/uploads/imageBox/Product_J-Link_EDU_140x.jpg)

J-Link EDU is a USB powered JTAG emulator supporting a large number of CPU cores.
Based on a 32-bit RISC CPU, it can communicate at high speed with the supported target CPUs. J-Link is used around the world in tens of thousand places for development and production (flash programming) purposes.
J-Link is supported by all major IDEs such as IAR EWARM, Keil MDK, Rowley CrossWorks, Atollic TrueSTUDIO, IAR EWRX, Renesas HEW, Renesas e2studio, and many others.
Including all models, more than 100,000 J-Links have been sold so far, making J-Link probably the most popular emulator for ARM cores and the de-facto standard.

  * ## MDK-ARM Lite ##
The [MDK-ARM](http://www.keil.com/arm/mdk.asp) is a complete software development environment for Cortex™-M, Cortex™-[r4](https://code.google.com/p/nrf51822-bleeh/source/detail?r=4), ARM7™ and ARM9™ processor-based devices. [MDK-ARM](http://www.keil.com/arm/mdk.asp) is specifically designed for microcontroller applications, it is easy to learn and use, yet powerful enough for the most demanding embedded applications. All tools are integrated into µVision which includes project management, editor and debugger in a single easy-to-use environment. The fully integrated ARM C/C++ Compiler offers significant code-size and performance benefits to the embedded developer, however, MDK can also be used with the GNU GCC Compiler.

![http://www.element14.com/community/servlet/JiveServlet/showImage/102-41791-39-140670/uvision.png](http://www.element14.com/community/servlet/JiveServlet/showImage/102-41791-39-140670/uvision.png)

MDK-Lite (32KB) Edition is available for [download](https://www.keil.com/demo/eval/arm.htm). It does not require a serial number or license key.

  * ## GCC & Eclipse ##
Alternatively to MDK-ARM, it's possible to use Eclipse, the GNU C Compiler (GCC), and the GNU Debugger (GDB) to develop and debug programs on Nordic Semiconductor’s nRF51 series devices.
The nRF51 series Software Development Kits (SDK) come with GCC compatible examples. See this [Application Note](https://www.nordicsemi.com/eng/content/download/16271/261587/file/nAN-29%20%20nRF51%20Development%20with%20GCC%20and%20Eclipse%20Application%20Note%20v1.1.pdf).

  * ## S110 Softdevice-SDK ##
The [S110 SoftDevice](https://www.nordicsemi.com/eng/Products/S110-SoftDevice-v7.0) is a Bluetooth® low energy (BLE) Peripheral/Broadcaster protocol stack solution.
It integrates a Bluetooth low energy controller and host, and provides a full and flexible API for building Bluetooth low energy System on Chip (SoC) solutions.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/S110_stack.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/S110_stack.png)

The S110 Softdevice is a pre-compiled and linked binary software implementing a Bluetooth4.0 low energy (BLE) protocol stack on the nRF51822 chip. The Application Programming Interface (API) is a standard C language set of functions and data types that give the application complete compiler and linker independence from the Softdevice implementation. The Softdevice enables the application programmer to develop their code as a standard ARM® Cortex™-M0 project without needing to integrate with proprietary chip-vendor software frameworks. This means that any ARM® Cortex™-M0 compatible toolchain can be used to develop Bluetooth low energy applications with the S110 Softdevice.

  * ## Stable power supply ##
The speed sensor must be supplied with a 1.8V stable voltage as in the final application. Thus I can use some linear regulator such as LM317 to obtain this output voltage from a 5 V input.

  * ## Multimeter ##
It's useful to measure power supply voltages or the average current consumption.

  * ## Oscilloscope ##
It's essential to measure the instantaneous current consumption and the cadence (wheel's rotation period) input signal (the signal in input to comparator).

# The goal #
The goal is to design a firmware for nRF51822 SoC that elaborate speed information (measuring the front wheel's rotation period) and periodically transmit a byte of data to remote Bluetooth® devices. This must be done using the least amount of energy because the available energy is very limited.
  * ## Speed measurement ##
The speed is merely calculated as front-wheel's-circumference divided the wheel's-rotation-period thus, to calculate the speed, it is necessary to measure the time period between two consecutive pulses.
The wheel's rotation period is sensed with a coil-magnet device that produce a pulse with the following shape:

|![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/pulse%405kmh_80%25.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/pulse%405kmh_80%25.png)|![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/pulse%4050kmh_80%25.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/pulse%4050kmh_80%25.png)|
|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

The width and the amplitude of the pulse, as the period of the pulses train is proportional to speed of the bicycle.
To sense this analog pulse the best way is to put the analog signal on the comparator pin of the SoC. Definitely to be careful to high voltage and to reverse-voltage of the pulse! Because of this it's mandatory to insert an interface between the coil that produce the pulse and the SoC in order to preserve the SoC integrity.
In this work, the pulses are emulated connecting a button to low power SoC comparator thus the debouncing techniques are not evaluated and interface circuit is not designed (it will be a future work).
To achieve the minimum energy consumption the SoC, once it has elaborate and transmit speed information, must be placed in sleep state. It must be awakened from the following pulse, by the interrupt generated by the low power comparator. Obviously at least a comparator and a timer must be on during this "sleep-time" in order to measure the speed.

  * ## Bluetooth transmission ##
The elaborated speed information is transmitted using the Bluetooth® low energy standard and the [S110 SoftDevice](https://www.nordicsemi.com/eng/Products/S110-SoftDevice-v7.0) stack.
In Bluetooth® there's various profiles such as the [CSCP](https://developer.bluetooth.org/TechnologyOverview/Pages/CSCP.aspx) (Cycling Speed and Cadence Profile). Probably it's useful use a predefined profile.
Also in this case we must use tricks to obtain ultra low power consumption such as:
  * Sleep the MCU and transceiver when there isn't information to elaborate or transmit.
  * Use a maximum data transmission rate of 500-700 ms even though the wheel's rotation period is lower. Infact with bicycle there's no tremendous accelerations and the refresh-rate of the speedometer can be relaxed.

  * ## Evaluate energy consumption ##
When the previous steps were made it's time to join the functionality and transmit the speed information whenever the button is pressed. Finally must be measured the current consumption over a period of 1.49 s and evaluated the energy consumption. I hope that it's less than minimum available energy at 5 km/h, that is: 93.33 nW\*h.

During development phase the keyword to do not forget is: preserve energy!!

# Work Planning #

I've not experience with ARM devices development, so I start from simplest things:

  1. ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_done.jpg](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_done.jpg) [IDEsetup\_and\_Blinky\_Example](https://code.google.com/p/nrf51822-bleeh/wiki/1_IDEsetup_and_Blinky_Example): setup of the software development environment (MDK-ARM) and downloading of blinky example (hello world);
  1. ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_done.jpg](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_done.jpg) [Setup\_of\_a\_Blank\_Project](https://code.google.com/p/nrf51822-bleeh/wiki/2a_Setup_of_a_Blank_Project) and [Development\_of\_Comparator\_Driver](https://code.google.com/p/nrf51822-bleeh/wiki/2b_Development_of_Comparator_Driver) using an external button to trigger the comparator during development and experimental phases;
  1. ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_done.jpg](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_done.jpg) [Study\_and\_Development\_of\_UltraLowPower\_Techniques](https://code.google.com/p/nrf51822-bleeh/wiki/3_Study_and_Development_of_UltraLowPower_Techniques) (e.g. sleep the SoC and awake it after a comparator interrupt is received); evaluation of the ULP techniques measuring the current consumption;
  1. ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_done.jpg](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_done.jpg) [First\_Bluetooth\_Low\_Energy\_application](https://code.google.com/p/nrf51822-bleeh/wiki/4_First_Bluetooth_Low_Energy_application) using nRF51822 ([nAN-36](http://www.nordicsemi.com/eng/nordic/download_resource/24020/3/7144725) by Nordic Semiconductor)
  1. ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_undone.jpg](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_undone.jpg) Study of main Bluetooth low energy profiles and implementation of [CSCP profile](https://developer.bluetooth.org/TechnologyOverview/Pages/CSCP.aspx).
  1. ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_undone.jpg](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/checkbox_undone.jpg) ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/work-in-progress.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/work-in-progress.png) [Final\_Application\_development](https://code.google.com/p/nrf51822-bleeh/wiki/6_Final_Application_development?ts=1419088105&updated=6_Final_Application_development) development of Bluetooth transmission of speed byte once the comparator interrupt is triggered (button pressed by user). Once the byte is transmitted, put the SoC in sleep-state to preserve energy. Finally measure the current consumption during transmission and sleep phases; compare the measured energy with theoretically available energy and... it's time to drawn the conclusions!!!

The steps will be described into the [wiki](https://code.google.com/p/nrf51822-bleeh/w/list) section.

# Future works #
This work will be merged with the existing energy harvester/sensor; a new interface circuit to link the coil and the SoC's comparator must be designed and finally the running condition will be evaluated.

