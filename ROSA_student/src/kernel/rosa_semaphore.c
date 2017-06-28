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

#include "kernel/rosa_semaphore.h"

#define Success 0
#define Undefined_error_1 1
#define Undefined_error_2 2


semHandle_s * LOCKED_SEMAPHORE_LIST;

/***********************************************************
 * Semaphore
 *
 * Comment:
 * 	
 * 	
 * 
 **********************************************************/

 // ----- "Public functions" -----

unsigned int ROSA_semaphoreCreate(semHandle* sem)
{     
	semHandle_s* temp;
	
	*sem = (semHandle) malloc(sizeof(semHandle_s));
	temp = *sem;

	temp->isTaken = 0; // 0 means false = not taken
	temp->owner_tcb = NULL;
	temp->registered_tcbs = NULL;
	temp->registered_tcb_count = 0; // how many tasks wants to use the semaphore
	temp->priority = 0;
	temp->next_sem = NULL;
	temp->previous_sem = NULL;
	
	*sem = temp;

	/* If everything went right return success*/
	return Success;
}

unsigned int ROSA_semaphoreTake(semHandle sTakeHandle)
{   
	//semHandle_s* temp = sTakeHandle;
	/* judge if task already takes a semaphore  */
	interruptDisable();
	/* Check if the semaphore exists */
	if(sTakeHandle == NULL)
	{
		/* Undefined_error_1 */
		interruptEnable();
		return Undefined_error_1;
		//return Success;
	}
	else if(canTakeSemaphore(EXECTASK, sTakeHandle))
	{   
		/* Insert the the semaphore in the linked list of locked semaphore */
		/* Set this field to true */

		insertSemaphoreToLockedList(sTakeHandle);
		insertSemIntoTakenList(EXECTASK, sTakeHandle);
		sTakeHandle->isTaken = 1;
		sTakeHandle->owner_tcb = EXECTASK;
		/* Raise the priority of the task to the one of the semaphore */
		setTaskPriorty(EXECTASK);

		/* return Success */
		interruptEnable();
		return Success;	
	}
	else
	{
		interruptEnable();
		return Undefined_error_2;
	}
}

unsigned int ROSA_semaphoreGive(semHandle sGiveHandle)
{ 
	//semHandle_s* temp = *sGiveHandle;
	/* Check if the semaphore exists */
	interruptDisable();
	if(sGiveHandle != NULL && sGiveHandle->isTaken != 0)
	{
		
		removeSemFromLockedList(sGiveHandle);

		/* Set the priority to the highest locked semaphore, if not present,
		   set it to its original one */
		removeSemFromTakenList(EXECTASK, sGiveHandle);

		/* Remove the task from the task linked list of the semaphore */
		sGiveHandle->isTaken = 0;
		sGiveHandle->owner_tcb = NULL;

		/* Raise the priority of the task to the one of the semaphore */
		setTaskPriorty(EXECTASK);
		
		rescheduleExecutingtTask();
		
		/* return Success*/
		interruptEnable();
		return Success;
	}
	else
	{
		/* throw an Undefined_error_1. */
		interruptEnable();
		return Undefined_error_1;
	}
}

	
// ----- "Private functions" -----

void setTaskPriorty(tcb* task)
{
	/* If the task didn't take any semaphore or give a semaphore
	   its priority should go back to the original. */
	if(task->taken_sem == NULL)
	{
		task->priority = task->originalPriority;
	}
	/* If the task take a semaphore it should raise it's priority to the one of the semaphore.*/
	else
	{
		task->priority = task->taken_sem->sem->priority;
	}
	
}

void setSemphorePriorty(semHandle_s * sem)
{
	sem->priority = sem->registered_tcbs->task->priority;
}

unsigned int canTakeSemaphore(tcb * task, semHandle sem)
{   
	struct semHandle_s* temp;
	/* if it is the first one or it has priority grater than the locked semaphore it can take it */
	//if (task->priority>LOCKED_SEMAPHORE_LIST->priority||LOCKED_SEMAPHORE_LIST==NULL)
	//return 1;
	if(sem->isTaken)
	{
		return 0;
	}
	
	temp = LOCKED_SEMAPHORE_LIST;
	while(temp != NULL)
	{
		if(task->priority <= temp->priority && task != temp->owner_tcb)
		{
			return 0;
		}
		if (temp->next_sem != NULL)
		{
			temp = temp->next_sem;
		}
		else temp = NULL;
	}

	return 1;
}

