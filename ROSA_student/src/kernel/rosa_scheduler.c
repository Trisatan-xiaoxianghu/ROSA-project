/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///' '///,'/////',/////'  /////'/;,

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

#include "kernel/rosa_scheduler.h"

#define Success 0
#define Undefined_Error 1
#define Null_Pointer_Error 2
#define Invalid_Task_State_Error 3

/***********************************************************
 * READY_LIST
 *
 * Comment:
 * 	Global list that contains the ready tasks (sorted by priority).
 **********************************************************/
tcb * READY_LIST;

/***********************************************************
 * WAITING_LIST
 *
 * Comment:
 * 	Global list containing the waiting tasks (sorted by wakeuptime).
 **********************************************************/
tcb * WAITING_LIST;

/***********************************************************
 * SUSPENSION_LIST
 *
 * Comment:
 * 	Global list containing the suspended tasks (Not sorted).
 **********************************************************/
 tcb * SUSPENSION_LIST;


void scheduler(void)
{
	//Find the next task to execute
	//EXECTASK = EXECTASK->nexttcb;
	EXECTASK = READY_LIST;
}

void tcbInsertReady(tcb * tcbTask)
{
	tcb * tcbTemp;
	tcbTask->nexttcb = NULL;
	tcbTask->previoustcb = NULL;
	
	/* Is it the first TCB or should it be first in the queue? */
	if(READY_LIST == NULL)
	{
		READY_LIST = tcbTask;
	}
	else if(READY_LIST->priority < tcbTask->priority)
	{
		tcbTask->nexttcb = READY_LIST;
		READY_LIST->previoustcb = tcbTask;
		READY_LIST = tcbTask;
	}
	else
	{
		tcbTemp = READY_LIST;
		
		/* Finds the TCB before the new task to be inserted */
		while(tcbTemp->nexttcb != NULL && tcbTask->priority <= tcbTemp->nexttcb->priority)
		{
			/* Iterates the pointer (++) */
			tcbTemp = tcbTemp->nexttcb;
		}
		
		/* Connects the nodes */
		if(tcbTemp->nexttcb != NULL)
		{
			// Sets nexts previous to the new task and the next of the new to that task
			tcbTemp->nexttcb->previoustcb = tcbTask;
			tcbTask->nexttcb = tcbTemp->nexttcb;
		}
		else tcbTask->nexttcb = NULL;

		// set temp next to the new task and set new task previous to temp
		tcbTemp->nexttcb = tcbTask;
		tcbTask->previoustcb = tcbTemp;
	}
	tcbTask->status = READY;
}

void tcbInsertWaitingList(unsigned int wakeup)
{
	tcb * tcbTask = READY_LIST;

	//removes the task from the ready list
	READY_LIST = READY_LIST->nexttcb;
	READY_LIST->previoustcb = NULL;

	tcbInsertWaitingListInternal(tcbTask, wakeup);
}

void tcbInsertWaitingListInternal(tcb * tcbTask, unsigned int wakeup)
{
	tcb * tcbTemp;
	//tcb * tcbTask;
	tcbTemp = WAITING_LIST;
	//tcbTask = READY_LIST;
	
	//sets task parameters
	tcbTask->nexttcb = NULL;
	tcbTask->previoustcb = NULL;
	tcbTask->wakeup = wakeup;

	/* Is it the first TCB or should it be first in the queue? */
	if(tcbTemp == NULL)
	{
		WAITING_LIST = tcbTask;
	}
	else if(tcbTask->wakeup < tcbTemp->wakeup)
	{
		tcbTask->nexttcb = tcbTemp;
		tcbTemp->previoustcb = tcbTask;
		WAITING_LIST = tcbTask;
	}
	else
	{
		/* Finds the TCB before the new task to be inserted */
		while(tcbTemp->nexttcb != NULL && tcbTask->wakeup >= tcbTemp->nexttcb->wakeup)
		{
			/* Iterates the pointer (++) */
			tcbTemp = tcbTemp->nexttcb;
		}
		
		/* Connects the nodes */
		if(tcbTemp->nexttcb != NULL)
		{
			// Sets nexts previous to the new task and the next of the new to that task
			tcbTemp->nexttcb->previoustcb = tcbTask;
			tcbTask->nexttcb = tcbTemp->nexttcb;
		}
		else tcbTask->nexttcb = NULL;

		// set temp next to the new task and set new task previous to temp
		tcbTemp->nexttcb = tcbTask;
		tcbTask->previoustcb = tcbTemp;
	}
	
	tcbTask->status = WAITING;
}

