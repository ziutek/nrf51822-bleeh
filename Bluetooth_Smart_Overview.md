

# Introduction #
![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/Bluetooth_Smart_Logo.svg.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/Bluetooth_Smart_Logo.svg.png)

Bluetooth standard is a standard for personal area network allowing short range and low power. Effective ranges varies from 1 meter to 100 meter.
Bluetooth uses a radio technology called frequency hopping spread spectrum (FHSS) allowing to make a very robust link. It operates in the ISM 2.4 GHz band.
This is a brief video by Nordic Semiconductor introducing Bluetooth Smart.

<a href='http://www.youtube.com/watch?feature=player_embedded&v=BZwOrQ6zkzE' target='_blank'><img src='http://img.youtube.com/vi/BZwOrQ6zkzE/0.jpg' width='425' height=344 /></a>

Since 1999 the SIG (Special Interest Group) has released 4 different major specifications:
  * BR (specification 1.1): Basic Rate, 1 Mbit/s;
  * EDR (specification 2.0): Enhanced Data Rate, 2 and 3 Mbit/s;
  * HS (specification 3.0): High Speed (Alternate MAC/PHY);
  * LE (specification 4.0): Low Energy, 1 Mbit/s, ultra low power;
Bluetooth low energy is not backward compatible with the older version thus SIG introduces two new terms for separate them:
  * Bluetooth Smart (4.0): Single Mode, LE only radio;
  * Bluetooth Smart Ready (4.0): Dual mode, BR/EDR and LE dual radio;

The nRF51822 support only Bluetooth low energy mode.

Bluetooth Low Energy is particularly suitable for low energy because it uses small packet to transmit information (only 27 bytes), has short TX and RX windows, there is always a race to idle (turn the radio ON as seldom as possible and turn OFF the radio as soon as possible). A connection is accomplished in only 6 ms with fast tear down (hundred of ms with Bluetooth classic). It also has a low memory footprint (8 kB RAM on nRF51822). Finally the protocol is stateless creating every request as independent transaction.

## BLE profiles ##
Profiles are definitions of possible application and specifies general behaviors that devices can use to communicate. Some examples are:
  * Hid over GATT: human Interface Device

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/pc-mouse_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/pc-mouse_BLE.png)

  * Heart Rate: Used in sport and medicine

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/HRMBelt_Smartphone_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/HRMBelt_Smartphone_BLE.png)

  * Proximity: proximity monitoring of two device

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/keyfob-tablet_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/keyfob-tablet_BLE.png)

In detail, profiles are specifications that describe two or more devices with one or more services on each device. Profiles also describe how the device should be discoverable and connectable trough the GAP and GATT features. A profile also describe what services are required in each device. The application can implement one of the standard profiles defined by Bluetooth SIG or one can implement a proprietary profile creating your own specification for device behavior and included services.

## Bluetooth Low Energy architecture ##

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/BLE_architecture.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/BLE_architecture.png)

Bluetooth Low Energy has tree main part: the controller, the host and the application.

### The Controller ###

Is the physical device that can transmit and receive radio signals and understand how the signals can be interpreted. It is composed by:
  * **Physical Layer (PHY)**, the nRF51 series radio module;
  * **Link Layer (LL)**, the most complex part of BLE. It has 5 main states:Standby, Advertising, Scanning, Initiating, Connection. **-Advertising**: the device is transmitting advertising packets; the only way a device can be discovered in BLE is if it is advertising and to connect to a device it must be discovered first. Advertising mode is also used for broadcasting data. **-Scanning**: the device scans for advertising packets. Are possible _passive scanning_ (only receive adv. data) and _active scanning_ (scanner requests are sent to adv. devices to obtain additional scanner response data. **-Initiating** state, the device scans for advertising packets and then sends out a connect request to the advertiser. The device will then move to the connection state; **-Connection** state, has two sub-states: master and slave;

Packet structure is composed by two types of packets: Advertising packets understandable by all devices and Data packets understandable only by 2 devices, master and slave.
BLE utilizes 40 channel separated by 2 MHz (instead of the classic 1 MHz) : 3 advertising channels and 37 data channels.
Link layer is also responsible for establishment and management of connections. An example is shown below.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/adv_scan_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/adv_scan_BLE.png)

