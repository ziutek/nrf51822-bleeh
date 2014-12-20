/*********************************************************************************************************
** -----------------------------------------------------------------------------------------------------**
** Filename: speed_sensor.h																																							**												
** -----------------------------------------------------------------------------------------------------**	
** Description: comparator driver for Nordic Semiconductor nRF51822 to use with SoftDevice S110	7.1.0		**
**							and SDK 6.0.0																																						**
** -----------------------------------------------------------------------------------------------------**
** Author: Luca Buccolini																																								**
** -----------------------------------------------------------------------------------------------------**
** Start date: Sep 25, 2014 																																						**
** -----------------------------------------------------------------------------------------------------**
** Finish date:	Oct 06, 2014    																																				**
** -----------------------------------------------------------------------------------------------------**
** Modified to gain SoftDevice compatibility on:	Dec 10, 2014   																				**
** -----------------------------------------------------------------------------------------------------**
*********************************************************************************************************/

#ifndef SPEED_SENSOR_H__
#define SPEED_SENSOR_H__

#include <stdint.h>
#include "app_scheduler.h"

/***************************************************************************************************
*_______________________________________ TYPE DEFINITIONS___________________________________________
***************************************************************************************************/
/**@brief Speed sensor event handler type. Function pointer to speed_sensor handler function in main.c */
typedef void (*app_speed_sensor_handler_t)(uint32_t speed_data);

/**@brief Data structure to handle the event and relative event-data*/
typedef struct																											
{
	app_speed_sensor_handler_t speed_sensor_handler;
	uint32_t speed_data;
}app_speed_sensor_event_t;

/***************************************************************************************************
*______________________________________LOCAL FUNCTIONS______________________________________________
***************************************************************************************************/
/**@brief Function calling the event with approriate data*/
static void app_speed_sensor_evt_get(void * p_event_data, uint16_t event_size)				
{
    app_speed_sensor_event_t * p_speed_sensor_event = (app_speed_sensor_event_t *)p_event_data;
    
    APP_ERROR_CHECK_BOOL(event_size == sizeof(app_speed_sensor_event_t));
	p_speed_sensor_event->speed_sensor_handler(p_speed_sensor_event->speed_data);
}

/**@brief Function that create the data-structure of the event and that put the event into scheduler's queue.*/
static uint32_t app_speed_sensor_evt_schedule(app_speed_sensor_handler_t speed_sensor_handler, uint32_t speed_data)		
{
	app_speed_sensor_event_t speed_sensor_event;
	
	speed_sensor_event.speed_sensor_handler = speed_sensor_handler;
	speed_sensor_event.speed_data 					= speed_data;
	
	return app_sched_event_put(&speed_sensor_event, sizeof(speed_sensor_event),app_speed_sensor_evt_get);
}


/***************************************************************************************************
*______________________________________APIs DECLARATION_____________________________________________
***************************************************************************************************/
void LPCOMP_init (void);
void LPCOMP_IRQHandler(void);

#endif 		//SPEED_SENSOR_H__
