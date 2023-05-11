/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

#include "DD_Scheduler.h"
/*-----------------------------------------------------------*/
#define mainQUEUE_LENGTH 100;
/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );

xQueueHandle xQueue_handle = 0;


void vTask1_TimerCallback(xTimerHandle xTimer);
void vTask2_TimerCallback(xTimerHandle xTimer);
void vTask3_TimerCallback(xTimerHandle xTimer);
void vTask4_TimerCallback(xTimerHandle xTimer);

/*-----------------------------------------------------------*/

int main(void)
{

	/* Configure the system ready to run the demo.  The clock configuration
	can be done here if it was not done before main() was called. */
 	prvSetupHardware();
	DD_Scheduler_Init();

 	xTaskCreate( PeriodicTaskGenerator_1 , "PeriodGenTask1"  , configMINIMAL_STACK_SIZE , NULL , DD_TASK_PRIORITY_GENERATOR , &Periodic_task_gen_handle_1);
 	xTaskCreate( PeriodicTaskGenerator_2 , "PeriodGenTask2"  , configMINIMAL_STACK_SIZE , NULL , DD_TASK_PRIORITY_GENERATOR , &Periodic_task_gen_handle_2);
	xTaskCreate( PeriodicTaskGenerator_3 , "PeriodGenTask3"  , configMINIMAL_STACK_SIZE , NULL , DD_TASK_PRIORITY_GENERATOR , &Periodic_task_gen_handle_3);

	vTask1_Timer = xTimerCreate("Task1", task_1_period, pdTRUE, Periodic_task_gen_handle_1, vTask1_TimerCallback); // value is 500.
	vTask2_Timer = xTimerCreate("Task2", task_2_period, pdTRUE, Periodic_task_gen_handle_2, vTask2_TimerCallback); // value is 500.
	vTask3_Timer = xTimerCreate("Task3", task_3_period, pdTRUE, Periodic_task_gen_handle_3, vTask3_TimerCallback); // value is 500.

	xTimerStart(vTask1_Timer, 0);
	xTimerStart(vTask2_Timer, 0);
	xTimerStart(vTask3_Timer, 0);

    vTaskStartScheduler();

	return 0;
}


void vTask1_TimerCallback(xTimerHandle xTimer){
	//resume the task 1.
	TaskHandle_t h = (TaskHandle_t) pvTimerGetTimerID(xTimer);
	vTaskResume(h);
}

void vTask2_TimerCallback(xTimerHandle xTimer){
	//resume the task 2.
	TaskHandle_t h = (TaskHandle_t) pvTimerGetTimerID(xTimer);
	vTaskResume(h);
}

void vTask3_TimerCallback(xTimerHandle xTimer){
	//resume the task 3.
	TaskHandle_t h = (TaskHandle_t) pvTimerGetTimerID(xTimer);
	vTaskResume(h);
}

/*
 * User defined task 1.
 * PeriodicTask_1
 */
void PeriodicTask_1 ( void *pvParameters )
{
	//User Defined Task 1.

	DD_TaskPtr_t my_task = (DD_TaskPtr_t)pvParameters;
	TickType_t counter = xTaskGetTickCount() ;
	TickType_t total = task_1_exec + xTaskGetTickCount();

	for(int i=0; i<14000*task_1_exec;i++){}

	complete_DD_task(1);
} // end PeriodicTask_1



void PeriodicTaskGenerator_1( void *pvParameters )
{

	while(1){
		        DD_TaskPtr_t generated_task = DD_Task_Allocate();
		        generated_task->task_handle = PeriodicTask_1;
		        generated_task->type     = DD_TT_PERIODIC;
		        generated_task->task_id = 1;
		        TickType_t current_time = xTaskGetTickCount();     // fetch the current time to calculate deadline.
		        generated_task->release_time = current_time;
		        uint32_t deadline = current_time + task_1_period;
		        generated_task->absolute_deadline   = deadline;        //needs to be recalculated.
		        release_DD_task(generated_task);
		vTaskSuspend(Periodic_task_gen_handle_1);
	}

}

void PeriodicTask_2(void *pvParameters){

	//User Defined Task 2.
	DD_TaskPtr_t my_task = (DD_TaskPtr_t)pvParameters;
	TickType_t counter = xTaskGetTickCount() ;
	TickType_t total = task_2_exec + xTaskGetTickCount();

	for(int i=0; i<14000*task_2_exec;i++){}

	complete_DD_task(2);

}

void PeriodicTaskGenerator_2( void *pvParameters ){

	while(1){
			        DD_TaskPtr_t generated_task = DD_Task_Allocate();
			        generated_task->task_handle = PeriodicTask_2;
			        generated_task->type     = DD_TT_PERIODIC;
			        generated_task->task_id = 2;
			        TickType_t  current_time = xTaskGetTickCount();     // fetch the current time to calculate deadline.
			        generated_task->release_time = current_time;
			        uint32_t deadline = current_time + task_2_period;
			        generated_task->absolute_deadline   = deadline;
			        release_DD_task(generated_task);
			vTaskSuspend(Periodic_task_gen_handle_2);
		}

}

void PeriodicTask_3(void *pvParameters){

	//User Defined Task 3.
	DD_TaskPtr_t my_task = (DD_TaskPtr_t)pvParameters;
	TickType_t counter = xTaskGetTickCount() ;
	TickType_t total = task_3_exec + xTaskGetTickCount();

	for(int i=0; i<14000*task_3_exec;i++){}

	complete_DD_task(3);


}

void PeriodicTaskGenerator_3( void *pvParameters ){
	while(1){
			        DD_TaskPtr_t generated_task = DD_Task_Allocate();
			        generated_task->task_handle = PeriodicTask_3;
			        generated_task->type     = DD_TT_PERIODIC;
			        generated_task->task_id = 3;
			        TickType_t current_time = xTaskGetTickCount();     // fetch the current time to calculate deadline.
			        generated_task->release_time = current_time;
			        uint32_t deadline = current_time + task_3_period;
			        generated_task->absolute_deadline   = deadline;
			        release_DD_task(generated_task);
			vTaskSuspend(Periodic_task_gen_handle_3);
		}

}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

