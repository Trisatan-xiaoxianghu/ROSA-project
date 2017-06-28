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

#ifndef ROSA_SEMAPHORE_H_
#define ROSA_SEMAPHORE_H_

#include "kernel/rosa_ker.h"
#include "stdlib.h"
/***********************************************************
 * scheduler
 *
 * Comment:
 * 
 * 	
 * 	
 **********************************************************/

/* Global variables */
struct semHandle_s * LOCKED_SEMAPHORE_LIST;
//taskRegister * TASK_PRIORITY_LIST;

//tasks to be registered in one semaphore.
typedef struct linked_tcb{
	tcb * task;
	struct linked_tcb * next_tcb;
	struct linked_tcb * previous_tcb;
}linked_tcb;

typedef struct semHandle_s{
	int isTaken;
	tcb * owner_tcb;
	//struct taskRegister * tasksPriorityList;
	linked_tcb * registered_tcbs;
	int registered_tcb_count;
	unsigned int priority;
	struct semHandle_s * next_sem;
	struct semHandle_s * previous_sem;
} semHandle_s;

typedef semHandle_s* semHandle;



unsigned int ROSA_semaphoreCreate(semHandle * sem);

unsigned int ROSA_semaphoreTake(semHandle sTakeHandle);

unsigned int ROSA_semaphoreGive(semHandle sGiveHandle);

void removeTaskFromSemTaskRegisterList(tcb * task, semHandle_s * sem);

linked_tcb*  initialize_linked_tcb(tcb * task);

void ROSA_semaphoreRegister(tcb * task, semHandle_s * sem);

void  ROSA_semaphoreUnregister(tcb * task, semHandle_s * sem);

void insertSemaphoreToLockedList(semHandle_s * lockedSem);

void insertTaskIntoSemTaskRegisterList(linked_tcb* semTask, semHandle_s * sem);

void removeSemFromLockedList(semHandle_s * lockedSem);

unsigned int canTakeSemaphore(tcb * task, semHandle sem);

linked_sem*  initialize_linked_sem(semHandle_s * sem);

void insertSemIntoTakenList(tcb * task, semHandle_s * sem);

void removeSemFromTakenList(tcb * task, semHandle_s * sem);

void removeMultiSemForDeleteTask(tcb * task);

linked_sem*  initialize_linked_sem(semHandle_s * sem);

void setSemphorePriorty(semHandle_s * sem);

void setTaskPriorty(tcb* task);

void removeSemFromTaskSemRegisterList(tcb * task, semHandle_s * sem);

void insertSemIntoTaskSemRegisterList(tcb * task, semHandle_s * sem);


#endif /* ROSA_SEMAPHORE_H_ */