
#include "DD_Scheduler.h"


static QueueHandle_t DD_Scheduler_Message_Queue;
static QueueHandle_t DD_Monitor_Message_Queue;
DD_TaskPtr_t get_completed_list();
DD_TaskPtr_t get_active_list();
DD_TaskPtr_t get_overdue_list();
void pushToMonitorQueue();
uint32_t count = 0;
DD_TaskPtr_t active_list=NULL;
DD_TaskPtr_t completed_list = NULL;
DD_TaskPtr_t completed_list_end = NULL;
DD_TaskPtr_t overdue_list = NULL;
DD_TaskPtr_t overdue_list_end = NULL;
bool flag=true;
void vTask3_TimerCallback(xTimerHandle xTimer);
void MonitorTaskCallback(xTimerHandle xTimer);

/* Create the queue used by the queue send and queue receive tasks.
 http://www.freertos.org/a00116.html */
// Initializes the lists and communication queues necessary for the DD_Scheduler
void DD_Scheduler_Init(){


	//Initialize DD_Scheduler_Message_Queue queue.
	DD_Scheduler_Message_Queue = xQueueCreate(DD_TASK_RANGE, sizeof(DD_Message_t));
	vQueueAddToRegistry(DD_Scheduler_Message_Queue,"Scheduler Queue");
	DD_Monitor_Message_Queue = xQueueCreate(DD_TASK_RANGE, sizeof(DD_Message_t));
	vQueueAddToRegistry(DD_Monitor_Message_Queue,"Monitor Queue");
	active_list=NULL;
	completed_list=NULL;
	overdue_list = NULL;
	xTaskCreate( DD_Scheduler , "DD_Scheduler"  , configMINIMAL_STACK_SIZE , NULL , DD_TASK_PRIORITY_SCHEDULER , NULL);
	xTaskCreate(MonitorTask, "Monitor Task",  configMINIMAL_STACK_SIZE , NULL , DD_TASK_PRIORITY_MONITOR , &MonitorTaskhandle);

	MonitorTimer = xTimerCreate("MonitorTaskTimer", 1502, pdTRUE, MonitorTaskhandle, MonitorTaskCallback); // callback should resume monitor.
	xTimerStart(MonitorTimer, 0);

} // end DD_Scheduler_Init


/*create a task instance and initializes it. */
DD_TaskPtr_t DD_Task_Allocate(){
	 DD_TaskPtr_t newtask = (DD_TaskPtr_t)malloc(sizeof(DD_task_t));
	 newtask->task_handle = NULL;
	 newtask->type = DD_TT_INIT;
	 newtask->task_id = 0;
	 newtask->release_time=0;
	 newtask->absolute_deadline=0;
	 newtask->execution_time=0; //same as execution time. change later.
	 newtask->next_task = NULL;
	 return newtask;
}

//insert function for list according to deadline.
DD_TaskPtr_t DD_AddTasktoList(DD_TaskPtr_t insert_task, DD_TaskPtr_t insert_list ){			// Insert function.

	if(insert_list == NULL){
		insert_list=insert_task;
		return insert_list;
	}

	DD_TaskPtr_t current = insert_list;
	DD_TaskPtr_t prev = current;


	if(insert_task->absolute_deadline < current->absolute_deadline ){
		//list exists and first task is to be replaced .
		if(current->next_task==NULL){
			insert_list = insert_task;
			insert_task->next_task = current;
			current->next_task = NULL;
			return insert_list;
		}

		insert_task->next_task = current;
		insert_list = insert_task;
		return insert_list;
	}

	current=current->next_task;

	while(current!=NULL){
		if(insert_task->absolute_deadline < current->absolute_deadline){
			prev->next_task = insert_task;
			insert_task->next_task = current;
			return insert_list;
		}
		prev= current;
		current= current->next_task;
	}

	prev->next_task=insert_task;

	return insert_list;

}

void print_list( DD_task_t* insert_list){

	DD_TaskPtr_t current = insert_list;
	if(current==NULL){
		printf("Empty\n");
		return;
	}
	while(current!=NULL){

		printf(" %d ", current->task_id);
		current=current->next_task;
	}
	printf(" \n");

}


void setPriorities(DD_task_t* active_list){
	//set priority of first element to 1 and rest to 0.

	if(active_list==NULL){
		return;
	}

	vTaskPrioritySet(active_list->task_handle, DD_TASK_PRIORITY_EXEC);
	DD_TaskPtr_t current = active_list;
	current = current->next_task;

	while(current != NULL){
		vTaskPrioritySet(current->task_handle,DD_TASK_PRIORITY_IDLE);
		current= current->next_task;
	}

	return ;
}

void insertCompletedList(DD_TaskPtr_t task){
	if(completed_list==NULL){
		completed_list=task;
		completed_list_end = task;
		return;
	}

	completed_list_end->next_task = task;
	completed_list_end = completed_list_end->next_task;

}

