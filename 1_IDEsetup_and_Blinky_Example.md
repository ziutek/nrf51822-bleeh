# Introduction #
Here are described the step to follow in order to setup the IDE and run the first example on the nRF51822\_EK (PCA100001).
## IDE & tools setup ##

Using the Evaluation Kit (PCA10001), you must:
  1. Download and install Keil MDK-ARM Lite from [here](https://www.keil.com/demo/eval/arm.htm) to your hard drive. Keil downloads to c:\Keil\unless you change the location when installing.
  1. Download and run the J-Link Software and documentation pack for Windows from [here](http://www.segger.com/jlink-software.html). The serial number from your SEGGER J-Link hardware is needed to identify your device.
  1. Register an account on [http://www.nordicsemi.com](http://www.nordicsemi.com) and enter the product key of your evaluation board. This make it possible to download documents and the nRF51 SDK installer. Be sure to download the installer and not the zipped folder, this avoid annoying setup of the SDK. Finally run the SDK installer.
In this tutorial I've used nrf51 SDK v.6.0.0.43681.

More information on setup [here](https://www.nordicsemi.com/eng/user/login/?next=Products/Bluetooth-Smart-Bluetooth-low-energy/nRF51822-Evaluation-Kit?resource=18974) (you must be registered on Nordic Semiconductor's website).

## Hot to download the example code into the board ##

  1. Make sure that KEIL uVision is correctly setted: open the application and click on _File>Device Database.._.You must see the nRF51822 device in the list. ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/DeviceDatabase.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/DeviceDatabase.png)
  1. Once uVision is ok, let's to locate the Blinky project under _<keil path>\ARM\Device\Nordic\nRF51822\Board\PCA10001\blinky\_example\arm_.
  1. Open the Blinky project in Keil μVision by double clicking the _blinky.uvprojfile_.
  1. Select nRF51822from the Select Target list and click Build or press F7 to compile the Blinky project.
  1. Click the Load icon ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/downloadButton.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/downloadButton.png) to download and run the Blinky example firmware. LED 0and LED 1on the PCA10001 should now blink sequentially.

## Debug mode ##
The debug mode, in KEIL uVision, is very complete and full of features. It allow, in combination with the on-board & build-in Segger J-Link, to debug the application right in the evaluation board with different views such as disassebly window, register window, performance analyzer, logic analyzer, instruction trace, etc..
Entering in debug mode is very simple: just click on the start/stop debug session button or press Ctrl+F5 on keyboard.
![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/debug_button.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/debug_button.png)
In debug tab, there are the command to run the application, run it step-by-step, to place/remove the breakpoints, etc.
Likewise, in the tab Peripherals->System Viewer->... there are the peripherals registers viewers. Select the one you're interested in to show the window.
The following image show the GPIO out register during debugging the Blinky project on nRF51822-EK board.
![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/debug_GPIO.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/debug_GPIO.png)

At that time the LED 1 was ON. Notice the corresponding PIN19 was high (1).