

# Introduction #
To start with Bluetooth Smart, a simple but complete application was provided by Nordic Semiconductor. It can be found on GitHub, at [https://github.com/NordicSemiconductor/nrf51-ble-app-lbs](https://github.com/NordicSemiconductor/nrf51-ble-app-lbs). [Here](http://www.nordicsemi.com/eng/content/download/34055/573345/file/nAN-36_v1.1.pdf) is available the related application note (nAN-36).
Minimum requisites to understand the application is to have some knowledge about [Bluetooth Smart technology](https://code.google.com/p/nrf51822-bleeh/wiki/Bluetooth_Smart_Overview).

# The Led Button application #
The LED Button application example was created in order to give you an environment where you can learn how to use Bluetooth low energy on the nRF51822 chip. It is a simple BLE application that demonstrates bidirectional communication over BLE. When it is running, you will be able to toggle an LED output on the nRF51822 chip from the Central, and receive a notification when button input on nRF51822 is pressed.
The application is implemented in one service, with two characteristics—the LED characteristic, which can be used to control the LED remotely through the write without response operation, and the Button characteristic, which sends a notification when the button is pressed or released.

## Download firmware into evaluation board PCA10001 ##
There are some simple steps to follow. Note that I've SDK v6.0.0 and SoftDevice S110 v7.1.0.
  1. **Install** following Nordic Semiconductor software: [Studio](nRFgo.md) and [Control Panel](Master.md). Download SoftDevice BLE protocol stack solution.
  1. **Download** last SoftDevice sw stack on USB dongle (PCA10000): open nRFgo Studio and in device manager menu select nRF51 development dongle and its embedded J-Link programmer. Then select "Program SoftDevice" tab and browse for S110 hex file. Using S110 v7.1.0 its name is _s110\_nrf51822\_7.1.0\_softdevice.hex_. Finally click on _Program_. ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/SD_download_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/SD_download_BLE.png)
  1. **Download** application FW into usb dongle (PCA10000): open Master Control Panel and click on File->Flash programming... then select last FW (_MEFW nRF51822 0.10.0_).Finally click on _Program_. Now usb dongle is ready to work with Master Control Panel. ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/App_download_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/App_download_BLE.png)
  1. **Download** last SoftDevice sw stack on evaluation board (PCA10001): open nRFgo Studio and in device manager menu select nRF51 evaluation boards and its embedded J-Link programmer. Then select "Program SoftDevice" tab and browse for S110 hex file. Using S110 v7.1.0 its name is _s110\_nrf51822\_7.1.0\_softdevice.hex_. Finally click on _Program_.
  1. **Setup** proper Flash and RAM start addresses and size. On KEIL uVision, open _ble\_app\_lbs.uvproj_ then go to Flash->Configure Flash Tools... and go to target tab. Check if the IROM1 and IRAM1 parameters match with following and, if not, change them. ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/IRAM_IROM_settings_SD.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/IRAM_IROM_settings_SD.png) Then click on _Debug_ tab, and on settings of J-LINK programmer. In new window select _Flash Download_ tab and check if the RAM for Algorithm is properly setted to Start: 0x20000000 and Size: 0x2000, like following image. If mismatch, change it with correct values. ![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/jlink_RAM_SD.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/jlink_RAM_SD.png)
  1. **Download** application FW into usb evaluation board (PCA10001): in KEIL, build the project (press F7) and download it into evaluation board.
Now the application is running on evaluation board. Push reset button to reset the device. You must see the LED 0 on, indicating the device is in advertising mode.

## Application analysis ##
The LED Button application example was created in order to give you an environment where you can learn how to use Bluetooth low energy on the nRF51822 chip. It is a simple BLE application that demonstrates bidirectional communication over BLE. When it is running, you will be able to toggle an LED output on the nRF51822 chip from the Central, and receive a notification when button input on nRF51822 is pressed.
The application is implemented in one service, with two characteristics—the LED characteristic, which can be used to control the LED remotely through the write without response operation, and the Button
characteristic, which sends a notification when the button is pressed or released.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/nRF_MCP_android_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/nRF_MCP_android_BLE.png)

