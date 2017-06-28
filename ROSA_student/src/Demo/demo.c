/*
 * demo.c
 *
 * Created: 2016-12-22 11:03:36
 *  Author: granh
 */ 

 #include "Demo/demo.h"

 //Standard library includes
#include <avr32/io.h>

//Kernel includes
#include "kernel/rosa_ker.h"
#include "kernel/rosa_semaphore.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"
#include "drivers/button.h"
#include "drivers/pot.h"

//Include configuration
#include "rosa_config.h"

#define SUCCESS 0

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

semaphore sems[1];
semHandle s1;

char* itoa(int val, int base){

    static char buf[32] = {0};
    int i = 30;

    for(; val && i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];

    return &buf[i+1];
}

/*************************************************************
 * Potentiometer task, reads pot value
 ************************************************************/
int potentiometerValue = 0;
void task_gaspedal(void)
{
	unsigned int t = ROSA_getTicks();
	while(1) {
		potentiometerValue = potGetValue();
		ROSA_taskDelayUntil(&t, 10);
	}
}

/*************************************************************
 * Headlight Task
 ************************************************************/
int headlightOn = 0;
void task_headlight(void)
{
	unsigned int t = ROSA_getTicks();
	int buttonDown = isButton(PUSH_BUTTON_0);

	int buttonClicked = 0;
	int buttonReleased = 1;

	while(1) {
		buttonDown = isButton(PUSH_BUTTON_0);

		if(buttonDown && buttonReleased)
		{
			headlightOn = !headlightOn;
			buttonClicked = 1;
			buttonReleased = 0;
		}
		if (!buttonDown && buttonClicked)
		{
			buttonReleased = 1;
		}

		if(headlightOn)
		{
			ledOn(LED1_GPIO);
		}
		else
		{
			ledOff(LED1_GPIO);
		}
	}
}

int braking = 0;
int brake_active = 0;
/*************************************************************
 * ABS
 ************************************************************/
void task_abs(void)
{
	unsigned int t = ROSA_getTicks();
	int buttonDown = 0;

	while(1) {
		
		while (ROSA_semaphoreTake(s1) != SUCCESS)
		{
			ROSA_taskDelayUntil(&t, 5);
		}
		if (braking)
		{
			ledOn(LED4_GPIO);
			brake_active = 1;
			ROSA_taskDelayUntil(&t, 100);
			ledOff(LED4_GPIO);
			brake_active = 0;
			ROSA_taskDelayUntil(&t, 50);
		}
		ROSA_semaphoreGive(s1);
		ROSA_taskDelayUntil(&t, 5);
	}
}

/*************************************************************
 * Brake
 ************************************************************/

void task_brake(void)
{
	unsigned int t = ROSA_getTicks();
	int buttonDown = 0;
	int tcbCreated = 0;

	while(1) {
		buttonDown = isButton(PUSH_BUTTON_1);

		while (ROSA_semaphoreTake(s1) != SUCCESS)
		{
			ROSA_taskDelay(5);
		}

		if(buttonDown && !tcbCreated)
		{
			ledOn(LED5_GPIO);
			ledOn(LED4_GPIO);
			braking = 1;
			brake_active = 1;
			//if(!tcbCreated)
			{
				ROSA_tcbCreate(&t4_tcb, "tsk2", task_abs, t4_stack, T4_STACK_SIZE, 1, NULL, sems, 1);
				tcbCreated = 1;
			}
		}
		else if (!buttonDown && tcbCreated)
		{
			ledOff(LED5_GPIO);
			ledOff(LED4_GPIO);
			braking = 0;
			brake_active = 0;
			//if (tcbCreated)
			{
				ROSA_tcbDelete(&t4_tcb);
				tcbCreated = 0;
			}
		}

		ROSA_semaphoreGive(s1);

		ROSA_taskDelayUntil(&t, 5);
	}
}

/*************************************************************
 * Steering
 ************************************************************/
int steeringLeft = 0;
int steeringRight = 0;
void task_steering(void)
{
	unsigned int t = ROSA_getTicks();

	while(1) {

		steeringLeft = isJoystickLeft();
		steeringRight = isJoystickRight();

		if(steeringLeft)
		{
			ledOn(LED6_GPIO);
		}
		else
		{
			ledOff(LED6_GPIO);
		}
		if(steeringRight)
		{
			ledOn(LED7_GPIO);
		}
		else
		{
			ledOff(LED7_GPIO);
		}

		ROSA_taskDelayUntil(&t, 5);
	}
}

/*************************************************************
 * Task1
 * LED0 lights up
 * LED1 goes dark
 ************************************************************/
void task_serialWriter(void)
{
	unsigned int t = ROSA_getTicks();
	unsigned int stall;

	//int potValue = 0;
	char *pot;
	char brake;
	char brakeActive;
	char headlight;
	char leftSteering;
	char rightSteering;
	
	while(1) {
		//potValue = potGetValue();
		pot = itoa(potentiometerValue, 10);
		headlight = headlightOn + '0';
		brake = braking + '0';
		brakeActive = brake_active + '0';
		leftSteering = steeringLeft + '0';
		rightSteering = steeringRight + '0';

		ledOn(LED0_GPIO);
		usartWriteLine(USART, pot);
		usartWriteChar(USART, ',');
		usartWriteChar(USART, headlight);
		usartWriteChar(USART, ',');
		usartWriteChar(USART, brake);
		usartWriteChar(USART, ',');
		usartWriteChar(USART, brakeActive);
		usartWriteChar(USART, ',');
		usartWriteChar(USART, leftSteering);
		usartWriteChar(USART, ',');
		usartWriteChar(USART, rightSteering);
		usartWriteChar(USART, '|');
		ROSA_taskDelayUntil(&t, 20);
		ledOff(LED0_GPIO);
		ROSA_taskDelayUntil(&t, 20);
	}
}


/*************************************************************
 * Main function
 ************************************************************/
void run_demo(void)
{
	//Initialize the ROSA kernel
	ROSA_Extended_Init();

	buttonInit();
	potInit();
	joystickInit();

	
	ROSA_semaphoreCreate(&s1);
	sems[0] = s1;

	//Create tasks and install them into the ROSA kernel
	// Gas pedal task, reads potentiometer
	ROSA_tcbCreate(&t1_tcb, "tsk1", task_gaspedal, t1_stack, T1_STACK_SIZE, 1, NULL, NULL, 0);
	// Headlight button task
	ROSA_tcbCreate(&t2_tcb, "tsk2", task_headlight, t2_stack, T2_STACK_SIZE, 1, NULL, NULL, 0);
	// Break handler task
	ROSA_tcbCreate(&t3_tcb, "tsk3", task_brake, t3_stack, T3_STACK_SIZE, 2, NULL, sems, 1);

	// t4 is used for the ABS task 

	// Serial writer task
	ROSA_tcbCreate(&t5_tcb, "tsk5", task_serialWriter, t5_stack, T5_STACK_SIZE, 2, NULL, NULL, 0);
	// Steering task
	ROSA_tcbCreate(&t6_tcb, "tsk6", task_steering, t6_stack, T6_STACK_SIZE, 2, NULL, NULL, 0);


	//Start the ROSA kernel
	ROSA_Extended_Start();
	/* Execution will never return here */
	while(1);
}