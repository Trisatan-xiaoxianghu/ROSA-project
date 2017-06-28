/*
 * Test1.c
 *
 * Created: 2016-12-13 21:44:03
 *  Author: jean_
 */ 
 #include "Test1.h"
 #include "drivers/led.h"
 #include "kernel/rosa_ker.h"
 #include "kernel/rosa_semaphore.h"

 semHandle s;
 tcbHandle test1_1_h;
 tcbHandle test1_2_h;

 void test1_1(void)
 {
	 while(1)
	 {
		 while(ROSA_semaphoreTake(s) != 0) 
		 {
			ROSA_taskDelay(10);
		 }
		 ledOn(LED0_GPIO);
		 ROSA_tcbSuspend(&test1_1_h);
		 ROSA_taskDelay(1000);
		 ledOff(LED0_GPIO);
		 ROSA_semaphoreGive(s);
		 ROSA_taskDelay(1000);
	 }
 }
 void test1_2(void)
 {
	 ROSA_taskDelay(3000);
	 while(1)
	 {
		 while(ROSA_semaphoreTake(s) != 0)
		 {
			 ROSA_taskDelay(10);
		 }
		 ledOn(LED1_GPIO);
		 ROSA_semaphoreGive(s);
		 ROSA_taskDelay(1000);
		 ledOff(LED1_GPIO);
	 }
 }
 #define TASKSTACKSIZE 0x40
 int runTest_1(void)
 {
	 //Initialize the ROSA kernel
	 ROSA_Extended_Init();
	 //timerPeriodSet(10);
	 //Create tasks and install them into the ROSA kernel
	 ROSA_semaphoreCreate(&s);
	 ROSA_tcbCreate(&test1_1_h, "T1", test1_1, 0, TASKSTACKSIZE, 1, 0, &s, 1);
	 ROSA_tcbCreate(&test1_2_h, "T2", test1_2, 0, TASKSTACKSIZE, 1, 0, &s, 1);

	 ROSA_Extended_Start();
	 ///* Execution will never return here */
	 while(1);
 }