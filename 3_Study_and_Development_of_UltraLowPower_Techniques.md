

# Introduction: Ultra Low Power #
Here is described the using of Ultra Low Power tecniques. These tips come from different sources and are joined together. Most useful tips come from:
  * [nRF51 Series Reference Manual V2.1, Nordic Semiconductor](https://www.nordicsemi.com/eng/nordic/download_resource/20337/10/53074698)
  * [CorteCortex™-M0 Devices-Generic User Guide (ARM DUI 0497A)](http://infocenter.arm.com/help/topic/com.arm.doc.dui0497a/DUI0497A_cortex_m0_r0p0_generic_ug.pdf)
  * [The basics of low-power programming on the Cortex-M0, Joseph Yiu, ARM Ltd.](http://www.embedded.com/design/mcus-processors-and-socs/4230085/The-basics-of-low-power-programming-on-the-Cortex-M0)
  * [ULP Advisor, Texas Instruments](http://processors.wiki.ti.com/index.php/ULP_Advisor)
  * [Energy efficient C code for ARM devices, Chris Shore, ARM](http://www.embedded.com/design/real-time-and-performance/4210470/1/Efficient-C-Code-for-ARM-Devices)

Following, are developed timer's drivers. Finally they're used in a new project implementing ULP tecniques.
At the end is designed a low-current measuring circuit and is used to evaluate energy consumption of the system.

The firmware to be designed will sleep the CPU until a pulse is received from AIN4 (LPCOMP peripheral). This guarantee low power consumption during inactivity state but a timer must be on to measure time interval among two pulses. It's mandatory to use a low power timer/clock.
When the second pulse is received, the delta-time is calculated and a speed-related-information could be extrapolated.
During test phases, the current consumption are evaluated and compared among two different solutions:
  * using of ULP tecniques (e.g. sleep the CPU during inactivity state);
  * not using of ULP tecniques;

## ULP techniques ##

Ultra Low Power techniques involve either hardware and firmware feature.
The choosing of this IC is related to its low voltage power supply and to SOC warranties such as low component count (due to integrated radio module). However the nRF51 supports three different power supply alternatives: internal DC/DC converter setup, internal LDO setup, and Low Voltage mode setup thus, to preserve energy, DC/DC converter and LDO must be disabled.
In firmware, different solutions will be applied. E.g. hold the nRF51 in sleep mode during inactivity time, maintain Real Time Clock and interrupt from LPCOMP on, etc.
These solutions is described in following paragraphs.


### The power management ###
As mentioned above, maximum power saving mode is Low Voltage mode. Unfortunately this mode could be reach only modifying circuit topology, as shown in following schematic:
![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/LowPowerMode.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/LowPowerMode.png)
Since PCA10001 hasn't any DEC2 pin on external header, it's very difficult to apply this little change in circuit's topology thus I prefer use the internal LDO. This can be obtained disabling internal DC/DC and, eventually, short-circuited SB1 (equal to short-circuited AVDD and VDD), as shown in following images:

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/InternalLDOOnly.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/InternalLDOOnly.png)

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/SB1_link.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/SB1_link.png)

I've no short-circuited SB1 to preserve board integrity, so a small resistance (and energy wasting) is in series with supply current flowing. I think the resistance is the Rds resistance of a mosfet so it's in the order of tenth of milliohms.
The supply voltage range is 1.8 V - 3.6 V using internal LDO regulator. I chose to supply the nRF51 with external well regulated 1.8 V to preserve power.

Firmware task to disable DC/DC converter is very simple:
```
   //Disable internal DC/DC step down
   NRF_POWER->DCDCEN=POWER_DCDCEN_DCDCEN_Disabled;	
```

#### Peripherals power control ####
Writing code I noticed, for all peripherals, a POWER register (in header file nrf51\_bitfields.h). It seems logical to disable power for each unused peripheral however there's no documents to describe power management of peripherals. This is because the POWER registers are deprecated and all peripherals, now, are powered by default (set in the startup file), as there was no added current draw of enabling them [see here](https://devzone.nordicsemi.com/question/1130/nrf51-peripheral-power-control/).

#### System mode and sub-power mode ####
nRF51 series has two system-mode:
  * System OFF mode: is the deepest power saving mode the system can enter. In this mode, the system’s core functionality is powered down and all ongoing tasks are terminated. The only mechanism that is functional and responsive in this mode is the reset and the wake-up mechanism. One or more blocks of RAM can be retained in System OFF mode depending on the settings in the RAMON register. The system can be woken up from System OFF mode either from the DETECT signal generated by the GPIO peripheral, by the ANADETECT signal generated by the LPCOMP module, or from a reset. When the system wakes up from OFF mode, a system reset is performed.
Unfortunately System OFF mode all timers are shut down although LPCOMP is on; thus this mode is useless.

  * System ON mode: is a fully operational mode, where the CPU and selected peripherals can be brought into a state where they are functional and more or less responsive depending on the sub power mode selected. In System ON mode the CPU can either be active or sleeping. The CPU enters sleep by executing the WFI or WFE instruction found in the CPU’s instruction set. In WFI sleep the CPU will wake up as a result of an interrupt request if the associated interrupt is enabled in the NVIC. In WFE sleep the CPU will wake up as a result of an interrupt request regardless of the associated interrupt being enabled in the NVIC or not.
The system implements mechanisms to automatically switch on and off the appropriate power sources depending on how many peripherals are active, and how much power is needed at any given time. The power requirement of a peripheral is directly related to its activity level.

**Sub power modes**

During CPU sleep, in System ON mode, the system can reside in one of the following two sub power modes:

  * Constant Latency
  * Low Power

In Constant Latency mode the CPU wakeup latency and the PPI task response will be constant and kept at a minimum. This is secured by forcing a set of base resources while in sleep, see the device specific product specification for more information about which resources are forced on. The advantage of having a constant and predictable latency will be at the cost of having increased power consumption. The Constant Latency mode is selected by triggering the CONSTLAT task.

In Low Power mode the automatic power management system, will be most effective and save most power. The advantage of having low power will be at the cost of having varying CPU wakeup latency and PPI task response. The Low Power mode is selected by triggering the LOWPWR task but, when the system enters ON mode, it will, by default, reside in the Low Power sub power mode.
Definitely the configuration chosen is System ON mode->Low Power.

ISO/IEC C cannot directly generate the WFI, WFE, and SEV ARM Cortex M0 instructions. The CMSIS provides the following intrinsic functions for these instructions:

```
void __WFE(void) // Wait for Event
void __WFI(void) // Wait for Interrupt
void __SEV(void) // Send Event
```

The interesting one in this context is WFI() that suspends execution until one of the following events occurs:
  * an exception
  * an interrupt becomes pending, which would preempt if PRIMASK was clear
  * a Debug Entry request, regardless of whether debug is enabled.

_Note_
_WFI is intended for power saving only. When writing software assume that WFI might behave as a NOP operation_

Thus, to enter in sleep mode, simply write:
```
__WFI();
```

### Run code from RAM ###
Analyzing nRF51 product specification document, in chapter 8.4, it's easy to see the difference in current consumption between running code from flash memory and running code from RAM. In first case, typical current consumption is 4.4 mA @ 16 MHz instead, executing code from RAM, due to drawn only a current of 2.4 mA from power supply @ 16 MHz.
Well, thus move code from FLASH to RAM memory is a to-do thing (if S110 SoftDevice stack permits it)!

### RAM retention ###
Each of the available RAM blocks, can power up and down independently in both System ON and System OFF mode. nRF51822 has two 8 kB blocks. Code to enable RAM block 0 in OFF-mode and ON-mode and disable block 1 either in OFF-mode and ON-mode is below.
```
// Switch-off unused RAM blocks
NRF_POWER->RAMON= (POWER_RAMON_ONRAM0_RAM0On<<POWER_RAMON_ONRAM0_Pos)    |  // Ram block 0  ON  in ON-MODE
                  (POWER_RAMON_ONRAM1_RAM1Off<<POWER_RAMON_ONRAM1_Pos)   |  // Ram block 1 OFF  in ON-MODE
		  (POWER_RAMON_OFFRAM0_RAM0Off<<POWER_RAMON_OFFRAM0_Pos) |  // Ram block 0 OFF in OFF-MODE
                  (POWER_RAMON_OFFRAM1_RAM1Off<<POWER_RAMON_OFFRAM1_Pos) ;  // Ram block 1 OFF in OFF-MODE
```
This help to reduce current consumption due to RAM retention.

### The choice of clock and timer ###
#### Clock ####
The lowest possible sleep current while maintaining BLE connection timing is using Real Time Counter (RTC). This timer use the Low Frequency Clock (LFCLK) @ 32.768 kHz, using an external crystal oscillator.
Thus, the total current consumption in sleep mode is about I\_on + I\_RTC + I\_X32k + I\_LPC, typical 2.3 + 0.2 + 0.4 + 0.5 = 3.4 µA where I\_on is the SYSTEM-ON base current with 16 kB RAM enabled, I\_RTC is the Timer (LFCLK source) current consumption, I\_X32k is the run current for 32.768 kHz crystal oscillator and I\_LPC is the run current for LPCOMP.
When nRF51 is awake, is needed a 16 MHz clock to handle the CPU, thus a second clock must be activated during CPU elaboration and transceiver transmission. The second clock is the High Frequency Clock (HFCLK source). Let's go to choice the two clock sources with lower energy consumption regard their startup times.

  * Low frequency clock (LFCLK) - The system supports three LFCLK clock sources: the 32.768 kHz crystal oscillator, the 32.768 kHz RC oscillator, and the 32.768 kHz synthesized clock. The 32.768 kHz crystal oscillator requires an external AT-cut quartz crystal to be connected to the X L1 and X L 2 pins in parallel resonant mode. The PCA10001, fortunately, has this quartz crystal. Since the low frequency clock is kept on either during CPU sleep and CPU off, I don't care about crystal start-up and de-bounce time. The current consumption of crystal oscillator is 0.4 uA typical and is lower than current consumption of RC oscillator (0.8 uA typical) thus I chose to use the external quartz crystal as source of low frequency clock even if the start-up time for crystal oscillator is 0.3 s versus a start-up time for RC oscillator of only 100 us. The LFCLK clock is started by first selecting the preferred clock source in the LFCLKSRC register and then triggering the LFCLKSTART task. If the selected clock source cannot be started immediately the 32.768 kHz RC oscillator will start automatically and generate the LFCLK until the selected clock source is available.
Code for initialization of LFCLK is:

```
  // Select the preferred clock source as Cryatal Quartz
  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
  // Clear LFCLKSTARTED event
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  // Trigger LFCLKSTART task
  NRF_CLOCK->TASKS_LFCLKSTART = 1;

  // Wait for the low frequency clock to start
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {}
```

  * High frequency clock (HFCLK) - The system supports two high frequency clock sources: the 16/32 MHz crystal oscillator and the 16 MHz RC oscillator. The HFCLK (16/32 MHz) crystal oscillators require an external AT-cut quartz crystal to be connected to the XC 1 and X C2 pins in parallel resonant mode. The PCA10001 has a 16 MHz quartz crystal. When the system enters ON mode, the 16 MHz RC oscillator will start up automatically to provide the HFCLK to the CPU and other active parts of the system. The HFCLK crystal oscillator is started by triggering the HFCLKSTART task and stopped using the HFCLKSTOP task. A HFCLKSTARTED event will be generated when the selected HFCLK crystal oscillator has started. If the system does not require a 16 MHz clock, the 16 MHz RC oscillator may be switched off automatically to save power. This occurs if all peripherals that require the HFCLK are appropriately stopped or disabled, and the CPU is sleeping. When this condition is no longer met the 16 MHz RC oscillator is automatically restarted. These optimization steps are only performed when the HFCLK is generated from the 16 MHz RC oscillator.
The current consumption of high frequency crystal oscillator is 470 uA typical and is lower than current consumption of RC-HF oscillator (750 uA typical) however I chose to use the internal 16 MHz RC oscillator because of start-up time. In fact, start-up time of RC oscillator is 2.5 us typically versus a typically start-up time of 800 us for crystal oscillator. It's a huge difference!

No code is needed to setup HFCLK because RC oscillator is automatically started/restarted when no peripheral require HFCLK. E.g. in sleep mode.

#### Timer ####
The choosing of timer is quite simple because nRF51 has two timer peripherals:
  * Timers/Counters (TIMER): The timer/counter runs on the high-frequency clock source (HFCLK) and includes a 4 bit (1/2X) prescaler that can divide the HFCLK. Typical current consumption is 30 uA. Switch-on the high frequency clock also in sleep mode mean a constant high current consumption.
  * Real Time Counter (RTC): The Real Time Counter (RTC) module provides a generic, low power timer on the low-frequency clock source (LFCLK). The RTC features a 24 bit COUNTER, 12 bit (1/X) prescaler, capture/compare registers, and a tick event generator for low power, tickless RTOS implementation. Typical current consumption is only 0.1 uA and require only low frequency clock that's already running in sleep mode, thus this is the right timer!
The schematic of RTC is shown below:

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/RTC_schematic.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/RTC_schematic.png)

Code initialization for RTC is void as the default values of RTC peripheral matched the requisites. I.e. Prescaler is set to zero (RTC frequency is 32768 kHz), all events are disabled, etc. Thus `RTC0_init()` function do nothing.
Once started, I chose to never stop the RTC0 counter as it drawn only 0.1 uA of current and because stopping and restarting RTC mean add some jitter/delay to counting (see paragraph 18.1.8 of [nRF51 Series Reference Manual V2.1](https://www.nordicsemi.com/eng/nordic/download_resource/20337/10/53074698)). This complicate precise delta time measurement. Only difference is an absolute value subtraction between actual counter value and previous counter value must be done in order to compute a positive delta-time.


---


# The algorithm development: measure bicycle's speed by measuring delta-time among two pulses #

Following flow graph describe the algorithm to compute speed in km/h by measuring delta-time among two pulses at AIN4 (LPCOMP) port.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/ComputeSpeed_Algorithm.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/ComputeSpeed_Algorithm.png)

Below the C code of above flow graph.

```
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
			
			//Print to SEGGER_RTT debugger tool the “speed”
			/*++++++++++++++++++++++++++++___START-OF-DEBUG-CODE___+++++++++++++++++++++++++++++++++++++*/
			//
			//SEGGER_RTT_WriteString(0,"Hello World from SEGGER RTT!!\r\n");
			SEGGER_RTT_printf(0, "debug_cnt=%d\r\n", debug_cnt++);
			SEGGER_RTT_printf(0, "DeltaTime=%u\r\n", deltatime);
			SEGGER_RTT_printf(0, "Speed_kmh=%.2u\r\n", speed_kmh);
			//
			/*++++++++++++++++++++++++++++++___END-OF-DEBUG-CODE___+++++++++++++++++++++++++++++++++++++*/
		}
		
	

	}
/*+++++++++++++++++++++++++++++++++++___END-OF-INFINITE-LOOP___+++++++++++++++++++++++++++++++++++*/
```

where `SPEED_KMH()` depends on tyre circumference and RTC resolution. It was built using the following macros:
```
/***************************************************************************************************
*_____________________________________________MACRO_________________________________________________
***************************************************************************************************/
#define TYRE_CIRCUMFERENCE_M 2070E-3
#define RTC0_RESOLUTION_S 30.517E-6
#define SPEED_MS(dt) ((TYRE_CIRCUMFERENCE_M)/(dt*RTC0_RESOLUTION_S))
#define SPEED_KMH(dt) (SPEED_MS(dt)*3.6)			
```

Thanks to [SEGGER RTT](https://www.segger.com/jlink-real-time-terminal.html) functionality of the build-in debugger, it's easy to print the variables value to the terminal (I'm using [PuTTY](http://www.putty.org/)).

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/terminal_speed.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/terminal_speed.png)

<a href='http://www.youtube.com/watch?feature=player_embedded&v=1cFkiIV8wbg' target='_blank'><img src='http://img.youtube.com/vi/1cFkiIV8wbg/0.jpg' width='425' height=344 /></a>

In video, three new lines appear each time the button is pressed that is each time the bicycle's wheel rotates at 360'. The lines are:
  * debug\_cnt=x: it's a counter of occurred interrupt;
  * DeltaTime=y: it's the elapsed time among two pulses (interrupts), expressed in tick. 1 tick= 30.517E-6 s
  * Speed\_kmh=z: it's the estimated speed in km/h, rounded to unit.
The button was pressed with at different time. The time elapsed between two occurring button presses is correctly calculated and the speed in km/h corresponds to this delta-time.
In time elapsed between two pulses the CPU should be sleeping as a "Waiting For Interrupt" (`__WFI()`) instruction is executed. A precise current measurement should reveal it.

Firmware is located in [/svn/tags/3a\_compute\_speed](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F3a_compute_speed%253Fstate%253Dclosed).

---


# Current consumption on nRF51 #
Current consumption on nRF51822 in ON-mode, during CPU sleeping should be given by:

  * SYSTEM-ON base current with 16 kB RAM enabled (I use only 8 kB but this spec. is not available), I\_ON = 2.6 uA.
  * Real Time Counter (RTC) current, I\_RTC = 0.1 uA.
  * Run current for 32.768 kHz crystal oscillator, I\_X32k = 0.4 uA
  * Run current for LPCOMP, I\_LPC = 0.5 uA.
Thus, total current consumption in ON mode (CPU sleeping) should be: I\_ON + I\_RTC + I\_X32k + I\_LPC = 2.6 + 0.1 + 0.4 + 0.5 = 3.6 uA

While the total current consumption in ON-mode when the CPU is running, is given by:
  * Total current consumption in sleep mode I\_TOT\_SLP = 3.6 uA
  * Run current for 16 MHz RC oscillator, I\_RC16M = 750 uA
  * Run current at 16 MHz executing code from flash memory, I\_CPU\_FLASH = 4.4 mA
Thus, total current consumption in ON mode (CPU running) should be: I\_TOT\_SLP + I\_RC16M + I\_CPU\_FLASH = 3.6 + 750 + 4400 = 5.154 mA


## Circuit setup to measure low current ##
Measuring such low current is a challenge with traditional instruments and some high performances current sense amplifier are needed. However, without these, it's possible to perform some quasi-static measurement. In detail the measurement is split in two state: CPU on mode and CPU in sleep mode. No transition among these two states are evaluated.

Evaluation board is not powered by USB but by a special 1.8 V voltage regulator based on famous LM317. This copy the real conditions of the working environment.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/PreciseVoltegeRegulator_circuit.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/PreciseVoltegeRegulator_circuit.png)

### Connection to evaluation board (PCA-10001) ###
Current consumption measurement are provided within the board (see [Evaluation Kit User Guide](https://www.nordicsemi.com/eng/user/login/?next=Products/Bluetooth-Smart-Bluetooth-low-energy/nRF51822-Evaluation-Kit?resource=18974) paragraph 5.1.11) however, to correctly measure current consumption, a 0402 smd 10 ohm resitor must be fitted. I've not such a small resistor.
Besides, since I need to power the circuit from an external 1.8 V regulator, I chose to insert an ammeter (my DMM UT-61E) in the power supply chain. Thus, connection to PCA-10001 board are at pin 2 (1.8 V) and pin 4 (GND) of header P1 unlike connections suggested on EK User Guide.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/PCA10001_power_supply_circuitry.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/PCA10001_power_supply_circuitry.png)

Connections to PCA10001 board are shown below where P1.2 and P1.4 are pin 2 and pin 4 of pin-header P1:

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/current_measurement_circuitry.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/current_measurement_circuitry.png)

This is because i want to supply 1.8 V directly to nRF51 chip, without any protection diode inserted. In fact, voltage drop across diode lead to power the nRF51822 with lower voltage and power measurement will be wrong.

## Measurement and comparison of energy consumption among CPU-active-mode and CPU-sleep-mode ##
In order to measure current consumption, a new firmware was build. In fact, as described, measure such a low current with high dynamic range is a hard challenge with hobbyist tools. The application will stay in ON-mode all time but, every time the external button is pressed, the state will change among CPU-sleeping and CPU-ON. Current measurement in this two states are performed with a digital multimeter (UNI-T UT61-E). Measurement resolutions are:
  * 10 nA with an accuracy of (0.5% + 10 counts) when measuring micro-ampere i.e. in CPU-sleep mode;
  * 1 uA with an accuracy of (0.5% + 10 counts) when measuring milli-ampere i.e. in CPU-ON mode;

Real measurement are:

  * **System-ON, CPU ON**: 4.319 ± 0.032  mA (estimated current consumption was 5.154 mA);

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/3_current_measurement_ON.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/3_current_measurement_ON.png)

  * **System-ON, CPU sleep (Low Power mode)**: 3.05 ± 0.116 uA (estimated current consumption was 3.6 uA);

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/3_current_measurement_sleep.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/3_current_measurement_sleep.png)

Test firmware is located in [svn/branches/3b\_current\_measurement](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Fbranches%2F3b_current_measurement).

---
