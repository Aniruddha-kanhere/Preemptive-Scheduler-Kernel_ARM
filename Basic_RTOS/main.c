#include<MKL25Z4.h>
#include<stdio.h>
#include<stdlib.h>
#include "os_kernel.h"
#include "Board_LED.h"

#define RED      0
#define GREEN    1
#define BLUE     2

static void delay(int ms)
{
	int set_val = 4800;
	int count = set_val;
	int time = ms;
	while(time--)
	{
		while(count--);
		count = set_val;
	}
}

void tsk1(void *arg)
{
	while(1){
		LED_Off(RED); LED_Off(GREEN);
		LED_On(BLUE);
	}
}

void tsk2(void *arg)
{
	while(1){		
		LED_Off(BLUE); LED_Off(GREEN);		
		LED_On(RED);
	}
}

void tsk3(void *arg)
{
	while(1){
		LED_Off(BLUE); LED_Off(RED);
		LED_On(GREEN);
	}
}

uint8_t stack1[STACK_SIZE];
uint8_t stack2[STACK_SIZE];
uint8_t stack3[STACK_SIZE];

int main()
{	
	struct task task1;
	struct task task2;
	struct task task3;
		
	InsertTask(tsk1, (void*)0, Medium, (StackPtr_t*)stack1, &task1);
	InsertTask(tsk2, (void*)0, Medium, (StackPtr_t*)stack2, &task2);
	InsertTask(tsk3, (void*)0, Medium, (StackPtr_t*)stack3, &task3);
	
	LED_Initialize();
	
	os_start();
	
	while(1);
}
