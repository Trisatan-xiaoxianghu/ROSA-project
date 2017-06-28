/*****************************************************************************

                 ///////,   .////    .///' /////,
                ///' ./// ///'///  ///,     '///
               ///////'  ///,///   '/// //;';//,
             ,///' ////,'/////',/////'  /////'/;,

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

//Kernel includes
#include "kernel/rosa_def.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"
#include "kernel/rosa_scheduler.h"
#include "kernel/rosa_semaphore.h"

//Driver includes
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"
#include "drivers/usart.h"

/***********************************************************
 * TCBLIST
 *
 * Comment:
 * 	Global variables that contain the list of TCB's that
 * 	have been installed into the kernel with ROSA_tcbInstall()
 **********************************************************/
tcb * TCBLIST;

/***********************************************************
 * EXECTASK
 *
 * Comment:
 * 	Global variables that contain the current running TCB.
 **********************************************************/
tcb * EXECTASK;



/***********************************************************
 * EXECTASK
 *
 * Comment:
 * 	Global variables that contain the current running TCB.
 **********************************************************/
//TODO: tcb * LAST_LIST; den där vi pratade om p?API mötet

#define IDLE_STACK_SIZE 0x10
static int idle_stack[IDLE_STACK_SIZE];
static tcb idle_tcb;

int rosa_initialized = 0;
int rosa_started = 0;

void idle_task(void)
{
	int counter = 0;
	while (1) 
	{	
		counter++;
	}
}

/***********************************************************
 * ROSA_init
 *
 * Comment:
 * 	Initialize the ROSA system
 *
 **********************************************************/
void ROSA_init(void)
{
	//Do initialization of I/O drivers
	ledInit();									//LEDs
	buttonInit();								//Buttons
	joystickInit();								//Joystick
	potInit();									//Potentiometer
	usartInit(USART, &usart_options, FOSC0);	//Serial communication

	//Start with empty TCBLIST and no EXECTASK.
	TCBLIST = NULL;
	EXECTASK = NULL;

	//Initialize the timer to 100 ms period.
	//...
	//timerInit(100);
	//...
	//timerPeriodSet(100);
}

int ROSA_Extended_Init()
{
	if (!rosa_initialized)
	{
		rosa_initialized = 1;
		ROSA_init();
		ROSA_tcbCreate(&idle_tcb, "idleTask", idle_task, idle_stack, IDLE_STACK_SIZE, 0, NULL, NULL, 0);
		timerInit(CLOCK_PERIOD);
		interruptInit();
		
		return 0;
	}
	return 1;
}

/***********************************************************
 * ROSA_tcbCreate
 *
 * Comment:
 * 	Create the TCB with correct values.
 *
 **********************************************************/

int ROSA_tcbCreate(tcbHandle * tcbTaskHandler, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize, unsigned int priority, void * tcbArg, semaphore * semaphores, int sem_amount)
{
	int i;
	
	tcb * tcbTask = (tcb*)malloc(sizeof(tcb));
	
	*tcbTaskHandler = (tcbHandle)tcbTask;

	//Initialize the tcb with the correct values
	for(i = 0; i < NAMESIZE; i++) {
		//Copy the id/name
		tcbTask->id[i] = tcbName[i];
	}
	
	//Set priority
	tcbTask->priority = priority;
	
	//Set the original priority
	tcbTask->originalPriority = priority;
    
	//Initialize the structure for the list of taken semaphores.
   // struct takenSemaphoreList c;
	//tcbTask->semaphoresTakenByThisTask=&c;
	tcbTask->taken_sem = NULL;

	//Initialize the structure for the list of semaphores is going to take.	
	//struct allSemaphoresIsGoingToTakeList d;
	//tcbTask->semaphoresIsGoingToBeTakenByThisTask=&d;
	tcbTask->registered_sem = NULL;

	//Don't link this TCB anywhere yet.
	tcbTask->nexttcb = NULL;

	//Set the task function start and return address.
	tcbTask->staddr = tcbFunction;
	tcbTask->retaddr = (int)tcbFunction;

	//Set up the stack.
	tcbTask->datasize = tcbStackSize;
	tcbTask->dataarea = tcbStack + tcbStackSize;
	tcbTask->saveusp = tcbTask->dataarea;

	//Set the initial SR.
	tcbTask->savesr = ROSA_INITIALSR;

	tcbTask->tcbArg = tcbArg;

	//Initialize context.
	contextInit(tcbTask);
	
	ROSA_tcbInstall(tcbTask);
	
	// Register semaphores
	for (i = 0; i < sem_amount; i++)
	{
		ROSA_semaphoreRegister(tcbTask/*(tcbTaskHandler)*/, semaphores[i]);
	}

	return 0;
}

int ROSA_tcbDelete(tcbHandle * tcbTaskHandler)
{
	tcb * tcbTask = (tcb *)(*tcbTaskHandler);
	
	interruptDisable();

	//removeFromlist(tcbTask), scheduler fucntion call
	tcbRemoveFromCurrentList(tcbTask);
	removeMultiSemForDeleteTask(tcbTask);
	
	//The system does not like it when these pointers are freed
	//free(tcbTask->dataarea);
	//free(tcbTask->saveusp);
	//free(tcbTaskHandler); //Do not think this is something we need to do, as the OS does not dynamically give memory space to tasks, they are specified in task creation
	
	if(EXECTASK == tcbTask)
	{
		free(*tcbTaskHandler);
		interruptEnable(); //no idea where else to put interruptEnable?
		ROSA_yield();
	}
	
	free(*tcbTaskHandler);
	interruptEnable();
	
	return 0;
}

/***********************************************************
 * ROSA_tcbInstall
 *
 * Comment:
 * 	Install the TCB into the TCBLIST.
 *
 **********************************************************/
void ROSA_tcbInstall(tcb * tcbTask)
{
	//tcb * tcbTmp;

	/* Is this the first tcb installed? */
	//if(TCBLIST == NULL) {
		//TCBLIST = tcbTask;
		//TCBLIST->nexttcb = tcbTask;			//Install the first tcb
		//tcbTask->nexttcb = TCBLIST;			//Make the list circular
	//}
	//else {
		//tcbTmp = TCBLIST;					//Find last tcb in the list
		//while(tcbTmp->nexttcb != TCBLIST) {
			//tcbTmp = tcbTmp->nexttcb;
		//}
		//tcbTmp->nexttcb = tcbTask;			//Install tcb last in the list
		//tcbTask->nexttcb = TCBLIST;			//Make the list circular
	//}
	
	tcbInsertReady(tcbTask);
}

int ROSA_Extended_Start(void)
{
	if(rosa_initialized && !rosa_started)
	{
		TCBLIST = READY_LIST;
		previousTask = READY_LIST;
		
		rosa_started = 1;
		timerStart();
		ROSA_start();
		// If this reached, something went wrong, return Error;
		return 1;
	}	
	
	return 1;
}