void updateWaitingList(unsigned int currTime)
{
	tcb * tcbTemp = WAITING_LIST;
	
	while(tcbTemp != NULL && currTime >= tcbTemp->wakeup)
	{
		tcbTemp->wakeup = 0;
		WAITING_LIST = tcbTemp->nexttcb;
		WAITING_LIST->previoustcb = NULL;

		tcbInsertReady(tcbTemp);
		tcbTemp = WAITING_LIST;
	}
}

int ROSA_tcbSuspend(tcbHandle * task)
{
	tcb * tcbTemp = (tcb*)*task;

	if(tcbTemp == NULL)
		return Null_Pointer_Error;

	tcbRemoveFromCurrentList(tcbTemp);
	
	tcbTemp->status = SUSPENDED;
	tcbTemp->nexttcb = SUSPENSION_LIST;
	SUSPENSION_LIST = tcbTemp;

	return Success;
}

int ROSA_tcbResume(tcbHandle * task)
{
	tcb * tcbTemp = (tcb*)*task;

	if(tcbTemp == NULL)
		return Null_Pointer_Error;
	if (tcbTemp->status != SUSPENDED)
		return Invalid_Task_State_Error;

	tcbRemoveFromList(tcbTemp, &SUSPENSION_LIST);

	// TODO - Have to implement an insert function to waiting list that takes a task if waiting time is greater than 0;
	int t = ROSA_getTicks();
	if(tcbTemp->wakeup > t)
	{
		tcbInsertWaitingListInternal(tcbTemp, tcbTemp->wakeup);
	}
	else
	{
		tcbTemp->wakeup = 0;
		tcbInsertReady(tcbTemp);
	}

	return Success;
}

int tcbRemoveFromCurrentList(tcb * task)
{
	if(task == NULL)
		return Null_Pointer_Error;

	if (task->status == READY) tcbRemoveFromList(task, &READY_LIST);
	else if (task->status == WAITING) tcbRemoveFromList(task, &WAITING_LIST);
	else if (task->status == SUSPENDED) tcbRemoveFromList(task, &SUSPENSION_LIST);
	else return Invalid_Task_State_Error;

	return Success;
}

int tcbRemoveFromList(tcb * task, tcb ** List)
{
	tcb * l = *List;

	if(*List == NULL || task == NULL)
	{
		return Null_Pointer_Error;
	}
	
	if (task == l)	
	{
		if (l->nexttcb == NULL)
		{
			l = NULL;
		}
		else // If task is equal to the first element in the list and not the only one in list
		{
			l = l->nexttcb;
			l->previoustcb = NULL;
		}
	}
	else if (l->nexttcb == NULL)
	{
		task->previoustcb->nexttcb = NULL;
	}
	else
	{
		task->previoustcb->nexttcb = task->nexttcb;
		task->nexttcb->previoustcb = task->previoustcb;
	}

	*List = l;
	task->previoustcb = NULL;
	task->nexttcb = NULL;
	return Success;
}

void rescheduleExecutingtTask()
{
	tcb * temp = EXECTASK;
	interruptDisable();
	tcbRemoveFromList(temp, &READY_LIST);
	tcbInsertReady(temp);
	interruptEnable();

	if(EXECTASK != READY_LIST)
	{
		ROSA_yield();
		//ROSA_yieldFromISR();
	}
}