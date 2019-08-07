#ifndef __OS_KERNEL_H
#define __OS_KERNEL_H

#include <stdint.h>
#include <MKL25Z4.h>
#include <stdio.h>
#include <stdlib.h>
#include "context.h"
#include "systick.h"
#include "debug.h"

//Define the stack size for all the tasks here
#define STACK_SIZE                 140

//define the maximum nuber of tasks that the kernel can run
#define MAX_NUM_TASKS              10

//Define the kernel tick freqency. High freq -> decreased response time -> more overhead
#define KERNEL_TICK_FREQUENCY      1000

#define OFFSET_OF(type, object)    ((size_t)(&(((type*)0)->object)))

enum task_status   {Ready, Running, Blocked, Preempted, Finished};
enum task_priority {Lowest, Low, Medium, High, Highest};

typedef uint32_t StackPtr_t;

struct task{
	StackPtr_t *stack_ptr;
	enum task_status status;
	enum task_priority priority;	
	void (*task)(void*);
	uint16_t timeout;
};


typedef struct task_list{
	struct task* TaskPtr;
	struct task_list *next;
}task_list;

extern uint32_t tick_count;

//defined in os_kernel.s
void PendSV_Handler(void);
void SysTick_Handler(void);
void Start_Kernel(void);

//defined in startup_MKL25Z4.s
void HardFault_Handler(void);

//defined in os_kernel.c
void os_initialise(void);
void os_InsertTask(void (*func)(void *), void *arg, enum task_priority priority, StackPtr_t *stack_ptr, struct task *task);
void os_start(void);
void os_Schedule(void);
void os_Delay(uint16_t);
struct task * os_FindNextTask(void);

#endif