linked_tcb*  initialize_linked_tcb(tcb * task)
{
	linked_tcb* newtaskRegister = malloc(sizeof(linked_tcb));

	newtaskRegister->task = task;
	newtaskRegister->next_tcb = NULL;
	newtaskRegister->previous_tcb = NULL;
	
	return newtaskRegister;
}

 linked_sem* initialize_linked_sem(semHandle_s * sem)
{
	linked_sem* semList = malloc(sizeof(linked_sem));
	
	semList->sem = sem;
	semList->next_sem = NULL;
	semList->previous_sem = NULL;

	return semList;
}

void ROSA_semaphoreRegister(tcb * task, semHandle_s * sem)
{
	 linked_tcb * semTask;
	 semTask = initialize_linked_tcb(task);
	 insertTaskIntoSemTaskRegisterList(semTask, sem);
	 insertSemIntoTaskSemRegisterList(task, sem);
	 setSemphorePriorty(sem);
	 sem->registered_tcb_count += 1;
}

void  ROSA_semaphoreUnregister(tcb * task, semHandle_s * sem)
{
	//traverse the list and delete the aimed task (element)
	task->registered_sem->sem->registered_tcb_count--;
	removeTaskFromSemTaskRegisterList(task, sem);
    removeSemFromTaskSemRegisterList(task, sem);
	setSemphorePriorty(sem);
	
}

void insertSemIntoTakenList(tcb * task, semHandle_s * sem)
{   
	linked_sem* temp_sem;// = malloc(sizeof(struct linked_sem));
	linked_sem * currenttakensemaphore = initialize_linked_sem(sem);
	currenttakensemaphore->next_sem = NULL;
	currenttakensemaphore->previous_sem = NULL;
	
	//takenSemaphoreList * Temp;

	/* No element in the list */
	if(task->taken_sem == NULL)
	{
		task->taken_sem = currenttakensemaphore;
	}
	else if(task->taken_sem->sem->priority < currenttakensemaphore->sem->priority)
	{
		currenttakensemaphore->next_sem =task->taken_sem ;
		task->taken_sem->previous_sem = currenttakensemaphore;
		task->taken_sem =  currenttakensemaphore;
	}
	else
	{
		temp_sem = task->taken_sem ;
		
		/* Iterate through the list to find where to insert it.*/
		while(temp_sem->next_sem != NULL && currenttakensemaphore->sem->priority <= temp_sem->next_sem->sem->priority)
		{
			temp_sem = temp_sem->next_sem;
		}
		
		/* If it is a middle situation (not at the beginning, neither at the end)*/
		if(temp_sem->next_sem != NULL){
			temp_sem->next_sem->previous_sem = currenttakensemaphore;
			currenttakensemaphore->next_sem = temp_sem->next_sem;
		}
		else currenttakensemaphore->next_sem = NULL;
		
		// set temp next to the new semaphore and set new semaphore previous to temp
		temp_sem->next_sem = currenttakensemaphore;
		currenttakensemaphore->previous_sem = temp_sem;
	}
}

void removeSemFromTakenList(tcb * task, semHandle_s * sem)
{
	linked_sem * temp;
	/* The list and the element does not exists return an error */
	if(task->taken_sem == NULL || sem == NULL)
	{
		return Undefined_error_2; // Return from void function... GG
	}

	// Find the semaphore in the taken list
	temp = task->taken_sem;
	while(temp->sem != sem && temp->next_sem != NULL)
	{
		temp = temp->next_sem;
	}
	if (temp->sem != sem) // Semaphore was not in the list
	{
		return Undefined_error_2;
	}
	
	/* First element */
	if(temp->sem == task->taken_sem->sem)
	{
		if(task->taken_sem->next_sem == NULL)
		{
			task->taken_sem = NULL;
		}
		/* First element + other elements present in the list */
		else
		{
			task->taken_sem = task->taken_sem->next_sem;
			task->taken_sem->previous_sem = NULL;
		}
	}
	else if(temp->next_sem == NULL) // If it is the last one element of the list
	{
		/* remove the last element by pointing the previous next to null*/
		temp->previous_sem->next_sem = NULL;
	}
	else //If we are in the middle of the list
	{
		//reset the pointer of each structure after the delete of the structure.
		temp->previous_sem->next_sem = temp->next_sem;
		temp->next_sem->previous_sem = temp->previous_sem;
	}

	// Free the memory of temp
	free(temp);
}

