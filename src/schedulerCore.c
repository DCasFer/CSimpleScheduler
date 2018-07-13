/*
 * schedulerCore.c
 *
 *  STM32F4 Round Robin Simple Scheduler
 *  Author: David Castaneda davidcasfer@gmail.com
 */

#include <scheduler.h>
#include "stm32f4xx.h"


/* Function name           : task1()                    */
/* Description             : First task to be scheduled     */
void
task1(void)
{
	volatile int counter = 0;

	while (1)
	{
		if ((counter > 0) && (counter < 150000))
		{
			GPIOD-> BSRR = LED_ON(LED_YELLOW); //Set LED
		}
		else if ((counter > 150001) && (counter < 340000))
		{

			GPIOD->BSRR = LED_OFF(LED_YELLOW); //Clear LED
		}
		counter = (counter + 1) % 340000;
	}

}

/* Function name           : task2()                    */
/* Description             : Second task to be scheduled     */
void
task2(void)
{
	volatile int counter = 0;

	while (1)
	{
		if ((counter > 0) && (counter < 150000))
		{
			GPIOD->BSRR = LED_ON(LED_ORANGE);
		}
		else if ((counter > 150001) && (counter < 300000))
		{

			GPIOD->BSRR = LED_OFF(LED_ORANGE);
		}
		counter = (counter + 1) % 300000;
	}

}

/* Function name           : task3()                    */
/* Description             : Third task to be scheduled     */
void
task3(void)
{
	volatile int counter = 0;

	while (1)
	{
		if ((counter > 0) && (counter < 150000))
		{
			GPIOD->BSRR = LED_ON(LED_RED);
		}
		else if ((counter > 150001) && (counter < 300000))
		{

			GPIOD->BSRR = LED_OFF(LED_RED);
		}
		counter = (counter + 1) % 300000;
	}

}


/* Function name           : LED_board_initialize()                    */
/* Description             : All board LEDS are initialised in order to add as many LEDs as we want     */
void
LED_board_initialize (void)
{
	//Configure LED outputs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; 	//Enable Port D clock

	//Set pin 12, 13 ,14, 15 as general output mode (pull-push)
	//There may be some extra
	GPIOD->MODER |= (GPIO_MODER_MODER12_0 |
					GPIO_MODER_MODER13_0 |
					GPIO_MODER_MODER14_0 |
					GPIO_MODER_MODER15_0 );


	GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12 |
					GPIO_OSPEEDER_OSPEEDR13 |
					GPIO_OSPEEDER_OSPEEDR14 |
					GPIO_OSPEEDER_OSPEEDR15 );

	GPIOD->PUPDR = 0;	//No pull up. no pull down

	return;
}

/* Function name           : trap()                 */
/* Description             : Error handling     */
void
trap()
{
	while (1)
	{
	}
}



/**Creates the stack frame for each task
 * Param 1: Loop iterator
 * Returns the relative PSP of each task.
*/
uint32_t
create_task_frame(uint32_t i, task Tasks_[], void (*led)(), long long *task_stack)
{
	//Initialisation of part of the task struct
	Tasks_[i].coreTask = (unsigned long*) led;
	Tasks_[i].task_stack = task_stack;
	//Initialisation of each task PSP and save coreTask and xPSR for each task
	Tasks_[i].PSP = ((unsigned int) Tasks_[i].task_stack) + TASK_STACK_SIZE - (8 * 8);
	HW32_REG((Tasks_[i].PSP + (14 << 2))) = (unsigned long)Tasks_[i].coreTask;
	HW32_REG((Tasks_[i].PSP + (15 << 2))) = 0x01000000;		//xPSR

	return Tasks_[i].PSP;
}
