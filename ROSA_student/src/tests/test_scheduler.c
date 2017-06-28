/*
 * test_scheduler.c
 *
 * Created: 2016-12-09 08:45:29
 *  Author: Andreas
 */ 

#include "test_scheduler.h"

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
#define T1_STACK_SIZE 0x40
static int t1_stack[T1_STACK_SIZE];
static tcb t1_tcb;

#define T2_STACK_SIZE 0x40
static int t2_stack[T2_STACK_SIZE];
static tcb t2_tcb;

#define T3_STACK_SIZE 0x40
static int t3_stack[T3_STACK_SIZE];
static tcb t3_tcb;

#define T4_STACK_SIZE 0x40
static int t4_stack[T4_STACK_SIZE];
static tcb t4_tcb;

#define T5_STACK_SIZE 0x40
static int t5_stack[T5_STACK_SIZE];
static tcb t5_tcb;

#define T6_STACK_SIZE 0x40
static int t6_stack[T6_STACK_SIZE];
static tcb t6_tcb;

#define T7_STACK_SIZE 0x40
static int t7_stack[T7_STACK_SIZE];
static tcb t7_tcb;


void task5(void);

/*************************************************************
 * Task1
 * LED0 lights up
 * LED1 goes dark
 ************************************************************/
void task1(void)
{
	unsigned int t = ROSA_getTicks();
	//unsigned int stall;
	
	while(1) {
		ledOn(LED0_GPIO);
		//stall = timerGetTicks() + 100;
		//while(stall > timerGetTicks())
		//{
			////stalls the task for 1000ms
		//}
		ROSA_taskDelayUntil(&t, 500);
		ledOff(LED0_GPIO);
		ROSA_taskDelayUntil(&t, 500);
		//timerDelayUntil(&t, 350); funkar inte än d?vi ej har waiting queue 
		//[not working when we don't have waiting queue]
		//ROSA_yield();
	}
}

/*************************************************************
 * Task2
 * LED0 goes dark
 * LED1 lights up
 ************************************************************/
void task2(void)
{
	int x = 0;
	//tcbHandler tcbH = (tcbHandler)&t1_tcb;
	//ROSA_tcbDelete(tcbH);
	while(1) {
		ledOff(LED1_GPIO);
		while (x < 100000)
		{
			x++;
		}
		x = 0;
		ledOn(LED1_GPIO);
		while (x < 100000)
		{
			x++;
		}
		x = 0;
		//delay_ms(1000);
		//ROSA_yield();
	}
}

void task3(void)
{
	int x = 0;
	//tcbHandler tcbH = (tcbHandler)&t1_tcb;
	//ROSA_tcbDelete(tcbH);
	while(1) {
		ledOff(LED2_GPIO);
		while (x < 100000)
		{
			x++;
		}
		x = 0;
		ledOn(LED2_GPIO);
		while (x < 100000)
		{
			x++;
		}
		x = 0;
		//ledOn(LED2_GPIO);
		//delay_ms(1000);
		//ROSA_yield();
	}
}

void task4(void)
{
	unsigned int t = ROSA_getTicks();
	ROSA_tcbCreate(&t5_tcb, "tsk5", task5, t5_stack, T5_STACK_SIZE, 4, NULL, NULL, 0);
	while(1) {
		ledOn(LED3_GPIO);
		ROSA_taskDelayUntil(&t, 2000);
		ledOff(LED3_GPIO);
		ROSA_taskDelayUntil(&t, 2000);
	}
	//ROSA_tcbDelete(&t5_tcb);
}

void task5(void)
{
	unsigned int t = ROSA_getTicks();

	while(1) {
		ledOn(LED5_GPIO);
		ROSA_taskDelayUntil(&t, 100);
		ledOff(LED5_GPIO);
		ROSA_taskDelayUntil(&t, 100);
	}
}

void task6(void)
{
	unsigned int t = ROSA_getTicks();

	while(1) {
		ledOn(LED7_GPIO);
		ROSA_tcbSuspend(&t7_tcb);
		ROSA_taskDelayUntil(&t, 333);
		ledOff(LED7_GPIO);
		ROSA_tcbResume(&t7_tcb);
		ROSA_taskDelayUntil(&t, 333);
	}
}

void task7(void)
{
	unsigned int t = ROSA_getTicks();

	while(1) {
		ledOn(LED6_GPIO);
		ROSA_taskDelayUntil(&t, 50);
		ledOff(LED6_GPIO);
		ROSA_taskDelayUntil(&t, 50);
	}
}


void test_scheduler()
{		
		//Initialize the ROSA kernel
		ROSA_Extended_Init();
		
		// Default task test (LED 0-2)
		ROSA_tcbCreate(&t1_tcb, "tsk1", task1, t1_stack, T1_STACK_SIZE, 2, NULL, NULL, 0);
		ROSA_tcbCreate(&t2_tcb, "tsk2", task2, t2_stack, T2_STACK_SIZE, 1, NULL, NULL, 0);
		ROSA_tcbCreate(&t3_tcb, "tsk3", task3, t3_stack, T3_STACK_SIZE, 1, NULL, NULL, 0);

		// Dynamic task Create task (LED 3/5)
		ROSA_tcbCreate(&t4_tcb, "tsk4", task4, t4_stack, T4_STACK_SIZE, 3, NULL, NULL, 0);


		// Suspend / Resume tasks (LED 6/7) When the green LED is on, the red LED should not flash since task7 is suspended
		ROSA_tcbCreate(&t6_tcb, "tsk6", task6, t6_stack, T6_STACK_SIZE, 3, NULL, NULL, 0);
		ROSA_tcbCreate(&t7_tcb, "tsk7", task7, t7_stack, T7_STACK_SIZE, 3, NULL, NULL, 0);
		
		//Start the ROSA kernel
		ROSA_Extended_Start();
		/* Execution will never return here */
}