DD_TaskPtr_t deletefromList(char type[], DD_TaskPtr_t list, uint32_t task_id){	//delete function.

	DD_TaskPtr_t current = list ;
	DD_TaskPtr_t prev =  NULL;

	if(current==NULL){
		return NULL;
	}


	if(current->task_id==task_id){

		if(type=="Complete"){
			insertCompletedList(current);
		}
		else{
			insertOverdueList(current);
		}

		if(current->next_task==NULL){
			vTaskDelete(current->task_handle);
			return NULL;
		}

		prev=current;
		current=current->next_task;
		prev->next_task=NULL;
		vTaskDelete(prev->task_handle);
		return current;
	}

	prev=current;
	current=current->next_task;

	while(current!=NULL){
		if(current->task_id==task_id){

			if(type=="Complete"){
			insertCompletedList(current);
			}
			else{
				insertOverdueList(current);
			}

			prev->next_task=current->next_task;
			vTaskDelete(current->task_handle);
			return list ;
		}
		prev=current;
		current=current->next_task;
	}

	return list ;

}


void insertOverdueList(DD_TaskPtr_t insert_task){

	if(overdue_list==NULL){
		overdue_list=insert_task;
		overdue_list_end = overdue_list;
		return;
	}

	overdue_list_end->next_task = insert_task;
	overdue_list_end = overdue_list_end->next_task;
}



void pushToMonitorQueue(DD_TaskPtr_t list){
	 if( DD_Monitor_Message_Queue != NULL) { // check is queue exists.
	          if( xQueueSend(DD_Monitor_Message_Queue, &list, portMAX_DELAY ) != pdPASS ){ // ensure the message was sent
	                 printf("ERROR: Couldn't send request on DD_Monitor_Message_Queue!\n");
	                 return 0;
	             }
	      }

	     else { // Queue doesn't exist, error out, entire system will fail.
	          printf("ERROR: DD_Monitor_Message_Queue is NULL.\n");
	          return 0;
	      }
}

void DD_Scheduler( void *pvParameters){

	DD_Message_t received_message;
	DD_TaskPtr_t DD_task_handle = NULL;
	uint32_t task_id;
	while(1){
		//read message from queue.
		if(xQueueReceive( DD_Scheduler_Message_Queue, (void*)&received_message, portMAX_DELAY ) == pdTRUE){
			switch(received_message.message_type){

				case(DD_Message_Create):
						//creates the DD message by sending it to the queue. Add to array.
						DD_task_handle = received_message.message_data;
						active_list= DD_AddTasktoList(DD_task_handle,active_list); //inserts tasks into the active list.
						setPriorities(active_list); //adjust priorities.
						break;

				case(DD_Message_Complete):
						//message for task completed. Remove the specific task using its task id from the active list.
						task_id = (int) received_message.message_data;
						active_list=deletefromList("Complete", active_list, task_id);
						setPriorities(active_list);
						break;
				//send list to message queue.
				case(DD_Message_Completed_List):
						DD_task_handle = completed_list;
						pushToMonitorQueue(DD_task_handle);
						break;

				case(DD_Message_Active_List):
						DD_task_handle = active_list;
						pushToMonitorQueue(DD_task_handle);
						break;

				case(DD_Message_Overdue_List):
						DD_task_handle = overdue_list;
						pushToMonitorQueue(DD_task_handle);
						break;
			}
		}
	}
}


void MonitorTask(void *pvParamters){

 while(1){
	 	 //monitor task.
	 	printf("\nMonitoring Task: Current Time = %u, Priority = %u\n", (unsigned int)xTaskGetTickCount(), (unsigned int)uxTaskPriorityGet( NULL ));

	 	printf("active list ->");
	 	print_list(get_active_list());
	 	printf("completed list ->");
	 	print_list(get_completed_list());
		printf("overdue list ->");
	 	print_list(get_overdue_list());
		vTaskSuspend(MonitorTaskhandle);
 	 }
}

void MonitorTaskCallback(xTimerHandle xTimer){

	TaskHandle_t h = (TaskHandle_t) pvTimerGetTimerID(xTimer);
	vTaskResume(h);
}

bool lookupList(DD_TaskPtr_t list, uint32_t task_id){
	DD_TaskPtr_t current = list;

	while(current!=NULL){
		if(current->task_id==task_id){
			return true;
		}
		current=current->next_task;
	}

	return false;
}



