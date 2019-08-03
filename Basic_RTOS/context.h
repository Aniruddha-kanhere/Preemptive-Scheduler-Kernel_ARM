#ifndef __CONTEXT_H
#define __CONTEXT_H

#include<MKL25Z4.h>
#include<stdio.h>
#include<stdlib.h>

struct context{
	//Need to save the following registers
	//Kept these first since this would allow easy access
	uint32_t   r4;
	uint32_t   r5;
	uint32_t   r6;
	uint32_t   r7;
	uint32_t   r8;
	uint32_t   r9;
	uint32_t   r10;
	uint32_t   r11;
	
	//Saved by the hardware
	//Registers are stored on stack with smallest register on smallest
	//address (i.e. top of the stack). Thus, user shouldn't change the
	//order of declaration of these registers in this struct.
	uint32_t   r0;	
	uint32_t   r1;
	uint32_t   r2;
	uint32_t   r3;
	uint32_t   r12;
	uint32_t   lr;
	uint32_t   pc;
	uint32_t   xPsr;	
};

typedef struct context   osContext_t;
typedef struct context * osContextPtr_t;

#endif
