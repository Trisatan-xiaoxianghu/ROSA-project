/*
 * test_task_handler.c
 *
 * Created: 2016-12-09 08:45:46
 *  Author: Andreas
 */ 

#include "test_task_handler.h"

/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// ///''\\,
             ,///' '///,'/////',/////'  /////'\\,

    Copyright 2010 Marcus Jansson <mjansson256@yahoo.se>

    This file is part of ROSA - Realtime Operating System for AVR32.

    ROSA is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROSA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ROSA.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
/* Tab size: 4 */

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

/*************************************************************
 * Task1
 * LED0 lights up
 * LED1 goes dark
 ************************************************************/
void task_handler_t1(void)
{
	unsigned int t = ROSA_getTicks();
	unsigned int stall;
	
	while(1) {
		ledOff(LED1_GPIO);
		ledOff(LED2_GPIO);
		ledOn(LED0_GPIO);
	}
}

/*************************************************************
 * Task2
 * LED0 goes dark
 * LED1 lights up
 ************************************************************/
void task_handler_t2(void)
{
	int x = 0;
	//tcbHandler tcbH = (tcbHandler)&t1_tcb;
	//ROSA_tcbDelete(tcbH);
	while(1) {
		ledOff(LED0_GPIO);
		ledOff(LED2_GPIO);
		ledOn(LED1_GPIO);
		//delay_ms(1000);
		//ROSA_yield();
	}
}


void task_handler_t4(void)
{
	while(1)
	{
		ledOff(LED0_GPIO);
		ledOff(LED1_GPIO);
		ledOn(LED2_GPIO);
	}
}

void task_handler_t3(void)
{
	int x = 0;
	//tcbHandler tcbH = (tcbHandler)&t1_tcb;
	//ROSA_tcbDelete(tcbH);
	
	while(1) {
		
		ROSA_tcbCreate(&t4_tcb, "tsk4", task_handler_t4, t4_stack, T4_STACK_SIZE, 2, NULL, NULL, 0);
		ROSA_taskDelay(1000);
		ROSA_tcbDelete(&t4_tcb);
		ROSA_taskDelay(1000);
	}
}

/*************************************************************
 * Main function
 ************************************************************/
void test_task_handler(void)
{
	//Initialize the ROSA kernel
	ROSA_Extended_Init();

	//Create tasks and install them into the ROSA kernel
	ROSA_tcbCreate(&t1_tcb, "tsk1", task_handler_t1, t1_stack, T1_STACK_SIZE, 2, NULL, NULL, 0);
	//ROSA_tcbInstall(&t1_tcb);
	ROSA_tcbCreate(&t2_tcb, "tsk2", task_handler_t2, t2_stack, T2_STACK_SIZE, 2, NULL, NULL, 0);
	ROSA_tcbCreate(&t3_tcb, "tsk3", task_handler_t3, t3_stack, T3_STACK_SIZE, 4, NULL, NULL, 0);
	//ROSA_tcbInstall(&t2_tcb);

	//Start the ROSA kernel
	ROSA_Extended_Start();
	/* Execution will never return here */
	while(1);
}
