/*
 * grp5_testcase3.c
 *
 * Checks if task suspension is handled correctly in regards to waiting tasks, ie. if a task gets suspended and resumed
 * when it's in the waiting queue, it should return to the waiting queue and not the ready queue.
 * If the task is released to the ready queue instead of the waiting queue and is allowed to run
 * before the waiting time is over, the red LED (LED4) will switch on meaning the test has failed.
 *
 * Created: 2016-12-13 11:04:41
 *  Author: Skoog
 */ 

#include "grp5_testcase3.h"

//Standard library includes
#include <avr32/io.h>

//The include for the API functions
#include "kernel/ROSA_Extended.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"


#define TC3_T1_STACK_SIZE 0x40
#define TC3_T1_PRIO 3
static int t1_stack[TC3_T1_STACK_SIZE];
static tcbHandle t1_tcb;

#define TC3_T2_STACK_SIZE 0x40
#define TC3_T2_PRIO 3
static int t2_stack[TC3_T2_STACK_SIZE];
static tcbHandle t2_tcb;

#define TC3_T3_STACK_SIZE 0x10
#define TC3_T3_PRIO 1
static int t3_stack[TC3_T3_STACK_SIZE];
static tcbHandle t3_tcb;

#define TC3_T4_STACK_SIZE 0x10
#define TC3_T4_PRIO 1
static int t4_stack[TC3_T4_STACK_SIZE];
static tcbHandle t4_tcb;

/*************************************************************
 * Task1
 * LED0 lights up, delays 1 sec
 * LED0 goes dark, delays 1 sec
 ************************************************************/
void tc3_task1(void)
{
	ticktime t = 0;

	while(1)
	{
		ledOn(LED0_GPIO);
		ROSA_taskDelayUntil(&t, 1000);
		if(ROSA_getTicks() < t)
		{
			ledOn(LED4_GPIO);
		}
		ledOff(LED0_GPIO);
		ROSA_taskDelayUntil(&t, 1000);
		if(ROSA_getTicks() < t)
		{
			ledOn(LED4_GPIO);
		}
	}
}

/*************************************************************
 * Task2
 * LED1 lights up, suspends and resumes Task 1, delays 1 sec
 * LED1 goes dark, suspends and resumes Task 1, delays 1 sec
 ************************************************************/
void tc3_task2(void)
{
	ticktime t = 0;

	while(1)
	{
		ledOn(LED1_GPIO);
		//suspends/resumes in order to see if it results in an incorrect execution for task1 (ie messing with waiting queues)
		ROSA_tcbSuspend(&t1_tcb);
		ROSA_tcbResume(&t1_tcb);
		ROSA_taskDelayUntil(&t, 500);
		//suspends/resumes in order to see if it results in an incorrect execution for task1 (ie messing with waiting queues)
		ROSA_tcbSuspend(&t1_tcb);
		ROSA_tcbResume(&t1_tcb);
		ledOff(LED1_GPIO);
		ROSA_taskDelayUntil(&t, 500);
	}
}

/*************************************************************
 * Task3
 * LED2 lights up
 * LED3 goes dark
 ************************************************************/
void tc3_task3()
{
	while(1)
	{
		ledOn(LED2_GPIO);
		ledOff(LED3_GPIO);
	}
}

/*************************************************************
 * Task4
 * LED3 lights up
 * LED2 goes dark
 ************************************************************/
void tc3_task4()
{
	while(1)
	{
		ledOn(LED3_GPIO);
		ledOff(LED2_GPIO);
	}
}

void test_case_3()
{
	//Initialize the ROSA kernel
	ROSA_Extended_Init();

	// Suspend / Resume tasks (LED 0/1) LED0 should turn on/off every second while LED1 should turn on/off every half a second
	ROSA_tcbCreate(&t1_tcb, "tsk1", tc3_task1, t1_stack, TC3_T1_STACK_SIZE, TC3_T1_PRIO, NULL, NULL, 0);
	ROSA_tcbCreate(&t2_tcb, "tsk2", tc3_task2, t2_stack, TC3_T2_STACK_SIZE, TC3_T2_PRIO, NULL, NULL, 0);
	//Task 3 & 4 only lights up LED2/LED3 in order to see that other tasks can run concurrently to Task 1 & 2
	ROSA_tcbCreate(&t3_tcb, "t3", tc3_task3, t3_stack, TC3_T3_STACK_SIZE, TC3_T3_PRIO, NULL, NULL, 0);
	ROSA_tcbCreate(&t4_tcb, "t4", tc3_task4, t4_stack, TC3_T4_STACK_SIZE, TC3_T4_PRIO, NULL, NULL, 0);
	
	//Start the ROSA kernel
	ROSA_Extended_Start();
	/* Execution will never return here */
}