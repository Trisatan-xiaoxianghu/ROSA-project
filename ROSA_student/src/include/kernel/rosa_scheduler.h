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

#ifndef _scheduler_H_
#define _scheduler_H_

#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"

extern tcb * READY_LIST;
extern tcb * WAITING_LIST;
extern tcb * SUSPENSION_LIST;

void scheduler(void);

void tcbInsertReady(tcb * tcbTask);

void tcbInsertWaitingList(unsigned int wakeup);

void tcbInsertWaitingListInternal(tcb * tcbTask, unsigned int wakeup);

void updateWaitingList(unsigned int currTime);

int ROSA_tcbSuspend(tcbHandle * task);

int ROSA_tcbResume(tcbHandle * task);

int tcbRemoveFromCurrentList(tcb * tcbTask);

int tcbRemoveFromList(tcb * task, tcb ** List);

void rescheduleExecutingtTask();

#endif /* _scheduler_H_ */
