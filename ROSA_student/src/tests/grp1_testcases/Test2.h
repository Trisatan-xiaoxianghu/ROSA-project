/*
 * Test2.h
 *
 * Created: 2016-12-13 21:43:52
 *  Author: jean_
 */ 


#ifndef TEST2_H_
#define TEST2_H_
#include "rosa_config.h"

//INCLUDE ANY .h files necessary to get the entire API (or gather them in one file and replace the path defined below)
//#define PATH_TO_ROSA_API_INCLUDE "kernel/extended_rosa.h"
//#include PATH_TO_ROSA_API_INCLUDE

//extern typedef void * semHandle;
//extern typedef void * tcbHandle;
//extern typedef unsigned int ticktime;
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

/*Test 2: Tries to make the program run out of memory. If LED4 lights up during the test a task creation has failed.
*/
int runTest_2(void);

#endif /* TEST1_H_ */