void insertSemIntoTaskSemRegisterList(tcb * task, semHandle_s * sem)
{
	linked_sem* temp_sem;// = malloc(sizeof(struct linked_sem));
	linked_sem* semToRegister = initialize_linked_sem(sem);
	semToRegister->next_sem = NULL;
	semToRegister->previous_sem = NULL;
	
	/* No element in the list */
	if(task->registered_sem == NULL)
	{
		task->registered_sem = semToRegister;
	}
	else if(task->registered_sem->sem->priority < semToRegister->sem->priority)
	{
		semToRegister->next_sem = task->registered_sem;
		task->registered_sem->previous_sem = semToRegister;
		task->registered_sem = semToRegister;
	}
	else{
		temp_sem = task->registered_sem;
		
		/* Iterate through the list to find where to insert it.*/
		while(temp_sem->next_sem != NULL && semToRegister->sem->priority <= temp_sem->next_sem->sem->priority)
		{
			temp_sem = temp_sem->next_sem;
		}
		
		/* If it is a middle situation (not at the beginning, neither at the end)*/
		if(temp_sem->next_sem != NULL)
		{
			temp_sem->next_sem->previous_sem = semToRegister;
			semToRegister->next_sem = temp_sem->next_sem;
		}
		else semToRegister->next_sem = NULL;
		
		// set temp next to the new semaphore and set new semaphore previous to temp
		temp_sem->next_sem = semToRegister;
		semToRegister->previous_sem = temp_sem;
	}
	//free(Temp);
}

void removeSemFromTaskSemRegisterList(tcb * task, semHandle_s * sem)
{
	linked_sem * temp;

	/* The list and the element does not exists return an error */
	if(task->registered_sem == NULL || sem == NULL)
	{
		return Undefined_error_2 ;
	}

	// Find the semaphore in the taken list
	temp = task->registered_sem;
	while(temp->sem != sem && temp->next_sem != NULL)
	{
		temp = temp->next_sem;
	}
	if (temp->sem != sem) // Semaphore was not in the list
	{
		return Undefined_error_2;
	}
	
	/* First element + no other element present in the list */
	if(temp->sem == task->registered_sem->sem)
	{
		if(task->registered_sem->next_sem == NULL)
		{
			task->registered_sem = NULL;
		}
		/* First element + other elements present in the list */
		else
		{
			task->registered_sem = task->registered_sem->next_sem;
			task->registered_sem->previous_sem = NULL;
		}
	}
	else if(temp->next_sem == NULL) /* If it is the last one element of the list */
	{		
		/* remove the last element by pointing the previous next to null*/
		temp->previous_sem->next_sem = NULL;
	}
	else /* If we are in the middle of the list */
	{
		//reset the pointer of each structure after the delete of the structure.
		temp->previous_sem->next_sem = temp->next_sem;
		temp->next_sem->previous_sem = temp->previous_sem;
	}
	free(temp);
}

void insertSemaphoreToLockedList(semHandle_s * lockedSem)
{
	semHandle_s * semTemp;
	lockedSem->next_sem = NULL;
	lockedSem->previous_sem = NULL;
	
	/* No element in the list */
	if(LOCKED_SEMAPHORE_LIST == NULL)
	{
		LOCKED_SEMAPHORE_LIST = lockedSem;
	}
	else if(LOCKED_SEMAPHORE_LIST->priority < lockedSem->priority)
	{
		lockedSem->next_sem = LOCKED_SEMAPHORE_LIST;
		LOCKED_SEMAPHORE_LIST->previous_sem = lockedSem;
		LOCKED_SEMAPHORE_LIST = lockedSem;
	}
	else
	{
		semTemp = LOCKED_SEMAPHORE_LIST;
		
		/* Iterate through the list to find where to insert it.*/
		while(semTemp->next_sem != NULL && lockedSem->priority <= semTemp->next_sem->priority )
		{
			semTemp = semTemp->next_sem;
		}
		
		/* If it is a middle situation (not at the beginning, neither at the end)*/
		if(semTemp->next_sem !=NULL)
		{
			semTemp->next_sem->previous_sem = lockedSem;
			lockedSem->next_sem = semTemp->next_sem;
		}
		else lockedSem->next_sem = NULL;
		
		// set temp next to the new semaphore and set new semaphore previous to temp
		semTemp->next_sem = lockedSem;
		lockedSem->previous_sem = semTemp;
	}
}

