/*
steps to configure EXTI:
- Enable the clocks for GPIOs and AFIO.
- Configure the Alternate Function to use a GPIO (usually standard after reset).
- Configure the interrupt input pin as input using CRL and CRH registers.
- Select the interrupt input pin using AFIO_EXTICR.
- Configure the RTSR and FTSR registers to choose the interrupt edge.
- Set the EXTI_IMR register to enable the interrupt for the line.
- Enable the EXTI interrupt using the ISER register or NVIC_EnableIRQ function.
- Clear the PR flag in the interrupt routine.
*/
#include <stm32f10x.h>

/*
	Selects PB1 as the input for external interrupt.
	On each falling edge, an interrupt is generated.
	Interrupt service routine toggles Led on PC13(IRQHandler).
*/
void delay_msx(uint16_t t);


void EXTI4_IRQHandler(){
	EXTI->PR = (1 << 4);
	for(int i=1;i<5;i++){
	GPIOC->ODR ^= 1 << 13;
	delay_msx(200);
	}
}


void ext_interrupt(void){
	//delay_init();
	AFIO->EXTICR[1] = 1 << 0;	//selects PB4 for line 4 
	EXTI->FTSR = (1 << 4);		//interrupt on falling edge
	EXTI->IMR = (1 << 4);			//enable interrupt EXTI4
	NVIC_EnableIRQ(EXTI4_IRQn);	//enable interrupt EXTI4
}
void delay_msx(uint16_t t){
	volatile unsigned long l=0;
	for(uint16_t i = 0; i < t;i++)
	for(l = 0;l < 6000;l++)
	{
	}
}