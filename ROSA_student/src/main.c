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

////Standard library includes
//#include <avr32/io.h>
//
////Kernel includes
//#include "kernel/rosa_ker.h"
//
////Driver includes
//#include "drivers/led.h"
//#include "drivers/delay.h"
//#include "drivers/usart.h"
//
////Include configuration
//#include "rosa_config.h"

#include "tests/test_cases.h"
#include "tests/test_scheduler.h"
#include "tests/test_semaphores.h"
#include "tests/test_task_handler.h"
#include "tests/grp2_testcases/test.h"
#include "tests/grp5_testcases/grp5_testcase1.h"
#include "tests/grp5_testcases/grp5_testcase2.h"
#include "tests/grp5_testcases/grp5_testcase3.h"
#include "Demo/demo.h"


/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//test_cases();
	//test_scheduler();
	//test_task_handler();
	//test_semaphore();
	//scheduler_init_test();
	//delayUnt_test();
	
	//test_case_1();
	//test_case_2();
	//test_case_3();
	
	run_demo();

	while(1);
}
