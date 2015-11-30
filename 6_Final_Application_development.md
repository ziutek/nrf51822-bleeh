![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/work-in-progress_BIG.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/work-in-progress_BIG.png)



# Introduction #
Here are described the modifies to nrf51-ble-app-lbs-master code available on [GitHub](https://github.com/NordicSemiconductor/nrf51-ble-app-lbs) and described [here](https://code.google.com/p/nrf51822-bleeh/wiki/4_First_Bluetooth_Low_Energy_application). Since the example's code is very similar final application code, some changes were done to the example's application in order start with a fully working code. Main changes are related to deleting unnecessary BLE-GATT characteristics, add useful speed data to transmission task and adapt connection parameters to the desired one.
In detail, main modifies to actuate are:
  1. delete the LED characteristic and others minor changes;
  1. modify the BUTTON characteristic replacing button status data with custom data. This data will be replaced by actual speed data in step 3.
  1. add LPCOMP functionality and measure speed with help of RTC and LFCLK using code of [2b\_Development\_of\_Comparator\_Driver](https://code.google.com/p/nrf51822-bleeh/wiki/2b_Development_of_Comparator_Driver) and [3\_Study\_and\_Development\_of\_UltraLowPower\_Techniques](https://code.google.com/p/nrf51822-bleeh/wiki/3_Study_and_Development_of_UltraLowPower_Techniques) wiki pages. Replace custom data sent in step 2 with actual speed data;
  1. in order to preserve energy, transmit data only when it's essential;
  1. delete all extra functionality minimizing current consumption (tips [here](https://devzone.nordicsemi.com/question/5186/how-to-minimize-current-consumption-for-ble-application-on-nrf51822/));

The code could be downloaded from GitHub into your PC. Note that in order to work properly, the code must be copied into a specific folder (../nrf51822/Board/pca10001/s110/), otherwise the header files and source files of the libraries will not found. However I copied such files in the project folder, into the "Include" and "Source" folders and I've modify some project's proprieties  to have portability feature.
To easily access to code, a stand-alone version is available here for download: [svn\tags\6\_Final\_Application\_development\_BaseCode](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F6_Final_Application_development_BaseCode)
All the mentioned steps are described below.

## 1-Deleting of the LED characteristic and other minor changes ##
The first simple modify is to change device's name. The file `main.c` at row 44 is modified as follow:
```
#define DEVICE_NAME                     "BLE Speed Sensor"                          /**< Name of device. Will be included in the advertising data. */
```
Then let's go to delete the led characteristic: it isn't necessary for our purposes.
Interesting files, as mentioned in the application note are 3:
  1. `main.c`
  1. `ble_lbs.h`
  1. `ble_lbs.c`


The structure is the same as in other SDK examples, with `main.c` implementing application behavior and the separate service files implementing the service and its behavior. All I/O handling is left to the application.


In `main.c`, can be deleted (or commented out) following code lines:
  * 
```
    nrf_gpio_cfg_output(LEDBUTTON_LED_PIN_NO);
```
  * 
```
    static void led_write_handler(ble_lbs_t * p_lbs, uint8_t led_state)
{
    if (led_state)
    {
        nrf_gpio_pin_set(LEDBUTTON_LED_PIN_NO);
    }
    else
    {
        nrf_gpio_pin_clear(LEDBUTTON_LED_PIN_NO);
    }
}
```
  * 
```
    init.led_write_handler = led_write_handler;
```
while in `ble_lbs.c` can be deleted following lines regarding the function for adding the new led characteristic to the existing GATT service:
  * 
```
/**@brief Function for adding the LED characteristic.
 *
 */
static uint32_t led_char_add(ble_lbs_t * p_lbs, const ble_lbs_init_t * p_lbs_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_lbs->uuid_type;
    ble_uuid.uuid = LBS_UUID_LED_CHAR;
    
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_lbs->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_lbs->led_char_handles);
}
```
together with following lines that really call the above-mentioned function.
```
    err_code = led_char_add(p_lbs, p_lbs_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
```
and the following lines that handle the LED writing event:
```
/**@brief Function for handling the Write event.
 *
 * @param[in]   p_sss       LED Button Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_sss_t * p_sss, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    
    if ((p_evt_write->handle == p_sss->led_char_handles.value_handle) &&
        (p_evt_write->len == 1) &&
        (p_sss->led_write_handler != NULL))
    {
        p_sss->led_write_handler(p_sss, p_evt_write->data[0]);
    }
}
```

and following case of switch-case structure, in the ble\_sss\_on\_ble\_evt() function that handle the BLE event that is the point in which the above on\_write() function is called:
```
        case BLE_GATTS_EVT_WRITE:
            on_write(p_sss, p_ble_evt);
            break;
```

No led-related functions are available to other c modules thus the file `ble_lbs.h` is unchanged.
Code for this step is here: [svn\tags\6\_1\_Final\_Application\_development\_step\_1](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F6_1_Final_Application_development_step_1).

## 2-modify the BUTTON characteristic replacing button status data with custom data ##

The service of the final application need only one characteristic that reflects the speed sensor data. In actual example, the button characteristic notifies on button state change, but also allows the peer device to read the button state. This is very similar to the behavior of the speed sensor. The button's status will be replaced with actual speed data, so I can base implementation of my custom code on this, following steps below:

  * Find the method called "button\_char\_add" and rename it to "speed\_sensor\_char\_add".
  * Rename the files `ble_lbs.c` into `ble_sss.c` and `ble_lbs.h` into `ble_sss.h`. The string sss stand for Speed Sensor Service.
  * Find all string containing `lbs` and and replace them with `sss`.

### Replace state data with custom data ###
To send some data via Bluetooth Low Energy, a simple modification to button characteristic was done. It consist on adding a new variable (a simple static counter) to example's code. Exactly in `ble_sss.c` file, into `ble_sss_on_button_change()` function.
When handling the button press, the example's application send a notification to the peer device with the new button state. The SoftDevice API method for doing this is called sd\_ble\_gatts\_hvx, and takes a connection handle and a ble\_gatts\_hvx\_params\_t structure as parameters. It then manages the process when a value is to be notified. In the ble\_gatts\_hvx\_params\_t structure, you set whether you want a notification or  indication, which attribute handle to be notified, the new data, and the new data length. The method will look as follows:

```
uint32_t ble_sss_on_button_change(ble_sss_t * p_sss, uint8_t button_state)
{
    ble_gatts_hvx_params_t params;
    //uint16_t len = sizeof(button_state);
    static uint8_t counter_to_send=0;				        //new variable to test custom data sending
    ++counter_to_send;							//increment the counter
    static uint16_t counter_to_send_len=sizeof(counter_to_send);	//lenght of the counter
    
    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;	                        //state here if you want notification or indication
    params.handle = p_sss->button_char_handles.value_handle;
    params.p_data = &counter_to_send;					//counter_to_send copied to hvx structure
    params.p_len = &counter_to_send_len;			        //counter_to_send_len copied to hvx structure
    
    return sd_ble_gatts_hvx(p_sss->conn_handle, &params);	        //issue the PDU
}
```

Previous code will notify a custom variable to peer device each time the counter has changed its value. The variable is static, thus its lifetime is the same of the entire application's one (in other words the counter has memory and only a board-reset can reset the counter's value).

### Why notification must be enabled by peer device? ###

As described in Bluetooth specification, notification and indication mode must be enabled by adding one more attribute: the client characteristic descriptor (CCCD). This characteristic must be set so one to enable notification (two for indication), as described [here](https://developer.bluetooth.org/gatt/descriptors/Pages/DescriptorViewer.aspx?u=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml). Furthermore, the default value for the Client Characteristic Configuration descriptor is 0x00. Upon connection of non-binded clients, this descriptor is set to the default value.
So it's a client's charge to write 1 to the CCCD value each time the devices start a connection.
The Master Control Panel does not automatically write to CCCDs unless you click the "enable services"-button or manually write e.g. 01-00 to the descriptor. The peer device that is interested in consuming the data would, on the other hand, automatically write to the descriptor to start getting data flowing.
Into Nordic Semiconductor Developer's forum there are some useful information: https://devzone.nordicsemi.com/question/14340/can-i-enable-notification-in-my-ble-peripheral/

### Update connection parameters (on start-up) to preserve energy decreasing data frame rate ###
[Connection parameters](https://code.google.com/p/nrf51822-bleeh/wiki/Bluetooth_Smart_Overview#The_Controller) are fundamentals in order to obtain minimum current consumption from device. This is very important in ultra-low-power device.
As explained [here](https://code.google.com/p/nrf51822-bleeh/wiki/Bluetooth_Smart_Overview#The_Controller), main connection parameters are three:

  1. Connection interval: Determines how often the Central will ask for data from the Peripheral. When the Peripheral requests an update, it supplies a maximum and a minimum wanted interval. The connection interval must be between 7.5 ms and 4 s.
  1. Slave latency: By setting a non-zero slave latency, the Peripheral can choose to not answer when the Central asks for data up to the slave latency number of times. However, if the Peripheral has data to send, it can choose to send data at any time. This enables a peripheral to stay sleeping for a longer time, if it doesn't have data to send, but still send data fast if needed.
  1. Connection supervision timeout: This timeout determines the timeout from the last data exchange till a link is considered lost. A Central will not start trying to reconnect before the timeout has passed, so if you have a device which goes in and out of range often, and you need to notice when that happens, it might make sense to have a short timeout.

In this application optimal connection parameters are due to a mix of feature.
Most important is low-power so it could be used a low frame-rate and a long supervisory timeout. However it's good to update speed-value with actual value every 500 ms.
This will be the value of connection interval: if the speed has an high change-rate, it will be transmitted a new speed-value every 500 ms. Instead, if speed is constant for a long period (if speed is low, this is often true), a new packet will be transmitted every (1 + connSlaveLatency) x connInterval= 4 s if the Slave\_Latency is set to 7. Connection supervision timeout (connSupervisionTimeout) is a parameter that defines the maximum time between two received Data Packet PDUs before the connection is considered lost. The connSupervisionTimeout shall be a multiple of 10 ms in the range of 100 ms to 32.0 s and it shall be larger than (1 + connSlaveLatency) x connInterval x 2. Empirically it seems 10 s is a good value.
Summarize, main connection parameters to be used are:
  * Connection interval = 500 ms;
  * Slave latency = 7;
  * Supervisory timeout = 10 s;

This allow to have a minimum transmitting period of 0.5 s if the speed change frequently and a maximum transmitting period of 4 s if the speed is quite constant. To change connection parameters simply overwrite following preprocessor's directive in main.c.

```
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (0.5 second). */
#define SLAVE_LATENCY                   7                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(10000, UNIT_10_MS)            /**< Connection supervisory timeout (10 seconds). */
```

On connection start-up, connection parameters are forced by central device so, in Master Control Panel we'll see following string in log tab:

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/MCP_init_connection_parameters.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/MCP_init_connection_parameters.png)

Therefore there must be a feature of the central device application to perform a connection with ad hoc parameter settings.
In peripheral application could be requested some parameter update trough the function [sd\_ble\_gap\_conn\_param\_update()](http://developer.nordicsemi.com/nRF51_SDK/doc/7.1.0/s110/html/a01049.html#ga6e9dce98f85415d2fc3c47f8a599f8fd) but it is up to the central to decide what the parameters should update to.
Otherwise, as in ble\_lbs example, a call to [sd\_ble\_gap\_ppcp\_set()](http://developer.nordicsemi.com/nRF51_SDK/doc/7.1.0/s110/html/a01049.html#ga8c6db7d9bc501929c9cc63b4b04fdfbb) function could be done. It updates some values in the GATT database that can later be read out by the central device after connecting. So, after some seconds, Master Control Panel show the successful of updating task:

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/MCP_update_connection_parameters.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/MCP_update_connection_parameters.png)

Useful information about connection parameters are available here:
  * https://devzone.nordicsemi.com/question/60/what-is-connection-parameters/
  * https://devzone.nordicsemi.com/question/22689/who-set-initial-connection-parameters-on-ble/
  * https://devzone.nordicsemi.com/question/12545/update-connection-parameter-programmatically/

All features described in this step, are showed in following video. See the custom data replacing the prior button-state data, observe the connection parameters are updating after some seconds from starting and note the enable services button needed to set to one the CCCD value: it allow the GATT server (the peripheral that is the speed sensor) to notify and must be set by the client.

<a href='http://www.youtube.com/watch?feature=player_embedded&v=OZw28PdlXQU' target='_blank'><img src='http://img.youtube.com/vi/OZw28PdlXQU/0.jpg' width='425' height=344 /></a>

Code for this step is here: [svn\tags\6\_2\_Final\_Application\_development\_step\_2](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F6_2_Final_Application_development_step_2).

## 3-Add LPCOMP functionality and measure speed with help of RTC ##
In order to measure bicycle's speed computing delta-time among two pulses, the low power comparator LPCOMP and a low power clock must be enabled. Code in [2b\_Development\_of\_Comparator\_Driver](https://code.google.com/p/nrf51822-bleeh/wiki/2b_Development_of_Comparator_Driver) and [3\_Study\_and\_Development\_of\_UltraLowPower\_Techniques](https://code.google.com/p/nrf51822-bleeh/wiki/3_Study_and_Development_of_UltraLowPower_Techniques) wiki pages is used.

However, in order to interact with SoftDevice SW stack, some changes was made. In details two new files were created: `speed_sensor.c` and its header file `speed_sensor.h`.
They contains some new typedefs and functions to handle events and put events into scheduler queue, interrupt handler linked to LPCOMP and some code to compute the speed. In [ble-app-lbs](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F6_Final_Application_development_BaseCode) code was used a scheduler to handle events and events queue.

### Events and event scheduler ###
The scheduler is used for transferring execution from the interrupt context to the main context.

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/scheduler_logic.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/scheduler_logic.png)

To use scheduler some logic must be implemented. it is spitted into two parts:
#### Logic in main context: ####
  * Define an event handler for each type of event expected. In order to transmit speed data, an event handler is placed in `main.c` file: this is the `speed_event_handler` function.
```
void speed_event_handler(uint32_t speed_data)
{
	uint32_t err_code;
	err_code= ble_sss_on_speed_change(&m_sss, speed_data);
	
	if (err_code != NRF_SUCCESS &&
                err_code != BLE_ERROR_INVALID_CONN_HANDLE &&
                err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
}
```
  * Initialize the scheduler by calling the APP\_SCHED\_INIT() macro before entering the application main loop. This was already done in [ble-app-lbs](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F6_Final_Application_development_BaseCode) code since the example's code already uses the scheduler.
```
/**@brief Function for the Event Scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}
```
  * Call app\_sched\_execute() from the main loop each time the application wakes up because of an event (typically when sd\_app\_evt\_wait() returns). Also this code was present in [ble-app-lbs](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F6_Final_Application_development_BaseCode) code since the example's code already uses the scheduler.
```
    // Enter main loop
    for (;;)
    {
        app_sched_execute();
        power_manage();
    }
```

#### Logic in interrupt context: ####
  * In the interrupt handler, call app\_sched\_event\_put() with the appropriate data and event handler. This will insert an event into the scheduler's queue. The app\_sched\_execute() function will pull this event and call its handler in the main context.
The LPCOMP interrupt handler is placed into `speed_sensor.c` and is the following:
```
/***************************************************************************************************
*____________________________________________LPCOMP_ISR_____________________________________________
***************************************************************************************************/

/**
 * Interrupt handler for LPCOMP
 *
 * @param  none
 * @return none
 *
 * @brief  	ISR for LPCOMP: 
 * 					clear event's flag, 
 */
	 /* Interrupt handler for LPCOMP */
void LPCOMP_IRQHandler(void)
{
	//____________________________LOCAL-VARIABLES-DEFINITION_________________________________________

	//Declaration and initialization of previous counter value useful to compute delta time: it must have memory
	static uint32_t PreviousRTC0CounterValue=0;
	//Declaration of DeltaTime and Speed variables
	uint32_t deltatime;
	static uint32_t old_speed_kmh;
	
	//____________________________SERVICE-ROUTINES___________________________________________________
		
	// Clear event
	NRF_LPCOMP->EVENTS_UP = 0;

	//______________________________COMPUTATION______________________________________________________
		
	//compute actual delta time between two LPCOMP upward crossing event
	deltatime=abs(NRF_RTC0->COUNTER - PreviousRTC0CounterValue);   
	//update value of PreviuousRTC0CounterValue
	PreviousRTC0CounterValue=NRF_RTC0->COUNTER;
	//Compute Speed in km/h from Delta Time in RTC0 ticks;
	uint32_t actual_speed_kmh=SPEED_KMH(deltatime);
	
	
	
	if(old_speed_kmh!=actual_speed_kmh)		//speed has changed: generate an event and send value through BLE to central device
	{
		old_speed_kmh=actual_speed_kmh;
				
		app_speed_sensor_evt_schedule(speed_event_handler,actual_speed_kmh);
		
	}

	static uint8_t LPCOMP_interrupt_counter=0;

	//release external crystal
	sd_clock_hfclk_release();
}
```

the `app_speed_sensor_evt_schedule(speed_event_handler,actual_speed_kmh)` called in the previous code is defined in header file as follow:

```
/**@brief Function that create the data-structure of the event and that put the event into scheduler's queue.*/
static uint32_t app_speed_sensor_evt_schedule(app_speed_sensor_handler_t speed_sensor_handler, uint32_t speed_data)		
{
	app_speed_sensor_event_t speed_sensor_event;
	
	speed_sensor_event.speed_sensor_handler = speed_sensor_handler;
	speed_sensor_event.speed_data 					= speed_data;
	
	return app_sched_event_put(&speed_sensor_event, sizeof(speed_sensor_event),app_speed_sensor_evt_get);
}
```

As commented, the function create a data-structure of the event and put the event into scheduler's queue through the function pointer `app_speed_sensor_evt_get` shown below.

```
/**@brief Function calling the event with appropriate data*/
static void app_speed_sensor_evt_get(void * p_event_data, uint16_t event_size)				
{
    app_speed_sensor_event_t * p_speed_sensor_event = (app_speed_sensor_event_t *)p_event_data;
    
    APP_ERROR_CHECK_BOOL(event_size == sizeof(app_speed_sensor_event_t));
	p_speed_sensor_event->speed_sensor_handler(p_speed_sensor_event->speed_data);
}
```

To handle the event, a data structure and a function pointer was defined in `speed_sensor.h`, in a similar way of the button's event.

```
/**@brief Speed sensor event handler type. Function pointer to speed_sensor handler function in main.c */
typedef void (*app_speed_sensor_handler_t)(uint32_t speed_data);

/**@brief Data structure to handle the event and relative event-data*/
typedef struct																											
{
	app_speed_sensor_handler_t speed_sensor_handler;
	uint32_t speed_data;
}app_speed_sensor_event_t;
```

Other generic useful information on events and scheduler are available in SDK, [here](http://developer.nordicsemi.com/nRF51_SDK/doc/7.0.0/s110/html/a00778.html#details).

A session log of this step is available [here](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/BLEEH_6_3_rev333_sniffing.pcapng) (the file could be opened with [Wireshark](https://www.wireshark.org/)).

Code for this step is here: [svn\tags\6\_3\_Final\_Application\_development\_step\_3](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F6_3_Final_Application_development_step_3).

## 4-Transmit data only when essential ##
Once the code was developed, a simple test was done. The [button](https://code.google.com/p/nrf51822-bleeh/wiki/2b_Development_of_Comparator_Driver#Circuit_setup) at port P0.03 was pushed and some over-the-air packets were registered with [nRF-Sniffer](https://www.youtube.com/watch?v=Sccst1loWbU). This is the result after the connection parameters update was done:

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/Sniffer_WS_6_3.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/Sniffer_WS_6_3.png)

Connection parameters was set to: 500 ms of connection interval, slave latency of 7 and supervisory timeout of 10 s.

With this connection parameters, the PCA10001 board (the device with link-layer in slave mode) connect to the master (a Bluetoorh Smart Ready smartphone) every 4 s if it has no data to send. This is clear from above image: observe the green-highlighted rows. In-fact, is not available any data to transmit, the slave, transmit an empty PDU every 4 s (e.g. at t1=26.36 s and t2=30.36 s). The master, instead, transmit an empty packets every 500 ms however the slave is not listen for them and could be in low-power mode.
Yellow-highlighted rows are the data-packets send from slave (the speed-sensor) to master device. Observe there are three packets on-air in 30 ms. I don't want this because the user cannot appreciate an ultra fast refresh rate. It's only a waste of energy.

As I mentioned above, I desire a maximum refresh-rate of 2 frame per second corresponding to a min. period of 500 ms.
Multiple packets sending in only 30 ms occur because in firmware, at every comparator-related interrupt, a call to  function `app_speed_sensor_evt_schedule(speed_event_handler,actual_speed_kmh)` is done. This function put the event into scheduler's queue. When the scheduler executes the event, the function `ble_sss_on_speed_change(ble_sss_t * p_sss, uint8_t actual_speed)` is executed and it calls the SoftDevice function `sd_ble_gatts_hvx(p_sss->conn_handle, &params)` that transmit a data-packet on the air.

Thus the goal is to put the event into scheduler's queue only when the speed has changed and at a maximum frame rate of 2 fps. This is reached with following code:

```
/***************************************************************************************************
*____________________________________________LPCOMP_ISR_____________________________________________
***************************************************************************************************/

/**
 * Interrupt handler for LPCOMP
 *
 * @param  none
 * @return none
 *
 * @brief  	ISR for LPCOMP
 */
	 /* Interrupt handler for LPCOMP */
void LPCOMP_IRQHandler(void)
{
	//____________________________LOCAL-VARIABLES-DEFINITION_________________________________________

	//Declaration and initialization of previous counter value useful to compute delta time: it must have memory
	static uint32_t PreviousRTC0CounterValue=0;
	//Declaration of DeltaTime and Speed variables
	uint32_t deltatime_ticks;
	static uint32_t old_speed_kmh;
	static uint32_t last_tx_evt_time;
	//____________________________SERVICE's-ROUTINES___________________________________________________
		
	// Clear LPCOMP event
	NRF_LPCOMP->EVENTS_UP = 0;

	//______________________________COMPUTATION______________________________________________________
		
	//compute actual delta time between two LPCOMP upward crossing event
	deltatime_ticks=abs(NRF_RTC0->COUNTER - PreviousRTC0CounterValue);   
	//update value of PreviuousRTC0CounterValue
		PreviousRTC0CounterValue=NRF_RTC0->COUNTER;

	//if time elapsed is greater than minimum BLE TX interval, compute speed and, if speed has changed, tx data.
	if(abs(last_tx_evt_time - NRF_RTC0->COUNTER )>=MIN_TX_INTERVAL_TICKS)
	{
		//compute Speed in km/h from Delta Time in RTC0 ticks;
		uint32_t actual_speed_kmh=SPEED_KMH(deltatime_ticks);
		
		//speed has changed: generate an event and send value through BLE to central device: TX data!
		if(old_speed_kmh!=actual_speed_kmh)		
		{
			old_speed_kmh=actual_speed_kmh;
			app_speed_sensor_evt_schedule(speed_event_handler,actual_speed_kmh);
			last_tx_evt_time=NRF_RTC0->COUNTER;
			
		}
	}

	//release external crystal
	sd_clock_hfclk_release();
}
```

With previous code, a maximum refresh rate equal to 2 frame per second is obtained. A packet is sent from slave to master every 500 ms at minimum and only if data has changed. If speed-data has not changed, slave transmit an empty packet every 4 s. See a nRF-Sniffer session below:

![https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/Sniffer_WS_6_4.png](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/Sniffer_WS_6_4.png)

Observe the yellow-highlighted rows: they're spaced at least 500 ms each other as desired. If no data has to be transmitted, an empty PDU is transmitted every 4 s in order to maintain the connection active. A session log of this step is available [here](https://dl.dropboxusercontent.com/u/43091897/HostedFile/nRF51822_project/BLEEH_6_4_rev335_sniffing.pcapng) (the file could be opened with [Wireshark](https://www.wireshark.org/)).

Code for this step is here: [svn\tags\6\_4\_Final\_Application\_development\_step\_4](https://code.google.com/p/nrf51822-bleeh/source/browse/#svn%2Ftags%2F6_4_Final_Application_development_step_4).

## 5-Delete all extra functionality minimizing current consumption ##
  * **Choice of 32 kHz clock source**: this clock source is ever running in system ON mode, either in CPU sleep and CPU on. A high accuracy external crystal is clearly the best choice in terms of current consumption. PCA10001 Evaluation board mount a 20 PPM accuracy crystal oscillator.
  * **Choice of 16 MHz clock source**: to minimize current consumption, is chosen 16MHz external crystal. Chose a crystal with low load capacitance, i.e. around 9 pF instead of the specified maximum of 16 pF for the nRF51 to have minimal “startup” current consumption. Unfortunately changing capacitors is not practical with this evaluation kit.
  * **Use of timers**: use the application timers that are generally used with SoftDevice stack, RTC0 in the background which is very power efficient and it's already running.
  * **Tuning connection parameters**: this is already done in [Update connection parameters](https://code.google.com/p/nrf51822-bleeh/wiki/6_Final_Application_development?ts=1419354921&updated=6_Final_Application_development#Update_connection_parameters_(on_start-up)_to_preserve_energy_de) paragraph.
  * **Tuning advertising parameters**: Current consumption during advertising mainly depends on the advertising interval, which is adjusted by modifying the following constant:
```
#define APP_ADV_INTERVAL                     40                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
```
The longer the advertising interval the less current will the device consume during advertising. However, when connecting to a central device, increasing the advertising interval will normally not decrease current consumption as it will take longer for the central device to discover the peripheral device. The result is that current consumption will be lower for the peripheral during advertising, but it will normally have to advertise for a longer time, therefore, energy consumed before connecting to the central will generally be more or less the same as when short advertising interval is chosen.
For some scenarios, it is convenient to advertise periodically, and that will save current instead of having the device advertise continuously. The timeout for the advertising can be adjusted by modifying the following parameter:
```
#define APP_ADV_TIMEOUT_IN_SECONDS           180                                        /**< The advertising timeout in units of seconds. */
```
Another parameter that effects the current consumption during advertising is how many payload bytes are sent in each advertising packet.
  * **LDO mode vs Low Voltage mode vs DCDC mode**: maximum power saving mode is using Low Voltage mode however it isn't possible use this mode with the with evaluation board. [LDO mode](https://code.google.com/p/nrf51822-bleeh/wiki/3_Study_and_Development_of_UltraLowPower_Techniques#The_power_management) is used instead. With Softdevice, it's achieved as shown:
```
/**@brief Function for POWER configuration initialization
 *
 * @details Disable internal DC/DC step down and select low power mode when in system on mode.
 */
static void power_config_init(void)
{
	uint32_t                				err_code;
	
	err_code=sd_power_dcdc_mode_set(NRF_POWER_DCDC_MODE_OFF);	
	APP_ERROR_CHECK(err_code);
	
	err_code=sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
	APP_ERROR_CHECK(err_code);
}
```
  * **Compiler optimization**: enable compiler optimization in order for you application to be executed in as few instructions as possible. This will make the code execute faster and finish sooner and will therefore save power. To enable compiler optimization in Keil, select Options for Target -> C/C++ tab and set Optimization to “Level 3” and check “Optimize for Time”.
  * **Change the TX power** with function [sd\_ble\_gap\_tx\_power\_set()](http://developer.nordicsemi.com/nRF51_SDK/doc/7.0.0/s110/html/a01047.html#ga0f1931af876bef39520c58de5ac060ca). Accepted values are -40, -30, -20, -16, -12, -8, -4, 0, and 4 dBm.
  * **Disable GPIOTE module**: In first and second revision of SOC hardware there are some current consumption issues thus GPIOTE module could be disabled. Furthermore led (LED0 and LED1 indicating advertising mode and connected mode) and push buttons (somewhat pull-up/down resistors) drawn some current.

Code for this step is here: [.md](.md).