The analysis is done with help of Master Control Panel on a PC or with a Bluetooth Smart ready smartphone and [nRF Master Control Panel App](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp) from Play Store.
On PC, connect either devices to USB and open Master Control Panel application. Select as master emulator the serial port corresponding to serial number of embedded J-LINK of the USB dongle. In log table you must see a message claims the server has started.
Clicking on Start discovery button the discovered device table will populate with the "LedButtonDemo" device information. This is the advertising data of evaluation board PCA10001.
Now click on _Select device_ then on _Connect_ and on _Discover services_. You'll see the fundamentals primary services, GAP and GATT, together with the main service of the LedButtonDemo application. Grouped under services there are the corresponding characteristics declaration, characteristic definition (values) and some characteristic descriptors.
Below are described these attributes.

### Services ###
As specified [here](https://code.google.com/p/nrf51822-bleeh/wiki/Bluetooth_Smart_Overview#Generic_Attribute_Profile_(GATT)), in order to establish a connection, minimum number of services is two: GAP service (0x1800) and GATT service (0x1801) (highlighted in yellow in figure below).

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/LedButtonExample_attributes_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/LedButtonExample_attributes_BLE.png)

Furthermore, an additional service (highlighted in green) is provided to include application data. This service is implemented in two files: 'ble\_lbs.c' and its header 'ble\_lbs.h'.

#### Characteristics ####

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/GAP_characteristics_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/GAP_characteristics_BLE.png)

As shown in the image above, into GAP service (0x1800), are provided 3 different characteristics declarations and its related values:
  1. Device Name Characteristic (UUID=0x2A00): contain the name of the device as an UTF-8 string; see Bluetooth Core Specification Volume 3, Part C, Section 12.1.
  1. Appareance (UUID=0x2A01): defines the representation of the external appearance of the device. This enables the discovering device to represent the device to the user using an icon, or a string, or similar; see Bluetooth Core Specification Volume 3, Part C, Section 12.2.
  1. Peripheral Preferred Connection Parameters (UUID=0x2A04): contains the preferred connection parameters of the Peripheral; see Bluetooth Core Specification Volume 3, Part C, Section 12.5.

while GATT service has no characteristic.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/CustomService_characteristics_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/CustomService_characteristics_BLE.png)

Last service, instead, has 2 characteristics and one descriptor. Since it is a custom attributes, a full 128-bit UUID should be used.
  1. Led characteristic (UUID=0x...1525...): The LED state characteristic needs to be writable and readable, without any notification.
  1. Button state characteristic (UID=0x...1524...): The button characteristic notifies on button state change, but also allows the peer device to read the button state. To implement notification, must be included another attribute, the Client Characteristic Configuration Descriptor (CCCD) and its value must be set to 1 (see [here](https://code.google.com/p/nrf51822-bleeh/wiki/Bluetooth_Smart_Overview#Generic_Attribute_Profile_(GATT))). In above image application, button changes don't perform notify since CCCD value is 0x00 (see fourth line).

The SoftDevice organizes UUIDs in a similar way to how the Bluetooth Core Specification defines the Bluetooth SIG UUIDS, that is, you add a custom base UUID and then define 16-bit numbers, similar to the aliases, to be used on top of this base. It’s easiest to use one base UUID for all custom attributes, at least within the same service. For the LED Button example, 0x0000xxxx-1212-EFDE-1523-785FEABCD123 will be used as the base, while 0x1523 is used for the service, 0x1524 for the LED characteristic, and 0x1525 for the button state characteristic.

## Firmware analysis ##

The example code is divided into three files:
  * main.c
  * ble\_lbs.c
  * ble\_lbs.h
The structure is the same as in other SDK examples, with main.c implementing application behavior and the separate service files implementing the service and its behavior. All I/O handling is left to the application.

The basic flow of a typical BLE application running on the nRF51822 chip is to initialize all needed parts, start advertising, and possibly enter a power-saving mode and wait for a BLE event. When an event is received, it is passed to all BLE services and modules. An event can be, but is not restricted to, one of the following:
  * When a peer device connects to nRF51822.
  * When a peer device writes to a characteristic.
  * An indication that advertising has timed out.
This flow makes the application very modular and a service can usually be added to an application by initializing it and ensuring its event handler is called when an event comes in.

# Final considerations #
Starting from this example could be developed the final "BLEEH" application to sense bicycle's speed and send speed-data to external devices such as Bluetooth Smart Ready smartphones.
Probably I need to use the Button characteristic and remove LED characteristic besides to implement tips suggested on wiki page [Bluetooth\_Smart\_and\_Low\_Power\_considerations](https://code.google.com/p/nrf51822-bleeh/wiki/Bluetooth_Smart_and_Low_Power_considerations) of this project.