// This test tries to initialize and start ROSA again from a 
// running task. If the LED is still blinking, this means that
// multiple calls to ROSA_Extended_Init are dealt with and 
// that the program continues to work correctly.

#include "kernel/rosa_ker.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/usart.h"

//Include configuration
#include "rosa_config.h"

#include "test.h"

#define T_STACK_SIZE 0x100
static int t_stack[T_STACK_SIZE];
tcbHandle t_tcb;

void task(void){
	while(1){
		int init_error = ROSA_Extended_Init();
		int start_error = ROSA_Extended_Start();
		
		if(init_error != 0 && start_error != 0){
			ledOn(LED0_GPIO);
			delay_ms(1000);
			ledOff(LED0_GPIO);
			delay_ms(1000);
		}
	}
}

int scheduler_init_test(void){
	ROSA_Extended_Init();
	
	
	void* arguments = NULL;
	//semHandle* semaphores = NULL;
	int semaphore_number = 0;
	ROSA_tcbCreate(&t_tcb, "tsk1", task, t_stack, T_STACK_SIZE, 3, arguments, NULL/*semaphores*/, semaphore_number);
	
	ROSA_Extended_Start();
	
	while(1);
}