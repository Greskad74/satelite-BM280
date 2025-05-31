#include "usart.h"
#include <stdio.h>
#include "stm32f10x.h"

static char *dma_buffer;
static uint16_t dma_buffer_size;

void uart_dma_init(char *rx_buffer, uint16_t buffer_size) {
    dma_buffer = rx_buffer;
    dma_buffer_size = buffer_size;

    // GPIOA y USART1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
    GPIOA->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9);
    GPIOA->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1); // TX
    GPIOA->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0; // RX

    // USART1 9600 8N1
    USART1->BRR = 72000000 / 9600;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE;
    USART1->CR3 |= USART_CR3_DMAR; // DMA para RX
    USART1->CR1 |= USART_CR1_UE;

    // Habilitar reloj DMA1
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // DMA1 Canal 5 para USART1_RX
    DMA1_Channel5->CCR &= ~(1U << 0);
    DMA1_Channel5->CPAR = (uint32_t)&USART1->DR;
    DMA1_Channel5->CMAR = (uint32_t)dma_buffer;
    DMA1_Channel5->CNDTR = dma_buffer_size;
    DMA1_Channel5->CCR = (1U << 7) | (1U << 5) | (0b10U << 12); // MEM INC, circular, alta prioridad
    DMA1_Channel5->CCR |= (1U << 0);
}

void uart_send_char(char c) {
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

void uart_send_string(const char *s) {
    while (*s) uart_send_char(*s++);
}

void uart_send_double(double val) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.6f", val);
    uart_send_string(buf);
}
