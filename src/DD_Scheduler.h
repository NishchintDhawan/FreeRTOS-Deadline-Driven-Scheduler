
#ifndef DD_SCHEDULER_H_
#define DD_SCHEDULER_H_s
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

/* STM / RTOS includes. */
#include "stm32f4_discovery.h"

#define DD_TASK_LIST_H_

/*--------------------------- Task Priority Definitions --------------------------------*/

#define DD_TASK_PRIORITY_IDLE           (0)
#define DD_TASK_PRIORITY_EXEC	        (1)
#define DD_TASK_PRIORITY_GENERATOR      (2)
#define DD_TASK_PRIORITY_MONITOR        (3)
#define DD_TASK_PRIORITY_SCHEDULER      (4) // set to the highest priority, defined in FreeRTOSConfig.h

#define DD_TASK_RANGE (5) // Number of tasks allowed

#define task_1_period (500)
#define task_1_exec   (95)

#define task_2_period (500)
#define task_2_exec   (150)

#define task_3_period (750)
#define task_3_exec   (250)

#define ap_task_exec 		(500)
#define ap_task_deadline 	(900)

/*--------------------------- Task  --------------------------------*/
typedef enum task_type{
	DD_TT_INIT,
	DD_TT_PERIODIC,
	DD_TT_APERIODIC				// type of task.
} task_type;

typedef struct DD_Task_t {			// task struct.
TaskHandle_t task_handle;
task_type type;
uint32_t task_id;
uint32_t release_time;
uint32_t absolute_deadline;
uint32_t execution_time; //same as execution time. change later.
struct DD_Task_t* next_task;
} DD_task_t;

typedef DD_task_t* DD_TaskPtr_t;


/*--------------------------- DD Scheduler Message --------------------------------*/

// Message types
typedef enum DD_Message_Type_t
{
    DD_Message_Create,
    DD_Message_Complete,
	DD_Message_Completed_List,
	DD_Message_Active_List,
    DD_Message_Overdue_List
}DD_Message_Type_t;

// Message structure
typedef struct DD_Message_t
{
    DD_Message_Type_t message_type;
    DD_TaskPtr_t      message_data;
}DD_Message_t;


TaskHandle_t Periodic_task_gen_handle_1;
TaskHandle_t Periodic_task_gen_handle_2;
TaskHandle_t Periodic_task_gen_handle_3;
TaskHandle_t MonitorTaskhandle;
TaskHandle_t Aperiodic_task_gen_handle_1;

/*--------------------------- Periodic Tasks --------------------------------*/

void PeriodicTask_1 ( void *pvParameters );
void PeriodicTaskGenerator_1( void *pvParameters );
void PeriodicTask_2 ( void *pvParameters );
void PeriodicTaskGenerator_2( void *pvParameters );
void PeriodicTask_3 ( void *pvParameters );
void PeriodicTaskGenerator_3( void *pvParameters );
/*--------------------------- Task Creation/Deletion (List Elements) --------------------------------*/

// Creates a DD_TaskPtr_t
DD_TaskPtr_t DD_Task_Allocate();
// Frees the memory allocated from DD_Task_Allocate()
bool DD_Task_Free( DD_TaskPtr_t task_to_remove );

/*--------------------------- Task Initialization --------------------------------*/


/*--------------------------- Task List Access Functions --------------------------------*/

// Insert element in ordered list based off deadline.
//void DD_TaskList_Deadline_Insert( DD_TaskPtr_t task_to_insert , DD_TaskListHandle_t insert_list );

// Remove item from list given its DD_TaskPtr_t
//void DD_TaskList_Remove( TaskHandle_t task_to_remove , DD_TaskListHandle_t remove_list , bool clear_memory );

// Remove head of list.
//void DD_TaskList_Remove_Head( DD_TaskListHandle_t remove_list );

// Removes overdue tasks from active list, adds them to the overdue list.
//void DD_TaskList_Transfer_Overdue( DD_TaskListHandle_t active_list , DD_TaskListHandle_t overdue_list );

// Returns a formatted string of task names and deadline in a list.
//char * DD_TaskList_Formatted_Data( DD_TaskListHandle_t list );

/*--------------------------- DD Scheduler --------------------------------*/

void DD_Scheduler( void *pvParameters );
DD_TaskPtr_t DD_AddTasktoList(DD_TaskPtr_t , DD_TaskPtr_t  );
DD_TaskPtr_t deletefromList(char type[], DD_TaskPtr_t list, uint32_t task_id);
void print_list( DD_task_t*);
void pushToMonitorQueue(DD_TaskPtr_t list);

/*--------------------------- DD Scheduler Public Access Functions --------------------------------*/

void DD_Scheduler_Init( void );
uint32_t release_DD_task(DD_TaskPtr_t create_task );
uint32_t add_to_completed_list(uint32_t task_id);

/*--------------------------- DD Scheduler Monitoring Functions --------------------------------*/

void MonitorTask ( void *pvParameters );
DD_TaskPtr_t get_active_list();
DD_TaskPtr_t get_completed_list();
DD_TaskPtr_t get_overdue_list();


TimerHandle_t vTask1_Timer;
TimerHandle_t vTask2_Timer;
TimerHandle_t vTask3_Timer;
TimerHandle_t MonitorTimer;

#endif /* DD_SCHEDULER_H_ */
