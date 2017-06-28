/*
* This test case will test if the user is aware of the time of execution after the ticktime parameter
* for ROSA_taskDelayUntil is collected. If the execution takes too long and the delay is to short the delay time
* will be passed by the clock. If the error is not collected and a 0 is returned a red led will blink.
*/

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

#include "test.h"

//Data blocks for the tasks
#define T1_STACK_SIZE 0x100
static int t1_stack[T1_STACK_SIZE];
static tcb t1_tcb;





void taskToDelay(void)
{
	ticktime start = ROSA_getTicks();

	while(1) {

		int count = 0;
		while (count < 10000){
			count++;
		}
		ledOn(LED2_GPIO);
		int o = ROSA_taskDelayUntil(&start, 1);
		usartWriteChar(USART, o + '0');

		ledOff(LED2_GPIO);
		ROSA_taskDelayUntil(&start, 1000);
		
		if (o == 0){
			ledOn(LED4_GPIO);
			delay_ms(1000);
			ledOff(LED4_GPIO);
			delay_ms(1000);
		}
	}
}
void delayUnt_test(void)
{


	//Initialize the ROSA kernel
	ROSA_Extended_Init();

	void* arguments = NULL;
	//semHandle* semaphore = NULL;
	int sem_number = 0;

	ROSA_tcbCreate(&t1_tcb,"prod", taskToDelay, t1_stack, T1_STACK_SIZE, 1, arguments, NULL/*semaphore*/, sem_number);
	
	ROSA_Extended_Start();
	while(1);
}

