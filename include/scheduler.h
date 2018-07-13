/*
 * scheduler.h
 *
 *  STM32F4 Round Robin Simple Scheduler
 *  Author: David Castaneda davidcasfer@gmail.com
 */

#include "stm32f4xx.h"

#ifndef SCHEDULER_H_
#define SCHEDULER_H_


// [ILG]
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif

#ifdef __cplusplus
 extern "C" {
#endif

#define LED_ON(BITPOSITION) 	(1 << BITPOSITION)
#define LED_OFF(BITPOSITION) 	(1 << (BITPOSITION + 16))
// Macro for word accesses
#define HW32_REG(ADDRESS)	(*((volatile unsigned long *)(ADDRESS)))
//LEDs
#define LED_YELLOW 12
#define LED_ORANGE 13
#define LED_RED    14
//Tasks
#define TASK_0 0
#define TASK_1 1
#define TASK_2 2
#define NUM_TASK 3
#define TASK_STACK_SIZE 1024

 typedef enum {
 	task_1 = 0,
 	task_2 = 1,
 	task_3 = 2,
 } scheduler_task;

 typedef struct task {
 	uint32_t PSP;
 	long long *task_stack;
 	unsigned long *coreTask;
 } task;


 //--------------Prototypes----------------
 void LED_board_initialize(void); 	//initialize LEDs
 void task1(void); 			//Toggle LED1
 void task2(void);			//Toogle LED2
 void task3(void);			//Toogle LED3
 uint32_t create_task_frame(uint32_t i, task Tasks[], void (*led)(), long long *task_stack);
 void trap(void);			//Error handler

#ifdef __cplusplus
 }
#endif

#endif /* SCHEDULER_H_ */
