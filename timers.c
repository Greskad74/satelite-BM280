#include "stm32f10x.h"
/*steps to configure timers
	CNT-> counter register
	ARR -> Auto Reload Register, his value is continuously compared to CNT
	CR1 -> Control Register
	SR -> Status Register when ARR=CNT UIF(update interrupt flag) is set
	and CNT=0
*/

void delay_timer2()
{
	RCC->APB1ENR |= 1 << 0; //enable TIM2 clock 
	TIM2->ARR = 7;
	TIM2->SR |= ~(1 << 0); //clear uif flag
	TIM2->CR1 |= ~(1 << 4);//upcounting
	while (TIM2->SR & (1 << 0))
	TIM2->CR1 = ~(1 << 0); //counter disable 
}

void timer2_ch1_oc(void){
	
	RCC->APB1ENR|= 1 << 0; //TIM2 clk enabled
	/* 
	Tout=[(ARR+1)(PSC+1)]/Fclk
	or ARR = {[(Tout)(Fclk)]/(PSC+1)}-1
	Tout: is desired pulse width
	PSC: Prescaler (can be a supposed value, but ARR <= 65,535)
	Fclk: Clock frequency (72 Mhz if internal clock used)
	Example for 1ms pulse width and PSC=1
		ARR = [(1ms)(72Mhz)/(1+1)]-1 = 35,999
	*/
	TIM2->PSC=1;			//prescaler value
	TIM2->ARR=35999;	//Value to compare
	TIM2->CCMR1 &= ~(1 << 6);	//OC1M in Toggle on Match Mode ch1
	TIM2->CCMR1 |= (1 << 5) | (1 << 4);	
	TIM2->CCER |= (1 << 0);		//ch1 output enable
	TIM2->CR1 |= (1 << 0);		//counter enabled
}

void timer3_ch1_pwm(){

	RCC->APB1ENR |= (1 << 1);	//TIM3 clk enabled
	/* 
	DutyCycle = [CCR/(ARR+1]x100%
	Fpwm = Fclk/[(ARR + 1)(PSC+1)]
	Example: 1ms pulse over 20ms(50hz)
	Assuming PSC=21
	ARR = [(Fclk)/(F_pwm)(PSC+1)]-1 = [(72MHz)/(50Hz)(21+1)] -1 = 65453
	DutyCycle=(2ms/20ms)x100%=10%
	CCR = DutyCycle(ARR+1)
	CCR=0.05x(65453+1)=3,272.7
	*/
	TIM3->PSC=21;			//prescaler value
	TIM3->ARR=65453;	//Value to compare
	TIM3->CNT=0;
	//TIM3->CCR1 = 3273;	//Duty cicle 
	TIM3->CCMR1 |= (1 << 6) | (1 << 5); //pwm mode1 ch1
	TIM3->CCMR1 &= ~(1 << 4);
	TIM3->CCER	|=	(1 << 0);	//ch1 output enable
	TIM3->CR1		|=	(1 << 0); //counter enabled
}
