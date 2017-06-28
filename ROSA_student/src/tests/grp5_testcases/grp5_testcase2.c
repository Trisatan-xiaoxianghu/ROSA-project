/*
 * grp5_testcase2.c
 *
 * Tests if delay function on same priority tasks using the same semaphore can cause issues in the queues,
 * ie. if T1 takes sem1, gets put into waiting queue, T2 tries to take sem1 (get denied), T1 is woken up but
 * put behind T2 in the queue as they have the same priority. Now T2 waits for T1 to give up the semaphore, but 
 * T1 will never run as it is behind T2 in the ready queue, thus creating a deadlock.
 *
 * Created: 2016-12-13 10:26:31
 *  Author: Skoog
 */ 

#include "grp5_testcase2.h"
//Standard library includes
#include <avr32/io.h>

//The include for the API functions
#include "kernel/ROSA_Extended.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"


#define TC2_T1_STACK_SIZE 0x40
#define TC2_T1_PRIO 2
static int t1_stack[TC2_T1_STACK_SIZE];
static tcbHandle t1_tcb;

#define TC2_T2_STACK_SIZE 0x40
#define TC2_T2_PRIO 2
static int t2_stack[TC2_T2_STACK_SIZE];
static tcbHandle t2_tcb;

#define TC2_T3_STACK_SIZE 0x20
#define TC2_T3_PRIO 1
static int t3_stack[TC2_T3_STACK_SIZE];
static tcbHandle t3_tcb;

#define TC2_T4_STACK_SIZE 0x20
#define TC2_T4_PRIO 1
static int t4_stack[TC2_T4_STACK_SIZE];
static tcbHandle t4_tcb;

semHandle s1;

/*************************************************************
 * Task1
 * takes sem1, LED1 goes dark
 * LED0 lights up, delays 1 sec
 * gives back sem1, delays 50ms to give other tasks time
 ************************************************************/
void tc2_task1()
{
	while(1)
	{
		while(ROSA_semaphoreTake(s1))
		{
			ROSA_taskDelay(2);
		}
		ledOff(LED1_GPIO);
		ledOn(LED0_GPIO);
		ROSA_taskDelay(1000);
		ROSA_semaphoreGive(s1);
		ROSA_taskDelay(50);
	}
}

/*************************************************************
 * Task2
 * takes sem1, LED0 goes dark
 * LED1 lights up, delays 1 sec
 * gives back sem1, delays 50ms to give other tasks time
 ************************************************************/
void tc2_task2()
{
	while(1)
	{
		while(ROSA_semaphoreTake(s1))
		{
			ROSA_taskDelay(2);
		}
		ledOff(LED0_GPIO);
		ledOn(LED1_GPIO);
		ROSA_taskDelay(1000);
		ROSA_semaphoreGive(s1);
		ROSA_taskDelay(50);
	}
}

/*************************************************************
 * Task3
 * LED2 lights up
 * LED3 goes dark
 ************************************************************/
void tc2_task3()
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
void tc2_task4()
{
	while(1)
	{
		ledOn(LED3_GPIO);
		ledOff(LED2_GPIO);
	}
}

void test_case_2()
{
	//Initialize the ROSA kernel
	ROSA_Extended_Init();
	ROSA_semaphoreCreate(&s1);
	
	//Task 1 & 2 should take turns each second lightning up their respective led (LED0/LED1)
	ROSA_tcbCreate(&t1_tcb, "t1", tc2_task1, t1_stack, TC2_T1_STACK_SIZE, TC2_T1_PRIO, NULL, &s1, 1);
	ROSA_tcbCreate(&t2_tcb, "t2", tc2_task2, t2_stack, TC2_T2_STACK_SIZE, TC2_T2_PRIO, NULL, &s1, 1);
	//Task 3 & 4 only lights up LED2/LED3 in order to see that other tasks can run concurrently to Task 1 & 2
	ROSA_tcbCreate(&t3_tcb, "t3", tc2_task3, t3_stack, TC2_T3_STACK_SIZE, TC2_T3_PRIO, NULL, NULL, 0);
	ROSA_tcbCreate(&t4_tcb, "t4", tc2_task4, t4_stack, TC2_T4_STACK_SIZE, TC2_T4_PRIO, NULL, NULL, 0);
	
	//Start the ROSA kernel
	ROSA_Extended_Start();
	/* Execution will never return here */
	while(1);
}