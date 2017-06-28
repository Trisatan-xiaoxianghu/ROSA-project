/*****************************************************************************
Created by group 3.
Erik Martín Campaña
Elisabeth Eriksson
Tamilkumari Padmanathan
Shijie Zhu
                             Test Cases 2
-----------------------------------------------------------
		  Priority   Period(ms)   Offset(ms)     Semaphore
-----------------------------------------------------------
task 1       2         1000          0             S1,S2
task 2       4         200           30            S1
task 3       6         200           20            -
task 4       8         10000         0             S1
-----------------------------------------------------------

Ceil(S1) = Priority(task4) = 8
Ceil(S2) = Priority(task1) = 2

This case is used to test two parts:
1st: Before task4 does the semaphore tasking, we test the IPCP algorithm. 
	 Task1, Task2 and Task4 register Semaphore 1, but they have different 
	 Start time. Task1 takes S1 first and its priority is changed to 8.
	 Task2 will be blocked because S1 is taken by task1.
	 Task3 cannot execute because Priority(Task3) < Ceil(S1).
	 Both tasks are blocked until task1 releases S1
2nd: Task4 wants to take S2 but hasn't registered it, so the system should
	 show the error. 

Passing the test: task2 and task3 stop the led blinking when task1 takes S1.
	 And when task4 takes S2, the system should show the error.
*****************************************************************************/

//Standard library includes
#include <avr32/io.h>

//Kernel includes
#include "kernel/rosa_ker.h"
#include "kernel/rosa_semaphore.h"
//#include "kernel/rosa_extended_ker.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"

//Include configuration
#include "rosa_config.h"

#include "group3tests.h"

//Data blocks for the tasks
#define T1_PRIORITY	  (2)
static int t1_stack[MIN_STACK_SIZE];

#define T2_PRIORITY	  (4)
static int t2_stack[MIN_STACK_SIZE];

#define T3_PRIORITY	  (6)
static int t3_stack[MIN_STACK_SIZE];

#define T4_PRIORITY	  (8)
static int t4_stack[MIN_STACK_SIZE];

#define MS_TICK_TIME 1

//Semaphore & Array;
static semHandle Semaphore1  ;
static semHandle Semaphore2  ;

void task1(void){
	ticktime StartTime = 0	 /MS_TICK_TIME;
	ticktime Period	   = 1000/MS_TICK_TIME;
	while(1){
		while(ROSA_semaphoreTake(Semaphore1));
		ledOn(LED0_GPIO);
		ROSA_taskDelay(500);
		ledOff(LED0_GPIO);
		ROSA_semaphoreGive(Semaphore1);

		//StartTime = ROSA_getTicks();
		ROSA_taskDelayUntil(&StartTime,Period);
	}
}

void task2(void){
	ticktime StartTime = 0  /MS_TICK_TIME;
	ticktime Offset	   = 30 /MS_TICK_TIME;
	ticktime Period    = 200/MS_TICK_TIME;
	while(1){
		ROSA_taskDelayUntil(&StartTime,Offset);

		while(ROSA_semaphoreTake(Semaphore1));
		ledToggle(LED1_GPIO);
		ROSA_semaphoreGive(Semaphore1);

		ROSA_taskDelayUntil(&StartTime,(Period-Offset));
	}
}

void task3(void){
	ticktime StartTime = 0  / CLOCK_PERIOD;
	ticktime Offset    = 20 / CLOCK_PERIOD;
	ticktime Period    = 200/ CLOCK_PERIOD;
	while(1){
		ROSA_taskDelayUntil(&StartTime,Offset);
		ledToggle(LED2_GPIO);
		ROSA_taskDelayUntil(&StartTime,(Period-Offset));
	}
}

void task4(void){
	ticktime StartTime = 0	  /CLOCK_PERIOD;
	ticktime PeriodTime= 5000 / CLOCK_PERIOD;
	while(1){
		ledOn(LED3_GPIO);
		ROSA_taskDelayUntil(&StartTime,PeriodTime);
		ledOff(LED3_GPIO);
		ROSA_taskDelayUntil(&StartTime,PeriodTime);
		while(ROSA_semaphoreTake(Semaphore2));  //to test the case that task wants to take the semaphore which is not belong to it.
	}
}

/*************************************************************
 * Main function
 ************************************************************/
int test2_grp3(void)
{
	//Initialize the ROSA kernel
	/*	ROSA_init();*/
	ROSA_Extended_Init();
	//Create semaphores
	Semaphore1 = 1;
	Semaphore2 = 2;

	semHandle Task1_Array[2] = {Semaphore1,Semaphore2};
	semHandle Task2_Array[1] = {Semaphore1};
	semHandle Task4_Array[1] = {Semaphore1};

	ROSA_semaphoreCreate(&Semaphore1);
	ROSA_semaphoreCreate(&Semaphore2);

	//Create tasks and install them into the ROSA kernel
	ROSA_tcbCreate(task1, "tsk1", task1, t1_stack, MIN_STACK_SIZE,T1_PRIORITY,NULL,Task1_Array,2);
 	ROSA_tcbCreate(task2, "tsk2", task2, t2_stack, MIN_STACK_SIZE,T2_PRIORITY,NULL,Task2_Array,1);
	ROSA_tcbCreate(task3, "tsk3", task3, t3_stack, MIN_STACK_SIZE,T3_PRIORITY,NULL,NULL,0);
	ROSA_tcbCreate(task4, "tsk4", task4, t4_stack, MIN_STACK_SIZE,T4_PRIORITY,NULL,Task4_Array,1);
	//ROSA_TCBlist_print();

	//Start the ROSA kernel
	ROSA_Extended_Start();
	/* Execution will never return here */
	while(1);
}
