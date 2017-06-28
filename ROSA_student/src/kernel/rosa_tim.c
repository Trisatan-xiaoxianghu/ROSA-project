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

#include "rosa_config.h"
#include "drivers/delay.h"
#include "kernel/rosa_int.h"
#include "kernel/rosa_scheduler.h"

unsigned long ticks;

/***********************************************************
 * timerInterruptHandler
 *
 * Comment:
 * 	This is the basic timer interrupt service routine.
 **********************************************************/
__attribute__((__interrupt__))
void timerISR(void)
{
	int sr;
	volatile avr32_tc_t * tc = &AVR32_TC;

	//Read the timer status register to determine if this is a valid interrupt
	sr = tc->channel[0].sr;
	if(sr & AVR32_TC_CPCS_MASK)
	{
		ticks++;
		updateWaitingList(ticks * CLOCK_PERIOD);
		
 		if (EXECTASK == previousTask && EXECTASK->nexttcb != NULL && EXECTASK->priority == EXECTASK->nexttcb->priority)
		{
			tcbRemoveFromList(EXECTASK, &READY_LIST);
			tcbInsertReady(EXECTASK);
		}

		// Set previous task to the first task in Ready List
		previousTask = READY_LIST;

		//Checks if a new task should be executed, TODO: check if we should interleave (if EXECTASK->nxttcb->priority...)
		if(EXECTASK != READY_LIST)
		{
			ROSA_yieldFromISR();
		}
	}
		//ROSA_yieldFromISR();
}


/***********************************************************
 * timerPeriodSet
 *
 * Comment:
 * 	Set the timer period to 'ms' milliseconds.
 *
 **********************************************************/
int timerPeriodSet(unsigned int ms)
{

	int rc, prescale;
	int f[] = { 2, 8, 32, 128 };
	//FOSC0 / factor_prescale * time[s];
	prescale = AVR32_TC_CMR0_TCCLKS_TIMER_CLOCK5;
	rc = FOSC0 / f[prescale - 1] * ms / 1000;
	timerPrescaleSet(prescale);
	timerRCSet(rc);
	return rc * prescale / FOSC0;
}

ticktime ROSA_getTicks()
{
	return ticks * CLOCK_PERIOD;
}

int ROSA_taskDelay(ticktime ms)
{
	//unsigned long t = ticks * CLOCK_PERIOD;
	
	//while(ticks * CLOCK_PERIOD < t + ms) {}
	tcbInsertWaitingList(ms + ticks * CLOCK_PERIOD);
	ROSA_yield();
	
	return 0;
}

int ROSA_taskDelayUntil(ticktime * start, ticktime ms)
{
	int status = 0;
	*start = *start + ms;

	if (*start < ticks * CLOCK_PERIOD)
		status = 1;

	tcbInsertWaitingList(*start);
	ROSA_yield();

	return status;
}