uint32_t release_DD_task(DD_TaskPtr_t create_task){
//add to queue by packaging as a message.

    // Check that the input data isn't empty
    if( create_task == NULL )
    {
        printf("ERROR: Sent NULL DD_TaskHandle_t to DD_Task_Create! \n");
        return 0;
    }

    if(lookupList(active_list,create_task->task_id)){
    	//if already exists in active list, task is overdue.
        active_list=deletefromList("Overdue", active_list,create_task->task_id);
    }


    //Create the message structure, with the DD_TaskPtr_t in the data field for the linked list element.
     DD_Message_t create_task_message = { DD_Message_Create ,            // Message Type Request is to create a task
                                            create_task };               // DD_TaskPtr_t is a pointer to the task data.

     xTaskCreate(
       		create_task->task_handle,
   			create_task->task_id,
   			configMINIMAL_STACK_SIZE,
   			(void*) create_task,
   			DD_TASK_PRIORITY_IDLE,		// lowest priority.
   			&(create_task->task_handle));

          // Send the message to the DD_Scheduler queue
     if( DD_Scheduler_Message_Queue != NULL) { // check is queue exists.
          if( xQueueSend(DD_Scheduler_Message_Queue, &create_task_message, portMAX_DELAY ) != pdPASS ){ // ensure the message was sent
                 printf("ERROR: DD_Task_Create couldn't send request on DD_Scheduler_Message_Queue!\n");
                 return 0;
             }
      }

     else { // Queue doesn't exist, error , entire system will fail.
          printf("ERROR: DD_Scheduler_Message_Queue is NULL.\n");
          return 0;
      }
     return 0 ;
}


uint32_t complete_DD_task(uint32_t task_id){
	//called by the task which completed execution. It is sent to the DDS.

	DD_Message_t completed_message = {DD_Message_Complete,
										task_id
									};
	 if( DD_Scheduler_Message_Queue != NULL) { // check is queue exists.
	          if( xQueueSend(DD_Scheduler_Message_Queue, &completed_message, portMAX_DELAY ) != pdPASS ){ // ensure the message was sent
	                 printf("ERROR: add_to_completed_list couldn't send request on DD_Scheduler_Message_Queue!\n");
	                 return 0;
	             }
	      }

	     else { // Queue doesn't exist, error out, entire system will fail.
	          printf("ERROR: DD_Scheduler_Message_Queue is NULL.\n");
	          return 0;
	      }

	 return 0;
}

DD_TaskPtr_t get_completed_list(){

	//Ask DDS to give completed list.
	DD_Message_t completed_message = {DD_Message_Completed_List,
											"Complete"
										};

	if( DD_Scheduler_Message_Queue != NULL) { // check is queue exists.
		if( xQueueSend(DD_Scheduler_Message_Queue, &completed_message, portMAX_DELAY ) != pdPASS ){ // ensure the message was sent
		printf("ERROR: add_to_completed_list couldn't send request on DD_Scheduler_Message_Queue!\n");
		return 0;
		}
	}

	else { // Queue doesn't exist, error out, entire system will fail.
		printf("ERROR: DD_Scheduler_Message_Queue is NULL.\n");
		return 0;
	}



	DD_TaskPtr_t dd_task_ptr;
	DD_TaskPtr_t received_data;

	if(xQueueReceive( DD_Monitor_Message_Queue, (void*)&received_data, portMAX_DELAY ) == pdTRUE){
		dd_task_ptr = received_data;
	}
	else{
		dd_task_ptr= NULL;
	}

	return dd_task_ptr;

}

DD_TaskPtr_t get_active_list(){

	//Ask DDS to give active list.
	DD_Message_t completed_message = {DD_Message_Active_List,
											"Active"
										};

	if( DD_Scheduler_Message_Queue != NULL) { // check is queue exists.
		if( xQueueSend(DD_Scheduler_Message_Queue, &completed_message, portMAX_DELAY ) != pdPASS ){ // ensure the message was sent
		printf("ERROR: add_to_completed_list couldn't send request on DD_Scheduler_Message_Queue!\n");
		return 0;
		}
	}

	else { // Queue doesn't exist, error out, entire system will fail.
		printf("ERROR: DD_Scheduler_Message_Queue is NULL.\n");
		return 0;
	}

	DD_TaskPtr_t dd_task_ptr;
	DD_TaskPtr_t received_data;

	if(xQueueReceive( DD_Monitor_Message_Queue, (void*)&received_data, portMAX_DELAY ) == pdTRUE){
		dd_task_ptr = received_data;
	}
	else{
		dd_task_ptr= NULL;
	}

	return dd_task_ptr;

}

DD_TaskPtr_t get_overdue_list(){

	//Ask DDS to give overdue list.
	DD_Message_t completed_message = {DD_Message_Overdue_List,
											"Overdue"
										};

	if( DD_Scheduler_Message_Queue != NULL) { // check is queue exists.
		if( xQueueSend(DD_Scheduler_Message_Queue, &completed_message, portMAX_DELAY ) != pdPASS ){ // ensure the message was sent
		printf("ERROR: add_to_completed_list couldn't send request on DD_Scheduler_Message_Queue!\n");
		return 0;
		}
	}

	else { // Queue doesn't exist, error out, entire system will fail.
		printf("ERROR: DD_Scheduler_Message_Queue is NULL.\n");
		return 0;
	}

	DD_TaskPtr_t dd_task_ptr;
	DD_TaskPtr_t received_data;

	if(xQueueReceive( DD_Monitor_Message_Queue, (void*)&received_data, portMAX_DELAY ) == pdTRUE){
		dd_task_ptr = received_data;
	}
	else{
		dd_task_ptr= NULL;
	}

	return dd_task_ptr;

}
