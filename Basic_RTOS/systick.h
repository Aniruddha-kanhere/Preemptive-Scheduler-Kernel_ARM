#ifndef __SYSTICK_H
#define __SYSTICK_H

#include<MKL25Z4.h>
#include<stdio.h>
#include<stdlib.h>

void Config_SysTick(uint32_t frequency);
void Systick_Enable(void);
void Systick_Disable(void);

#endif