The two BLE devices in above image are: an advertiser, that is a device with the link layer in the advertising state (it sends out advertising packets) and a scanner, that is a device in scanning state (maybe active or a passive scanner which receive only adv. packets).

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/adv_init_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/adv_init_BLE.png)

A device in the initiating state that receive advertising packets could send a connection request to advertiser and move into the connection state.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/slave_master_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/slave_master_BLE.png)

thus the devices becomes master and slave.

The advertising data is send over three different channels named advertising channels. They're channel 37, 38 and 39, located at 2402 MHz, 2426 MHz and 2480 MHz to be immune to domestic Wi-Fi interference. Advertising intervals can be set in a range of 20 ms to 10 s. It specifies the interval between consecutive advertising packets. For scanner mode, BLE specifies a scan window, which determines the duration of the scan as well as the scan interval, meaning how often it repeats. Here an example.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/scanning_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/scanning_BLE.png)

Above is shown as example how the advertising and scanning work. The device in advertising mode (in image) has an advertising interval of 20 ms, and it send one advertiser packet on channel 37, 38 and 39 (all advertising channels). The device in scanning mode has a scan interval of 50 ms and a scan window of 25 ms. It start scanning at 0 ms and scan on channel 37 until 25 ms. It receive two packets in this channel. Afterwards it shut-down radio for 25 ms. Then the scanner jump to channel 38 and scan there for another 25 ms; the same with channel 39 and so on, restarting with channel 37.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/advertising_rxPeriod_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/advertising_rxPeriod_BLE.png)

As shown in detail in above image each advertiser, after sent an advertising packet, has a receive window (RX period) in which it listen for connection request. If a connection request is received on that channel, it acts as shown below.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/advertising_ConnDONE_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/advertising_ConnDONE_BLE.png)

Previous image represent the situation where there is an initiator instead of a scanner and it want to establish a connection. The initiator, after received an advertising packet, send a connection request (CREQ) on the same channel (ch. 38) during RX period. If a connection request is received, advertiser stop immediately. this is followed by a mandatory delay of 1.25 ms and a transmit window offset; then the transmit windows is opened and slave turns on its receiver and waits for a packet from the master. If it receives a packet, the slave responds, and a connection is estabilished.

When a connection is established, the initiator device (which will become a master) supplies the advertising device (which will become a slave device) with a set of critical data defining the channel and timing of the master-slave data exchange. In particular, this data specifies two important parameters: connection interval and slave latency. The connection interval determines the time between the start of the data packet exchange sequence called connection events. Latency, on the other hand, is a number of communication intervals that a slave may ignore without losing the connection. It gives the slave an opportunity to optimize and preserve power consumption. Once a connection is established, the slave may request an update of communication parameters to better fit the slave application. This is shown in below image.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/Conn_timings_M-S.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/Conn_timings_M-S.png)

The connection interval can be from 7.5 ms to 4 seconds. Multiple packets can be sent every connection interval (6 maximum).

Each communication event is started by the master transmission and it serves as an anchor point to calculate the time for the next event. During a communication event, the master and slave alternate sending and receiving packets until either side stops sending packets. The current event is considered closed and the data exchange is suspended until the next communication event.

### The Host ###
The Host is a software stack: manage how two or more devices communicate each others. It is composed by following blocks:

#### Logical Link Control and Adaptation Layer Protocol (L2CAP) ####

Provides data services to upper layer protocols like security manager protocol and attribute protocol. It is responsible for protocol multiplexing and data segmentation into smaller packets for the LL controller, and de-multiplexing and reassembly operation on the other end. The L2CAP is a backend interface for the generic access profile (GAP) that defines the generic procedures related to the discovery of BLE devices and link management aspects of connecting to other LE devices.  The GAP provides an interface for the application to configure and enables different modes of operation, e.g. advertising or scanning, and also to initiate, establish, and manage connection with other devices.

#### Security Manager (SM) ####

Is responsible for device pairing and key distribution. The security manager protocol (SMP) is defined as how the device’s SM communicates with its counterpart on the other device.  The SM provides additional cryptographic functions that may be used by other components of the stack.

