#include "os_kernel.h"

struct task *current_task;
struct task *next_task;

static uint32_t tick_count;

static task_list LIST[MAX_NUM_TASKS];
static task_list *head = LIST;

void os_IdleTask()
{	
	for( ; ; )
	{
		
	}	
}

static StackPtr_t * put_context(void (*func)(void *), void *arg, StackPtr_t *end_of_stack)
{
	osContextPtr_t ctx;
	int i;
	uint32_t *ptr;
	ctx = (osContextPtr_t)(end_of_stack - 1 - sizeof(struct context));
	
	ptr = (uint32_t *)ctx;
	for(i=0; i<(sizeof(struct context)/4) ; i++)
		ptr[i] = 0;  //initialise everything to 0
	
	ctx->lr = 0xFFFFFFFD;
	ctx->pc = (uint32_t)func;
	ctx->r0 = (uint32_t)arg;
	ctx->xPsr = 0x01000000U;	
	ctx->r1 = 0x11111111;
	ctx->r2 = 0x22222222;
	ctx->r3 = 0x33333333;
	ctx->r4 = 0x44444444;
	ctx->r5 = 0x55555555;
	ctx->r6 = 0x66666666;
	ctx->r7 = 0x77777777;
	ctx->r8 = 0x88888888;
	ctx->r9 = 0x99999999;
	ctx->r10 = 0xAAAAAAAA;
	ctx->r11 = 0xBBBBBBBB;
	ctx->r12 = 0xCCCCCCCC;
	
	return (StackPtr_t *)ctx;
}

void InsertTask(void (*func)(void *), void *arg, enum task_priority priority, StackPtr_t *stack_ptr, struct task *task)
{
	static uint8_t init=0;
	int count = 0;
	
	task->task = func;
	task->priority = priority;
	task->stack_ptr = put_context(func, arg, stack_ptr + STACK_SIZE);
	task->status = Ready;
	task->timeout = 0;
	
	if(init==0)
	{
		init = 1;
		head->next = NULL;		
		head->TaskPtr = task;
		next_task = current_task = task;
	}
	else
	{
		task_list *ptr = head;
		
		while(ptr->next != NULL)
		{
			ptr = ptr->next;
			count++;
		}
		count++;
		if(count >= MAX_NUM_TASKS)
			return;                  //Don't add it to the list
		
		ptr->next = &LIST[count];
		ptr->next->next = NULL;
		ptr->next->TaskPtr = task;		
	}
}

void TimerTick()
{
	//round robin implementation		
	task_list *next_task_ptr = head;
	while(next_task_ptr->TaskPtr != current_task)
	{
		next_task_ptr = next_task_ptr->next;
		if(next_task_ptr == NULL)
			HardFault_Handler();
	}
	if(next_task_ptr->next == NULL)
		next_task_ptr = head;
	else
		next_task_ptr = next_task_ptr->next;
	
	next_task = next_task_ptr->TaskPtr;
	//current_task = temp;
	tick_count++;
}

void os_start()
{
  Config_SysTick(KERNEL_TICK_FREQUENCY);
	Systick_Enable();	
	Start_Kernel();
}
