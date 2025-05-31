
#include <stm32f10x.h>

void systickdelay(int delay)
{
	//Sets the reload value (value=36Mhz*200ms)=7200000
	//Register descripction in Technical Reference Manual (pag.159)
	SysTick->LOAD = 7200000;
	
	//Clears the SysTick current value register
	SysTick->VAL= 0;

	//Enable SysTick in the control register and select internal system clock source
	SysTick->CTRL = (1 << 0) | (1 << 2);
	for(int i=delay; i > 0; i--)
	{
		while(!(SysTick->CTRL & (1 << 16)));
	}

	//Disable SysTick counter in the Control Register
	SysTick->CTRL = 0;
}