#### Attribute Protocol (ATT) ####

Define a set of rules for accessing data on a device (the server). Defines how a client can find and access the attributes on a server. Allows an attribute server to expose a set of attributes and their associated values to an attribute client. These attributes can be discovered, read, and written by peer devices. It is structured in six basic operations: request, response, command, indication, confirmation, notification.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/attribute_operations_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/attribute_operations_BLE.png)

The client sends a **request** to the server that the server has to do something. The server sends back a **response**.
The client can also sends commands to the server. **Commands** are used when a clients wants the server to do something but it doesn't need an immediately response.
**Indications** are send by a server to a client to inform it that a particular attribute has a given value. Indications are similar to requests because indications needs a confirmation.
The server sends **notification** to the client to inform it that a particular attribute has a given value. Notifications are similar to commands because they don't need a response.


An **attribute** can be look that as a piece of labelled addressable data. An attribute has four values associated with it:
  * handle: uniquely identifies an attribute on a server allowing the client to reference it;
  * type: specifies what the attribute represents defined by universally unique identifier (UUID).  A UUID is a 128-bit number that is globally unique and is used to identify an attribute type;
  * value: is the state data, the actual data [0-512] bytes.
  * permissions: let us know if the attribute can be readable, writable or both.
Image below shown the architecture of four different attributes, note that each row corresponds to one attribute.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/attribute_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/attribute_BLE.png)

Services, characteristic and descriptor are explained in detail below.

#### Generic Attribute Profile (GATT) ####

Defines a hierarchical grouping of attributes and procedures for discovery and access. It interfaces with the application through the application’s profiles. Generic Attribute Profile (GATT) is built on top of the Attribute Protocol (ATT) and establishes common operations and a framework for the data transported and stored by the Attribute Protocol. GATT defines two roles: Server and Client. The device holding data is the GATT Server, while the device accessing it is the GATT Client.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/GATT_hierarchy_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/GATT_hierarchy_BLE.png)

The GATT **Profile** specifies the structure in which profile data is exchanged (see above image). This structure defines basic elements such as services and characteristics, used in a profile. The top level of the hierarchy is a profile. A profile is composed of one or more services necessary to fulfill a use case. E.g. Heart rate profile requires the sensor to have Heart Rate Service and Device Information Service. A profile is just defined a set of services.

A profile document includes information on services that are required or optional for that particular profile as well as how the peers will interact with each other. This includes both which GAP and GATT Roles the devices will be in during data exchange. Therefore, this document will often contain information on what kind of advertising and connection intervals should be used, whether security is required, and similar.

A GATT Server organizes data in what is called an attribute table and it is the attributes that contain the actual data, following there is an example:

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/GATT_attributetable_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/GATT_attributetable_BLE.png)

It should be noted that a profile does not have an attribute in the attribute table.

Each row of previous table is an **attribute**; as was described in ATT section, attribute has a handle, a UUID, and a value. A handle is the index in the GATT table for the attribute and is unique for each attribute in a device. The UUID contains information on the type of data within the attribute, which is key information for understanding the bytes that are contained in the value of the attribute. There may be many attributes in a GATT table with the same UUID.

A **characteristic** consists of at least two attributes: a characteristic declaration (the light-blue row in the table below) and an attribute that holds the value for the characteristic (the white row in the table below). It could optionally contain descriptors that describe the value or permit configuration of the server with respect to the characteristic value.
All data that will be transferred through a GATT Service must be mapped to a set of characteristics. It is a good idea to consider bundling the data up so that each characteristic is a self-contained, single instance data point. For example, if some pieces of data always change together, it will often make sense to collect them in one characteristic.
A characteristic is a value used in a service along with properties and configuration information about how the value is accessed and information about how the value is displayed or represented.
A characteristic declaration (the light-blue row in the table below) contains the characteristic properties, the handle of the characteristic value and the UUID.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/GATT_characteristic_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/GATT_characteristic_BLE.png)

