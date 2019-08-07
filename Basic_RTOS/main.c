#include<MKL25Z4.h>
#include<stdio.h>
#include<stdlib.h>
#include "os_kernel.h"
#include "Board_LED.h"
#include "debug.h"

#define RED      0
#define GREEN    1
#define BLUE     2

/*static void delay(int ms)
{
	int set_val = 4800;
	int count = set_val;
	int time = ms;
	while(time--)
	{
		while(count--);
		count = set_val;
	}
}*/

void tsk1(void *arg)
{
	while(1){
		//LED_Off(RED); LED_Off(GREEN);
		//LED_On(BLUE);
		
		os_Delay(1);
		DEBUG_TOGGLE(DBG_3);
	}
}

void tsk2(void *arg)
{
	while(1){
		/*LED_Off(BLUE); LED_Off(GREEN);
		LED_On(RED);
		os_Delay(3);*/
		//os_Delay(1);
		DEBUG_TOGGLE(DBG_4);
	}
}

void tsk3(void *arg)
{
	while(1){
		/*
		LED_Off(BLUE); LED_Off(RED);
		LED_On(GREEN);
		os_Delay(7);*/
		//os_Delay(1);
		DEBUG_TOGGLE(DBG_5);
	}
}

uint8_t stack1[STACK_SIZE];
uint8_t stack2[STACK_SIZE];
uint8_t stack3[STACK_SIZE];

struct task task1;
struct task task2;
struct task task3;

int main()
{	
	Init_Debug_Signals();
	LED_Initialize();
	
	tsk2((void *)0);
  os_initialise();
	os_InsertTask(tsk1, (void*)0, Medium, (StackPtr_t*)stack1, &task1);
	os_InsertTask(tsk2, (void*)0, Medium, (StackPtr_t*)stack2, &task2);
	os_InsertTask(tsk3, (void*)0, Medium, (StackPtr_t*)stack3, &task3);
			
	//os_initialise();	
	os_start();
	
	while(1);
}
