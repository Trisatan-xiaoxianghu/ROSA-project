/*
 * test_semaphore.c
 *
 * Created: 2016-12-09 08:46:00
 *  Author: Andreas
 */ 

#include "test_semaphores.h"

#include "kernel/rosa_semaphore.h"

#define Success 0

#define nrSemaphores 3

//Standard library includes
#include <avr32/io.h>

//Kernel includes
#include "kernel/rosa_ker.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"

//Include configuration
#include "rosa_config.h"

//Data blocks for the tasks
#define SEM_T1_STACK_SIZE 0x40
static int sem_t1_stack[SEM_T1_STACK_SIZE];
static tcb sem_t1_tcb;

#define SEM_T2_STACK_SIZE 0x40
static int sem_t2_stack[SEM_T2_STACK_SIZE];
static tcb sem_t2_tcb;

#define SEM_T3_STACK_SIZE 0x40
static int sem_t3_stack[SEM_T3_STACK_SIZE];
static tcb sem_t3_tcb;

semHandle s1, s2, s3;

void sem_task1(void)
{
	while(1) 
	{	
		
		if(ROSA_semaphoreTake(s1) == Success)
		{
			if(ROSA_semaphoreTake(s2) == Success)
			{
				//if(ROSA_semaphoreTake(s3) == Success)
				{
					ledOn(LED0_GPIO);
					ROSA_taskDelay(50);
					ledOff(LED0_GPIO);
					//ROSA_semaphoreGive(s3);
				}
				ROSA_semaphoreGive(s2);
			}
			ROSA_semaphoreGive(s1);
			ROSA_taskDelay(10);
		}
	}
}

void sem_task2(void)
{
	while(1) 
	{
		
		//ROSA_tcbDelete(&sem_t1_tcb);
		if(ROSA_semaphoreTake(s2) == Success)
		{
			if(ROSA_semaphoreTake(s1) == Success)
			{
				//if(ROSA_semaphoreTake(s3) == Success)
				{
					ledOn(LED1_GPIO);
					ROSA_taskDelay(50);
					ledOff(LED1_GPIO);
					//ROSA_semaphoreGive(s3);
				}
				ROSA_semaphoreGive(s1);
			}
			ROSA_semaphoreGive(s2);
			ROSA_taskDelay(100);
		}
	}
}

void sem_task3(void)
{
	while(1)
	{
		if(ROSA_semaphoreTake(s3) == Success)
		{
			ledOn(LED2_GPIO);
			ROSA_taskDelay(50);
			ledOff(LED2_GPIO);
			ROSA_semaphoreGive(s3);
			ROSA_taskDelay(10);
		}
	}
}

void test_semaphore()
{
	semaphore semst1t2[2];
	semaphore semst3[1];
	
	//Initialize the ROSA kernel
	ROSA_Extended_Init();
	ROSA_semaphoreCreate(&s1);
	ROSA_semaphoreCreate(&s2);
	ROSA_semaphoreCreate(&s3);
	
	semst1t2[0] = s1;
	semst1t2[1] = s2;
	//semst1t2[2] = s3;
	semst3[0] = s3;
	
	ROSA_tcbCreate(&sem_t1_tcb, "tsk1", sem_task1, sem_t1_stack, SEM_T1_STACK_SIZE, 1, NULL, semst1t2, 2);
	ROSA_tcbCreate(&sem_t2_tcb, "tsk2", sem_task2, sem_t2_stack, SEM_T2_STACK_SIZE, 2, NULL, semst1t2, 2);

	ROSA_tcbCreate(&sem_t3_tcb, "tsk3", sem_task3, sem_t3_stack, SEM_T3_STACK_SIZE, 3, NULL, semst3, 1);
	
	//Start the ROSA kernel
	ROSA_Extended_Start();
	/* Execution will never return here */
}

