/*
* Test2.c
*
* Created: 2016-12-13 21:44:03
*  Author: jean_
*/
#include "Test2.h"
#include "drivers/led.h"

#include "kernel/rosa_ker.h"
#include "kernel/rosa_semaphore.h"

semHandle s;
tcbHandle test2_1_h;
#define TASKSTACKSIZE 0x40
#define CREATETASKAMOUNT 20
#define TASKPRIO 0
void test2_2(void)
{
	ROSA_taskDelay(100);
	while(1)
	{
		asm("");
	}
}
void test2_1(void)
{
	int i = 0;
	tcbHandle tasks[CREATETASKAMOUNT];
	while(1)
	{
		for(i = 0; i < CREATETASKAMOUNT; i++)
		{
			if(ROSA_tcbCreate(&tasks[i], "T2", test2_2, 0, TASKSTACKSIZE, TASKPRIO, 0, 0, 0) != 0)
			{
				ledOn(LED4_GPIO);
			}
		}
		for(i = 0; i < CREATETASKAMOUNT; i++)
		{
			ROSA_tcbDelete(&tasks[i]);
		}
	}
}
int runTest_2(void)
{
	//Initialize the ROSA kernel
	ROSA_Extended_Init();
	//timerPeriodSet(10);
	//Create tasks and install them into the ROSA kernel
	ROSA_tcbCreate(&test2_1_h, "T1", test2_1, 0, 0, 1, 0, 0, 0);

	ROSA_Extended_Start();
	///* Execution will never return here */
	while(1);
}