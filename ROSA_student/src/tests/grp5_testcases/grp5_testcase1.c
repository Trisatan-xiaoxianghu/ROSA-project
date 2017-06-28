/*
 * grp5_testcase1.c
 *
 * Checks dynamic task creation/deletion and if it there is any issues with deleting a task wich holds a semaphore
 *
 * Created: 2016-12-13 10:20:59
 *  Author: Skoog
 */ 

#include "grp5_testcase1.h"
//Standard library includes
#include <avr32/io.h>

//The include for the API functions
#include "kernel/ROSA_Extended.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"

#define TC1_T1_STACK_SIZE 0x40
#define TC1_T1_PRIO 2
static int t1_stack[TC1_T1_STACK_SIZE];
static tcbHandle t1_tcb;

#define TC1_T2_STACK_SIZE 0x40
#define TC1_T2_PRIO 1
static int t2_stack[TC1_T2_STACK_SIZE];
static tcbHandle t2_tcb;

#define TC1_T3_STACK_SIZE 0x10
#define TC1_T3_PRIO 1
static int t3_stack[TC1_T3_STACK_SIZE];
static tcbHandle t3_tcb;

#define TC1_T4_STACK_SIZE 0x10
#define TC1_T4_PRIO 1
static int t4_stack[TC1_T4_STACK_SIZE];
static tcbHandle t4_tcb;

semHandle s1;

/*************************************************************
 * Task2
 * takes sem 1 and then delays indefinitely
 * is to be created/deleted by Task1 dynamically
 ************************************************************/
void tc1_task2(void)
{
	while(ROSA_semaphoreTake(s1))
	{
	}
	
	while(1)
	{
		ROSA_taskDelay(10);
	}
}

/*************************************************************
 * Task1
 * creates Task1, turns on LED0, delays 1sec
 * deletes Task1, turns off LED0, tries to take sem1, delays 1sec
 ************************************************************/
void tc1_task1(void)
{
	ticktime t = 0;
	while(1)
	{
		ROSA_tcbCreate(&t2_tcb, "t2", tc1_task2, t2_stack, TC1_T2_STACK_SIZE, TC1_T2_PRIO, NULL, &s1, 1);
		ledOn(LED0_GPIO);
		ROSA_taskDelayUntil(&t, 1000);
		
		ROSA_tcbDelete(&t2_tcb);
		
		ledOff(LED0_GPIO);
		
		while(ROSA_semaphoreTake(s1))
		{
		}
		ROSA_semaphoreGive(s1);
		ROSA_taskDelayUntil(&t, 1000);
	}
}

/*************************************************************
 * Task3
 * LED2 lights up
 * LED3 goes dark
 ************************************************************/
void tc1_task3()
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
void tc1_task4()
{
	while(1)
	{
		ledOn(LED3_GPIO);
		ledOff(LED2_GPIO);
	}
}

void test_case_1()
{
	//Initialize the ROSA kernel
	ROSA_Extended_Init();
	ROSA_semaphoreCreate(&s1);
	
	//LED0 should turn on/off every second
	ROSA_tcbCreate(&t1_tcb, "t1", tc1_task1, t1_stack, TC1_T1_STACK_SIZE, TC1_T1_PRIO, NULL, &s1, 1);
	//Task 3 & 4 only lights up LED2/LED3 in order to see that other tasks can run concurrently to Task 1 & 2
	ROSA_tcbCreate(&t3_tcb, "t3", tc1_task3, t3_stack, TC1_T3_STACK_SIZE, TC1_T3_PRIO, NULL, NULL, 0);
	ROSA_tcbCreate(&t4_tcb, "t4", tc1_task4, t4_stack, TC1_T4_STACK_SIZE, TC1_T4_PRIO, NULL, NULL, 0);
	
	//Start the ROSA kernel
	ROSA_Extended_Start();
	/* Execution will never return here */
	while(1);
}