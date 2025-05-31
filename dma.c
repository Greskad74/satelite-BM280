/*
steps to configure DMA:
- Initialize CPAR and CMAR with source and destination address.
- Initilize CNDTR with the number of data to be copied.
	CNDTR is decremented each transfer and channel stops transferring
	when CNDTR = 0.
- In CCR configure channel priority, transfer direction, data size,
	memory and data peripheral increment mode, circular mode and interrupts.
- Enable channel.
*/
#include <stm32f10x.h>
volatile char transmision_complete =0;

void dma_mem_to_mem(){
	char source[10]="0123456789";
	char destination[10];
	RCC->AHBENR = RCC_AHBENR_DMA1EN; //DMA clock enable
	//Define source and destination address
	DMA1_Channel1->CPAR = (uint32_t) source; 
	DMA1_Channel1->CMAR = (uint32_t) destination;
	
	DMA1_Channel1->CNDTR = 10; //number of bytes to transfer
	/********************************************************/
	DMA1_Channel1->CCR |= (1 << 14)|(1 << 7)|(1 << 6)|(1 << 1);
	/*mem to mem transfer, memory increment, peripheral increment
	transfer complete interrupt enable*/
	
	DMA1_Channel1->CCR |= 1; //enable channel
	
	NVIC_EnableIRQ(DMA1_Channel1_IRQn); //Enable interrupt for channel 1
	
	while(transmision_complete == 0); //wait for transmision complete
}

//interrupt handler
void DMA1_Channel1_IRQHandler(void){
	if ((DMA1->ISR & (1 << 1)) != 0) //if transmission complete
	{
		DMA1->IFCR = (1 << 1); //set channel transfer complete
		transmision_complete = 1;
	}
}
