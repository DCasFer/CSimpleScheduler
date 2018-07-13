/*
 *
 * main.c
 *
 * STM32F407 Round Robin Simple Scheduler
 * Author: David Castaneda davidcasfer@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "scheduler.h"
#include "cortexm/ExceptionHandlers.h"

// ----------------------------------------------------------------------------
//
// Semihosting STM32F4 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//


// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

//============ OS Data ==============
uint32_t curr_task = 0;
uint32_t next_task = 1;
uint32_t PSP_array[NUM_TASK];

//=========== Each Task data ============
//stack for each task (8Kbytes each - 1024 x 8 bytes)
long long task1_stack[TASK_STACK_SIZE], task2_stack[TASK_STACK_SIZE], task3_stack[TASK_STACK_SIZE];
task Tasks[NUM_TASK];

int
main(int argc, char* argv[])
{
	SCB->CCR |= SCB_CCR_STKALIGN_Msk;

	LED_board_initialize();

	//Initialise each task and creation of its stack frame
	for (int i = 0; i < NUM_TASK; i++)
	{
		if (i == task_1)
		{
			PSP_array[i] = create_task_frame(i, Tasks, task1, task1_stack);
		}
		else if (i == task_2)
		{
			PSP_array[i] = create_task_frame(i, Tasks, task2, task2_stack);
		}
		else if (i == task_3)
		{
			PSP_array[i] = create_task_frame(i, Tasks, task3, task3_stack);
		}
	}

	curr_task = 0; //Switch to task 0 (current task)

	__set_PSP((PSP_array[curr_task] + 16 * 4)); //set PSP to top of the task 0 stack

	NVIC_SetPriority(PendSV_IRQn, 0xFF); //Set PendSV to lowest possible possible priority

	SysTick_Config(150);

	__set_CONTROL(0x3); //Switch to use Process Stack. Unpriviliged state

	__ISB(); //It flashes the processor pipeline to start task1 by scratch
	task1();

	//Error handling
	trap(); //Should never get this point



  return 0;
}



//===================== Exception Handlers ==============================
/*
 * Both Systick() and PendSV() exception handlers have been commented out at system/src/cortexm/exception_handlers.c
 * so that I could add my own implementation right here and this implementation doesn't remain hidden for the
 * developer.
 */

void __attribute__ ((section(".after_vectors"),weak))
SysTick_Handler (void)
{

#if defined(USE_HAL_DRIVER)
	  HAL_IncTick();
#endif

	//Finite State Machine System Flow
	switch (curr_task)
	{
		//Task 0 state -> Task 1 state
		case (TASK_0):
			next_task = 1;
			break;
		//Task 1 state -> Task 2 state
		case (TASK_1):
			next_task = 2;
			break;
		//Task 2 state -> Task 0 state
		case (TASK_2):
			next_task = 0;
			break;
		//Default state= Task 0 state -> Task 1 state
		default:
			next_task = 1;
			curr_task = 0;
	}

	//if true => trigger context switch
	if (curr_task != next_task)
	{
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; //Set PendSV to pending
	}

}



//--------- PendSV_Handler()------------------------------------------------
void __attribute__ ((section(".after_vectors"),weak))
PendSV_Handler (void)
{
#if defined(DEBUG)
	//-------------------Context switch ---------------

	//Save context
	__asm volatile("MRS 	R0,PSP");				//1) PSP register to R0
	__asm volatile("STMDB 	R0!,{R4 - R11}"); 		//2) Push registers R4 to R11 on top off R0
	__asm volatile("LDR 	R1,=curr_task");		//3) Save address of curr_task in R1
	__asm volatile("LDR		R2,[R1]");				//4) Dereference R1 and save value in R2
	__asm volatile("LDR 	R3,=PSP_array");		//5) Save PSP_array base address in R3
	__asm volatile("STR		R0,[R3, R2, LSL #2]");	//6) Save PSP (R0) in PSP_array[curr_task]

	//Load Context
	__asm volatile ("LDR 	R4,=next_task");		//1) Save next_task address in R4
	__asm volatile ("LDR 	R4,[R4]");				//2) Dereference R4 and save value in R4
	__asm volatile ("STR 	R4,[R1]");				//3) Dereference R1 and save vale in R4. curr_task=next_task
	__asm volatile ("LDR 	R0,[R3, R4, LSL #2]");	//4) Load to R0 next PSP. R0=PSP_array[curr_task]
	__asm volatile ("LDMIA	R0!,{R4 - R11}");		//5) Pop off registers R4 - R11
	__asm volatile ("MSR 	PSP,R0");				//6) Save R0, which holds next PSP, to PSP register
	__asm volatile (".align 	4");				//8)Align 4 bytes, because it is double word alignment
	__asm volatile ("BX 	LR"); 					//8) Return to next task


#endif
	 while (1)
	{
	}
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