void removeSemFromLockedList(semHandle_s * lockedSem)
{
	/* The list and the element does not exists return an error */
	if(LOCKED_SEMAPHORE_LIST == NULL|| lockedSem == NULL)
	{
		return Undefined_error_2 ;
	}

	/* First element + no other element present in the list */
	if(lockedSem == LOCKED_SEMAPHORE_LIST)
	{
		if(LOCKED_SEMAPHORE_LIST->next_sem == NULL)
		{
			LOCKED_SEMAPHORE_LIST = NULL;
		}
		/* First element + other elements present in the list */
		else
		{
			LOCKED_SEMAPHORE_LIST = LOCKED_SEMAPHORE_LIST->next_sem;
			LOCKED_SEMAPHORE_LIST->previous_sem = NULL;
		}
	}
	/* If it is the last element of the list */
	else if(LOCKED_SEMAPHORE_LIST->next_sem == NULL)
	{
		lockedSem->previous_sem->next_sem = NULL;
	}
	/* If we are in the middle of the list */
	else
	{
		lockedSem->previous_sem->next_sem = lockedSem->next_sem;
		lockedSem->next_sem->previous_sem = lockedSem->previous_sem;
	}
	
	lockedSem->next_sem = NULL;
	lockedSem->previous_sem = NULL;
}

/* This function is used to insert a task into the linked list of tasks which are going to take a semaphore */
void insertTaskIntoSemTaskRegisterList(linked_tcb* semTask, semHandle_s * sem)
{
	linked_tcb* temp;

	semTask->next_tcb = NULL;
	semTask->previous_tcb = NULL;

	/* No element in the list */
	if(sem->registered_tcbs == NULL)
	{
		sem->registered_tcbs = semTask;
	}
	else if(sem->registered_tcbs->task->priority < semTask->task->priority)
	{
		semTask->next_tcb = sem->registered_tcbs;
		sem->registered_tcbs->previous_tcb = semTask;
		sem->registered_tcbs = semTask;
	}
	else{
		temp = sem->registered_tcbs;
		
		/* Iterate through the list to find where to insert it.*/
		while(temp->next_tcb != NULL && semTask->task->priority <= temp->next_tcb->task->priority)
		{
			temp = temp->next_tcb;
		}
		
		/* If it is a middle situation (not at the beginning, neither at the end)*/
		if(temp->next_tcb != NULL){
			temp->next_tcb->previous_tcb = semTask;
			semTask->next_tcb = temp->next_tcb;
		}
		else semTask->next_tcb = NULL;
		
		// set temp next to the new semaphore and set new semaphore previous to temp
		temp->next_tcb = semTask;
		semTask->previous_tcb = temp;
	}
}

/* This function is used to remove a task from the linked list of tasks which are going to take a semaphore */
void removeTaskFromSemTaskRegisterList(tcb * task, semHandle_s * sem)
{
	linked_tcb * temp;

	/* The list and the element does not exists return an error */
	if(sem->registered_tcbs == NULL || task == NULL)
	{
		return Undefined_error_2 ;
	}

	// Find the semaphore in the taken list
	temp = sem->registered_tcbs;
	while(temp->task != task && temp->next_tcb != NULL)
	{
		temp = temp->next_tcb;
	}
	if (temp->task != task) // task was not in the list
	{
		return Undefined_error_2;
	}
	 
	/* First element + no other element present in the list */
	if(temp->task == sem->registered_tcbs->task)
	{
		if(sem->registered_tcbs->next_tcb == NULL)
		{
			sem->registered_tcbs = NULL;
		}
		else
		{
			sem->registered_tcbs = sem->registered_tcbs->next_tcb;
			sem->registered_tcbs->previous_tcb = NULL;
		}
	}
	else if(temp->next_tcb == NULL)
	{
		temp->previous_tcb->next_tcb = NULL;
	}
	else
	{
		temp->previous_tcb->next_tcb = temp->next_tcb;
		temp->next_tcb->previous_tcb = temp->previous_tcb;
		 
	}
	free(temp);
 }

///* This function is called when we have to delete a task and we have to remove the semaphore 
   //from the locked list and from the Taken list as well */
void removeMultiSemForDeleteTask(tcb * task)
{
	linked_sem* temp_taken = task->taken_sem;
	semHandle_s* temp_registered = NULL;
	if (task->registered_sem != NULL)
	{
		temp_registered = task->registered_sem->sem;
	}

	while (temp_taken != NULL) // If any semaphores are taken by the current task, release them
	{
		temp_taken->sem->isTaken = 0;
		temp_taken->sem->owner_tcb = NULL;
		removeSemFromLockedList(temp_taken->sem);
		removeSemFromTakenList(task, temp_taken->sem);

		temp_taken = task->taken_sem;
		//if (temp_taken == NULL)
		//{
			//break;
		//}
		//else temp_taken = temp_taken->next_sem;
	}

	while (temp_registered != NULL) // If any semaphores are taken by the current task, release them
	{
		ROSA_semaphoreUnregister(task, temp_registered);

		if(task->registered_sem != NULL)
			temp_registered = task->registered_sem->sem;
		else break;
		//if (temp_registered->next_sem == NULL)
		//{
			//break;
		//}
		//else temp_registered = temp_registered->next_sem;
	}
}

