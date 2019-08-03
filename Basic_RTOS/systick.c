#include "systick.h"

void Config_SysTick(uint32_t frequency)
{
	uint32_t load;	
	if(frequency == 0U)
		return;
	
	load = (SystemCoreClock / frequency) - 1U;
	
  if (load > 0x00FFFFFFU)
    return;
  
	SysTick->CTRL =  SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
  SysTick->LOAD =  load;
  SysTick->VAL  =  0U;
}

void Systick_Enable()
{
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void Systick_Disable()
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
