#ifndef ROSA_EXTENDED_H_
#define ROSA_EXTENDED_H_

#include "rosa_config.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_semaphore.h"
#include "kernel/rosa_tim.h"

extern int ROSA_Extended_Init (void);
extern int ROSA_Extended_Start (void);
extern int ROSA_tcbCreate(tcbHandle*task_h, char tcbName[NAMESIZE], void* tcbFunction, int* tcbStack, int tcbStackSize, unsigned int priority, void *tcbArg, semaphore *semaphores, int Sem_amount);
extern int ROSA_tcbDelete (tcbHandle* task_h);
extern int ROSA_tcbSuspend (tcbHandle* task_h);
extern int ROSA_tcbResume (tcbHandle* task_h);
extern ticktime ROSA_getTicks(void);
extern int ROSA_taskDelay(ticktime t);
extern int ROSA_taskDelayUntil(ticktime *start, ticktime t);
extern unsigned int ROSA_semaphoreCreate(semHandle *sem);
extern unsigned int ROSA_semaphoreTake(semHandle sTakeHandle);
extern unsigned int ROSA_semaphoreGive(semHandle sGiveHandle);



#endif /* ROSA_EXTENDED_H_ */