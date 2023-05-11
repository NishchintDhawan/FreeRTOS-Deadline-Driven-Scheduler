# FreeRTOS-Deadline-Driven-Scheduler

A Deadline-driven task scheduler using FreeRTOS on an STM32F4 Discovery board for ECE 455 at UVic
 
There are 4 core tasks performed by the Deadline-Driven Scheduler (DDS) - 

## Deadline-Driven Scheduler Task
This is the FreeRTOS task which runs the main scheduler. It is responsible for managing DDS tasks in an active list by determining their priorities. It must also manage the tasks which are completed (meet their deadline) and which are overdue (did not meet their deadline) in a completed and
overdue list respectively.
## Deadline-Driven Task Generator Task
This is responsible for creating the tasks for the DDS to schedule. It creates the task structure, initialises values for it and sends it to the DDS using queues. It runs periodically to generate tasks.
## User-Defined Tasks
These are the tasks which contain the actual task to be run. For this experiment an empty loop runs for a given amount of time to simulate running of actual tasks. Whenever they are completed, they send the message to the DDS using queues to notify about their completion.
## Monitor Task
This task is used to report the status of the DDS and our tasks periodically (every hyper period). It gets the information of the lists from our DDS and reports them to the user. For simplicity in this experiment, it simply prints the information to our console. 
<br>

#### Sample test bench 

| task | Execution time (ms) | Period (ms) | 
|---|---|---|
| t1 | 95  | 250 |
| t2 |  150 | 500  |
| t3 |  250 |  750 |

The hyper period for t1, t2 and t3 is 1500 ms.
<br>

#### Monitor task output showing results for 3 hyper periods

<img width="559" alt="image" src="https://github.com/NishchintDhawan/FreeRTOS-Deadline-Driven-Scheduler/assets/33113405/c15d8615-ed08-433e-9584-de536a415d37">

The task is added to the active list when it is released. As the tasks are completed, we add them to the completed list. The overdue tasks at the time of reporting are stored in the overdue list.  
<br>

#### Recorded values for first hyper period 

| Event # | Event | Measured Time (ms) | Expected Time (ms) |
| --- | --- | --- | --- |
|1| Task 1 released | 0| 0|
|2|Task 2 released| 0| 0|
|3| Task 3 released| 1| 0|
|4 |Task 1 completed| 95 |95|
|5 |Task 2 completed| 245| 245|
|6 |Task 3 completed| 495| 495|
|7 |Task 1 released| 500 |500|
|8| Task 2 released| 500| 500|
|9 |Task 1 completed| 595| 595|
|10 |Task 2 completed| 745| 745|
|11 |Task 3 released| 750| 750|
|12 |Task 1 released| 1000| 1000|
|13 |Task 2 released| 1000| 1000|
|14 |Task 3 completed| 1000| 1000|
|15 |Task 1 completed| 1095| 1095|
|16 |Task 2 completed| 1245| 1245|
|17 |Task 1 released| 1500| 1500|
