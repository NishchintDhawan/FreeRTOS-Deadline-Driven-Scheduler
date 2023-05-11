/*
 * STMRTOSconfig.h
 *
 */

#ifndef STMRTOSCONFIG_H_
#define STMRTOSCONFIG_H_

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "stm32f4_discovery.h"

/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

#define PORT                     GPIOC
#define ADC_PIN                  GPIO_Pin_3


#define SHIFT_REG_1_RESET_PIN    GPIO_Pin_8
#define SHIFT_REG_1_DATA_PIN     GPIO_Pin_6
#define SHIFT_REG_CLK_1_PIN      GPIO_Pin_7


#define TRAFFIC_LIGHT_RED_PIN    GPIO_Pin_0
#define TRAFFIC_LIGHT_YELLOW_PIN GPIO_Pin_1
#define TRAFFIC_LIGHT_GREEN_PIN  GPIO_Pin_2

// Traffic light task priorities
#define TRAFFIC_FLOW_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1 )
#define TRAFFIC_CREATE_TASK_PRIORITY	( tskIDLE_PRIORITY  )
#define TRAFFIC_LIGHT_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2 )
#define TRAFFIC_DISPLAY_TASK_PRIORITY	( tskIDLE_PRIORITY  )

uint16_t global_flowrate;
uint16_t global_light_color;

xSemaphoreHandle xMutexFlow;
xSemaphoreHandle xMutexLight;

TimerHandle_t xRedLightTimer;
TimerHandle_t xYellowLightTimer;
TimerHandle_t xGreenLightTimer;
#endif /* STMRTOSCONFIG_H_ */

