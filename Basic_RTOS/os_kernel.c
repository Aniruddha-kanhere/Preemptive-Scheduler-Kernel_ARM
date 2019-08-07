#include "os_kernel.h"

struct task *current_task;
struct task *next_task;

static uint32_t tick_count;

static task_list LIST[MAX_NUM_TASKS];
static task_list *head = LIST;

uint8_t IdleTaskStack[STACK_SIZE];

const int task_status_offset = OFFSET_OF(struct task, status);

struct task IdleTask;
void os_IdleTask(void *arg)
{
	for( ; ; )
	{
		DEBUG_TOGGLE(DBG_2);		
	}	
}

__weak void os_Thread_exit(void)
{
	current_task->status = Finished;
	SCB->ICSR |= 1<<28;
	while(1);
}

static StackPtr_t * put_context(void (*func)(void *), void *arg, StackPtr_t *end_of_stack)
{
	osContextPtr_t ctx = (osContextPtr_t)( (uint32_t)end_of_stack - sizeof(struct context));
	
	ctx->lr   = (uint32_t)os_Thread_exit;//0xFFFFFFFD;
	ctx->pc   = (uint32_t)func;
	ctx->r0   = (uint32_t)arg;
	ctx->xPsr = 0x01000000U;	
	ctx->r1   = 0x11111111;
	ctx->r2   = 0x22222222;
	ctx->r3   = 0x33333333;
	ctx->r4   = 0x44444444;
	ctx->r5   = 0x55555555;
	ctx->r6   = 0x66666666;
	ctx->r7   = 0x77777777;
	ctx->r8   = 0x88888888;
	ctx->r9   = 0x99999999;
	ctx->r10  = 0xAAAAAAAA;
	ctx->r11  = 0xBBBBBBBB;
	ctx->r12  = 0xCCCCCCCC;
	
	return (StackPtr_t *)ctx;
}

void os_InsertTask(void (*func)(void *), void *arg, enum task_priority priority, StackPtr_t *stack_ptr, struct task *task)
{
	static uint8_t init=0;
	int count = 0;
	task_list *ptr;
	
	task->task = func;
	task->priority = priority;
	task->stack_ptr = put_context(func, arg, (StackPtr_t *)((uint32_t)stack_ptr + STACK_SIZE));
	task->status = Ready;
	task->timeout = 0;
	
	if(init==0)
	{
		init = 1;
    ptr = head;
		ptr->next = head;
		ptr->TaskPtr = task;
		
		//Initialise the task pointers to the first task
		next_task = current_task = task;
	}
	else
	{
		ptr = head;
		while(ptr->next != head)
		{
			ptr = ptr->next;
			
			//Nothing should be NULL in case of circular queue
			if(ptr == NULL)
				HardFault_Handler();
			
			count++;
		}
				
		if(count++ >= MAX_NUM_TASKS)
			return;                  //Don't add it to the list
		
		//Else add it to the list
		ptr->next = &LIST[count];				
		ptr->next->next = head;
		ptr->next->TaskPtr = task;		
	}
}


void os_Delay(uint16_t timeout)
{
	if(timeout)
	{
		//Update the timeout value and put the task in blocked state
		current_task->timeout = timeout;
		current_task->status  = Blocked;
		
		//Call the PendSV Handler for scheduling
		SCB->ICSR |= 1<<28;
	}
}


struct task * os_FindNextTask(void)
{
	task_list *next_task_ptr = head;
	
	//If the head is NULL then something is definately not right
	if(next_task_ptr == NULL)
		HardFault_Handler();
	
	while(next_task_ptr->TaskPtr != current_task)
	{
		next_task_ptr = next_task_ptr->next;
		
		//if we have come back to head somehow without finding the
		//current task then it means that something is amis
		if(next_task_ptr == head)
			HardFault_Handler();
		
		//if somehow the next pointer is NULL then too we have an error
		//task_list is a circular queue
		if(next_task_ptr == NULL)
			HardFault_Handler();
	}
	
	//Get the next task after the current_task
	next_task_ptr = next_task_ptr->next;
	
	while(1)
	{
		//If the next task after current task is also current task
		if(next_task_ptr->TaskPtr == current_task)
		{
			//And if it is ready then return this.
			if(next_task_ptr->TaskPtr->status == Ready)
				return next_task_ptr->TaskPtr;
			
			//If no task is ready (since we have come back to the current task)
			//we should run the idle task
			else
			{
				return &IdleTask;
			}
		}
		
		//if the next task is ready and it is NOT the idle task, then return this value
		if(next_task_ptr->TaskPtr->status == Ready  &&
			 next_task_ptr->TaskPtr->task != os_IdleTask)
		{
			return next_task_ptr->TaskPtr;
		}
		
		//update the value
		next_task_ptr = next_task_ptr->next;
	}
}

static void os_UpdateTimeouts()
{
	task_list *task_list_ptr = head;
	do //while(task_list_ptr != NULL)
	{
		if(task_list_ptr->TaskPtr->timeout)
		{
			task_list_ptr->TaskPtr->timeout--;
			if(task_list_ptr->TaskPtr->timeout == 0)
				task_list_ptr->TaskPtr->status = Ready;
		}
		
		task_list_ptr = task_list_ptr->next;
	}while(task_list_ptr != head);
}

void TimerTick()
{	
	os_UpdateTimeouts();
	
	//round robin implementation
	next_task = os_FindNextTask();		
	tick_count++;
}

void os_initialise()
{
	os_InsertTask(os_IdleTask, (void *)0, Lowest, (StackPtr_t*)IdleTaskStack, &IdleTask);
  Config_SysTick(KERNEL_TICK_FREQUENCY);
}

void os_start()
{	
	Systick_Enable();	
	Start_Kernel();
}
