/*
 * Test1.h
 *
 * Created: 2016-12-13 21:43:52
 *  Author: jean_
 */ 


#ifndef TEST1_H_
#define TEST1_H_
#include "rosa_config.h"
//#define PATH_TO_ROSA_API_INCLUDE "kernel/extended_rosa.h"
//#include PATH_TO_ROSA_API_INCLUDE
//typedef void * semHandle;
//typedef void * tcbHandle;
//typedef unsigned int ticktime;
//
//extern int ROSA_Extended_Init(void);
//extern int ROSA_Extended_Start(void);
//extern unsigned int ROSA_tcbCreate(tcbHandle * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize, unsigned int priority, void * tcbArg, semHandle * semaphores, int Sem_amount);
//extern unsigned int ROSA_tcbSuspend(tcbHandle * tcbTask);
//extern unsigned int ROSA_tcbResume(tcbHandle * tcbTask);
//extern unsigned int ROSA_tcbDelete(tcbHandle * tcbTask);
//extern ticktime ROSA_GetTicks();  
//extern int ROSA_taskDelay (ticktime Period);
//extern int ROSA_taskDelayUntil (ticktime *start, ticktime Period);
//extern unsigned int ROSA_semaphoreCreate(semHandle* sem);
//extern unsigned int ROSA_semaphoreTake(semHandle sem);
//extern unsigned int ROSA_semaphoreGive(semHandle sem);

/*Test 1: Tries to deadlock the system by suspending a task while it's holding a semaphore, making the other task try to grab the semaphore forever.
*/
int runTest_1(void);

#endif /* TEST1_H_ */