The characteristic properties determine if the characteristic value can be read, written, notified, indicated or broadcast-ed. Be aware that some of the properties requires certain descriptors to be available.
The handle of the characteristic value is the handle of the attribute that contains the characteristic value.
The UUID in the characteristic declaration must be the same UUID that holds the characteristic value.

The characteristic value (the white row in the above table) is just an ordinary attribute.

Any attribute within a characteristic definition that is not the characteristic value is, by definition, a **descriptor**. A [descriptor](https://developer.bluetooth.org/gatt/descriptors/Pages/DescriptorsHomePage.aspx) is an additional attribute that provides more information about a characteristic, for instance a human-readable description of the characteristic. See fourth row on table below. The descriptor associate an human readable text string ("Bedroom") to related characteristic

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/GATT_characteristic_Descriptor_CCCD_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/GATT_characteristic_Descriptor_CCCD_BLE.png)

However, there is one special descriptor that is worth mentioning in particular: the Client Characteristic Configuration Descriptor (**CCCD**). This descriptor is added for any characteristic that supports the Notify or Indicate properties. A CCCD is in the last row of the above table.
Writing a ‘1’ to the CCCD enables notifications, while writing a ‘2’ enables indications. Writing a ‘0’ disables both notifications and indications.

A **service** consists of one or more characteristics, and is a logical collection of related characteristics. GATT services typically include pieces of related functionality - such as a particular sensor’s readings and settings or the inputs and outputs of a Human Interface Device. Organizing related characteristics into services is both useful and practical, since it promotes a clearer separation based on logical and use-case defined boundaries and helps code reuse across different applications. The GATT-based SIG profiles and services make good use of this approach, and it is recommended to follow their strategy for user-defined profiles.

GATT procedures can be spitted into three types:
  * _Discovery_ : used to discover the services, the characteristics and the attributes on a server;
  * _Client initiated_ : used to read or write the attributes on a server;
  * _Server initiated_ : used to send data without a client asked for it directly.

Minimum number of services on a server is two:
  * Generic Access Profile (GAP) service;
  * Generic Attribute Profile service (GATT);
These uses six attributes (note the GATT service has no characteristics):

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/minimum_requisites_BLE.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/minimum_requisites_BLE.png)

#### Generic Access Profile (GAP) ####

Defines the generic procedures related to the discovery of BLE devices and link management aspects of connecting to other LE devices.  The GAP provides an interface for the application to configure and enables different modes of operation, e.g. advertising or scanning, and also to initiate, establish, and manage connection with other devices.
GAP defines four roles:
  * Broadcaster: transmits advertising packets, it's never in a connection.
  * Observer: scans for advertising packets, it's never in a connection.
  * Peripheral: transmits advertising packets and has a slave role in a connection.
  * Central: scans for advertising packets and initiate a connection; it has a master role in a connection.

There are also a number of modes and procedures defined within the General Access Profiles. The modes describe how the device can be discovered and connected to and if it can be bonded with. The procedures are connected to the different modes.

## Application profiles and services ##

To use Bluetooth® wireless technology, a device must be able to interpret certain Bluetooth profiles. Bluetooth profiles are definitions of possible applications and specify general behaviors that Bluetooth enabled devices use to communicate with other Bluetooth devices. There is a wide range of Bluetooth profiles describing many different types of applications or use cases for devices. By following the guidance provided by the Bluetooth specification, developers can create applications to work with other Bluetooth devices.
The application profile defines the collection of attributes and any permission needed for these attributes to be used in the communication.

# Bibliography #
  1. [Bluetooth Developer Portal](https://developer.bluetooth.org)
  1. [Bluetooth 4.0: An introduction to Bluetooth Low Energy—Part I](http://www.eetimes.com/document.asp?doc_id=1278927), Mikhail Galeev, Z-Focus Consulting.
  1. [Bluetooth 4.0: An introduction to Bluetooth Low Energy—Part II](http://www.eetimes.com/document.asp?doc_id=1278966), Mikhail Galeev, Z-Focus Consulting.
  1. [nAN-36\_v1.1-Creating Bluetooth low energy applications using nRF51822 Application Note v1.1](https://www.nordicsemi.com/eng/nordic/download_resource/24020/5/78061434),  Nordic Semiconductor.